#ifndef HGUARD_META_SCRYPT
#define HGUARD_META_SCRYPT

namespace meta
{

inline void SimpleDecrypt( const void* src, unsigned int off, unsigned int iv, void* out, unsigned int size )
{
	for ( unsigned int i = 0; i<size; i+=4 )
	{
		unsigned int block = *(const unsigned int*)( (const char*)src + off + i );
		unsigned int seed = ((off+i)*1103515245+iv);
		unsigned int value = seed^(block-iv);
		*(unsigned int*)( (char*)out + i ) = value;
	}
}
inline void SimpleEncrypt( void* inout, unsigned int off, unsigned int iv, unsigned int size )
{
	for ( unsigned int i = 0; i<size; i+=4 )
	{
		unsigned int& block = *(unsigned int*)( (char*)inout + i );
		unsigned int seed = ((off+i)*1103515245+iv);
		unsigned int value = iv+(seed^block);
		block = value;
	}
}
inline unsigned int SimpleIV()
{
	return static_cast<unsigned int>( __rdtsc() )*1103515245+12345;
}
}

#endif // !HGUARD_META_SCRYPT
