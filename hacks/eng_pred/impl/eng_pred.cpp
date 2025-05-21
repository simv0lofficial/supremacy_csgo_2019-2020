#include "../../../supremacy.hpp"

namespace supremacy::hacks {
	void c_eng_pred::prepare() {
		if (m_last_frame_stage == valve::e_frame_stage::net_update_end) {
			valve::g_prediction->update(
				valve::g_client_state->m_delta_tick,
				valve::g_client_state->m_delta_tick > 0,
				valve::g_client_state->m_last_cmd_ack,
				valve::g_client_state->m_last_cmd_out + valve::g_client_state->m_choked_cmds
			);

			if (const auto weapon = valve::g_local_player->weapon())
				if (weapon->item_index() == valve::e_item_index::revolver
					&& weapon->postpone_fire_ready_time() == std::numeric_limits< float >::max())
					weapon->postpone_fire_ready_time() = m_postpone_fire_ready_time;
		}

		m_backup.m_cur_time = valve::g_global_vars->m_cur_time;
		m_backup.m_frame_time = valve::g_global_vars->m_frame_time;

		valve::g_global_vars->m_cur_time = valve::to_time(valve::g_local_player->tick_base());
		valve::g_global_vars->m_frame_time = valve::g_global_vars->m_interval_per_tick;
	}

	void c_eng_pred::predict(valve::user_cmd_t* const user_cmd) {
		m_net_vars.at((user_cmd->m_number - 1) % 150).restore(user_cmd->m_number - 1);

		m_local_data.at(user_cmd->m_number % 150).init(*user_cmd);

		g_context->addresses().m_pred_player = valve::g_local_player;
		*g_context->addresses().m_pred_seed = user_cmd->m_random_seed;

		valve::g_local_player->set_local_view_angles(user_cmd->m_view_angles);

		valve::g_local_player->cur_user_cmd() = user_cmd;
		valve::g_local_player->last_user_cmd() = *user_cmd;

		const auto backup_in_prediction = valve::g_prediction->m_in_prediction;
		const auto backup_first_time_predicted = valve::g_prediction->m_first_time_predicted;

		valve::g_prediction->m_in_prediction = true;
		valve::g_prediction->m_first_time_predicted = false;

		valve::g_move_helper->set_host(valve::g_local_player);

		valve::g_movement->start_track_prediction_errors(valve::g_local_player);

		const auto backup_velocity_modifier = valve::g_local_player->velocity_modifier();

		valve::g_prediction->setup_move(valve::g_local_player, user_cmd, valve::g_move_helper, &m_move_data);

		valve::g_movement->process_movement(valve::g_local_player, &m_move_data);

		valve::g_prediction->finish_move(valve::g_local_player, user_cmd, &m_move_data);

		valve::g_movement->finish_track_prediction_errors(valve::g_local_player);

		valve::g_move_helper->set_host(nullptr);

		update_accuracy();

		m_local_data.at(user_cmd->m_number % 150).save(user_cmd->m_number);

		valve::g_local_player->velocity_modifier() = backup_velocity_modifier;

		valve::g_prediction->m_in_prediction = backup_in_prediction;
		valve::g_prediction->m_first_time_predicted = backup_first_time_predicted;

		update_shoot_pos();
	}

	void c_eng_pred::restore() {
		g_context->addresses().m_pred_player = nullptr;
		*g_context->addresses().m_pred_seed = -1;

		valve::g_global_vars->m_cur_time = m_backup.m_cur_time;
		valve::g_global_vars->m_frame_time = m_backup.m_frame_time;
	}

	void c_eng_pred::update_accuracy() {
		if (const auto weapon = valve::g_local_player->weapon()) {
			weapon->update_inaccuracy();

			m_inaccuracy = weapon->inaccuracy();
			m_spread = weapon->spread();

			const auto item_index = weapon->item_index();
			const auto v11 =
				item_index == valve::e_item_index::awp || item_index == valve::e_item_index::g3sg1
				|| item_index == valve::e_item_index::scar20 || item_index == valve::e_item_index::ssg08;
			const auto wpn_data = weapon->wpn_data();

			if (valve::g_local_player->flags() & valve::e_ent_flags::ducking)
				m_min_inaccuracy = v11 ? wpn_data->m_inaccuracy_crouch_alt : wpn_data->m_inaccuracy_crouch;
			else
				m_min_inaccuracy = v11 ? wpn_data->m_inaccuracy_stand_alt : wpn_data->m_inaccuracy_stand;
		}
		else
			m_inaccuracy = m_spread = 0.f;
	}

	void c_eng_pred::update_shoot_pos() const {
		const auto anim_state = valve::g_local_player->anim_state();
		if (!anim_state)
			return;

		valve::g_local_player->set_abs_angles({ 0.f, anim_state->m_foot_yaw, 0.f });

		auto& bones = g_anim_sync->local_data().m_shoot_pos_bones;
		g_anim_sync->setup_bones(valve::g_local_player, bones, valve::g_global_vars->m_cur_time, 15);

		auto eye_pos = valve::g_local_player->origin() + valve::g_local_player->view_offset();

		if (anim_state->m_player
			&& (anim_state->m_landing || anim_state->m_duck_amount || anim_state->m_player->ground_entity() == valve::e_ent_handle::invalid)) {
			const auto bone_index = valve::g_local_player->lookup_bone("head_0");
			if (bone_index != -1) {
				vec3_t head_pos{
					bones[bone_index][0u][3u],
					bones[bone_index][1u][3u],
					bones[bone_index][2u][3u] + 1.7f
				};

				if (head_pos.z < eye_pos.z) {
					constexpr auto firstperson_to_thirdperson_vertical_tolerance_min = 4.f;
					constexpr auto firstperson_to_thirdperson_vertical_tolerance_max = 10.f;

					auto lerp = math::simple_spline_remap_val_clamped(abs(eye_pos.z - head_pos.z),
						firstperson_to_thirdperson_vertical_tolerance_min,
						firstperson_to_thirdperson_vertical_tolerance_max,
						0.f, 1.f);

					eye_pos.z = math::lerp(lerp, eye_pos.z, head_pos.z);
				}
			}
		}

		g_context->shoot_pos() = eye_pos;
	}
}