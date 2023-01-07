//
// Created by hh on 2022/11/21.
//

#include "Leaf.h"
#include "NonLeafKey.h"


Leaf::Leaf(int num, cod co_d, saxt lsaxt, saxt rsaxt, LeafKey *leafKeys) {
    this->num = num;
    this->co_d = co_d;
    memcpy(this->lsaxt, lsaxt, saxt_size);
    memcpy(this->rsaxt, rsaxt, saxt_size);
    memcpy(this->leafKeys, leafKeys, sizeof(LeafKey) * num);
}

void Leaf::add(LeafKey *leafKeys, int num) {
    memcpy(this->leafKeys + this->num, leafKeys, sizeof(LeafKey) * num);
    this->num += num;
}



void Leaf::setLeafKeys(LeafKey *leafKeys) {
    memcpy(this->leafKeys, leafKeys, sizeof(LeafKey) * num);
}

void Leaf::setLsaxt(saxt saxt_) {
    memcpy(lsaxt, saxt_, saxt_size);
}

void Leaf::setRsaxt(saxt saxt_) {
    memcpy(rsaxt, saxt_, saxt_size);
}

Leaf::Leaf() {

}


void Leaf::set(NonLeafKey& nonLeafKey) {
  num = nonLeafKey.num;
  co_d = nonLeafKey.co_d;
  memcpy(lsaxt, nonLeafKey.lsaxt, saxt_size);
  memcpy(rsaxt, nonLeafKey.rsaxt, saxt_size);
}

Leaf::Leaf(NonLeafKey& nonLeafKey) {
  num = nonLeafKey.num;
  co_d = nonLeafKey.co_d;
  memcpy(lsaxt, nonLeafKey.lsaxt, saxt_size);
  memcpy(rsaxt, nonLeafKey.rsaxt, saxt_size);
}

void Leaf::sort(LeafKey* dst) {
  mempcpy(dst, leafKeys, sizeof(LeafKey) * num);
  std::sort(dst, dst + num);
}

void Leaf::sort() {
  std::sort(leafKeys, leafKeys + num);
}
