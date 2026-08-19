#pragma once

namespace util {
	__forceinline const char* translate_hitgroup(const int index) {
		switch (index) {
		case 0: return ("generic"); break;
		case 1: return ("head"); break;
		case 2: return ("chest"); break;
		case 3: return ("stomach"); break;
		case 4: return ("left arm"); break;
		case 5: return ("right arm"); break;
		case 6: return ("left leg"); break;
		case 7: return ("right leg"); break;
		case 8: return ("neck"); break;
		}

		return ("generic");
	}

	__forceinline const char* translate_safe_points(const int intersections_120, const int intersections_30, const int intersections_15) {
		if (intersections_120 >= 3)
			return ("+-120");

		if (intersections_30 >= 3)
			return ("+-30");

		if (intersections_15 >= 3)
			return ("+-15");

		return ("0");
	}

	__forceinline const char* bool_as_text(bool b) {
		return b ? "true" : "false";
	}

	__forceinline std::string to_utf8(const std::wstring_view str) {
		if (str.empty())
			return {};

		const auto len = WideCharToMultiByte(CP_UTF8, 0, str.data(), str.size(), 0, 0, 0, 0);

		std::string ret{};

		ret.resize(len);

		WideCharToMultiByte(CP_UTF8, 0, str.data(), str.size(), ret.data(), len, 0, 0);

		return ret;
	}

	__forceinline std::wstring to_utf16(const std::string_view str) {
		if (str.empty())
			return {};

		const auto len = MultiByteToWideChar(CP_UTF8, 0, str.data(), str.size(), nullptr, 0);

		std::wstring ret{};

		ret.resize(len);

		MultiByteToWideChar(CP_UTF8, 0, str.data(), str.size(), ret.data(), len);

		return ret;
	}
}