//
// Created by hh on 2022/12/1.
//

#include "zsbtree_table.h"





namespace leveldb{


zsbtree_table::zsbtree_table(Arena* const arena_) : arena_(arena_) {}


void zsbtree_table::Insert(LeafKey& leafKey) {
  root_Insert(*root, leafKey);
}


};



