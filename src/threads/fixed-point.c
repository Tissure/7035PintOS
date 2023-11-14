#include "threads/fixed-point.h"

// Bitshift by 'q', which is part of the 17.14 format.
#define F (1 << 14)

int convert_int_to_fp(int inInteger)
{
    // Bit shift by 14 to align with 17.14 format.
    return inInteger * F;
}

int convert_fp_to_integer(int fpNumber)
{
    // Take the FP number and dvide by F, which is bitshifted by 14.
    return fpNumber / F;
}

int convert_fp_to_integer_rounded(int fpNumber)
{
    if (fpNumber >= 0)
    {
        return (fpNumber + F / 2) / F;
    }
    if (fpNumber <= 0)
    {
        return (fpNumber - F / 2) / F;
    }
}

int add_fp(int fpNumOne, int fpNumTwo)
{
    return (fpNumOne + fpNumTwo);
}

int subtract_fp(int fpNumOne, int fpNumTwo)
{
    return (fpNumOne - fpNumTwo);
}

int add_integer_to_fp(int fpNum, int intNum)
{
    return (fpNum + intNum * F);
}

int subtract_integer_to_fp(int fpNum, int intNum)
{
    return (fpNum - intNum * F);
}

int multiply_fp(int fpNumOne, int fpNumTwo)
{
    return ((int64_t)fpNumOne) * fpNumTwo / F;
}

int multiply_fp_by_int(int fpNumOne, int intNum)
{
    return (fpNumOne * intNum);
}

int divide_fp(int fpNumOne, int fpNumTwo)
{
    return ((int64_t)fpNumOne) * F / fpNumTwo;
}

int divide_fp_by_int(int fpNumOne, int intNum)
{
    return (fpNumOne / intNum);
}
