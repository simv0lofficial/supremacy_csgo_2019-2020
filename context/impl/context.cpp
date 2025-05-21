#include "../../supremacy.hpp"
#include "../../dependencies/imgui/settings.hpp"
#include "../../dependencies/threading/new_threads.h"

uintptr_t __stdcall init_main(const HMODULE h_module) {
	supremacy::g_context->init();
	return 0;
}

uintptr_t __stdcall init_radio(const HMODULE h_module) {
	BASS::bass_lib_handle = BASS::bass_lib.LoadFromMemory(bass_dll_image, sizeof(bass_dll_image));

	if (BASS_INIT_ONCE())
		BASS::bass_init = true;

	static auto bass_needs_reinit = false;
	static auto current_channel = 0;
	static const char* channels[] = {
		"http://stream-158.zeno.fm/71ntub27u18uv?zs=R4yvq6kaRPyekzJdUwP1VA",
		"http://chanson.hostingradio.ru:8041/chanson-uncensored128.mp3",
		"http://podrubasik.ru/radio/8000/radio.mp3",
		"http://mp3.stream.tb-group.fm/clt.mp3",
		"http://mp3.stream.tb-group.fm/ht.mp3",
		"http://69.195.153.34/cvgm192",
		"http://lofi.stream.laut.fm/lofi",
		"http://eurobeat.stream.laut.fm/eurobeat",
		"http://icecast.vgtrk.cdnvideo.ru/vestifm_mp3_128kbps"
	};

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		const auto desired_channel = sdk::g_config_system->channel;

		if (BASS::bass_init
			&& key_handler::check_key(sdk::g_config_system->radio_key, sdk::g_config_system->radio_key_style)) {
			if (current_channel != desired_channel
				|| bass_needs_reinit) {
				bass_needs_reinit = false;
				BASS_Start();
				BASS_OPEN_STREAM(channels[desired_channel]);
				current_channel = desired_channel;
			}

			BASS_SET_VOLUME(BASS::stream_handle, sdk::g_config_system->volume / 100.f);
			BASS_PLAY_STREAM();
		}
		else if (BASS::bass_init) {
			bass_needs_reinit = true;
			BASS_StreamFree(BASS::stream_handle);
		}
	}

	BASS::bass_init = false;
	BASS_Stop();
	BASS_StreamFree(BASS::stream_handle);
	BASS_Free();
	return 0;
}

struct DllArguments {
	HMODULE hModule;
	LPVOID lpReserved;
};

BOOL APIENTRY DllMain(HMODULE h_module, uintptr_t  dw_reason_for_call, LPVOID lp_reserved) {
	switch (dw_reason_for_call) {
	case DLL_PROCESS_ATTACH:
	{
		DisableThreadLibraryCalls(h_module);

		auto current_process = GetCurrentProcess();
		auto priority_class = GetPriorityClass(current_process);

		if (priority_class != HIGH_PRIORITY_CLASS
			&& priority_class != REALTIME_PRIORITY_CLASS)
			SetPriorityClass(current_process, HIGH_PRIORITY_CLASS);

		supremacy::g_context->h_module = h_module;
		CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(init_main), h_module, NULL, NULL);
		CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(init_radio), h_module, NULL, NULL);

		return true;
	}
	default:
		return true;
	}
}

namespace supremacy {
	std::uintptr_t find_byte_seq(const code_section_t& section, const std::string& seq) {
		using seq_byte_t = std::pair< std::uint8_t, bool >;

		std::string str{};
		std::vector< seq_byte_t > byte_seq{};

		std::stringstream stream{ seq };
		while (stream >> str) {
			if (str[0u] == '?') {
				byte_seq.emplace_back(0u, true);

				continue;
			}

			if (!std::isxdigit(str[0u])
				|| !std::isxdigit(str[1u]))
				continue;

			byte_seq.emplace_back(
				static_cast<std::uint8_t>(
					std::strtoul(str.data(), nullptr, 16)
					), false
			);
		}

		const auto end = reinterpret_cast<std::uint8_t*>(section.m_base + section.m_size);

		const auto ret = std::search(
			reinterpret_cast<std::uint8_t*>(section.m_base), end, byte_seq.begin(), byte_seq.end(),
			[](const std::uint8_t byte, const seq_byte_t& seq_byte) {
				return std::get< bool >(seq_byte) || byte == std::get< std::uint8_t >(seq_byte);
			}
		);

		return reinterpret_cast<std::uintptr_t>(ret);
	}

	bool is_debug_build() {
		return (strstr(GetCommandLine(), xorstr_("-supremacy_debug")));
	}

