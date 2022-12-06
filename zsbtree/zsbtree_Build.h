//
// Created by hh on 2022/11/26.
//

#ifndef TODOZSBTREE_ZSBTREE_BUILD_H
#define TODOZSBTREE_ZSBTREE_BUILD_H

#include "zsbtree_LeafBuild.h"
#include "zsbtree_NonLeafBuild.h"

NonLeaf* build_tree_from_leaf(newVector<LeafKey> &leafKeys, const int n, const int m) {
    vector<NonLeafKey> nonLeafKeys;
    leaf_method::buildtree(leafKeys, nonLeafKeys, n, m);
    vector<NonLeafKey>& nonleafKeys_out = nonLeafKeys;

    bool isleaf = true;
    while (nonLeafKeys.size()>Leaf_maxnum) {
        newVector<NonLeafKey> nonleafKeys_in(nonleafKeys_out);
        //创建下一层
        vector<NonLeafKey> newNonleafKeys_out;
        nonleafKeys_out = newNonleafKeys_out;
        nonleaf_method::buildtree(nonleafKeys_in, nonleafKeys_out, isleaf, Leaf_maxnum, Leaf_minnum);
        if (isleaf) isleaf = false;
//        for (int i=0;i<10;i++) {
//            out("l");
//            saxt_print(((Leaf *)(nonleafKeys_in[i].p))->lsaxt);
//            out("r");
//            saxt_print(((Leaf *)(nonleafKeys_in[i].p))->rsaxt);
//        }
    }
    saxt lsaxt = nonleafKeys_out[0].lsaxt;
    saxt rsaxt = nonleafKeys_out.back().rsaxt;
    cod co_d = get_co_d_from_saxt(lsaxt, rsaxt);
    return new NonLeaf(nonleafKeys_out.size(), co_d, isleaf, lsaxt, rsaxt, nonleafKeys_out.data());
}

NonLeaf* build_tree_from_nonleaf(newVector<NonLeafKey> &nonLeafKeys) {


  vector<NonLeafKey>& nonleafKeys_out = nonLeafKeys.v;

  bool isleaf = true;
  while (nonLeafKeys.size()>Leaf_maxnum) {
    if (isleaf) {
      vector<NonLeafKey> newNonleafKeys_out;
      nonleafKeys_out = newNonleafKeys_out;
      nonleaf_method::buildtree(nonLeafKeys, nonleafKeys_out, isleaf, Leaf_maxnum, Leaf_minnum);
      isleaf = false;
    } else {
      newVector<NonLeafKey> nonleafKeys_in(nonleafKeys_out);
      //创建下一层
      vector<NonLeafKey> newNonleafKeys_out;
      nonleafKeys_out = newNonleafKeys_out;
      nonleaf_method::buildtree(nonleafKeys_in, nonleafKeys_out, isleaf, Leaf_maxnum, Leaf_minnum);
    }
//        for (int i=0;i<10;i++) {
//            out("l");
//            saxt_print(((Leaf *)(nonleafKeys_in[i].p))->lsaxt);
//            out("r");
//            saxt_print(((Leaf *)(nonleafKeys_in[i].p))->rsaxt);
//        }
  }
  if (isleaf) {
    saxt lsaxt = nonLeafKeys[0].lsaxt;
    saxt rsaxt = nonLeafKeys.back().rsaxt;
    cod co_d = get_co_d_from_saxt(lsaxt, rsaxt);
    return new NonLeaf(nonLeafKeys.size(), co_d, isleaf, lsaxt, rsaxt, nonLeafKeys.data());
  } else {
    saxt lsaxt = nonleafKeys_out[0].lsaxt;
    saxt rsaxt = nonleafKeys_out.back().rsaxt;
    cod co_d = get_co_d_from_saxt(lsaxt, rsaxt);
    return new NonLeaf(nonleafKeys_out.size(), co_d, isleaf, lsaxt, rsaxt, nonleafKeys_out.data());
  }
}







#endif //TODOZSBTREE_ZSBTREE_BUILD_H
