//
// Created by hh on 2022/11/21.
//

#ifndef TODOZSBTREE_LEAF_H
#define TODOZSBTREE_LEAF_H



#include "LeafKey.h"
#include <cstring>
class Leaf {
public:
    Leaf();
    Leaf(int num, cod co_d, saxt lsaxt, saxt rsaxt, LeafKey *leafKeys);
    //添加一个saxt，p
    void add(LeafKey leafKey);
    void add(LeafKey *leafKeys, int num);
    void setLeafKeys(LeafKey *leafKeys);
    void setLsaxt(saxt saxt_);
    void setRsaxt(saxt saxt_);
    //有几个
    int num = 0;
    //相聚度
    cod co_d;
    saxt_type lsaxt[Bit_cardinality];
    saxt_type rsaxt[Bit_cardinality];
    //叶结点中的key元组
    LeafKey leafKeys[Leaf_rebuildnum];
};


#endif //TODOZSBTREE_LEAF_H
