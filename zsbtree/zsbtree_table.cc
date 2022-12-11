//
// Created by hh on 2022/12/1.
//

#include "zsbtree_table.h"





namespace leveldb {

bool zsbtree_table::Insert(LeafKey& leafKey) {
  return root_Insert(*root, leafKey);
}

void zsbtree_table::BuildTree(newVector<NonLeafKey>& nonLeafKeys) {
  root = build_tree_from_nonleaf(nonLeafKeys);
  leafNum = nonLeafKeys.size();
}

zsbtree_table_mem zsbtree_table::BuildTree_new(newVector<NonLeafKey>& nonLeafKeys) {
  return {build_tree_from_nonleaf(nonLeafKeys), (int)(nonLeafKeys.size())};
}

zsbtree_table_mem zsbtree_table::Rebalance(int tmp_leaf_maxnum,
                                           int tmp_leaf_minnum, int Nt) {
  vector<LeafKey> sortleafKeys;
  sortleafKeys.reserve(Nt);
  //从root开始dfs删除
  Rebalance_dfs(root, sortleafKeys);
  newVector<LeafKey> new_sortleafKeys(sortleafKeys);
  int newleafNum;
  return {build_tree_from_leaf(new_sortleafKeys, tmp_leaf_maxnum,
                               tmp_leaf_minnum, leafNum),
          newleafNum};
}

void zsbtree_table::Rebalance_dfs(NonLeaf* nonLeaf,
                                  vector<LeafKey>& sortleafKeys) {
  //查看下一层是否是leaf
  if (nonLeaf->isleaf) {
    //遍历子结点
    for (int i = 0; i < nonLeaf->num; i++) {
      Leaf* tmpleaf = (Leaf*)nonLeaf->nonLeafKeys[i].p;
      tmpleaf->sort();
      auto dst = sortleafKeys.data() + sortleafKeys.size();
      sortleafKeys.resize(sortleafKeys.size() + tmpleaf->num);
      mempcpy(dst, tmpleaf->leafKeys, leaf_key_size * tmpleaf->num);
    }
  } else {
    //遍历子结点
    for (int i = 0; i < nonLeaf->num; i++) {
      Rebalance_dfs((NonLeaf*)nonLeaf->nonLeafKeys[i].p, sortleafKeys);
    }
  }
}

void zsbtree_table::LoadNonLeafKeys(vector<NonLeafKey>& nonLeafKeys) {
  isleafuse = true;
  LoadNonLeafKeys_dfs(root, nonLeafKeys);
}

void zsbtree_table::LoadNonLeafKeys_dfs(NonLeaf* nonLeaf,
                                        vector<NonLeafKey>& nonLeafKeys) {
  //查看下一层是否是leaf
  if (nonLeaf->isleaf) {
    auto dst = nonLeafKeys.data() + nonLeafKeys.size();
    nonLeafKeys.resize(nonLeafKeys.size() + nonLeaf->num);
    mempcpy(dst, nonLeaf->nonLeafKeys, nonleaf_key_size * nonLeaf->num);
  } else {
    //遍历子结点
    for (int i = 0; i < nonLeaf->num; i++) {
      LoadNonLeafKeys_dfs((NonLeaf*)nonLeaf->nonLeafKeys[i].p, nonLeafKeys);
    }
  }
}

zsbtree_table::zsbtree_table() {}

zsbtree_table::zsbtree_table(zsbtree_table& im) {
  //复制树结构
  //先申请内存
  root = (NonLeaf*)malloc(sizeof(NonLeaf));
  CopyTree_dfs(root, im.root);
}

zsbtree_table::zsbtree_table(zsbtree_table_mem table_mem) {
  leafNum = table_mem.leafNum;
  root = table_mem.root;
}

void zsbtree_table::CopyTree_dfs(NonLeaf* nonLeaf, NonLeaf* copyNonLeaf) {
  memcpy(nonLeaf, copyNonLeaf, sizeof(NonLeaf));
  if (copyNonLeaf->isleaf) {
    //给叶子申请空间
    char* leafp = (char*)malloc(nonLeaf->num * sizeof(Leaf));
    for (int i = 0; i < nonLeaf->num; i++) {
      nonLeaf->nonLeafKeys[i].num = 0;
      nonLeaf->nonLeafKeys[i].p = leafp;
      ((Leaf*)leafp)->set(nonLeaf->nonLeafKeys[i]);
      leafp += sizeof(Leaf);
    }
  } else {
    //给非叶子申请空间
    char* nonLeafp = (char*)malloc(nonLeaf->num * sizeof(NonLeaf));
    for (int i = 0; i < nonLeaf->num; i++) {
      nonLeaf->nonLeafKeys[i].p = nonLeafp;
      CopyTree_dfs((NonLeaf*)nonLeafp, (NonLeaf*)copyNonLeaf->nonLeafKeys[i].p);
      nonLeafp += sizeof(NonLeaf);
    }
  }
}

zsbtree_table::~zsbtree_table() {
  if (root != nullptr) DelTree_dfs(root);
}

void zsbtree_table::DelTree_dfs(NonLeaf* nonLeaf) {
  if (nonLeaf->isleaf) {
    if (!isleafuse) {
      for (int i = 0; i < nonLeaf->num; i++) {
        free((Leaf*)nonLeaf->nonLeafKeys[i].p);
      }
    }
  } else {
    for (int i = 0; i < nonLeaf->num; i++) {
      DelTree_dfs((NonLeaf*)nonLeaf->nonLeafKeys[i].p);
    }
  }
  free(nonLeaf);
}

//查
void zsbtree_table::GetLeafKeys(saxt key, vector<LeafKey>& leafKeys) {
  ZsbTree_finder zsbTreeFinder(leafKeys);
  LeafKey leafKey(key);
  zsbTreeFinder.root_Get(*root, leafKey);
}

}




