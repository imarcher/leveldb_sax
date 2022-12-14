// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// WriteBatch::rep_ :=
//    sequence: fixed64
//    count: fixed32
//    data: record[count]
// record :=
//    kTypeValue varstring varstring         |
//    kTypeDeletion varstring
// varstring :=
//    len: varint32
//    data: uint8[len]

#include "leveldb/write_batch.h"

#include "db/dbformat.h"

#include "db/write_batch_internal.h"
#include <util/mutexlock.h>



#include "util/coding.h"
#include "iostream"


namespace leveldb {

// WriteBatch header has an 8-byte sequence number followed by a 4-byte count.
static const size_t kHeader = 12;

WriteBatch::WriteBatch() { Clear(); }

WriteBatch::~WriteBatch() = default;

WriteBatch::Handler::~Handler() = default;

void WriteBatch::Clear() {
  rep_.clear();
  rep_.resize(kHeader);
}

size_t WriteBatch::ApproximateSize() const { return rep_.size(); }

Status WriteBatch::Iterate(Handler* handler, int memNum) const {
  Slice input(rep_);
  if (input.size() < kHeader) {
    return Status::Corruption("malformed WriteBatch (too small)");
  }

  input.remove_prefix(kHeader);
  //从0开始
  int found = -1;
  while (!input.empty()) {
    found++;
    if (input.size() >= sizeof(LeafTimeKey)) {
      //先改变memtable的时间
      LeafTimeKey* leafTimeKey = (LeafTimeKey*)input.data();
      handler->SetTime(leafTimeKey->keytime);
      //我们添加进table 如果满了，重组
      if (!handler->Put(leafTimeKey->leafKey)) {
        out("重组");
        out(memNum + found + 1);
        int Nt = memNum + found + 1;
        int nt = max(Nt * Leaf_maxnum / Table_maxnum, Leaf_maxnum_rebalance);

        handler->Rebalance(nt, nt/2, Nt);
      }
      input.remove_prefix(sizeof(LeafTimeKey));
    } else {
      return Status::Corruption("bad WriteBatch Put");
    }
  }
  found++;
  if (found != WriteBatchInternal::Count(this)) {
    return Status::Corruption("WriteBatch has wrong count");
  } else {
    return Status::OK();
  }
}

int WriteBatchInternal::Count(const WriteBatch* b) {
  return DecodeFixed32(b->rep_.data() + 8);
}

void WriteBatchInternal::SetCount(WriteBatch* b, int n) {
  EncodeFixed32(&b->rep_[8], n);
}

SequenceNumber WriteBatchInternal::Sequence(const WriteBatch* b) {
  return SequenceNumber(DecodeFixed64(b->rep_.data()));
}

void WriteBatchInternal::SetSequence(WriteBatch* b, SequenceNumber seq) {
  EncodeFixed64(&b->rep_[0], seq);
}

void WriteBatch::Put(const LeafTimeKey& key) {
  WriteBatchInternal::SetCount(this, WriteBatchInternal::Count(this) + 1);
  rep_.append((char*)&key, sizeof(LeafTimeKey));
}

void WriteBatch::Delete(const Slice& key) {
  WriteBatchInternal::SetCount(this, WriteBatchInternal::Count(this) + 1);
  rep_.push_back(static_cast<char>(kTypeDeletion));
  PutLengthPrefixedSlice(&rep_, key);
}

void WriteBatch::Append(const WriteBatch& source) {
  WriteBatchInternal::Append(this, &source);
}

namespace {
class MemTableInserter : public WriteBatch::Handler {
 public:
//  SequenceNumber sequence_;
  MemTable*& mem_;
  port::Mutex* mutex_;
  mem_version_set* versionSet;
  int memId;
  MemTableInserter(MemTable*& mem, port::Mutex* mutex, int memId, mem_version_set* versionSet)
      : mem_(mem), mutex_(mutex), memId(memId), versionSet(versionSet){}

  bool Put(LeafKey& key) override {
    return mem_->Add(key);
  }

  void SetTime(ts_time newtime) override {
    if (mem_->endTime < newtime) mem_->endTime = newtime;
    else if (mem_->startTime > newtime) mem_->startTime = newtime;
  }


  void Rebalance(int tmp_leaf_maxnum, int tmp_leaf_minnum, int Nt) override {
    MemTable *oldmem = mem_;
    MemTable *newmem = oldmem->Rebalance(tmp_leaf_maxnum, tmp_leaf_minnum, Nt);
    MutexLock l(mutex_);
    mem_ = newmem;
    versionSet->newversion_small(mem_, memId);
  }


  void Delete(const Slice& key) override {
//    mem_->Add(sequence_, kTypeDeletion, key, Slice());
//    sequence_++;
  }
};
}  // namespace

Status WriteBatchInternal::InsertInto(Writes_vec& b, MemTable*& memtable, port::Mutex* mutex, int memNum, int memId, mem_version_set* versionSet) {
  MemTableInserter inserter(memtable, mutex, memId, versionSet);
  int found = 0;
  LeafTimeKey *data = b.keys;
  for(;found<b.size_;found++){
    inserter.SetTime(data[found].keytime);
    //我们添加进table 如果满了，重组
    if (!inserter.Put(data[found].leafKey)) {
      out("重组");
      out(memNum + found + 1);
      int Nt = memNum + found + 1;
      int nt = max(Nt * Leaf_maxnum / Table_maxnum, Leaf_maxnum_rebalance);

      inserter.Rebalance(nt, nt/2, Nt);
    }
  }
  return Status();
}

Status WriteBatchInternal::InsertInto(LeafTimeKey& b, MemTable*& memtable, port::Mutex* mutex, int memNum, int memId, mem_version_set* versionSet) {
  MemTableInserter inserter(memtable, mutex, memId, versionSet);
  int found = 1;
  inserter.SetTime(b.keytime);
  //我们添加进table 如果满了，重组
  if (!inserter.Put(b.leafKey)) {
    out("重组");
    out(memNum + found);
    int Nt = memNum + found;
    int nt = max(Nt * Leaf_maxnum / Table_maxnum, Leaf_maxnum_rebalance);
    inserter.Rebalance(nt, nt/2, Nt);
  }

  return Status();
}

void WriteBatchInternal::SetContents(WriteBatch* b, const Slice& contents) {
  assert(contents.size() >= kHeader);
  b->rep_.assign(contents.data(), contents.size());
}

void WriteBatchInternal::Append(WriteBatch* dst, const WriteBatch* src) {
  SetCount(dst, Count(dst) + Count(src));
  assert(src->rep_.size() >= kHeader);
  dst->rep_.append(src->rep_.data() + kHeader, src->rep_.size() - kHeader);
}


}  // namespace leveldb
