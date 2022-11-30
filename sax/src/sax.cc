//
//  sax.c
//  isaxlib
//
//  Created by Kostas Zoumpatianos on 3/10/12.
//  Copyright 2012 University of Trento. All rights reserved.
//
//#define DEBUG;
#include "../include/config.h"
#include "../include/globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef VALUES
	#include <values.h>
#include <iostream>

#endif

#include "../include/sax.h"
#include "../include/ts.h"
#include "../include/sax_breakpoints.h"
#include "algorithm"
/** 
 This is used for converting to sax
 */
int compare(const void *a, const void *b)
{
    float * c = (float *) b - 1;
    if (*(float*)a>*(float*)c && *(float*)a<=*(float*)b) {
        //printf("Found %lf between %lf and %lf\n",*(float*)a,*(float*)c,*(float*)b);
        return 0;
    }
    else if (*(float*)a<=*(float*)c) {
        return -1;
    }
    else
    {
        return 1;
    }
}

/** 
 Calculate paa.
 */
enum response paa_from_ts (ts_type *ts_in, ts_type *paa_out) {
    int s, i;
    for (s=0; s<Segments; s++) {
        paa_out[s] = 0;
        for (i=0; i<Ts_values_per_segment; i++) {
            paa_out[s] += ts_in[(s * Ts_values_per_segment)+i];
        }
        paa_out[s] /= Ts_values_per_segment;
    }
    return SUCCESS;
}


enum response sax_from_paa (ts_type *paa, sax_type *sax) {

    //printf("FROM %lf TO %lf\n", sax_breakpoints[offset], sax_breakpoints[offset + cardinality - 2]);
    
    int si;
    for (si=0; si<Segments; si++) {
        sax[si] = 0;
        
        // First object = sax_breakpoints[offset]
        // Last object = sax_breakpoints[offset + cardinality - 2]
        // Size of sub-array = cardinality - 1
        
        float *res = (float *) bsearch(&paa[si], &sax_breakpoints[sax_offset], Cardinality - 1,
                                       sizeof(ts_type), compare);
        if(res != NULL)
	  {
            //sax[si] = (int) (res -  &sax_breakpoints[offset]);
	    sax[si] = (sax_type) (res -  &sax_breakpoints[sax_offset]);
	  }
        else if (paa[si] > 0)
	  {
            sax[si] = Cardinality-1;
	  }
    }

    return SUCCESS;
}

/**
 This function converts a ts record into its sax representation.
 */
enum response sax_from_ts(ts_type *ts_in, sax_type *sax_out)
{
    // Create PAA representation
    float * paa = static_cast<float *>(malloc(sizeof(float) * Segments));
    if(paa == NULL) {
        fprintf(stderr,"error: could not allocate memory for PAA representation.\n");
        return FAILURE;
    }
    
    int s, i;
    for (s=0; s<Segments; s++) {
        paa[s] = 0;
        for (i=0; i<Ts_values_per_segment; i++) {
            paa[s] += ts_in[(s * Ts_values_per_segment)+i];
        }
        paa[s] /= Ts_values_per_segment;
//#ifdef DEBUG
        //printf("%d: %lf\n", s, paa[s]);
//#endif
    }
    
    // Convert PAA to SAX
    // Note: Each cardinality has cardinality - 1 break points if c is cardinality
    //       the breakpoints can be found in the following array positions: 
    //       FROM (c - 1) * (c - 2) / 2 
    //       TO   (c - 1) * (c - 2) / 2 + c - 1

    //printf("FROM %lf TO %lf\n", sax_breakpoints[offset], sax_breakpoints[offset + cardinality - 2]);
    
    int si;
    for (si=0; si<Segments; si++) {
        sax_out[si] = 0;
        
        // First object = sax_breakpoints[offset]
        // Last object = sax_breakpoints[offset + cardinality - 2]
        // Size of sub-array = cardinality - 1
        
        float *res = (float *) bsearch(&paa[si], &sax_breakpoints[sax_offset], Cardinality - 1,
                                       sizeof(ts_type), compare);
        if(res != NULL)
	  {
            //sax_out[si] = (int) (res -  &sax_breakpoints[offset]);
	    sax_out[si] = (sax_type) (res -  &sax_breakpoints[sax_offset]);
	  }
        else if (paa[si] > 0)
	  sax_out[si] = (sax_type) (Cardinality-1);
    }
    
    //sax_print(sax_out, segments, cardinality);
    free(paa);
    return SUCCESS;
}

enum response saxt_from_ts(ts_type *ts_in, saxt_type *saxt_out) {
    // Create PAA representation
    float * paa = static_cast<float *>(malloc(sizeof(float) * Segments));
    sax_type *sax_out = static_cast<sax_type *>(malloc(sizeof(sax_type) * Segments));
    if(paa == NULL) {
        fprintf(stderr,"error: could not allocate memory for PAA representation.\n");
        return FAILURE;
    }

