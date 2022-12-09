//
// Created by hh on 2022/12/9.
//

#include "STkeyinfo.h"


STkeyinfo::STkeyinfo(cod co_d, int num) {
  co_d_num = num;
  unsigned short tmp = co_d;
  co_d_num |= tmp << 11;
}


cod STkeyinfo::GetCo_d() {
  return co_d_num >> 11;
}


int STkeyinfo::GetNum() {
  return co_d_num << 5 >> 5;
}
