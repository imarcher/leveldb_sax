//
// Created by hh on 2022/12/1.
//

#include "zsbtree_table.h"





namespace leveldb{


zsbtree_table::zsbtree_table(Arena* const arena_) : arena_(arena_) {}


bool zsbtree_table::Insert(LeafKey& leafKey) {
  return root_Insert(*root, leafKey);
}


};



