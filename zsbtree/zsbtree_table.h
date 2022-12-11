//
// Created by hh on 2022/12/1.
//

#ifndef LEVELDB_ZSBTREE_TABLE_H
#define LEVELDB_ZSBTREE_TABLE_H


#include "zsbtree_Build.h"
#include "zsbtree_Insert.h"
#include "STpos.h"
#include "STkeyinfo.h"
#include "zsbtree_Get.h"

namespace leveldb {

typedef struct {
  NonLeaf* root;
  int leafNum;
} zsbtree_table_mem;

class zsbtree_table {
 public:

  zsbtree_table();

  zsbtree_table(zsbtree_table &im);

  zsbtree_table(zsbtree_table_mem table_mem);

  ~zsbtree_table();

  //false 重组
  bool Insert(LeafKey& leafKey);

  void BuildTree(newVector<NonLeafKey>& nonLeafKeys);

  zsbtree_table_mem BuildTree_new(newVector<NonLeafKey>& nonLeafKeys);

  zsbtree_table_mem Rebalance(int tmp_leaf_maxnum, int tmp_leaf_minnum, int Nt);

  void LoadNonLeafKeys(vector<NonLeafKey>& nonLeafKeys);

  //查
  void GetLeafKeys(saxt key, vector<LeafKey>& leafKeys);

  //标记叶子结点是否被使用，主要用来看删除不删除叶子
  bool isleafuse = false;
  //叶子数量， drange重构时用
  int leafNum;
  NonLeaf* root;

 private:
  void Rebalance_dfs(NonLeaf* nonLeaf, vector<LeafKey>& sortleafKeys);

  void LoadNonLeafKeys_dfs(NonLeaf* nonLeaf, vector<NonLeafKey>& nonLeafKeys);

  void CopyTree_dfs(NonLeaf* nonLeaf, NonLeaf* copyNonLeaf);

  void DelTree_dfs(NonLeaf* nonLeaf);
};

}
#endif  // LEVELDB_ZSBTREE_TABLE_H
