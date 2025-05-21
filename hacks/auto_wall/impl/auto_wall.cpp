#include "../../../supremacy.hpp"

namespace supremacy::hacks {
	bool c_auto_wall::trace_to_exit(
		const vec3_t& src, const vec3_t& dir,
		const valve::trace_t& enter_trace, valve::trace_t& exit_trace
	) const {
		float dist{};
		valve::e_mask first_contents{};

		constexpr auto k_step_size = 4.f;
		constexpr auto k_max_dist = 90.f;

		while (dist <= k_max_dist) {
			dist += k_step_size;

			const auto out = src + (dir * dist);

			const auto cur_contents = valve::g_engine_trace->point_contents(out, valve::e_mask::shot_player);

			if (!first_contents)
				first_contents = cur_contents;

			if (cur_contents & valve::e_mask::shot_hull
				&& (!(cur_contents & valve::e_mask::contents_hitbox) || cur_contents == first_contents))
				continue;

			valve::g_engine_trace->trace_ray({ out, out - dir * k_step_size }, valve::e_mask::shot_player, nullptr, &exit_trace);

			if (exit_trace.m_start_solid
				&& exit_trace.m_surface.m_flags & valve::e_mask::surf_hitbox) {
				valve::trace_filter_simple_t trace_filter{ exit_trace.m_hit_entity, 0 };

				valve::g_engine_trace->trace_ray(
					{ out, src }, valve::e_mask::shot_hull,
					reinterpret_cast<valve::trace_filter_t*>(&trace_filter), &exit_trace
				);

				if (exit_trace.hit()
					&& !exit_trace.m_start_solid)
					return true;

				continue;
			}

			if (!exit_trace.hit()
				|| exit_trace.m_start_solid) {
				if (enter_trace.m_hit_entity
					&& enter_trace.m_hit_entity->index()
					&& enter_trace.m_hit_entity->breakable()) {
					exit_trace = enter_trace;
					exit_trace.m_end_pos = src + dir;

					return true;
				}

				continue;
			}

			if (exit_trace.m_surface.m_flags & valve::e_mask::surf_nodraw) {
				if (exit_trace.m_hit_entity->breakable()
					&& enter_trace.m_hit_entity->breakable())
					return true;

				if (!(enter_trace.m_surface.m_flags & valve::e_mask::surf_nodraw))
					continue;
			}

			if (exit_trace.m_plane.m_normal.dot(dir) <= 1.f)
				return true;
		}

		return false;
	}

	void c_auto_wall::clip_trace_to_player(
		const vec3_t& src, const vec3_t& dst, valve::trace_t& trace,
		valve::c_player* const player, const valve::should_hit_fn_t& should_hit_fn
	) const {
		if (should_hit_fn
			&& !should_hit_fn(player, valve::e_mask::shot_player))
			return;

		const auto pos = player->origin() + (player->obb_min() + player->obb_max()) * 0.5f;
		const auto to = pos - src;

		auto dir = src - dst;
		const auto len = dir.normalize();
		const auto range_along = dir.dot(to);

		const auto range =
			range_along < 0.f ? -(to).length()
			: range_along > len ? -(pos - dst).length()
			: (pos - (src + dir * range_along)).length();

		if (range > 60.f)
			return;

		valve::trace_t new_trace{};

		valve::g_engine_trace->clip_ray_to_entity({ src, dst }, valve::e_mask::shot_player, player, &new_trace);

		if (new_trace.m_fraction > trace.m_fraction)
			return;

		trace = new_trace;
	}

