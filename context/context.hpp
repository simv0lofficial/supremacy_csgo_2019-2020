#pragma once

namespace supremacy {
	struct code_section_t {
		__forceinline constexpr code_section_t( ) = default;

		__forceinline constexpr code_section_t(
			const std::size_t size, const std::uintptr_t base
		) : m_size{ size }, m_base{ base } {}

		__forceinline code_section_t( const HMODULE module ) {
			const auto dos_header = reinterpret_cast< PIMAGE_DOS_HEADER >( module );
			if ( dos_header->e_magic != IMAGE_DOS_SIGNATURE )
				return;

			const auto nt_headers = reinterpret_cast< PIMAGE_NT_HEADERS >(
				reinterpret_cast< std::uintptr_t >( module ) + dos_header->e_lfanew
			);
			if ( nt_headers->Signature != IMAGE_NT_SIGNATURE )
				return;

			m_size = nt_headers->OptionalHeader.SizeOfCode;
			m_base = reinterpret_cast< std::uintptr_t >( module ) + nt_headers->OptionalHeader.BaseOfCode;
		}

		std::size_t		m_size{};
		std::uintptr_t	m_base{};
	};

	std::uintptr_t find_byte_seq( const code_section_t& section, const std::string& seq );

	namespace valve {
		struct cvar_t;
		
		struct user_cmd_t;

		class c_player;

		class c_weapon;

		struct weapon_data_t;
	}

	enum e_context_flags : std::uint8_t {
		can_choke = 1u << 0u,
		choke = 1u << 1u,
		can_shoot = 1u << 2u,
		recharge = 1u << 3u,
		aim_fire = 1u << 4u,
		has_target = 1u << 5u
	};
	DEFINE_ENUM_FLAG_OPERATORS( e_context_flags )

	class c_context {
	private:
		struct addresses_t {
			using line_goes_thru_smoke_t = bool( __cdecl* )( vec3_t, vec3_t );

			using random_float_t = float( __cdecl* )( const float, const float );

			using random_int_t = int( __cdecl* )( const int, const int );

			using random_seed_t = void( __cdecl* )( const int );

			using angle_matrix_t = void( __fastcall* )( const qangle_t&, mat3x4_t& );

			using set_clan_tag_t = int(__fastcall*)(const char*, const char*);

			using change_callback_t = void(__fastcall*)(valve::cvar_t*, const char*, float);

			random_float_t		m_random_float{};
			random_int_t		m_random_int{};
			random_seed_t		m_random_seed{};

			std::uintptr_t		m_reset_anim_state{}, m_update_anim_state{}, m_weapon_prefix{},
								m_set_abs_angles{}, m_set_abs_origin{},
								m_lookup_seq_act{}, m_breakable{}, m_user_cmd_calc_checksum{},
								m_set_collision_bounds{}, m_invalidate_bone_cache{}, m_lookup_bone{}, m_lookup_sequence{},
								m_trace_filter_simple_vtable{}, m_trace_filter_skip_two_entities_vtable{},
								m_write_user_cmd{}, m_weapon_system{}, m_calc_shotgun_spread{},
								m_key_values_init{}, m_key_values_load_from_buffer{}, m_cam_sv_cheats_ret{},
								m_add_notify{}, m_ret_to_scope_arc{}, m_ret_to_scope_lens{},
								m_start_drawing{}, m_finish_drawing{}, m_add_clred_text{}, m_calc_text_size{},
								m_set_font_glyph_set{}, m_has_c4{}, m_ret_insert_into_tree{}, m_smoke_count{},
								m_disable_post_processing{}, m_crosshair_ret{}, m_ret_to_scope_clear{}, m_ret_to_scope_blurry{},
								m_ret_to_fire_bullet{}, m_ret_set_first_person_viewangles{}, m_allow_extrapolation{},
								m_item_system{}, m_hud{}, m_find_hud_element{}, m_clear_hud_wpn{},
								m_clear_hud_wpn_icon{}, m_construct_voice_data_message{}, m_compute_hitbox_surround_box{}, m_lock_studio_hdr{},
								m_setup_movement{}, m_get_sequence_linear_motion{}, m_invalidate_physics_recursive{}, m_thread_id_allocated{};

			int*				m_pred_seed{};
			valve::c_player*	m_pred_player{};

