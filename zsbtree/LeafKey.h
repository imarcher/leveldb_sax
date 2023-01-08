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
    LeafKey(saxt_only saxt_);
    LeafKey(saxt_only saxt_, void* p);
    LeafKey(saxt_only prefix, char* stleafkey, cod noco_size) {
      asaxt = prefix;
      memcpy(this, stleafkey, noco_size);
    }



    void setAsaxt(saxt_only saxt_);

    bool operator< (const LeafKey& leafKey) const ;

    bool operator> (const LeafKey& leafKey) const ;

    bool operator<= (const LeafKey& leafKey) const ;

    bool operator>= (const LeafKey& leafKey) const ;

    void* p;
    saxt_only asaxt;
};


typedef struct{
  LeafKey leafKey;
  ts_time keytime;
} LeafTimeKey;






#endif //TODOZSBTREE_LEAFKEY_H
