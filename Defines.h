#include <stdio.h>
#include <xil_io.h>

#define order 5
#define amount 1


float x_terms[amount][order] = {
		{1,2,3,4,5},
//		{},
//		{},
//		{}
};

u8 IIROrder[amount] = {0};//,0,0,0};

/* Fill the terms for the y[n] in in order of the filters.
 * If a filter is a FIR filter, keep the array empty.
 *
 * Example of 5th order filters with 2 FIR and one IIR:
 * float y_terms_IIR[amount][order] = {
 * 		{},				//FIR
 * 		{},				//FIR
 * 		{1,2,4,8,12},	//IIR
 *	};
 */
float y_terms_IIR[amount][order-1] = {
		{0,0,0,0},
//		{},
//		{},
//		{}
};


