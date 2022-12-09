//
// Created by hh on 2022/12/9.
//

#include "STpos.h"


STpos::STpos(unsigned short size, size_t offset) {
  pos = offset;
  *(unsigned short*)(&pos) = size;
}

unsigned short STpos::GetSize() {
  return *(unsigned short*)(&pos);
}

size_t STpos::GetOffset() {
  return pos << 16 >> 16;
}

STpos::STpos() {}

void STpos::Set(unsigned short size, size_t offset) {
  pos = offset;
  *(unsigned short*)(&pos) = size;
}

void* STpos::Get() {
  return (void*)pos;
}
