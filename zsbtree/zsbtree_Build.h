//
// Created by hh on 2022/11/26.
//

#ifndef TODOZSBTREE_ZSBTREE_BUILD_H
#define TODOZSBTREE_ZSBTREE_BUILD_H

#include "zsbtree_LeafBuild.h"
#include "zsbtree_NonLeafBuild.h"

NonLeaf* build_tree_from_leaf(newVector<LeafKey> &leafKeys, const int n, const int m, int &leafNum) {
    vector<NonLeafKey> nonLeafKeys;
    leaf_method::buildtree(leafKeys, nonLeafKeys, n, m);
    leafNum = nonLeafKeys.size();
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

inline int get_1_Num(int x) {
  int res = 0;
  while(x) x -= x & (-x) , res++;
  return res;
}


int get_drange_rebalance(vector<int> &memNum_period){
  int m = memNum_period.size();
  //平均值
  int nd = 0;
  for(auto item: memNum_period) {
    nd += item;
  }
  nd /= m;
  //调用重构
  //遍历每一个不平衡的结点
  //百分比 1.2，0.8
  vector<float> percentage;
  //从低到高
  int mark = 0;
  int pos = 1;
  for(auto item: memNum_period) {
    float per = (float)item / nd;
    if (abs(per - 1) > 0.2) mark |= pos;
    percentage.push_back((float)item / nd);
    pos <<= 1;
  }

  //本来就平衡了
  if (!mark) return 0;

  //初始化答案
  int res = 1;
  for(int i=0;i<m-1;i++){
    res = (res << 1) | 1;
  }
  int resNum = m;
  //与mark一定会要等于mark，然后不能出现单独的1，
  for (int i = 1; i < 1 << m; i++) {
    if ((i & mark) == mark) {
      if ((i & 3) == 1) continue;
      bool flag = true;
      for (int j = 0; j < m - 1; j++) {
        if (((i >> j) & 7) == 2) {
          flag = false;
          break;
        }
      }
      if (!flag) continue;
      int num_1 = get_1_Num(i);
      if (num_1 >= resNum) continue;
      //格式不合格的排完了
      int ans = i;
      bool isRight = true;
      int ans_id = 0;
      while (ans) {
        if (!(ans & 1)) {
          ans_id++;
          ans >>= 1;
          continue;
        }
        float mean_set = 0;
        int mean_set_num = 0;
        while (ans & 1) {
          mean_set += percentage[ans_id];
          mean_set_num++;
          ans_id++;
          ans >>= 1;
        }
        if(abs(mean_set / mean_set_num - 1) > 0.2) {
          //错误方案
          isRight = false;
          break;
        }
      }
      if (isRight) {
        resNum = num_1;
        res = i;
      }
    }
  }

  return res;
}







#endif //TODOZSBTREE_ZSBTREE_BUILD_H
