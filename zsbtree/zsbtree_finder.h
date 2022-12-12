//
// Created by hh on 2022/12/11.
//

#ifndef LEVELDB_ZSBTREE_FINDER_H
#define LEVELDB_ZSBTREE_FINDER_H
#include "zsbtree_table.h"


// -1 0 1代表在点左边，中间，右边


class ZsbTree_finder{
 public:
  
  explicit ZsbTree_finder(vector<LeafKey>& simiLeakKeys) : simi_leakKeys(simiLeakKeys) {}

 public:

  void root_Get(NonLeaf &nonLeaf, LeafKey &leafKey);

 private:
  void l_Get_NonLeaf(NonLeafKey &nonLeafKey, LeafKey &leafKey, bool isleaf);

  void r_Get_NonLeaf(NonLeafKey &nonLeafKey, LeafKey &leafKey, bool isleaf);

  inline void leaf_Get(Leaf &leaf, LeafKey &leafKey);

// 在nonLeaf的范围里, 才调用
  void nonLeaf_Get(NonLeaf &nonLeaf, LeafKey &leafKey);



 private:
  vector<LeafKey> &simi_leakKeys;

};



#endif  // LEVELDB_ZSBTREE_FINDER_H
