//
// Created by hh on 2022/12/12.
//

#include "STLeaf.h"


STLeaf::STLeaf(unsigned short num, size_t size): num(num), ismmap(false) {
  rep = new char[size];
}

STLeaf::~STLeaf() {
  if (!ismmap) delete rep;
}

char* STLeaf::Get_rep(int i) { return rep + i * sizeof(LeafKey); }


STLeaf::STLeaf(size_t size): num(0), ismmap(false) { rep = new char[size]; }

void STLeaf::Set(unsigned short num) {
  this->num = num;
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

void STLeaf::Setrep1(const char* newrep) {
  if (!ismmap)
    delete rep;
  ismmap = false;
  rep = const_cast<char*>(newrep);
}




