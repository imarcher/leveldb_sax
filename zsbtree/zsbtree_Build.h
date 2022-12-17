//
// Created by hh on 2022/11/26.
//

#ifndef TODOZSBTREE_ZSBTREE_BUILD_H
#define TODOZSBTREE_ZSBTREE_BUILD_H

#include "zsbtree_LeafBuild.h"
#include "zsbtree_NonLeafBuild.h"


static NonLeaf* build_tree_from_leaf(newVector<LeafKey> &leafKeys, const int n, const int m, int &leafNum) {
  vector<NonLeafKey> nonLeafKeys[2];

  leaf_method::buildtree(leafKeys, nonLeafKeys[0], n, m);
  leafNum = nonLeafKeys[0].size();


  bool isleaf = true;
  int out_1 = 0;
  while (isleaf || nonLeafKeys[out_1].size()>Leaf_maxnum) {
    newVector<NonLeafKey> nonleafKeys_in(nonLeafKeys[out_1]);

    nonleaf_method::buildtree(nonleafKeys_in, nonLeafKeys[1-out_1], isleaf, Leaf_maxnum, Leaf_minnum);
    if (isleaf) isleaf = false;
    nonLeafKeys[out_1].clear();
    out_1 = 1 - out_1;
//        for (int i=0;i<10;i++) {
//            out("l");
//            saxt_print(((Leaf *)(nonleafKeys_in[i].p))->lsaxt);
//            out("r");
//            saxt_print(((Leaf *)(nonleafKeys_in[i].p))->rsaxt);
//        }
  }
  if (nonLeafKeys[out_1].size() == 1) {
    return (NonLeaf*)nonLeafKeys[out_1][0].p;
  } else {
    saxt lsaxt = nonLeafKeys[out_1][0].lsaxt;
    saxt rsaxt = nonLeafKeys[out_1].back().rsaxt;
    cod co_d = get_co_d_from_saxt(lsaxt, rsaxt);
    return new NonLeaf(nonLeafKeys[out_1].size(), co_d, isleaf, lsaxt, rsaxt, nonLeafKeys[out_1].data());
  }
}

static NonLeaf* build_tree_from_nonleaf(newVector<NonLeafKey> &nonLeafKeys) {

  vector<NonLeafKey> nonLeafKeys_rep[2];


  int out_1 = 1;
  bool isleaf = true;
  while (isleaf || nonLeafKeys_rep[out_1].size()>Leaf_maxnum) {
    if (isleaf) {
      nonleaf_method::buildtree(nonLeafKeys, nonLeafKeys_rep[1 - out_1], isleaf, Leaf_maxnum, Leaf_minnum);
      out_1 = 1 - out_1;
      isleaf = false;
    } else {
      newVector<NonLeafKey> nonleafKeys_in(nonLeafKeys_rep[out_1]);
      //创建下一层
      nonleaf_method::buildtree(nonleafKeys_in, nonLeafKeys_rep[1 - out_1], isleaf, Leaf_maxnum, Leaf_minnum);
      nonLeafKeys_rep[out_1].clear();
      out_1 = 1 - out_1;
    }
//        for (int i=0;i<10;i++) {
//            out("l");
//            saxt_print(((Leaf *)(nonleafKeys_in[i].p))->lsaxt);
//            out("r");
//            saxt_print(((Leaf *)(nonleafKeys_in[i].p))->rsaxt);
//        }
  }
  if (nonLeafKeys_rep[out_1].size() == 1) {
    return (NonLeaf*)nonLeafKeys_rep[out_1][0].p;
  } else {
    saxt lsaxt = nonLeafKeys_rep[out_1][0].lsaxt;
    saxt rsaxt = nonLeafKeys_rep[out_1].back().rsaxt;
    cod co_d = get_co_d_from_saxt(lsaxt, rsaxt);
    return new NonLeaf(nonLeafKeys_rep[out_1].size(), co_d, isleaf, lsaxt, rsaxt, nonLeafKeys_rep[out_1].data());
  }
}

