#ifndef HGUARD_LIBEX_MATH_DEFS
#define HGUARD_LIBEX_MATH_DEFS
#pragma once

#include <cmath>

#define MATH_ALIGN16 __declspec(align(16))
#define MATH_MINMAX_FIX
#define MATH_NO_VARIADIC_TEMPLATES

namespace math
{

typedef unsigned int dim;

}

#endif // !HGUARD_LIBEX_MATH_DEFS
