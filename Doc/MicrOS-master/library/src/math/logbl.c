#include "../math.h"

long double logbl(long double x)
{
    // domain error
    if(x < 0)
    {
        if(_math_errhandling == MATH_ERRNO)
        {
            errno = EDOM;
        }
        else if(_math_errhandling == MATH_ERREXCEPT)
        {
            feraiseexcept(FE_INVALID);  
        }
        return 0;
    }
    // pole error
    if(x == 0)
    {
        if(_math_errhandling == MATH_ERRNO)
        {
            errno = ERANGE;
        }
        else if(_math_errhandling == MATH_ERREXCEPT)
        {
            feraiseexcept(FE_DIVBYZERO);  
        }
        return -__math_INFf();
    }
    // FLT_RADIX is guaranteed to be a constant expression.
    #if FLT_RADIX == 2
        return log2l(x);
    #else
        return log2l(x)/log2l(FLT_RADIX);
    #endif 
}