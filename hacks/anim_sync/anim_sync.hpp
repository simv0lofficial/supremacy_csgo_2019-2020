#pragma once

namespace supremacy::hacks {
	class c_anim_sync {
	private:
		float compute_server_playback_rate( valve::c_player* player ) const;

		void update(
			const player_entry_t& entry, lag_record_t* const current,
			lag_record_t* const previous, lag_record_t* const penultimate,
			const int side
		) const;

		void find_server_foot_yaw(
			player_entry_t& entry, lag_record_t* const current,
			lag_record_t* const previous, lag_record_t* const penultimate
		) const;

		struct local_data_t {
			valve::bones_t		m_shoot_pos_bones{};

			bool				m_shot_valid_wpn{};
			int					m_shot_cmd_number{};
			float				m_last_shot_time{}, m_min_body_yaw{}, m_max_body_yaw{};

			struct {
				valve::bones_t	m_bones{};
			}					m_real{};

			struct {
				valve::bones_t			m_bones{};

				float					m_spawn_time{}, m_abs_yaw{};

				valve::anim_state_t		m_anim_state{};
				valve::anim_layers_t	m_anim_layers{};
				valve::pose_params_t	m_pose_params{};
			}					m_fake{};
		} m_local_data{};
	public:
		bool setup_bones(
			valve::c_player* const player,
			valve::bones_t& bones, const float time, const int flags
		) const;

		void on_net_update(
			player_entry_t& entry, lag_record_t* const current,
			lag_record_t* const previous, lag_record_t* const penultimate
		) const;

		void update_local(const qangle_t& view_angles, const bool no_view_model);

		void update_local_real(valve::user_cmd_t& user_cmd);

		void setup_local_bones();

		__forceinline local_data_t& local_data();
	};

	inline const auto g_anim_sync = std::make_unique< c_anim_sync >();
}

#include "impl/anim_sync.inl"