	bool c_auto_wall::handle_bullet_penetration(
		valve::c_player* const shooter, const valve::weapon_data_t* const wpn_data,
		const valve::trace_t& enter_trace, vec3_t& src, const vec3_t& dir, int& pen_count,
		float& cur_dmg, const float pen_modifier
	) const {
		if (pen_count <= 0
			|| wpn_data->m_penetration <= 0.f)
			return false;

		valve::trace_t exit_trace{};

		if (!trace_to_exit(enter_trace.m_end_pos, dir, enter_trace, exit_trace)
			&& !(valve::g_engine_trace->point_contents(enter_trace.m_end_pos, valve::e_mask::shot_hull) & valve::e_mask::shot_hull))
			return false;

		auto final_dmg_modifier = 0.16f;
		float combined_pen_modifier{};

		const auto exit_surface_data = valve::g_surface_data->find(exit_trace.m_surface.m_surface_props);
		const auto enter_surface_data = valve::g_surface_data->find(enter_trace.m_surface.m_surface_props);

		if (enter_surface_data->m_game.m_material == 'G'
			|| enter_surface_data->m_game.m_material == 'Y') {
			final_dmg_modifier = 0.05f;
			combined_pen_modifier = 3.f;
		}
		else if (enter_trace.m_contents & valve::e_mask::contents_grate
			|| enter_trace.m_surface.m_flags & valve::e_mask::surf_nodraw) {
			final_dmg_modifier = 0.16f;
			combined_pen_modifier = 1.f;
		}
		else if (enter_trace.m_hit_entity
			&& g_context->cvars().m_ff_damage_reduction_bullets->get_float() == 0.f
			&& enter_surface_data->m_game.m_material == 'F'
			&& enter_trace.m_hit_entity->team() == shooter->team()) {
			const auto dmg_bullet_pen = g_context->cvars().m_ff_damage_bullet_penetration->get_float();
			if (dmg_bullet_pen == 0.f)
				return false;

			combined_pen_modifier = dmg_bullet_pen;
			final_dmg_modifier = 0.16f;
		}
		else {
			combined_pen_modifier = (
				enter_surface_data->m_game.m_penetration_modifier
				+ exit_surface_data->m_game.m_penetration_modifier
				) * 0.5f;

			final_dmg_modifier = 0.16f;
		}

		if (enter_surface_data->m_game.m_material == exit_surface_data->m_game.m_material) {
			if (exit_surface_data->m_game.m_material == 'U'
				|| exit_surface_data->m_game.m_material == 'W')
				combined_pen_modifier = 3.f;
			else if (exit_surface_data->m_game.m_material == 'L')
				combined_pen_modifier = 2.f;
		}

		const auto modifier = std::max(1.f / combined_pen_modifier, 0.f);
		const auto pen_dist = (exit_trace.m_end_pos - enter_trace.m_end_pos).length();

		const auto lost_dmg =
			cur_dmg * final_dmg_modifier
			+ pen_modifier * (modifier * 3.f)
			+ ((pen_dist * pen_dist) * modifier) / 24.f;

		if (lost_dmg > cur_dmg)
			return false;

		if (lost_dmg > 0.f)
			cur_dmg -= lost_dmg;

		if (cur_dmg < 1.f)
			return false;

		--pen_count;

		src = exit_trace.m_end_pos;

		return true;
	}

	void c_auto_wall::scale_dmg(
		valve::c_player* const player, float& dmg,
		const float armor_ratio, const int hitgroup
	) const {
		const auto has_heavy_armor = player->has_heavy_armor();

		switch (hitgroup) {
		case 1:
			dmg *= 4.f;

			if (has_heavy_armor)
				dmg *= 0.5f;

			break;
		case 3: dmg *= 1.25f; break;
		case 6:
		case 7:
			dmg *= 0.75f;

			break;
		}

		const auto armor_value = player->armor_value();
		if (!armor_value
			|| hitgroup < 0
			|| hitgroup > 5
			|| (hitgroup == 1 && !player->has_helmet()))
			return;

		auto heavy_ratio = 1.f, bonus_ratio = 0.5f, ratio = armor_ratio * 0.5f;

		if (has_heavy_armor) {
			ratio *= 0.2f;
			heavy_ratio = 0.25f;
			bonus_ratio = 0.33f;
		}

		auto dmg_to_hp = dmg * ratio;

		if (((dmg - dmg_to_hp) * (bonus_ratio * heavy_ratio)) > armor_value)
			dmg -= armor_value / bonus_ratio;
		else
			dmg = dmg_to_hp;
	}

