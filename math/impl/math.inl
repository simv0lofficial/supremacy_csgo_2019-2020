#pragma once

#include "../math.hpp"

namespace math {
	__forceinline void sin_cos( const float rad, float& sin, float& cos ) {
		sin = std::sin( rad );
		cos = std::cos( rad );
	}

	__forceinline float to_deg( const float rad ) { return rad * k_rad_pi; }

	__forceinline float to_rad( const float deg ) { return deg * k_deg_pi; }

	__forceinline float angle_diff( float src, float dst ) {
		return std::remainder(
			std::remainder(dst, 360.f) - std::remainder(src, 360.f),
			360.f
		);
	}

	__forceinline float approach_angle(float target, float value, float speed) {
		target = (target * 182.04445f) * 0.0054931641f;
		value = (value * 182.04445f) * 0.0054931641f;

		float delta = target - value;

		if (speed < 0.f)
			speed = -speed;

		if (delta < -180.f)
			delta += 360.f;
		else if (delta > 180.f)
			delta -= 360.f;

		if (delta > speed)
			value += speed;
		else if (delta < -speed)
			value -= speed;
		else
			value = target;

		return value;
	}

	__forceinline vec3_t calculate_angle(const vec3_t& src, const vec3_t& dst) {
		vec3_t angles{};
		vec3_t delta = src - dst;
		double hyp = sqrt(delta.x * delta.x + delta.y * delta.y);

		angles.x = atan(delta.z / hyp) * 57.295779513082f;
		angles.y = atan(delta.y / delta.x) * 57.295779513082f;
		angles.z = 0.f;

		if (delta.x >= 0.f)
			angles.y += 180.f;

		return angles;
	}

	__forceinline void concat_transforms( const mat3x4_t& in0, const mat3x4_t& in1, mat3x4_t& out ) {
		out[ 0 ][ 0 ] = in0[ 0 ][ 0 ] * in1[ 0 ][ 0 ] + in0[ 0 ][ 1 ] * in1[ 1 ][ 0 ] + in0[ 0 ][ 2 ] * in1[ 2 ][ 0 ];
		out[ 0 ][ 1 ] = in0[ 0 ][ 0 ] * in1[ 0 ][ 1 ] + in0[ 0 ][ 1 ] * in1[ 1 ][ 1 ] + in0[ 0 ][ 2 ] * in1[ 2 ][ 1 ];
		out[ 0 ][ 2 ] = in0[ 0 ][ 0 ] * in1[ 0 ][ 2 ] + in0[ 0 ][ 1 ] * in1[ 1 ][ 2 ] + in0[ 0 ][ 2 ] * in1[ 2 ][ 2 ];
		out[ 0 ][ 3 ] = in0[ 0 ][ 0 ] * in1[ 0 ][ 3 ] + in0[ 0 ][ 1 ] * in1[ 1 ][ 3 ] + in0[ 0 ][ 2 ] * in1[ 2 ][ 3 ] + in0[ 0 ][ 3 ];

		out[ 1 ][ 0 ] = in0[ 1 ][ 0 ] * in1[ 0 ][ 0 ] + in0[ 1 ][ 1 ] * in1[ 1 ][ 0 ] + in0[ 1 ][ 2 ] * in1[ 2 ][ 0 ];
		out[ 1 ][ 1 ] = in0[ 1 ][ 0 ] * in1[ 0 ][ 1 ] + in0[ 1 ][ 1 ] * in1[ 1 ][ 1 ] + in0[ 1 ][ 2 ] * in1[ 2 ][ 1 ];
		out[ 1 ][ 2 ] = in0[ 1 ][ 0 ] * in1[ 0 ][ 2 ] + in0[ 1 ][ 1 ] * in1[ 1 ][ 2 ] + in0[ 1 ][ 2 ] * in1[ 2 ][ 2 ];
		out[ 1 ][ 3 ] = in0[ 1 ][ 0 ] * in1[ 0 ][ 3 ] + in0[ 1 ][ 1 ] * in1[ 1 ][ 3 ] + in0[ 1 ][ 2 ] * in1[ 2 ][ 3 ] + in0[ 1 ][ 3 ];

		out[ 2 ][ 0 ] = in0[ 2 ][ 0 ] * in1[ 0 ][ 0 ] + in0[ 2 ][ 1 ] * in1[ 1 ][ 0 ] + in0[ 2 ][ 2 ] * in1[ 2 ][ 0 ];
		out[ 2 ][ 1 ] = in0[ 2 ][ 0 ] * in1[ 0 ][ 1 ] + in0[ 2 ][ 1 ] * in1[ 1 ][ 1 ] + in0[ 2 ][ 2 ] * in1[ 2 ][ 1 ];
		out[ 2 ][ 2 ] = in0[ 2 ][ 0 ] * in1[ 0 ][ 2 ] + in0[ 2 ][ 1 ] * in1[ 1 ][ 2 ] + in0[ 2 ][ 2 ] * in1[ 2 ][ 2 ];
		out[ 2 ][ 3 ] = in0[ 2 ][ 0 ] * in1[ 0 ][ 3 ] + in0[ 2 ][ 1 ] * in1[ 1 ][ 3 ] + in0[ 2 ][ 2 ] * in1[ 2 ][ 3 ] + in0[ 2 ][ 3 ];
	}

