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
  leafNum = nonLeafKeys.size();
}

void zsbtree_table::Rebalance(int tmp_leaf_maxnum, int tmp_leaf_minnum, int Nt) {
  vector<LeafKey> sortleafKeys;
  sortleafKeys.reserve(Nt);
  //从root开始dfs删除
  Rebalance_dfs(root, sortleafKeys);
  newVector<LeafKey> new_sortleafKeys(sortleafKeys);
  root = build_tree_from_leaf(new_sortleafKeys, tmp_leaf_maxnum, tmp_leaf_minnum, leafNum);
}

void zsbtree_table::Rebalance_dfs(NonLeaf* nonLeaf, vector<LeafKey>& sortleafKeys) {
  //查看下一层是否是leaf
  if (nonLeaf->isleaf) {
    //遍历子结点
    for(int i=0;i<nonLeaf->num;i++) {
      Leaf* tmpleaf = (Leaf*)nonLeaf->nonLeafKeys[i].p;
      tmpleaf->sort();
      auto dst = sortleafKeys.data() + sortleafKeys.size();
      sortleafKeys.resize(sortleafKeys.size() + tmpleaf->num);
      mempcpy(dst, tmpleaf->leafKeys, leaf_key_size * tmpleaf->num);
      free(tmpleaf);
    }
  }
  else {
    //遍历子结点
    for(int i=0;i<nonLeaf->num;i++) {
      Rebalance_dfs((NonLeaf*)nonLeaf->nonLeafKeys[i].p, sortleafKeys);
    }
  }
  free(nonLeaf);
}

void zsbtree_table::LoadNonLeafKeys(vector<NonLeafKey>& nonLeafKeys) {
  LoadNonLeafKeys_dfs(root, nonLeafKeys);
}

void zsbtree_table::LoadNonLeafKeys_dfs(NonLeaf* nonLeaf, vector<NonLeafKey>& nonLeafKeys) {
  //查看下一层是否是leaf
  if (nonLeaf->isleaf) {
    auto dst = nonLeafKeys.data() + nonLeafKeys.size();
    nonLeafKeys.resize(nonLeafKeys.size() + nonLeaf->num);
    mempcpy(dst, nonLeaf->nonLeafKeys, nonleaf_key_size * nonLeaf->num);
  }
  else {
    //遍历子结点
    for(int i=0;i<nonLeaf->num;i++) {
      LoadNonLeafKeys_dfs((NonLeaf*)nonLeaf->nonLeafKeys[i].p, nonLeafKeys);
    }
  }
  free(nonLeaf);
}

}




