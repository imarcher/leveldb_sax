//
// Created by hh on 2022/11/23.
//

#include "Cmp.h"



//排序用
bool LeafKey_cmp(const LeafKey &a, const LeafKey &b) {

    for (int i=0;i<Bit_cardinality;i++){
        if (a.asaxt[i] < b.asaxt[i]) return true;
        else if (a.asaxt[i] > b.asaxt[i]) return false;
    }
    return true;
}



bool saxt_cmp(saxt a, saxt b, cod co_d) {
    int d = co_d;
    for (;d<Bit_cardinality;d++) {
        if (a[d] < b[d]) return true;
        else if (a[d] > b[d]) return false;
    }
    return true;
}