#ifndef HGUARD_LIBEX_MYSTD_RANDOM
#define HGUARD_LIBEX_MYSTD_RANDOM
#pragma once

namespace mystd
{

// Linear congruential generator
template< typename T, T A, T C, T M >
class basic_lcg
{
public:
	typedef basic_lcg<T,A,C,M> this_t;
	typedef typename T type_t;

	basic_lcg( T seed ) : _seed(seed) { }
	T next() { return _bswap( _seed = M?((_seed*A+C)%M):(_seed*A+C) ); }
private:
	T _seed;
};
typedef basic_lcg<unsigned,1664525,1013904223,0> lcg_numerical_recipies;
typedef basic_lcg<unsigned,22695477,1,0> lcg_borland_cpp;
typedef basic_lcg<unsigned,1103515245,12345,2<<31> lcg_glibc;
typedef basic_lcg<unsigned,134775813,1,0> lcg_borland_deplphi;
typedef basic_lcg<unsigned,214013,2531011,0> lcg_visual_cpp;
typedef basic_lcg<unsigned,1140671485,12820163,2<<24> lcg_visual_basic;
typedef basic_lcg<unsigned,2147483629,2147483587,(2<<31)-1> lcg_rtluniform;
typedef basic_lcg<unsigned,16807,0,(2<<31)-1> lcg_carbonlib;
typedef basic_lcg<unsigned long long,6364136223846793005ULL,1442695040888963407ULL,0> lcg_mmix;
typedef basic_lcg<unsigned,69069,1,0> lcg_mth_random;
typedef basic_lcg<unsigned long long,25214903917ULL,11ULL,2ULL<<48> lcg_java_random;
typedef basic_lcg<unsigned,-333333333,0,-5> lcg_lc53;


template< typename G >
typename G::type_t random( G& g, typename G::type_t l, typename G::type_t h )
{
	return (g.next()%(h-l))+l;
}

}

#endif // !HGUARD_LIBEX_MYSTD_RANDOM
