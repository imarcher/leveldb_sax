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
#include <chrono>

#define BOOST_NO_EXCEPTIONS
#include <boost/throw_exception.hpp>
void boost::throw_exception(std::exception const & e){
//do nothing
}

#define over(a) std::cout<<a<<" finished"<<std::endl

int datanum = 1000 * 1000;
leveldb::DB* db;
leveldb::Options options;
std::chrono::steady_clock::time_point t1;
std::chrono::steady_clock::time_point t2;


void test_init(vector<LeafTimeKey>& leafKeys){


  db->Init(leafKeys.data(), leafKeys.size());
  over("init");
}


void test_put(vector<LeafTimeKey>& leafKeys){
  leveldb::WriteOptions writeOptions;
  int k=0;
  for(int i=0;i<1000000;i++) {
    db->Put(writeOptions, leafKeys[i]);
//    cout<<"finish:"<<k++<<endl;
  }
  over("put============");
  for(int i=0;i<1000000;i++) {
    db->Put(writeOptions, leafKeys[i]);
//    cout<<"finish:"<<k++<<endl;
  }
  over("put");
}


void test_put_multithread(vector<LeafTimeKey>& leafKeys){

  ThreadPool pool(16);
  leveldb::WriteOptions writeOptions;
  t1 = std::chrono::steady_clock::now();
  for(int i=0;i<1000000;i++) {
      pool.enqueue(std::bind(&leveldb::DB::Put, db, writeOptions, leafKeys[i]));
  }
  for(int i=0;i<1000000;i++) {
    pool.enqueue(std::bind(&leveldb::DB::Put, db, writeOptions, leafKeys[i]));
  }
  over("put_multithread");
//  ThreadPool pool(16);
//  for(int i=0;i<10;i++) pool.enqueue(std::bind(&ea::add, &aa));
}


void test_rebalance_small(vector<LeafTimeKey>& leafKeys){
  for(int i=920000;i<1000000;i++){
    db->Put(leveldb::WriteOptions(), leafKeys[920000+i%30]);
  }
  over("rebalance_small");
}


void test_st_compaction_0(vector<LeafTimeKey>& leafKeys){
  ThreadPool pool(8);
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

void test_get_mem(vector<LeafTimeKey>& leafKeys){
  vector<LeafKey> res;


  saxt_print(res[0].asaxt);
  saxt_print(res.back().asaxt);
  saxt_print(leafKeys[1000000-1].leafKey.asaxt);
  out("size:"+to_string(res.size()));
  over("get_mem");
}

void test_get_st(vector<LeafTimeKey>& leafKeys){
  LeafKey& tofind_leafkey = leafKeys[903000].leafKey;
  vector<LeafKey> res;

//  for(int i=0;i<res.size();i++){
//    saxt_print(res[i].asaxt);
//  }
  out("找到的最小和最大==========");
  out("size:"+to_string(res.size()));
  saxt_print(res[0].asaxt);
  saxt_print(res.back().asaxt);
  out("要找的");
  saxt_print(tofind_leafkey.asaxt);
  over("get_st");
}

int main(){


  char * filename = "../../data/saxt1000.bin";
  FILE * data_file;
  data_file = fopen (filename,"r");

  vector<LeafTimeKey> leafKeys;
  for(int i=0; i < datanum; i ++) {
    leafKeys.emplace_back();
    fread(leafKeys.back().leafKey.asaxt, sizeof(saxt_type), Bit_cardinality, data_file);
  }

  string dir = "./testdb";

  options.create_if_missing = true;
  leveldb::Status status = leveldb::DB::Open(options, dir, nullptr, &db);

  assert(status.ok());
  test_init(leafKeys);
  sleep(3);
  //一组测试
  t1 = std::chrono::steady_clock::now();
  test_put(leafKeys);
//  test_put_multithread(leafKeys);
//  test_rebalance_small(leafKeys);
//  test_st_compaction_0(leafKeys);
  t2 = std::chrono:: steady_clock::now();
  std::cout << std::chrono::duration_cast<std::chrono::milliseconds>( t2-t1 ).count() <<"ms"<< std::endl;


//  test_get_mem(leafKeys);
//  sleep(10);
//  test_get_st(leafKeys);
//
  sleep(30);
  out("finished");
  delete db;
}
