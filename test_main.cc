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

int datanum = 1000 * 1000;
leveldb::DB* db;
leveldb::Options options;





void test_init(vector<LeafKey>& leafKeys){

  vector<NonLeafKey> nonLeafKeys;
  db->InitLeaf(leafKeys, nonLeafKeys);
  db->InitDranges(nonLeafKeys, datanum);
}

void test_put(vector<LeafKey>& leafKeys){
//
//  threadPool pool(6);


  out("==================");
  int k=0;
  for(int i=0;i<90;i++){
    db->Put(leveldb::WriteOptions(), leafKeys[i%30]);
//    cout<<"finish:"<<k++<<endl;
  }
//  int j;
//  for(int i=0;i<leafKeys.size();i=j) {
//    for(int j=i;j<leafKeys.size();j+=Table_maxnum)
//      pool.append(std::bind(&leveldb::DB::Put, db, writeOptions, leafKeys[j]));
//  }
  out("over");
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
  test_put(leafKeys);



  delete db;
}
