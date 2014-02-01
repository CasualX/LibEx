
#include "vector4f.h"

namespace math_sse
{


void vector4f::cvt_ang2vecs( vector4f& forward, vector4f& up, vector4f& right ) const
{
	assert( false );
}
vector4f& vector4f::cvt_ang2vec()
{
	float sp, sy, cp, cy;
	
	float ry = reg.m128_f32[1];
	sy = sinf( ry );
	cy = cosf( ry );
	float rp = reg.m128_f32[0];
	sp = sinf(rp);
	cp = cosf(rp);
	
	reg.m128_f32[0] = cp*cy;
	reg.m128_f32[1] = cp*sy;
	reg.m128_f32[2] = -sp;
	reg.m128_f32[3] = 0.0f;

	return *this;
}
vector4f& vector4f::cvt_vec2ang()
{
	const float pi = 3.14159265358979323846f;
	
	float* forward = reg.m128_f32;
	float* angles = reg.m128_f32;

	float tmp, yaw, pitch;
	
	if ( forward[1]==0 && forward[0]==0 )
	{
		yaw = 0;
		if ( forward[2]>0 )
			pitch = pi + pi/2.f;
		else
			pitch = pi/2.f;
	}
	else
	{
		yaw = atan2( forward[1], forward[0] );
		if ( yaw<0 )
			yaw += pi*2.f;

		tmp = sqrtf( forward[0]*forward[0] + forward[1]*forward[1] );
		pitch = atan2( -forward[2], tmp );
		if ( pitch<0 )
			pitch += pi*2.f;
	}
	
	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;

	return *this;
}


}
