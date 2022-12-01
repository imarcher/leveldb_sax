//
// Created by hh on 2022/12/1.
//

#ifndef LEVELDB_ZSBTREE_TABLE_H
#define LEVELDB_ZSBTREE_TABLE_H


#include "zsbtree_Build.h"
#include "zsbtree_Insert.h"


namespace leveldb {

class zsbtree_table {
 public:
  explicit zsbtree_table(Arena* const arena_);

  void Insert(LeafKey& leafKey);

 private:
  Arena* const arena_;
  NonLeaf* root;
};

}
#endif  // LEVELDB_ZSBTREE_TABLE_H
