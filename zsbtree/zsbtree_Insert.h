//
// 插入，假设叶结点每满n
// Created by hh on 2022/11/25.
//

#ifndef TODOZSBTREE_ZSBTREE_INSERT_H
#define TODOZSBTREE_ZSBTREE_INSERT_H



#include <globals.h>
#include "arena.h"
#include "vector"
#include "sax.h"
#include "Leaf.h"
#include "NonLeaf.h"
#include "Cmp.h"
#include "../util/arena.h"

// -1 0 1代表在点左边，中间，右边
inline int whereofKey(saxt lsaxt, saxt rsaxt, saxt leafKey, cod co_d){
    if (saxt_cmp(leafKey, rsaxt, co_d) && saxt_cmp(lsaxt, leafKey, co_d)) return 0;
    if (saxt_cmp(leafKey, lsaxt, co_d)) return -1;
    if (saxt_cmp(rsaxt, leafKey, co_d)) return 1;
}


bool l_Insert_NonLeaf(NonLeafKey &nonLeafKey, LeafKey &leafKey, cod co_d, bool isleaf) {
    nonLeafKey.setLsaxt(leafKey.asaxt);
    nonLeafKey.co_d = get_co_d_from_saxt(nonLeafKey.lsaxt, nonLeafKey.rsaxt, co_d);
    if (isleaf) {
        Leaf &leaf = *((Leaf *)nonLeafKey.p);
        leaf.setLsaxt(leafKey.asaxt);
        leaf.co_d = nonLeafKey.co_d;
        leaf.add(leafKey);
        leaf.num++;
        return leaf.num != Leaf_rebuildnum;
    } else {
        NonLeaf &nonLeaf = *((NonLeaf *)nonLeafKey.p);
        nonLeaf.setLsaxt(leafKey.asaxt);
        nonLeaf.co_d = nonLeafKey.co_d;
        return l_Insert_NonLeaf(nonLeaf.nonLeafKeys[0], leafKey, nonLeafKey.co_d, nonLeaf.isleaf);
    }
}

bool r_Insert_NonLeaf(NonLeafKey &nonLeafKey, LeafKey &leafKey, cod co_d, bool isleaf) {
    nonLeafKey.setRsaxt(leafKey.asaxt);
    nonLeafKey.co_d = get_co_d_from_saxt(nonLeafKey.lsaxt, nonLeafKey.rsaxt, co_d);
    if (isleaf) {
        Leaf &leaf = *((Leaf *)nonLeafKey.p);
        leaf.setRsaxt(leafKey.asaxt);
        leaf.co_d = nonLeafKey.co_d;
        leaf.add(leafKey);
        leaf.num++;
        return leaf.num != Leaf_rebuildnum;
    } else {
        NonLeaf &nonLeaf = *((NonLeaf *)nonLeafKey.p);
        nonLeaf.setRsaxt(leafKey.asaxt);
        nonLeaf.co_d = nonLeafKey.co_d;
        return l_Insert_NonLeaf(nonLeaf.nonLeafKeys[nonLeaf.num-1], leafKey, nonLeafKey.co_d, nonLeaf.isleaf);
    }
}


inline bool leaf_Insert(Leaf &leaf, LeafKey &leafKey) {
    leaf.add(leafKey);
    leaf.num++;
    return leaf.num != Leaf_rebuildnum;
}