	pen_data_t c_auto_wall::fire_bullet(
		valve::c_player* const shooter, valve::c_player* const target,
		const valve::weapon_data_t* const wpn_data, const bool is_taser, vec3_t src, const vec3_t& dst
	) const {
		const auto pen_modifier = std::max((3.f / wpn_data->m_penetration) * 1.25f, 0.f);

		float cur_dist{};

		pen_data_t data{};

		data.m_remaining_pen = 4;

		auto cur_dmg = static_cast<float>(wpn_data->m_dmg);

		auto dir = dst - src;

		dir.normalize();

		valve::trace_t trace{};
		valve::trace_filter_skip_two_entities_t trace_filter{};

		valve::c_player* last_hit_player{};

		auto max_dist = wpn_data->m_range;

		while (cur_dmg > 0.f) {
			max_dist -= cur_dist;

			const auto cur_dst = src + dir * max_dist;

			trace_filter.m_ignore_entity0 = shooter;
			trace_filter.m_ignore_entity1 = last_hit_player;

			valve::g_engine_trace->trace_ray(
				{ src, cur_dst }, valve::e_mask::shot_player,
				reinterpret_cast<valve::trace_filter_t*>(&trace_filter), &trace
			);

			if (target)
				clip_trace_to_player(src, cur_dst + dir * 40.f, trace, target, trace_filter.m_should_hit_fn);

			if (trace.m_fraction == 1.f)
				break;

			cur_dist += trace.m_fraction * max_dist;
			cur_dmg *= std::pow(wpn_data->m_range_modifier, cur_dist / 500.f);

			if (trace.m_hit_entity) {
				const auto is_player = trace.m_hit_entity->is_player();
				if ((trace.m_hit_entity == target || (is_player && trace.m_hit_entity->team() != shooter->team()))
					&& ((trace.m_hitgroup >= 1 && trace.m_hitgroup <= 7) || trace.m_hitgroup == 10)) {
					data.m_hit_player = static_cast<valve::c_player*>(trace.m_hit_entity);
					data.m_hitbox = trace.m_hitbox;
					data.m_hitgroup = trace.m_hitgroup;

					if (is_taser)
						data.m_hitgroup = 0;

					scale_dmg(data.m_hit_player, cur_dmg, wpn_data->m_armor_ratio, data.m_hitgroup);

					data.m_dmg = static_cast<int>(cur_dmg);

					return data;
				}

				last_hit_player =
					is_player ? static_cast<valve::c_player*>(trace.m_hit_entity) : nullptr;
			}
			else
				last_hit_player = nullptr;

			if (is_taser
				|| (cur_dist > 3000.f && wpn_data->m_penetration > 0.f))
				break;

			const auto enter_surface = valve::g_surface_data->find(trace.m_surface.m_surface_props);
			if (enter_surface->m_game.m_penetration_modifier < 0.1f
				|| !handle_bullet_penetration(shooter, wpn_data, trace, src, dir, data.m_remaining_pen, cur_dmg, pen_modifier))
				break;
		}

		return data;
	}

	pen_data_t c_auto_wall::fire_emulated(
		valve::c_player* const shooter, valve::c_player* const target, vec3_t src, const vec3_t& dst
	) const {
		static const auto wpn_data = []() {
			valve::weapon_data_t wpn_data{};

			wpn_data.m_dmg = 115;
			wpn_data.m_range = 8192.f;
			wpn_data.m_penetration = 2.5f;
			wpn_data.m_range_modifier = 0.99f;
			wpn_data.m_armor_ratio = 1.95f;

			return wpn_data;
		}();

		const auto pen_modifier = std::max((3.f / wpn_data.m_penetration) * 1.25f, 0.f);

		float cur_dist{};

		pen_data_t data{};

		data.m_remaining_pen = 4;

		auto cur_dmg = static_cast<float>(wpn_data.m_dmg);

		auto dir = dst - src;

		const auto max_dist = dir.normalize();

		valve::trace_t trace{};
		valve::trace_filter_skip_two_entities_t trace_filter{};
		valve::c_player* last_hit_player{};

		while (cur_dmg > 0.f) {
			const auto dist_remaining = wpn_data.m_range - cur_dist;

			const auto cur_dst = src + dir * dist_remaining;

			trace_filter.m_ignore_entity0 = shooter;
			trace_filter.m_ignore_entity1 = last_hit_player;

			valve::g_engine_trace->trace_ray(
				{ src, cur_dst }, valve::e_mask::shot_player,
				reinterpret_cast<valve::trace_filter_t*>(&trace_filter), &trace
			);

			if (target)
				clip_trace_to_player(src, cur_dst + dir * 40.f, trace, target, trace_filter.m_should_hit_fn);

			if (trace.m_fraction == 1.f
				|| (trace.m_end_pos - src).length() > max_dist)
				break;

			cur_dist += trace.m_fraction * dist_remaining;
			cur_dmg *= std::pow(wpn_data.m_range_modifier, cur_dist / 500.f);

			if (trace.m_hit_entity) {
				const auto is_player = trace.m_hit_entity->is_player();
				if (trace.m_hit_entity == target) {
					data.m_hit_player = static_cast<valve::c_player*>(trace.m_hit_entity);
					data.m_hitbox = trace.m_hitbox;
					data.m_hitgroup = trace.m_hitgroup;
					data.m_dmg = static_cast<int>(cur_dmg);

					return data;
				}

				last_hit_player =
					is_player ? static_cast<valve::c_player*>(trace.m_hit_entity) : nullptr;
			}
			else
				last_hit_player = nullptr;

			if (cur_dist > 3000.f
				&& wpn_data.m_penetration > 0.f)
				break;

			const auto enter_surface = valve::g_surface_data->find(trace.m_surface.m_surface_props);
			if (enter_surface->m_game.m_penetration_modifier < 0.1f
				|| !handle_bullet_penetration(shooter, &wpn_data, trace, src, dir, data.m_remaining_pen, cur_dmg, pen_modifier))
				break;
		}

		return data;
	}
}