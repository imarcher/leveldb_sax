//
// 插入，假设叶结点每满n
// Created by hh on 2022/11/25.
//

#ifndef TODOZSBTREE_ZSBTREE_INSERT_H
#define TODOZSBTREE_ZSBTREE_INSERT_H



#include <globals.h>

#include "vector"
#include "sax.h"
#include "Leaf.h"
#include "NonLeaf.h"
#include "Cmp.h"

// -1 0 1代表在点左边，中间，右边
inline int whereofKey(saxt lsaxt, saxt rsaxt, saxt leafKey, cod co_d);


bool l_Insert_NonLeaf(NonLeafKey &nonLeafKey, LeafKey &leafKey, cod co_d, bool isleaf);

bool r_Insert_NonLeaf(NonLeafKey &nonLeafKey, LeafKey &leafKey, cod co_d, bool isleaf);


inline bool leaf_Insert(Leaf &leaf, LeafKey &leafKey);


// 在nonLeaf的范围里, 才调用
bool nonLeaf_Insert(NonLeaf &nonLeaf, LeafKey &leafKey);

// 从根节点插入，设叶节点无限大 , 返回true ,叶结点放得下，false，要重组了
bool root_Insert(NonLeaf &nonLeaf, LeafKey &leafKey);









#endif //TODOZSBTREE_ZSBTREE_INSERT_H
