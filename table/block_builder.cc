// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// BlockBuilder generates blocks where keys are prefix-compressed:
//
// When we store a key, we drop the prefix shared with the previous
// string.  This helps reduce the space requirement significantly.
// Furthermore, once every K keys, we do not apply the prefix
// compression and store the entire key.  We call this a "restart
// point".  The tail end of the block stores the offsets of all of the
// restart points, and can be used to do a binary search when looking
// for a particular key.  Values are stored as-is (without compression)
// immediately following the corresponding key.
//
// An entry for a particular key-value pair has the form:
//     shared_bytes: varint32
//     unshared_bytes: varint32
//     value_length: varint32
//     key_delta: char[unshared_bytes]
//     value: char[value_length]
// shared_bytes == 0 for restart points.
//
// The trailer of the block has the form:
//     restarts: uint32[num_restarts]
//     num_restarts: uint32
// restarts[i] contains the offset within the block of the ith restart point.

#include "table/block_builder.h"

#include <algorithm>
#include <cassert>

#include "leveldb/comparator.h"
#include "leveldb/options.h"
#include "util/coding.h"

namespace leveldb {


BlockBuilder::BlockBuilder(const Options* options)
    : options_(options), finished_(false) {
  assert(options->block_restart_interval >= 1);
}

void BlockBuilder::Reset() {
  buffer_.clear();
  finished_ = false;
}

size_t BlockBuilder::CurrentSizeEstimate() const {
  return buffer_.size();
}

Slice BlockBuilder::Finish() {
  // Append restart array
  finished_ = true;
  return Slice(buffer_);
}

//size_t BlockBuilder::Add(const Slice& key, const Slice& value) {
//  Slice last_key_piece(last_key_);
//  assert(!finished_);
//  assert(counter_ <= options_->block_restart_interval);
//  assert(buffer_.empty()  // No values yet?
//         || options_->comparator->Compare(key, last_key_piece) > 0);
//  size_t shared = 0;
//  if (counter_ < options_->block_restart_interval) {
//    // See how much sharing to do with previous string
//    const size_t min_length = std::min(last_key_piece.size(), key.size());
//    while ((shared < min_length) && (last_key_piece[shared] == key[shared])) {
//      shared++;
//    }
//  } else {
//    // Restart compression
//    restarts_.push_back(buffer_.size());
//    counter_ = 0;
//  }
//  const size_t non_shared = key.size() - shared;
//
//  // Add "<shared><non_shared><value_size>" to buffer_
//  PutVarint32(&buffer_, shared);
//  PutVarint32(&buffer_, non_shared);
//  PutVarint32(&buffer_, value.size());
//
//  // Add string delta to buffer_ followed by value
//  buffer_.append(key.data() + shared, non_shared);
//  buffer_.append(value.data(), value.size());
//
//  // Update state
//  last_key_.resize(shared);
//  last_key_.append(key.data() + shared, non_shared);
//  assert(Slice(last_key_) == key);
//  counter_++;
//}


void BlockBuilder::Add(Leaf* leaf) {
  cod co_d = leaf->co_d;
  size_t co_saxt_size = co_d * sizeof(saxt_type);
  size_t noco_saxt_size = saxt_size - co_saxt_size + 8;
  //把共享的压缩掉
  for(int i=0;i<leaf->num;i++){
    buffer_.append(((char*)leaf->leafKeys[i].asaxt) + co_saxt_size, noco_saxt_size);
  }

}

void BlockBuilder::Add(NonLeaf* nonLeaf, vector<void*> new_p) {
  cod co_d = nonLeaf->co_d;
  size_t co_saxt_size = co_d * sizeof(saxt_type);
  size_t noco_saxt_size = saxt_size - co_saxt_size;
  for(int i=0;i<nonLeaf->num;i++){
    NonLeafKey* nonLeafKey = nonLeaf->nonLeafKeys + i;
    STkeyinfo stkeyinfo(nonLeafKey->co_d, nonLeafKey->num);
    buffer_.append((char*)&stkeyinfo, 2);
    buffer_.append(((char*)nonLeafKey->lsaxt) + co_saxt_size, noco_saxt_size);
    buffer_.append(((char*)nonLeafKey->rsaxt) + co_saxt_size, noco_saxt_size);
    buffer_.append((char*)(new_p.data()+i*8), 8);
  }
  buffer_.append(nonLeaf->isleaf,1);
}

}  // namespace leveldb