	__forceinline void angle_vectors( const qangle_t& angle, vec3_t* const fwd, vec3_t* const right, vec3_t* const up ) {
		float cp{}, sp{}, cy{}, sy{}, cr{}, sr{};

		sin_cos( to_rad( angle.x ), sp, cp );
		sin_cos( to_rad( angle.y ), sy, cy );
		sin_cos( to_rad( angle.z ), sr, cr );

		if ( fwd ) {
			fwd->x = cp * cy;
			fwd->y = cp * sy;
			fwd->z = -sp;
		}

		if ( right ) {
			right->x = -1.f * sr * sp * cy + -1.f * cr * -sy;
			right->y = -1.f * sr * sp * sy + -1.f * cr * cy;
			right->z = -1.f * sr * cp;
		}

		if ( up ) {
			up->x = cr * sp * cy + -sr * -sy;
			up->y = cr * sp * sy + -sr * cy;
			up->z = cr * cp;
		}
	}

	__forceinline void vector_angles( const vec3_t& in, qangle_t& out ) {
		if ( in.x == 0.f
			&& in.y == 0.f ) {
			out.y = 0.f;
			out.x = in.z > 0.f ? 270.f : 90.f;
		}
		else {
			out.y = to_deg( atan2( in.y, in.x ) );
			if ( out.y < 0.f )
				out.y += 360.f;

			out.x = to_deg( atan2( -in.z, in.length_2d( ) ) );
			if ( out.x < 0.f )
				out.x += 360.f;
		}

		out.x = remainder( out.x, 360.f );
		out.y = remainder( out.y, 360.f );
		out.z = remainder( out.z, 360.f );
	}

	__forceinline float calc_fov( const qangle_t& view_angles, const vec3_t& src, const vec3_t& dst ) {
		const auto dir = ( dst - src ).normalized( );

		vec3_t fwd{};

		angle_vectors( view_angles, &fwd );

		return std::max( to_deg( acos( fwd.dot( dir ) ) ), 0.f );
	}

	__forceinline void vector_transform( const vec3_t& in, const mat3x4_t& matrix, vec3_t& out ) {
		out = {
			in.dot( { matrix[ 0 ][ 0 ], matrix[ 0 ][ 1 ], matrix[ 0 ][ 2 ] } ) + matrix[ 0 ][ 3 ],
			in.dot( { matrix[ 1 ][ 0 ], matrix[ 1 ][ 1 ], matrix[ 1 ][ 2 ] } ) + matrix[ 1 ][ 3 ],
			in.dot( { matrix[ 2 ][ 0 ], matrix[ 2 ][ 1 ], matrix[ 2 ][ 2 ] } ) + matrix[ 2 ][ 3 ]
		};
	}

	__forceinline void vector_rotate( const vec3_t& in, const mat3x4_t& matrix, vec3_t& out ) {
		out = { in.dot( matrix[ 0 ] ), in.dot( matrix[ 1 ] ), in.dot( matrix[ 2 ] ) };
	}

	__forceinline void vector_i_transform( const vec3_t& in, const mat3x4_t& matrix, vec3_t& out ) {
		const vec3_t diff{
			in.x - matrix[ 0 ][ 3 ],
			in.y - matrix[ 1 ][ 3 ],
			in.z - matrix[ 2 ][ 3 ]
		};

		out = {
			diff.x * matrix[ 0 ][ 0 ] + diff.y * matrix[ 1 ][ 0 ] + diff.z * matrix[ 2 ][ 0 ],
			diff.x * matrix[ 0 ][ 1 ] + diff.y * matrix[ 1 ][ 1 ] + diff.z * matrix[ 2 ][ 1 ],
			diff.x * matrix[ 0 ][ 2 ] + diff.y * matrix[ 1 ][ 2 ] + diff.z * matrix[ 2 ][ 2 ]
		};
	}

	__forceinline void vector_i_rotate( const vec3_t& in, const mat3x4_t& matrix, vec3_t& out ) {
		out = {
			in.x * matrix[ 0 ][ 0 ] + in.y * matrix[ 1 ][ 0 ] + in.z * matrix[ 2 ][ 0 ],
			in.x * matrix[ 0 ][ 1 ] + in.y * matrix[ 1 ][ 1 ] + in.z * matrix[ 2 ][ 1 ],
			in.x * matrix[ 0 ][ 2 ] + in.y * matrix[ 1 ][ 2 ] + in.z * matrix[ 2 ][ 2 ]
		};
	}

