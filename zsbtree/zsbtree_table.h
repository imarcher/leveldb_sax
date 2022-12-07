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



  //false 重组
  bool Insert(LeafKey& leafKey);

  void BuildTree(newVector<NonLeafKey>& nonLeafKeys);

  void Rebalance(int tmp_leaf_maxnum, int tmp_leaf_minnum, int Nt);

  void LoadNonLeafKeys(vector<NonLeafKey>& nonLeafKeys);

  //叶子数量， drange重构时用
  int leafNum;
  NonLeaf* root;

 private:
  void Rebalance_dfs(NonLeaf* nonLeaf, vector<LeafKey>& sortleafKeys);

  void LoadNonLeafKeys_dfs(NonLeaf* nonLeaf, vector<NonLeafKey>& nonLeafKeys);
};

}
#endif  // LEVELDB_ZSBTREE_TABLE_H