    int s, i;
    for (s=0; s<Segments; s++) {
        paa[s] = 0;
        for (i=0; i<Ts_values_per_segment; i++) {
            paa[s] += ts_in[(s * Ts_values_per_segment)+i];
        }
        paa[s] /= Ts_values_per_segment;
//#ifdef DEBUG
        //printf("%d: %lf\n", s, paa[s]);
//#endif
    }

    // Convert PAA to SAX
    // Note: Each cardinality has cardinality - 1 break points if c is cardinality
    //       the breakpoints can be found in the following array positions:
    //       FROM (c - 1) * (c - 2) / 2
    //       TO   (c - 1) * (c - 2) / 2 + c - 1
    //printf("FROM %lf TO %lf\n", sax_breakpoints[offset], sax_breakpoints[offset + cardinality - 2]);

    int si;
    for (si=0; si<Segments; si++) {
        sax_out[si] = 0;

        // First object = sax_breakpoints[offset]
        // Last object = sax_breakpoints[offset + cardinality - 2]
        // Size of sub-array = cardinality - 1

        float *res = (float *) bsearch(&paa[si], &sax_breakpoints[sax_offset], Cardinality - 1,
                                       sizeof(ts_type), compare);
        if(res != NULL)
        {
            //sax_out[si] = (int) (res -  &sax_breakpoints[offset]);
            sax_out[si] = (sax_type) (res -  &sax_breakpoints[sax_offset]);
        }
        else if (paa[si] > 0)
            sax_out[si] = (sax_type) (Cardinality-1);
    }

    //sax_print(sax_out, segments, cardinality);
    free(paa);
    for(int i=0; i<Bit_cardinality; i++) {
        for(int j=0; j<Segments; j++) {
            saxt_out[i] |= (sax_out[j]>>(Bit_cardinality-i-1) & 1) << (Segments-j-1);
        }
    }
    free(sax_out);
    return SUCCESS;
}

enum response saxt_from_sax(sax_type *sax_in, saxt_type *saxt_out) {

    for(int i=0; i<Bit_cardinality; i++) {
        for(int j=0; j<Segments; j++) {
            saxt_out[i] |= (sax_in[j]>>(Bit_cardinality-i-1) & 1) << (Segments-j-1);
        }
    }
    return SUCCESS;
}

enum response sax_from_saxt(sax_type *saxt_in, saxt_type *sax_out) {
    for(int i=0; i<Segments; i++) {
        for(int j=0; j<Bit_cardinality; j++) {
            sax_out[i] |= (saxt_in[j]>>(Segments-i-1) & 1) << (Bit_cardinality-j-1);
        }
    }
    return SUCCESS;
}


void printbin(long long unsigned int n, int size) {
    char *b = static_cast<char *>(malloc(sizeof(char) * (size + 1)));
    int i;
    
    for (i=0; i<size; i++) {
        b[i] = '0';
    }
    
    for (i=0; i<size; i++, n=n/2)
        if (n%2) b[size-1-i] = '1';
    
    b[size] = '\0';
    printf("%s\n", b);
    free(b);
}

void serial_printbin (unsigned long long n, int size) {
    char *b = static_cast<char *>(malloc(sizeof(char) * (size + 1)));
    int i;
    
    for (i=0; i<size; i++) {
        b[i] = '0';
    }
    
    for (i=0; i<size; i++, n=n/2)
        if (n%2) b[size-1-i] = '1';
    
    b[size] = '\0';
    printf(" %s ", b);
}


/**
 This function prints a sax record.
 */
void sax_print(sax_type *sax, int segments, int bit_cardinality)
{
    int i;
    for (i=0; i < segments; i++) {
        printf("%d:\t", i);
        printbin(sax[i], bit_cardinality);
    }
    printf("\n");
}

void saxt_print(saxt_type *saxt) {
    int i;
    for (i=0; i < Bit_cardinality; i++) {
        printf("%d:\t", i);
        printbin(saxt[i], Segments);
    }
    printf("\n");
}