	__forceinline mat3x4_t vector_matrix( const vec3_t& in ) {
		vec3_t right{}, up{};

		if ( in.x == 0.f
			&& in.y == 0.f ) {
			right = { 0.f, -1.f, 0.f };
			up = { -in.z, 0.f, 0.f };
		}
		else {
			right = in.cross( { 0.f, 0.f, 1.f } ).normalized( );
			up = right.cross( in ).normalized( );
		}

		mat3x4_t ret{};

		ret[ 0 ][ 0 ] = in.x;
		ret[ 1 ][ 0 ] = in.y;
		ret[ 2 ][ 0 ] = in.z;

		ret[ 0 ][ 1 ] = -right.x;
		ret[ 1 ][ 1 ] = -right.y;
		ret[ 2 ][ 1 ] = -right.z;

		ret[ 0 ][ 2 ] = up.x;
		ret[ 1 ][ 2 ] = up.y;
		ret[ 2 ][ 2 ] = up.z;

		return ret;
	}

	__forceinline float segment_to_segment( const vec3_t& s1, const vec3_t& s2, const vec3_t& k1, const vec3_t& k2 ) {
		const auto u = s2 - s1, v = k2 - k1, w = s1 - k1;

		const auto a = u.dot( u );
		const auto b = u.dot( v );
		const auto c = v.dot( v );
		const auto d = u.dot( w );
		const auto e = v.dot( w );
		const auto unk = a * c - b * b;

		auto sn = 0.f, tn = 0.f, sd = unk, td = unk;

		if ( unk < 0.00000001f ) {
			sn = 0.f;
			sd = 1.f;
			tn = e;
			td = c;
		}
		else {
			sn = b * e - c * d;
			tn = a * e - b * d;

			if ( sn < 0.f ) {
				sn = 0.f;
				tn = e;
				td = c;
			}
			else if ( sn > sd ) {
				sn = sd;
				tn = e + b;
				td = c;
			}
		}

		if ( tn < 0.f ) {
			tn = 0.f;

			if ( -d < 0.f ) {
				sn = 0.f;
			}
			else if ( -d > a ) {
				sn = sd;
			}
			else {
				sn = -d;
				sd = a;
			}
		}
		else if ( tn > td ) {
			tn = td;

			if ( -d + b < 0.f ) {
				sn = 0.f;
			}
			else if ( -d + b > a ) {
				sn = sd;
			}
			else {
				sn = -d + b;
				sd = a;
			}
		}

		const auto sc = abs( sn ) < 0.00000001f ? 0.f : sn / sd;
		const auto tc = abs( tn ) < 0.00000001f ? 0.f : tn / td;

		return ( w + u * sc - v * tc ).length( );
	}

	__forceinline bool intersect_line_with_bb( const vec3_t& src, const vec3_t& dst, const vec3_t& min, const vec3_t& max ) {
		auto inside = true;
		char quadrant[3];
		int i;

		vec3_t candidate_plane;
		for (i = 0; i < 3; i++) {
			if (src[i] < min[i]) {
				quadrant[i] = 1;
				candidate_plane[i] = min[i];
				inside = false;
			}
			else if (src[i] > max[i]) {
				quadrant[i] = 0;
				candidate_plane[i] = max[i];
				inside = false;
			}
			else {
				quadrant[i] = 2;
			}
		}

		if (inside)
			return true;

		vec3_t max_t;
		for (i = 0; i < 3; i++) {
			if (quadrant[i] != 2 && dst[i] != 0.f)
				max_t[i] = (candidate_plane[i] - src[i]) / dst[i];
			else
				max_t[i] = -1.f;
		}

		auto which_plane = 0;
		for (i = 1; i < 3; i++) {
			if (max_t[which_plane] < max_t[i])
				which_plane = i;
		}

		if (max_t[which_plane] < 0.f)
			return false;

		for (i = 0; i < 3; i++) {
			if (which_plane != i) {
				float temp = src[i] + max_t[which_plane] * dst[i];
				if (temp < min[i] || temp > max[i]) {
					return false;
				}				
			}			
		}

		return true;
	}

	__forceinline bool to_screen( const vec3_t& world, const vec2_t& screen_size, const mat4x4_t& matrix, vec2_t& out ) {
		out.x = matrix[ 0 ][ 0 ] * world.x + matrix[ 0 ][ 1 ] * world.y + matrix[ 0 ][ 2 ] * world.z + matrix[ 0 ][ 3 ];
		out.y = matrix[ 1 ][ 0 ] * world.x + matrix[ 1 ][ 1 ] * world.y + matrix[ 1 ][ 2 ] * world.z + matrix[ 1 ][ 3 ];

		const auto w = matrix[ 3 ][ 0 ] * world.x + matrix[ 3 ][ 1 ] * world.y + matrix[ 3 ][ 2 ] * world.z + matrix[ 3 ][ 3 ];
		if ( w < 0.001f ) {
			out *= 100000.f;

			return false;
		}

		out /= w;

		out.x = ( screen_size.x / 2.f ) + ( out.x * screen_size.x ) / 2.f;
		out.y = ( screen_size.y / 2.f ) - ( out.y * screen_size.y ) / 2.f;

		return true;
	}
}