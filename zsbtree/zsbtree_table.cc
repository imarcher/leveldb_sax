//
// Created by hh on 2022/12/1.
//

#include "zsbtree_table.h"
#include "zsbtree_finder.h"




namespace leveldb {

bool zsbtree_table::Insert(LeafKey& leafKey) {
  return root_Insert(*root, leafKey);
}

//初始化用
void zsbtree_table::BuildTree(newVector<NonLeafKey>& nonLeafKeys) {
  leafNum = nonLeafKeys.size();
  root = build_tree_from_nonleaf(nonLeafKeys);
}


zsbtree_table_mem zsbtree_table::Rebalance(int tmp_leaf_maxnum,
                                           int tmp_leaf_minnum, int Nt) {
  vector<LeafKey> sortleafKeys;
  sortleafKeys.reserve(Nt);
  //从root开始dfs
  Rebalance_dfs(root, sortleafKeys);
  newVector<LeafKey> new_sortleafKeys(sortleafKeys);
  int leafNum;
  return {build_tree_from_leaf(new_sortleafKeys, tmp_leaf_maxnum,
                               tmp_leaf_minnum, leafNum),
          leafNum};
}

void zsbtree_table::Rebalance_dfs(NonLeaf* nonLeaf,
                                  vector<LeafKey>& sortleafKeys) {
  //查看下一层是否是leaf
  if (nonLeaf->isleaf) {
    //遍历子结点
    for (int i = 0; i < nonLeaf->num; i++) {
      Leaf* tmpleaf = (Leaf*)nonLeaf->nonLeafKeys[i].p;
      if (tmpleaf->num) {
        auto dst = sortleafKeys.data() + sortleafKeys.size();
        sortleafKeys.resize(sortleafKeys.size() + tmpleaf->num);
        tmpleaf->sort(dst);
      }
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
    mempcpy(dst, nonLeaf->nonLeafKeys, sizeof(NonLeafKey) * nonLeaf->num);
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
  iscopy = true;
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
      NonLeafKey& this_nonLeafKeys = nonLeaf->nonLeafKeys[i];
      this_nonLeafKeys.num = 0;
      ((Leaf*)this_nonLeafKeys.p)->sort();
      this_nonLeafKeys.p = leafp;
      ((Leaf*)leafp)->set(this_nonLeafKeys);
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
  if (root != nullptr) {
    //有new的，还有统一的，也就是copy
    if (!iscopy) DelTree_dfs(root);
    else {
      DelTree_dfs_2(root);
      free(root);
    }
  }
}

void zsbtree_table::DelTree_dfs(NonLeaf* nonLeaf) {
  if (nonLeaf->isleaf) {
    if (!isleafuse) {
      for (int i = 0; i < nonLeaf->num; i++) {
        delete (Leaf*)nonLeaf->nonLeafKeys[i].p;
      }
    }
  } else {
    for (int i = 0; i < nonLeaf->num; i++) {
      DelTree_dfs((NonLeaf*)nonLeaf->nonLeafKeys[i].p);
    }
  }
  delete nonLeaf;
}

void zsbtree_table::DelTree_dfs_2(NonLeaf* nonLeaf) {
  if (nonLeaf->isleaf) {
    if (!isleafuse) {
      free(nonLeaf->nonLeafKeys[0].p);
    }
  } else {
    for (int i = 0; i < nonLeaf->num; i++) {
      DelTree_dfs_2((NonLeaf*)nonLeaf->nonLeafKeys[i].p);
    }
    free(nonLeaf->nonLeafKeys[0].p);
  }
}




}




