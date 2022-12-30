//
// Created by hh on 2022/12/28.
//

#ifndef LEVELDB_MEM_VERSION_SET_H
#define LEVELDB_MEM_VERSION_SET_H

#include <port/port_stdcxx.h>

#include "memtable.h"
#include "unordered_map"

namespace leveldb {



static inline void delete_mems(const vector<MemTable*>& mems_todel) {
  for (auto item : mems_todel)
    delete item;
}

class mems_todel {
 public:

  void push(MemTable* memTable);

  vector<MemTable*> get();


 private:
  //访问reps_的锁
  port::Mutex mutex_;
  vector<MemTable*> reps_;
};

//创建这个不用ref
class mems_boundary {
 public:
  explicit mems_boundary(vector<saxt_only>& bounds) :refs_(0), bounds(bounds){}

  void Ref();

  void Unref();

  vector<saxt_only> bounds;

 private:
  int refs_;
};

//创建这个不用ref
class mem_version {
 public:
  mem_version(vector<MemTable*> &new_mems, mems_boundary* boundary);

  void Ref();

  // Drop reference count.  Delete if no more references exist.

  void Unref();

  ~mem_version();

  //这个版本的mems
  vector<MemTable*> mems;
  mems_boundary* boundary;
 private:
  int refs_;

};


class mem_version_set {
 public:
  mem_version_set();


  mem_version* CurrentVersion();

  int CurrentVersionId();

  //查询,没有就是nullptr
  mem_version* OldVersion(int id);

  // 表满了会， 大重组会, 会上报
  int newversion(mem_version* memVersion);

  //小重组不改版本号，不上报
  void newversion_small(MemTable* newMem, int memId);

  //这个只有master来控制删除
  void Unref(int id);

 private:
  //版本号和对应的版本
  unordered_map<int, mem_version*> versions;
  int currentid;
  mem_version* currentversion;
};


}


#endif  // LEVELDB_MEM_VERSION_SET_H
