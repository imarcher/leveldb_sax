//
// Created by hh on 2022/11/21.
//

#ifndef TODOZSBTREE_ZSBTREE_LEAFBUILD_H
#define TODOZSBTREE_ZSBTREE_LEAFBUILD_H


#include <globals.h>
#include "util/arena.h"
#include "util/newVector.h"
#include "vector"
#include "sax.h"
#include "Leaf.h"
#include "NonLeaf.h"
#include "../util/arena.h"

using namespace std;

namespace leaf_method {
    //method2使用的临时数据结构
    typedef struct {
        cod co_d;
        int id;
        int num;
        saxt lsaxt;
        saxt rsaxt;
    } method2_node;


    inline saxt get_saxt_i(newVector<LeafKey> &leafKeys, int i){
        return leafKeys[i].asaxt;
    }

//构造leaf和索引点
    inline void build_leaf_and_nonleafkey(newVector<LeafKey> &leafKeys, vector<NonLeafKey> &nonLeafKeys, int id,
                                          int num, cod co_d, saxt lsaxt, saxt rsaxt) {
        //构造leaf
        Leaf *leaf = new Leaf(num, co_d, lsaxt, rsaxt, leafKeys.data()+id);
        //构造nonleaf索引点
        nonLeafKeys.emplace_back(num, co_d, lsaxt, rsaxt, leaf);
    }

//构造leaf和索引点,从中间平分
    inline void build_leaf_and_nonleafkey_two(newVector<LeafKey> &leafKeys, vector<NonLeafKey> &nonLeafKeys, int id,
                                              int num, cod co_d, saxt lsaxt, saxt rsaxt) {
        int tmpnum1 = num / 2;
        int tmpnum2 = num - tmpnum1;
        //构造leaf
        saxt tmpsaxt = leafKeys[id+tmpnum1-1].asaxt;
        Leaf *leaf = new Leaf(tmpnum1, get_co_d_from_saxt(lsaxt, tmpsaxt, co_d), lsaxt, tmpsaxt, leafKeys.data()+id);
        //构造nonleaf索引点
        nonLeafKeys.emplace_back(tmpnum1, leaf->co_d, lsaxt, tmpsaxt, leaf);
        //第二个叶
        //构造leaf
        tmpsaxt = leafKeys[id+tmpnum1].asaxt;
        leaf = new Leaf(tmpnum2, get_co_d_from_saxt(tmpsaxt, rsaxt, co_d), tmpsaxt, rsaxt, leafKeys.data()+id+tmpnum1);
        //构造nonleaf索引点
        nonLeafKeys.emplace_back(tmpnum2, leaf->co_d, tmpsaxt, rsaxt, leaf);
    }

//给一个叶子结点加一些key
    inline void add_nonleafkey(newVector<LeafKey> &leafKeys, vector<NonLeafKey> &nonLeafKeys, int id,
                               int num, cod co_d, saxt rsaxt) {
        NonLeafKey *nonLeafKey = nonLeafKeys.data()+nonLeafKeys.size()-1;
        Leaf *leaf = (Leaf *)(nonLeafKey->p);
        leaf->co_d = co_d;
        leaf->add(leafKeys.data()+id, num);
        leaf->setRsaxt(rsaxt);
        nonLeafKey->co_d = co_d;
        nonLeafKey->num = leaf->num;
        nonLeafKey->setRsaxt(rsaxt);
    }

//给一个叶子结点加一些key,到大于n了，平分
    inline void split_nonleafkey(newVector<LeafKey> &leafKeys, vector<NonLeafKey> &nonLeafKeys, int id, int allnum,
                                 int num, cod co_d, saxt rsaxt) {

        NonLeafKey *nonLeafKey = nonLeafKeys.data()+nonLeafKeys.size()-1;
        Leaf *leaf = (Leaf *)(nonLeafKey->p);
        int tmpnum1 = allnum / 2;
        int tmpnum2 = allnum - tmpnum1;
        //添加后一个点
        int tmpnum3 = tmpnum2 - num;
        saxt newLsaxt = leaf->leafKeys[tmpnum1].asaxt;
        Leaf *newLeaf = new Leaf(tmpnum3, get_co_d_from_saxt(newLsaxt, rsaxt, co_d), newLsaxt, rsaxt, leaf->leafKeys+tmpnum1);
        newLeaf->add(leafKeys.data()+id, num);
        nonLeafKeys.emplace_back(tmpnum2, newLeaf->co_d, newLsaxt, rsaxt, newLeaf);
        //更新前一个点
        leaf->num = tmpnum1;
        leaf->setRsaxt(leaf->leafKeys[tmpnum1-1].asaxt);
        leaf->co_d = get_co_d_from_saxt(leaf->lsaxt, leaf->rsaxt, leaf->co_d);
        nonLeafKey->num = tmpnum1;
        nonLeafKey->co_d = leaf->co_d;
        nonLeafKey->setRsaxt(leaf->rsaxt);
    }

//在method2中，遇到大于n的，分一下
    inline int getbestmid(newVector<LeafKey> &leafKeys, const int n, const int m, int id, int num, cod d1, saxt now_saxt, saxt tmplastsaxt) {
        int best_mid_id = id+num/2-1;
        int best_cod = d1+d1;
        for (int mid_id = id+m-1;mid_id<id+num-m;mid_id++){
            saxt tmprsaxt = get_saxt_i(leafKeys, mid_id);
            cod tmpd1 = get_co_d_from_saxt(now_saxt, tmprsaxt, d1);
            saxt tmplsaxt = get_saxt_i(leafKeys, mid_id+1);
            cod tmpd2 = get_co_d_from_saxt(tmplsaxt, tmplastsaxt, d1);
            int tmpd = tmpd2+tmpd1;
            if (tmpd>best_cod) {
                best_mid_id = mid_id;
                best_cod = tmpd;
            }
        }
        return best_mid_id;
    }


//待考虑几个平分时分节点有很多d=8的情况
//批量构建while循环内, 2n个
    int buildtree_window(newVector<LeafKey> &leafKeys, vector<NonLeafKey> &nonLeafKeys, const int n, const int m) {
        int end = 2 * n - 1;
        saxt first_saxt = get_saxt_i(leafKeys, 0);
        saxt last_saxt = get_saxt_i(leafKeys, end);
        cod window_co_d = get_co_d_from_saxt(first_saxt, last_saxt);
        saxt_prefix window_co_prefix = first_saxt;
        //method2 begin
        cod d1 = window_co_d + 1;
        //先从2n-1往前看,删掉后面的,最多删掉n个
        int new_end = end;
        saxt new_last_saxt = last_saxt;
        saxt pre_new_last_saxt = get_saxt_i(leafKeys, new_end-1);
        while(compare_saxt_d(new_last_saxt, pre_new_last_saxt, d1) && new_end > n){
            new_end--;
            new_last_saxt = pre_new_last_saxt;
            pre_new_last_saxt = get_saxt_i(leafKeys, new_end-1);;
        }
        //从前往后看
        std::vector<method2_node> d1Arr;
        int num = 1;
        int id = 0;
        saxt now_saxt = first_saxt;
        //跟第n/2个比
        saxt now1_saxt = get_saxt_i(leafKeys, m-1);
        bool mark = true;

        for(int i=0;; i++) {
            if (mark) {
                if (i>=new_end-m+1) break;
                if (compare_saxt_d(now_saxt, now1_saxt, d1)) {
                    num = m;
                    id = i;
                    mark = false;
                    now1_saxt = get_saxt_i(leafKeys, i+m);
                    i = i + m - 1;
                } else {
                    now_saxt = get_saxt_i(leafKeys, i+1);
                    now1_saxt = get_saxt_i(leafKeys, i+m);
                }
            } else {
                if (i>=new_end) {
                    if (num <= n) {
                        saxt tmpsaxt = get_saxt_i(leafKeys, i-1);
                        cod tmpd = get_co_d_from_saxt(now_saxt, tmpsaxt, d1);
                        d1Arr.push_back({tmpd, id, num, now_saxt, tmpsaxt});
                    } else {
                        //要找d+2,但这里先直接平分,其实可以从m开始一个一个找最小的区域
                        //平分代码
                        int tmpnum1 = num / 2;
                        int tmpnum2 = num - tmpnum1;
                        int id1 = id + tmpnum1;
                        saxt tmprsaxt = get_saxt_i(leafKeys, id1-1);
                        cod tmpd = get_co_d_from_saxt(now_saxt, tmprsaxt, d1);
                        d1Arr.push_back({tmpd, id, tmpnum1, now_saxt, tmprsaxt});
                        saxt tmplsaxt = get_saxt_i(leafKeys, id1);
                        tmprsaxt = get_saxt_i(leafKeys, i-1);
                        tmpd = get_co_d_from_saxt(tmplsaxt, tmprsaxt, d1);
                        d1Arr.push_back({tmpd, id1, tmpnum2, tmplsaxt, tmprsaxt});
                        //找d+2代码，遍历中间每一个，寻找d最大的方案，可能有3个就再分
                        //可能的区间 1000000才触发18次，太少了，不如平分
//                    saxt tmplastsaxt = get_saxt_i(leafKeys, i-1);
//                    int best_mid_id = getbestmid(leafKeys, n, m, id, num, d1, now_saxt, tmplastsaxt);
//                    int tmpnum1 = best_mid_id-id+1;
//                    int tmpnum2 = num-(best_mid_id-id+1);
//                    saxt tmprsaxt = get_saxt_i(leafKeys, best_mid_id);
//                    cod tmpd1 = get_co_d_from_saxt(now_saxt, tmprsaxt, d1);
//                    if (tmpnum1<=n) {
//                        d1Arr.push_back({tmpd1, id, tmpnum1, now_saxt, tmprsaxt});
//                    } else {
//                        //再分
//                        int best_mid_id_ = getbestmid(leafKeys, n, m, id, tmpnum1, tmpd1, now_saxt, tmprsaxt);
//                        int tmpnum1_ = best_mid_id_-id+1;
//                        int tmpnum2_ = tmpnum1-tmpnum1_;
//                        saxt tmprsaxt_ = get_saxt_i(leafKeys, best_mid_id_);
//                        cod tmpd1_ = get_co_d_from_saxt(now_saxt, tmprsaxt_, tmpd1);
//                        d1Arr.push_back({tmpd1_, id, tmpnum1_, now_saxt, tmprsaxt_});
//                        saxt tmplsaxt_ = get_saxt_i(leafKeys, best_mid_id_+1);
//                        cod tmpd2_ = get_co_d_from_saxt(tmplsaxt_, tmprsaxt, tmpd1);
//                        d1Arr.push_back({tmpd2_, best_mid_id_+1, tmpnum2_, tmplsaxt_, tmprsaxt});
//                    }
//                    saxt tmplsaxt = get_saxt_i(leafKeys, best_mid_id+1);
//                    cod tmpd2 = get_co_d_from_saxt(tmplsaxt, tmplastsaxt, d1);
//                    if (tmpnum2<=n) {
//                        d1Arr.push_back({tmpd2, best_mid_id+1, tmpnum2, tmplsaxt, tmplastsaxt});
//                    } else {
//                        //再分
//                        int best_mid_id_ = getbestmid(leafKeys, n, m, best_mid_id+1, tmpnum2, tmpd2, tmplsaxt, tmplastsaxt);
//                        int tmpnum1_ = best_mid_id_-best_mid_id;
//                        int tmpnum2_ = tmpnum2-tmpnum1_;
//                        saxt tmprsaxt_ = get_saxt_i(leafKeys, best_mid_id_);
//                        cod tmpd1_ = get_co_d_from_saxt(tmplsaxt, tmprsaxt_, tmpd2);
//                        d1Arr.push_back({tmpd1_, best_mid_id+1, tmpnum1_, tmplsaxt, tmprsaxt_});
//                        saxt tmplsaxt_ = get_saxt_i(leafKeys, best_mid_id_+1);
//                        cod tmpd2_ = get_co_d_from_saxt(tmplsaxt_, tmplastsaxt, tmpd2);
//                        d1Arr.push_back({tmpd2_, best_mid_id_+1, tmpnum2_, tmplsaxt_, tmplastsaxt});
//                    }
                    }
                    break;
                }
                if (compare_saxt_d(now_saxt, now1_saxt, d1)) {
                    num++;
                    now1_saxt = get_saxt_i(leafKeys, i+1);
                } else {
                    if (num <= n) {
                        saxt tmpsaxt = get_saxt_i(leafKeys, i-1);
                        cod tmpd = get_co_d_from_saxt(now_saxt, tmpsaxt, d1);
                        d1Arr.push_back({tmpd, id, num, now_saxt, tmpsaxt});
                    } else {
                        //要找d+2,但这里先直接平分,其实可以从m开始一个一个找最小的区域
                        //平分代码
                        int tmpnum1 = num / 2;
                        int tmpnum2 = num - tmpnum1;
                        int id1 = id + tmpnum1;
                        saxt tmprsaxt = get_saxt_i(leafKeys, id1-1);
                        cod tmpd = get_co_d_from_saxt(now_saxt, tmprsaxt, d1);
                        d1Arr.push_back({tmpd, id, tmpnum1, now_saxt, tmprsaxt});
                        saxt tmplsaxt = get_saxt_i(leafKeys, id1);
                        tmprsaxt = get_saxt_i(leafKeys, i-1);
                        tmpd = get_co_d_from_saxt(tmplsaxt, tmprsaxt, d1);
                        d1Arr.push_back({tmpd, id1, tmpnum2, tmplsaxt, tmprsaxt});
                        //找d+2代码，遍历中间每一个，寻找d最大的方案
                        //可能的区间
//                    saxt tmplastsaxt = get_saxt_i(leafKeys, i-1);
//                    int best_mid_id = getbestmid(leafKeys, n, m, id, num, d1, now_saxt, tmplastsaxt);
//                    int tmpnum1 = best_mid_id-id+1;
//                    int tmpnum2 = num-(best_mid_id-id+1);
//                    saxt tmprsaxt = get_saxt_i(leafKeys, best_mid_id);
//                    cod tmpd1 = get_co_d_from_saxt(now_saxt, tmprsaxt, d1);
//                    if (tmpnum1<=n) {
//                        d1Arr.push_back({tmpd1, id, tmpnum1, now_saxt, tmprsaxt});
//                    } else {
//                        //再分
//                        int best_mid_id_ = getbestmid(leafKeys, n, m, id, tmpnum1, tmpd1, now_saxt, tmprsaxt);
//                        int tmpnum1_ = best_mid_id_-id+1;
//                        int tmpnum2_ = tmpnum1-tmpnum1_;
//                        saxt tmprsaxt_ = get_saxt_i(leafKeys, best_mid_id_);
//                        cod tmpd1_ = get_co_d_from_saxt(now_saxt, tmprsaxt_, tmpd1);
//                        d1Arr.push_back({tmpd1_, id, tmpnum1_, now_saxt, tmprsaxt_});
//                        saxt tmplsaxt_ = get_saxt_i(leafKeys, best_mid_id_+1);
//                        cod tmpd2_ = get_co_d_from_saxt(tmplsaxt_, tmprsaxt, tmpd1);
//                        d1Arr.push_back({tmpd2_, best_mid_id_+1, tmpnum2_, tmplsaxt_, tmprsaxt});
//                    }
//                    saxt tmplsaxt = get_saxt_i(leafKeys, best_mid_id+1);
//                    cod tmpd2 = get_co_d_from_saxt(tmplsaxt, tmplastsaxt, d1);
//                    if (tmpnum2<=n) {
//                        d1Arr.push_back({tmpd2, best_mid_id+1, tmpnum2, tmplsaxt, tmplastsaxt});
//                    } else {
//                        //再分
//                        int best_mid_id_ = getbestmid(leafKeys, n, m, best_mid_id+1, tmpnum2, tmpd2, tmplsaxt, tmplastsaxt);
//                        int tmpnum1_ = best_mid_id_-best_mid_id;
//                        int tmpnum2_ = tmpnum2-tmpnum1_;
//                        saxt tmprsaxt_ = get_saxt_i(leafKeys, best_mid_id_);
//                        cod tmpd1_ = get_co_d_from_saxt(tmplsaxt, tmprsaxt_, tmpd2);
//                        d1Arr.push_back({tmpd1_, best_mid_id+1, tmpnum1_, tmplsaxt, tmprsaxt_});
//                        saxt tmplsaxt_ = get_saxt_i(leafKeys, best_mid_id_+1);
//                        cod tmpd2_ = get_co_d_from_saxt(tmplsaxt_, tmplastsaxt, tmpd2);
//                        d1Arr.push_back({tmpd2_, best_mid_id_+1, tmpnum2_, tmplsaxt_, tmplastsaxt});
//                    }
                    }
                    mark = true;
                    if (i>=new_end-m+1) break;
                    i--;
                    now_saxt = now1_saxt;
                    now1_saxt = get_saxt_i(leafKeys, i+m);
                }
            }
        }
        //method2 end
        //构建叶子结点和非叶子的索引点
        int todoid = 0;
        for(method2_node anode: d1Arr) {
            int todonum = anode.id - todoid;
            if (todonum == 0) {
//            out("laile");
                build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, todoid, anode.num, anode.co_d, anode.lsaxt, anode.rsaxt);
//            out("laile2");
                todoid += anode.num;
                continue;
            }
            //前面小范围的
            saxt re_lsaxt = get_saxt_i(leafKeys, todoid);
            saxt re_rsaxt = get_saxt_i(leafKeys, anode.id-1);
            if (todonum <= m) {
                if (nonLeafKeys.empty()){
                    //只看后面
                    //如果后面是d+1且加起来<=n，则合起来，不然不要了。这里直接合
                    int tmpnum = todonum + anode.num;
                    if (tmpnum <= n){
                        build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, todoid, tmpnum, window_co_d, re_lsaxt, anode.rsaxt);
                        todoid += tmpnum;
                        continue;
                    } else {
                        //构造leaf
                        build_leaf_and_nonleafkey_two(leafKeys, nonLeafKeys, todoid, tmpnum, window_co_d, re_lsaxt, anode.rsaxt);
                        todoid += tmpnum;
                        continue;
                    }
                } else {
                    //要看前面
                    cod preco_d = nonLeafKeys.back().co_d;
                    saxt prelsaxt = nonLeafKeys.back().lsaxt;
                    int prenum = nonLeafKeys.back().num;

                    cod nextco_d = anode.co_d;
                    saxt nextrsaxt = anode.rsaxt;
                    int nextnum = anode.num;
                    //先对比下降程度
                    cod co_d1;
                    if (todoid == 0) co_d1 = get_co_d_from_saxt(prelsaxt, re_rsaxt);
                    else co_d1 = get_co_d_from_saxt(prelsaxt, re_rsaxt, window_co_d);
                    cod co_d2 = get_co_d_from_saxt(re_lsaxt, anode.rsaxt, window_co_d);
                    if ((preco_d - co_d1) < (nextco_d - co_d2)) {
                        //跟前面合
                        int tmpnum = prenum + todonum;
                        if (tmpnum <= n) {
                            add_nonleafkey(leafKeys, nonLeafKeys, todoid, todonum, co_d1, re_rsaxt);
                        } else {
                            //平分
                            split_nonleafkey(leafKeys, nonLeafKeys, todoid, tmpnum, todonum, co_d1, re_rsaxt);
                        }
                        build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, anode.id, anode.num, anode.co_d, anode.lsaxt, anode.rsaxt);
                        todoid = todoid + todonum + anode.num;
                        continue;
                    } else if ((preco_d - co_d1) > (nextco_d - co_d2)) {
                        //跟后面合
                        int tmpnum = nextnum + todonum;
                        if (tmpnum <= n) {
                            build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, todoid, tmpnum, co_d2, re_lsaxt, anode.rsaxt);
                            todoid += tmpnum;
                            continue;
                        } else {
                            //平分
                            build_leaf_and_nonleafkey_two(leafKeys, nonLeafKeys, todoid, tmpnum, co_d2, re_lsaxt, anode.rsaxt);
                            todoid += tmpnum;
                            continue;
                        }
                    } else {
                        //再比大小
                        int tmpnum1 = prenum + todonum;
                        int tmpnum2 = nextnum + todonum;
                        if (tmpnum1 <= n && tmpnum2 > n) {
                            add_nonleafkey(leafKeys, nonLeafKeys, todoid, todonum, co_d1, re_rsaxt);
                            build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, anode.id, anode.num, anode.co_d, anode.lsaxt, anode.rsaxt);
                            todoid = todoid + todonum + anode.num;
                            continue;
                        } else if (tmpnum1 > n && tmpnum2 <= n) {
                            build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, todoid, tmpnum2, co_d2, re_lsaxt, anode.rsaxt);
                            todoid += tmpnum2;
                            continue;
                        } else if (tmpnum1 >n && tmpnum2 > n){
                            // 比相聚度,跟大的合
                            if (co_d2 >= co_d1) {
                                build_leaf_and_nonleafkey_two(leafKeys, nonLeafKeys, todoid, tmpnum2, co_d2, re_lsaxt, anode.rsaxt);
                                todoid += tmpnum2;
                                continue;
                            } else {
                                split_nonleafkey(leafKeys, nonLeafKeys, todoid, tmpnum1, todonum, co_d1, re_rsaxt);
                                build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, anode.id, anode.num, anode.co_d, anode.lsaxt, anode.rsaxt);
                                todoid += tmpnum2;
                                continue;
                            }
                        } else {
                            // 比相聚度
                            if (co_d2 >= co_d1) {
                                build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, todoid, tmpnum2, co_d2, re_lsaxt, anode.rsaxt);
                                todoid += tmpnum2;
                                continue;
                            } else {
                                add_nonleafkey(leafKeys, nonLeafKeys, todoid, todonum, co_d1, re_rsaxt);
                                build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, anode.id, anode.num, anode.co_d, anode.lsaxt, anode.rsaxt);
                                todoid += tmpnum2;
                                continue;
                            }
                        }
                    }
                }
            } else if (todonum >m && todonum <= n) {
                //其实就是window_co_d
                build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, todoid, todonum, get_co_d_from_saxt(re_lsaxt, re_rsaxt, window_co_d), re_lsaxt, re_rsaxt);
                build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, anode.id, anode.num, anode.co_d, anode.lsaxt, anode.rsaxt);
                todoid = todoid + todonum + anode.num;
                continue;
            } else {
                build_leaf_and_nonleafkey_two(leafKeys, nonLeafKeys, todoid, todonum, window_co_d, re_lsaxt, re_rsaxt);
                build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, anode.id, anode.num, anode.co_d, anode.lsaxt, anode.rsaxt);
                todoid = todoid + todonum + anode.num;
                continue;
            }
        }
        if (todoid == 0) {
            //打包n个
            build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, todoid, n, window_co_d, first_saxt, get_saxt_i(leafKeys, n-1));
            todoid += n;
        }
        return todoid;
    }


    void buildtree_window_last(newVector<LeafKey> &leafKeys, vector<NonLeafKey> &nonLeafKeys, int allnum, const int n, const int m) {
        int end = allnum - 1;
        saxt first_saxt = get_saxt_i(leafKeys, 0);
        saxt last_saxt = get_saxt_i(leafKeys, end);
        cod window_co_d = get_co_d_from_saxt(first_saxt, last_saxt);
        saxt_prefix window_co_prefix = first_saxt;
        //method2 begin
        cod d1 = window_co_d + 1;
        //先从2n-1往前看,删掉后面的,最多删掉n个
        int new_end = end;
        //从前往后看
        std::vector<method2_node> d1Arr;
        int num = 1;
        int id = 0;
        saxt now_saxt = first_saxt;
        //跟第n/2个比
        saxt now1_saxt = get_saxt_i(leafKeys, m - 1);
        bool mark = true;

        for (int i = 0;; i++) {
            if (mark) {
                if (i >= new_end - m + 1) break;
                if (compare_saxt_d(now_saxt, now1_saxt, d1)) {
                    num = m;
                    id = i;
                    mark = false;
                    now1_saxt = get_saxt_i(leafKeys, i + m);
                    i = i + m - 1;
                } else {
                    now_saxt = get_saxt_i(leafKeys, i + 1);
                    now1_saxt = get_saxt_i(leafKeys, i + m);
                }
            } else {
                if (i >= new_end) {
                    if (num <= n) {
                        saxt tmpsaxt = get_saxt_i(leafKeys, i - 1);
                        cod tmpd = get_co_d_from_saxt(now_saxt, tmpsaxt, d1);
                        d1Arr.push_back({tmpd, id, num, now_saxt, tmpsaxt});
                    } else {
                        //要找d+2,但这里先直接平分,其实可以从m开始一个一个找最小的区域
                        //平分代码
                        int tmpnum1 = num / 2;
                        int tmpnum2 = num - tmpnum1;
                        int id1 = id + tmpnum1;
                        saxt tmprsaxt = get_saxt_i(leafKeys, id1 - 1);
                        cod tmpd = get_co_d_from_saxt(now_saxt, tmprsaxt, d1);
                        d1Arr.push_back({tmpd, id, tmpnum1, now_saxt, tmprsaxt});
                        saxt tmplsaxt = get_saxt_i(leafKeys, id1);
                        tmprsaxt = get_saxt_i(leafKeys, i - 1);
                        tmpd = get_co_d_from_saxt(tmplsaxt, tmprsaxt, d1);
                        d1Arr.push_back({tmpd, id1, tmpnum2, tmplsaxt, tmprsaxt});
                        //找d+2代码，遍历中间每一个，寻找d最大的方案，可能有3个就再分
                        //可能的区间 1000000才触发18次，太少了，不如平分
//                    saxt tmplastsaxt = get_saxt_i(leafKeys, i-1);
//                    int best_mid_id = getbestmid(leafKeys, n, m, id, num, d1, now_saxt, tmplastsaxt);
//                    int tmpnum1 = best_mid_id-id+1;
//                    int tmpnum2 = num-(best_mid_id-id+1);
//                    saxt tmprsaxt = get_saxt_i(leafKeys, best_mid_id);
//                    cod tmpd1 = get_co_d_from_saxt(now_saxt, tmprsaxt, d1);
//                    if (tmpnum1<=n) {
//                        d1Arr.push_back({tmpd1, id, tmpnum1, now_saxt, tmprsaxt});
//                    } else {
//                        //再分
//                        int best_mid_id_ = getbestmid(leafKeys, n, m, id, tmpnum1, tmpd1, now_saxt, tmprsaxt);
//                        int tmpnum1_ = best_mid_id_-id+1;
//                        int tmpnum2_ = tmpnum1-tmpnum1_;
//                        saxt tmprsaxt_ = get_saxt_i(leafKeys, best_mid_id_);
//                        cod tmpd1_ = get_co_d_from_saxt(now_saxt, tmprsaxt_, tmpd1);
//                        d1Arr.push_back({tmpd1_, id, tmpnum1_, now_saxt, tmprsaxt_});
//                        saxt tmplsaxt_ = get_saxt_i(leafKeys, best_mid_id_+1);
//                        cod tmpd2_ = get_co_d_from_saxt(tmplsaxt_, tmprsaxt, tmpd1);
//                        d1Arr.push_back({tmpd2_, best_mid_id_+1, tmpnum2_, tmplsaxt_, tmprsaxt});
//                    }
//                    saxt tmplsaxt = get_saxt_i(leafKeys, best_mid_id+1);
//                    cod tmpd2 = get_co_d_from_saxt(tmplsaxt, tmplastsaxt, d1);
//                    if (tmpnum2<=n) {
//                        d1Arr.push_back({tmpd2, best_mid_id+1, tmpnum2, tmplsaxt, tmplastsaxt});
//                    } else {
//                        //再分
//                        int best_mid_id_ = getbestmid(leafKeys, n, m, best_mid_id+1, tmpnum2, tmpd2, tmplsaxt, tmplastsaxt);
//                        int tmpnum1_ = best_mid_id_-best_mid_id;
//                        int tmpnum2_ = tmpnum2-tmpnum1_;
//                        saxt tmprsaxt_ = get_saxt_i(leafKeys, best_mid_id_);
//                        cod tmpd1_ = get_co_d_from_saxt(tmplsaxt, tmprsaxt_, tmpd2);
//                        d1Arr.push_back({tmpd1_, best_mid_id+1, tmpnum1_, tmplsaxt, tmprsaxt_});
//                        saxt tmplsaxt_ = get_saxt_i(leafKeys, best_mid_id_+1);
//                        cod tmpd2_ = get_co_d_from_saxt(tmplsaxt_, tmplastsaxt, tmpd2);
//                        d1Arr.push_back({tmpd2_, best_mid_id_+1, tmpnum2_, tmplsaxt_, tmplastsaxt});
//                    }
                    }
                    break;
                }
                if (compare_saxt_d(now_saxt, now1_saxt, d1)) {
                    num++;
                    now1_saxt = get_saxt_i(leafKeys, i + 1);
                } else {
                    if (num <= n) {
                        saxt tmpsaxt = get_saxt_i(leafKeys, i - 1);
                        cod tmpd = get_co_d_from_saxt(now_saxt, tmpsaxt, d1);
                        d1Arr.push_back({tmpd, id, num, now_saxt, tmpsaxt});
                    } else {
                        //要找d+2,但这里先直接平分,其实可以从m开始一个一个找最小的区域
                        //平分代码
                        int tmpnum1 = num / 2;
                        int tmpnum2 = num - tmpnum1;
                        int id1 = id + tmpnum1;
                        saxt tmprsaxt = get_saxt_i(leafKeys, id1 - 1);
                        cod tmpd = get_co_d_from_saxt(now_saxt, tmprsaxt, d1);
                        d1Arr.push_back({tmpd, id, tmpnum1, now_saxt, tmprsaxt});
                        saxt tmplsaxt = get_saxt_i(leafKeys, id1);
                        tmprsaxt = get_saxt_i(leafKeys, i - 1);
                        tmpd = get_co_d_from_saxt(tmplsaxt, tmprsaxt, d1);
                        d1Arr.push_back({tmpd, id1, tmpnum2, tmplsaxt, tmprsaxt});
                        //找d+2代码，遍历中间每一个，寻找d最大的方案
                        //可能的区间
//                    saxt tmplastsaxt = get_saxt_i(leafKeys, i-1);
//                    int best_mid_id = getbestmid(leafKeys, n, m, id, num, d1, now_saxt, tmplastsaxt);
//                    int tmpnum1 = best_mid_id-id+1;
//                    int tmpnum2 = num-(best_mid_id-id+1);
//                    saxt tmprsaxt = get_saxt_i(leafKeys, best_mid_id);
//                    cod tmpd1 = get_co_d_from_saxt(now_saxt, tmprsaxt, d1);
//                    if (tmpnum1<=n) {
//                        d1Arr.push_back({tmpd1, id, tmpnum1, now_saxt, tmprsaxt});
//                    } else {
//                        //再分
//                        int best_mid_id_ = getbestmid(leafKeys, n, m, id, tmpnum1, tmpd1, now_saxt, tmprsaxt);
//                        int tmpnum1_ = best_mid_id_-id+1;
//                        int tmpnum2_ = tmpnum1-tmpnum1_;
//                        saxt tmprsaxt_ = get_saxt_i(leafKeys, best_mid_id_);
//                        cod tmpd1_ = get_co_d_from_saxt(now_saxt, tmprsaxt_, tmpd1);
//                        d1Arr.push_back({tmpd1_, id, tmpnum1_, now_saxt, tmprsaxt_});
//                        saxt tmplsaxt_ = get_saxt_i(leafKeys, best_mid_id_+1);
//                        cod tmpd2_ = get_co_d_from_saxt(tmplsaxt_, tmprsaxt, tmpd1);
//                        d1Arr.push_back({tmpd2_, best_mid_id_+1, tmpnum2_, tmplsaxt_, tmprsaxt});
//                    }
//                    saxt tmplsaxt = get_saxt_i(leafKeys, best_mid_id+1);
//                    cod tmpd2 = get_co_d_from_saxt(tmplsaxt, tmplastsaxt, d1);
//                    if (tmpnum2<=n) {
//                        d1Arr.push_back({tmpd2, best_mid_id+1, tmpnum2, tmplsaxt, tmplastsaxt});
//                    } else {
//                        //再分
////                        out("num");
////                        out(best_mid_id+1);
////                        out(tmpnum2);
//                        int best_mid_id_ = getbestmid(leafKeys, n, m, best_mid_id+1, tmpnum2, tmpd2, tmplsaxt, tmplastsaxt);
////                        out(best_mid_id_);
//                        int tmpnum1_ = best_mid_id_-best_mid_id;
//                        int tmpnum2_ = tmpnum2-tmpnum1_;
////                        out(tmpnum2_);
//                        saxt tmprsaxt_ = get_saxt_i(leafKeys, best_mid_id_);
//                        cod tmpd1_ = get_co_d_from_saxt(tmplsaxt, tmprsaxt_, tmpd2);
//                        d1Arr.push_back({tmpd1_, best_mid_id+1, tmpnum1_, tmplsaxt, tmprsaxt_});
//                        saxt tmplsaxt_ = get_saxt_i(leafKeys, best_mid_id_+1);
//                        cod tmpd2_ = get_co_d_from_saxt(tmplsaxt_, tmplastsaxt, tmpd2);
//                        d1Arr.push_back({tmpd2_, best_mid_id_+1, tmpnum2_, tmplsaxt_, tmplastsaxt});
//                    }
                    }
                    mark = true;
                    if (i >= new_end - m + 1) break;
                    i--;
                    now_saxt = now1_saxt;
                    now1_saxt = get_saxt_i(leafKeys, i + m);
                }
            }
        }
        //method2 end
        //构建叶子结点和非叶子的索引点
        int todoid = 0;
        for (method2_node anode: d1Arr) {
            int todonum = anode.id - todoid;
            if (todonum == 0) {
//            out("laile");
                build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, todoid, anode.num, anode.co_d, anode.lsaxt, anode.rsaxt);
//            out("laile2");
                todoid += anode.num;
                continue;
            }
            //前面小范围的
            saxt re_lsaxt = get_saxt_i(leafKeys, todoid);
            saxt re_rsaxt = get_saxt_i(leafKeys, anode.id - 1);
            if (todonum <= m) {
                if (nonLeafKeys.empty()) {
                    //只看后面
                    //如果后面是d+1且加起来<=n，则合起来，不然不要了。这里直接合
                    int tmpnum = todonum + anode.num;
                    if (tmpnum <= n) {
                        build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, todoid, tmpnum, window_co_d, re_lsaxt,
                                                  anode.rsaxt);
                        todoid += tmpnum;
                        continue;
                    } else {
                        //构造leaf
                        build_leaf_and_nonleafkey_two(leafKeys, nonLeafKeys, todoid, tmpnum, window_co_d, re_lsaxt,
                                                      anode.rsaxt);
                        todoid += tmpnum;
                        continue;
                    }
                } else {
//                out("Jinru:");
                    //要看前面
                    cod preco_d = nonLeafKeys.back().co_d;
                    saxt prelsaxt = nonLeafKeys.back().lsaxt;
                    int prenum = nonLeafKeys.back().num;

                    cod nextco_d = anode.co_d;
                    saxt nextrsaxt = anode.rsaxt;
                    int nextnum = anode.num;
                    //先对比下降程度
                    cod co_d1;
                    if (todoid == 0) co_d1 = get_co_d_from_saxt(prelsaxt, re_rsaxt);
                    else co_d1 = get_co_d_from_saxt(prelsaxt, re_rsaxt, window_co_d);
                    cod co_d2 = get_co_d_from_saxt(re_lsaxt, anode.rsaxt, window_co_d);

//                out((int)preco_d);
//                out((int)co_d1);
//                out((int)nextco_d);
//                out((int)co_d2);

                    if ((preco_d - co_d1) < (nextco_d - co_d2)) {
//                    out(prenum);
//                    out(todonum);
                        //跟前面合
                        int tmpnum = prenum + todonum;
                        if (tmpnum <= n) {
//                        out("laile");
                            add_nonleafkey(leafKeys, nonLeafKeys, todoid, todonum, co_d1, re_rsaxt);
                        } else {
                            //平分
                            split_nonleafkey(leafKeys, nonLeafKeys, todoid, tmpnum, todonum, co_d1, re_rsaxt);
                        }
                        build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, anode.id, anode.num, anode.co_d, anode.lsaxt,
                                                  anode.rsaxt);
                        todoid = todoid + todonum + anode.num;
                        continue;
                    } else if ((preco_d - co_d1) > (nextco_d - co_d2)) {
                        //跟后面合
                        int tmpnum = nextnum + todonum;
                        if (tmpnum <= n) {
                            build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, todoid, tmpnum, co_d2, re_lsaxt, anode.rsaxt);
                            todoid += tmpnum;
                            continue;
                        } else {
                            //平分
                            build_leaf_and_nonleafkey_two(leafKeys, nonLeafKeys, todoid, tmpnum, co_d2, re_lsaxt,
                                                          anode.rsaxt);
                            todoid += tmpnum;
                            continue;
                        }
                    } else {
                        //再比大小
                        int tmpnum1 = prenum + todonum;
                        int tmpnum2 = nextnum + todonum;
                        if (tmpnum1 <= n && tmpnum2 > n) {
                            add_nonleafkey(leafKeys, nonLeafKeys, todoid, todonum, co_d1, re_rsaxt);
                            build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, anode.id, anode.num, anode.co_d, anode.lsaxt,
                                                      anode.rsaxt);
                            todoid = todoid + todonum + anode.num;
                            continue;
                        } else if (tmpnum1 > n && tmpnum2 <= n) {
                            build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, todoid, tmpnum2, co_d2, re_lsaxt, anode.rsaxt);
                            todoid += tmpnum2;
                            continue;
                        } else if (tmpnum1 > n && tmpnum2 > n) {
                            // 比相聚度,跟大的合
                            if (co_d2 >= co_d1) {
                                build_leaf_and_nonleafkey_two(leafKeys, nonLeafKeys, todoid, tmpnum2, co_d2, re_lsaxt,
                                                              anode.rsaxt);
                                todoid += tmpnum2;
                                continue;
                            } else {
                                split_nonleafkey(leafKeys, nonLeafKeys, todoid, tmpnum1, todonum, co_d1, re_rsaxt);
                                build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, anode.id, anode.num, anode.co_d,
                                                          anode.lsaxt, anode.rsaxt);
                                todoid += tmpnum2;
                                continue;
                            }
                        } else {
                            // 比相聚度
                            if (co_d2 >= co_d1) {
                                build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, todoid, tmpnum2, co_d2, re_lsaxt,
                                                          anode.rsaxt);
                                todoid += tmpnum2;
                                continue;
                            } else {
                                add_nonleafkey(leafKeys, nonLeafKeys, todoid, todonum, co_d1, re_rsaxt);
                                build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, anode.id, anode.num, anode.co_d,
                                                          anode.lsaxt, anode.rsaxt);
                                todoid += tmpnum2;
                                continue;
                            }
                        }
                    }
                }
            } else if (todonum > m && todonum <= n) {
                //其实就是window_co_d
                build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, todoid, todonum,
                                          get_co_d_from_saxt(re_lsaxt, re_rsaxt, window_co_d), re_lsaxt, re_rsaxt);
                build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, anode.id, anode.num, anode.co_d, anode.lsaxt, anode.rsaxt);
                todoid = todoid + todonum + anode.num;
                continue;
            } else {
                build_leaf_and_nonleafkey_two(leafKeys, nonLeafKeys, todoid, todonum, window_co_d, re_lsaxt, re_rsaxt);
                build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, anode.id, anode.num, anode.co_d, anode.lsaxt, anode.rsaxt);
                todoid = todoid + todonum + anode.num;
                continue;
            }
        }
        int todonum = num - todoid;
        if (todonum > n) {
            //直接平分打包
            saxt lsaxt = get_saxt_i(leafKeys, todoid);
            build_leaf_and_nonleafkey_two(leafKeys, nonLeafKeys, todoid, todonum, window_co_d, lsaxt, last_saxt);
        } else if (todonum > m && todonum <= n) {
            //直接打包
            saxt lsaxt = get_saxt_i(leafKeys, todoid);
            build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, todoid, todonum, window_co_d, lsaxt, last_saxt);
        } else if (todonum > 0) {
            if (!nonLeafKeys.empty()) {
                //跟前面合
                saxt prelsaxt = nonLeafKeys.back().lsaxt;
                int prenum = nonLeafKeys.back().num;
                int tmpnum = prenum + todonum;
                cod co_d1;
                if (todoid == 0) co_d1 = get_co_d_from_saxt(prelsaxt, last_saxt);
                else co_d1 = get_co_d_from_saxt(prelsaxt, last_saxt, window_co_d);
                if (tmpnum <= n) {
                    add_nonleafkey(leafKeys, nonLeafKeys, todoid, todonum, co_d1, last_saxt);
                } else {
                    //平分
                    split_nonleafkey(leafKeys, nonLeafKeys, todoid, tmpnum, todonum, co_d1, last_saxt);
                }
            } else {
                //前面一个点没有直接打包
                build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, todoid, todonum, window_co_d, first_saxt, last_saxt);
            }
        }
    }

//批量构建树，后面是两个流
    void buildtree(newVector<LeafKey> &leafKeys, vector<NonLeafKey> &nonLeafKeys, const int n, const int m) {

        //左闭右开
        int l = 0;
        int r = 2*n;
        int todoid = 0;
        while (r<leafKeys.size()) {
            //复制了一片
            newVector<LeafKey> leafKey_window(leafKeys, l, r);
            todoid = buildtree_window(leafKey_window, nonLeafKeys, n, m);
            l += todoid;
            r += todoid;
        }
        int num = leafKeys.size() - l - 1;
        if (num>0 && num<=n) {
            //最后剩余小于等于n
            saxt lsaxt = get_saxt_i(leafKeys, l);
            saxt rsaxt = leafKeys.back().asaxt;
            build_leaf_and_nonleafkey(leafKeys, nonLeafKeys, l, num, get_co_d_from_saxt(lsaxt, rsaxt), lsaxt, rsaxt);
        } else if (num > n) {
            //复制了一片
            newVector<LeafKey> leafKey_window(leafKeys, l, leafKeys.size());
            buildtree_window_last(leafKey_window, nonLeafKeys, num, n, m);
        }

    }
}




#endif //TODOZSBTREE_ZSBTREE_LEAFBUILD_H