			angle_matrix_t		m_angle_matrix{};
			set_clan_tag_t		m_set_clan_tag{};
			
			line_goes_thru_smoke_t m_line_goes_thru_smoke{};
		}						m_addresses{};

		struct cvars_t {
			valve::cvar_t*	m_r_jiggle_bones{}, *m_cl_interp{},
							*m_cl_interp_ratio{}, *m_sv_client_min_interp_ratio{},
							*m_sv_client_max_interp_ratio{}, *m_cl_updaterate{},
							*m_sv_minupdaterate{}, *m_sv_maxupdaterate{},
							*m_ff_damage_reduction_bullets, *m_ff_damage_bullet_penetration{},
							*m_sv_autobunnyhopping{}, *m_sv_clockcorrection_msecs{},
							*m_sv_maxvelocity{}, *m_sv_friction{},
							*m_sv_accelerate{}, *m_sv_accelerate_use_weapon_speed{},
							*m_weapon_accuracy_nospread{}, *m_sv_maxunlag{},
							*m_sv_enablebunnyhopping{}, *m_sv_jump_impulse{}, *m_sv_gravity{},
							*m_weapon_recoil_scale{}, *m_weapon_accuracy_shotgun_spread_patterns{},
							*m_molotov_throw_detonate_time{}, *m_weapon_molotov_maxdetonateslope{},
							*m_cl_fullupdate{}, *m_mp_teammates_are_enemies{}, *m_cl_lagcompensation{},
							*m_name{}, *m_voice_loopback{}, *m_pitch{}, *m_yaw{}, *m_sensitivity{},
							*m_mp_damage_scale_t_head{}, *m_mp_damage_scale_t_body{}, *m_mp_damage_scale_ct_head{},
							*m_mp_damage_scale_ct_body{};
		}					m_cvars{};

		struct net_info_t {
			float	m_lerp{};

			struct {
				float m_in{}, m_out{};
			}		m_latency{};

			int m_server_tick{};

			bool m_on_local_server{};
		}						m_net_info{};

		vec3_t					m_shoot_pos{}, m_start_position{}, m_last_sent_origin{}, m_view_angles{};
		int						m_last_sent_cmd_number{}, m_last_cmd_number{};
		bool					m_should_auto_buy{}, m_freeze_time{}, m_allow_anim_update{},
								m_allow_setup_bones{}, m_force_bone_mask{},
			m_broke_lc{}, m_console_drawn{}, m_switch_lagcompensation_state{}, m_should_return{};

		std::vector< int >		m_sented_cmds{};

		e_context_flags			m_flags{};

		valve::c_weapon*		m_weapon{};
		valve::weapon_data_t*	m_wpn_data{};
	public:
		void init( );

		bool will_shoot( valve::c_weapon* const weapon, const valve::user_cmd_t& user_cmd ) const;

		bool can_shoot(
			valve::c_weapon* const weapon, const valve::user_cmd_t& user_cmd,
			bool r8_ret, const int shift_amount, const bool normal
		) const;

		void add_packet(const int cmd_number);

		__forceinline addresses_t& addresses( );

		__forceinline cvars_t& cvars( );

		__forceinline net_info_t& net_info( );

		__forceinline vec3_t& shoot_pos( );

		__forceinline vec3_t& start_position();

		__forceinline bool& console_drawn();

		__forceinline bool& switch_lagcompensation_state();

		__forceinline bool& should_return();

		__forceinline int& last_sent_cmd_number( );

		__forceinline int& last_cmd_number( );

		__forceinline bool& should_auto_buy();

		__forceinline bool& allow_anim_update( );

		__forceinline bool& allow_setup_bones( );

		__forceinline bool& force_bone_mask( );

		__forceinline std::vector< int >& sented_cmds( );

		__forceinline e_context_flags& flags( );

		__forceinline valve::c_weapon*& weapon( );

		__forceinline valve::weapon_data_t*& wpn_data( );

		__forceinline bool& freeze_time( );

		__forceinline bool& broke_lc( );

		__forceinline vec3_t& last_sent_origin( );

		__forceinline vec3_t& view_angles();

		HMODULE h_module;

		bool debug_build;
	};

	inline const auto g_context = std::make_unique< c_context >( );
}

#include "impl/context.inl"