#ifndef THREADS_FIXEDPOINT_H
#define THREADS_FIXEDPOINT_H

#include <stdint.h>
typedef int fixed_point;

// #define FRACTION 15
// #define convert_int_to_fp(x) ((fixed_point)(x << FRACTION))
// #define convert_fp_to_integer(x) (x >> FRACTION)
// #define convert_fp_to_integer_rounded(x) (x >= 0 ? ((x + (1 << (FRACTION - 1))) >> FRACTION) : ((x - (1 << (FRACTION - 1))) >> FRACTION))
// #define add_fp(x, y) (x + y)
// #define add_integer_to_fp(x, n) (x + (n << FRACTION))
// #define subtract_fp(x, y) (x - y)
// #define subtract_integer_to_fp(x, n) (x - (n << FRACTION))
// #define multiply_fp(x, y) (x * y)
// #define multiply_fp_by_int(x, n) ((fixed_point)(((int64_t)x) * n >> FRACTION))
// #define divide_fp(x, y) (x / y)
// #define divide_fp_by_int(x, n) ((fixed_point)(((int64_t)x) << FRACTION) / n)

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