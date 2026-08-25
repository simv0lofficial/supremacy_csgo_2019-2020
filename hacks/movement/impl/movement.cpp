#include "../../../supremacy.hpp"

namespace supremacy::hacks {
	void c_movement::correct_mouse(valve::user_cmd_t& user_cmd) {
		qangle_t prev_view_angles{};
		prev_view_angles = valve::g_engine->view_angles();

		float delta_x = std::remainderf(user_cmd.m_view_angles.x - prev_view_angles.x, 360.f);
		float delta_y = std::remainderf(user_cmd.m_view_angles.y - prev_view_angles.y, 360.f);

		if (delta_x != 0.f) {
			float mouse_y = -((delta_x / g_context->cvars().m_pitch->get_float()) / g_context->cvars().m_sensitivity->get_float());
			short mousedy;
			if (mouse_y <= 32767.f) {
				if (mouse_y >= -32768.f) {
					if (mouse_y >= 1.f
						|| mouse_y < 0.f) {
						if (mouse_y <= -1.f
							|| mouse_y > 0.f)
							mousedy = static_cast<short>(mouse_y);
						else
							mousedy = -1;
					}
					else
						mousedy = 1;
				}
				else
					mousedy = 0x8000u;
			}
			else
				mousedy = 0x7FFF;

			user_cmd.m_mouse_dy = mousedy;
		}

		if (delta_y != 0.f) {
			float mouse_x = -((delta_y / g_context->cvars().m_yaw->get_float()) / g_context->cvars().m_sensitivity->get_float());
			short mousedx;
			if (mouse_x <= 32767.f) {
				if (mouse_x >= -32768.f) {
					if (mouse_x >= 1.f
						|| mouse_x < 0.f) {
						if (mouse_x <= -1.f
							|| mouse_x > 0.f)
							mousedx = static_cast<short>(mouse_x);
						else
							mousedx = -1;
					}
					else
						mousedx = 1;
				}
				else
					mousedx = 0x8000u;
			}
			else
				mousedx = 0x7FFF;

			user_cmd.m_mouse_dx = mousedx;
		}
	}

	void c_movement::accelerate(
		const valve::user_cmd_t& user_cmd, const vec3_t& wishdir,
		const float wishspeed, vec3_t& velocity, float acceleration
	) const {
		const auto cur_speed = velocity.dot(wishdir);

		const auto add_speed = wishspeed - cur_speed;
		if (add_speed <= 0.f)
			return;

		const auto v57 = std::max(cur_speed, 0.f);

		const auto ducking =
			user_cmd.m_buttons & valve::e_buttons::in_duck
			|| valve::g_local_player->ducking()
			|| valve::g_local_player->flags() & valve::e_ent_flags::ducking;

		auto v20 = true;
		if (ducking
			|| !(user_cmd.m_buttons & valve::e_buttons::in_speed))
			v20 = false;

		auto finalwishspeed = std::max(wishspeed, 260.f);
		auto abs_finalwishspeed = finalwishspeed;

		const auto weapon = g_context->weapon();

		bool slow_down_to_fast_nigga{};

		if (weapon
			&& g_context->cvars().m_sv_accelerate_use_weapon_speed->get_bool()) {
			const auto item_index = static_cast<std::uint16_t>(weapon->item_index());
			if (weapon->zoom_lvl() > 0
				&& (item_index == 11 || item_index == 38 || item_index == 9 || item_index == 8 || item_index == 39 || item_index == 40))
				slow_down_to_fast_nigga = (m_max_weapon_speed * 0.52f) < 110.f;

			const auto modifier = std::min(1.f, m_max_weapon_speed / 260.f);

			abs_finalwishspeed *= modifier;

			if ((!ducking && !v20)
				|| slow_down_to_fast_nigga)
				finalwishspeed *= modifier;
		}

		if (ducking) {
			if (!slow_down_to_fast_nigga)
				finalwishspeed *= 0.34f;

			abs_finalwishspeed *= 0.34f;
		}

		if (v20) {
			if (!slow_down_to_fast_nigga)
				finalwishspeed *= 0.52f;

			abs_finalwishspeed *= 0.52f;

			const auto abs_finalwishspeed_minus5 = abs_finalwishspeed - 5.f;
			if (v57 < abs_finalwishspeed_minus5) {
				const auto v30 =
					std::max(v57 - abs_finalwishspeed_minus5, 0.f)
					/ std::max(abs_finalwishspeed - abs_finalwishspeed_minus5, 0.f);

				const auto v27 = 1.f - v30;
				if (v27 >= 0.f)
					acceleration = std::min(v27, 1.f) * acceleration;
				else
					acceleration = 0.f;
			}
		}

		const auto v33 = std::min(
			add_speed,
			((valve::g_global_vars->m_interval_per_tick * acceleration) * finalwishspeed)
			* valve::g_local_player->surface_friction()
		);

		velocity += wishdir * v33;

		const auto len = velocity.length();
		if (len
			&& len > m_max_weapon_speed)
			velocity *= m_max_weapon_speed / len;

	}

