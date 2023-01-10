// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_INCLUDE_TABLE_H_
#define STORAGE_LEVELDB_INCLUDE_TABLE_H_

#include <cstdint>
#include "leveldb/export.h"
#include "leveldb/iterator.h"
#include "zsbtree/zsbtree_table.h"
#include "stack"
#include "STNonLeaf.h"
#include "STLeaf.h"

namespace leveldb {

class Block;
class BlockHandle;
class Footer;
struct Options;
class RandomAccessFile;
struct ReadOptions;
class TableCache;


// A Table is a sorted map from strings to strings.  Tables are
// immutable and persistent.  A Table may be safely accessed from
// multiple threads without external synchronization.
class LEVELDB_EXPORT Table {
 public:
  // Attempt to open the table that is stored in bytes [0..file_size)
  // of "file", and read the metadata entries necessary to allow
  // retrieving data from the table.
  //
  // If successful, returns ok and sets "*table" to the newly opened
  // table.  The client should delete "*table" when no longer needed.
  // If there was an error while initializing the table, sets "*table"
  // to nullptr and returns a non-ok status.  Does not take ownership of
  // "*source", but the client must ensure that "source" remains live
  // for the duration of the returned table's lifetime.
  //
  // *file must remain live while this Table is in use.
  static Status Open(const Options& options, RandomAccessFile* file,
                     uint64_t file_size, Table** table);

  Table(const Table&) = delete;
  Table& operator=(const Table&) = delete;

  ~Table();

  // Returns a new iterator over the table contents.
  // The result of NewIterator() is initially invalid (caller must
  // call one of the Seek methods on the iterator before using it).
  Iterator* NewIterator(const ReadOptions&) const;

  // Given a key, return an approximate byte offset in the file where
  // the data for that key begins (or would begin if the key were
  // present in the file).  The returned value is in terms of file
  // bytes, and so includes effects like compression of the underlying data.
  // E.g., the approximate offset of the last key in the table will
  // be close to the file length.
  uint64_t ApproximateOffsetOf(const Slice& key) const;



 private:
  friend class TableCache;
  struct Rep;





  static Iterator* BlockReader(void*, const ReadOptions&, const Slice&);

  explicit Table(Rep* rep) : rep_(rep) {}

  // Calls (*handle_result)(arg, ...) with the entry found after a call
  // to Seek(key).  May not make such a call if filter policy says
  // that key is not present.
  Status InternalGet(const ReadOptions&, const Slice& key, vector<LeafKey>& leafKeys);

  void ReadMeta(const Footer& footer);
  void ReadFilter(const Slice& filter_handle_value);




  Rep* const rep_;

 public:
  class ST_finder{
   public:
    ST_finder(Table* rep, saxt_only saxt_, ts_type* paa_) : rep_(rep->rep_) {
      leafkey = saxt_;
      memcpy(paa, paa_, sizeof(ts_type) * Segments);
    }

    ~ST_finder() { delete to_find_nonleaf; }

   public:

    void root_Get();

    void find_One(LeafKey* res, int& res_num);

    void find_One(LeafKey* res, int& res_num, int id);

    void sort();

   private:

    inline STLeaf* getSTLeaf(STNonLeaf& nonLeaf, int i);
    inline STNonLeaf* getSTNonLeaf(STNonLeaf& nonLeaf, int i);
    inline int whereofKey(saxt lsaxt, saxt rsaxt, saxt leafKey, cod co_d);
    bool saxt_cmp(saxt a, saxt b, cod co_d);
    cod get_co_d_from_saxt(saxt a, saxt b, cod pre_d);
    void l_Get_NonLeaf(STNonLeaf& nonLeaf1, int i);

    void r_Get_NonLeaf(STNonLeaf& nonLeaf1, int i);

    inline void leaf_Get(STLeaf& leaf, LeafKey* res);

// 在nonLeaf的范围里, 才调用
    bool nonLeaf_Get(STNonLeaf& nonLeaf);

   public:
    vector<pair<float, int>> has_cod;
    vector<int> no_has_cod;
   private:
    saxt_only leafkey;
    ts_type paa[Segments];
    STNonLeaf* to_find_nonleaf;
    int oneId;//叶子在非叶结点中的位置
    Rep* const rep_;
  };


  class ST_Iter{
   public:
    ST_Iter(Table* table);
    bool next(LeafKey& res);
    void setPrefix(LeafKey& res);
    ~ST_Iter();
   private:
    bool getSTLeaf();
    void getSTNonLeaf();
    Rep* const rep_;
    //栈，要delete除了root
    vector<STNonLeaf*> st_nonleaf_stack;
    //指定当前的位置
    int top;
    //指定当前nonleaf的nonleafkey位置
    int nonleaftops[10];
    STLeaf stLeaf;
    int leaftop;
  };
};

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_INCLUDE_TABLE_H_
