//
// Created by hh on 2022/12/17.
//

#include <cassert>
#include <dirent.h>
#include <iostream>
#include <leveldb/db.h>
#include <string>
#include <zconf.h>

#include "sax.h"
#include "threadPool_2.h"
#include "zsbtree_table.h"

#define over(a) std::cout<<a<<" finished"<<std::endl

int datanum = 1000 * 1000;
leveldb::DB* db;
leveldb::Options options;





void test_init(vector<LeafKey>& leafKeys){

  vector<NonLeafKey> nonLeafKeys;
  db->InitLeaf(leafKeys, nonLeafKeys);
  db->InitDranges(nonLeafKeys, datanum);
  over("init");
}


void test_put(vector<LeafKey>& leafKeys){
  leveldb::WriteOptions writeOptions;
  int k=0;
  for(int i=0;i<100000;i++){
    db->Put(writeOptions, leafKeys[i]);
//    cout<<"finish:"<<k++<<endl;
  }
  over("put");
}


void test_put_multithread(vector<LeafKey>& leafKeys){

  ThreadPool pool(4);
  leveldb::WriteOptions writeOptions;

  for(int i=0;i<10000;i++) {
      pool.enqueue(std::bind(&leveldb::DB::Put, db, writeOptions, leafKeys[i]));
  }
//  int j;
//  for(int i=0;i<leafKeys.size();i=j) {
//    for(int j=i;j<leafKeys.size();j+=Table_maxnum)
//      pool.append(std::bind(&leveldb::DB::Put, db, writeOptions, leafKeys[j]));
//  }
  over("put_multithread");
}


void test_rebalance_small(vector<LeafKey>& leafKeys){
  for(int i=0;i<90;i++){
    db->Put(leveldb::WriteOptions(), leafKeys[i%30]);
  }
  over("rebalance_small");
}


void test_st_compaction_0(vector<LeafKey>& leafKeys){
  ThreadPool pool(6);
  leveldb::WriteOptions writeOptions;

  for(int i=0;i<10000;i++) {
    for(int j=i;j<leafKeys.size();j+=Table_maxnum)
      pool.enqueue(std::bind(&leveldb::DB::Put, db, writeOptions, leafKeys[j]));
  }
//  leveldb::WriteOptions writeOptions;
//  db->Put(writeOptions, leafKeys[0]);
//  db->Put(writeOptions, leafKeys[200000]);
  over("st_compaction_0");
}



int main(){


  char * filename = "../../data/saxt1000.bin";
  FILE * data_file;
  data_file = fopen (filename,"r");

  vector<LeafKey> leafKeys;
  for(int i=0; i < datanum; i ++) {
    leafKeys.emplace_back();
    fread(leafKeys.back().asaxt, sizeof(saxt_type), Bit_cardinality, data_file);
  }

  string dir = "./testdb";

  options.create_if_missing = true;
  leveldb::Status status = leveldb::DB::Open(options, dir, &db);

  assert(status.ok());
  test_init(leafKeys);

  //一组测试
//  test_put(leafKeys);
//  test_put_multithread(leafKeys);
//  test_rebalance_small(leafKeys);
  test_st_compaction_0(leafKeys);


  out("finished");

}
