//
// Created by hh on 2022/11/22.
//

#include "LeafKey.h"


LeafKey::LeafKey(saxt saxt_) {
    memcpy(asaxt, saxt_, saxt_size);
}

void LeafKey::setAsaxt(saxt saxt_) {
    memcpy(asaxt, saxt_, saxt_size);
}

LeafKey::LeafKey() {

}

LeafKey::LeafKey(saxt saxt_, void* p) {
    memcpy(asaxt, saxt_, saxt_size);
    this->p = p;
}
