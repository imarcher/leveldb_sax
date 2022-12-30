//
// Created by hh on 2022/12/11.
//

#include "zsbtree_finder.h"

void ZsbTree_finder::root_Get(NonLeaf &nonLeaf, LeafKey &leafKey) {
  int pos = whereofKey(nonLeaf.lsaxt, nonLeaf.rsaxt, leafKey.asaxt, 0);
  if (pos==0) nonLeaf_Get(nonLeaf, leafKey);
  else if (pos==-1) {
    l_Get_NonLeaf(nonLeaf.nonLeafKeys[0], leafKey, nonLeaf.isleaf);
  } else {
    r_Get_NonLeaf(nonLeaf.nonLeafKeys[nonLeaf.num-1] ,leafKey, nonLeaf.isleaf);
  }
}


void ZsbTree_finder::l_Get_NonLeaf(NonLeafKey &nonLeafKey, LeafKey &leafKey, bool isleaf) {

  if (isleaf) {
    Leaf &leaf = *((Leaf *)nonLeafKey.p);
    simi_leakKeys.reserve(leaf.num);
    simi_leakKeys.resize(leaf.num);
    memcpy(simi_leakKeys.data(), leaf.leafKeys, sizeof(LeafKey) * leaf.num);
  } else {
    NonLeaf &nonLeaf = *((NonLeaf *)nonLeafKey.p);
    l_Get_NonLeaf(nonLeaf.nonLeafKeys[0], leafKey, nonLeaf.isleaf);
  }
}

void ZsbTree_finder::r_Get_NonLeaf(NonLeafKey &nonLeafKey, LeafKey &leafKey, bool isleaf) {
  if (isleaf) {
    Leaf &leaf = *((Leaf *)nonLeafKey.p);
    simi_leakKeys.reserve(leaf.num);
    simi_leakKeys.resize(leaf.num);
    memcpy(simi_leakKeys.data(), leaf.leafKeys, sizeof(LeafKey) * leaf.num);
  } else {
    NonLeaf &nonLeaf = *((NonLeaf *)nonLeafKey.p);
    l_Get_NonLeaf(nonLeaf.nonLeafKeys[nonLeaf.num-1], leafKey, nonLeaf.isleaf);
  }
}


inline void ZsbTree_finder::leaf_Get(Leaf &leaf, LeafKey &leafKey) {
  simi_leakKeys.reserve(leaf.num);
  simi_leakKeys.resize(leaf.num);
  memcpy(simi_leakKeys.data(), leaf.leafKeys, sizeof(LeafKey) * leaf.num);
}


// 在nonLeaf的范围里, 才调用
void ZsbTree_finder::nonLeaf_Get(NonLeaf &nonLeaf, LeafKey &leafKey) {

  int l=0;
  int r=nonLeaf.num-1;
  while (l<r) {
    int mid = (l + r) / 2;
    if (saxt_cmp(leafKey.asaxt, nonLeaf.nonLeafKeys[mid].rsaxt, nonLeaf.co_d)) r = mid;
    else l = mid + 1;
  }
  int pos = whereofKey(nonLeaf.nonLeafKeys[l].lsaxt, nonLeaf.nonLeafKeys[l].rsaxt, leafKey.asaxt, nonLeaf.co_d);
  if (pos==0) {
    //里面
    if(nonLeaf.isleaf) leaf_Get(*(Leaf *)(nonLeaf.nonLeafKeys[l].p), leafKey);
    else nonLeaf_Get(*(NonLeaf *)(nonLeaf.nonLeafKeys[l].p), leafKey);
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
      r_Get_NonLeaf(nonLeaf.nonLeafKeys[l-1], leafKey, nonLeaf.isleaf);
    } else if ((preco_d - co_d1) > (nextco_d - co_d2)) {
      //跟后面
      l_Get_NonLeaf(nonLeaf.nonLeafKeys[l], leafKey, nonLeaf.isleaf);
    } else {
      if (co_d1 <= co_d2) {
        r_Get_NonLeaf(nonLeaf.nonLeafKeys[l-1], leafKey, nonLeaf.isleaf);
      } else {
        l_Get_NonLeaf(nonLeaf.nonLeafKeys[l], leafKey, nonLeaf.isleaf);
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
      r_Get_NonLeaf(nonLeaf.nonLeafKeys[l], leafKey, nonLeaf.isleaf);
    } else if ((preco_d - co_d1) > (nextco_d - co_d2)) {
      //跟后面
      l_Get_NonLeaf(nonLeaf.nonLeafKeys[l+1], leafKey, nonLeaf.isleaf);
    } else {
      if (co_d1 <= co_d2) {
        r_Get_NonLeaf(nonLeaf.nonLeafKeys[l], leafKey, nonLeaf.isleaf);
      } else {
        l_Get_NonLeaf(nonLeaf.nonLeafKeys[l+1], leafKey, nonLeaf.isleaf);
      }
    }
  }

}