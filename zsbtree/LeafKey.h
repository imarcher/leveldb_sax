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
    LeafKey(saxt prefix, char* stleafkey, cod co_size, cod noco_size);

    void Set(const LeafKey& leafKey);

    void Set(saxt prefix, char* stleafkey, cod co_size, cod noco_size);

    void setAsaxt(saxt saxt_);

    bool operator< (const LeafKey& leafKey) const ;

    bool operator> (const LeafKey& leafKey) const ;

    bool operator<= (const LeafKey& leafKey) const ;

    bool operator>= (const LeafKey& leafKey) const ;


    saxt_type asaxt[Bit_cardinality];
    void* p;
};


#endif //TODOZSBTREE_LEAFKEY_H
