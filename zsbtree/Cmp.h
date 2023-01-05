//
// Created by hh on 2022/11/23.
//

#ifndef TODOZSBTREE_CMP_H
#define TODOZSBTREE_CMP_H


#include "LeafKey.h"
#include "globals.h"
#include <algorithm>

//排序用
static bool LeafKey_cmp(const LeafKey &a, const LeafKey &b);

static bool LeafKey_cmp1(const LeafKey &a, const LeafKey &b);

// <=
static bool saxt_cmp(saxt a, saxt b, cod co_d) {
  int d = co_d;
  for (;d<Bit_cardinality;d++) {
    if (a[d] != b[d]) return a[d] < b[d];
  }
  return true;
}

// <=
static bool saxt_cmp(saxt a, saxt b) {
  for (int d = 0;d<Bit_cardinality;d++) {
    if (a[d] != b[d]) return a[d] < b[d];
  }
  return true;
}

static void get_dist_and_sort(ts_type* paa, LeafKey* leafKeys, int num, dist_p* dist_ps) {
  for(int i=0;i<=num;i++) {
    dist_ps[i] = {minidist_paa_to_saxt(paa, leafKeys[i].asaxt, Bit_cardinality),leafKeys[i].p};
  }
  std::sort(dist_ps, dist_ps + num);
}


#endif //TODOZSBTREE_CMP_H