	void c_movement::walk_move(
		const valve::user_cmd_t& user_cmd, vec3_t& move,
		vec3_t& fwd, vec3_t& right, vec3_t& velocity
	) const {
		if (fwd.z != 0.f)
			fwd.normalize();

		if (right.z != 0.f)
			right.normalize();

		vec3_t wishvel{
			fwd.x * move.x + right.x * move.y,
			fwd.y * move.x + right.y * move.y,
			0.f
		};

		auto wishdir = wishvel;

		auto wishspeed = wishdir.normalize();
		if (wishspeed
			&& wishspeed > m_max_player_speed) {
			wishvel *= m_max_player_speed / wishspeed;

			wishspeed = m_max_player_speed;
		}

		velocity.z = 0.f;
		accelerate(user_cmd, wishdir, wishspeed, velocity, g_context->cvars().m_sv_accelerate->get_float());
		velocity.z = 0.f;

		const auto speed_sqr = velocity.length_sqr();
		if (speed_sqr > (m_max_player_speed * m_max_player_speed))
			velocity *= m_max_player_speed / std::sqrt(speed_sqr);

		if (velocity.length() < 1.f)
			velocity = {};
	}

	void c_movement::full_walk_move(
		const valve::user_cmd_t& user_cmd, vec3_t& move,
		vec3_t& fwd, vec3_t& right, vec3_t& velocity
	) const {
		if (valve::g_local_player->ground_entity() != valve::e_ent_handle::invalid) {
			velocity.z = 0.f;

			const auto speed = velocity.length();
			if (speed >= 0.1f) {
				const auto friction = g_context->cvars().m_sv_friction->get_float() * valve::g_local_player->surface_friction();
				const auto sv_stopspeed = g_context->cvars().m_sv_friction->get_float();
				const auto control = speed < sv_stopspeed ? sv_stopspeed : speed;

				const auto new_speed = std::max(0.f, speed - ((control * friction) * valve::g_global_vars->m_interval_per_tick));
				if (speed != new_speed)
					velocity *= new_speed / speed;
			}

			walk_move(user_cmd, move, fwd, right, velocity);

			velocity.z = 0.f;
		}

		const auto sv_maxvelocity = g_context->cvars().m_sv_maxvelocity->get_float();
		for (std::size_t i{}; i < 3u; ++i) {
			auto& element = velocity[i];

			if (element > sv_maxvelocity)
				element = sv_maxvelocity;
			else if (element < -sv_maxvelocity)
				element = -sv_maxvelocity;
		}
	}

	void c_movement::modify_move(valve::user_cmd_t& user_cmd, vec3_t& velocity) const {
		vec3_t fwd{}, right{};

		math::angle_vectors(user_cmd.m_view_angles, &fwd, &right);

		const auto speed_sqr = user_cmd.m_move.length_sqr();
		if (speed_sqr > (m_max_player_speed * m_max_player_speed))
			user_cmd.m_move *= m_max_player_speed / std::sqrt(speed_sqr);

		full_walk_move(user_cmd, user_cmd.m_move, fwd, right, velocity);
	}

