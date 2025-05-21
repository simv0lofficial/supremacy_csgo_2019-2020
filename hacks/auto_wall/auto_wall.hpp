#pragma once

namespace supremacy::hacks {
	struct pen_data_t {
		valve::c_player* m_hit_player{};
		int					m_dmg{}, m_hitbox{}, m_hitgroup{}, m_remaining_pen{};
	};

	class c_auto_wall {
	private:
		bool trace_to_exit(
			const vec3_t& src, const vec3_t& dir,
			const valve::trace_t& enter_trace, valve::trace_t& exit_trace
		) const;

		void clip_trace_to_player(
			const vec3_t& src, const vec3_t& dst, valve::trace_t& trace,
			valve::c_player* const player, const valve::should_hit_fn_t& should_hit_fn
		) const;

		bool handle_bullet_penetration(
			valve::c_player* const shooter, const valve::weapon_data_t* const wpn_data,
			const valve::trace_t& enter_trace, vec3_t& src, const vec3_t& dir, int& pen_count,
			float& cur_dmg, const float pen_modifier
		) const;
	public:
		void scale_dmg(
			valve::c_player* const player, float& dmg,
			const float armor_ratio, const int hitgroup
		) const;

		pen_data_t fire_bullet(
			valve::c_player* const shooter, valve::c_player* const target,
			const valve::weapon_data_t* const wpn_data, const bool is_taser, vec3_t src, const vec3_t& dst
		) const;

		pen_data_t fire_emulated(
			valve::c_player* const shooter, valve::c_player* const target, vec3_t src, const vec3_t& dst
		) const;
	};

	inline const auto g_auto_wall = std::make_unique< c_auto_wall >();
}