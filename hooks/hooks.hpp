#pragma once

namespace supremacy::hooks {
	LRESULT WINAPI wnd_proc( const HWND hwnd, const UINT msg, const WPARAM wparam, const LPARAM lparam );
	inline HWND h_window;
	inline WNDPROC old_wnd_proc;

	long D3DAPI d3d9_present(
		IDirect3DDevice9* const device, RECT* const src_rect,
		RECT* const dst_rect, const HWND dst_wnd_override, RGNDATA* const dirty_region
	);
	inline decltype( &d3d9_present ) orig_d3d9_present{};

	void __fastcall paint_traverse(const std::uintptr_t ecx, const std::uintptr_t edx, unsigned int panel, bool force_repaint, bool force);
	inline decltype(&paint_traverse) orig_paint_traverse{};

	void __fastcall paint( const std::uintptr_t ecx, const std::uintptr_t edx, const int mode );
	inline decltype( &paint ) orig_paint{};

	int __fastcall sv_cheats_get_int( valve::cvar_t* const ecx, const std::uintptr_t edx );
	inline decltype( &sv_cheats_get_int ) orig_sv_cheats_get_int{};

	void __fastcall draw_model(
		valve::studio_render_t* const ecx, const std::uintptr_t edx,
		const std::uintptr_t a0, const valve::draw_model_info_t& info,
		mat3x4_t* const bones, float* const a3, float* const a4, const vec3_t& origin, int flags
	);
	inline decltype( &draw_model ) orig_draw_model{};

	void __fastcall render_view(
		valve::view_render_t* const ecx, const std::uintptr_t edx,
		valve::view_setup_t* const setup, valve::view_setup_t* const hud_view_setup,
		int flags, int what_to_draw
	);
	inline decltype(&render_view) orig_render_view{};

	void __fastcall lock_cursor( const std::uintptr_t ecx, const std::uintptr_t edx );
	inline decltype( &lock_cursor ) orig_lock_cursor{};

	void __fastcall on_screen_size_changed(const std::uintptr_t ecx, const std::uintptr_t edx, int old_width, int old_height);
	inline decltype(&on_screen_size_changed) orig_on_screen_size_changed{};

	void __fastcall draw_set_clr(
		valve::c_surface* const ecx, const std::uintptr_t edx, const int r, const int g, const int b, const int a
	);
	inline decltype( &draw_set_clr ) orig_draw_set_clr{};

	void __stdcall chl_create_move_proxy( const int sequence_number, const float input_sample_frame_time, const bool active );
	void __stdcall chl_create_move( const int sequence_number, const float input_sample_frame_time, const bool active, bool& send_packet );

	using orig_chl_create_move_t = void( __thiscall* )( valve::c_client* const, const int, const float, const bool );
	inline orig_chl_create_move_t orig_chl_create_move{};

	void __stdcall frame_stage_notify( const valve::e_frame_stage stage );
	inline decltype( &frame_stage_notify ) orig_frame_stage_notify{};

	void __fastcall update_client_side_anim( valve::c_player* const ecx, const std::uintptr_t edx );
	inline decltype( &update_client_side_anim ) orig_update_client_side_anim{};

	bool __fastcall setup_bones(
		const std::uintptr_t ecx, const std::uintptr_t edx, mat3x4_t* const bones, int max_bones, int mask, float time
	);
	inline decltype( &setup_bones ) orig_setup_bones{};

	void __fastcall do_extra_bones_processing(
		valve::c_player* const ecx, const std::uintptr_t edx, int a0, int a1, int a2, int a3, int a4, int a5
	);
	inline decltype( &do_extra_bones_processing ) orig_do_extra_bones_processing{};

	void __fastcall accumulate_layers(
		valve::c_player* const ecx, const std::uintptr_t edx, int a0, int a1, float a2, int a3
	);
	inline decltype( &accumulate_layers ) orig_accumulate_layers{};

	void __fastcall standard_blending_rules(
		valve::c_player* const ecx, const std::uintptr_t edx, valve::model_data_t* const mdl_data, int a1, int a2, float a3, int mask
	);
	inline decltype( &standard_blending_rules ) orig_standard_blending_rules{};

	void __fastcall packet_start(
		const std::uintptr_t ecx, const std::uintptr_t edx, const int in_seq, const int out_acked
	);
	inline decltype( &packet_start ) orig_packet_start{};

	void __fastcall packet_end( const std::uintptr_t ecx, const std::uintptr_t edx );
	inline decltype( &packet_end ) orig_packet_end{};

	void __fastcall run_command(const std::uintptr_t ecx, const std::uintptr_t edx, valve::c_player* const player, valve::user_cmd_t* user_cmd, valve::c_move_helper* move_helper);
	inline decltype(&run_command) orig_run_command{};

	void __fastcall physics_simulate( valve::c_player* const ecx, const std::uintptr_t edx );
	inline decltype( &physics_simulate ) orig_physics_simulate{};

	void __fastcall on_latch_interpolated_vars(
		valve::c_player* const ecx, const std::uintptr_t edx, const int flags
	);
	inline decltype( &on_latch_interpolated_vars ) orig_on_latch_interpolated_vars{};

