//
// Created by hh on 2022/12/17.
//

#include "threadPool_2.h"

threadPool::threadPool(int number) : stop(false)
{
  if (number <= 0 || number > MAX_THREADS)
    exit(1);
  for (int i = 0; i < number; i++)
  {
    std::cout << "创建第" << i << "个线程 " << std::endl;
    work_threads.emplace_back(threadPool::worker, this);
  }
}

threadPool::~threadPool()
{
  {
    std::unique_lock<std::mutex> lock(queue_mutex);
    stop = true;
  }
  condition.notify_all();
  for (auto &ww : work_threads)
    ww.join();
}

bool threadPool::append(Task task)
{
  /*操作工作队列时一定要加锁，因为他被所有线程共享*/
  queue_mutex.lock();
  tasks_queue.push(task);
  queue_mutex.unlock();
  condition.notify_one(); //线程池添加进去了任务，自然要通知等待的线程
  return true;
}
void *threadPool::worker(void *arg)
{
  threadPool *pool = (threadPool *)arg;
  pool->run();
  return pool;
}
void threadPool::run()
{
  while (!stop)
  {
    std::unique_lock<std::mutex> lk(this->queue_mutex);
    /*　unique_lock() 出作用域会自动解锁　*/
    this->condition.wait(lk, [this] { return !this->tasks_queue.empty(); });
    //如果任务队列不为空，就停下来等待唤醒
    if (this->tasks_queue.empty())
    {
      continue;
    }
    else
    {
      Task task = tasks_queue.front();
      tasks_queue.pop();
      task();
    }
  }
}
