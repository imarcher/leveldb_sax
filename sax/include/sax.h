//
//  sax.h
//  isaxlib
//
//  Created by Kostas Zoumpatianos on 3/10/12.
//  Copyright 2012 University of Trento. All rights reserved.
//

#ifndef isaxlib_sax_h
#define isaxlib_sax_h

#include "config.h"
#include "globals.h"
#include "ts.h"
#include <cstring>


enum response sax_from_ts(ts_type *ts_in, sax_type *sax_out);
enum response saxt_from_ts(ts_type *ts_in, saxt_type *sax_out);

void sax_print(sax_type *sax, int segments, int bit_cardinality);
void saxt_print(saxt_type *saxt);
void saxt_print(saxt_type *saxt, saxt_type *prefix, cod co_d);
void printbin(unsigned long long n, int size);
void serial_printbin (unsigned long long n, int size);
int compare(const void *a, const void *b);
float minidist_paa_to_isax(ts_type *paa, sax_type *sax, sax_type *sax_cardinalities,
                           sax_type max_bit_cardinality,
                           sax_type max_cardinality,
                           int number_of_segments,
                           int min_val, int max_val, float ratio_sqrt);
ts_type minidist_paa_to_isax_raw(ts_type *paa, sax_type *sax,
			       sax_type *sax_cardinalities,
			       sax_type max_bit_cardinality,
			       int max_cardinality,
			       int number_of_segments,
			       int min_val,
			       int max_val,
			       ts_type ratio_sqrt) ;

//分开
enum response paa_from_ts(ts_type *ts_in, ts_type *paa_out);
enum response sax_from_paa (ts_type *paa, sax_type *sax);
enum response saxt_from_sax(sax_type *sax_in, saxt_type *saxt_out);
enum response sax_from_saxt(sax_type *saxt_in, saxt_type *sax_out);
enum response saxt_copy(saxt_type *saxt_out, saxt_type *saxt_in);


//获得两个saxt的相聚度和公共前缀
cod get_co_d_from_saxt(saxt a, saxt b);
//获得两个saxt的相聚度和公共前缀,但有比较的起始d
cod get_co_d_from_saxt(saxt a, saxt b, cod pre_d);
//比较在第d个段是否相等
bool compare_saxt_d(saxt a, saxt b, cod d);
//比较saxt是否相等
bool compare_saxt(saxt a, saxt b);







#endif
