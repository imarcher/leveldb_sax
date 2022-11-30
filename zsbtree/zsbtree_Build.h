//
// Created by hh on 2022/11/26.
//

#ifndef TODOZSBTREE_ZSBTREE_BUILD_H
#define TODOZSBTREE_ZSBTREE_BUILD_H

#include "zsbtree_LeafBuild.h"
#include "zsbtree_NonLeafBuild.h"

NonLeaf* build_tree(vector<LeafKey> &leafKeys) {
    vector<NonLeafKey> nonleafKeys;
    leaf_method::buildtree(leafKeys, nonleafKeys);
    vector<NonLeafKey> &nonleafKeys_in = nonleafKeys;
    bool isleaf = true;
    while (nonleafKeys.size()>Leaf_maxnum) {
        //创建下一层
        vector<NonLeafKey> nonleafKeys_out;
        nonleaf_method::buildtree(nonleafKeys_in, nonleafKeys_out, isleaf);
        if (isleaf) isleaf = false;
        nonleafKeys_in = nonleafKeys_out;
//        for (int i=0;i<10;i++) {
//            out("l");
//            saxt_print(((Leaf *)(nonleafKeys_in[i].p))->lsaxt);
//            out("r");
//            saxt_print(((Leaf *)(nonleafKeys_in[i].p))->rsaxt);
//        }
    }
    saxt lsaxt = nonleafKeys_in.at(0).lsaxt;
    saxt rsaxt = nonleafKeys_in.back().rsaxt;
    cod co_d = get_co_d_from_saxt(lsaxt, rsaxt);
    return new NonLeaf(nonleafKeys_in.size(), co_d, isleaf, lsaxt, rsaxt, nonleafKeys_in.data());
}


#endif //TODOZSBTREE_ZSBTREE_BUILD_H
