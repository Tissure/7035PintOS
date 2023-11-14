#ifndef THREADS_FIXEDPOINT_H
#define THREADS_FIXEDPOINT_H

#include <stdint.h>

/*Convert inInteger to fixed point*/
int convert_int_to_fp(int inInteger);
/*Convert fpNumber to integer (rounding toward zero)*/
int convert_fp_to_integer(int fpNumber);
/*Convert fpNumber to integer (rounding to nearest)*/
int convert_fp_to_integer_rounded(int fpNumber);
/*Add fpNumOne and fpNumTwo*/
int add_fp(int fpNumOne, int fpNumTwo);
/*Subtract fpNumTwo from fpNumOne*/
int subtract_fp(int fpNumOne, int fpNumTwo);
/*Add fpNum and intNum*/
int add_integer_to_fp(int fpNum, int intNum);
/*Subtract fpNum from intNum:*/
int subtract_integer_to_fp(int fpNum, int intNum);
/*Multiply fpNumOne by fpNumTwo:*/
int multiply_fp(int fpNumOne, int fpNumTwo);
/*Multiply fpNumOne by intNum:*/
int multiply_fp_by_int(int fpNumOne, int intNum);
/*Divide fpNumOne by fpNumTwo*/
int divide_fp(int fpNumOne, int fpNumTwo);
/*Divide fpNumOne by intNum*/
int divide_fp_by_int(int fpNumOne, int intNum);

#endif /* threads/fixed-point.h */