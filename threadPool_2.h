//
// Created by hh on 2022/12/17.
//

#ifndef LEVELDB_THREADPOOL_2_H
#define LEVELDB_THREADPOOL_2_H

#include <vector>
#include <queue>
#include <thread>
#include <iostream>
#include <stdexcept>
#include <condition_variable>
#include <memory> //unique_ptr
#include <functional>

const int MAX_THREADS = 1000; //最大线程数目

typedef std::function<void(void)> Task;

class threadPool
{
 public:
  /*默认开一个线程*/
  threadPool(int number = 1);
  ~threadPool();
  /*往请求队列＜task_queue＞中添加任务<T *>*/
  bool append(Task task);

 private:
  /*工作线程需要运行的函数,不断的从任务队列中取出并执行*/
  static void *worker(void *arg);
  void run();

 private:
  std::vector<std::thread> work_threads; /*工作线程*/
  std::queue<Task> tasks_queue;		   /*任务队列*/

  std::mutex queue_mutex;
  std::condition_variable condition; /*必须与unique_lock配合使用*/
  bool stop;
};

//#include "threadPool_2.h"
//using namespace std;
//class Test
//{
// public:
//  void process_no_static_bind(const int i, const int j) /*推荐使用*/
//  {
//    cout << "bind:  i==" << i << " "
//         << "j==" << j << endl;
//  }
//};
//int main(void)
//{
//  threadPool pool(6);
//  Test tt_bind;
//  while (true)
//  {
//    pool.append(std::bind(&Test::process_no_static_bind, &tt_bind, 3, 4));
//  }
//}

#endif  // LEVELDB_THREADPOOL_2_H
