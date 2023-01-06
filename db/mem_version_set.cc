//
// Created by hh on 2022/12/28.
//

#include "mem_version_set.h"


namespace leveldb {


void mems_todel::push(MemTable* memTable) {
  mutex_.Lock();
  reps_.push_back(memTable);
  mutex_.Unlock();
}

vector<MemTable*> mems_todel::get() {
  mutex_.Lock();
  vector<MemTable*> res = reps_;
  reps_.clear();
  mutex_.Unlock();
  return res;
}


//创建这个不用ref

void mems_boundary::Ref() { ++refs_; }

void mems_boundary::Unref() {
  --refs_;
  assert(refs_ >= 0);
  if (refs_ <= 0) {
    delete this;
  }
}




mem_version::mem_version(vector<MemTable*>& new_mems, mems_boundary* boundary)
    : refs_(0), mems(new_mems), boundary(boundary) {
  boundary->Ref();
  for (auto item : mems) item->Ref();
}

void mem_version::Ref() { ++refs_; }

// Drop reference count.  Delete if no more references exist.

void mem_version::Unref() {
  --refs_;
  assert(refs_ >= 0);
  if (refs_ <= 0) {
    delete this;
  }
}

mem_version::~mem_version() {
  for (auto item : mems) item->Unref();
  boundary->Unref();
}



mem_version_set::mem_version_set() : currentid(0), currentversion(nullptr) {}

//插入
mem_version* mem_version_set::CurrentVersion() { return currentversion; }

//查询,没有就是nullptr
mem_version* mem_version_set::OldVersion(int id) { return versions[id]; }

// 表满了会， 大重组会, 会上报
int mem_version_set::newversion(mem_version* memVersion) {
  memVersion->Ref();
  currentversion = memVersion;
  currentid++;
  versions[currentid] = currentversion;
  return currentid;
}

//小重组不改版本号，不上报
void mem_version_set::newversion_small(MemTable* newMem, int memId) {
  vector<MemTable*> v = currentversion->mems;
  v[memId] = newMem;
  mem_version* newversion = new mem_version(v, currentversion->boundary);
  newversion->Ref();
  currentversion->Unref();
  currentversion = newversion;
  versions[currentid] = currentversion;
}

//这个只有master来控制删除
void mem_version_set::Unref(int id) {
  versions[id]->Unref();
  versions.erase(id);
}

int mem_version_set::CurrentVersionId() { return currentid; }
void mem_version_set::UnrefAll() {
  for(auto item: versions) {
    item.second->Unref();
  }
}

}