static inline int get_1_Num(int x) {
  int res = 0;
  while(x) x -= x & (-x) , res++;
  return res;
}


static int get_drange_rebalance(vector<int> &memNum_period){
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


//省空间模式
class Zsbtree_Build {
 public:

  Zsbtree_Build(int max_size, int min_size);

  void Add(LeafKey& leafkey);

  void Add(NonLeafKey& nonLeafKey, int dep);

  void Emplace(int num, cod co_d, saxt lsaxt, saxt rsaxt, void* leaf, int dep);

  void finish();


  NonLeafKey* GetRootKey();


 protected:
  //new或者存磁盘，但是不移动
  virtual void doleaf(NonLeafKey* nonLeafKey) = 0;
  virtual void dononleaf(NonLeafKey* nonLeafKey, bool isleaf) = 0;
 private:

  inline saxt get_saxt_i(newVector<LeafKey> &leafKeys, int i);
  inline void build_leaf_and_nonleafkey(newVector<LeafKey> &leafKeys, int id,
                                        int num, cod co_d, saxt lsaxt, saxt rsaxt);
  inline void build_leaf_and_nonleafkey_two(newVector<LeafKey> &leafKeys, int id,
                                            int num, cod co_d, saxt lsaxt, saxt rsaxt);
  inline void add_nonleafkey(newVector<LeafKey> &leafKeys, int id,
                             int num, cod co_d, saxt rsaxt);
  inline void split_nonleafkey(newVector<LeafKey> &leafKeys, int id, int allnum,
                               int num, cod co_d, saxt rsaxt);
  inline int getbestmid(newVector<LeafKey> &leafKeys, int id, int num, cod d1, saxt now_saxt, saxt tmplastsaxt);
  int buildtree_window(newVector<LeafKey> &leafKeys);
  void buildtree_window_last(newVector<LeafKey> &leafKeys, int allnum);

  inline saxt get_saxt_i(newVector<NonLeafKey> &leafKeys, int i);
  inline saxt get_saxt_i_r(newVector<NonLeafKey> &leafKeys, int i);
  inline void build_leaf_and_nonleafkey(newVector<NonLeafKey> &leafKeys, int id,
                                        int num, cod co_d, saxt lsaxt, saxt rsaxt, int dep);
  inline void build_leaf_and_nonleafkey_two(newVector<NonLeafKey> &leafKeys, int id,
                                            int num, cod co_d, saxt lsaxt, saxt rsaxt, int dep);
  inline void add_nonleafkey(newVector<NonLeafKey> &leafKeys, int id,
                             int num, cod co_d, saxt rsaxt, int dep);
  inline void split_nonleafkey(newVector<NonLeafKey> &leafKeys, int id, int allnum,
                               int num, cod co_d, saxt rsaxt, int dep);
  inline int getbestmid(newVector<NonLeafKey> &leafKeys, int id, int num, cod d1, saxt now_saxt, saxt tmplastsaxt);
  int buildtree_window(newVector<NonLeafKey> &leafKeys, int dep);
  void buildtree_window_last(newVector<NonLeafKey> &leafKeys, int allnum, int dep);


  typedef struct {
    cod co_d;
    int id;
    int num;
    saxt lsaxt;
    saxt rsaxt;
  } method2_node;

  int n;
  int m;
  newVector<LeafKey> leafkeys;
  vector<newVector<NonLeafKey> > nonleafkeys;

  vector<LeafKey> leafkeys_rep;
  vector<vector<NonLeafKey> > nonleafkeys_rep;

};


class Zsbtree_Build_Mem : public Zsbtree_Build{
  void doleaf(NonLeafKey* nonLeafKey) override ;
  void dononleaf(NonLeafKey* nonLeafKey, bool isleaf) override;

 public:
  Zsbtree_Build_Mem(int max_size, int min_size);
};







#endif //TODOZSBTREE_ZSBTREE_BUILD_H