	void c_context::init() {
		if (is_debug_build())
			debug_build = true;

		while (!GetModuleHandle(xorstr_("serverbrowser.dll")))
			std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });

		if (MH_Initialize() != MH_OK)
			return;

		const auto d3d9_device = **reinterpret_cast<IDirect3DDevice9***>(
			find_byte_seq(GetModuleHandle(xorstr_("shaderapidx9.dll")), xorstr_("A1 ? ? ? ? 50 8B 08 FF 51 0C")) + 1u
			);

		thread_pool = new c_thread_pool;
		const auto client_module = (TWENTYTWENTY ? GetModuleHandle(xorstr_("client.dll")) : GetModuleHandle(xorstr_("client_panorama.dll")));
		const auto engine_module = GetModuleHandle(xorstr_("engine.dll"));
		const auto tier0_module = GetModuleHandle(xorstr_("tier0.dll"));
		const auto materialsystem_module = GetModuleHandle(xorstr_("materialsystem.dll"));
		const auto vguimatsurface_module = GetModuleHandle(xorstr_("vguimatsurface.dll"));
		const auto localize_module = GetModuleHandle(xorstr_("localize.dll"));
		const auto datacache_module = GetModuleHandle(xorstr_("datacache.dll"));
		const auto vgui2_module = GetModuleHandle(xorstr_("vgui2.dll"));
		const auto vstdlib_module = GetModuleHandle(xorstr_("vstdlib.dll"));
		const auto client_code_section = code_section_t{ client_module };
		const auto engine_code_section = code_section_t{ engine_module };
		const auto tier0_code_section = code_section_t{ tier0_module };
		const auto materialsystem_code_section = code_section_t{ materialsystem_module };
		const auto vguimatsurface_code_section = code_section_t{ vguimatsurface_module };

		std::ofstream of_log("supremacy.log");
		of_log << "initializing valve interfaces...\n"; {
			valve::g_local_player = *reinterpret_cast<valve::c_local_player*>(
				find_byte_seq(client_code_section, xorstr_("8B 0D ? ? ? ? 83 FF FF 74 07")) + 0x2u
				);

			valve::g_client = util::get_interface<valve::c_client>(client_module, xorstr_("VClient018"));

			valve::g_global_vars = **reinterpret_cast<valve::global_vars_t***>(
				(*reinterpret_cast<std::uintptr_t**>(valve::g_client))[0u] + (TWENTYTWENTY ? 0x1fu : 0x1bu)
				);

			valve::g_engine = util::get_interface<valve::c_engine>(engine_module, xorstr_("VEngineClient014"));

			valve::g_entity_list = util::get_interface<valve::c_entity_list>(client_module, xorstr_("VClientEntityList003"));

			valve::g_client_state = **reinterpret_cast<valve::client_state_t***>(
				(*reinterpret_cast<std::uintptr_t**>(valve::g_engine))[12u] + 0x10u
				);

			valve::g_move_helper = **reinterpret_cast<valve::c_move_helper***>(
				find_byte_seq(client_code_section, xorstr_("8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01")) + 0x2u
				);

			valve::g_prediction = *reinterpret_cast<valve::prediction_t**>(
				find_byte_seq(client_code_section, xorstr_("A1 ? ? ? ? 8B 40 38 FF D0 84 C0")) + 0x1u
				);

			valve::g_movement = *reinterpret_cast<valve::c_movement**>(
				find_byte_seq(client_code_section, xorstr_("A1 ? ? ? ? B9 ? ? ? ? FF 50 04 EB 07")) + 0x1u
				);

			valve::g_cvar = **reinterpret_cast<valve::c_cvar***>(
				find_byte_seq(client_code_section, xorstr_("8B 0D ? ? ? ? 68 ? ? ? ? 8B 01 FF 50 44 8B C8 89")) + 0x2u
				);

			valve::g_engine_trace = **reinterpret_cast<valve::c_engine_trace***>(
				find_byte_seq(client_code_section, xorstr_("A1 ? ? ? ? F3 0F 11 45 ? 8B 30")) + 0x1u
				);

			valve::g_surface_data = **reinterpret_cast<valve::c_surface_data***>(
				find_byte_seq(client_code_section, xorstr_("8B 0D ? ? ? ? 0F BF 45 C8")) + 0x2u
				);

			valve::g_input = *reinterpret_cast<valve::input_t**>(
				find_byte_seq(client_code_section, xorstr_("B9 ? ? ? ? F3 0F 11 04 24 FF 50 10")) + 0x1u
				);

			valve::g_game_rules = *reinterpret_cast<valve::c_game_rules***>(
				find_byte_seq(client_code_section, xorstr_("A1 ? ? ? ? 85 C0 0F 84 ? ? ? ? 80 B8 ? ? ? ? ? 74")) + 0x1u
				);

			valve::g_game_event_mgr = **reinterpret_cast<valve::c_game_event_mgr***>(
				find_byte_seq(client_code_section, xorstr_("8B 0D ? ? ? ? 50 8B 11 FF 52 24 FF 15 ? ? ? ?")) + 0x2u
				);

			valve::g_studio_render = **reinterpret_cast<valve::studio_render_t***>(
				find_byte_seq(client_code_section, xorstr_("8B 0D ? ? ? ? 8D 54 24 40 52 8B 01 FF 50 34")) + 0x2u
				);

			valve::g_mat_system = **reinterpret_cast<valve::c_mat_system***>(
				find_byte_seq(client_code_section, xorstr_("8B 0D ?? ?? ?? ?? 8B 01 8B 80 ?? ?? ?? ?? FF D0 8B F0 89 74 24 48")) + 0x2u
				);

			valve::g_view_render = **reinterpret_cast<valve::view_render_t***>(
				find_byte_seq(client_code_section, xorstr_("8B 0D ? ? ? ? 8B 01 FF 50 4C 8B 06")) + 0x2u
				);

			valve::g_surface = **reinterpret_cast<valve::c_surface***>(
				find_byte_seq(client_code_section, xorstr_("8B 0D ? ? ? ? FF B7 ? ? ? ? 8B 01 FF 90 ? ? ? ? 8B 7C 24 1C")) + 0x2u
				);

			valve::g_localize = util::get_interface<valve::c_localize>(localize_module, xorstr_("Localize_001"));

			valve::g_font_mgr = *reinterpret_cast<valve::c_font_mgr**>(
				find_byte_seq(vguimatsurface_code_section, xorstr_("74 1D 8B 0D ? ? ? ? 68 ? ? ? ?")) + 0x4u
				);

			valve::g_model_info = *reinterpret_cast<valve::c_model_info**>(
				find_byte_seq(engine_code_section, xorstr_("B9 ? ? ? ? 8B 44 B8 04 83 C0 04 50 A1 ? ? ? ? FF 50 08 8B D0 8B 4C BB 0C")) + 0x1u
				);

			valve::g_beams = *reinterpret_cast<valve::c_beams**>(
				find_byte_seq(client_code_section, xorstr_("B9 ? ? ? ? A1 ? ? ? ? FF 10 A1 ? ? ? ? B9")) + 0x1u
				);

			valve::g_model_render = util::get_interface<valve::c_model_render>(engine_module, xorstr_("VEngineModel016"));

			valve::g_mdl_cache = util::get_interface<valve::c_mdl_cache>(datacache_module, xorstr_("MDLCache004"));

			valve::g_glow_mgr = *reinterpret_cast<valve::glow_mgr_t**>(
				find_byte_seq(client_code_section, xorstr_("0F 11 05 ? ? ? ? 83 C8 01")) + 0x3u
				);

			valve::g_debug_overlay = util::get_interface<valve::c_debug_overlay>(engine_module, xorstr_("VDebugOverlay004"));

			valve::g_effects = util::get_interface<valve::c_effects_t>(engine_module, xorstr_("VEngineEffects001"));

			valve::g_engine_sound = util::get_interface<valve::c_engine_sound>(engine_module, xorstr_("IEngineSoundClient003"));

			valve::g_mdl_cache = util::get_interface<valve::c_mdl_cache>(datacache_module, xorstr_("MDLCache004"));

			valve::g_panel = util::get_interface<valve::c_panel>(vgui2_module, xorstr_("VGUI_Panel009"));
		}

		of_log << "parsing client classes...\n";
		valve::g_net_vars->parse_client_classes();

		of_log << "parsing data maps...\n"; {
			std::vector< valve::data_map_t* > data_maps{};

			auto start = client_code_section.m_base;
			auto size = client_code_section.m_size;

			std::uintptr_t addr{};
			for (; ; ) {
				addr = find_byte_seq({ size, start }, xorstr_("CC CC CC CC C7 05 ? ? ? ? ? ? ? ? C7 05 ? ? ? ? ? ? ? ? C3 CC"));
				if (addr == (start + size))
					break;

				const auto data_map = reinterpret_cast<valve::data_map_t*>(
					*reinterpret_cast<std::uintptr_t*>(addr + 0x6u) - 0x4u
					);
				if (data_map
					&& data_map->m_size
					&& data_map->m_size <= 200
					&& data_map->m_description
					&& data_map->m_name)
					data_maps.emplace_back(data_map);

				size = (start + size) - (addr + 1u);

				start = addr + 1u;
			}

			if (data_maps.empty())
				return;

			valve::g_net_vars->parse_data_maps(data_maps);
		}

		of_log << "creating folders...\n"; {
			std::filesystem::create_directory(xorstr_("c:\\supremacy\\"));
			std::filesystem::create_directory(xorstr_("c:\\supremacy\\configs\\"));
		}

		of_log << "initializing addresses...\n"; {
			m_addresses.m_random_float = reinterpret_cast<addresses_t::random_float_t>(GetProcAddress(vstdlib_module, xorstr_("RandomFloat")));
			m_addresses.m_random_int = reinterpret_cast<addresses_t::random_int_t>(GetProcAddress(vstdlib_module, xorstr_("RandomInt")));
			m_addresses.m_random_seed = reinterpret_cast<addresses_t::random_seed_t>(GetProcAddress(vstdlib_module, xorstr_("RandomSeed")));

			m_addresses.m_reset_anim_state = find_byte_seq(client_code_section, xorstr_("56 6A 01 68 ? ? ? ? 8B F1"));
			m_addresses.m_update_anim_state = find_byte_seq(client_code_section, xorstr_("55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24"));
			m_addresses.m_weapon_prefix = find_byte_seq(client_code_section, xorstr_("53 56 57 8B F9 33 F6 8B 4F ? 8B 01 FF 90 ? ? ? ? 89 47"));
			m_addresses.m_set_abs_angles = find_byte_seq(client_code_section, xorstr_("55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1"));
			m_addresses.m_set_abs_origin = find_byte_seq(client_code_section, xorstr_("55 8B EC 83 E4 F8 51 53 56 57 8B F1"));
			m_addresses.m_lookup_seq_act = find_byte_seq(client_code_section, xorstr_("55 8B EC 53 8B 5D 08 56 8B F1 83"));
			m_addresses.m_breakable = find_byte_seq(client_code_section, xorstr_("55 8B EC 51 56 8B F1 85 F6 74 68"));
			m_addresses.m_user_cmd_calc_checksum = find_byte_seq(client_code_section, xorstr_("53 8B D9 83 C8"));

			m_addresses.m_pred_seed = *reinterpret_cast<int**>(
				find_byte_seq(client_code_section, xorstr_("A3 ? ? ? ? 66 0F 6E 86")) + 0x1u
				);

			m_addresses.m_pred_player = *reinterpret_cast<valve::c_player**>(
				find_byte_seq(client_code_section, xorstr_("89 35 ? ? ? ? F3 0F 10 48")) + 0x2u
				);

			const auto angle_matrix_rel = find_byte_seq(client_code_section, xorstr_("E8 ? ? ? ? 8B 07 89 46 0C"));
			m_addresses.m_angle_matrix = reinterpret_cast<addresses_t::angle_matrix_t>(
				angle_matrix_rel + 0x1u + sizeof(std::uintptr_t) + *reinterpret_cast<std::ptrdiff_t*>(angle_matrix_rel + 0x1u)
				);

			m_addresses.m_line_goes_thru_smoke = reinterpret_cast<addresses_t::line_goes_thru_smoke_t>(
				find_byte_seq(client_code_section, "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F")
				);

			m_addresses.m_set_collision_bounds = find_byte_seq(
				client_code_section, xorstr_("53 8B DC 83 EC 08 83 E4 F8 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 83 EC 10 56 57 8B 7B")
			);

			m_addresses.m_invalidate_bone_cache = find_byte_seq(client_code_section, xorstr_("80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81"));
			m_addresses.m_lookup_bone = find_byte_seq(client_code_section, xorstr_("55 8B EC 53 56 8B F1 57 83 BE ? ? ? ? ? 75"));
			m_addresses.m_lookup_sequence = find_byte_seq(client_code_section, xorstr_("55 8B EC 56 8B F1 83 BE 4C 29 00 00 00 75 14 8B 46 04 8D 4E 04 FF 50 ?? 85 C0 74 07 8B CE E8 ?? ?? ?? ?? 8B B6 4C 29 00 00 85 F6 74 48"));
			m_addresses.m_trace_filter_simple_vtable = *reinterpret_cast<std::uintptr_t*>(
				find_byte_seq(client_code_section, xorstr_("55 8B EC 83 E4 F0 83 EC 7C 56 52")) + 0x3du
				);

			m_addresses.m_trace_filter_skip_two_entities_vtable = *reinterpret_cast<std::uintptr_t*>(
				find_byte_seq(client_code_section, xorstr_("C7 45 ? ? ? ? ? 89 45 E4 8B 01")) + 0x3u
				);

			m_addresses.m_write_user_cmd = find_byte_seq(client_code_section, xorstr_("55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D"));
			m_addresses.m_weapon_system = *reinterpret_cast<std::uintptr_t*>(
				find_byte_seq(client_code_section, xorstr_("8B 35 ? ? ? ? FF 10 0F B7 C0")) + 0x2u
				);

			const auto calc_shotgun_spread_rel = find_byte_seq(client_code_section, xorstr_("E8 ? ? ? ? EB 38 83 EC 08"));

			m_addresses.m_calc_shotgun_spread =
				calc_shotgun_spread_rel + 0x1u + sizeof(std::uintptr_t)
				+ *reinterpret_cast<std::ptrdiff_t*>(calc_shotgun_spread_rel + 0x1u);

			m_addresses.m_key_values_init = find_byte_seq(client_code_section, xorstr_("55 8B EC 51 33 C0 C7 45"));
			m_addresses.m_key_values_load_from_buffer = find_byte_seq(client_code_section, xorstr_("55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89"));
			m_addresses.m_cam_sv_cheats_ret = find_byte_seq(client_code_section, xorstr_("85 C0 75 30 38 86"));
			m_addresses.m_add_notify = find_byte_seq(engine_code_section, xorstr_("55 8B EC 81 EC 10 08 00 00 80 3D"));
			m_addresses.m_ret_to_scope_arc = find_byte_seq(client_code_section, xorstr_("6A 00 FF 50 3C 8B 0D ? ? ? ? FF B7 ? ? ? ?")) + 0x5u;
			m_addresses.m_ret_to_scope_lens = find_byte_seq(client_code_section, xorstr_("FF 50 3C 8B 4C 24 20")) + 0x3u;
			m_addresses.m_set_clan_tag = reinterpret_cast<addresses_t::set_clan_tag_t>(find_byte_seq(engine_code_section, xorstr_("53 56 57 8B DA 8B F9 FF 15")));
			m_addresses.m_start_drawing = find_byte_seq(vguimatsurface_code_section, xorstr_("55 8B EC 83 E4 C0 83 EC 38"));
			m_addresses.m_finish_drawing = find_byte_seq(vguimatsurface_code_section, xorstr_("8B 0D ? ? ? ? 56 C6 05 ? ? ? ? ?"));
			m_addresses.m_add_clred_text = find_byte_seq(vguimatsurface_code_section, xorstr_("55 8B EC 81 EC ? ? ? ? 56 57 FF 75 10"));
			m_addresses.m_calc_text_size = find_byte_seq(vguimatsurface_code_section, xorstr_("55 8B EC 83 EC 18 83 7D 0C 00"));
			m_addresses.m_set_font_glyph_set = find_byte_seq(vguimatsurface_code_section, xorstr_("55 8B EC 83 EC 08 8B 45 08 53"));
			m_addresses.m_has_c4 = find_byte_seq(client_code_section, xorstr_("56 8B F1 85 F6 74 31"));
			m_addresses.m_ret_insert_into_tree = find_byte_seq(client_code_section, xorstr_("89 44 24 14 EB 08 C7 44 24 ? ? ? ? ? 8B 45"));
			m_addresses.m_smoke_count = find_byte_seq(client_code_section, xorstr_("A3 ? ? ? ? 57 8B CB")) + 0x1u;
			m_addresses.m_disable_post_processing = find_byte_seq(client_code_section, xorstr_("80 3D ? ? ? ? ? 53 56 57 0F 85")) + 0x2u;
			m_addresses.m_crosshair_ret = find_byte_seq(client_code_section, xorstr_("85 C0 0F 84 ? ? ? ? E8 ? ? ? ? 99"));
			m_addresses.m_ret_to_scope_clear = find_byte_seq(client_code_section, xorstr_("0F 82 ? ? ? ? FF 50 3C")) + 0x9u;
			m_addresses.m_ret_to_scope_blurry = find_byte_seq(client_code_section, xorstr_("FF B7 ? ? ? ? 8B 01 FF 90 ? ? ? ? 8B 4C 24 24")) - 0x6u;
			m_addresses.m_ret_to_fire_bullet = find_byte_seq(client_code_section, xorstr_("8B 0D ? ? ? ? F3 0F 7E 00 8B 40 08 89 45 E4"));
			m_addresses.m_ret_set_first_person_viewangles = find_byte_seq(client_code_section, xorstr_("8B 5D 0C 8B 08 89 0B 8B 48 04 89 4B 04 B9"));
			m_addresses.m_allow_extrapolation = find_byte_seq(client_code_section, xorstr_("A2 ? ? ? ? 8B 45 E8"));
			m_addresses.m_item_system = find_byte_seq(client_code_section, xorstr_("A1 ? ? ? ? 85 C0 75 ? A1 ? ? ? ? 56 68 E8 07 00 00"));
			m_addresses.m_hud = *reinterpret_cast<std::uintptr_t*>(
				find_byte_seq(client_code_section, xorstr_("B9 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 89")) + 0x1u
				);
			m_addresses.m_find_hud_element = find_byte_seq(client_code_section, xorstr_("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28"));
			m_addresses.m_clear_hud_wpn = find_byte_seq(client_code_section, xorstr_("55 8B EC 83 EC 0C 53 56 8B 71 58"));
			m_addresses.m_clear_hud_wpn_icon = find_byte_seq(client_code_section, xorstr_("55 8B EC 51 53 56 8B 75 08 8B D9 57 6B"));
			m_addresses.m_construct_voice_data_message = find_byte_seq(engine_code_section, xorstr_("56 57 8B F9 8D 4F 08 C7 07 ? ? ? ? E8 ? ? ? ? C7"));
			m_addresses.m_compute_hitbox_surround_box = find_byte_seq(client_code_section, xorstr_("E9 ? ? ? ? 32 C0 5D"));
			m_addresses.m_lock_studio_hdr = find_byte_seq(client_code_section, xorstr_("55 8B EC 51 53 8B D9 56 57 8D B3"));
			m_addresses.m_setup_movement = find_byte_seq(client_code_section, xorstr_("55 8B EC 83 E4 F8 81 ? ? ? ? ? 56 57 8B ? ? ? ? ? 8B F1"));
			m_addresses.m_get_sequence_linear_motion = find_byte_seq(client_code_section, xorstr_("55 8B EC 83 EC 0C 56 8B F1 57 8B FA 85 F6 75 14"));
			m_addresses.m_invalidate_physics_recursive = find_byte_seq(client_code_section, xorstr_("55 8B EC 83 E4 F8 83 EC 0C 53 8B 5D 08 8B C3"));
			m_addresses.m_thread_id_allocated = find_byte_seq(tier0_code_section, xorstr_("C6 86 ? ? ? ? ? 83 05 ? ? ? ? ? 5E 75 04 33 C0 87 07")) + 0x2u;
		}

		of_log << "initializing cvars...\n"; {
			m_cvars.m_r_jiggle_bones = valve::g_cvar->find_var(xorstr_("r_jiggle_bones"));
			m_cvars.m_cl_interp = valve::g_cvar->find_var(xorstr_("cl_interp"));
			m_cvars.m_cl_interp_ratio = valve::g_cvar->find_var(xorstr_("cl_interp_ratio"));
			m_cvars.m_sv_client_min_interp_ratio = valve::g_cvar->find_var(xorstr_("sv_client_min_interp_ratio"));
			m_cvars.m_sv_client_max_interp_ratio = valve::g_cvar->find_var(xorstr_("sv_client_max_interp_ratio"));
			m_cvars.m_cl_updaterate = valve::g_cvar->find_var(xorstr_("cl_updaterate"));
			m_cvars.m_sv_minupdaterate = valve::g_cvar->find_var(xorstr_("sv_minupdaterate"));
			m_cvars.m_sv_maxupdaterate = valve::g_cvar->find_var(xorstr_("sv_maxupdaterate"));
			m_cvars.m_ff_damage_reduction_bullets = valve::g_cvar->find_var(xorstr_("ff_damage_reduction_bullets"));
			m_cvars.m_ff_damage_bullet_penetration = valve::g_cvar->find_var(xorstr_("ff_damage_bullet_penetration"));
			m_cvars.m_sv_autobunnyhopping = valve::g_cvar->find_var(xorstr_("sv_autobunnyhopping"));
			m_cvars.m_sv_clockcorrection_msecs = valve::g_cvar->find_var(xorstr_("sv_clockcorrection_msecs"));
			m_cvars.m_sv_maxvelocity = valve::g_cvar->find_var(xorstr_("sv_maxvelocity"));
			m_cvars.m_sv_friction = valve::g_cvar->find_var(xorstr_("sv_friction"));
			m_cvars.m_sv_accelerate = valve::g_cvar->find_var(xorstr_("sv_accelerate"));
			m_cvars.m_sv_accelerate_use_weapon_speed = valve::g_cvar->find_var(xorstr_("sv_accelerate_use_weapon_speed"));
			m_cvars.m_weapon_accuracy_nospread = valve::g_cvar->find_var(xorstr_("weapon_accuracy_nospread"));
			m_cvars.m_sv_maxunlag = valve::g_cvar->find_var(xorstr_("sv_maxunlag"));
			m_cvars.m_sv_enablebunnyhopping = valve::g_cvar->find_var(xorstr_("sv_enablebunnyhopping"));
			m_cvars.m_sv_jump_impulse = valve::g_cvar->find_var(xorstr_("sv_jump_impulse"));
			m_cvars.m_sv_gravity = valve::g_cvar->find_var(xorstr_("sv_gravity"));
			m_cvars.m_weapon_recoil_scale = valve::g_cvar->find_var(xorstr_("weapon_recoil_scale"));
			m_cvars.m_weapon_accuracy_shotgun_spread_patterns = valve::g_cvar->find_var(xorstr_("weapon_accuracy_shotgun_spread_patterns"));
			m_cvars.m_molotov_throw_detonate_time = valve::g_cvar->find_var(xorstr_("molotov_throw_detonate_time"));
			m_cvars.m_weapon_molotov_maxdetonateslope = valve::g_cvar->find_var(xorstr_("weapon_molotov_maxdetonateslope"));
			m_cvars.m_cl_fullupdate = valve::g_cvar->find_var(xorstr_("cl_fullupdate"));
			m_cvars.m_mp_teammates_are_enemies = valve::g_cvar->find_var(xorstr_("mp_teammates_are_enemies"));
			m_cvars.m_cl_lagcompensation = valve::g_cvar->find_var(xorstr_("cl_lagcompensation"));
			m_cvars.m_name = valve::g_cvar->find_var(xorstr_("name"));
			m_cvars.m_voice_loopback = valve::g_cvar->find_var(xorstr_("voice_loopback"));
			m_cvars.m_pitch = valve::g_cvar->find_var(xorstr_("m_pitch"));
			m_cvars.m_yaw = valve::g_cvar->find_var(xorstr_("m_yaw"));
			m_cvars.m_sensitivity = valve::g_cvar->find_var(xorstr_("sensitivity"));
			m_cvars.m_mp_damage_scale_t_head = valve::g_cvar->find_var(xorstr_("mp_damage_scale_t_head"));
			m_cvars.m_mp_damage_scale_t_body = valve::g_cvar->find_var(xorstr_("mp_damage_scale_t_body"));
			m_cvars.m_mp_damage_scale_ct_head = valve::g_cvar->find_var(xorstr_("mp_damage_scale_ct_head"));
			m_cvars.m_mp_damage_scale_ct_body = valve::g_cvar->find_var(xorstr_("mp_damage_scale_ct_body"));
		}

		of_log << "a few useless corrections...\n";
		sdk::g_config_system->volume = valve::g_cvar->find_var(xorstr_("volume"))->get_float() * 100;

		of_log << "setting up event listener...\n"; {
			valve::g_game_event_mgr->add_listener(&hooks::g_event_listener, xorstr_("player_footstep"), false);
			valve::g_game_event_mgr->add_listener(&hooks::g_event_listener, xorstr_("player_hurt"), false);
			valve::g_game_event_mgr->add_listener(&hooks::g_event_listener, xorstr_("weapon_fire"), false);
			valve::g_game_event_mgr->add_listener(&hooks::g_event_listener, xorstr_("bullet_impact"), false);
			valve::g_game_event_mgr->add_listener(&hooks::g_event_listener, xorstr_("item_equip"), false);
			valve::g_game_event_mgr->add_listener(&hooks::g_event_listener, xorstr_("item_purchase"), false);
			valve::g_game_event_mgr->add_listener(&hooks::g_event_listener, xorstr_("round_freeze_end"), false);
			valve::g_game_event_mgr->add_listener(&hooks::g_event_listener, xorstr_("round_prestart"), false);
		}

		of_log << "creating fonts...\n";
		hacks::g_visuals->init();

		of_log << "setting up proxy hooks...\n"; {
			auto& velocity_modifier_prop = std::get< valve::recv_prop_t* >(valve::g_net_vars->entry(xorstr_("CCSPlayer->m_flVelocityModifier")));
			hooks::orig_velocity_modifier = reinterpret_cast<decltype(hooks::orig_velocity_modifier)>(velocity_modifier_prop->m_proxy_fn);
			velocity_modifier_prop->m_proxy_fn = reinterpret_cast<std::uintptr_t>(&hooks::velocity_modifier);
		}

		of_log << "setting up hooks...\n"; {
			hooks::h_window = FindWindow(xorstr_("Valve001"), NULL);
			hooks::old_wnd_proc = (WNDPROC)SetWindowLong(hooks::h_window, GWL_WNDPROC, util::force_cast<LONG>(hooks::wnd_proc));

			if (MH_CreateHook(
				(*reinterpret_cast<LPVOID**>(d3d9_device))[17u],
				reinterpret_cast<LPVOID>(&hooks::d3d9_present),
				reinterpret_cast<LPVOID*>(&hooks::orig_d3d9_present)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				(*reinterpret_cast<LPVOID**>(valve::g_panel))[41u],
				reinterpret_cast<LPVOID>(&hooks::paint_traverse),
				reinterpret_cast<LPVOID*>(&hooks::orig_paint_traverse)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				reinterpret_cast<LPVOID>(
					find_byte_seq(engine_code_section, xorstr_("55 8B EC 83 EC 40 53 8B D9 8B 0D ? ? ? ? 89 5D F8"))
					),
				reinterpret_cast<LPVOID>(&hooks::paint),
				reinterpret_cast<LPVOID*>(&hooks::orig_paint)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				(*reinterpret_cast<LPVOID**>(valve::g_cvar->find_var(xorstr_("sv_cheats"))))[13u],
				reinterpret_cast<LPVOID>(&hooks::sv_cheats_get_int),
				reinterpret_cast<LPVOID*>(&hooks::orig_sv_cheats_get_int)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				(*reinterpret_cast<LPVOID**>(valve::g_studio_render))[29u],
				reinterpret_cast<LPVOID>(&hooks::draw_model),
				reinterpret_cast<LPVOID*>(&hooks::orig_draw_model)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				(*reinterpret_cast<LPVOID**>(valve::g_view_render))[6u],
				reinterpret_cast<LPVOID>(&hooks::render_view),
				reinterpret_cast<LPVOID*>(&hooks::orig_render_view)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				(*reinterpret_cast<LPVOID**>(valve::g_surface))[67u],
				reinterpret_cast<LPVOID>(&hooks::lock_cursor),
				reinterpret_cast<LPVOID*>(&hooks::orig_lock_cursor)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				(*reinterpret_cast<LPVOID**>(valve::g_surface))[116u],
				reinterpret_cast<LPVOID>(&hooks::on_screen_size_changed),
				reinterpret_cast<LPVOID*>(&hooks::orig_on_screen_size_changed)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				(*reinterpret_cast<LPVOID**>(valve::g_surface))[15u],
				reinterpret_cast<LPVOID>(&hooks::draw_set_clr),
				reinterpret_cast<LPVOID*>(&hooks::orig_draw_set_clr)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				(*reinterpret_cast<LPVOID**>(valve::g_client))[22u],
				reinterpret_cast<LPVOID>(&hooks::chl_create_move_proxy),
				reinterpret_cast<LPVOID*>(&hooks::orig_chl_create_move)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				(*reinterpret_cast<LPVOID**>(valve::g_client))[37u],
				reinterpret_cast<LPVOID>(&hooks::frame_stage_notify),
				reinterpret_cast<LPVOID*>(&hooks::orig_frame_stage_notify)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				reinterpret_cast<LPVOID>(
					find_byte_seq(client_code_section, xorstr_("55 8B EC 83 E4 F0 B8 D8"))
					),
				reinterpret_cast<LPVOID>(&hooks::setup_bones),
				reinterpret_cast<LPVOID*>(&hooks::orig_setup_bones)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				reinterpret_cast<LPVOID>(
					find_byte_seq(client_code_section, xorstr_("55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74"))
					),
				reinterpret_cast<LPVOID>(&hooks::update_client_side_anim),
				reinterpret_cast<LPVOID*>(&hooks::orig_update_client_side_anim)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				reinterpret_cast<LPVOID>(
					find_byte_seq(client_code_section, xorstr_("55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 8B F1 57 89 74 24 1C"))
					),
				reinterpret_cast<LPVOID>(&hooks::do_extra_bones_processing),
				reinterpret_cast<LPVOID*>(&hooks::orig_do_extra_bones_processing)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				reinterpret_cast<LPVOID>(
					find_byte_seq(client_code_section, xorstr_("55 8B EC 57 8B F9 8B 0D ? ? ? ? 8B 01 8B 80 ? ? ? ?"))
					),
				reinterpret_cast<LPVOID>(&hooks::accumulate_layers),
				reinterpret_cast<LPVOID*>(&hooks::orig_accumulate_layers)
			) != MH_OK)
				return;
#if 0
			if (MH_CreateHook(
				reinterpret_cast<LPVOID>(
					find_byte_seq(client_code_section, xorstr_("55 8B EC 83 E4 F0 B8 F8 10"))
					),
				reinterpret_cast<LPVOID>(&hooks::standard_blending_rules),
				reinterpret_cast<LPVOID*>(&hooks::orig_standard_blending_rules)
			) != MH_OK)
				return;
#endif
			if (MH_CreateHook(
				(*reinterpret_cast<LPVOID**>((valve::client_state_t*)(uint32_t(valve::g_client_state) + 0x8)))[5u],
				reinterpret_cast<LPVOID>(&hooks::packet_start),
				reinterpret_cast<LPVOID*>(&hooks::orig_packet_start)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				(*reinterpret_cast<LPVOID**>((valve::client_state_t*)(uint32_t(valve::g_client_state) + 0x8)))[6u],
				reinterpret_cast<LPVOID>(&hooks::packet_end),
				reinterpret_cast<LPVOID*>(&hooks::orig_packet_end)
			) != MH_OK)
				return;
#if 0
			if (MH_CreateHook(
				(*reinterpret_cast<LPVOID**>(valve::g_prediction))[19u],
				reinterpret_cast<LPVOID>(&hooks::run_command),
				reinterpret_cast<LPVOID*>(&hooks::orig_run_command)
			) != MH_OK)
				return;
#endif
			if (MH_CreateHook(
				reinterpret_cast<LPVOID>(
					find_byte_seq(
						client_code_section,
						xorstr_("56 8B F1 8B 8E ? ? ? ? 83 F9 FF 74 21")
					)
					),
				reinterpret_cast<LPVOID>(&hooks::physics_simulate),
				reinterpret_cast<LPVOID*>(&hooks::orig_physics_simulate)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				reinterpret_cast<LPVOID>(
					find_byte_seq(
						client_code_section,
						xorstr_("55 8B EC 83 EC 10 53 56 8B F1 57 80 BE ? ? ? ? ? 75 41")
					)
					),
				reinterpret_cast<LPVOID>(&hooks::on_latch_interpolated_vars),
				reinterpret_cast<LPVOID*>(&hooks::orig_on_latch_interpolated_vars)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				reinterpret_cast<LPVOID>(
					find_byte_seq(
						client_code_section,
						xorstr_("56 8B F1 E8 ? ? ? ? 3B F0")
					)
					),
				reinterpret_cast<LPVOID>(&hooks::should_interpolate),
				reinterpret_cast<LPVOID*>(&hooks::orig_should_interpolate)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				(*reinterpret_cast<LPVOID**>(valve::g_client))[24u],
				reinterpret_cast<LPVOID>(&hooks::write_user_cmd_delta_to_buffer),
				reinterpret_cast<LPVOID*>(&hooks::orig_write_user_cmd_delta_to_buffer)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				reinterpret_cast<LPVOID>(
					find_byte_seq(
						client_code_section,
						xorstr_("55 8B EC 83 E4 F8 83 EC 5C 53 8B D9 56 57 83")
					)
					),
				reinterpret_cast<LPVOID>(&hooks::modify_eye_pos),
				reinterpret_cast<LPVOID*>(&hooks::orig_modify_eye_pos)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				reinterpret_cast<LPVOID>(
					find_byte_seq(
						engine_code_section,
						xorstr_("55 8B EC 83 E4 F8 83 EC 4C A1 ? ? ? ? 80 B8 ? ? ? ? ?")
					)
					),
				reinterpret_cast<LPVOID>(&hooks::process_temp_entities),
				reinterpret_cast<LPVOID*>(&hooks::orig_process_temp_entities)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				(*reinterpret_cast<LPVOID**>(valve::g_client))[7u],
				reinterpret_cast<LPVOID>(&hooks::level_shutdown),
				reinterpret_cast<LPVOID*>(&hooks::orig_level_shutdown)
			) != MH_OK)
				return;

			if (TWENTYTWENTY) {
				if (MH_CreateHook(
					reinterpret_cast<LPVOID>(
						find_byte_seq(
							client_code_section,
							xorstr_("55 8B EC 83 EC 14 53 8B 5D 0C 56 57 85 DB 74")
						)
						),
					reinterpret_cast<LPVOID>(&hooks::glow_effect_spectator),
					reinterpret_cast<LPVOID*>(&hooks::orig_glow_effect_spectator)
				) != MH_OK)
					return;

			}
			else if (MH_CreateHook(
				reinterpret_cast<LPVOID>(
					find_byte_seq(
						client_code_section,
						xorstr_("55 8B EC 83 EC 14 53 8B 5D ? 85 DB 75")
					)
					),
				reinterpret_cast<LPVOID>(&hooks::glow_effect_spectator),
				reinterpret_cast<LPVOID*>(&hooks::orig_glow_effect_spectator)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				reinterpret_cast<LPVOID>(
					find_byte_seq(
						materialsystem_code_section,
						xorstr_("55 8B EC 83 EC ? 56 8B F1 8A 46")
					)
					),
				reinterpret_cast<LPVOID>(&hooks::get_clr_modulation),
				reinterpret_cast<LPVOID*>(&hooks::orig_get_clr_modulation)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				reinterpret_cast<LPVOID>(
					find_byte_seq(
						materialsystem_code_section,
						xorstr_("56 8B F1 8A 46 20 C0 E8 02 A8 01 75 0B 6A 00 6A 00 6A 00 E8 ? ? ? ? 80 7E 22 05 76 0E")
					)
					),
				reinterpret_cast<LPVOID>(&hooks::get_alpha_modulation),
				reinterpret_cast<LPVOID*>(&hooks::orig_get_alpha_modulation)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				reinterpret_cast<LPVOID>(
					find_byte_seq(
						client_code_section,
						xorstr_("55 8B EC 83 E4 F8 51 8B 45 18")
					)
					),
				reinterpret_cast<LPVOID>(&hooks::draw_models),
				reinterpret_cast<LPVOID*>(&hooks::orig_draw_models)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				reinterpret_cast<LPVOID>(
					find_byte_seq(
						engine_code_section,
						xorstr_("8B 0D ? ? ? ? 81 F9 ? ? ? ? 75 ? A1 ? ? ? ? 35 ? ? ? ? EB ? 8B 01 FF 50 ? 83 F8 ? 0F 85 ? ? ? ? 8B 0D")
					)
					),
				reinterpret_cast<LPVOID>(&hooks::is_using_static_prop_debug_modes),
				reinterpret_cast<LPVOID*>(&hooks::orig_is_using_static_prop_debug_modes)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				(*reinterpret_cast<LPVOID**>(valve::g_engine->bsp_tree_query()))[6u],
				reinterpret_cast<LPVOID>(&hooks::list_leaves_in_box),
				reinterpret_cast<LPVOID*>(&hooks::orig_list_leaves_in_box)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				(*reinterpret_cast<LPVOID**>(valve::g_mat_system))[21u],
				reinterpret_cast<LPVOID>(&hooks::override_cfg),
				reinterpret_cast<LPVOID*>(&hooks::orig_override_cfg)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				reinterpret_cast<LPVOID>(
					find_byte_seq(client_code_section, xorstr_("55 8B EC 8B 0D ? ? ? ? 83 EC 0C 8B 01 53"))
					),
				reinterpret_cast<LPVOID>(&hooks::fog_enabled),
				reinterpret_cast<LPVOID*>(&hooks::orig_fog_enabled)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				reinterpret_cast<LPVOID>(
					find_byte_seq(vguimatsurface_code_section, xorstr_("80 3D ? ? ? ? ? 74 06 B8"))
					),
				reinterpret_cast<LPVOID>(&hooks::get_foreign_fallback_font_name),
				reinterpret_cast<LPVOID*>(&hooks::orig_get_foreign_fallback_font_name)
			) != MH_OK)
				return;

			const auto client_mode = **reinterpret_cast<std::uintptr_t***>(
				(*reinterpret_cast<std::uintptr_t**>(valve::g_client))[10u] + 0x5u
				);

			if (MH_CreateHook(
				(*reinterpret_cast<LPVOID**>(client_mode))[18u],
				reinterpret_cast<LPVOID>(&hooks::override_view),
				reinterpret_cast<LPVOID*>(&hooks::orig_override_view)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				(*reinterpret_cast<LPVOID**>(client_mode))[44u],
				reinterpret_cast<LPVOID>(&hooks::do_post_screen_space_effects),
				reinterpret_cast<LPVOID*>(&hooks::orig_do_post_screen_space_effects)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				(*reinterpret_cast<LPVOID**>(valve::g_cvar->find_var(xorstr_("weapon_debug_spread_show"))))[13u],
				reinterpret_cast<LPVOID>(&hooks::weapon_debug_spread_show_get_int),
				reinterpret_cast<LPVOID*>(&hooks::orig_weapon_debug_spread_show_get_int)
			) != MH_OK)
				return;

			if (TWENTYTWENTY) {
				if (MH_CreateHook(
					reinterpret_cast<LPVOID>(
						find_byte_seq(client_code_section, xorstr_("55 8B EC 83 EC 14 53 56 57 FF 75 18"))
						),
					reinterpret_cast<LPVOID>(&hooks::calc_view),
					reinterpret_cast<LPVOID*>(&hooks::orig_calc_view)
				) != MH_OK)
					return;
			}
			else if (MH_CreateHook(
				reinterpret_cast<LPVOID>(
					find_byte_seq(client_code_section, xorstr_("55 8B EC 83 E4 F8 83 EC 24 53 56 57 FF 75"))
					),
				reinterpret_cast<LPVOID>(&hooks::calc_view),
				reinterpret_cast<LPVOID*>(&hooks::orig_calc_view)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				reinterpret_cast<LPVOID>(
					find_byte_seq(client_code_section, xorstr_("56 8B F1 85 F6 74 32"))
					),
				reinterpret_cast<LPVOID>(&hooks::get_eye_angles),
				reinterpret_cast<LPVOID*>(&hooks::orig_get_eye_angles)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				(*reinterpret_cast<LPVOID**>(valve::g_engine))[101u],
				reinterpret_cast<LPVOID>(&hooks::aspect_ratio),
				reinterpret_cast<LPVOID*>(&hooks::orig_aspect_ratio)
			) != MH_OK)
				return;

			if (MH_CreateHook(
				reinterpret_cast<LPVOID>(
					find_byte_seq(engine_code_section, xorstr_("55 8B EC 83 E4 F8 A1 ? ? ? ? 81 EC 84 01 00"))
					),
				reinterpret_cast<LPVOID>(&hooks::svc_msg_voice_data),
				reinterpret_cast<LPVOID*>(&hooks::orig_svc_msg_voice_data)
			) != MH_OK)
				return;

			if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
				return;
		}

		of_log << "initializing multi-threading...\n";
		auto tier0 = GetModuleHandle(xorstr_("tier0.dll"));
		if (tier0)		
			thread_pool->init();		

		of_log << "done!\n";
		of_log.close();

		valve::g_cvar->find_var(xorstr_("developer"))->set_int(1);
		valve::g_cvar->find_var(xorstr_("con_enable"))->set_int(2);
		valve::g_cvar->find_var(xorstr_("con_filter_enable"))->set_int(1);
		valve::g_cvar->find_var(xorstr_("con_filter_text"))->set_str(xorstr_("[supremacy]"));
		valve::g_cvar->find_var(xorstr_("con_filter_text_out"))->set_str(xorstr_(" "));
		valve::g_cvar->find_var(xorstr_("contimes"))->set_int(10);
		valve::g_cvar->find_var(xorstr_("@panorama_disable_blur"))->set_int(1);
		valve::g_engine->unrestricted_cmd(xorstr_("clear; toggleconsole; "));
	}

	bool c_context::will_shoot(valve::c_weapon* const weapon, const valve::user_cmd_t& user_cmd) const {
		if (!weapon)
			return false;

		const auto item_index = weapon->item_index();
		const auto wpn_data = weapon->wpn_data();

		if ((item_index < valve::e_item_index::flashbang || item_index > valve::e_item_index::inc_grenade)
			&& !(user_cmd.m_buttons & valve::e_buttons::in_attack)
			&& (!(user_cmd.m_buttons & valve::e_buttons::in_attack2)
				|| weapon->next_secondary_attack() >= valve::g_global_vars->m_cur_time
				|| (item_index != valve::e_item_index::revolver && (!wpn_data || wpn_data->m_unk_type != 1))
				)
			)
			return false;

		return true;
	}

	bool c_context::can_shoot(
		valve::c_weapon* const weapon, const valve::user_cmd_t& user_cmd,
		bool r8_ret, const int shift_amount, const bool normal
	) const {
		const auto& anim_data = hacks::g_anim_sync->local_data();
		if (!normal
			&& anim_data.m_shot_valid_wpn
			&& anim_data.m_shot_cmd_number > valve::g_client_state->m_last_cmd_out
			&& anim_data.m_shot_cmd_number < (valve::g_client_state->m_last_cmd_out + 100))
			return false;

		if (!weapon
			|| m_freeze_time
			|| !valve::g_local_player
			|| user_cmd.m_weapon_select
			|| valve::g_local_player->flags() & valve::e_ent_flags::frozen
			|| !valve::g_local_player->alive())
			return false;

		const auto wpn_data = weapon->wpn_data();
		if (!wpn_data)
			return false;

		const auto item_index = weapon->item_index();

		const auto v10 = static_cast<std::size_t>(wpn_data->m_unk_type - 2) <= 5 || item_index == valve::e_item_index::taser;

		const auto cur_time = valve::to_time(
			v10 ? valve::g_local_player->tick_base() - shift_amount : valve::g_local_player->tick_base()
		);

		if (valve::g_local_player->next_attack() > cur_time)
			return false;

		if (item_index == valve::e_item_index::c4)
			return true;

		if (item_index >= valve::e_item_index::flashbang
			&& item_index <= valve::e_item_index::inc_grenade)
			return weapon->throw_time() > 0.f && cur_time > weapon->throw_time();

		if ((v10 || item_index == valve::e_item_index::taser) && (weapon->ammo() <= 0 || weapon->in_reload()))
			return false;

		if (weapon->next_primary_attack() > cur_time)
			return false;

		return item_index == valve::e_item_index::revolver ? r8_ret : true;
	}

	void c_context::add_packet(const int cmd_number) {
		const auto user_cmd = &valve::g_input->m_cmds[cmd_number % 150];
		if (!user_cmd) {
			m_last_sent_cmd_number = cmd_number;
			m_sented_cmds.emplace_back(cmd_number);

			return;
		}

		if (user_cmd->m_tick != std::numeric_limits< int >::max()) {
			if (!hacks::g_exploits->cur_shift_amount()
				|| hacks::g_exploits->type() == 2
				|| hacks::g_exploits->type() == 3)
				if (valve::g_client_state->m_last_cmd_out != hacks::g_exploits->recharge_cmd())
					m_last_sent_cmd_number = cmd_number;
		}

		m_sented_cmds.emplace_back(cmd_number);

		for (auto& shot : hacks::g_shots->elements()) {
			if (shot.m_cmd_number != -1)
				continue;

			shot.m_cmd_number = cmd_number;
		}
	}
}