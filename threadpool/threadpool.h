#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <queue>
#include <memory>
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <functional>
#include <unordered_map>

// Any类型 可以接受任意的数据类型
class Any {
public:
    Any() = default;
    ~Any() = default;
    Any(Any&&) = default;  // 移动构造函数
    Any& operator=(Any&&) = default;  // 移动赋值运算符
    Any(const Any&) = delete;
    Any& operator=(const Any&) = delete;

    template<typename T>
    Any(T data) : base_(std::make_unique<Drive<T>>(data)) {}

    // 提取Any对象里面存储的data数据
    template<typename T>
    T cast_() {
        Drive<T>* pd = dynamic_cast<Drive<T>*>(base_.get());
        if (pd == nullptr) {
            throw "type mismatch"; 
        }
        return pd->data_;
    }

private:
    // 基类类型
    class Base {
    public:
        virtual ~Base() = default;
    private:

    };

    // 派生类类型
    template<typename T>
    class Drive : public Base {
    public:
        Drive(T data) : data_(data) {}
        T data_;
    };

    std::unique_ptr<Base> base_;  // 基类指针
};


// 信号量类型
class Semaphore {
public:
    Semaphore(int limits = 0) : resLimit_(limits) {}
    ~Semaphore() = default;

    // 移动构造函数
    Semaphore(Semaphore&& other) noexcept
        : resLimit_(other.resLimit_),
          mutex_(),
          cond_() {
    }

    // 移动赋值运算符
    Semaphore& operator=(Semaphore&& other) noexcept {
        if (this != &other) {
            resLimit_ = other.resLimit_;
        }
        return *this;
    }

    void wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [&]()->bool { return resLimit_ > 0; });
        resLimit_--;
    }

    void post() {
        std::unique_lock<std::mutex> lock(mutex_);
        resLimit_++;
        cond_.notify_all();
    }

private:
    int resLimit_;
    std::mutex mutex_;
    std::condition_variable cond_;
};


class Task;
// Result类型  Result为Task执行完成后返回值类型
class Result {
public:
    Result(std::shared_ptr<Task> task, bool isValid = true);
    ~Result() = default;

    // 移动构造函数
    Result(Result&& other) noexcept
        : any_(std::move(other.any_)),
          sem_(std::move(other.sem_)),
          task_(std::move(other.task_)),
          isValid_(other.isValid_.load()) {
    }

    // 移动赋值运算符
    Result& operator=(Result&& other) noexcept {
        if (this != &other) {
            any_ = std::move(other.any_);
            sem_ = std::move(other.sem_);
            task_ = std::move(other.task_);
            isValid_ = other.isValid_.load();
        }
        return *this;
    }

    // 
    void setVal(Any any_);

    // 获取Task的返回值
    Any get();
private:
    Any any_;
    Semaphore sem_;
    std::shared_ptr<Task> task_;
    std::atomic_bool isValid_;
};


// 任务的抽象基类
class Task {
public:
    Task();
    ~Task() = default;
    // 用户可以自定义任务类型，从Task类继承而来，重写run()方法 
    virtual Any run() = 0;

    void setResult(Result* res);
    void exec();

private:
    Result* res_;
};


// 线程类型
class Thread {
public:
    using ThreadFunc = std::function<void(int)>;

    Thread(ThreadFunc func);
    ~Thread();

    // 启动线程
    void start();

    // 查询线程id
    int getId() const;

private:
    ThreadFunc func_;
    static int genId_;
    int threadId_;
};


// 线程池工作模式
enum class PoolMode {
    Mode_Fixed,
    Mode_Cached,
};


// 线程池类型
class ThreadPool {
public:
    ThreadPool();
    ~ThreadPool();

    // 开启线程池
    void start(size_t initThreadSize = std::thread::hardware_concurrency()); 

    // 设置线程池的工作模式
    void setMode(PoolMode mode);

    // 设置线程数量上限(Cached模式下)
    void setThreadSizeThreshold(size_t thread_Threshold);

    // 设置线程池的任务队列容量上限
    void setTaskQueThreshold(size_t task_Threshold);

    // 提交任务至线程池
    Result submitTask(std::shared_ptr<Task> sp);

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

private:
    PoolMode poolMode_;  // 线程池工作模式 
    std::atomic_bool isRunning_;  // 线程池是否已经启动

    std::unordered_map<int, std::unique_ptr<Thread>> threads_;  // 线程列表
    size_t initThreadSize_;  // 初始线程数量
    size_t threadSizeThreshold_;  // 线程数量上限(Cached模式下)
    std::atomic_int idleThreadSize_;  // 空闲线程数量
    std::atomic_int curThreadSize_;  // 当前线程数量

    std::queue<std::shared_ptr<Task>> taskQue_;  // 任务队列
    std::atomic_uint taskSize_;  // 任务数量
    size_t taskQueThreshold_;  // 任务队列容量上限

    std::mutex taskQueMutex_;  // 保证任务队列的线程安全
    std::condition_variable notFull_;
    std::condition_variable notEmpty_;
    std::condition_variable exitCond_;  // 等带线程资源全部回收 

    void threadFuc(int thread_id);  // 线程执行函数
    bool checkState() const;  // 查询线程池运行状态
};

#endif