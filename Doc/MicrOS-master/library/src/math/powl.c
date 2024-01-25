#include "../math.h"

long double powl(long double base, long double exponent)
{
    feclearexcept(FE_OVERFLOW);
    feclearexcept(FE_UNDERFLOW);
    
    if(base < 0 && ceill(exponent) == exponent && floorl(exponent) == exponent)
    {
        //domain error
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
    if(base == 0 && exponent == 0)
    {
        //domain error
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
    if(base == 0 && exponent < 0)
    {
        //domain error
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
    
    // Use 2^(y*log2(x)) 
    // Compute y*log2(x)
    long double ex;
    __asm__ (
        "fldt %2 \n" \
        "fldt %1 \n" \
        "fyl2x \n" \
        "fstpt %0"
        : "=m"(ex): "m"(base), "m"(exponent));

    // Scale
    long double integer, remainder;
    remainder = modfl(ex, &integer);

    // Compute 2^ex
    long double resultBeforeScale;
    __asm__ (
        "fldt %1 \n" \
        "f2xm1 \n" \
        "fld1 \n" \
        "faddp \n" \
        "fstpt %0"
        : "=m"(resultBeforeScale): "m"(remainder));
    
    // Result after scale
    long double result;
    __asm__ (
        "fldt %2 \n" \
        "fldt %1 \n" \
        "fscale \n" \
        "fstpt %0"
        : "=m"(result): "m"(resultBeforeScale), "m"(integer));
    
    fexcept_t exceptions = __FPU_read_status_word();
    // range error
    if(exceptions.overflow == 1 || exceptions.underflow == 1)
    {
        if(_math_errhandling == MATH_ERRNO)
        {
            errno = ERANGE;
            feclearexcept(FE_OVERFLOW);
            feclearexcept(FE_UNDERFLOW);
        }
        // In other case overflow or underflow is already set so we don't need to cover _math_errhandling == MATH_ERREXCEPT
        return 0;
    }
    return result;  
}