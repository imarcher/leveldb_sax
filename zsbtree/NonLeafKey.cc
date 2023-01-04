//
// Created by hh on 2022/11/22.
//

#include "NonLeafKey.h"



NonLeafKey::NonLeafKey(int num, cod co_d, saxt lsaxt, saxt rsaxt, void *p) {
    this->num = num;
    this->co_d = co_d;
    memcpy(this->lsaxt, lsaxt, saxt_size);
    memcpy(this->rsaxt, rsaxt, saxt_size);
    this->p = p;
}


void NonLeafKey::setLsaxt(saxt saxt_) {
    memcpy(lsaxt, saxt_, saxt_size);
}

void NonLeafKey::setRsaxt(saxt saxt_) {
    memcpy(rsaxt, saxt_, saxt_size);
}

NonLeafKey::NonLeafKey() {

}

void NonLeafKey::Set(int num, cod co_d, saxt lsaxt, saxt rsaxt, void* p) {
  this->num = num;
  this->co_d = co_d;
  memcpy(this->lsaxt, lsaxt, saxt_size);
  memcpy(this->rsaxt, rsaxt, saxt_size);
  this->p = p;
}