// 在nonLeaf的范围里, 才调用
bool nonLeaf_Insert(NonLeaf &nonLeaf, LeafKey &leafKey) {

    int l=0;
    int r=nonLeaf.num-1;
    while (l<r) {
        int mid = (l + r) / 2;
        if (saxt_cmp(leafKey.asaxt, nonLeaf.nonLeafKeys[mid].rsaxt, nonLeaf.co_d)) r = mid;
        else l = mid + 1;
    }
    int pos = whereofKey(nonLeaf.nonLeafKeys[l].lsaxt, nonLeaf.nonLeafKeys[l].rsaxt, leafKey.asaxt, nonLeaf.co_d);
    if (pos==0) {
        //里面 直接插入
        if(nonLeaf.isleaf) return leaf_Insert(*(Leaf *)(nonLeaf.nonLeafKeys[l].p), leafKey);
        else return nonLeaf_Insert(*(NonLeaf *)(nonLeaf.nonLeafKeys[l].p), leafKey);
    } else if (pos==-1){
        //前面有 先比相聚度下降程度,再看数量,但目前没有在非叶节点记录这种东西，所以这里直接比相聚度大小
        cod preco_d = nonLeaf.nonLeafKeys[l-1].co_d;
        saxt prelsaxt = nonLeaf.nonLeafKeys[l-1].lsaxt;
        cod nextco_d = nonLeaf.nonLeafKeys[l].co_d;
        saxt nextrsaxt = nonLeaf.nonLeafKeys[l].rsaxt;
        cod co_d1 = get_co_d_from_saxt(prelsaxt, leafKey.asaxt, nonLeaf.co_d);
        cod co_d2 = get_co_d_from_saxt(leafKey.asaxt, nextrsaxt, nonLeaf.co_d);
        if ((preco_d - co_d1) < (nextco_d - co_d2)) {
            // 跟前面
            return r_Insert_NonLeaf(nonLeaf.nonLeafKeys[l-1], leafKey, nonLeaf.co_d, nonLeaf.isleaf);
        } else if ((preco_d - co_d1) > (nextco_d - co_d2)) {
            //跟后面
            return l_Insert_NonLeaf(nonLeaf.nonLeafKeys[l], leafKey, nonLeaf.co_d, nonLeaf.isleaf);
        } else {
            if (co_d1 <= co_d2) {
                return r_Insert_NonLeaf(nonLeaf.nonLeafKeys[l-1], leafKey, nonLeaf.co_d, nonLeaf.isleaf);
            } else {
                return l_Insert_NonLeaf(nonLeaf.nonLeafKeys[l], leafKey, nonLeaf.co_d, nonLeaf.isleaf);
            }
        }

    } else {
        //后面有
        cod preco_d = nonLeaf.nonLeafKeys[l].co_d;
        saxt prelsaxt = nonLeaf.nonLeafKeys[l].lsaxt;
        cod nextco_d = nonLeaf.nonLeafKeys[l+1].co_d;
        saxt nextrsaxt = nonLeaf.nonLeafKeys[l+1].rsaxt;
        cod co_d1 = get_co_d_from_saxt(prelsaxt, leafKey.asaxt, nonLeaf.co_d);
        cod co_d2 = get_co_d_from_saxt(leafKey.asaxt, nextrsaxt, nonLeaf.co_d);
        if ((preco_d - co_d1) < (nextco_d - co_d2)) {
            // 跟前面
            return r_Insert_NonLeaf(nonLeaf.nonLeafKeys[l], leafKey, nonLeaf.co_d, nonLeaf.isleaf);
        } else if ((preco_d - co_d1) > (nextco_d - co_d2)) {
            //跟后面
            return l_Insert_NonLeaf(nonLeaf.nonLeafKeys[l+1], leafKey, nonLeaf.co_d, nonLeaf.isleaf);
        } else {
            if (co_d1 <= co_d2) {
                return r_Insert_NonLeaf(nonLeaf.nonLeafKeys[l], leafKey, nonLeaf.co_d, nonLeaf.isleaf);
            } else {
                return l_Insert_NonLeaf(nonLeaf.nonLeafKeys[l+1], leafKey, nonLeaf.co_d, nonLeaf.isleaf);
            }
        }
    }

}

// 从根节点插入，设叶节点无限大 , 返回true ,叶结点放得下，false，要重组了
bool root_Insert(NonLeaf &nonLeaf, LeafKey &leafKey) {
    int pos = whereofKey(nonLeaf.lsaxt, nonLeaf.rsaxt, leafKey.asaxt, 0);
    if (pos==0) return nonLeaf_Insert(nonLeaf, leafKey);
    else if (pos==-1) {
        cod co_d = get_co_d_from_saxt(leafKey.asaxt, nonLeaf.nonLeafKeys[0].rsaxt);
        return l_Insert_NonLeaf(nonLeaf.nonLeafKeys[0], leafKey, co_d, nonLeaf.isleaf);
    } else {
        cod co_d = get_co_d_from_saxt(nonLeaf.nonLeafKeys[nonLeaf.num-1].lsaxt, leafKey.asaxt);
        return r_Insert_NonLeaf(nonLeaf.nonLeafKeys[nonLeaf.num-1] ,leafKey, co_d, nonLeaf.isleaf);
    }
}







#endif //TODOZSBTREE_ZSBTREE_INSERT_H
