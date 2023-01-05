//
//  defines.h
//  isaxlib
//
//  Created by Kostas Zoumpatianos on 3/19/12.
//  Copyright 2012 University of Trento. All rights reserved.
//



#include "bitset"
#include "config.h"
#include "iostream"
#include <cassert>
#ifndef isax_globals_h
#define isax_globals_h

// 0, 1, 2 代表 一个，一部分，一个叶
#define lookupi 2
// 初始化时存入st
#define init_st 1

///// TYPES /////
#define out(a) std::cout<<a<<std::endl
//这里基数256变为512则用short
//typedef unsigned short sax_type;
typedef unsigned char sax_type;
typedef sax_type* sax;


//段数为16，为8变为char
//typedef unsigned short saxt_type;
typedef unsigned char saxt_type;
typedef saxt_type* saxt;
typedef saxt_type* saxt_prefix;
typedef float ts_type;
typedef ts_type *ts;
typedef time_t ts_time;

typedef unsigned char cod;

#define Cardinality 256
#define Bit_cardinality 8
#define Segments 8
#define nchuw 32 // Ts_length / Segments
#define Ts_values_per_segment 32
#define Ts_length 256
#define Leaf_maxnum 64
#define Leaf_minnum 32
//最小
#define Leaf_maxnum_rebalance 10
#define Leaf_minnum_rebalance 5


//一个memtable存的数量
#define Table_maxnum 100000



//超过这个重构叶结点
static const int Leaf_rebuildnum = Leaf_maxnum * 1.5;

//static int cardinality = 256;
//static int bit_cardinality = 8;
//static int segments = 8;

static const int sax_offset = ((Cardinality - 1) * (Cardinality - 2)) / 2;
static int sax_offset_i[Bit_cardinality+1] = {0,0,3,21,105,465,1953,8001,32385};
static int cardinality_1_i[Bit_cardinality+1] = {0,1,3,7,15,31,63,127,255};

static const size_t saxt_size = sizeof(saxt_type)*Bit_cardinality;


static const size_t send_size1 = 1+sizeof(int)*2+sizeof(uint64_t)+saxt_size*2+sizeof(ts_time)*2;
static const size_t send_size2 = 1+sizeof(int)*3;
static const size_t send_size2_del = saxt_size*2 + sizeof(ts_time)*2;
static const size_t send_size2_add = sizeof(uint64_t) + saxt_size*2 + sizeof(ts_time)*2;

typedef struct {
  ts_type ts[Ts_length];
} ts_only;

typedef struct {
  saxt_type asaxt[Bit_cardinality];
} saxt_only;

typedef struct {
  ts_type apaa[Segments];
} paa_only;

typedef struct {
  ts_type ts[Ts_length];
  ts_time tsTime;
} tsKey;

typedef struct {
  ts_type ts[Ts_length];
  ts_time tsTime;
  saxt_type asaxt[Bit_cardinality];
} putKey;

typedef struct {
  ts_type ts[Ts_length];
  ts_time startTime;
  ts_time endTime;
} aquery_rep;

typedef struct {
  aquery_rep rep;
  int k;
  ts_type paa[Segments];
  saxt_type asaxt[Bit_cardinality];
} aquery;





typedef struct ares{
  tsKey atskey;
  float dist;

  bool operator< (const ares& a) const {
    return dist < a.dist;
  }
  bool operator> (const ares& a) const {
    return dist > a.dist;
  }
} ares;

typedef std::pair<float, void*> dist_p;





typedef unsigned long long file_position_type;
typedef unsigned long long root_mask_type;

enum response {OUT_OF_MEMORY_FAILURE, FAILURE, SUCCESS};
enum insertion_mode {PARTIAL = 1,
                     TMP = 2,
                     FULL = 4,
                     NO_TMP = 8};

enum buffer_cleaning_mode {FULL_CLEAN, TMP_ONLY_CLEAN, TMP_AND_TS_CLEAN};
enum node_cleaning_mode {DO_NOT_INCLUDE_CHILDREN = 0,
                         INCLUDE_CHILDREN = 1};

