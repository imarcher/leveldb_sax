//
// Created by hh on 2022/11/22.
//

#include "NonLeaf.h"



NonLeaf::NonLeaf(int num, cod co_d, bool isleaf, saxt lsaxt, saxt rsaxt, NonLeafKey *nonLeafKeys) {
    this->num = num;
    this->co_d = co_d;
    this->isleaf = isleaf;
    memcpy(this->lsaxt, lsaxt, saxt_size);
    memcpy(this->rsaxt, rsaxt, saxt_size);
    memcpy(this->nonLeafKeys, nonLeafKeys, sizeof(NonLeafKey) * num);
}


void NonLeaf::add(NonLeafKey nonLeafKey) {
    this->nonLeafKeys[num] = nonLeafKey;
    num++;
}

void NonLeaf::add(NonLeafKey *nonLeafKeys, int num) {
    memcpy(this->nonLeafKeys + this->num, nonLeafKeys, sizeof(LeafKey) * num);
    this->num += num;
}


void NonLeaf::setLsaxt(saxt saxt_) {
    memcpy(lsaxt, saxt_, saxt_size);
}

void NonLeaf::setRsaxt(saxt saxt_) {
    memcpy(rsaxt, saxt_, saxt_size);
}




