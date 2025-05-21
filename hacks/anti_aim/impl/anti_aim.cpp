#include "../../../supremacy.hpp"

namespace supremacy::hacks {
	void c_anti_aim::at_target(float& yaw) const {
		if (!sdk::g_config_system->yaw_base)
			return;

		valve::c_player* best_player{};
		auto best_value = std::numeric_limits< float >::max();

		float total_yaw{};
		int valid_count{};

		for (auto i = 1; i <= valve::g_global_vars->m_max_clients; ++i) {
			const auto player = static_cast<valve::c_player*>(
				valve::g_entity_list->find_entity(i)
				);
			if (!player
				|| player->dormant()
				|| !player->alive()
				|| player->friendly()
				|| player == valve::g_local_player)
				continue;

			switch (sdk::g_config_system->yaw_base) {
			case 1: {
				const auto dist = (valve::g_local_player->origin() - player->origin()).length();
				if (dist >= best_value)
					continue;

				best_value = dist;
				best_player = player;
			} break;
			case 2: {
				++valid_count;

				const auto x = player->origin().x - valve::g_local_player->origin().x;
				const auto y = player->origin().y - valve::g_local_player->origin().y;

				if (x == 0.f
					&& y == 0.f)
					continue;

				total_yaw += math::to_deg(std::atan2(y, x));
			} break;
			case 3: {
				const auto fov = math::calc_fov(g_context->view_angles(), g_context->shoot_pos(), player->world_space_center());
				if (fov >= best_value)
					continue;

				best_value = fov;
				best_player = player;
			} break;
			}
		}

		if (sdk::g_config_system->yaw_base == 2) {
			if (valid_count)
				yaw = total_yaw / valid_count;

			return;
		}

		if (!best_player)
			return;

		const auto x = best_player->origin().x - valve::g_local_player->origin().x;
		const auto y = best_player->origin().y - valve::g_local_player->origin().y;

		yaw = x == 0.f && y == 0.f ? 0.f : math::to_deg(std::atan2(y, x));
	}

	bool c_anti_aim::auto_direction(float& yaw) {
		bool should_freestand = sdk::g_config_system->freestanding && key_handler::check_key(sdk::g_config_system->freestanding_key, sdk::g_config_system->freestanding_key_style);

		if (!sdk::g_config_system->automatic_side_selection && !should_freestand)
			return false;		

		constexpr float STEP{ 4.f };
		constexpr float RANGE{ 32.f };
		const auto& local_shoot_pos = g_context->shoot_pos();

		struct auto_target_t { float fov; valve::c_player* player; };
		auto_target_t target{ 180.f + 1.f, nullptr };

		for (auto i = 1; i <= valve::g_global_vars->m_max_clients; ++i) {
			const auto player = static_cast<valve::c_player*>(
				valve::g_entity_list->find_entity(i)
				);

			if (!player
				|| player->dormant()
				|| !player->alive()
				|| player->friendly()
				|| player == valve::g_local_player)
				continue;

			const auto fov = math::calc_fov(g_context->view_angles(), g_context->shoot_pos(), player->world_space_center());

			if (fov < target.fov) {
				target.fov = fov;
				target.player = player;
			}
		}

		if (!target.player) {
			if (should_freestand) {
				yaw = m_auto_yaw = std::remainder(yaw - 180.f, 360.f);
				return true;
			}

			return false;
		}

		std::vector< c_adaptive_angle > angles{ };
		angles.emplace_back(yaw - 180.f);
		angles.emplace_back(yaw + 90.f);
		angles.emplace_back(yaw - 90.f);				

		bool valid{ false };

		auto start = target.player->origin() + target.player->view_offset();
		for (auto it = angles.begin(); it != angles.end(); ++it) {
			const auto rad_yaw = math::to_rad(it->m_yaw);
			vec3_t end{ local_shoot_pos.x + std::cos(rad_yaw) * RANGE,
				local_shoot_pos.y + std::sin(rad_yaw) * RANGE,
				local_shoot_pos.z };

			vec3_t dir = end - start;
			float len = dir.normalize();

			if (len <= 0.f)
				continue;

			for (float i{ 0.f }; i < len; i += STEP) {
				vec3_t point = start + (dir * i);

				int contents = valve::g_engine_trace->point_contents(point, valve::e_mask::shot_hull);

				if (!(contents & valve::e_mask::shot_hull))
					continue;

				float mult = 1.f;

				if (i > (len * 0.5f))
					mult = 1.25f;

				if (i > (len * 0.75f))
					mult = 1.25f;

				if (i > (len * 0.9f))
					mult = 2.f;

				it->m_dist += (STEP * mult);

				valid = true;
			}
		}

		if (!valid) {
			if (should_freestand) {
				yaw = m_auto_yaw = std::remainder(yaw - 180.f, 360.f);
				return true;
			}

			return false;
		}

		if (std::abs(angles.at(0u).m_dist - angles.at(1u).m_dist) >= 15.f
			|| std::abs(angles.at(0u).m_dist - angles.at(2u).m_dist) >= 15.f) {
			std::sort(angles.begin(), angles.end(),
				[](const c_adaptive_angle& a, const c_adaptive_angle& b) {
				return a.m_dist > b.m_dist;
			});

			c_adaptive_angle* best = &angles.front();
			if (best->m_dist > 400.f)
				return false;						

			if (should_freestand)
				m_auto_yaw = std::remainder(best->m_yaw, 360.f);

			if (sdk::g_config_system->automatic_side_selection) {
				const auto diff = math::angle_diff(yaw, best->m_yaw);
				m_auto_dir_side = (sdk::g_config_system->automatic_side_selection == 1 ? diff >= 0.f : diff < 0.f);

				if (std::abs(sdk::g_config_system->yaw) > 90.f)
					m_auto_dir_side = -m_auto_dir_side;

				if (should_freestand)
					m_auto_dir_side = 0;

				m_auto_dir_side += 1;
			}
		}

		if (should_freestand) {
			yaw = m_auto_yaw;
			return true;
		}

		return false;
	}