ts_type minidist_paa_to_isax(ts_type  *paa, sax_type *sax,
                             sax_type *sax_cardinalities,
                             sax_type max_bit_cardinality,
                             sax_type max_cardinality,
                             int number_of_segments,
                             int min_val,
                             int max_val,
                             float ratio_sqrt)
{

    ts_type distance = 0;
    // TODO: Store offset in index settings. and pass index settings as parameter.
    
    int offset = ((max_cardinality - 1) * (max_cardinality - 2)) / 2;
    
    // For each sax record find the break point
    int i;
    for (i=0; i<number_of_segments; i++) {
        
        sax_type c_c = sax_cardinalities[i];
        sax_type c_m = max_bit_cardinality;
        sax_type v = sax[i];
        
        
        sax_type region_lower = v << (c_m - c_c);
        sax_type region_upper = (~((int)MAXFLOAT << (c_m - c_c)) | region_lower);
        

	/*
        int region_lower = v << (c_m - c_c);
        int region_upper = (~((int)MAXFLOAT << (c_m - c_c)) | region_lower);
	*/
	
        ts_type breakpoint_lower = 0; // <-- TODO: calculate breakpoints.
        ts_type breakpoint_upper = 0; // <-- - || -
        
        
        if (region_lower == 0) {
            breakpoint_lower = min_val;
        }
        else
        {
            breakpoint_lower = sax_breakpoints[offset + region_lower - 1];
        }
        if (region_upper == max_cardinality - 1) {
            breakpoint_upper = max_val;
        }
        else
        {
            breakpoint_upper = sax_breakpoints[offset + region_upper];
        }
        //printf("FROM: \n");
        //sax_print(&region_lower, 1, c_m);
        //printf("TO: \n");
        //sax_print(&region_upper, 1, c_m);
        //printf("\n%d.%d is from %d to %d, %lf - %lf\n", v, c_c, region_lower, region_upper,
        //       breakpoint_lower, breakpoint_upper);
        
        if (breakpoint_lower > paa[i]) {
            distance += pow(breakpoint_lower - paa[i], 2);
        }
        else if(breakpoint_upper < paa[i]) {
            distance += pow(breakpoint_upper - paa[i], 2);
        }
//        else {
//            printf("%lf is between: %lf and %lf\n", paa[i], breakpoint_lower, breakpoint_upper);
//        }
    }
    
    //distance = ratio_sqrt * sqrtf(distance);  
    distance = ratio_sqrt * distance;
    return distance;
}


ts_type minidist_paa_to_isax_raw(ts_type *paa, sax_type *sax, 
                           sax_type *sax_cardinalities,
                           sax_type max_bit_cardinality,
                           int max_cardinality,
                           int number_of_segments,
                           int min_val,
                           int max_val,
                           ts_type ratio_sqrt) 
{
   
    float distance = 0;
    // TODO: Store offset in index settings. and pass index settings as parameter.
    
    int offset = ((max_cardinality - 1) * (max_cardinality - 2)) / 2;
    
    // For each sax record find the break point
    int i;
    for (i=0; i<number_of_segments; i++) {
        
        sax_type c_c = sax_cardinalities[i];
        sax_type c_m = max_bit_cardinality;
        sax_type v = sax[i];
        //sax_print(&v, 1, c_m);
        
        sax_type region_lower = (v >> (c_m - c_c)) <<  (c_m - c_c);
        sax_type region_upper = (~((int)MAXFLOAT << (c_m - c_c)) | region_lower);
		//printf("[%d, %d] %d -- %d\n", sax[i], c_c, region_lower, region_upper);

        float breakpoint_lower = 0; // <-- TODO: calculate breakpoints.
        float breakpoint_upper = 0; // <-- - || -
        
        
        if (region_lower == 0) {
            breakpoint_lower = min_val;
        }
        else
        {
            breakpoint_lower = sax_breakpoints[offset + region_lower - 1];
        }
        if (region_upper == max_cardinality - 1) {
            breakpoint_upper = max_val;
        }
        else
        {
            breakpoint_upper = sax_breakpoints[offset + region_upper];
        }

        //printf("\n%d.%d is from %d to %d, %lf - %lf\n", v, c_c, region_lower, region_upper,
        //       breakpoint_lower, breakpoint_upper);

        //printf("FROM: \n");
        //sax_print(&region_lower, 1, c_m);
        //printf("TO: \n");
        //sax_print(&region_upper, 1, c_m);
		
		//printf ("\n---------\n");
        
        if (breakpoint_lower > paa[i]) {
            distance += pow(breakpoint_lower - paa[i], 2);
        }
        else if(breakpoint_upper < paa[i]) {
            distance += pow(breakpoint_upper - paa[i], 2);
        }
//        else {
//            printf("%lf is between: %lf and %lf\n", paa[i], breakpoint_lower, breakpoint_upper);
//        }
    }
    
    //distance = ratio_sqrt * sqrtf(distance);
    distance = ratio_sqrt * distance;
    return distance;
}



cod get_co_d_from_saxt(saxt a, saxt b) {
    int d;
    for(d=0; d<Bit_cardinality; d++){
        if (a[d] != b[d]) return d;
    }
    return d;
}

cod get_co_d_from_saxt(saxt a, saxt b, cod pre_d) {
    int d = pre_d;
    for(; d<Bit_cardinality; d++){
        if (a[d] != b[d]) return d;
    }
    return d;
}


bool compare_saxt_d(saxt a, saxt b, cod d) {
    return *(a + d - 1) == *(b + d - 1);
}

