#pragma once

using vec2_t = ImVec2;

using vec3_t = ImVec3;

using qangle_t = ImVec3;

struct mat3x4_t {
	__forceinline float* operator []( const int i ) { return m_matrix[ i ]; }

	__forceinline const float* operator []( const int i ) const { return m_matrix[ i ]; }

	__forceinline mat3x4_t operator+(const mat3x4_t& other) const {
		mat3x4_t ret;
		auto& m = m_matrix;
		for (int i = 0; i < 12; i++) {
			((float*)ret.m_matrix)[i] = ((float*)m)[i] + ((float*)other.m_matrix)[i];
		}
		return ret;
	}

	__forceinline mat3x4_t operator-(const mat3x4_t& other) const {
		mat3x4_t ret;
		auto& m = m_matrix;
		for (int i = 0; i < 12; i++) {
			((float*)ret.m_matrix)[i] = ((float*)m)[i] - ((float*)other.m_matrix)[i];
		}
		return ret;
	}

	__forceinline mat3x4_t operator*(const float& other) const {
		mat3x4_t ret;
		auto& m = m_matrix;
		for (int i = 0; i < 12; i++) {
			((float*)ret.m_matrix)[i] = ((float*)m)[i] * other;
		}
		return ret;
	}

	float m_matrix[ 3u ][ 4u ]{};

	__forceinline vec3_t get_origin() { return vec3_t(m_matrix[0][3], m_matrix[1][3], m_matrix[2][3]);	}
};

struct mat4x4_t {
	__forceinline float* operator []( const int i ) { return m_matrix[ i ]; }

	__forceinline const float* operator []( const int i ) const { return m_matrix[ i ]; }

	float m_matrix[ 4u ][ 4u ]{};
};

namespace math {
	constexpr auto k_pi = 3.14159265358979323846f;

	constexpr auto k_pi2 = k_pi * 2.f;

	constexpr auto k_rad_pi = 180.f / k_pi;

	constexpr auto k_deg_pi = k_pi / 180.f;

	__forceinline void sin_cos( const float rad, float& sin, float& cos );

	__forceinline float to_deg( const float rad );

	__forceinline float to_rad( const float deg );

	__forceinline float angle_diff( float src, float dst );

	__forceinline float approach_angle(float target, float value, float speed);

	__forceinline vec3_t calculate_angle(const vec3_t& src, const vec3_t& dst);

	__forceinline void concat_transforms( const mat3x4_t& in0, const mat3x4_t& in1, mat3x4_t& out );

	__forceinline void angle_vectors(
		const qangle_t& angle, vec3_t* const fwd, vec3_t* const right = nullptr, vec3_t* const up = nullptr
	);

	__forceinline void vector_angles( const vec3_t& in, qangle_t& out );

	__forceinline float calc_fov( const qangle_t& view_angles, const vec3_t& src, const vec3_t& dst );

	__forceinline void vector_transform( const vec3_t& in, const mat3x4_t& matrix, vec3_t& out );

	__forceinline void vector_rotate( const vec3_t& in, const mat3x4_t& matrix, vec3_t& out );

	__forceinline void vector_i_transform( const vec3_t& in, const mat3x4_t& matrix, vec3_t& out );

	__forceinline void vector_i_rotate( const vec3_t& in, const mat3x4_t& matrix, vec3_t& out );

	__forceinline mat3x4_t vector_matrix( const vec3_t& in );

	__forceinline float segment_to_segment( const vec3_t& s1, const vec3_t& s2, const vec3_t& k1, const vec3_t& k2 );

	__forceinline bool intersect_line_with_bb( const vec3_t& src, const vec3_t& dst, const vec3_t& min, const vec3_t& max );

	__forceinline bool to_screen( const vec3_t& world, const vec2_t& screen_size, const mat4x4_t& matrix, vec2_t& out );

	template <class t>
	__forceinline t lerp(float percent, t const& a, t const& b)	{
		return a + (b - a) * percent;
	}

	template < class T >
	__forceinline T anim_lerp(const T& current, const T& target, const int progress, const int maximum) {
		return current + (((target - current) / maximum) * progress);
	}

	template < typename T >
	__forceinline T hermite_spline(
		T p1,
		T p2,
		T d1,
		T d2,
		float t) {
		float t_sqr = t * t;
		float t_cube = t * t_sqr;

		float b1 = 2.f * t_cube - 3.f * t_sqr + 1.f;
		float b2 = 1.f - b1;
		float b3 = t_cube - 2 * t_sqr + t;
		float b4 = t_cube - t_sqr;

		T output;
		output = p1 * b1;
		output += p2 * b2;
		output += d1 * b3;
		output += d2 * b4;

		return output;
	}

	template < typename T >
	__forceinline T hermite_spline(T p0, T p1, T p2, float t) {
		return hermite_spline(p1, p2, p1 - p0, p2 - p1, t);
	}

	__forceinline float simple_spline(float value) {
		float value_squared = value * value;
		return (3 * value_squared - 2 * value_squared * value);
	}

	__forceinline float simple_spline_remap_val_clamped(float val, float a, float b, float c, float d) {
		if (a == b)
			return val >= b ? d : c;

		float c_val = (val - a) / (b - a);
		c_val = std::clamp(c_val, 0.f, 1.f);
		return c + (d - c) * simple_spline(c_val);
	}
}

#include "impl/math.inl"