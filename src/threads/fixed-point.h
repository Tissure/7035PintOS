#ifndef THREADS_FIXEDPOINT_H
#define THREADS_FIXEDPOINT_H

/*Convert n to fixed point*/
static int convert_int_to_fp(int n);
/*Convert x to integer (rounding toward zero)*/
static int convert_fp_to_int(int x);
/*Convert x to integer (rounding to nearest):*/
static int convert_fp_to_int_round(int x);
/*Add x and y:*/
static int add_fp(int x, int y);
/*Subtract y from x:*/
static int subtract_fp(int x, int y);
/*Add x and n:*/
static int add_int_to_fp(int x, int n);
/*Subtract n from x:*/
static int subtract_int_from_fp(int x, int n);
/*Multiply x by y:*/
static int multiply_fp(int x, int y);
/*Multiply x by n:*/
static int multiply_fp_by_int(int x, int n);
/*Divide x by y:*/
static int divide_fp(int x, int y);
/*Divide x by n:*/
static int divide_fp_by_int(int x, int n);

#endif /* threads/fixed-point.h */