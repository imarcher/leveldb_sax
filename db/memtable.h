// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_DB_MEMTABLE_H_
#define STORAGE_LEVELDB_DB_MEMTABLE_H_

#include <string>

#include "db/dbformat.h"
#include "db/skiplist.h"
#include "leveldb/db.h"
#include "util/arena.h"
#include "zsbtree/zsbtree_table.h"

namespace leveldb {

class InternalKeyComparator;
class MemTableIterator;

class MemTable {
 public:
  // MemTables are reference counted.  The initial reference count
  // is zero and the caller must call Ref() at least once.
  MemTable();
  MemTable(MemTable* im);
  MemTable(zsbtree_table_mem table_mem);

  MemTable(const MemTable&) = delete;
  MemTable& operator=(const MemTable&) = delete;

  // Increase reference count.
  void Ref() { ++refs_; }

  // Drop reference count.  Delete if no more references exist.
  void Unref() {
    --refs_;
    assert(refs_ >= 0);
    if (refs_ <= 0) {
      delete this;
    }
  }

  // Returns an estimate of the number of bytes of data in use by this
  // data structure. It is safe to call when MemTable is being modified.
//  size_t ApproximateMemoryUsage();

  // Return an iterator that yields the contents of the memtable.
  //
  // The caller must ensure that the underlying MemTable remains live
  // while the returned iterator is live.  The keys returned by this
  // iterator are internal keys encoded by AppendInternalKey in the
  // db/format.{h,cc} module.
  Iterator* NewIterator();

  // Add an entry into memtable that maps key to value at the
  // specified sequence number and with the specified type.
  // Typically value will be empty if type==kTypeDeletion.
  bool Add(SequenceNumber seq, saxt saxt_, uint64_t fileOffset);

  // If memtable contains a value for key, store it in *value and return true.
  // If memtable contains a deletion for key, store a NotFound() error
  // in *status and return true.
  // Else, return false.
  void Get(const saxt key, vector<LeafKey>& leafKeys);

  MemTable* BuildTree_new(newVector<NonLeafKey>& nonLeafKeys);
  saxt Getlsaxt();
  saxt Getrsaxt();
  cod Getcod();
  MemTable* Rebalance(int tmp_leaf_maxnum, int tmp_leaf_minnum, int Nt);
  int GetleafNum();
  void LoadNonLeafKeys(vector<NonLeafKey> &nonLeafKeys);
 private:
  friend class MemTableIterator;
  friend class MemTableBackwardIterator;

//  struct KeyComparator {
//    const InternalKeyComparator comparator;
//    explicit KeyComparator(const InternalKeyComparator& c) : comparator(c) {}
//    int operator()(const char* a, const char* b) const;
//  };

//  typedef SkipList<const char*, KeyComparator> Table;



  ~MemTable();  // Private since only Unref() should be used to delete it

  //add的时候用
  LeafKey tmpLeafKey;
//  KeyComparator comparator_;
  int refs_;

 public:
//  Arena arena_;
  typedef zsbtree_table Table;
  //根结点
  Table table_;
};

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_DB_MEMTABLE_H_