	int c_anti_aim::select_side() {
		if (!valve::g_client_state->m_choked_cmds
			&& m_prev_tick_count != valve::g_global_vars->m_tick_count)
			++m_side_counter;

		m_prev_tick_count = valve::g_global_vars->m_tick_count;

		const auto shot_cmd = g_anim_sync->local_data().m_shot_cmd_number;
		if (shot_cmd > valve::g_client_state->m_last_cmd_out
			&& shot_cmd <= (valve::g_client_state->m_last_cmd_out + valve::g_client_state->m_choked_cmds + 1)) {
			switch (sdk::g_config_system->on_shot_side) {
			case 1: return 2; break;
			case 2: return 1; break;
			case 3: {
				const auto& local_data = g_eng_pred->local_data().at(valve::g_client_state->m_last_cmd_out % 150);

				return (math::angle_diff(local_data.m_anim_state.m_eye_yaw, local_data.m_anim_state.m_foot_yaw) <= 0.f) + 1;
			} break;
			case 4: return m_on_shot_side_counter = (m_on_shot_side_counter != 2) + 1; break;
			}

			return 0;
		}

		if (sdk::g_config_system->body_yaw) {
			if (sdk::g_config_system->body_yaw == 2)
				return 2 - (m_side_counter % 2);

			if (sdk::g_config_system->automatic_side_selection
				&& (m_manual_side == 1 || m_manual_side == 2))
				m_auto_dir_side = (sdk::g_config_system->automatic_side_selection == 1 ? 1 : 2);

			if (m_auto_dir_side)
				return m_auto_dir_side;

			return key_handler::check_key(sdk::g_config_system->body_yaw_key, sdk::g_config_system->body_yaw_key_style) ? 2 : 1;
		}

		return 0;
	}

	void c_anti_aim::choke(valve::user_cmd_t* const user_cmd) {
		if (g_exploits->charged()
			|| g_context->freeze_time()
			|| g_movement->should_fake_duck()
			|| valve::g_client_state->m_choked_cmds > 14
			|| !(g_context->flags() & e_context_flags::can_choke)
			|| valve::g_local_player->flags() & valve::e_ent_flags::frozen)
			return;

		if (enabled(user_cmd)
			&& valve::g_client_state->m_choked_cmds < 1)
			g_context->flags() |= e_context_flags::choke;

		if (!sdk::g_config_system->fake_lag)
			return;

		bool should_choke{};
		if (sdk::g_config_system->conditions[0])
			should_choke = true;

		if (!should_choke
			&& sdk::g_config_system->conditions[1]
			&& valve::g_local_player->velocity().length() > 2.f)
			should_choke = true;

		if (!should_choke
			&& sdk::g_config_system->conditions[2]
			&& !(valve::g_local_player->flags() & valve::e_ent_flags::on_ground))
			should_choke = true;

		if (!should_choke			
			&& g_aim_bot->is_peeking())
			should_choke = true;

		if (should_choke
			&& m_choke_start_cmd_number != valve::g_client_state->m_last_cmd_out)
			m_choke_start_cmd_number = valve::g_client_state->m_last_cmd_out;

		if (g_context->flags() & e_context_flags::choke
			|| (!should_choke && m_choke_start_cmd_number != valve::g_client_state->m_last_cmd_out))
			return;

		switch (sdk::g_config_system->amount) {
		case 0:
		{
			int wish_ticks{};
			int adaptive_ticks{};
			bool should_choked{};
			const int units_per_tick = static_cast<int>(valve::to_time(valve::g_local_player->velocity().length()));

			if (wish_ticks * units_per_tick > 68)
				should_choked = valve::g_client_state->m_choked_cmds < wish_ticks;
			else if ((adaptive_ticks - 1) * units_per_tick > 68) {
				++wish_ticks;
				should_choked = valve::g_client_state->m_choked_cmds < wish_ticks;
			}
			else if (adaptive_ticks * units_per_tick > 68)
				should_choked = valve::g_client_state->m_choked_cmds < wish_ticks + 2;
			else if ((adaptive_ticks + 1) * units_per_tick > 68)
				should_choked = valve::g_client_state->m_choked_cmds < wish_ticks + 3;
			else {
				if ((adaptive_ticks + 2) * units_per_tick <= 68) {
					adaptive_ticks += 5;
					wish_ticks += 5;

					if (adaptive_ticks > 16) {
						should_choked = valve::g_client_state->m_choked_cmds < wish_ticks;

						break;
					}
				}

				should_choked = valve::g_client_state->m_choked_cmds < wish_ticks + 4;
			}

			if (!should_choked)
				g_context->flags() &= ~e_context_flags::choke;
			else
				g_context->flags() |= e_context_flags::choke;
		}
		break;
		case 1:
			g_context->flags() |= e_context_flags::choke;
			break;
		case 2:
			if (user_cmd->m_number % 30 >= 15)
				g_context->flags() |= e_context_flags::choke;

			break;
		case 3:
			if (g_context->broke_lc())
				g_context->flags() &= ~e_context_flags::choke;
			else
				g_context->flags() |= e_context_flags::choke;

			break;
		}

		if (valve::g_client_state->m_choked_cmds >= sdk::g_config_system->factor_limit
			&& !(sdk::g_config_system->force_when_peeking && g_aim_bot->is_peeking()))
			g_context->flags() &= ~e_context_flags::choke;
	}

