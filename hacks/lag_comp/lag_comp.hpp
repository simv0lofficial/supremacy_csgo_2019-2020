#pragma once

namespace supremacy::hacks {
	struct lag_backup_t {
		__forceinline constexpr lag_backup_t() = default;

		__forceinline lag_backup_t(valve::c_player* const player);

		__forceinline void restore(valve::c_player* const player) const;

		float			m_foot_yaw{};
		vec3_t			m_origin{}, m_abs_origin{},
			m_obb_min{}, m_obb_max{};

		valve::bones_t	m_bones{};
		int				m_bones_count{},
			m_readable_bones{}, m_writable_bones{};

		unsigned long	m_mdl_bone_counter{};
	};

	struct lag_record_t {
		struct anim_side_t {
			valve::bones_t			m_bones{};
			std::size_t				m_bones_count{};
			float					m_foot_yaw{}, m_prev_foot_yaw{},
				m_move_yaw_ideal{}, m_move_yaw_cur_to_ideal{},
				m_move_yaw{};

			valve::anim_layers_t	m_anim_layers{};
		};

		__forceinline constexpr lag_record_t() = default;

		__forceinline lag_record_t(
			valve::c_player* const player
		) : m_dormant{ player->dormant() },
			m_flags{ player->flags() },
			m_sim_time{ player->sim_time() },
			m_old_sim_time{ player->old_sim_time() },
			m_sim_ticks{ valve::to_ticks(player->sim_time() - player->old_sim_time()) },
			m_receive_tick{ valve::g_client_state->m_server_tick },
			m_lby{ player->lby() },
			m_duck_amount{ player->duck_amount() },
			m_third_person_recoil{ player->third_person_recoil() },
			m_weapon{ player->weapon() },
			m_last_shot_time{ m_weapon ? m_weapon->last_shot_time() : std::numeric_limits< float >::max() },
			m_eye_angles{ player->eye_angles() },
			m_abs_angles{ player->abs_angles() },
			m_strafing{ player->strafing() },
			m_walking{ player->walking() },
			m_velocity{ player->velocity() }, 
			m_origin{ player->origin() },
			m_abs_origin{ player->abs_origin() },
			m_obb_min{ player->obb_min() }, 
			m_obb_max{ player->obb_max() },
			m_anim_layers{ player->anim_layers() },
			m_pose_params{ player->pose_params() } {}
	
		__forceinline void restore(valve::c_player* const player, const int anim_index, const bool only_anim = false) const;

		__forceinline bool valid() const;

		bool							m_dormant{}, m_trying_to_resolve{}, m_broke_lc{},
			m_shot{}, m_throw{}, m_strafing{}, m_walking{}, m_sideways{}, m_forward{},
			m_accelerating{}, m_extrapolated{}, m_shifting{}, m_first_after_dormant{}, m_should_force_normal_sp{};

		valve::e_ent_flags				m_flags{};
		valve::c_weapon* m_weapon{};

		int								 m_sim_ticks{}, m_side{}, m_priority{}, m_type{},
			m_receive_tick{}, m_shot_tick{}, m_extrapolate_ticks{};
		float							m_sim_time{}, m_old_sim_time{}, m_lby{}, m_duck_amount{}, m_third_person_recoil{}, m_max_delta{},
			m_last_shot_time{};

		qangle_t						m_eye_angles{}, m_abs_angles{};
		vec3_t							m_velocity{}, m_origin{}, m_abs_origin{}, m_obb_min{}, m_obb_max{};

		valve::anim_layers_t			m_anim_layers{};
		valve::pose_params_t			m_pose_params{};

		std::array< anim_side_t, 7u >	m_anim_sides{};
	};

	struct player_entry_t {		
		__forceinline void reset();

		valve::c_player* m_player{};

		float											m_spawn_time{}, m_highest_simtime{}, m_receive_time{};
		vec3_t											m_render_origin{};

		valve::bones_t									m_bones{};

		bool											m_unk{}, m_left_dormancy{}, m_try_anim_resolver{ true }, m_try_lby_resolver{ true };
		int												m_misses{}, m_prev_side{}, m_trace_side{}, m_prev_type{};
		valve::anim_layers_t							m_anim_layers{};
		std::deque< std::shared_ptr< lag_record_t > >	m_lag_records{};
	};

	class c_lag_comp {
	private:
		std::array< player_entry_t, 64u > m_entries{};
	public:

		void on_post_data_update(valve::c_player* player);

		void on_net_update();

		__forceinline float calc_time_delta(const float sim_time) const;

		__forceinline player_entry_t& entry(const std::size_t i);
	};

	inline const auto g_lag_comp = std::make_unique< c_lag_comp >();
}

#include "impl/lag_comp.inl"