	void c_movement::predict_move(const valve::user_cmd_t& user_cmd, vec3_t& velocity) const {
		vec3_t fwd{}, right{};

		math::angle_vectors(user_cmd.m_view_angles, &fwd, &right);

		auto move = user_cmd.m_move;

		const auto speed_sqr = user_cmd.m_move.length_sqr();
		if (speed_sqr > (m_max_player_speed * m_max_player_speed))
			move *= m_max_player_speed / std::sqrt(speed_sqr);

		full_walk_move(user_cmd, move, fwd, right, velocity);
	}

	void c_movement::auto_strafe(valve::user_cmd_t& user_cmd) {
		if (!sdk::g_config_system->air_strafe
			|| valve::g_local_player->flags() & valve::e_ent_flags::on_ground)
			return;
				
		auto wish_angles = user_cmd.m_view_angles;

		if ((user_cmd.m_move.x != 0.f || user_cmd.m_move.y != 0.f))
			wish_angles.y = std::remainder(
				wish_angles.y
				+ std::remainder(
					math::to_deg(
						std::atan2(user_cmd.m_move.x, user_cmd.m_move.y)
					) - 90.f, 360.f
				), 360.f
			);

		user_cmd.m_move.x = user_cmd.m_move.y = 0.f;
				
		const bool holding_w = user_cmd.m_buttons & valve::e_buttons::in_forward;
		const bool holding_a = user_cmd.m_buttons & valve::e_buttons::in_move_left;
		const bool holding_s = user_cmd.m_buttons & valve::e_buttons::in_back;
		const bool holding_d = user_cmd.m_buttons & valve::e_buttons::in_move_right;
		const auto& velocity = valve::g_local_player->velocity();
		const auto speed_2d = velocity.length_2d();

		if (!(holding_w || holding_a || holding_s || holding_d)
			&& speed_2d < 30.f)
			return;

		const auto ideal_strafe = std::min(90.f, math::to_deg(std::asin(15.f / speed_2d)));

		const auto mult = m_strafe_switch ? 1.f : -1.f;

		m_strafe_switch = !m_strafe_switch;

		auto delta = std::remainder(wish_angles.y - m_prev_view_yaw, 360.f);
		if (delta)
			user_cmd.m_move.y = delta < 0.f ? 450.f : -450.f;

		delta = std::abs(delta);

		if (delta >= 30.f
			|| ideal_strafe >= delta) {
			const auto vel_angle = math::to_deg(std::atan2(velocity.y, velocity.x));
			const auto vel_delta = std::remainder(wish_angles.y - vel_angle, 360.f);

			if (speed_2d <= 15.f
				|| ideal_strafe >= vel_delta) {
				if (speed_2d <= 15.f
					|| vel_delta >= -ideal_strafe) {
					user_cmd.m_move.y = 450.f * mult;
					wish_angles.y += ideal_strafe * mult;
				}
				else {
					user_cmd.m_move.y = 450.f;
					wish_angles.y = vel_angle - ideal_strafe;
				}
			}
			else {
				user_cmd.m_move.y = -450.f;
				wish_angles.y = vel_angle + ideal_strafe;
			}

			rotate(
				user_cmd, wish_angles,
				valve::g_local_player->flags(),
				valve::g_local_player->move_type()
			);
		}
	}

