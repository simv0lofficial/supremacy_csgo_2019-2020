#pragma once

namespace util {
	template <typename T> T* get_interface(const HMODULE module, std::string szinterface) {
		using create_interface_fn = T * (*)(const char* szname, int ret);
		const create_interface_fn create_interface = reinterpret_cast<create_interface_fn>(GetProcAddress(module, xorstr_("CreateInterface")));

		return create_interface(szinterface.c_str(), 0);
	}

	__forceinline const char* translate_hitgroup(const int index);

	__forceinline const char* translate_safe_points(const int intersections_120, const int intersections_30, const int intersections_15);

	__forceinline const char* bool_as_text(bool b);

	__forceinline std::string to_utf8(const std::wstring_view str);

	__forceinline std::wstring to_utf16(const std::string_view str);

	template< typename o = void*, typename i = void* >
	__forceinline o force_cast(i in) {
		union { i in; o out; }
		u = { in };
		return u.out;
	};

	class c_notify {
	public:
		void print_notify(bool print_in_console, bool print_notify, const std::uint32_t& color, const char* format, ...);
		void print_logo();
	};

	inline const auto g_notify = std::make_unique< c_notify >();

	class c_color {
	public:
		int a, r, g, b;
		c_color() = default;
		c_color(const int r, const int g, const int b, const int a = 255) {
			this->r = r;
			this->g = g;
			this->b = b;
			this->a = a;
		}
		c_color(c_color col, const int a) {
			this->r = col.r;
			this->g = col.g;
			this->b = col.b;
			this->a = a;
		}
		c_color(float col, const int a) {
			this->r = col;
			this->g = col;
			this->b = col;
			this->a = a;
		}

		explicit c_color(const uint32_t color) {
			this->a = (color >> 24) & 0xff;
			this->r = (color >> 16) & 0xff;
			this->g = (color >> 8) & 0xff;
			this->b = (color & 0xff);
		}

		static c_color from_uint(const uint32_t uint) {
			return c_color(uint);
		}

		static D3DCOLOR from_color(const c_color col) {
			return D3DCOLOR_ARGB(col.a, col.r, col.g, col.b);
		}

		inline float r_base() const { return r / 255.f; }
		inline float g_base() const { return g / 255.f; }
		inline float b_base() const { return b / 255.f; }
		inline float a_base() const { return a / 255.f; }

		std::array<float, 3U> base()
		{
			std::array<float, 3U> arr_base_color = { };
			arr_base_color.at(0) = r / 255.f;
			arr_base_color.at(1) = g / 255.f;
			arr_base_color.at(2) = b / 255.f;
			return arr_base_color;
		}

		__forceinline unsigned long& u32()
		{
			unsigned char _color[4];
			_color[0] = (unsigned char)(r);
			_color[1] = (unsigned char)(g);
			_color[2] = (unsigned char)(b);
			_color[3] = (unsigned char)(a);
			return *((unsigned long*)_color);
		}

		__forceinline c_color override_alpha(int alpha, bool adaptive = false) {
			return c_color(r, g, b, adaptive ? static_cast<int>(static_cast<float>(alpha) * (a / 255.f)) : alpha);
		}

		static c_color hsl_to_rgb(float h, float s, float l) {
			float q;

			if (l < 0.5f)
				q = l * (s + 1.f);

			else
				q = l + s - (l * s);

			float p = 2 * l - q;

			float rgb[3];
			rgb[0] = h + (1.f / 3.f);
			rgb[1] = h;
			rgb[2] = h - (1.f / 3.f);

			for (int i = 0; i < 3; ++i) {
				if (rgb[i] < 0)
					rgb[i] += 1.f;

				if (rgb[i] > 1)
					rgb[i] -= 1.f;

				if (rgb[i] < (1.f / 6.f))
					rgb[i] = p + ((q - p) * 6 * rgb[i]);
				else if (rgb[i] < 0.5f)
					rgb[i] = q;
				else if (rgb[i] < (2.f / 3.f))
					rgb[i] = p + ((q - p) * 6 * ((2.f / 3.f) - rgb[i]));
				else
					rgb[i] = p;
			}

			return {
				int(rgb[0] * 255.f),
				int(rgb[1] * 255.f),
				int(rgb[2] * 255.f)
			};
		}
	};

	inline const auto g_color = std::make_unique< c_color >();
}

#include "impl/util.inl"