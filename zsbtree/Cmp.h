//
// Created by hh on 2022/11/23.
//

#ifndef TODOZSBTREE_CMP_H
#define TODOZSBTREE_CMP_H


#include "LeafKey.h"
#include "globals.h"


//排序用
static bool LeafKey_cmp(const LeafKey &a, const LeafKey &b);

static bool LeafKey_cmp1(const LeafKey &a, const LeafKey &b);

// <=
static bool saxt_cmp(saxt a, saxt b, cod co_d);

// <=
static bool saxt_cmp(saxt a, saxt b);

static void get_dist_and_sort(ts_type* paa, LeafKey* leafKeys, int num, dist_p* dist_ps);


#endif //TODOZSBTREE_CMP_H
