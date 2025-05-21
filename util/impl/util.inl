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

	__forceinline const char* translate_safe_points(const int intersections, const int intersections_low) {
		if (intersections >= 3)
			return ("strict");

		if (intersections == 2) {
			if (intersections_low >= 3)
				return ("normal | strict");

			return ("normal");
		}

		if (intersections_low >= 3)
			return ("false | strict");

		if (intersections_low == 2)
			return ("false | normal");

		return ("false");
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