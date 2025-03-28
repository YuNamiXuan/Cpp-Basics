#include "threadpool.h"
#include <functional>
#include <thread>
#include <iostream>

const size_t TASK_QUE_THRESHOLD = 1024;
const size_t THREAD_SIZE_THRESHOLD = 100;
const int THREAD_MAX_IDEL_TIME = 10;

/*
任务类方法实现
*/
Task::Task()
    : res_(nullptr) {
}

void Task::setResult(Result* res) {
    res_ = res;
}

void Task::exec() {
    if (res_ != nullptr) {
        res_->setVal(run());
    }
}


/*
线程池类方法实现
*/

ThreadPool::ThreadPool()   
    : initThreadSize_(0), 
      idleThreadSize_(0),
      curThreadSize_(0),
      threadSizeThreshold_(THREAD_SIZE_THRESHOLD),
      taskSize_(0), 
      taskQueThreshold_(TASK_QUE_THRESHOLD), 
      poolMode_(PoolMode::Mode_Fixed),
      isRunning_(false) {
}

ThreadPool::~ThreadPool() {
    isRunning_ = false;
    
    std::unique_lock<std::mutex> lock(taskQueMutex_);
    notEmpty_.notify_all();
    exitCond_.wait(lock, [&]()->bool { return threads_.size() == 0; });
}

// 开启线程池
void ThreadPool::start(size_t initThreadSize) {
    // 设置线程池运行状态
    isRunning_ = true;

    // 初始化线程数量
    initThreadSize_ = initThreadSize;
    curThreadSize_ = initThreadSize;

    // 创建线程对象
    for (int i = 0; i < initThreadSize_; i++) {
        auto ptr = std::make_unique<Thread>(std::bind(&ThreadPool::threadFuc, this, std::placeholders::_1));
        int thread_id = ptr->getId();
        threads_.emplace(thread_id, std::move(ptr));
    }
    // 启动所有线程
    for (int i = 0; i < initThreadSize_; i++) {
        threads_[i]->start();
        idleThreadSize_++;
    }
}

// 设置线程池的线程上线(Cached模式下)
void ThreadPool::setThreadSizeThreshold(size_t thread_threshold) {
    if (checkState()) {
        return ;
    }
    if (poolMode_ == PoolMode::Mode_Cached) {
        threadSizeThreshold_ = thread_threshold;
    }
}

// 设置线程池的工作模式
void ThreadPool::setMode(PoolMode mode) {
    if (checkState()) {
        return ;
    } 
    poolMode_ = mode; 
}

// 查询线程池工作状态
bool ThreadPool::checkState() const {
    return isRunning_;
}

// 设置线程池的任务队列容量上限
void ThreadPool::setTaskQueThreshold(size_t task_threshold) {
    taskQueThreshold_ = task_threshold;
}

// 提交任务至线程池    用户调用该接口向任务队列中添加任务
Result ThreadPool::submitTask(std::shared_ptr<Task> sp) {
    // 获取锁
    std::unique_lock<std::mutex> lock(taskQueMutex_);

    // 线程通信 等待任务队列空余
    if (!notFull_.wait_for(lock, std::chrono::seconds(1), [&]()->bool { return taskQue_.size() < taskQueThreshold_; })) {
        std::cerr << "Task queue is full, submit task fail." << std::endl;
        return Result(std::move(sp), false);
    }

    // 任务队列空余，将任务加入队列
    taskQue_.emplace(sp);
    taskSize_++;

    notEmpty_.notify_all(); 

    // Cached模式下，根据任务数量和空闲线程数量判断是否需要创建新线程
    if (poolMode_ == PoolMode::Mode_Cached && taskSize_ > idleThreadSize_ && curThreadSize_ < threadSizeThreshold_) {
        auto ptr = std::make_unique<Thread>(std::bind(&ThreadPool::threadFuc, this, std::placeholders::_1));
        std::cout << "---Create new thread---" << std::endl;
        int thread_id = ptr->getId();
        threads_.emplace(thread_id, std::move(ptr));
        threads_[thread_id]->start();
        curThreadSize_++;
        idleThreadSize_++;
    }

    return Result(std::move(sp));
}

// 定义线程函数  线程池的所有线程从任务队列中获取任务并执行
void ThreadPool::threadFuc(int thread_id) {
    auto lastTime = std::chrono::high_resolution_clock().now();

    for (;;) {
        std::shared_ptr<Task> task;
        // 建立作用域限制线程获取锁的时间
        {
            // 获取锁
            std::unique_lock<std::mutex> lock(taskQueMutex_);

            std::cout << "Thread id: " << std::this_thread::get_id() << " try to acquire task..." << std::endl;

            // Cached模式下，回收超过空闲时间的多余线程
            while (taskQue_.size() == 0) {
                if (!isRunning_) {
                    threads_.erase(thread_id);
                    std::cout << "Thread id: " << std::this_thread::get_id() << " exit..." << std::endl;
                    exitCond_.notify_all();
                    return ;
                }

                if (poolMode_ == PoolMode::Mode_Cached) {
                    if (std::cv_status::timeout == notEmpty_.wait_for(lock, std::chrono::seconds(1))) {
                        auto nowTime = std::chrono::high_resolution_clock().now();
                        auto durTime = std::chrono::duration_cast<std::chrono::seconds>(nowTime - lastTime);
                        if (durTime.count() >= THREAD_MAX_IDEL_TIME && curThreadSize_ > initThreadSize_) {
                            threads_.erase(thread_id);
                            std::cout << "---Thread exit, id: " << std::this_thread::get_id() << "---" << std::endl; 
                            curThreadSize_--;
                            idleThreadSize_--;
                            return ;
                        }
                    }
                } else {
                    // 等待notEmpty条件
                    notEmpty_.wait(lock);
                }

                // if (!isRunning_) {
                //     threads_.erase(thread_id);
                //     exitCond_.notify_all();
                //     std::cout << "Thread id: " << std::this_thread::get_id() << " exit..." << std::endl;
                //     return ;
                // }
            }

            std::cout << "Thread id: " << std::this_thread::get_id() << " acquire task successfully..." << std::endl;
            idleThreadSize_--;

            // 从任务队列中获取任务
            task = taskQue_.front();
            taskQue_.pop();
            taskSize_--;

            if (taskSize_ > 0) {
                notEmpty_.notify_all();
            }

            notFull_.notify_all();
        }

        // 当前线程执行该任务
        if (task != nullptr) {
            task->exec();
        }

        idleThreadSize_++;
        lastTime = std::chrono::high_resolution_clock().now();
    }
}


/*
线程类方法实现
*/
int Thread::genId_ = 0;

Thread::Thread(ThreadFunc func) 
    : func_(func),
      threadId_(genId_++) {
}

Thread::~Thread() {    
}

// 启动线程
void Thread::start() {
    std::thread t(func_, threadId_);
    t.detach();
}

// 查询线程id
int Thread::getId() const {
    return threadId_;
}

/*
Result类方法实现
*/
Result::Result(std::shared_ptr<Task> task, bool isValid)
    : task_(task), 
      isValid_(isValid) {
      task_->setResult(this);
}

Any Result::get() {
    if (!isValid_) {
        return NULL;
    }
    sem_.wait();
    return std::move(any_);
}

void Result::setVal(Any any) {
    this->any_ = std::move(any);
    sem_.post();
}