	void c_anti_aim::select_yaw(float& yaw, const int side) {
		if (!valve::g_client_state->m_choked_cmds)
			m_choke_cycle_switch = !m_choke_cycle_switch;

		yaw = g_context->view_angles().y;

		m_auto_yaw = std::remainder(yaw - 180.f, 360.f);
		m_auto_dir_side = 0;

		if (m_manual_side == 1)
			yaw += 90.f;
		else if (m_manual_side == 2)
			yaw -= 90.f;
		else if (m_manual_side == 3)
			yaw += 180.f;
		else {
			at_target(yaw);
			if (!auto_direction(yaw))
				yaw += sdk::g_config_system->yaw;

			yaw += (sdk::g_config_system->jitter_switch ? g_context->addresses().m_random_int(-sdk::g_config_system->jitter_range * 0.5f, sdk::g_config_system->jitter_range * 0.5f) : sdk::g_config_system->jitter_range * (m_choke_cycle_switch ? 0.5f : -0.5f))
				+ ((sdk::g_config_system->rotate_range && sdk::g_config_system->rotate_speed) ? std::fmod(valve::g_global_vars->m_cur_time * (sdk::g_config_system->rotate_speed * 20.f), sdk::g_config_system->rotate_range) - sdk::g_config_system->rotate_range * 0.5f : 0.f)
				+ (sdk::g_config_system->body_yaw ? (side == 2 ? sdk::g_config_system->inverted_body_lean : sdk::g_config_system->body_lean) : 0.f);
		}
	}

	void c_anti_aim::set_pitch(valve::user_cmd_t& user_cmd) {
		const auto anim_state = valve::g_local_player->anim_state();
		if (!anim_state
			|| !enabled(&user_cmd)
			|| (g_context->flags() & e_context_flags::can_shoot
				&& g_context->will_shoot(valve::g_local_player->weapon(), user_cmd)))
			return;

		user_cmd.m_view_angles.x = sdk::g_config_system->pitch;
	}

	void c_anti_aim::process(valve::user_cmd_t& user_cmd, const float yaw, const int side, const int choked_cmds) const {
		const auto anim_state = valve::g_local_player->anim_state();
		if (!anim_state
			|| !enabled(&user_cmd))
			return;

		const auto shot_cmd_number = g_anim_sync->local_data().m_shot_cmd_number;
		if (user_cmd.m_number == shot_cmd_number)
			return;

		if (!side) {
			user_cmd.m_view_angles.y = std::remainder(yaw, 360.f);

			return;
		}

		const auto in_shot = shot_cmd_number > valve::g_client_state->m_last_cmd_out
			&& shot_cmd_number <= (valve::g_client_state->m_last_cmd_out + valve::g_client_state->m_choked_cmds + 1);

		const auto speed = ((anim_state->m_walk_to_run_transition * 20.f) + 30.f) * valve::g_global_vars->m_interval_per_tick;

		auto delta = side == 1 ? 60.f : -60.f;
		if (!in_shot) {
			const auto upper_limit = sdk::g_config_system->yaw_limit + speed;
			if (delta > upper_limit)
				delta = upper_limit;
			else {
				const auto lower_limit = (sdk::g_config_system->yaw_limit * -1.f) - speed;
				if (lower_limit > delta)
					delta = lower_limit;
			}
		}

		if (choked_cmds <= 0) {
			user_cmd.m_view_angles.y = std::remainder(yaw, 360.f);

			return;
		}

		const auto& local_data = g_eng_pred->local_data().at(valve::g_client_state->m_last_cmd_out % 150);
		if (std::abs(delta - 5.f) > std::abs(math::angle_diff(local_data.m_anim_state.m_eye_yaw, local_data.m_anim_state.m_foot_yaw)))
			delta = std::copysign(120.f, delta);

		user_cmd.m_view_angles.y = std::remainder(yaw - delta, 360.f);
	}
}