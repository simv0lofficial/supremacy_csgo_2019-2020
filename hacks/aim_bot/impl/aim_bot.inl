#pragma once

#include "../aim_bot.hpp"

namespace supremacy::hacks {
	__forceinline const aim_last_target_t& c_aim_bot::last_target() const {
		return m_last_target;
	}
}