	bool __fastcall should_interpolate( valve::c_player* const ecx, const std::uintptr_t edx );
	inline decltype( &should_interpolate ) orig_should_interpolate{};

	bool __fastcall write_user_cmd_delta_to_buffer(
		const std::uintptr_t ecx, const std::uintptr_t edx,
		const int slot, valve::bf_write_t* const buffer, int from, int to, const bool is_new_cmd
	);
	inline decltype( &write_user_cmd_delta_to_buffer ) orig_write_user_cmd_delta_to_buffer{};

	void __cdecl velocity_modifier(valve::recv_proxy_data_t* const data, valve::c_entity* const entity, void* const out);
	inline decltype(&velocity_modifier) orig_velocity_modifier{};

	void __fastcall modify_eye_pos( valve::anim_state_t* const ecx, const std::uintptr_t edx, vec3_t& eye_pos );
	inline decltype( &modify_eye_pos ) orig_modify_eye_pos{};

	bool __fastcall process_temp_entities( const std::uintptr_t ecx, const std::uintptr_t edx, const std::uintptr_t msg );
	inline decltype( &process_temp_entities ) orig_process_temp_entities{};

	void __fastcall level_shutdown( const std::uintptr_t ecx, const std::uintptr_t edx );
	inline decltype( &level_shutdown ) orig_level_shutdown{};

	bool __cdecl glow_effect_spectator(
		valve::c_player* player, valve::c_player* local,
		int& style, vec3_t& clr, float& alpha_from, float& alpha_to,
		float& time_from, float& time_to, bool& animate
	);
	inline decltype( &glow_effect_spectator ) orig_glow_effect_spectator{};

	void __fastcall get_clr_modulation(
		valve::material_t* const ecx, const std::uintptr_t edx,
		float* const r, float* const g, float* const b
	);
	inline decltype( &get_clr_modulation ) orig_get_clr_modulation{};

	float __fastcall get_alpha_modulation(
		valve::material_t* const ecx, const std::uintptr_t edx
	);
	inline decltype(&get_alpha_modulation) orig_get_alpha_modulation{};

	void __fastcall draw_models(
		void* ecx, void* edx,
		int a2, int a3, int a4, int a5, int a6, char a7
	);
	inline decltype(&draw_models) orig_draw_models{};

	bool __stdcall is_using_static_prop_debug_modes( );
	inline decltype( &is_using_static_prop_debug_modes ) orig_is_using_static_prop_debug_modes{};

	int __fastcall list_leaves_in_box(
		const std::uintptr_t ecx, const std::uintptr_t edx,
		const vec3_t& mins, const vec3_t& maxs, const uint16_t* const list, const int max
	);
	inline decltype( &list_leaves_in_box ) orig_list_leaves_in_box{};

	bool __fastcall override_cfg(
		valve::c_mat_system* const ecx, const std::uintptr_t edx,
		valve::mat_sys_cfg_t* const cfg, const bool update
	);
	inline decltype( &override_cfg ) orig_override_cfg{};

	const char* __fastcall get_foreign_fallback_font_name(const std::uintptr_t ecx, const std::uintptr_t edx);
	inline decltype(&get_foreign_fallback_font_name) orig_get_foreign_fallback_font_name{};

	bool __fastcall fog_enabled( const std::uintptr_t ecx, const std::uintptr_t edx );
	inline decltype( &fog_enabled ) orig_fog_enabled{};

	void __fastcall override_view(
		const std::uintptr_t ecx, const std::uintptr_t edx, valve::view_setup_t* const setup
	);
	inline decltype( &override_view ) orig_override_view{};

	void __fastcall process_movement(std::uintptr_t ecx, std::uintptr_t edx, valve::c_player* player, valve::move_data_t* move_data);
	inline decltype(&process_movement) orig_process_movement{};

	int __fastcall do_post_screen_space_effects(
		const std::uintptr_t ecx, const std::uintptr_t edx, valve::view_setup_t* const setup
	);
	inline decltype( &do_post_screen_space_effects ) orig_do_post_screen_space_effects{};
	
	int __fastcall weapon_debug_spread_show_get_int( valve::cvar_t* const ecx, const std::uintptr_t edx );
	inline decltype( &weapon_debug_spread_show_get_int ) orig_weapon_debug_spread_show_get_int{};

	void __fastcall calc_view(
		valve::c_player* const ecx, const std::uintptr_t edx,
		vec3_t& origin, qangle_t& angles, float& z_near, float& z_far, float& fov
	);
	inline decltype( &calc_view ) orig_calc_view{};

	qangle_t* __fastcall get_eye_angles( valve::c_player* const ecx, const std::uintptr_t edx );
	inline decltype( &get_eye_angles ) orig_get_eye_angles{};

	float __fastcall aspect_ratio(const std::uintptr_t ecx, const std::uintptr_t edx, int width, int height);
	inline decltype(&aspect_ratio) orig_aspect_ratio{};

	bool __fastcall svc_msg_voice_data(std::uintptr_t ecx, std::uintptr_t edx, valve::c_svc_msg_voice_data* msg);
	inline decltype(&svc_msg_voice_data) orig_svc_msg_voice_data;

	struct event_listener_t : public valve::base_event_listener_t {
		void fire_game_event( valve::game_event_t* const event ) override;
	} inline g_event_listener{};
}