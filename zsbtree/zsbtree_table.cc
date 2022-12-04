//
// Created by hh on 2022/12/1.
//

#include "zsbtree_table.h"





namespace leveldb{





bool zsbtree_table::Insert(LeafKey& leafKey) {
  return root_Insert(*root, leafKey);
}


void zsbtree_table::BuildTree(newVector<NonLeafKey>& nonLeafKeys) {
  root = build_tree_from_nonleaf(nonLeafKeys);
}






}