	bool c_movement::stop(valve::user_cmd_t& user_cmd) {
		if (!m_stop_type)
			return false;

		const auto stop_type = m_stop_type;

		m_stop_type = 0;

		if (user_cmd.m_buttons & valve::e_buttons::in_jump
			|| !(valve::g_local_player->flags() & valve::e_ent_flags::on_ground))
			return false;

		if (g_context->cvars().m_weapon_accuracy_nospread->get_bool())
			return false;

		const auto weapon = g_context->weapon();
		if (!weapon)
			return false;

		const auto wpn_data = weapon->wpn_data();
		if (!wpn_data)
			return false;

		float target_speed{};
		if (stop_type == 2)
			target_speed = weapon->max_speed() / 3.f;

		auto velocity = valve::g_local_player->velocity();

		const auto speed_2d = velocity.length_2d();

		if (stop_type == 1) {
			if (speed_2d < 1.f) {
				user_cmd.m_move.x = user_cmd.m_move.y = 0.f;

				return true;
			}
		}
		else if (target_speed >= speed_2d) {
			m_max_player_speed = m_max_weapon_speed = target_speed;

			modify_move(user_cmd, velocity);

			return true;
		}

		user_cmd.m_buttons &= ~valve::e_buttons::in_speed;

		const auto max_weapon_speed = weapon->max_speed();

		auto finalwishspeed = std::min(max_weapon_speed, 260.f);

		const auto ducking =
			user_cmd.m_buttons & valve::e_buttons::in_duck
			|| valve::g_local_player->ducking()
			|| valve::g_local_player->flags() & valve::e_ent_flags::ducking;

		bool slow_down_to_fast_nigga{};

		if (weapon
			&& g_context->cvars().m_sv_accelerate_use_weapon_speed->get_bool()) {
			const auto item_index = static_cast<std::uint16_t>(weapon->item_index());
			if (weapon->zoom_lvl() > 0
				&& (item_index == 11 || item_index == 38 || item_index == 9 || item_index == 8 || item_index == 39 || item_index == 40))
				slow_down_to_fast_nigga = (max_weapon_speed * 0.52f) < 110.f;

			if (!ducking
				|| slow_down_to_fast_nigga)
				finalwishspeed *= std::min(1.f, max_weapon_speed / 260.f);
		}

		if (ducking
			&& !slow_down_to_fast_nigga)
			finalwishspeed *= 0.34f;

		finalwishspeed =
			((valve::g_global_vars->m_interval_per_tick * g_context->cvars().m_sv_accelerate->get_float()) * finalwishspeed)
			* valve::g_local_player->surface_friction();

		if (target_speed <= (speed_2d - finalwishspeed)) {
			if (finalwishspeed <= 0.f) {
				m_max_player_speed = m_max_weapon_speed = target_speed;

				modify_move(user_cmd, velocity);

				return true;
			}

			goto change_move;
		}

		finalwishspeed = speed_2d - target_speed;

		if (finalwishspeed < 0.f) {
			m_max_player_speed = m_max_weapon_speed = target_speed;

			modify_move(user_cmd, velocity);

			return true;
		}
	change_move:
		vec3_t dir{};
		math::vector_angles(velocity *= -1.f, dir);

		dir.y = user_cmd.m_view_angles.y - dir.y;

		math::angle_vectors(dir, &dir);

		user_cmd.m_move.x = dir.x * finalwishspeed;
		user_cmd.m_move.y = dir.y * finalwishspeed;

		return true;
	}

	bool c_movement::force_lby_update(valve::user_cmd_t& user_cmd) {
		if (!sdk::g_config_system->enabled2
			|| !sdk::g_config_system->body_yaw
			|| user_cmd.m_buttons & valve::e_buttons::in_jump
			|| !(valve::g_local_player->flags() & valve::e_ent_flags::on_ground))
			return false;

		m_max_weapon_speed = 260.f;
		if (const auto weapon = g_context->weapon())
			m_max_weapon_speed = weapon->max_speed();

		m_max_player_speed = valve::g_local_player->max_speed();

		if (valve::g_local_player->velocity().length_2d_sqr() > 2.f
			|| !valve::g_local_player->anim_state())
			return false;

		float duck_amount{};
		if (user_cmd.m_buttons & valve::e_buttons::in_duck)
			duck_amount = std::min(
				1.f,
				valve::g_local_player->duck_amount()
				+ (valve::g_global_vars->m_interval_per_tick * 0.8f) * valve::g_local_player->duck_speed()
			);
		else
			duck_amount =
			valve::g_local_player->duck_amount()
			- std::max(1.5f, valve::g_local_player->duck_speed()) * valve::g_global_vars->m_interval_per_tick;

		float move{};
		if (user_cmd.m_buttons & valve::e_buttons::in_duck
			|| valve::g_local_player->ducking()
			|| valve::g_local_player->flags() & valve::e_ent_flags::anim_ducking)
			move = 1.1f / (((duck_amount * 0.34f) + 1.f) - duck_amount);
		else
			move = 1.1f;

		if (std::abs(user_cmd.m_move.x) > move
			|| std::abs(user_cmd.m_move.y) > move)
			return false;

		if (!(user_cmd.m_number & 1))
			move *= -1.f;

		user_cmd.m_move.x = move;

		return true;
	}