//
/////// DEFINITIONS /////
//#define MINVAL -2000000
//#define MAXVAL 2000000
//#define DELIMITER ' '
//#define TRUE 1
//#define FALSE 0
//#define BUFFER_REALLOCATION_RATE  2
//
/////// GLOBAL VARIABLES /////
//int FLUSHES;
//
/////// MACROS /////
//#define CREATE_MASK(mask, index, sax_array)\
//	int mask__i; \
//	for (mask__i=0; mask__i < index->settings->paa_segments; mask__i++) \
//		if(index->settings->bit_masks[index->settings->sax_bit_cardinality - 1] & sax_array[mask__i]) \
//			mask |= index->settings->bit_masks[index->settings->paa_segments - mask__i - 1];
//
/////// BNECHMARKING /////
////#ifdef BENCHMARK
//		#include <time.h>
//		#include <sys/time.h>
//
//        double tS;
//        double tE;
//
//        struct timeval total_time_start;
//        struct timeval parse_time_start;
//        struct timeval input_time_start;
//        struct timeval output_time_start;
//        struct timeval load_node_start;
//        struct timeval current_time;
//        struct timeval fetch_start;
//        struct timeval fetch_check_start;
//        double total_input_time;
//        double load_node_time;
//        double total_output_time;
//        double total_parse_time;
//        double total_time;
//
///*
//        int total_tree_nodes;
//        int loaded_nodes;
//        int checked_nodes;
//        file_position_type loaded_records;
//*/
//
//
//        struct timeval partial_time_start;
//        struct timeval partial_input_time_start;
//        struct timeval partial_output_time_start;
//        struct timeval partial_load_node_time_start;
//
//        double partial_time;
//        double partial_input_time;
//        double partial_output_time;
//        double partial_load_node_time;
//
//        unsigned long long partial_seq_input_count;
//        unsigned long long partial_seq_output_count;
//        unsigned long long partial_rand_input_count;
//        unsigned long long partial_rand_output_count;
//
//        unsigned long total_nodes_count;
//        unsigned long leaf_nodes_count;
//        unsigned long empty_leaf_nodes_count;
//        unsigned long loaded_nodes_count;
//        unsigned long checked_nodes_count;
//        unsigned long loaded_ts_count;
//        unsigned long checked_ts_count;
//        unsigned long total_ts_count;
//        unsigned long total_queries_count;
//        ts_type total_node_tlb;
//        ts_type total_data_tlb;
//
//        #define INIT_STATS() total_input_time = 0;\
//                             total_output_time = 0;\
//                             total_time = 0;\
//                             total_parse_time = 0;\
//                             load_node_time= 0;\
//			     partial_time = 0;\
//			     partial_input_time = 0;\
//			     partial_output_time = 0;\
//			     partial_load_node_time = 0;\
//                             partial_seq_input_count = 0;\
//                             partial_seq_output_count = 0;\
//                             partial_rand_input_count = 0;\
//                             partial_rand_output_count = 0;\
//			     total_nodes_count = 0;\
//			     leaf_nodes_count = 0;\
//			     empty_leaf_nodes_count = 0;\
//			     loaded_nodes_count = 0;\
//			     loaded_ts_count = 0;\
//			     checked_ts_count = 0;\
//                             checked_nodes_count = 0;\
//			     total_ts_count = 0;
//
//
///*
//        #define INIT_STATS() total_input_time = 0;\
//                             total_output_time = 0;\
//                             total_time = 0;\
//                             total_parse_time = 0;\
//                             total_tree_nodes = 0;\
//                             loaded_nodes = 0;\
//                             checked_nodes = 0;\
//                             load_node_time=0;\
//                             loaded_records = 0; \
//        printf("input\t output\t parse\t nodes\t checked_nodes\t loaded_nodes\t loaded_records\t distance\t load_node_time\t total\n");
//        #define PRINT_STATS(result_distance) printf("%lf\t %lf\t %lf\t %d\t %d\t %d\t %lld\t %lf\t %lf\t %lf\n", \
//        total_input_time, total_output_time, \
//        total_parse_time, total_tree_nodes, \
//        checked_nodes, loaded_nodes, \
//        loaded_records, result_distance, load_node_time, total_time);
//
//        #define COUNT_NEW_NODE() total_tree_nodes++;
//        #define COUNT_LOADED_NODE() loaded_nodes++;
//        #define COUNT_CHECKED_NODE() checked_nodes++;
//
//        #define COUNT_LOADED_RECORD() loaded_records++;
//
//        #define COUNT_INPUT_TIME_START gettimeofday(&input_time_start, NULL);
//        #define COUNT_OUTPUT_TIME_START gettimeofday(&output_time_start, NULL);
//        #define COUNT_TOTAL_TIME_START gettimeofday(&total_time_start, NULL);
//        #define COUNT_PARSE_TIME_START gettimeofday(&parse_time_start, NULL);
//        #define COUNT_LOAD_NODE_START gettimeofday(&load_node_start, NULL);
//        #define COUNT_INPUT_TIME_END  gettimeofday(&current_time, NULL); \
//                                      tS = input_time_start.tv_sec*1000000 + (input_time_start.tv_usec); \
//                                      tE = current_time.tv_sec*1000000 + (current_time.tv_usec); \
//                                      total_input_time += (tE - tS);
//        #define COUNT_OUTPUT_TIME_END gettimeofday(&current_time, NULL); \
//                                      tS = output_time_start.tv_sec*1000000 + (output_time_start.tv_usec); \
//                                      tE = current_time.tv_sec*1000000  + (current_time.tv_usec); \
//                                      total_output_time += (tE - tS);
//        #define COUNT_TOTAL_TIME_END  gettimeofday(&current_time, NULL); \
//                                      tS = total_time_start.tv_sec*1000000 + (total_time_start.tv_usec); \
//                                      tE = current_time.tv_sec*1000000  + (current_time.tv_usec); \
//                                      total_time += (tE - tS);
//        #define COUNT_PARSE_TIME_END  gettimeofday(&current_time, NULL);  \
//                                      tS = parse_time_start.tv_sec*1000000 + (parse_time_start.tv_usec); \
//                                      tE = current_time.tv_sec*1000000  + (current_time.tv_usec); \
//                                      total_parse_time += (tE - tS);
//        #define COUNT_LOAD_NODE_END   gettimeofday(&current_time, NULL);  \
//                                      tS = load_node_start.tv_sec*1000000 + (load_node_start.tv_usec); \
//                                      tE = current_time.tv_sec*1000000  + (current_time.tv_usec); \
//                                      load_node_time += (tE - tS);
//
//*/
//
//
//        #define COUNT_NEW_NODE ++total_nodes_count;
//        #define COUNT_LEAF_NODE ++leaf_nodes_count;
//        #define COUNT_EMPTY_LEAF_NODE ++empty_leaf_nodes_count;
//        #define COUNT_EMPTY_LEAF_NODE_CANCEL --empty_leaf_nodes_count;
//        #define COUNT_TOTAL_TS(num_ts) total_ts_count+=num_ts; //actual ts inserted in index
//
//        #define COUNT_CHECKED_NODE ++checked_nodes_count;
//        #define COUNT_LOADED_NODE ++loaded_nodes_count;
//        #define COUNT_LOADED_TS(num_ts) loaded_ts_count +=num_ts; //ts loaded to answer query
//        #define COUNT_CHECKED_TS(num_ts) checked_ts_count +=num_ts; //ts loaded to answer query
//
//
//      #define RESET_QUERY_COUNTERS() loaded_nodes_count = 0;\
//                                     loaded_ts_count = 0;\
//                                     checked_nodes_count = 0;\
//                                     checked_ts_count = 0;
//
//      #define RESET_PARTIAL_COUNTERS() partial_seq_output_count = 0;\
//                                       partial_seq_input_count = 0;\
//                                       partial_rand_output_count = 0;\
//                                       partial_rand_input_count = 0;\
//				       partial_input_time = 0;\
//				       partial_output_time = 0;\
//				       partial_load_node_time = 0;\
//				       partial_time = 0;
//
//       #define PRINT_QUERY_COUNTERS() printf("loaded_nodes and checked_ts = %lu and %lu\n", loaded_nodes_count, checked_ts_count);
//       #define PRINT_PARTIAL_COUNTERS() printf("seq_output and partial_time = %llu and %f\n",partial_seq_output_count,  partial_time);
//
//        #define COUNT_PARTIAL_SEQ_INPUT ++partial_seq_input_count;
//        #define COUNT_PARTIAL_SEQ_OUTPUT ++partial_seq_output_count;
//        #define COUNT_PARTIAL_RAND_INPUT ++partial_rand_input_count;
//        #define COUNT_PARTIAL_RAND_OUTPUT ++partial_rand_output_count;
//
//
//        #define COUNT_INPUT_TIME_START gettimeofday(&input_time_start, NULL);
//        #define COUNT_OUTPUT_TIME_START gettimeofday(&output_time_start, NULL);
//        #define COUNT_TOTAL_TIME_START gettimeofday(&total_time_start, NULL);
//
//        #define COUNT_PARTIAL_TIME_START gettimeofday(&partial_time_start, NULL);
//        #define COUNT_PARTIAL_INPUT_TIME_START gettimeofday(&partial_input_time_start, NULL);
//        #define COUNT_PARTIAL_OUTPUT_TIME_START gettimeofday(&partial_output_time_start, NULL);
//        #define COUNT_PARTIAL_LOAD_NODE_TIME_START gettimeofday(&partial_load_node_time_start, NULL);
//
//        #define COUNT_PARSE_TIME_START gettimeofday(&parse_time_start, NULL);
//        #define COUNT_LOAD_NODE_START gettimeofday(&load_node_start, NULL);
//        #define COUNT_INPUT_TIME_END  gettimeofday(&current_time, NULL);\
//                                      tS = input_time_start.tv_sec*1000000 + (input_time_start.tv_usec); \
//                                      tE = current_time.tv_sec*1000000 + (current_time.tv_usec); \
//                                      total_input_time += (tE - tS);
//        #define COUNT_OUTPUT_TIME_END gettimeofday(&current_time, NULL); \
//                                      tS = output_time_start.tv_sec*1000000 + (output_time_start.tv_usec); \
//                                      tE = current_time.tv_sec*1000000  + (current_time.tv_usec); \
//                                      total_output_time += (tE - tS);
//        #define COUNT_TOTAL_TIME_END  gettimeofday(&current_time, NULL); \
//                                      tS = total_time_start.tv_sec*1000000 + (total_time_start.tv_usec); \
//                                      tE = current_time.tv_sec*1000000  + (current_time.tv_usec); \
//                                      total_time += (tE - tS);
//        #define COUNT_PARTIAL_INPUT_TIME_END  gettimeofday(&current_time, NULL); \
//                                      tS = partial_input_time_start.tv_sec*1000000 + (partial_input_time_start.tv_usec); \
//                                      tE = current_time.tv_sec*1000000 + (current_time.tv_usec); \
//                                      partial_input_time += (tE - tS);
//        #define COUNT_PARTIAL_LOAD_NODE_TIME_END  gettimeofday(&current_time, NULL); \
//                                      tS = partial_load_node_time_start.tv_sec*1000000 + (partial_load_node_time_start.tv_usec); \
//                                      tE = current_time.tv_sec*1000000 + (current_time.tv_usec); \
//                                      partial_load_node_time += (tE - tS);
//        #define COUNT_PARTIAL_OUTPUT_TIME_END gettimeofday(&current_time, NULL); \
//                                      tS = partial_output_time_start.tv_sec*1000000 + (partial_output_time_start.tv_usec); \
//				      tE = current_time.tv_sec*1000000  + (current_time.tv_usec); \
//                                      partial_output_time += (tE - tS);
//        #define COUNT_PARTIAL_TIME_END  gettimeofday(&current_time, NULL); \
//                                      tS = partial_time_start.tv_sec*1000000 + (partial_time_start.tv_usec); \
//                                      tE = current_time.tv_sec*1000000  + (current_time.tv_usec); \
//                                      partial_time += (tE - tS);
//        #define COUNT_PARSE_TIME_END  gettimeofday(&current_time, NULL);  \
//                                      tS = parse_time_start.tv_sec*1000000 + (parse_time_start.tv_usec); \
//                                      tE = current_time.tv_sec*1000000  + (current_time.tv_usec); \
//                                      total_parse_time += (tE - tS);
//        #define COUNT_LOAD_NODE_END   gettimeofday(&current_time, NULL);  \
//                                      tS = load_node_start.tv_sec*1000000 + (load_node_start.tv_usec); \
//                                      tE = current_time.tv_sec*1000000  + (current_time.tv_usec); \
//                                      load_node_time += (tE - tS);
//
///*
//  #else
//        #define INIT_STATS() ;
//        #define PRINT_STATS() ;
//        #define COUNT_NEW_NODE() ;
//        #define COUNT_CHECKED_NODE();
//        #define COUNT_LOADED_NODE() ;
//        #define COUNT_LOADED_RECORD() ;
//        #define COUNT_INPUT_TIME_START ;
//        #define COUNT_INPUT_TIME_END ;
//        #define COUNT_OUTPUT_TIME_START ;
//        #define COUNT_OUTPUT_TIME_END ;
//        #define COUNT_TOTAL_TIME_START ;
//        #define COUNT_TOTAL_TIME_END ;
//        #define COUNT_PARSE_TIME_START ;
//        #define COUNT_PARSE_TIME_END ;
//    #endif
//
//*/
#endif
