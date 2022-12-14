//
// Created by hh on 2022/11/22.
//

#include "LeafKey.h"


LeafKey::LeafKey(saxt saxt_) {
    memcpy(asaxt, saxt_, saxt_size);
}

void LeafKey::setAsaxt(saxt saxt_) {
    memcpy(asaxt, saxt_, saxt_size);
}

LeafKey::LeafKey() {

}

LeafKey::LeafKey(saxt saxt_, void* p) {
    memcpy(asaxt, saxt_, saxt_size);
    this->p = p;
}

LeafKey::LeafKey(saxt prefix, char* stleafkey, cod co_size, cod noco_size) {
  memcpy(asaxt, prefix, co_size);
  memcpy(((char*)asaxt)+co_size, stleafkey, noco_size);
}

void LeafKey::Set(const LeafKey& leafKey) {
  memcpy(this, &leafKey, leaf_key_size);
}

void LeafKey::Set(saxt prefix, char* stleafkey, cod co_size, cod noco_size) {
  memcpy(asaxt, prefix, co_size);
  memcpy(((char*)asaxt)+co_size, stleafkey, noco_size);
}


bool LeafKey::operator<(const LeafKey& leafKey) const {
  for (int i=0;i<Bit_cardinality;i++){
    if (asaxt[i] != leafKey.asaxt[i]) return asaxt[i] < leafKey.asaxt[i];
  }
  return false;
}

bool LeafKey::operator>(const LeafKey& leafKey) const {
  for (int i=0;i<Bit_cardinality;i++){
    if (asaxt[i] != leafKey.asaxt[i]) return asaxt[i] > leafKey.asaxt[i];
  }
  return false;
}