	void c_movement::normalize(valve::user_cmd_t& user_cmd) const {
		user_cmd.m_view_angles.x = std::remainder(user_cmd.m_view_angles.x, 360.f);
		user_cmd.m_view_angles.y = std::remainder(user_cmd.m_view_angles.y, 360.f);
		user_cmd.m_view_angles.z = std::remainder(user_cmd.m_view_angles.z, 360.f);

		user_cmd.m_view_angles.x = std::clamp(user_cmd.m_view_angles.x, -89.f, 89.f);
		user_cmd.m_view_angles.y = std::clamp(user_cmd.m_view_angles.y, -180.f, 180.f);
		user_cmd.m_view_angles.z = std::clamp(user_cmd.m_view_angles.z, -90.f, 90.f);

		user_cmd.m_move.x = std::clamp(user_cmd.m_move.x, -450.f, 450.f);
		user_cmd.m_move.y = std::clamp(user_cmd.m_move.y, -450.f, 450.f);
		user_cmd.m_move.z = std::clamp(user_cmd.m_move.z, -320.f, 320.f);

		if (valve::g_local_player->move_type() != valve::e_move_type::walk)
			return;

		user_cmd.m_buttons &= ~(
			valve::e_buttons::in_forward
			| valve::e_buttons::in_back
			| valve::e_buttons::in_move_right
			| valve::e_buttons::in_move_left
			);

		if (user_cmd.m_move.x != 0.f)
			user_cmd.m_buttons |=
			(sdk::g_config_system->slide_walk ? user_cmd.m_move.x < 0.f : user_cmd.m_move.x > 0.f)
			? valve::e_buttons::in_forward : valve::e_buttons::in_back;

		if (user_cmd.m_move.y == 0.f)
			return;

		user_cmd.m_buttons |=
			(sdk::g_config_system->slide_walk ? user_cmd.m_move.y < 0.f : user_cmd.m_move.y > 0.f)
			? valve::e_buttons::in_move_right : valve::e_buttons::in_move_left;
	}

