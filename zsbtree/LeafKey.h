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


    inline void Set(saxt prefix, char* stleafkey, cod co_size, cod noco_size) {
      memcpy(asaxt, prefix, co_size);
      memcpy(((char*)asaxt)+co_size, stleafkey, noco_size);
    }

    inline void Set1(saxt prefix, cod co_size) {
      memcpy(asaxt, prefix, co_size);
    }

    inline void Set2(char* stleafkey, cod co_size, cod noco_size) {
      memcpy(((char*)asaxt)+co_size, stleafkey, noco_size);
    }

    void setAsaxt(saxt saxt_);

    bool operator< (const LeafKey& leafKey) const ;

    bool operator> (const LeafKey& leafKey) const ;

    bool operator<= (const LeafKey& leafKey) const ;

    bool operator>= (const LeafKey& leafKey) const ;


    saxt_type asaxt[Bit_cardinality];
    void* p;
};


typedef struct{
  LeafKey leafKey;
  ts_time keytime;
} LeafTimeKey;

typedef struct Writes_vec{
  Writes_vec(): size_(0){}
  LeafTimeKey keys[256];
  int size_;
} Writes_vec;




#endif //TODOZSBTREE_LEAFKEY_H
