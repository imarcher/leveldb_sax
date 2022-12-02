//
// Created by hh on 2022/11/22.
//

#ifndef TODOZSBTREE_LEAFKEY_H
#define TODOZSBTREE_LEAFKEY_H

#include "sax.h"
#include "cstring"
class LeafKey {
public:

    LeafKey();
    LeafKey(saxt saxt_);
    LeafKey(saxt saxt_, void* p);

    void setAsaxt(saxt saxt_);

    saxt_type asaxt[Bit_cardinality];
    void* p;
};


#endif //TODOZSBTREE_LEAFKEY_H
