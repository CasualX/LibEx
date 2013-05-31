#ifndef HGUARD_MYSTD_SIMPLE_VECTOR
#define HGUARD_MYSTD_SIMPLE_VECTOR

namespace mystd
{

template< typename T, unsigned int N >
class vector_t
{
public:
	vector_t();

	// Declarations
	typedef typename T* iterator;
	typedef typename const T* const_iterator;

	// Accessors
	unsigned int size() const;
	unsigned int max_size() const;
	T& get( unsigned int idx );
	const T& get( unsigned int idx ) const;
	T& operator[] ( unsigned int idx );
	const T& operator[] ( unsigned int idx ) const;

	// Manipulation
	T& push();
	T& pop();
	void clear();
	void resize( unsigned int size );
	T& insert( iterator pos );
	T& insert( unsigned int idx );
	void erase( iterator pos );
	void erase( unsigned int idx );
	
	// Iteration
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
	
	// Not sure if this belongs...
	template< typename U >
	iterator find_first( const U& value );

private:
	unsigned int count;
	T data[N];
};


//
// Implementation
//

template< typename T, unsigned int N >
vector_t<T,N>::vector_t() : count(0)
{
}
template< typename T, unsigned int N >
inline unsigned int vector_t<T,N>::size() const
{
	return count;
}
template< typename T, unsigned int N >
inline unsigned int vector_t<T,N>::max_size() const
{
	return N;
}
template< typename T, unsigned int N >
inline T& vector_t<T,N>::get( unsigned int idx )
{
	assert(idx<count);
	return data[idx];
}
template< typename T, unsigned int N >
inline const T& vector_t<T,N>::get( unsigned int idx ) const
{
	assert(idx<count);
	return data[idx];
}
template< typename T, unsigned int N >
inline T& vector_t<T,N>::operator[] ( unsigned int idx )
{
	assert(idx<count);
	return data[idx];
}
template< typename T, unsigned int N >
inline const T& vector_t<T,N>::operator[] ( unsigned int idx ) const
{
	assert(idx<count);
	return data[idx];
}

template< typename T, unsigned int N >
inline T& vector_t<T,N>::push()
{
	assert(count<N);
	return data[count++];
}
template< typename T, unsigned int N >
inline T& vector_t<T,N>::pop()
{
	assert(count>0);
	return data[--count];
}
template< typename T, unsigned int N >
inline void vector_t<T,N>::clear()
{
	count = 0;
}
template< typename T, unsigned int N >
inline void vector_t<T,N>::resize( unsigned int size )
{
	count = size;
}
template< typename T, unsigned int N >
T& vector_t<T,N>::insert( iterator pos )
{
	assert( pos>=begin() && pos<=end() );
	push();
	for ( iterator it = end(); --it>pos; )
	{
		it[0] = it[-1];
	}
	return *pos;
}
template< typename T, unsigned int N >
inline T& vector_t<T,N>::insert( unsigned int idx )
{
	return insert( data + idx );
}
template< typename T, unsigned int N >
void vector_t<T,N>::erase( iterator pos )
{
	assert( pos>=begin() && pos<end() );
	for ( iterator it = pos, en = end()-1; it<end; ++it )
	{
		it[0] = it[1];
	}
	--count;
}
template< typename T, unsigned int N >
inline void vector_t<T,N>::erase( unsigned int idx )
{
	erase( data + idx );
}

template< typename T, unsigned int N >
inline typename vector_t<T,N>::iterator vector_t<T,N>::begin()
{
	return data;
}
template< typename T, unsigned int N >
inline typename vector_t<T,N>::iterator vector_t<T,N>::end()
{
	return data+count;
}
template< typename T, unsigned int N >
inline typename vector_t<T,N>::const_iterator vector_t<T,N>::begin() const
{
	return data;
}
template< typename T, unsigned int N >
inline typename vector_t<T,N>::const_iterator vector_t<T,N>::end() const
{
	return data+count;
}

template< typename T, unsigned int N >
template< typename U >
typename vector_t<T,N>::iterator vector_t<T,N>::find_first( const U& value )
{
	iterator it;
	for ( it = begin(), en = end(); it!=en; ++it )
	{
		if ( *it==value )
			break;
	}
	return it;
}


}

#endif // !HGUARD_MYSTD_SIMPLE_VECTOR
