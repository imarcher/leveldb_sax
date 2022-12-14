//
// Created by hh on 2022/12/14.
//

#include "ST_merge.h"
#include "leveldb/cache.h"


namespace leveldb {

struct TableAndFile {
  RandomAccessFile* file;
  Table* table;
};


ST_merge::ST_merge(VersionSet* ver, Compaction* c) : cache(ver->table_cache_){
  for (auto & files : c->inputs_) {
    for (auto & file : files) {
      Cache::Handle* handle = nullptr;
      Status s = cache->FindTable(file->number, file->file_size, &handle);
      if (s.ok()) {
        Table* t = reinterpret_cast<TableAndFile*>(cache->cache_->Value(handle))->table;
        Table::ST_Iter* stIter = new Table::ST_Iter(t);
        st_iters.insert(stIter);
        handles[stIter] = handle;
//        cache->cache_->Release(handle);
      }
    }
  }

  for(auto item: st_iters) {
    LeafKey leafKey;
    if (item->next(leafKey)) heap.emplace(leafKey, item);
    else {
      del(item);
    }
  }

}

bool ST_merge::next(LeafKey& leafKey) {
  if (!heap.empty()){
    leafKey.Set(heap.top().first);
    auto item = heap.top().second;
    heap.pop();
    LeafKey leafKey1;
    if (item->next(leafKey1)) heap.emplace(leafKey1, item);
    else {
      del(item);
    }
    return true;
  }
  return false;
}


void ST_merge::del(Table::ST_Iter* it) {
  cache->cache_->Release(handles[it]);
  handles.erase(it);
  st_iters.erase(it);
  delete it;
}


}
