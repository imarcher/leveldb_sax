//
// Created by hh on 2022/12/12.
//

#include "STLeaf.h"


STLeaf::STLeaf(unsigned short num, cod co_d, size_t size): num(num), co_d(co_d), ismmap(false) {
  co_size = co_d * sizeof(saxt_type);
  noco_size = sizeof(saxt_only) - co_size + 8;
  rep = new char[size];
}

STLeaf::~STLeaf() {
  if (!ismmap) delete rep;
}

char* STLeaf::Get_rep(int i) { return rep + i * noco_size; }


STLeaf::STLeaf(size_t size): num(0), ismmap(false) { rep = new char[size]; }

void STLeaf::Set(unsigned short num, cod co_d) {
  this->num = num;
  this->co_d = co_d;
  co_size = co_d * sizeof(saxt_type);
  noco_size = sizeof(saxt_only) - co_size + 8;
}
void STLeaf::Setrep(const char* newrep) {
  if (newrep!=rep){
    if (!ismmap) delete rep;
    ismmap = true;
    rep = const_cast<char*>(newrep);
  }
}

void STLeaf::Setnewroom(size_t size) {
  rep = new char[size];
  ismmap = false;
}
void STLeaf::Setprefix(saxt_only prefix, saxt stleafkey,
                       cod noco_size) {
  this->prefix = prefix;
  memcpy(this->prefix.asaxt, stleafkey, noco_size);
}

void STLeaf::Setprefix(saxt_only prefix1) {
  prefix = prefix1;
}

void STLeaf::Setrep1(const char* newrep) {
  if (!ismmap)
    delete rep;
  ismmap = false;
  rep = const_cast<char*>(newrep);
}
