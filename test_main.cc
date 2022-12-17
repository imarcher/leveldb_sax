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
  for(auto item:leafKeys) {
    db->Put(leveldb::WriteOptions(), item);
    out(22);
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
  if (access(dir.c_str(), 0) == 0)
  {
    cout<<dir<<" exists"<<endl;
    cout<<"now delete it"<<endl;
    int flag=rmdir(dir.c_str());
    if (flag == 0)
    {
      cout<<"delete it successfully"<<endl;
    } else {
      cout<<"delete it errorly"<<endl;
    }
  }

  options.create_if_missing = true;
  leveldb::Status status = leveldb::DB::Open(options, dir, &db);

  assert(status.ok());
  test_init(leafKeys);
  test_put(leafKeys);



//  assert(status.ok());

}
