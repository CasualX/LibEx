#ifndef HGUARD_MYSTD_SIMPLE_VECTOR
#define HGUARD_MYSTD_SIMPLE_VECTOR

namespace mystd
{

template< typename T, unsigned int N >
class vector_t
{
public:
	vector_t() : count(0) { }
	
	// Manipulation
	inline T& push() { assert(count<N); return data[count++]; }
	inline T& pop() { asser(count>0); return data[--count]; }
	inline void clear() { count = 0; }

	// Accessor
	inline unsigned int size() const { return count; }
	inline T& get( unsigned int idx ) { assert(idx<count); return data[idx]; }
	inline const T& get( unsigned int idx ) const { assert(idx<count); return data[idx]; }
	inline T& operator[] ( unsigned int idx ) { assert(idx<count); return data[idx]; }
	inline const T& operator[] ( unsigned int idx ) const { assert(idx<count); return data[idx]; }

	// Iteration
	typedef T* iterator;
	inline iterator begin() { return data; }
	inline iterator end() { return data+count; }
	typedef const T* const_iterator;
	inline const_iterator begin() const { return data; }
	inline const_iterator end() const { return data+count; }

private:
	unsigned int count;
	T data[N];
};

}

#endif // !HGUARD_MYSTD_SIMPLE_VECTOR
