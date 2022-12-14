// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "leveldb/table.h"

#include "leveldb/cache.h"
#include "leveldb/comparator.h"
#include "leveldb/env.h"
#include "leveldb/filter_policy.h"
#include "leveldb/options.h"
#include "table/block.h"
#include "table/filter_block.h"
#include "table/format.h"
#include "table/two_level_iterator.h"
#include "util/coding.h"

namespace leveldb {

struct Table::Rep {
  ~Rep() {
//    delete filter;
//    delete[] filter_data;
    delete stNonLeaf;
  }

  Options options;
  Status status;
  RandomAccessFile* file;
  uint64_t cache_id;
//  FilterBlockReader* filter;
//  const char* filter_data;

//  BlockHandle metaindex_handle;  // Handle to metaindex_block: saved from footer
  STNonLeaf* stNonLeaf;
  saxt_type rsaxt[Bit_cardinality];
};



//获得了root
Status Table::Open(const Options& options, RandomAccessFile* file,
                   uint64_t size, Table** table) {
  *table = nullptr;
  if (size < sizeof(NonLeafKey)) {
    return Status::Corruption("file is too short to be an sstable");
  }


  // 这里是申请了空间，但是不一定有用，mmap的时候就没用
  NonLeafKey rootkey;
  Slice rootkey_input;
  Status s = file->Read(size - sizeof(NonLeafKey), sizeof(NonLeafKey),
                        &rootkey_input, (char*)&rootkey);


  if (!s.ok()) return s;

  //先不读校验码
  NonLeafKey* real_rootkey = (NonLeafKey*)rootkey_input.data();
  STpos* sTpos = (STpos*)&real_rootkey->p;
  size_t stNonLeaf_size = sTpos->GetSize();
  Slice stNonLeaf_input;
  //开了空间的
  STNonLeaf* stNonLeaf = new STNonLeaf(real_rootkey->num, real_rootkey->co_d, stNonLeaf_size);
  stNonLeaf->Setprefix(real_rootkey->lsaxt);
  s = file->Read(sTpos->GetOffset(), stNonLeaf_size,
                 &stNonLeaf_input, stNonLeaf->rep);
  stNonLeaf->Setrep(stNonLeaf_input.data());
  stNonLeaf->Setisleaf();

  if (s.ok()) {
    // We've successfully read the footer and the index block: we're
    // ready to serve requests.
    Rep* rep = new Table::Rep;
    rep->options = options;
    rep->file = file;
    rep->stNonLeaf = stNonLeaf;
    saxt_copy(rep->rsaxt, real_rootkey->rsaxt);
    rep->cache_id = (options.block_cache ? options.block_cache->NewId() : 0);
    *table = new Table(rep);
//    (*table)->ReadMeta(footer);
  } else {
    delete stNonLeaf;
  }

  return s;
}

void Table::ReadMeta(const Footer& footer) {
  if (rep_->options.filter_policy == nullptr) {
    return;  // Do not need any metadata
  }

  // TODO(sanjay): Skip this if footer.metaindex_handle() size indicates
  // it is an empty block.
  ReadOptions opt;
  if (rep_->options.paranoid_checks) {
    opt.verify_checksums = true;
  }
  BlockContents contents;
  if (!ReadBlock(rep_->file, opt, footer.metaindex_handle(), &contents).ok()) {
    // Do not propagate errors since meta info is not needed for operation
    return;
  }
  Block* meta = new Block(contents);

  Iterator* iter = meta->NewIterator(BytewiseComparator());
  std::string key = "filter.";
  key.append(rep_->options.filter_policy->Name());
  iter->Seek(key);
  if (iter->Valid() && iter->key() == Slice(key)) {
    ReadFilter(iter->value());
  }
  delete iter;
  delete meta;
}

void Table::ReadFilter(const Slice& filter_handle_value) {
//  Slice v = filter_handle_value;
//  BlockHandle filter_handle;
//  if (!filter_handle.DecodeFrom(&v).ok()) {
//    return;
//  }
//
//  // We might want to unify with ReadBlock() if we start
//  // requiring checksum verification in Table::Open.
//  ReadOptions opt;
//  if (rep_->options.paranoid_checks) {
//    opt.verify_checksums = true;
//  }
//  BlockContents block;
//  if (!ReadBlock(rep_->file, opt, filter_handle, &block).ok()) {
//    return;
//  }
//  if (block.heap_allocated) {
//    rep_->filter_data = block.data.data();  // Will need to delete later
//  }
//  rep_->filter = new FilterBlockReader(rep_->options.filter_policy, block.data);
}

Table::~Table() { delete rep_; }

static void DeleteBlock(void* arg, void* ignored) {
  delete reinterpret_cast<Block*>(arg);
}

static void DeleteCachedBlock(const Slice& key, void* value) {
  Block* block = reinterpret_cast<Block*>(value);
  delete block;
}

static void ReleaseBlock(void* arg, void* h) {
  Cache* cache = reinterpret_cast<Cache*>(arg);
  Cache::Handle* handle = reinterpret_cast<Cache::Handle*>(h);
  cache->Release(handle);
}

// Convert an index iterator value (i.e., an encoded BlockHandle)
// into an iterator over the contents of the corresponding block.
Iterator* Table::BlockReader(void* arg, const ReadOptions& options,
                             const Slice& index_value) {
  Table* table = reinterpret_cast<Table*>(arg);
  Cache* block_cache = table->rep_->options.block_cache;
  Block* block = nullptr;
  Cache::Handle* cache_handle = nullptr;

  BlockHandle handle;
  Slice input = index_value;
  Status s = handle.DecodeFrom(&input);
  // We intentionally allow extra stuff in index_value so that we
  // can add more features in the future.

  if (s.ok()) {
    BlockContents contents;
    if (block_cache != nullptr) {
      char cache_key_buffer[16];
      EncodeFixed64(cache_key_buffer, table->rep_->cache_id);
      EncodeFixed64(cache_key_buffer + 8, handle.offset());
      Slice key(cache_key_buffer, sizeof(cache_key_buffer));
      cache_handle = block_cache->Lookup(key);
      if (cache_handle != nullptr) {
        block = reinterpret_cast<Block*>(block_cache->Value(cache_handle));
      } else {
        s = ReadBlock(table->rep_->file, options, handle, &contents);
        if (s.ok()) {
          block = new Block(contents);
          if (contents.cachable && options.fill_cache) {
            cache_handle = block_cache->Insert(key, block, block->size(),
                                               &DeleteCachedBlock);
          }
        }
      }
    } else {
      s = ReadBlock(table->rep_->file, options, handle, &contents);
      if (s.ok()) {
        block = new Block(contents);
      }
    }
  }

  Iterator* iter;
  if (block != nullptr) {
    iter = block->NewIterator(table->rep_->options.comparator);
    if (cache_handle == nullptr) {
      iter->RegisterCleanup(&DeleteBlock, block, nullptr);
    } else {
      iter->RegisterCleanup(&ReleaseBlock, block_cache, cache_handle);
    }
  } else {
    iter = NewErrorIterator(s);
  }
  return iter;
}

Iterator* Table::NewIterator(const ReadOptions& options) const {
//  return NewTwoLevelIterator(
//      rep_->index_block->NewIterator(rep_->options.comparator),
//      &Table::BlockReader, const_cast<Table*>(this), options);
}





Status Table::InternalGet(const ReadOptions& options, const Slice& k, vector<LeafKey>& leafKeys) {
  Status s;
  saxt key = (saxt)k.data();
  LeafKey leafKey(key);
  assert(0);
//  ST_finder stFinder(this, leafKeys);
//  stFinder.root_Get(leafKey);


//  Iterator* iiter = rep_->index_block->NewIterator(rep_->options.comparator);
//  iiter->Seek(k);
//  if (iiter->Valid()) {
//    Slice handle_value = iiter->value();
//    FilterBlockReader* filter = rep_->filter;
//    BlockHandle handle;
//    if (filter != nullptr && handle.DecodeFrom(&handle_value).ok() &&
//        !filter->KeyMayMatch(handle.offset(), k)) {
//      // Not found
//    } else {
//      Iterator* block_iter = BlockReader(this, options, iiter->value());
//      block_iter->Seek(k);
//      if (block_iter->Valid()) {
//        (*handle_result)(arg, block_iter->key(), block_iter->value());
//      }
//      s = block_iter->status();
//      delete block_iter;
//    }
//  }
//  if (s.ok()) {
//    s = iiter->status();
//  }
//  delete iiter;
  return s;
}

uint64_t Table::ApproximateOffsetOf(const Slice& key) const {
//  Iterator* index_iter =
//      rep_->index_block->NewIterator(rep_->options.comparator);
//  index_iter->Seek(key);
//  uint64_t result;
//  if (index_iter->Valid()) {
//    BlockHandle handle;
//    Slice input = index_iter->value();
//    Status s = handle.DecodeFrom(&input);
//    if (s.ok()) {
//      result = handle.offset();
//    } else {
//      // Strange: we can't decode the block handle in the index block.
//      // We'll just return the offset of the metaindex block, which is
//      // close to the whole file size for this case.
//      result = rep_->metaindex_handle.offset();
//    }
//  } else {
//    // key is past the last key in the file.  Approximate the offset
//    // by returning the offset of the metaindex block (which is
//    // right near the end of the file).
//    result = rep_->metaindex_handle.offset();
//  }
//  delete index_iter;
//  return result;
}



void Table::ST_finder::root_Get() {
  STNonLeaf *root = rep_->stNonLeaf;
  out("===========");
  out("开始查");
  out(root->size);
  out((int)root->co_d);
  out(root->isleaf);
  out(root->num);
  out("最大最小");
  saxt_print(root->prefix);
  saxt_print(rep_->rsaxt);
  out("===========");
//  for (int i = 0; i < root->num; ++i) {
//    out("第"+to_string(i)+":");
//    saxt_print(root->Get_lsaxt(i));
//    saxt_print(root->Get_rsaxt(i));
//  }
  if (root->isleaf) {
    to_find_nonleaf = root;
    return;
  }
  int pos = whereofKey(root->prefix, rep_->rsaxt, leafkey, 0);
  if (pos==0) nonLeaf_Get(*root);
  else if (pos==-1) {
    l_Get_NonLeaf(*root, 0);
  } else {
    r_Get_NonLeaf(*root, root->num-1);
  }
}

//leafkey都是没用的
void Table::ST_finder::l_Get_NonLeaf(STNonLeaf& nonLeaf1, int i) {
  STNonLeaf* nonLeaf = getSTNonLeaf(nonLeaf1, i);
  if (nonLeaf->isleaf) {
    to_find_nonleaf = nonLeaf;
    return;
  }
  l_Get_NonLeaf(*nonLeaf, 0);
  delete nonLeaf;

//  if (nonLeaf.isleaf) {
//    STLeaf* stLeaf = getSTLeaf(nonLeaf, i);
//    leaf_Get(*stLeaf, leafKey);
//    delete stLeaf;
//  } else {
//    STNonLeaf* stNonLeaf = getSTNonLeaf(nonLeaf, i);
//    l_Get_NonLeaf(*stNonLeaf, 0, leafKey);
//    delete stNonLeaf;
//  }
}

void Table::ST_finder::r_Get_NonLeaf(STNonLeaf& nonLeaf1, int i) {
  STNonLeaf* nonLeaf = getSTNonLeaf(nonLeaf1, i);
  if (nonLeaf->isleaf) {
    to_find_nonleaf = nonLeaf;
    return;
  }
  r_Get_NonLeaf(*nonLeaf, nonLeaf->num-1);
  delete nonLeaf;
}

void Table::ST_finder::leaf_Get(STLeaf& leaf, LeafKey* res) {
  for(int i=0;i<leaf.num;i++){
    leaf.SetLeafKey(res+i, i);
  }
}

bool Table::ST_finder::nonLeaf_Get(STNonLeaf& nonLeaf) {
//
//  out("查看每一个范围=================");
//  for(int i=0;i<nonLeaf.num;i++){
//    out("最大最小");
//  }
//  out("查看每一个范围结束=================");
  if (nonLeaf.isleaf) {
    to_find_nonleaf = &nonLeaf;
    return false;
  }
  int l=0;
  int r=nonLeaf.num-1;
  while (l<r) {
    int mid = (l + r) / 2;
    if (saxt_cmp(leafkey + nonLeaf.co_d, nonLeaf.Get_rsaxt(mid), nonLeaf.co_d)) r = mid;
    else l = mid + 1;
  }
  int pos = whereofKey(nonLeaf.Get_lsaxt(l), nonLeaf.Get_rsaxt(l), leafkey + nonLeaf.co_d, nonLeaf.co_d);
  if (pos==0) {
    //里面
    STNonLeaf* stNonLeaf = getSTNonLeaf(nonLeaf, l);
    if (nonLeaf_Get(*stNonLeaf)) delete stNonLeaf;
  } else if (pos==-1){
    //前面有 先比相聚度下降程度,再看数量,但目前没有在非叶节点记录这种东西，所以这里直接比相聚度大小
    cod preco_d = nonLeaf.Get_co_d(l-1);
    saxt prelsaxt = nonLeaf.Get_lsaxt(l-1);
    cod nextco_d = nonLeaf.Get_co_d(l);
    saxt nextrsaxt = nonLeaf.Get_rsaxt(l);
    cod co_d1 = get_co_d_from_saxt(prelsaxt, leafkey + nonLeaf.co_d, nonLeaf.co_d);
    cod co_d2 = get_co_d_from_saxt(leafkey + nonLeaf.co_d, nextrsaxt, nonLeaf.co_d);
    if ((preco_d - co_d1) < (nextco_d - co_d2)) {
      // 跟前面
      r_Get_NonLeaf(nonLeaf, l-1);
    } else if ((preco_d - co_d1) > (nextco_d - co_d2)) {
      //跟后面
      l_Get_NonLeaf(nonLeaf, l);
    } else {
      if (co_d1 <= co_d2) {
        r_Get_NonLeaf(nonLeaf, l-1);
      } else {
        l_Get_NonLeaf(nonLeaf, l);
      }
    }

  } else {
    //后面有
    cod preco_d = nonLeaf.Get_co_d(l);
    saxt prelsaxt = nonLeaf.Get_lsaxt(l);
    cod nextco_d = nonLeaf.Get_co_d(l+1);
    saxt nextrsaxt = nonLeaf.Get_rsaxt(l+1);
    cod co_d1 = get_co_d_from_saxt(prelsaxt, leafkey + nonLeaf.co_d, nonLeaf.co_d);
    cod co_d2 = get_co_d_from_saxt(leafkey + nonLeaf.co_d, nextrsaxt, nonLeaf.co_d);
    if ((preco_d - co_d1) < (nextco_d - co_d2)) {
      // 跟前面
      r_Get_NonLeaf(nonLeaf, l);
    } else if ((preco_d - co_d1) > (nextco_d - co_d2)) {
      //跟后面
      l_Get_NonLeaf(nonLeaf, l+1);
    } else {
      if (co_d1 <= co_d2) {
        r_Get_NonLeaf(nonLeaf, l);
      } else {
        l_Get_NonLeaf(nonLeaf, l+1);
      }
    }
  }
  return true;
}

void Table::ST_finder::find_One(LeafKey* res, int& res_num) {
  STNonLeaf& nonLeaf = *to_find_nonleaf;
  saxt_only rsaxt;
  nonLeaf.SetSaxt(rsaxt.asaxt, nonLeaf.Get_rsaxt(nonLeaf.num-1));
  int pos = whereofKey(nonLeaf.prefix, rsaxt.asaxt, leafkey, 0);
  if (pos==0) {
    int l=0;
    int r=nonLeaf.num-1;
    while (l<r) {
      int mid = (l + r) / 2;
      if (saxt_cmp(leafkey + nonLeaf.co_d, nonLeaf.Get_rsaxt(mid), nonLeaf.co_d)) r = mid;
      else l = mid + 1;
    }
    pos = whereofKey(nonLeaf.Get_lsaxt(l), nonLeaf.Get_rsaxt(l), leafkey + nonLeaf.co_d, nonLeaf.co_d);
    if (pos==0) {
      //里面
      STLeaf* stLeaf = getSTLeaf(nonLeaf, l);
      oneId = l;
      leaf_Get(*stLeaf, res);
      res_num = stLeaf->num;
      delete stLeaf;
    } else if (pos==-1){
      //前面有 先比相聚度下降程度,再看数量,但目前没有在非叶节点记录这种东西，所以这里直接比相聚度大小
      cod preco_d = nonLeaf.Get_co_d(l-1);
      saxt prelsaxt = nonLeaf.Get_lsaxt(l-1);
      cod nextco_d = nonLeaf.Get_co_d(l);
      saxt nextrsaxt = nonLeaf.Get_rsaxt(l);
      cod co_d1 = get_co_d_from_saxt(prelsaxt, leafkey + nonLeaf.co_d, nonLeaf.co_d);
      cod co_d2 = get_co_d_from_saxt(leafkey + nonLeaf.co_d, nextrsaxt, nonLeaf.co_d);
      if ((preco_d - co_d1) < (nextco_d - co_d2)) {
        // 跟前面
        STLeaf* stLeaf = getSTLeaf(nonLeaf, l-1);
        oneId = l-1;
        leaf_Get(*stLeaf, res);
        res_num = stLeaf->num;
        delete stLeaf;
      } else if ((preco_d - co_d1) > (nextco_d - co_d2)) {
        //跟后面
        STLeaf* stLeaf = getSTLeaf(nonLeaf, l);
        oneId = l;
        leaf_Get(*stLeaf, res);
        res_num = stLeaf->num;
        delete stLeaf;
      } else {
        if (co_d1 <= co_d2) {
          STLeaf* stLeaf = getSTLeaf(nonLeaf, l-1);
          oneId = l-1;
          leaf_Get(*stLeaf, res);
          res_num = stLeaf->num;
          delete stLeaf;
        } else {
          STLeaf* stLeaf = getSTLeaf(nonLeaf, l);
          oneId = l;
          leaf_Get(*stLeaf, res);
          res_num = stLeaf->num;
          delete stLeaf;
        }
      }

    } else {
      //后面有
      cod preco_d = nonLeaf.Get_co_d(l);
      saxt prelsaxt = nonLeaf.Get_lsaxt(l);
      cod nextco_d = nonLeaf.Get_co_d(l+1);
      saxt nextrsaxt = nonLeaf.Get_rsaxt(l+1);
      cod co_d1 = get_co_d_from_saxt(prelsaxt, leafkey + nonLeaf.co_d, nonLeaf.co_d);
      cod co_d2 = get_co_d_from_saxt(leafkey + nonLeaf.co_d, nextrsaxt, nonLeaf.co_d);
      if ((preco_d - co_d1) < (nextco_d - co_d2)) {
        // 跟前面
        STLeaf* stLeaf = getSTLeaf(nonLeaf, l);
        oneId = l;
        leaf_Get(*stLeaf, res);
        res_num = stLeaf->num;
        delete stLeaf;
      } else if ((preco_d - co_d1) > (nextco_d - co_d2)) {
        //跟后面
        STLeaf* stLeaf = getSTLeaf(nonLeaf, l+1);
        oneId = l+1;
        leaf_Get(*stLeaf, res);
        res_num = stLeaf->num;
        delete stLeaf;
      } else {
        if (co_d1 <= co_d2) {
          STLeaf* stLeaf = getSTLeaf(nonLeaf, l);
          oneId = l;
          leaf_Get(*stLeaf, res);
          res_num = stLeaf->num;
          delete stLeaf;
        } else {
          STLeaf* stLeaf = getSTLeaf(nonLeaf, l+1);
          oneId = l+1;
          leaf_Get(*stLeaf, res);
          res_num = stLeaf->num;
          delete stLeaf;
        }
      }
    }
  }
  else if (pos==-1) {
    STLeaf* stLeaf = getSTLeaf(nonLeaf, 0);
    oneId = 0;
    leaf_Get(*stLeaf, res);
    res_num = stLeaf->num;
    delete stLeaf;
  } else {
    STLeaf* stLeaf = getSTLeaf(nonLeaf, nonLeaf.num-1);
    oneId = nonLeaf.num-1;
    leaf_Get(*stLeaf, res);
    res_num = stLeaf->num;
    delete stLeaf;
  }
}

void Table::ST_finder::find_One(LeafKey* res, int& res_num, int id) {
  STLeaf* stLeaf = getSTLeaf(*to_find_nonleaf, id);
  leaf_Get(*stLeaf, res);
  res_num = stLeaf->num;
  delete stLeaf;
}

void Table::ST_finder::sort() {
  for (int i=0;i<to_find_nonleaf->num;i++) {
    if(i!=oneId) {
      cod co_d = to_find_nonleaf->Get_co_d(i);
      if (co_d) {
        saxt_only saxtOnly;
        to_find_nonleaf->SetSaxt(saxtOnly.asaxt, to_find_nonleaf->Get_lsaxt(i));
        has_cod.emplace_back(minidist_paa_to_saxt(paa, saxtOnly.asaxt, co_d), i);
      } else {
        no_has_cod.push_back(i);
      }
    }
  }
  std::sort(has_cod.begin(), has_cod.end());
}

cod Table::ST_finder::get_co_d_from_saxt(saxt a, saxt b, cod pre_d) {
  int d = 0;
  for(; d<Bit_cardinality - pre_d; d++){
    if (a[d] != b[d]) return d;
  }
  return pre_d + d;
}

bool Table::ST_finder::saxt_cmp(saxt a, saxt b, cod co_d) {
  for (int d = 0;d<Bit_cardinality - co_d;d++) {
    if (a[d] != b[d]) return a[d] < b[d];
  }
  return true;
}

int Table::ST_finder::whereofKey(saxt lsaxt, saxt rsaxt, saxt leafKey,
                                 cod co_d) {
  if (saxt_cmp(leafKey, rsaxt, co_d) && saxt_cmp(lsaxt, leafKey, co_d)) return 0;
  if (saxt_cmp(leafKey, lsaxt, co_d)) return -1;
  return 1;
}

STLeaf* Table::ST_finder::getSTLeaf(STNonLeaf& nonLeaf, int i) {
  Slice slice;
  STpos sTpos = nonLeaf.Get_pos(i);
  size_t stLeaf_size = sTpos.GetSize();
//  out("geiSTLeaf");
  STLeaf* stLeaf = new STLeaf(nonLeaf.Getnum(i), nonLeaf.Get_co_d(i), stLeaf_size);
  stLeaf->Setprefix(nonLeaf.prefix, nonLeaf.Get_lsaxt(i), nonLeaf.co_size, saxt_size - nonLeaf.co_size);
  rep_->file->Read(sTpos.GetOffset(), stLeaf_size,
                   &slice, stLeaf->rep);
  stLeaf->Setrep(slice.data());
//  out("geiSTLeaf完毕");
  return stLeaf;
}

STNonLeaf* Table::ST_finder::getSTNonLeaf(STNonLeaf& nonLeaf, int i) {
  Slice slice;
  STpos sTpos = nonLeaf.Get_pos(i);
  size_t stNonLeaf_size = sTpos.GetSize();
//  out("getSTNonLeaf==================");
//  out(stNonLeaf_size);
//  out(sTpos.GetOffset());
//  out(nonLeaf.Getnum(1));
//  out((int)nonLeaf.Get_co_d(1));
  STNonLeaf* stNonLeaf = new STNonLeaf(nonLeaf.Getnum(i), nonLeaf.Get_co_d(i), stNonLeaf_size);
  stNonLeaf->Setprefix(nonLeaf.prefix, nonLeaf.Get_lsaxt(i), nonLeaf.co_size, saxt_size - nonLeaf.co_size);

  rep_->file->Read(sTpos.GetOffset(), stNonLeaf_size,
                   &slice, stNonLeaf->rep);
  stNonLeaf->Setrep(slice.data());
  stNonLeaf->Setisleaf();
//  out(stNonLeaf->Getnum(1));
//  saxt_print(stNonLeaf->Get_lsaxt(1), stNonLeaf->prefix, stNonLeaf->co_d);
//  saxt_print(stNonLeaf->Get_rsaxt(1), stNonLeaf->prefix, stNonLeaf->co_d);
  return stNonLeaf;
}


Table::ST_Iter::ST_Iter(Table* table) :rep_(table->rep_),top(0),leaftop(0),stLeaf(sizeof(Leaf)) {
  st_nonleaf_stack.push_back(rep_->stNonLeaf);
//  out("root");
//  saxt_print(rep_->stNonLeaf->prefix);
//  saxt_print(rep_->rsaxt);

  memset(nonleaftops, -1, sizeof nonleaftops);
  LeafKey l;
  next(l);
  leaftop--;
}

bool Table::ST_Iter::next(LeafKey& res) {
//  out("next");
//  out(leaftop);
//  out(stLeaf.num);
  while(++leaftop >= stLeaf.num) {
    while (top >= 0) {
//      out("top"+to_string(top));
//      out(st_nonleaf_stack[top]->isleaf);
//      out(st_nonleaf_stack[top]->num);
      if (st_nonleaf_stack[top]->isleaf) {
        if (++nonleaftops[top] < st_nonleaf_stack[top]->num) {
          //只换叶节点
//          out("叶");
          getSTLeaf();
          res.Set1(stLeaf.prefix, stLeaf.co_size);
//          res.setAsaxt(stLeaf.prefix);
//          out("有stleaf");
//          out((int)stLeaf.co_d);
//          saxt_print(stLeaf.prefix);
//          saxt_print((saxt)stLeaf.Get_rep(0), stLeaf.prefix, stLeaf.co_d);
//          saxt_print((saxt)stLeaf.Get_rep(stLeaf.num-1), stLeaf.prefix, stLeaf.co_d);
          break;
        } else {
          top--;
        }
      } else {
        if (++nonleaftops[top] < st_nonleaf_stack[top]->num) {
//          out("非叶");
          getSTNonLeaf();
//          out("STNonLeaf");
//          out((int)st_nonleaf_stack[top]->co_d);
//          saxt_print(st_nonleaf_stack[top]->prefix);
        } else {
          top--;
        }
      }
    }
    if (top < 0) {
      //遍历完了
//      out("cuole");
      return false;
    }
  }

//  res.Set1(stLeaf.prefix, stLeaf.co_size);
  res.Set2(stLeaf.Get_rep(leaftop), stLeaf.co_size, stLeaf.noco_size);
//  res.Set(stLeaf.prefix, stLeaf.Get_rep(leaftop), stLeaf.co_size, stLeaf.noco_size);
  return true;
}

void Table::ST_Iter::getSTLeaf() {
  STNonLeaf &nonLeaf = *st_nonleaf_stack[top];
  int i = nonleaftops[top];
  Slice slice;
  STpos sTpos = nonLeaf.Get_pos(i);
  size_t stLeaf_size = sTpos.GetSize();
//  out((int)nonLeaf.co_d);
//  out((int)nonLeaf.Get_co_d(i));
  stLeaf.Set(nonLeaf.Getnum(i), nonLeaf.Get_co_d(i));
  stLeaf.Setprefix(nonLeaf.prefix, nonLeaf.Get_lsaxt(i), nonLeaf.co_size, saxt_size - nonLeaf.co_size);
  if (stLeaf.ismmap) stLeaf.Setnewroom(sizeof(Leaf));
  rep_->file->Read(sTpos.GetOffset(), stLeaf_size,
                   &slice, stLeaf.rep);
  stLeaf.Setrep(slice.data());
  leaftop = -1;

}

void Table::ST_Iter::getSTNonLeaf() {
  STNonLeaf &nonLeaf = *st_nonleaf_stack[top];
  int i = nonleaftops[top];
  Slice slice;
  STpos sTpos = nonLeaf.Get_pos(i);
  size_t stNonLeaf_size = sTpos.GetSize();
  top++;
  if (top < st_nonleaf_stack.size()){
    if (st_nonleaf_stack[top]->ismmap) st_nonleaf_stack[top]->Setnewroom(sizeof(NonLeaf));
    st_nonleaf_stack[top]->Set(nonLeaf.Getnum(i), nonLeaf.Get_co_d(i), stNonLeaf_size);
    st_nonleaf_stack[top]->Setprefix(nonLeaf.prefix, nonLeaf.Get_lsaxt(i), nonLeaf.co_size, saxt_size - nonLeaf.co_size);
  } else {
    STNonLeaf* stNonLeaf = new STNonLeaf(nonLeaf.Getnum(i), nonLeaf.Get_co_d(i), sizeof(NonLeaf));
    stNonLeaf->Setprefix(nonLeaf.prefix, nonLeaf.Get_lsaxt(i), nonLeaf.co_size, saxt_size - nonLeaf.co_size);
    stNonLeaf->size = stNonLeaf_size;
    st_nonleaf_stack.push_back(stNonLeaf);
  }
  rep_->file->Read(sTpos.GetOffset(), stNonLeaf_size,
                   &slice, st_nonleaf_stack[top]->rep);
  st_nonleaf_stack[top]->Setrep(slice.data());
  st_nonleaf_stack[top]->Setisleaf();
  nonleaftops[top] = -1;
}

Table::ST_Iter::~ST_Iter() {
  for(int i=1;i<st_nonleaf_stack.size();i++) delete st_nonleaf_stack[i];
}

void Table::ST_Iter::setPrefix(LeafKey& res) {
  res.Set1(stLeaf.prefix, stLeaf.co_size);
}

}  // namespace leveldb