	void c_movement::rotate(
		valve::user_cmd_t& user_cmd, const qangle_t& wish_angles,
		const valve::e_ent_flags flags, const valve::e_move_type move_type
	) const {
		if (user_cmd.m_view_angles.z != 0.f
			&& !(flags & valve::e_ent_flags::on_ground))
			user_cmd.m_move.y = 0.f;

		auto move_2d = vec2_t(user_cmd.m_move.x, user_cmd.m_move.y);

		if (const auto speed_2d = move_2d.length()) {
			const auto delta = user_cmd.m_view_angles.y - wish_angles.y;

			vec2_t v1;

			math::sin_cos(
				math::to_rad(
					std::remainder(
						math::to_deg(
							std::atan2(move_2d.y / speed_2d, move_2d.x / speed_2d)
						) + delta, 360.f
					)
				), v1.x, v1.y
			);

			const auto cos_x = std::cos(
				math::to_rad(
					std::remainder(
						math::to_deg(std::atan2(0.f, speed_2d)), 360.f
					)
				)
			);

			move_2d.x = cos_x * v1.y * speed_2d;
			move_2d.y = cos_x * v1.x * speed_2d;

			if (move_type == valve::e_move_type::ladder) {
				if (wish_angles.x < 45.f
					&& std::abs(delta) <= 65.f
					&& user_cmd.m_view_angles.x >= 45.f) {
					move_2d.x *= -1.f;
				}
			}
			else if (std::abs(user_cmd.m_view_angles.x) > 90.f)
				move_2d.x *= -1.f;
		}

		user_cmd.m_move.x = move_2d.x;
		user_cmd.m_move.y = move_2d.y;

		user_cmd.m_buttons &= ~(
			valve::e_buttons::in_forward
			| valve::e_buttons::in_back
			| valve::e_buttons::in_move_right
			| valve::e_buttons::in_move_left
			);

		if (move_type == valve::e_move_type::ladder) {
			if (std::abs(user_cmd.m_move.x) > 200.f)
				user_cmd.m_buttons |=
				user_cmd.m_move.x > 0.f
				? valve::e_buttons::in_forward : valve::e_buttons::in_back;

			if (std::abs(user_cmd.m_move.y) <= 200.f)
				return;

			user_cmd.m_buttons |=
				user_cmd.m_move.y > 0.f
				? valve::e_buttons::in_move_right : valve::e_buttons::in_move_left;

			return;
		}

		if (sdk::g_config_system->slide_walk
			&& move_type == valve::e_move_type::walk) {
			if (user_cmd.m_move.x != 0.f)
				user_cmd.m_buttons |=
				user_cmd.m_move.x < 0.f
				? valve::e_buttons::in_forward : valve::e_buttons::in_back;

			if (user_cmd.m_move.y == 0.f)
				return;

			user_cmd.m_buttons |=
				user_cmd.m_move.y < 0.f
				? valve::e_buttons::in_move_right : valve::e_buttons::in_move_left;

			return;
		}

		if (user_cmd.m_move.x != 0.f)
			user_cmd.m_buttons |=
			user_cmd.m_move.x > 0.f
			? valve::e_buttons::in_forward : valve::e_buttons::in_back;

		if (user_cmd.m_move.y == 0.f)
			return;

		user_cmd.m_buttons |=
			user_cmd.m_move.y > 0.f
			? valve::e_buttons::in_move_right : valve::e_buttons::in_move_left;
	}

	bool c_movement::peek_assistence(valve::user_cmd_t& user_cmd) {
		if (!g_context->should_return())
			return false;

		const auto current_position = valve::g_local_player->abs_origin();
		const auto difference = current_position - g_context->start_position();

		if (difference.length_2d() > 5.f) {
			const auto angle = math::calculate_angle(valve::g_local_player->abs_origin(), g_context->start_position());
			auto wish_angles = user_cmd.m_view_angles;
			wish_angles = angle;
			user_cmd.m_move.x = 450.f;
			user_cmd.m_move.y = 0.f;
			rotate(
				user_cmd, wish_angles,
				valve::g_local_player->flags(),
				valve::g_local_player->move_type()
			);
			return true;			
		}
		else
			g_context->should_return() = false;

		return false;
	}

