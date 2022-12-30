//
// Created by hh on 2022/11/23.
//

#include "Cmp.h"



//<=
bool LeafKey_cmp(const LeafKey &a, const LeafKey &b) {
    for (int i=0;i<Bit_cardinality;i++){
      if (a.asaxt[i] != b.asaxt[i]) return a.asaxt[i] < b.asaxt[i];
    }
    return true;
}

//<
bool LeafKey_cmp1(const LeafKey &a, const LeafKey &b) {
    for (int i=0;i<Bit_cardinality;i++){
        if (a.asaxt[i] != b.asaxt[i]) return a.asaxt[i] < b.asaxt[i];
    }
    return false;
}


//<=
bool saxt_cmp(saxt a, saxt b, cod co_d) {
    int d = co_d;
    for (;d<Bit_cardinality;d++) {
      if (a[d] != b[d]) return a[d] < b[d];
    }
    return true;
}

bool saxt_cmp(saxt a, saxt b) {
    for (int d = 0;d<Bit_cardinality;d++) {
        if (a[d] != b[d]) return a[d] < b[d];
    }
    return true;
}