//
// Created by hh on 2022/11/22.
//

#ifndef TODOZSBTREE_NONLEAFKEY_H
#define TODOZSBTREE_NONLEAFKEY_H

#include "Leaf.h"
#include "STpos.h"
#include "sax.h"

class NonLeafKey {
public:
    NonLeafKey();
    NonLeafKey(int num, cod co_d, saxt lsaxt, saxt rsaxt, void *p);
    void setLsaxt(saxt saxt_);
    void setRsaxt(saxt saxt_);

    int num;
    cod co_d;
    saxt_type lsaxt[Bit_cardinality];
    saxt_type rsaxt[Bit_cardinality];
    void* p;
};


#endif //TODOZSBTREE_NONLEAFKEY_H