	void c_movement::on_create_move(valve::user_cmd_t& user_cmd) {
		correct_mouse(user_cmd);

		if (valve::g_local_player->move_type() != valve::e_move_type::walk)
			return;

		m_max_player_speed = valve::g_local_player->max_speed();
		m_max_weapon_speed = g_context->weapon() ? g_context->weapon()->max_speed() : 260.f;

		m_velocity = valve::g_local_player->velocity();

		predict_move(user_cmd, m_velocity);

		if (sdk::g_config_system->infinite_duck)
			user_cmd.m_buttons |= valve::e_buttons::in_bullrush;

		if (m_should_fake_duck) {
			if (g_exploits->type()) {
				if (valve::g_local_player->duck_amount() > 0.5f)
					user_cmd.m_buttons |= valve::e_buttons::in_duck;
				else
					user_cmd.m_buttons &= ~valve::e_buttons::in_duck;
			}
			else {
				user_cmd.m_buttons |= valve::e_buttons::in_bullrush;

				const auto pred_duck_amount = std::min(1.f, valve::g_local_player->duck_speed() * valve::to_time(7));
				if (valve::g_local_player->duck_amount() <= pred_duck_amount) {
					if (valve::g_client_state->m_choked_cmds < 7)
						user_cmd.m_buttons &= ~valve::e_buttons::in_duck;
					else
						user_cmd.m_buttons |= valve::e_buttons::in_duck;

					if (g_context->flags() & e_context_flags::can_choke)
						g_context->flags() |= e_context_flags::choke;
					else
						g_context->flags() &= ~e_context_flags::choke;
				}
				else {
					user_cmd.m_buttons &= ~valve::e_buttons::in_duck;

					g_context->flags() &= ~e_context_flags::choke;
				}
			}
		}

		const auto stopped = stop(user_cmd);
		const auto returned = peek_assistence(user_cmd);
		const auto forced_lby_update = force_lby_update(user_cmd);

		if (!stopped
			&& !returned) {
			if (key_handler::check_key(sdk::g_config_system->slow_motion_key, sdk::g_config_system->slow_motion_key_style)
				&& !(user_cmd.m_buttons & valve::e_buttons::in_jump)
				&& valve::g_local_player->flags() & valve::e_ent_flags::on_ground
				&& (user_cmd.m_move.x != 0.f || user_cmd.m_move.y != 0.f)) {

				if (!sdk::g_config_system->speed_limit)
					m_max_player_speed = m_max_weapon_speed;
				else if (sdk::g_config_system->speed_limit <= m_velocity.length())
					m_max_player_speed = m_max_weapon_speed = sdk::g_config_system->speed_limit;

				auto velocity = valve::g_local_player->velocity();

				modify_move(user_cmd, velocity);
			}

			if (!forced_lby_update) {
				if (sdk::g_config_system->bunny_hop) {
					static auto last_jumped = false;
					static auto should_fake = false;

					if (!last_jumped && should_fake) {
						should_fake = false;
						user_cmd.m_buttons |= valve::e_buttons::in_jump;
					}
					else if (user_cmd.m_buttons & valve::e_buttons::in_jump) {
						if (valve::g_local_player->flags() & valve::e_ent_flags::on_ground) {
							last_jumped = true;
							should_fake = true;
						}
						else {
							user_cmd.m_buttons &= ~valve::e_buttons::in_jump;
							last_jumped = false;
						}
					}
					else {
						last_jumped = false;
						should_fake = false;
					}
				}

				auto_strafe(user_cmd);
			}
		}

		user_cmd.m_buttons &= ~(
			valve::e_buttons::in_forward
			| valve::e_buttons::in_back
			| valve::e_buttons::in_move_right
			| valve::e_buttons::in_move_left
			);

		if (sdk::g_config_system->slide_walk
			&& valve::g_local_player->move_type() == valve::e_move_type::walk) {
			if (user_cmd.m_move.x != 0.f)
				user_cmd.m_buttons |=
				user_cmd.m_move.x < 0.f
				? valve::e_buttons::in_forward : valve::e_buttons::in_back;

			if (user_cmd.m_move.y == 0.f)
				return;

			user_cmd.m_buttons |=
				user_cmd.m_move.y < 0.f
				? valve::e_buttons::in_move_right : valve::e_buttons::in_move_left;

			return;
		}

		if (user_cmd.m_move.x != 0.f)
			user_cmd.m_buttons |=
			user_cmd.m_move.x > 0.f
			? valve::e_buttons::in_forward : valve::e_buttons::in_back;

		if (user_cmd.m_move.y == 0.f)
			return;

		user_cmd.m_buttons |=
			user_cmd.m_move.y > 0.f
			? valve::e_buttons::in_move_right : valve::e_buttons::in_move_left;
	}
}