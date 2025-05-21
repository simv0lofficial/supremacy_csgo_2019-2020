#include "../../supremacy.hpp"
#include "../../dependencies/imgui/settings.hpp"

namespace supremacy::hooks
{
	LRESULT __stdcall wnd_proc(const HWND hwnd, const UINT msg, const WPARAM wparam, const LPARAM lparam) {
		switch (msg) {
		case WM_LBUTTONDOWN:
			g_input->set_down(VK_LBUTTON);
			break;
		case WM_LBUTTONUP:
			g_input->set_up(VK_LBUTTON);
			break;
		case WM_RBUTTONDOWN:
			g_input->set_down(VK_RBUTTON);
			break;
		case WM_RBUTTONUP:
			g_input->set_up(VK_RBUTTON);
			break;
		case WM_MBUTTONDOWN:
			g_input->set_down(VK_MBUTTON);
			break;
		case WM_MBUTTONUP:
			g_input->set_up(VK_MBUTTON);
			break;
		case WM_XBUTTONDOWN:
			if (GET_XBUTTON_WPARAM(wparam) == XBUTTON1)
				g_input->set_down(VK_XBUTTON1);
			else if (GET_XBUTTON_WPARAM(wparam) == XBUTTON2)
				g_input->set_down(VK_XBUTTON2);
			break;
		case WM_XBUTTONUP:
			if (GET_XBUTTON_WPARAM(wparam) == XBUTTON1)
				g_input->set_up(VK_XBUTTON1);
			else if (GET_XBUTTON_WPARAM(wparam) == XBUTTON2)
				g_input->set_up(VK_XBUTTON2);
			break;
		case WM_KEYDOWN:
			if ((size_t)wparam < g_input->m_keys.size())
				g_input->set_down(wparam);
			break;
		case WM_KEYUP:
			if ((size_t)wparam < g_input->m_keys.size())
				g_input->set_up(wparam);
			break;
		case WM_SYSKEYDOWN:
			if (wparam == VK_MENU)
				g_input->set_down(VK_MENU);
			break;
		case WM_SYSKEYUP:
			if (wparam == VK_MENU)
				g_input->set_up(VK_MENU);
			break;
		case WM_MOUSEWHEEL:
			g_input->m_scroll = (GET_WHEEL_DELTA_WPARAM(wparam) / WHEEL_DELTA);
			break;
		case WM_CHAR:
			switch (wparam) {
			case VK_BACK:
				if (!g_input->m_buffer.empty())
					g_input->m_buffer.pop_back();
				break;
			case VK_ESCAPE:
			case VK_TAB:
			case VK_RETURN:
				break;
			default:
				if (wparam > 0 && wparam < 0x10000)
					g_input->m_buffer += static_cast<char>(wparam);
				break;
			}
			break;
		default:
			break;
		}

		if (g_menu->m_is_opened) {
			ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
			if (wparam != 'W' && wparam != 'A' && wparam != 'S' && wparam != 'D' && wparam != VK_SHIFT && wparam != VK_CONTROL && wparam != VK_TAB && wparam != VK_SPACE || ui::GetIO().WantTextInput)
				return true;
		}

		return CallWindowProc(old_wnd_proc, hwnd, msg, wparam, lparam);
	}

	long D3DAPI d3d9_present(
		IDirect3DDevice9* const device, RECT* const src_rect,
		RECT* const dst_rect, const HWND dst_wnd_override, RGNDATA* const dirty_region
	) {
		if (g_input->get_key_press(sdk::g_config_system->manual_left_key)) {
			if (hacks::g_anti_aim->manual_side() != 1)
				hacks::g_anti_aim->manual_side() = 1;
			else
				hacks::g_anti_aim->manual_side() = 0;
		}

		if (g_input->get_key_press(sdk::g_config_system->manual_right_key)) {
			if (hacks::g_anti_aim->manual_side() != 2)
				hacks::g_anti_aim->manual_side() = 2;
			else
				hacks::g_anti_aim->manual_side() = 0;
		}

		if (g_input->get_key_press(sdk::g_config_system->manual_back_key)) {
			if (hacks::g_anti_aim->manual_side() != 3)
				hacks::g_anti_aim->manual_side() = 3;
			else
				hacks::g_anti_aim->manual_side() = 0;
		}

		if (g_input->get_key_press(sdk::g_config_system->menu_key))
			g_menu->set_menu_opened(!g_menu->is_menu_opened());

		static bool initialized_renderer = false;

		if (!initialized_renderer)
		{
			initialized_renderer = true;
			sdk::g_config_system->setup_values();
			sdk::g_config_system->refresh();
		}
		else
		{
			g_input->update();
			IDirect3DStateBlock9* state = NULL;
			IDirect3DVertexDeclaration9* vertDec;
			IDirect3DVertexShader9* vertShader;
			device->CreateStateBlock(D3DSBT_PIXELSTATE, &state);
			state->Capture();
			device->GetVertexDeclaration(&vertDec);
			device->GetVertexShader(&vertShader);

			device->SetVertexShader(nullptr);
			device->SetPixelShader(nullptr);
			device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
			device->SetRenderState(D3DRS_LIGHTING, false);
			device->SetRenderState(D3DRS_FOGENABLE, false);
			device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

			device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
			device->SetRenderState(D3DRS_SCISSORTESTENABLE, true);
			device->SetRenderState(D3DRS_ZWRITEENABLE, false);
			device->SetRenderState(D3DRS_STENCILENABLE, false);

			device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, false);
			device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, true);

			device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
			device->SetRenderState(D3DRS_ALPHATESTENABLE, false);
			device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, true);
			device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA);
			device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);

			device->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
			device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);

			device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

			device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

			g_menu->draw_begin(device);
			{
				g_menu->draw();
			}
			g_menu->draw_end();

			state->Apply();
			state->Release();
			device->SetVertexDeclaration(vertDec);
			device->SetVertexShader(vertShader);
		}

		return orig_d3d9_present(device, src_rect, dst_rect, dst_wnd_override, dirty_region);
	}

	void __fastcall paint_traverse(const std::uintptr_t ecx, const std::uintptr_t edx, unsigned int panel, bool force_repaint, bool force) {
		const auto panel_name = valve::g_panel->get_panel_name(panel);

		if (strstr(panel_name, xorstr_("GameConsole"))
			|| strstr(panel_name, xorstr_("CompletionList"))) {
			static bool first_time_drawn = true;

			if (first_time_drawn
				&& strstr(panel_name, xorstr_("GameConsole"))) {
				util::g_notify->print_logo();
				valve::g_cvar->con_print(0xffc0c0c0u, xorstr_("successfully injected\n"));
				util::g_notify->print_logo();
				valve::g_cvar->con_print(0xffc0c0c0u, xorstr_("press 'insert' to open the menu\n"));				
				util::g_notify->print_logo();
				valve::g_cvar->con_print(0xffc0c0c0u, xorstr_("telegram channel - t.me/s1legendirl\n"));
				util::g_notify->print_logo();
				valve::g_cvar->con_print_format(0xffc0c0c0u, xorstr_("last update - %s, %s\n\n\n"), __DATE__, __TIME__);

				first_time_drawn = false;
			}
		
			g_context->console_drawn() = true;
			orig_paint_traverse(ecx, edx, panel, force_repaint, force);
			g_context->console_drawn() = false;
			return;
		}

		orig_paint_traverse(ecx, edx, panel, force_repaint, force);
	}

	void __fastcall paint(const std::uintptr_t ecx, const std::uintptr_t edx, const int mode) {
		orig_paint(ecx, edx, mode);

		if (mode & 1 || mode & 2) {
			valve::g_surface->start_drawing();

			hacks::g_visuals->on_paint();

			valve::g_surface->finish_drawing();
		}
	}

	int __fastcall sv_cheats_get_int(valve::cvar_t* const ecx, const std::uintptr_t edx) {
		if (*reinterpret_cast<std::uintptr_t*>(_AddressOfReturnAddress()) != g_context->addresses().m_cam_sv_cheats_ret)
			return orig_sv_cheats_get_int(ecx, edx);

		return 1;
	}

	void __fastcall draw_model(
		valve::studio_render_t* const ecx, const std::uintptr_t edx,
		const std::uintptr_t a0, const valve::draw_model_info_t& info,
		mat3x4_t* const bones, float* const a3, float* const a4, const vec3_t& origin, int flags
	) {
		if (!hacks::g_visuals->on_draw_model(ecx, edx, a0, info, bones, a3, a4, origin, flags))
			return;

		orig_draw_model(ecx, edx, a0, info, bones, a3, a4, origin, flags);
	}

	void __fastcall render_view(
		valve::view_render_t* const ecx, const std::uintptr_t edx,
		valve::view_setup_t* const setup, valve::view_setup_t* const hud_view_setup,
		int flags, int what_to_draw
	) {
		if (sdk::g_config_system->override_fov) {
			if (valve::g_local_player->scoped()) {
				if (g_context->weapon() && sdk::g_config_system->override_fov_when_scoped) {
					switch (g_context->weapon()->zoom_lvl())
					{
					case 1:
						setup->m_fov = sdk::g_config_system->override_fov_amount;
						break;
					case 2:
						setup->m_fov = sdk::g_config_system->override_fov_amount / 2;
						break;
					default:
						setup->m_fov = sdk::g_config_system->override_fov_amount;
						break;
					}
				}
			}
			else setup->m_fov = sdk::g_config_system->override_fov_amount;
		}

		if (sdk::g_config_system->override_viewmodel_fov)
			setup->m_view_model_fov = sdk::g_config_system->override_viewmodel_fov_amount;

		orig_render_view(ecx, edx, setup, hud_view_setup, flags, what_to_draw);
	}

	void __fastcall lock_cursor(const std::uintptr_t ecx, const std::uintptr_t edx) {
		using unlock_cursor_t = void(__thiscall*)(const std::uintptr_t);

		if (g_menu->m_is_opened)
			return (*reinterpret_cast<unlock_cursor_t**>(ecx))[66u](ecx);

		orig_lock_cursor(ecx, edx);
	}

	void __fastcall on_screen_size_changed(const std::uintptr_t ecx, const std::uintptr_t edx, int old_width, int old_height) {
		orig_on_screen_size_changed(ecx, edx, old_width, old_height);

		hacks::g_visuals->init();
	}

	void __fastcall draw_set_clr(
		valve::c_surface* const ecx, const std::uintptr_t edx, const int r, const int g, const int b, const int a
	) {		
		if (g_context->console_drawn())	{
			int new_r = int(r * 0.3176470588235294f);
			int new_g = int(g * 0.3176470588235294f);
			int new_b = int(b * 0.3176470588235294f);
			int new_a = int(a * 0.8235294117647059f);

			return orig_draw_set_clr(ecx, edx, new_r, new_g, new_b, new_a);
		}

		if (!sdk::g_config_system->remove_scope_overlay)
			return orig_draw_set_clr(ecx, edx, r, g, b, a);

		const auto ret_addr = *reinterpret_cast<std::uintptr_t*>(_AddressOfReturnAddress());
		if (ret_addr == g_context->addresses().m_ret_to_scope_arc
			|| ret_addr == g_context->addresses().m_ret_to_scope_lens)
			return orig_draw_set_clr(ecx, edx, r, g, b, 0);

		if (ret_addr != g_context->addresses().m_ret_to_scope_clear
			&& ret_addr != g_context->addresses().m_ret_to_scope_blurry)
			return orig_draw_set_clr(ecx, edx, r, g, b, a);

		orig_draw_set_clr(ecx, edx, 0, 0, 0, 255);

		const auto& screen_size = ui::GetIO().DisplaySize;
		const auto center = screen_size / 2.f;

		ecx->add_line({ center.x, 0.f }, { center.x, screen_size.y });
		ecx->add_line({ 0.f, center.y }, { screen_size.x, center.y });

		orig_draw_set_clr(ecx, edx, r, g, b, 0);
	}

	__declspec (naked) void __stdcall chl_create_move_proxy(const int seq_number, const float input_sample_frame_time, const bool active) {
		__asm {
			push ebp
			mov ebp, esp
			push ebx
			lea ecx, [esp]
			push ecx
			push dword ptr[ebp + 10h]
			push dword ptr[ebp + 0Ch]
			push dword ptr[ebp + 8]
			call chl_create_move
			pop ebx
			pop ebp
			retn 0Ch
		}
	}

	void __stdcall chl_create_move(const int seq_number, const float input_sample_frame_time, const bool active, bool& send_packet) {
		orig_chl_create_move(valve::g_client, seq_number, input_sample_frame_time, active);

		send_packet = true;

		hacks::g_movement->should_fake_duck() = false;

		if (!valve::g_local_player)
			return;

		if (g_context->should_auto_buy()
			&& valve::g_local_player->alive()
			&& valve::g_local_player->money() >= sdk::g_config_system->auto_buy_minimal_money) {
			std::string buy;
			auto team_num = valve::g_local_player->team();

			if (sdk::g_config_system->primary_weapons[0])
				buy += xorstr_("buy nova; ");

			if (sdk::g_config_system->primary_weapons[1])
				buy += xorstr_("buy xm1014; ");

			if (sdk::g_config_system->primary_weapons[2]) {
				if (team_num == 2)
					buy += xorstr_("buy sawedoff; ");
				else if (team_num == 3)
					buy += xorstr_("buy mag7; ");
			}

			if (sdk::g_config_system->primary_weapons[3])
				buy += xorstr_("buy m249; ");

			if (sdk::g_config_system->primary_weapons[4])
				buy += xorstr_("buy negev; ");

			if (sdk::g_config_system->primary_weapons[5]) {
				if (team_num == 2)
					buy += xorstr_("buy mac10; ");
				else if (team_num == 3)
					buy += xorstr_("buy mp9; ");
			}

			if (sdk::g_config_system->primary_weapons[6])
				buy += xorstr_("buy mp7; ");

			if (sdk::g_config_system->primary_weapons[7])
				buy += xorstr_("buy ump45; ");

			if (sdk::g_config_system->primary_weapons[8])
				buy += xorstr_("buy p90; ");

			if (sdk::g_config_system->primary_weapons[9])
				buy += xorstr_("buy bizon; ");

			if (sdk::g_config_system->primary_weapons[10]) {
				if (team_num == 2)
					buy += xorstr_("buy galilar; ");
				else if (team_num == 3)
					buy += xorstr_("buy famas; ");
			}

			if (sdk::g_config_system->primary_weapons[11]) {
				if (team_num == 2)
					buy += xorstr_("buy ak47; ");
				else if (team_num == 3)
					buy += xorstr_("buy m4a1; ");
			}

			if (sdk::g_config_system->primary_weapons[12])
				buy += xorstr_("buy ssg08; ");

			if (sdk::g_config_system->primary_weapons[13]) {
				if (team_num == 2)
					buy += xorstr_("buy sg556; ");
				else if (team_num == 3)
					buy += xorstr_("buy aug; ");
			}

			if (sdk::g_config_system->primary_weapons[14])
				buy += xorstr_("buy awp; ");

			if (sdk::g_config_system->primary_weapons[15]) {
				if (team_num == 2)
					buy += xorstr_("buy g3sg1; ");
				else if (team_num == 3)
					buy += xorstr_("buy scar20; ");
			}

			if (sdk::g_config_system->secondary_weapons[0])
				buy += xorstr_("buy elite; ");

			if (sdk::g_config_system->secondary_weapons[1])
				buy += xorstr_("buy p250; ");

			if (sdk::g_config_system->secondary_weapons[2]) {
				if (team_num == 2)
					buy += xorstr_("buy tec9; ");
				else if (team_num == 3)
					buy += xorstr_("buy fn57; ");
			}

			if (sdk::g_config_system->secondary_weapons[3])
				buy += xorstr_("buy deagle; ");

			if (sdk::g_config_system->equipment[0])
				buy += xorstr_("buy vest; ");

			if (sdk::g_config_system->equipment[1])
				buy += xorstr_("buy vesthelm; ");

			if (sdk::g_config_system->equipment[2])
				buy += xorstr_("buy taser; ");

			if (sdk::g_config_system->equipment[3] && team_num == 3)
				buy += xorstr_("buy defuser; ");

			if (sdk::g_config_system->equipment[4]) {
				if (team_num == 2)
					buy += xorstr_("buy molotov; ");
				else if (team_num == 3)
					buy += xorstr_("buy incgrenade; ");
			}

			if (sdk::g_config_system->equipment[5])
				buy += xorstr_("buy decoy; ");

			if (sdk::g_config_system->equipment[6])
				buy += xorstr_("buy flashbang; ");

			if (sdk::g_config_system->equipment[7])
				buy += xorstr_("buy flashbang; ");

			if (sdk::g_config_system->equipment[8])
				buy += xorstr_("buy hegrenade; ");

			if (sdk::g_config_system->equipment[9])
				buy += xorstr_("buy smokegrenade; ");

			valve::g_engine->unrestricted_cmd(buy.c_str());
			g_context->should_auto_buy() = false;
		}

		auto& user_cmd = valve::g_input->m_cmds[seq_number % 150];
		if (!user_cmd.m_number)
			return;

		g_context->view_angles() = user_cmd.m_view_angles;

		const auto net_channel_info = valve::g_engine->net_channel_info();
		if (!net_channel_info)
			return;

		auto& net_info = g_context->net_info();

		net_info.m_lerp = std::max(
			g_context->cvars().m_cl_interp->get_float(),
			g_context->cvars().m_cl_interp_ratio->get_float() / g_context->cvars().m_cl_updaterate->get_float()
		);

		net_info.m_latency = { net_channel_info->latency(1), net_channel_info->latency(0) };
		net_info.m_server_tick = valve::g_global_vars->m_tick_count + valve::to_ticks(net_info.m_latency.m_out + net_info.m_latency.m_in);
		net_info.m_on_local_server = !strcmp(net_channel_info->ip(), xorstr_("loopback"));

		hacks::g_visuals->on_pre_create_move(user_cmd);

		const auto backup_team = valve::g_local_player->team();
		if (g_context->switch_lagcompensation_state()) {
			valve::g_engine->exec_cmd(xorstr_("spectate"));
			g_context->cvars().m_cl_lagcompensation->set_int(!g_context->cvars().m_cl_lagcompensation->get_bool());

			valve::g_engine->exec_cmd(std::format("jointeam {} 1", backup_team).data());
			g_context->switch_lagcompensation_state() = false;
		}

		if (!valve::g_local_player->alive())
			return;

		if ((g_context->weapon() = valve::g_local_player->weapon()))
			g_context->wpn_data() = g_context->weapon()->wpn_data();
		else
			g_context->wpn_data() = nullptr;

		// todo: simv0l - make this shit NOT LIKE THIS)0))
		if (g_context->wpn_data()) {
			if (g_context->weapon()->item_index() == valve::e_item_index::elite
				|| g_context->weapon()->item_index() == valve::e_item_index::fiveseven
				|| g_context->weapon()->item_index() == valve::e_item_index::glock
				|| g_context->weapon()->item_index() == valve::e_item_index::p250
				|| g_context->weapon()->item_index() == valve::e_item_index::tec9
				|| g_context->weapon()->item_index() == valve::e_item_index::hkp2000
				|| g_context->weapon()->item_index() == valve::e_item_index::usp_silencer
				|| g_context->weapon()->item_index() == valve::e_item_index::cz75a) {
				if (!g_menu->m_is_opened)
					sdk::g_config_system->weapon_selection = 0;
				sdk::g_config_system->target_hitbox[0] = sdk::g_config_system->pistols_target_hitbox[0];
				sdk::g_config_system->target_hitbox[1] = sdk::g_config_system->pistols_target_hitbox[1];
				sdk::g_config_system->target_hitbox[2] = sdk::g_config_system->pistols_target_hitbox[2];
				sdk::g_config_system->target_hitbox[3] = sdk::g_config_system->pistols_target_hitbox[3];
				sdk::g_config_system->target_hitbox[4] = sdk::g_config_system->pistols_target_hitbox[4];
				sdk::g_config_system->target_hitbox[5] = sdk::g_config_system->pistols_target_hitbox[5];
				sdk::g_config_system->multi_point[0] = sdk::g_config_system->pistols_multi_point[0];
				sdk::g_config_system->multi_point[1] = sdk::g_config_system->pistols_multi_point[1];
				sdk::g_config_system->multi_point[2] = sdk::g_config_system->pistols_multi_point[2];
				sdk::g_config_system->multi_point[3] = sdk::g_config_system->pistols_multi_point[3];
				sdk::g_config_system->multi_point[4] = sdk::g_config_system->pistols_multi_point[4];
				sdk::g_config_system->multi_point[5] = sdk::g_config_system->pistols_multi_point[5];
				sdk::g_config_system->static_scale = sdk::g_config_system->pistols_static_scale;
				sdk::g_config_system->head_scale = sdk::g_config_system->pistols_head_scale;
				sdk::g_config_system->body_scale = sdk::g_config_system->pistols_body_scale;
				sdk::g_config_system->automatic_fire = sdk::g_config_system->pistols_automatic_fire;
				sdk::g_config_system->silent_aim = sdk::g_config_system->pistols_silent_aim;
				sdk::g_config_system->minimum_hitchance = sdk::g_config_system->pistols_minimum_hitchance;
				sdk::g_config_system->minimum_damage = sdk::g_config_system->pistols_minimum_damage;
				sdk::g_config_system->automatic_penetration = sdk::g_config_system->pistols_automatic_penetration;
				sdk::g_config_system->penetration_minimum_damage = sdk::g_config_system->pistols_penetration_minimum_damage;
				sdk::g_config_system->override_minimum_damage = sdk::g_config_system->pistols_override_minimum_damage;
				sdk::g_config_system->automatic_scope = sdk::g_config_system->pistols_automatic_scope;
				sdk::g_config_system->maximum_fov = sdk::g_config_system->pistols_maximum_fov;
				sdk::g_config_system->remove_recoil = sdk::g_config_system->pistols_remove_recoil;
				sdk::g_config_system->accuracy_boost = sdk::g_config_system->pistols_accuracy_boost;
				sdk::g_config_system->quick_stop = sdk::g_config_system->pistols_quick_stop;
				sdk::g_config_system->type = sdk::g_config_system->pistols_type;
				sdk::g_config_system->between_shots = sdk::g_config_system->pistols_between_shots;
				sdk::g_config_system->lag_compensation = sdk::g_config_system->pistols_lag_compensation;
				sdk::g_config_system->body_aim_conditions[0] = sdk::g_config_system->pistols_body_aim_conditions[0];
				sdk::g_config_system->body_aim_conditions[1] = sdk::g_config_system->pistols_body_aim_conditions[1];
				sdk::g_config_system->body_aim_conditions[2] = sdk::g_config_system->pistols_body_aim_conditions[2];
				sdk::g_config_system->body_aim_conditions[3] = sdk::g_config_system->pistols_body_aim_conditions[3];
				sdk::g_config_system->body_aim_conditions[4] = sdk::g_config_system->pistols_body_aim_conditions[4];
				sdk::g_config_system->body_aim_conditions[5] = sdk::g_config_system->pistols_body_aim_conditions[5];
				sdk::g_config_system->body_aim_conditions[6] = sdk::g_config_system->pistols_body_aim_conditions[6];
				sdk::g_config_system->body_aim_conditions[7] = sdk::g_config_system->pistols_body_aim_conditions[7];
				sdk::g_config_system->body_aim_conditions[8] = sdk::g_config_system->pistols_body_aim_conditions[8];
				sdk::g_config_system->max_misses_body_aim = sdk::g_config_system->pistols_max_misses_body_aim;
				sdk::g_config_system->safe_point_conditions[0] = sdk::g_config_system->pistols_safe_point_conditions[0];
				sdk::g_config_system->safe_point_conditions[1] = sdk::g_config_system->pistols_safe_point_conditions[1];
				sdk::g_config_system->safe_point_conditions[2] = sdk::g_config_system->pistols_safe_point_conditions[2];
				sdk::g_config_system->safe_point_conditions[3] = sdk::g_config_system->pistols_safe_point_conditions[3];
				sdk::g_config_system->safe_point_conditions[4] = sdk::g_config_system->pistols_safe_point_conditions[4];
				sdk::g_config_system->safe_point_conditions[5] = sdk::g_config_system->pistols_safe_point_conditions[5];
				sdk::g_config_system->safe_point_conditions[6] = sdk::g_config_system->pistols_safe_point_conditions[6];
				sdk::g_config_system->safe_point_conditions[7] = sdk::g_config_system->pistols_safe_point_conditions[7];
				sdk::g_config_system->safe_point_type = sdk::g_config_system->pistols_safe_point_type;
				sdk::g_config_system->max_misses_safe_point = sdk::g_config_system->pistols_max_misses_safe_point;
				sdk::g_config_system->double_tap = sdk::g_config_system->pistols_double_tap;
				sdk::g_config_system->max_shift_amount = sdk::g_config_system->pistols_max_shift_amount;
			}
			else if (g_context->weapon()->item_index() == valve::e_item_index::deagle
				|| g_context->weapon()->item_index() == valve::e_item_index::revolver) {
				if (!g_menu->m_is_opened)
					sdk::g_config_system->weapon_selection = 1;
				sdk::g_config_system->target_hitbox[0] = sdk::g_config_system->heavy_pistols_target_hitbox[0];
				sdk::g_config_system->target_hitbox[1] = sdk::g_config_system->heavy_pistols_target_hitbox[1];
				sdk::g_config_system->target_hitbox[2] = sdk::g_config_system->heavy_pistols_target_hitbox[2];
				sdk::g_config_system->target_hitbox[3] = sdk::g_config_system->heavy_pistols_target_hitbox[3];
				sdk::g_config_system->target_hitbox[4] = sdk::g_config_system->heavy_pistols_target_hitbox[4];
				sdk::g_config_system->target_hitbox[5] = sdk::g_config_system->heavy_pistols_target_hitbox[5];
				sdk::g_config_system->multi_point[0] = sdk::g_config_system->heavy_pistols_multi_point[0];
				sdk::g_config_system->multi_point[1] = sdk::g_config_system->heavy_pistols_multi_point[1];
				sdk::g_config_system->multi_point[2] = sdk::g_config_system->heavy_pistols_multi_point[2];
				sdk::g_config_system->multi_point[3] = sdk::g_config_system->heavy_pistols_multi_point[3];
				sdk::g_config_system->multi_point[4] = sdk::g_config_system->heavy_pistols_multi_point[4];
				sdk::g_config_system->multi_point[5] = sdk::g_config_system->heavy_pistols_multi_point[5];
				sdk::g_config_system->static_scale = sdk::g_config_system->heavy_pistols_static_scale;
				sdk::g_config_system->head_scale = sdk::g_config_system->heavy_pistols_head_scale;
				sdk::g_config_system->body_scale = sdk::g_config_system->heavy_pistols_body_scale;
				sdk::g_config_system->automatic_fire = sdk::g_config_system->heavy_pistols_automatic_fire;
				sdk::g_config_system->silent_aim = sdk::g_config_system->heavy_pistols_silent_aim;
				sdk::g_config_system->minimum_hitchance = sdk::g_config_system->heavy_pistols_minimum_hitchance;
				sdk::g_config_system->minimum_damage = sdk::g_config_system->heavy_pistols_minimum_damage;
				sdk::g_config_system->automatic_penetration = sdk::g_config_system->heavy_pistols_automatic_penetration;
				sdk::g_config_system->penetration_minimum_damage = sdk::g_config_system->heavy_pistols_penetration_minimum_damage;
				sdk::g_config_system->override_minimum_damage = sdk::g_config_system->heavy_pistols_override_minimum_damage;
				sdk::g_config_system->automatic_scope = sdk::g_config_system->heavy_pistols_automatic_scope;
				sdk::g_config_system->maximum_fov = sdk::g_config_system->heavy_pistols_maximum_fov;
				sdk::g_config_system->remove_recoil = sdk::g_config_system->heavy_pistols_remove_recoil;
				sdk::g_config_system->accuracy_boost = sdk::g_config_system->heavy_pistols_accuracy_boost;
				sdk::g_config_system->quick_stop = sdk::g_config_system->heavy_pistols_quick_stop;
				sdk::g_config_system->type = sdk::g_config_system->heavy_pistols_type;
				sdk::g_config_system->between_shots = sdk::g_config_system->heavy_pistols_between_shots;
				sdk::g_config_system->lag_compensation = sdk::g_config_system->heavy_pistols_lag_compensation;
				sdk::g_config_system->body_aim_conditions[0] = sdk::g_config_system->heavy_pistols_body_aim_conditions[0];
				sdk::g_config_system->body_aim_conditions[1] = sdk::g_config_system->heavy_pistols_body_aim_conditions[1];
				sdk::g_config_system->body_aim_conditions[2] = sdk::g_config_system->heavy_pistols_body_aim_conditions[2];
				sdk::g_config_system->body_aim_conditions[3] = sdk::g_config_system->heavy_pistols_body_aim_conditions[3];
				sdk::g_config_system->body_aim_conditions[4] = sdk::g_config_system->heavy_pistols_body_aim_conditions[4];
				sdk::g_config_system->body_aim_conditions[5] = sdk::g_config_system->heavy_pistols_body_aim_conditions[5];
				sdk::g_config_system->body_aim_conditions[6] = sdk::g_config_system->heavy_pistols_body_aim_conditions[6];
				sdk::g_config_system->body_aim_conditions[7] = sdk::g_config_system->heavy_pistols_body_aim_conditions[7];
				sdk::g_config_system->body_aim_conditions[8] = sdk::g_config_system->heavy_pistols_body_aim_conditions[8];
				sdk::g_config_system->max_misses_body_aim = sdk::g_config_system->heavy_pistols_max_misses_body_aim;
				sdk::g_config_system->safe_point_conditions[0] = sdk::g_config_system->heavy_pistols_safe_point_conditions[0];
				sdk::g_config_system->safe_point_conditions[1] = sdk::g_config_system->heavy_pistols_safe_point_conditions[1];
				sdk::g_config_system->safe_point_conditions[2] = sdk::g_config_system->heavy_pistols_safe_point_conditions[2];
				sdk::g_config_system->safe_point_conditions[3] = sdk::g_config_system->heavy_pistols_safe_point_conditions[3];
				sdk::g_config_system->safe_point_conditions[4] = sdk::g_config_system->heavy_pistols_safe_point_conditions[4];
				sdk::g_config_system->safe_point_conditions[5] = sdk::g_config_system->heavy_pistols_safe_point_conditions[5];
				sdk::g_config_system->safe_point_conditions[6] = sdk::g_config_system->heavy_pistols_safe_point_conditions[6];
				sdk::g_config_system->safe_point_conditions[7] = sdk::g_config_system->heavy_pistols_safe_point_conditions[7];
				sdk::g_config_system->safe_point_type = sdk::g_config_system->heavy_pistols_safe_point_type;
				sdk::g_config_system->max_misses_safe_point = sdk::g_config_system->heavy_pistols_max_misses_safe_point;
				sdk::g_config_system->double_tap = sdk::g_config_system->heavy_pistols_double_tap;
				sdk::g_config_system->max_shift_amount = sdk::g_config_system->heavy_pistols_max_shift_amount;
			}
			else if (g_context->weapon()->item_index() == valve::e_item_index::mac10
				|| g_context->weapon()->item_index() == valve::e_item_index::p90
				|| g_context->weapon()->item_index() == valve::e_item_index::mp5sd
				|| g_context->weapon()->item_index() == valve::e_item_index::ump45
				|| g_context->weapon()->item_index() == valve::e_item_index::bizon
				|| g_context->weapon()->item_index() == valve::e_item_index::mp7
				|| g_context->weapon()->item_index() == valve::e_item_index::mp9) {
				if (!g_menu->m_is_opened)
					sdk::g_config_system->weapon_selection = 2;
				sdk::g_config_system->target_hitbox[0] = sdk::g_config_system->sub_machine_guns_target_hitbox[0];
				sdk::g_config_system->target_hitbox[1] = sdk::g_config_system->sub_machine_guns_target_hitbox[1];
				sdk::g_config_system->target_hitbox[2] = sdk::g_config_system->sub_machine_guns_target_hitbox[2];
				sdk::g_config_system->target_hitbox[3] = sdk::g_config_system->sub_machine_guns_target_hitbox[3];
				sdk::g_config_system->target_hitbox[4] = sdk::g_config_system->sub_machine_guns_target_hitbox[4];
				sdk::g_config_system->target_hitbox[5] = sdk::g_config_system->sub_machine_guns_target_hitbox[5];
				sdk::g_config_system->multi_point[0] = sdk::g_config_system->sub_machine_guns_multi_point[0];
				sdk::g_config_system->multi_point[1] = sdk::g_config_system->sub_machine_guns_multi_point[1];
				sdk::g_config_system->multi_point[2] = sdk::g_config_system->sub_machine_guns_multi_point[2];
				sdk::g_config_system->multi_point[3] = sdk::g_config_system->sub_machine_guns_multi_point[3];
				sdk::g_config_system->multi_point[4] = sdk::g_config_system->sub_machine_guns_multi_point[4];
				sdk::g_config_system->multi_point[5] = sdk::g_config_system->sub_machine_guns_multi_point[5];
				sdk::g_config_system->static_scale = sdk::g_config_system->sub_machine_guns_static_scale;
				sdk::g_config_system->head_scale = sdk::g_config_system->sub_machine_guns_head_scale;
				sdk::g_config_system->body_scale = sdk::g_config_system->sub_machine_guns_body_scale;
				sdk::g_config_system->automatic_fire = sdk::g_config_system->sub_machine_guns_automatic_fire;
				sdk::g_config_system->silent_aim = sdk::g_config_system->sub_machine_guns_silent_aim;
				sdk::g_config_system->minimum_hitchance = sdk::g_config_system->sub_machine_guns_minimum_hitchance;
				sdk::g_config_system->minimum_damage = sdk::g_config_system->sub_machine_guns_minimum_damage;
				sdk::g_config_system->automatic_penetration = sdk::g_config_system->sub_machine_guns_automatic_penetration;
				sdk::g_config_system->penetration_minimum_damage = sdk::g_config_system->sub_machine_guns_penetration_minimum_damage;
				sdk::g_config_system->override_minimum_damage = sdk::g_config_system->sub_machine_guns_override_minimum_damage;
				sdk::g_config_system->automatic_scope = sdk::g_config_system->sub_machine_guns_automatic_scope;
				sdk::g_config_system->maximum_fov = sdk::g_config_system->sub_machine_guns_maximum_fov;
				sdk::g_config_system->remove_recoil = sdk::g_config_system->sub_machine_guns_remove_recoil;
				sdk::g_config_system->accuracy_boost = sdk::g_config_system->sub_machine_guns_accuracy_boost;
				sdk::g_config_system->quick_stop = sdk::g_config_system->sub_machine_guns_quick_stop;
				sdk::g_config_system->type = sdk::g_config_system->sub_machine_guns_type;
				sdk::g_config_system->between_shots = sdk::g_config_system->sub_machine_guns_between_shots;
				sdk::g_config_system->lag_compensation = sdk::g_config_system->sub_machine_guns_lag_compensation;
				sdk::g_config_system->body_aim_conditions[0] = sdk::g_config_system->sub_machine_guns_body_aim_conditions[0];
				sdk::g_config_system->body_aim_conditions[1] = sdk::g_config_system->sub_machine_guns_body_aim_conditions[1];
				sdk::g_config_system->body_aim_conditions[2] = sdk::g_config_system->sub_machine_guns_body_aim_conditions[2];
				sdk::g_config_system->body_aim_conditions[3] = sdk::g_config_system->sub_machine_guns_body_aim_conditions[3];
				sdk::g_config_system->body_aim_conditions[4] = sdk::g_config_system->sub_machine_guns_body_aim_conditions[4];
				sdk::g_config_system->body_aim_conditions[5] = sdk::g_config_system->sub_machine_guns_body_aim_conditions[5];
				sdk::g_config_system->body_aim_conditions[6] = sdk::g_config_system->sub_machine_guns_body_aim_conditions[6];
				sdk::g_config_system->body_aim_conditions[7] = sdk::g_config_system->sub_machine_guns_body_aim_conditions[7];
				sdk::g_config_system->body_aim_conditions[8] = sdk::g_config_system->sub_machine_guns_body_aim_conditions[8];
				sdk::g_config_system->max_misses_body_aim = sdk::g_config_system->sub_machine_guns_max_misses_body_aim;
				sdk::g_config_system->safe_point_conditions[0] = sdk::g_config_system->sub_machine_guns_safe_point_conditions[0];
				sdk::g_config_system->safe_point_conditions[1] = sdk::g_config_system->sub_machine_guns_safe_point_conditions[1];
				sdk::g_config_system->safe_point_conditions[2] = sdk::g_config_system->sub_machine_guns_safe_point_conditions[2];
				sdk::g_config_system->safe_point_conditions[3] = sdk::g_config_system->sub_machine_guns_safe_point_conditions[3];
				sdk::g_config_system->safe_point_conditions[4] = sdk::g_config_system->sub_machine_guns_safe_point_conditions[4];
				sdk::g_config_system->safe_point_conditions[5] = sdk::g_config_system->sub_machine_guns_safe_point_conditions[5];
				sdk::g_config_system->safe_point_conditions[6] = sdk::g_config_system->sub_machine_guns_safe_point_conditions[6];
				sdk::g_config_system->safe_point_conditions[7] = sdk::g_config_system->sub_machine_guns_safe_point_conditions[7];
				sdk::g_config_system->safe_point_type = sdk::g_config_system->sub_machine_guns_safe_point_type;
				sdk::g_config_system->max_misses_safe_point = sdk::g_config_system->sub_machine_guns_max_misses_safe_point;
				sdk::g_config_system->double_tap = sdk::g_config_system->sub_machine_guns_double_tap;
				sdk::g_config_system->max_shift_amount = sdk::g_config_system->sub_machine_guns_max_shift_amount;
			}
			else if (g_context->weapon()->item_index() == valve::e_item_index::ak47
				|| g_context->weapon()->item_index() == valve::e_item_index::aug
				|| g_context->weapon()->item_index() == valve::e_item_index::famas
				|| g_context->weapon()->item_index() == valve::e_item_index::galil
				|| g_context->weapon()->item_index() == valve::e_item_index::m4a1
				|| g_context->weapon()->item_index() == valve::e_item_index::sg553
				|| g_context->weapon()->item_index() == valve::e_item_index::m4a1_silencer) {
				if (!g_menu->m_is_opened)
					sdk::g_config_system->weapon_selection = 3;
				sdk::g_config_system->target_hitbox[0] = sdk::g_config_system->rifles_target_hitbox[0];
				sdk::g_config_system->target_hitbox[1] = sdk::g_config_system->rifles_target_hitbox[1];
				sdk::g_config_system->target_hitbox[2] = sdk::g_config_system->rifles_target_hitbox[2];
				sdk::g_config_system->target_hitbox[3] = sdk::g_config_system->rifles_target_hitbox[3];
				sdk::g_config_system->target_hitbox[4] = sdk::g_config_system->rifles_target_hitbox[4];
				sdk::g_config_system->target_hitbox[5] = sdk::g_config_system->rifles_target_hitbox[5];
				sdk::g_config_system->multi_point[0] = sdk::g_config_system->rifles_multi_point[0];
				sdk::g_config_system->multi_point[1] = sdk::g_config_system->rifles_multi_point[1];
				sdk::g_config_system->multi_point[2] = sdk::g_config_system->rifles_multi_point[2];
				sdk::g_config_system->multi_point[3] = sdk::g_config_system->rifles_multi_point[3];
				sdk::g_config_system->multi_point[4] = sdk::g_config_system->rifles_multi_point[4];
				sdk::g_config_system->multi_point[5] = sdk::g_config_system->rifles_multi_point[5];
				sdk::g_config_system->static_scale = sdk::g_config_system->rifles_static_scale;
				sdk::g_config_system->head_scale = sdk::g_config_system->rifles_head_scale;
				sdk::g_config_system->body_scale = sdk::g_config_system->rifles_body_scale;
				sdk::g_config_system->automatic_fire = sdk::g_config_system->rifles_automatic_fire;
				sdk::g_config_system->silent_aim = sdk::g_config_system->rifles_silent_aim;
				sdk::g_config_system->minimum_hitchance = sdk::g_config_system->rifles_minimum_hitchance;
				sdk::g_config_system->minimum_damage = sdk::g_config_system->rifles_minimum_damage;
				sdk::g_config_system->automatic_penetration = sdk::g_config_system->rifles_automatic_penetration;
				sdk::g_config_system->penetration_minimum_damage = sdk::g_config_system->rifles_penetration_minimum_damage;
				sdk::g_config_system->override_minimum_damage = sdk::g_config_system->rifles_override_minimum_damage;
				sdk::g_config_system->automatic_scope = sdk::g_config_system->rifles_automatic_scope;
				sdk::g_config_system->maximum_fov = sdk::g_config_system->rifles_maximum_fov;
				sdk::g_config_system->remove_recoil = sdk::g_config_system->rifles_remove_recoil;
				sdk::g_config_system->accuracy_boost = sdk::g_config_system->rifles_accuracy_boost;
				sdk::g_config_system->quick_stop = sdk::g_config_system->rifles_quick_stop;
				sdk::g_config_system->type = sdk::g_config_system->rifles_type;
				sdk::g_config_system->between_shots = sdk::g_config_system->rifles_between_shots;
				sdk::g_config_system->lag_compensation = sdk::g_config_system->rifles_lag_compensation;
				sdk::g_config_system->body_aim_conditions[0] = sdk::g_config_system->rifles_body_aim_conditions[0];
				sdk::g_config_system->body_aim_conditions[1] = sdk::g_config_system->rifles_body_aim_conditions[1];
				sdk::g_config_system->body_aim_conditions[2] = sdk::g_config_system->rifles_body_aim_conditions[2];
				sdk::g_config_system->body_aim_conditions[3] = sdk::g_config_system->rifles_body_aim_conditions[3];
				sdk::g_config_system->body_aim_conditions[4] = sdk::g_config_system->rifles_body_aim_conditions[4];
				sdk::g_config_system->body_aim_conditions[5] = sdk::g_config_system->rifles_body_aim_conditions[5];
				sdk::g_config_system->body_aim_conditions[6] = sdk::g_config_system->rifles_body_aim_conditions[6];
				sdk::g_config_system->body_aim_conditions[7] = sdk::g_config_system->rifles_body_aim_conditions[7];
				sdk::g_config_system->body_aim_conditions[8] = sdk::g_config_system->rifles_body_aim_conditions[8];
				sdk::g_config_system->max_misses_body_aim = sdk::g_config_system->rifles_max_misses_body_aim;
				sdk::g_config_system->safe_point_conditions[0] = sdk::g_config_system->rifles_safe_point_conditions[0];
				sdk::g_config_system->safe_point_conditions[1] = sdk::g_config_system->rifles_safe_point_conditions[1];
				sdk::g_config_system->safe_point_conditions[2] = sdk::g_config_system->rifles_safe_point_conditions[2];
				sdk::g_config_system->safe_point_conditions[3] = sdk::g_config_system->rifles_safe_point_conditions[3];
				sdk::g_config_system->safe_point_conditions[4] = sdk::g_config_system->rifles_safe_point_conditions[4];
				sdk::g_config_system->safe_point_conditions[5] = sdk::g_config_system->rifles_safe_point_conditions[5];
				sdk::g_config_system->safe_point_conditions[6] = sdk::g_config_system->rifles_safe_point_conditions[6];
				sdk::g_config_system->safe_point_conditions[7] = sdk::g_config_system->rifles_safe_point_conditions[7];
				sdk::g_config_system->safe_point_type = sdk::g_config_system->rifles_safe_point_type;
				sdk::g_config_system->max_misses_safe_point = sdk::g_config_system->rifles_max_misses_safe_point;
				sdk::g_config_system->double_tap = sdk::g_config_system->rifles_double_tap;
				sdk::g_config_system->max_shift_amount = sdk::g_config_system->rifles_max_shift_amount;
			}
			else if (g_context->weapon()->item_index() == valve::e_item_index::g3sg1
				|| g_context->weapon()->item_index() == valve::e_item_index::scar20) {
				if (!g_menu->m_is_opened)
					sdk::g_config_system->weapon_selection = 4;
				sdk::g_config_system->target_hitbox[0] = sdk::g_config_system->auto_snipers_target_hitbox[0];
				sdk::g_config_system->target_hitbox[1] = sdk::g_config_system->auto_snipers_target_hitbox[1];
				sdk::g_config_system->target_hitbox[2] = sdk::g_config_system->auto_snipers_target_hitbox[2];
				sdk::g_config_system->target_hitbox[3] = sdk::g_config_system->auto_snipers_target_hitbox[3];
				sdk::g_config_system->target_hitbox[4] = sdk::g_config_system->auto_snipers_target_hitbox[4];
				sdk::g_config_system->target_hitbox[5] = sdk::g_config_system->auto_snipers_target_hitbox[5];
				sdk::g_config_system->multi_point[0] = sdk::g_config_system->auto_snipers_multi_point[0];
				sdk::g_config_system->multi_point[1] = sdk::g_config_system->auto_snipers_multi_point[1];
				sdk::g_config_system->multi_point[2] = sdk::g_config_system->auto_snipers_multi_point[2];
				sdk::g_config_system->multi_point[3] = sdk::g_config_system->auto_snipers_multi_point[3];
				sdk::g_config_system->multi_point[4] = sdk::g_config_system->auto_snipers_multi_point[4];
				sdk::g_config_system->multi_point[5] = sdk::g_config_system->auto_snipers_multi_point[5];
				sdk::g_config_system->static_scale = sdk::g_config_system->auto_snipers_static_scale;
				sdk::g_config_system->head_scale = sdk::g_config_system->auto_snipers_head_scale;
				sdk::g_config_system->body_scale = sdk::g_config_system->auto_snipers_body_scale;
				sdk::g_config_system->automatic_fire = sdk::g_config_system->auto_snipers_automatic_fire;
				sdk::g_config_system->silent_aim = sdk::g_config_system->auto_snipers_silent_aim;
				sdk::g_config_system->minimum_hitchance = sdk::g_config_system->auto_snipers_minimum_hitchance;
				sdk::g_config_system->minimum_damage = sdk::g_config_system->auto_snipers_minimum_damage;
				sdk::g_config_system->automatic_penetration = sdk::g_config_system->auto_snipers_automatic_penetration;
				sdk::g_config_system->penetration_minimum_damage = sdk::g_config_system->auto_snipers_penetration_minimum_damage;
				sdk::g_config_system->override_minimum_damage = sdk::g_config_system->auto_snipers_override_minimum_damage;
				sdk::g_config_system->automatic_scope = sdk::g_config_system->auto_snipers_automatic_scope;
				sdk::g_config_system->maximum_fov = sdk::g_config_system->auto_snipers_maximum_fov;
				sdk::g_config_system->remove_recoil = sdk::g_config_system->auto_snipers_remove_recoil;
				sdk::g_config_system->accuracy_boost = sdk::g_config_system->auto_snipers_accuracy_boost;
				sdk::g_config_system->quick_stop = sdk::g_config_system->auto_snipers_quick_stop;
				sdk::g_config_system->type = sdk::g_config_system->auto_snipers_type;
				sdk::g_config_system->between_shots = sdk::g_config_system->auto_snipers_between_shots;
				sdk::g_config_system->lag_compensation = sdk::g_config_system->auto_snipers_lag_compensation;
				sdk::g_config_system->body_aim_conditions[0] = sdk::g_config_system->auto_snipers_body_aim_conditions[0];
				sdk::g_config_system->body_aim_conditions[1] = sdk::g_config_system->auto_snipers_body_aim_conditions[1];
				sdk::g_config_system->body_aim_conditions[2] = sdk::g_config_system->auto_snipers_body_aim_conditions[2];
				sdk::g_config_system->body_aim_conditions[3] = sdk::g_config_system->auto_snipers_body_aim_conditions[3];
				sdk::g_config_system->body_aim_conditions[4] = sdk::g_config_system->auto_snipers_body_aim_conditions[4];
				sdk::g_config_system->body_aim_conditions[5] = sdk::g_config_system->auto_snipers_body_aim_conditions[5];
				sdk::g_config_system->body_aim_conditions[6] = sdk::g_config_system->auto_snipers_body_aim_conditions[6];
				sdk::g_config_system->body_aim_conditions[7] = sdk::g_config_system->auto_snipers_body_aim_conditions[7];
				sdk::g_config_system->body_aim_conditions[8] = sdk::g_config_system->auto_snipers_body_aim_conditions[8];
				sdk::g_config_system->max_misses_body_aim = sdk::g_config_system->auto_snipers_max_misses_body_aim;
				sdk::g_config_system->safe_point_conditions[0] = sdk::g_config_system->auto_snipers_safe_point_conditions[0];
				sdk::g_config_system->safe_point_conditions[1] = sdk::g_config_system->auto_snipers_safe_point_conditions[1];
				sdk::g_config_system->safe_point_conditions[2] = sdk::g_config_system->auto_snipers_safe_point_conditions[2];
				sdk::g_config_system->safe_point_conditions[3] = sdk::g_config_system->auto_snipers_safe_point_conditions[3];
				sdk::g_config_system->safe_point_conditions[4] = sdk::g_config_system->auto_snipers_safe_point_conditions[4];
				sdk::g_config_system->safe_point_conditions[5] = sdk::g_config_system->auto_snipers_safe_point_conditions[5];
				sdk::g_config_system->safe_point_conditions[6] = sdk::g_config_system->auto_snipers_safe_point_conditions[6];
				sdk::g_config_system->safe_point_conditions[7] = sdk::g_config_system->auto_snipers_safe_point_conditions[7];
				sdk::g_config_system->safe_point_type = sdk::g_config_system->auto_snipers_safe_point_type;
				sdk::g_config_system->max_misses_safe_point = sdk::g_config_system->auto_snipers_max_misses_safe_point;
				sdk::g_config_system->double_tap = sdk::g_config_system->auto_snipers_double_tap;
				sdk::g_config_system->max_shift_amount = sdk::g_config_system->auto_snipers_max_shift_amount;
			}
			else if (g_context->weapon()->item_index() == valve::e_item_index::ssg08) {
				if (!g_menu->m_is_opened)
					sdk::g_config_system->weapon_selection = 5;
				sdk::g_config_system->target_hitbox[0] = sdk::g_config_system->scout_target_hitbox[0];
				sdk::g_config_system->target_hitbox[1] = sdk::g_config_system->scout_target_hitbox[1];
				sdk::g_config_system->target_hitbox[2] = sdk::g_config_system->scout_target_hitbox[2];
				sdk::g_config_system->target_hitbox[3] = sdk::g_config_system->scout_target_hitbox[3];
				sdk::g_config_system->target_hitbox[4] = sdk::g_config_system->scout_target_hitbox[4];
				sdk::g_config_system->target_hitbox[5] = sdk::g_config_system->scout_target_hitbox[5];
				sdk::g_config_system->multi_point[0] = sdk::g_config_system->scout_multi_point[0];
				sdk::g_config_system->multi_point[1] = sdk::g_config_system->scout_multi_point[1];
				sdk::g_config_system->multi_point[2] = sdk::g_config_system->scout_multi_point[2];
				sdk::g_config_system->multi_point[3] = sdk::g_config_system->scout_multi_point[3];
				sdk::g_config_system->multi_point[4] = sdk::g_config_system->scout_multi_point[4];
				sdk::g_config_system->multi_point[5] = sdk::g_config_system->scout_multi_point[5];
				sdk::g_config_system->static_scale = sdk::g_config_system->scout_static_scale;
				sdk::g_config_system->head_scale = sdk::g_config_system->scout_head_scale;
				sdk::g_config_system->body_scale = sdk::g_config_system->scout_body_scale;
				sdk::g_config_system->automatic_fire = sdk::g_config_system->scout_automatic_fire;
				sdk::g_config_system->silent_aim = sdk::g_config_system->scout_silent_aim;
				sdk::g_config_system->minimum_hitchance = sdk::g_config_system->scout_minimum_hitchance;
				sdk::g_config_system->minimum_damage = sdk::g_config_system->scout_minimum_damage;
				sdk::g_config_system->automatic_penetration = sdk::g_config_system->scout_automatic_penetration;
				sdk::g_config_system->penetration_minimum_damage = sdk::g_config_system->scout_penetration_minimum_damage;
				sdk::g_config_system->override_minimum_damage = sdk::g_config_system->scout_override_minimum_damage;
				sdk::g_config_system->automatic_scope = sdk::g_config_system->scout_automatic_scope;
				sdk::g_config_system->maximum_fov = sdk::g_config_system->scout_maximum_fov;
				sdk::g_config_system->remove_recoil = sdk::g_config_system->scout_remove_recoil;
				sdk::g_config_system->accuracy_boost = sdk::g_config_system->scout_accuracy_boost;
				sdk::g_config_system->quick_stop = sdk::g_config_system->scout_quick_stop;
				sdk::g_config_system->type = sdk::g_config_system->scout_type;
				sdk::g_config_system->between_shots = sdk::g_config_system->scout_between_shots;
				sdk::g_config_system->lag_compensation = sdk::g_config_system->scout_lag_compensation;
				sdk::g_config_system->body_aim_conditions[0] = sdk::g_config_system->scout_body_aim_conditions[0];
				sdk::g_config_system->body_aim_conditions[1] = sdk::g_config_system->scout_body_aim_conditions[1];
				sdk::g_config_system->body_aim_conditions[2] = sdk::g_config_system->scout_body_aim_conditions[2];
				sdk::g_config_system->body_aim_conditions[3] = sdk::g_config_system->scout_body_aim_conditions[3];
				sdk::g_config_system->body_aim_conditions[4] = sdk::g_config_system->scout_body_aim_conditions[4];
				sdk::g_config_system->body_aim_conditions[5] = sdk::g_config_system->scout_body_aim_conditions[5];
				sdk::g_config_system->body_aim_conditions[6] = sdk::g_config_system->scout_body_aim_conditions[6];
				sdk::g_config_system->body_aim_conditions[7] = sdk::g_config_system->scout_body_aim_conditions[7];
				sdk::g_config_system->body_aim_conditions[8] = sdk::g_config_system->scout_body_aim_conditions[8];
				sdk::g_config_system->max_misses_body_aim = sdk::g_config_system->scout_max_misses_body_aim;
				sdk::g_config_system->safe_point_conditions[0] = sdk::g_config_system->scout_safe_point_conditions[0];
				sdk::g_config_system->safe_point_conditions[1] = sdk::g_config_system->scout_safe_point_conditions[1];
				sdk::g_config_system->safe_point_conditions[2] = sdk::g_config_system->scout_safe_point_conditions[2];
				sdk::g_config_system->safe_point_conditions[3] = sdk::g_config_system->scout_safe_point_conditions[3];
				sdk::g_config_system->safe_point_conditions[4] = sdk::g_config_system->scout_safe_point_conditions[4];
				sdk::g_config_system->safe_point_conditions[5] = sdk::g_config_system->scout_safe_point_conditions[5];
				sdk::g_config_system->safe_point_conditions[6] = sdk::g_config_system->scout_safe_point_conditions[6];
				sdk::g_config_system->safe_point_conditions[7] = sdk::g_config_system->scout_safe_point_conditions[7];
				sdk::g_config_system->safe_point_type = sdk::g_config_system->scout_safe_point_type;
				sdk::g_config_system->max_misses_safe_point = sdk::g_config_system->scout_max_misses_safe_point;
				sdk::g_config_system->double_tap = sdk::g_config_system->scout_double_tap;
				sdk::g_config_system->max_shift_amount = sdk::g_config_system->scout_max_shift_amount;
			}
			else if (g_context->weapon()->item_index() == valve::e_item_index::awp) {
				if (!g_menu->m_is_opened)
					sdk::g_config_system->weapon_selection = 6;
				sdk::g_config_system->target_hitbox[0] = sdk::g_config_system->awp_target_hitbox[0];
				sdk::g_config_system->target_hitbox[1] = sdk::g_config_system->awp_target_hitbox[1];
				sdk::g_config_system->target_hitbox[2] = sdk::g_config_system->awp_target_hitbox[2];
				sdk::g_config_system->target_hitbox[3] = sdk::g_config_system->awp_target_hitbox[3];
				sdk::g_config_system->target_hitbox[4] = sdk::g_config_system->awp_target_hitbox[4];
				sdk::g_config_system->target_hitbox[5] = sdk::g_config_system->awp_target_hitbox[5];
				sdk::g_config_system->multi_point[0] = sdk::g_config_system->awp_multi_point[0];
				sdk::g_config_system->multi_point[1] = sdk::g_config_system->awp_multi_point[1];
				sdk::g_config_system->multi_point[2] = sdk::g_config_system->awp_multi_point[2];
				sdk::g_config_system->multi_point[3] = sdk::g_config_system->awp_multi_point[3];
				sdk::g_config_system->multi_point[4] = sdk::g_config_system->awp_multi_point[4];
				sdk::g_config_system->multi_point[5] = sdk::g_config_system->awp_multi_point[5];
				sdk::g_config_system->static_scale = sdk::g_config_system->awp_static_scale;
				sdk::g_config_system->head_scale = sdk::g_config_system->awp_head_scale;
				sdk::g_config_system->body_scale = sdk::g_config_system->awp_body_scale;
				sdk::g_config_system->automatic_fire = sdk::g_config_system->awp_automatic_fire;
				sdk::g_config_system->silent_aim = sdk::g_config_system->awp_silent_aim;
				sdk::g_config_system->minimum_hitchance = sdk::g_config_system->awp_minimum_hitchance;
				sdk::g_config_system->minimum_damage = sdk::g_config_system->awp_minimum_damage;
				sdk::g_config_system->automatic_penetration = sdk::g_config_system->awp_automatic_penetration;
				sdk::g_config_system->penetration_minimum_damage = sdk::g_config_system->awp_penetration_minimum_damage;
				sdk::g_config_system->override_minimum_damage = sdk::g_config_system->awp_override_minimum_damage;
				sdk::g_config_system->automatic_scope = sdk::g_config_system->awp_automatic_scope;
				sdk::g_config_system->maximum_fov = sdk::g_config_system->awp_maximum_fov;
				sdk::g_config_system->remove_recoil = sdk::g_config_system->awp_remove_recoil;
				sdk::g_config_system->accuracy_boost = sdk::g_config_system->awp_accuracy_boost;
				sdk::g_config_system->quick_stop = sdk::g_config_system->awp_quick_stop;
				sdk::g_config_system->type = sdk::g_config_system->awp_type;
				sdk::g_config_system->between_shots = sdk::g_config_system->awp_between_shots;
				sdk::g_config_system->lag_compensation = sdk::g_config_system->awp_lag_compensation;
				sdk::g_config_system->body_aim_conditions[0] = sdk::g_config_system->awp_body_aim_conditions[0];
				sdk::g_config_system->body_aim_conditions[1] = sdk::g_config_system->awp_body_aim_conditions[1];
				sdk::g_config_system->body_aim_conditions[2] = sdk::g_config_system->awp_body_aim_conditions[2];
				sdk::g_config_system->body_aim_conditions[3] = sdk::g_config_system->awp_body_aim_conditions[3];
				sdk::g_config_system->body_aim_conditions[4] = sdk::g_config_system->awp_body_aim_conditions[4];
				sdk::g_config_system->body_aim_conditions[5] = sdk::g_config_system->awp_body_aim_conditions[5];
				sdk::g_config_system->body_aim_conditions[6] = sdk::g_config_system->awp_body_aim_conditions[6];
				sdk::g_config_system->body_aim_conditions[7] = sdk::g_config_system->awp_body_aim_conditions[7];
				sdk::g_config_system->body_aim_conditions[8] = sdk::g_config_system->awp_body_aim_conditions[8];
				sdk::g_config_system->max_misses_body_aim = sdk::g_config_system->awp_max_misses_body_aim;
				sdk::g_config_system->safe_point_conditions[0] = sdk::g_config_system->awp_safe_point_conditions[0];
				sdk::g_config_system->safe_point_conditions[1] = sdk::g_config_system->awp_safe_point_conditions[1];
				sdk::g_config_system->safe_point_conditions[2] = sdk::g_config_system->awp_safe_point_conditions[2];
				sdk::g_config_system->safe_point_conditions[3] = sdk::g_config_system->awp_safe_point_conditions[3];
				sdk::g_config_system->safe_point_conditions[4] = sdk::g_config_system->awp_safe_point_conditions[4];
				sdk::g_config_system->safe_point_conditions[5] = sdk::g_config_system->awp_safe_point_conditions[5];
				sdk::g_config_system->safe_point_conditions[6] = sdk::g_config_system->awp_safe_point_conditions[6];
				sdk::g_config_system->safe_point_conditions[7] = sdk::g_config_system->awp_safe_point_conditions[7];
				sdk::g_config_system->safe_point_type = sdk::g_config_system->awp_safe_point_type;
				sdk::g_config_system->max_misses_safe_point = sdk::g_config_system->awp_max_misses_safe_point;
				sdk::g_config_system->double_tap = sdk::g_config_system->awp_double_tap;
				sdk::g_config_system->max_shift_amount = sdk::g_config_system->awp_max_shift_amount;
			}
			else if (g_context->weapon()->item_index() == valve::e_item_index::xm1014
				|| g_context->weapon()->item_index() == valve::e_item_index::mag7
				|| g_context->weapon()->item_index() == valve::e_item_index::sawedoff
				|| g_context->weapon()->item_index() == valve::e_item_index::nova) {
				if (!g_menu->m_is_opened)
					sdk::g_config_system->weapon_selection = 7;
				sdk::g_config_system->target_hitbox[0] = sdk::g_config_system->shotguns_target_hitbox[0];
				sdk::g_config_system->target_hitbox[1] = sdk::g_config_system->shotguns_target_hitbox[1];
				sdk::g_config_system->target_hitbox[2] = sdk::g_config_system->shotguns_target_hitbox[2];
				sdk::g_config_system->target_hitbox[3] = sdk::g_config_system->shotguns_target_hitbox[3];
				sdk::g_config_system->target_hitbox[4] = sdk::g_config_system->shotguns_target_hitbox[4];
				sdk::g_config_system->target_hitbox[5] = sdk::g_config_system->shotguns_target_hitbox[5];
				sdk::g_config_system->multi_point[0] = sdk::g_config_system->shotguns_multi_point[0];
				sdk::g_config_system->multi_point[1] = sdk::g_config_system->shotguns_multi_point[1];
				sdk::g_config_system->multi_point[2] = sdk::g_config_system->shotguns_multi_point[2];
				sdk::g_config_system->multi_point[3] = sdk::g_config_system->shotguns_multi_point[3];
				sdk::g_config_system->multi_point[4] = sdk::g_config_system->shotguns_multi_point[4];
				sdk::g_config_system->multi_point[5] = sdk::g_config_system->shotguns_multi_point[5];
				sdk::g_config_system->static_scale = sdk::g_config_system->shotguns_static_scale;
				sdk::g_config_system->head_scale = sdk::g_config_system->shotguns_head_scale;
				sdk::g_config_system->body_scale = sdk::g_config_system->shotguns_body_scale;
				sdk::g_config_system->automatic_fire = sdk::g_config_system->shotguns_automatic_fire;
				sdk::g_config_system->silent_aim = sdk::g_config_system->shotguns_silent_aim;
				sdk::g_config_system->minimum_hitchance = sdk::g_config_system->shotguns_minimum_hitchance;
				sdk::g_config_system->minimum_damage = sdk::g_config_system->shotguns_minimum_damage;
				sdk::g_config_system->automatic_penetration = sdk::g_config_system->shotguns_automatic_penetration;
				sdk::g_config_system->penetration_minimum_damage = sdk::g_config_system->shotguns_penetration_minimum_damage;
				sdk::g_config_system->override_minimum_damage = sdk::g_config_system->shotguns_override_minimum_damage;
				sdk::g_config_system->automatic_scope = sdk::g_config_system->shotguns_automatic_scope;
				sdk::g_config_system->maximum_fov = sdk::g_config_system->shotguns_maximum_fov;
				sdk::g_config_system->remove_recoil = sdk::g_config_system->shotguns_remove_recoil;
				sdk::g_config_system->accuracy_boost = sdk::g_config_system->shotguns_accuracy_boost;
				sdk::g_config_system->quick_stop = sdk::g_config_system->shotguns_quick_stop;
				sdk::g_config_system->type = sdk::g_config_system->shotguns_type;
				sdk::g_config_system->between_shots = sdk::g_config_system->shotguns_between_shots;
				sdk::g_config_system->lag_compensation = sdk::g_config_system->shotguns_lag_compensation;
				sdk::g_config_system->body_aim_conditions[0] = sdk::g_config_system->shotguns_body_aim_conditions[0];
				sdk::g_config_system->body_aim_conditions[1] = sdk::g_config_system->shotguns_body_aim_conditions[1];
				sdk::g_config_system->body_aim_conditions[2] = sdk::g_config_system->shotguns_body_aim_conditions[2];
				sdk::g_config_system->body_aim_conditions[3] = sdk::g_config_system->shotguns_body_aim_conditions[3];
				sdk::g_config_system->body_aim_conditions[4] = sdk::g_config_system->shotguns_body_aim_conditions[4];
				sdk::g_config_system->body_aim_conditions[5] = sdk::g_config_system->shotguns_body_aim_conditions[5];
				sdk::g_config_system->body_aim_conditions[6] = sdk::g_config_system->shotguns_body_aim_conditions[6];
				sdk::g_config_system->body_aim_conditions[7] = sdk::g_config_system->shotguns_body_aim_conditions[7];
				sdk::g_config_system->body_aim_conditions[8] = sdk::g_config_system->shotguns_body_aim_conditions[8];
				sdk::g_config_system->max_misses_body_aim = sdk::g_config_system->shotguns_max_misses_body_aim;
				sdk::g_config_system->safe_point_conditions[0] = sdk::g_config_system->shotguns_safe_point_conditions[0];
				sdk::g_config_system->safe_point_conditions[1] = sdk::g_config_system->shotguns_safe_point_conditions[1];
				sdk::g_config_system->safe_point_conditions[2] = sdk::g_config_system->shotguns_safe_point_conditions[2];
				sdk::g_config_system->safe_point_conditions[3] = sdk::g_config_system->shotguns_safe_point_conditions[3];
				sdk::g_config_system->safe_point_conditions[4] = sdk::g_config_system->shotguns_safe_point_conditions[4];
				sdk::g_config_system->safe_point_conditions[5] = sdk::g_config_system->shotguns_safe_point_conditions[5];
				sdk::g_config_system->safe_point_conditions[6] = sdk::g_config_system->shotguns_safe_point_conditions[6];
				sdk::g_config_system->safe_point_conditions[7] = sdk::g_config_system->shotguns_safe_point_conditions[7];
				sdk::g_config_system->safe_point_type = sdk::g_config_system->shotguns_safe_point_type;
				sdk::g_config_system->max_misses_safe_point = sdk::g_config_system->shotguns_max_misses_safe_point;
				sdk::g_config_system->double_tap = sdk::g_config_system->shotguns_double_tap;
				sdk::g_config_system->max_shift_amount = sdk::g_config_system->shotguns_max_shift_amount;
			}
			else if (g_context->weapon()->item_index() == valve::e_item_index::m249
				|| g_context->weapon()->item_index() == valve::e_item_index::negev) {
				if (!g_menu->m_is_opened)
					sdk::g_config_system->weapon_selection = 8;
				sdk::g_config_system->target_hitbox[0] = sdk::g_config_system->machine_guns_target_hitbox[0];
				sdk::g_config_system->target_hitbox[1] = sdk::g_config_system->machine_guns_target_hitbox[1];
				sdk::g_config_system->target_hitbox[2] = sdk::g_config_system->machine_guns_target_hitbox[2];
				sdk::g_config_system->target_hitbox[3] = sdk::g_config_system->machine_guns_target_hitbox[3];
				sdk::g_config_system->target_hitbox[4] = sdk::g_config_system->machine_guns_target_hitbox[4];
				sdk::g_config_system->target_hitbox[5] = sdk::g_config_system->machine_guns_target_hitbox[5];
				sdk::g_config_system->multi_point[0] = sdk::g_config_system->machine_guns_multi_point[0];
				sdk::g_config_system->multi_point[1] = sdk::g_config_system->machine_guns_multi_point[1];
				sdk::g_config_system->multi_point[2] = sdk::g_config_system->machine_guns_multi_point[2];
				sdk::g_config_system->multi_point[3] = sdk::g_config_system->machine_guns_multi_point[3];
				sdk::g_config_system->multi_point[4] = sdk::g_config_system->machine_guns_multi_point[4];
				sdk::g_config_system->multi_point[5] = sdk::g_config_system->machine_guns_multi_point[5];
				sdk::g_config_system->static_scale = sdk::g_config_system->machine_guns_static_scale;
				sdk::g_config_system->head_scale = sdk::g_config_system->machine_guns_head_scale;
				sdk::g_config_system->body_scale = sdk::g_config_system->machine_guns_body_scale;
				sdk::g_config_system->automatic_fire = sdk::g_config_system->machine_guns_automatic_fire;
				sdk::g_config_system->silent_aim = sdk::g_config_system->machine_guns_silent_aim;
				sdk::g_config_system->minimum_hitchance = sdk::g_config_system->machine_guns_minimum_hitchance;
				sdk::g_config_system->minimum_damage = sdk::g_config_system->machine_guns_minimum_damage;
				sdk::g_config_system->automatic_penetration = sdk::g_config_system->machine_guns_automatic_penetration;
				sdk::g_config_system->penetration_minimum_damage = sdk::g_config_system->machine_guns_penetration_minimum_damage;
				sdk::g_config_system->override_minimum_damage = sdk::g_config_system->machine_guns_override_minimum_damage;
				sdk::g_config_system->automatic_scope = sdk::g_config_system->machine_guns_automatic_scope;
				sdk::g_config_system->maximum_fov = sdk::g_config_system->machine_guns_maximum_fov;
				sdk::g_config_system->remove_recoil = sdk::g_config_system->machine_guns_remove_recoil;
				sdk::g_config_system->accuracy_boost = sdk::g_config_system->machine_guns_accuracy_boost;
				sdk::g_config_system->quick_stop = sdk::g_config_system->machine_guns_quick_stop;
				sdk::g_config_system->type = sdk::g_config_system->machine_guns_type;
				sdk::g_config_system->between_shots = sdk::g_config_system->machine_guns_between_shots;
				sdk::g_config_system->lag_compensation = sdk::g_config_system->machine_guns_lag_compensation;
				sdk::g_config_system->body_aim_conditions[0] = sdk::g_config_system->machine_guns_body_aim_conditions[0];
				sdk::g_config_system->body_aim_conditions[1] = sdk::g_config_system->machine_guns_body_aim_conditions[1];
				sdk::g_config_system->body_aim_conditions[2] = sdk::g_config_system->machine_guns_body_aim_conditions[2];
				sdk::g_config_system->body_aim_conditions[3] = sdk::g_config_system->machine_guns_body_aim_conditions[3];
				sdk::g_config_system->body_aim_conditions[4] = sdk::g_config_system->machine_guns_body_aim_conditions[4];
				sdk::g_config_system->body_aim_conditions[5] = sdk::g_config_system->machine_guns_body_aim_conditions[5];
				sdk::g_config_system->body_aim_conditions[6] = sdk::g_config_system->machine_guns_body_aim_conditions[6];
				sdk::g_config_system->body_aim_conditions[7] = sdk::g_config_system->machine_guns_body_aim_conditions[7];
				sdk::g_config_system->body_aim_conditions[8] = sdk::g_config_system->machine_guns_body_aim_conditions[8];
				sdk::g_config_system->max_misses_body_aim = sdk::g_config_system->machine_guns_max_misses_body_aim;
				sdk::g_config_system->safe_point_conditions[0] = sdk::g_config_system->machine_guns_safe_point_conditions[0];
				sdk::g_config_system->safe_point_conditions[1] = sdk::g_config_system->machine_guns_safe_point_conditions[1];
				sdk::g_config_system->safe_point_conditions[2] = sdk::g_config_system->machine_guns_safe_point_conditions[2];
				sdk::g_config_system->safe_point_conditions[3] = sdk::g_config_system->machine_guns_safe_point_conditions[3];
				sdk::g_config_system->safe_point_conditions[4] = sdk::g_config_system->machine_guns_safe_point_conditions[4];
				sdk::g_config_system->safe_point_conditions[5] = sdk::g_config_system->machine_guns_safe_point_conditions[5];
				sdk::g_config_system->safe_point_conditions[6] = sdk::g_config_system->machine_guns_safe_point_conditions[6];
				sdk::g_config_system->safe_point_conditions[7] = sdk::g_config_system->machine_guns_safe_point_conditions[7];
				sdk::g_config_system->safe_point_type = sdk::g_config_system->machine_guns_safe_point_type;
				sdk::g_config_system->max_misses_safe_point = sdk::g_config_system->machine_guns_max_misses_safe_point;
				sdk::g_config_system->double_tap = sdk::g_config_system->machine_guns_double_tap;
				sdk::g_config_system->max_shift_amount = sdk::g_config_system->machine_guns_max_shift_amount;
			}
			/*else if (g_context->weapon()->item_index() == valve::e_item_index::taser) {
				if (!g_menu->m_is_opened)
					sdk::g_config_system->weapon_selection = 9;
				sdk::g_config_system->target_hitbox[0] = sdk::g_config_system->taser_target_hitbox[0];
				sdk::g_config_system->target_hitbox[1] = sdk::g_config_system->taser_target_hitbox[1];
				sdk::g_config_system->target_hitbox[2] = sdk::g_config_system->taser_target_hitbox[2];
				sdk::g_config_system->target_hitbox[3] = sdk::g_config_system->taser_target_hitbox[3];
				sdk::g_config_system->target_hitbox[4] = sdk::g_config_system->taser_target_hitbox[4];
				sdk::g_config_system->target_hitbox[5] = sdk::g_config_system->taser_target_hitbox[5];
				sdk::g_config_system->multi_point[0] = sdk::g_config_system->taser_multi_point[0];
				sdk::g_config_system->multi_point[1] = sdk::g_config_system->taser_multi_point[1];
				sdk::g_config_system->multi_point[2] = sdk::g_config_system->taser_multi_point[2];
				sdk::g_config_system->multi_point[3] = sdk::g_config_system->taser_multi_point[3];
				sdk::g_config_system->multi_point[4] = sdk::g_config_system->taser_multi_point[4];
				sdk::g_config_system->multi_point[5] = sdk::g_config_system->taser_multi_point[5];
				sdk::g_config_system->static_scale = sdk::g_config_system->taser_static_scale;
				sdk::g_config_system->head_scale = sdk::g_config_system->taser_head_scale;
				sdk::g_config_system->body_scale = sdk::g_config_system->taser_body_scale;
				sdk::g_config_system->automatic_fire = sdk::g_config_system->taser_automatic_fire;
				sdk::g_config_system->silent_aim = sdk::g_config_system->taser_silent_aim;
				sdk::g_config_system->minimum_hitchance = sdk::g_config_system->taser_minimum_hitchance;
				sdk::g_config_system->minimum_damage = sdk::g_config_system->taser_minimum_damage;
				sdk::g_config_system->automatic_penetration = sdk::g_config_system->taser_automatic_penetration;
				sdk::g_config_system->penetration_minimum_damage = sdk::g_config_system->taser_penetration_minimum_damage;
				sdk::g_config_system->override_minimum_damage = sdk::g_config_system->taser_override_minimum_damage;
				sdk::g_config_system->automatic_scope = sdk::g_config_system->taser_automatic_scope;
				sdk::g_config_system->maximum_fov = sdk::g_config_system->taser_maximum_fov;
				sdk::g_config_system->remove_recoil = sdk::g_config_system->taser_remove_recoil;
				sdk::g_config_system->accuracy_boost = sdk::g_config_system->taser_accuracy_boost;
				sdk::g_config_system->quick_stop = sdk::g_config_system->taser_quick_stop;
				sdk::g_config_system->type = sdk::g_config_system->taser_type;
				sdk::g_config_system->between_shots = sdk::g_config_system->taser_between_shots;
				sdk::g_config_system->lag_compensation = sdk::g_config_system->taser_lag_compensation;
				sdk::g_config_system->body_aim_conditions[0] = sdk::g_config_system->taser_body_aim_conditions[0];
				sdk::g_config_system->body_aim_conditions[1] = sdk::g_config_system->taser_body_aim_conditions[1];
				sdk::g_config_system->body_aim_conditions[2] = sdk::g_config_system->taser_body_aim_conditions[2];
				sdk::g_config_system->body_aim_conditions[3] = sdk::g_config_system->taser_body_aim_conditions[3];
				sdk::g_config_system->body_aim_conditions[4] = sdk::g_config_system->taser_body_aim_conditions[4];
				sdk::g_config_system->body_aim_conditions[5] = sdk::g_config_system->taser_body_aim_conditions[5];
				sdk::g_config_system->body_aim_conditions[6] = sdk::g_config_system->taser_body_aim_conditions[6];
				sdk::g_config_system->body_aim_conditions[7] = sdk::g_config_system->taser_body_aim_conditions[7];
				sdk::g_config_system->body_aim_conditions[8] = sdk::g_config_system->taser_body_aim_conditions[8];
				sdk::g_config_system->max_misses_body_aim = sdk::g_config_system->taser_max_misses_body_aim;
				sdk::g_config_system->safe_point_conditions[0] = sdk::g_config_system->taser_safe_point_conditions[0];
				sdk::g_config_system->safe_point_conditions[1] = sdk::g_config_system->taser_safe_point_conditions[1];
				sdk::g_config_system->safe_point_conditions[2] = sdk::g_config_system->taser_safe_point_conditions[2];
				sdk::g_config_system->safe_point_conditions[3] = sdk::g_config_system->taser_safe_point_conditions[3];
				sdk::g_config_system->safe_point_conditions[4] = sdk::g_config_system->taser_safe_point_conditions[4];
				sdk::g_config_system->safe_point_conditions[5] = sdk::g_config_system->taser_safe_point_conditions[5];
				sdk::g_config_system->safe_point_conditions[6] = sdk::g_config_system->taser_safe_point_conditions[6];
				sdk::g_config_system->safe_point_conditions[7] = sdk::g_config_system->taser_safe_point_conditions[7];
				sdk::g_config_system->safe_point_type = sdk::g_config_system->taser_safe_point_type;
				sdk::g_config_system->max_misses_safe_point = sdk::g_config_system->taser_max_misses_safe_point;
				sdk::g_config_system->double_tap = sdk::g_config_system->taser_double_tap;
				sdk::g_config_system->max_shift_amount = sdk::g_config_system->taser_max_shift_amount;
			}*/
		}

		static auto prev_spawn_time = valve::g_local_player->spawn_time();
		if (prev_spawn_time != valve::g_local_player->spawn_time()) {
			hacks::g_eng_pred->reset_on_spawn();

			hacks::g_exploits->ticks_allowed() = 0;

			prev_spawn_time = valve::g_local_player->spawn_time();
		}	

		hacks::g_movement->should_fake_duck()
			= sdk::g_config_system->fake_duck_key
			&& key_handler::check_key(sdk::g_config_system->fake_duck_key, sdk::g_config_system->fake_duck_key_style)
			&& valve::g_local_player->flags() & valve::e_ent_flags::on_ground
			&& !(user_cmd.m_buttons & valve::e_buttons::in_jump);

		hacks::g_exploits->in_charge() = false;
		hacks::g_eng_pred->old_user_cmd() = user_cmd;

		if (!hacks::g_exploits->try_to_recharge()) {
			hacks::g_eng_pred->prepare();

			hacks::g_exploits->manage_wpn(user_cmd);

			if (g_context->weapon() && g_context->wpn_data()
				&& !(g_context->flags() & e_context_flags::can_shoot)
				&& ((static_cast<std::size_t>(g_context->wpn_data()->m_unk_type - 2) <= 5
					|| g_context->weapon()->item_index() == valve::e_item_index::taser)
					&& g_context->weapon()->item_index() != valve::e_item_index::revolver))
				user_cmd.m_buttons &= ~valve::e_buttons::in_attack;

			hacks::g_movement->on_create_move(user_cmd);

			hacks::g_visuals->on_create_move(user_cmd);

			hacks::g_eng_pred->predict(&user_cmd);

			hacks::g_anti_aim->choke(&user_cmd);

			if (g_context->flags() & e_context_flags::recharge) {
				if (!hacks::g_movement->should_fake_duck()
					&& !hacks::g_exploits->charged())
					g_context->flags() &= ~e_context_flags::choke;

				user_cmd.m_buttons &= ~valve::e_buttons::in_attack;
			}
			else {
				hacks::g_aim_bot->on_create_move(user_cmd);

				if (user_cmd.m_buttons & valve::e_buttons::in_attack
					&& !(g_context->flags() & e_context_flags::aim_fire)
					&& g_context->flags() & e_context_flags::can_shoot) {

					if (sdk::g_config_system->remove_recoil)
						user_cmd.m_view_angles -= valve::g_local_player->aim_punch() * g_context->cvars().m_weapon_recoil_scale->get_float();

					user_cmd.m_view_angles.x = std::remainder(user_cmd.m_view_angles.x, 360.f);
					user_cmd.m_view_angles.y = std::remainder(user_cmd.m_view_angles.y, 360.f);
					user_cmd.m_view_angles.z = std::remainder(user_cmd.m_view_angles.z, 360.f);

					user_cmd.m_view_angles.x = std::clamp(user_cmd.m_view_angles.x, -89.f, 89.f);
					user_cmd.m_view_angles.y = std::clamp(user_cmd.m_view_angles.y, -180.f, 180.f);
					user_cmd.m_view_angles.z = std::clamp(user_cmd.m_view_angles.z, -90.f, 90.f);
				}
			}

			hacks::g_anti_aim->set_pitch(user_cmd);

			hacks::g_movement->normalize(user_cmd);

			hacks::g_movement->rotate(
				user_cmd, hacks::g_eng_pred->old_user_cmd().m_view_angles,
				valve::g_local_player->flags(), valve::g_local_player->move_type()
			);

			if (g_context->flags() & e_context_flags::can_shoot
				&& g_context->will_shoot(g_context->weapon(), user_cmd)) {
				auto& anim_data = hacks::g_anim_sync->local_data();

				anim_data.m_shot_cmd_number = user_cmd.m_number;

				if (g_context->wpn_data()) {
					if (static_cast<std::size_t>(g_context->wpn_data()->m_unk_type - 2) > 5)
						anim_data.m_shot_valid_wpn = g_context->weapon()->item_index() == valve::e_item_index::taser;
					else
						anim_data.m_shot_valid_wpn = true;
				}
				else
					anim_data.m_shot_valid_wpn = false;

				if (anim_data.m_shot_valid_wpn) {
					if (!hacks::g_movement->should_fake_duck()) {
						hacks::g_exploits->cur_shift_amount() = hacks::g_exploits->next_shift_amount();

						g_context->flags() &= ~e_context_flags::choke;
					}

					if (g_context->flags() & e_context_flags::aim_fire) {
						const auto& aim_target = hacks::g_aim_bot->last_target();

						if (sdk::g_config_system->on_shot)
							hacks::g_visuals->add_shot_mdl(aim_target.m_entry->m_player, aim_target.m_lag_record);

						hacks::g_shots->add(
							g_context->shoot_pos(), &aim_target,
							hacks::g_exploits->next_shift_amount()
						);

						if (aim_target.m_entry
							&& aim_target.m_lag_record
							&& aim_target.m_entry->m_player)
							if (const auto player_info = valve::g_engine->player_info(aim_target.m_entry->m_player->index()); player_info.has_value()) {
#ifdef ALPHA
								util::g_notify->print_logo();
								util::g_notify->print_notify(true, false, 0xff998877u, xorstr_("fired shot at %s (hc: %d | dmg: %d | aimed: %s | center: %s | sp: %s | bt: %d | type: %d | side: %d | lc: %s | choked: %d)\n"),
									player_info.value().m_name,
									aim_target.m_hit_chance,
									aim_target.m_dmg,
									util::translate_hitgroup(aim_target.m_point.m_hitgroup),
									util::bool_as_text(aim_target.m_point.m_center),
									util::translate_safe_points(aim_target.m_point.m_intersections, aim_target.m_point.m_low_intersections),
									aim_target.m_lag_record->m_extrapolated ? std::min(0, -aim_target.m_lag_record->m_extrapolate_ticks) : std::max(0, (valve::g_global_vars->m_tick_count - valve::to_ticks(aim_target.m_lag_record->m_sim_time))),
									aim_target.m_lag_record->m_type,
									aim_target.m_lag_record->m_side,
									util::bool_as_text(aim_target.m_lag_record->m_broke_lc),
									aim_target.m_lag_record->m_sim_ticks - 1
								);

								util::g_notify->print_logo();
								util::g_notify->print_notify(true, false, 0xff998877u, xorstr_("%.4f | %.4f | %.4f | %.4f\n"), aim_target.m_lag_record->m_server_rate * 1000.f, aim_target.m_lag_record->m_negative_rate * 1000.f, aim_target.m_lag_record->m_positive_rate * 1000.f, aim_target.m_lag_record->m_zero_rate * 1000.f);
								util::g_notify->print_logo();
								util::g_notify->print_notify(true, false, 0xff998877u, xorstr_("%.4f | %.4f\n"), aim_target.m_lag_record->m_low_negative_rate * 1000.f, aim_target.m_lag_record->m_low_positive_rate * 1000.f);
#else
#ifdef _DEBUG
								util::g_notify->print_logo();
								util::g_notify->print_notify(true, false, 0xff998877u, xorstr_("fired shot at %s (hc: %d | dmg: %d | aimed: %s | center: %s | sp: %s | bt: %d | type: %d | side: %d | lc: %s | choked: %d)\n"),
									player_info.value().m_name,
									aim_target.m_hit_chance,
									aim_target.m_dmg,
									util::translate_hitgroup(aim_target.m_point.m_hitgroup),
									util::bool_as_text(aim_target.m_point.m_center),
									util::translate_safe_points(aim_target.m_point.m_intersections, aim_target.m_point.m_low_intersections),
									aim_target.m_lag_record->m_extrapolated ? std::min(0, -aim_target.m_lag_record->m_extrapolate_ticks) : std::max(0, (valve::g_global_vars->m_tick_count - valve::to_ticks(aim_target.m_lag_record->m_sim_time))),
									aim_target.m_lag_record->m_type,
									aim_target.m_lag_record->m_side,
									util::bool_as_text(aim_target.m_lag_record->m_broke_lc),
									aim_target.m_lag_record->m_sim_ticks - 1
								);

								util::g_notify->print_logo();
								util::g_notify->print_notify(true, false, 0xff998877u, xorstr_("%.4f | %.4f | %.4f | %.4f\n"), aim_target.m_lag_record->m_server_rate * 1000.f, aim_target.m_lag_record->m_negative_rate * 1000.f, aim_target.m_lag_record->m_positive_rate * 1000.f, aim_target.m_lag_record->m_zero_rate * 1000.f);
								util::g_notify->print_logo();
								util::g_notify->print_notify(true, false, 0xff998877u, xorstr_("%.4f | %.4f\n"), aim_target.m_lag_record->m_low_negative_rate * 1000.f, aim_target.m_lag_record->m_low_positive_rate * 1000.f);
#else
								if (g_context->debug_build) {
									util::g_notify->print_logo();
									util::g_notify->print_notify(true, false, 0xff998877u, xorstr_("fired shot at %s (hc: %d | dmg: %d | aimed: %s | center: %s | sp: %s | bt: %d | type: %d | side: %d | lc: %s | choked: %d)\n"),
										player_info.value().m_name,
										aim_target.m_hit_chance,
										aim_target.m_dmg,
										util::translate_hitgroup(aim_target.m_point.m_hitgroup),
										util::bool_as_text(aim_target.m_point.m_center),
										util::translate_safe_points(aim_target.m_point.m_intersections, aim_target.m_point.m_low_intersections),
										aim_target.m_lag_record->m_extrapolated ? std::min(0, -aim_target.m_lag_record->m_extrapolate_ticks) : std::max(0, (valve::g_global_vars->m_tick_count - valve::to_ticks(aim_target.m_lag_record->m_sim_time))),
										aim_target.m_lag_record->m_type,
										aim_target.m_lag_record->m_side,
										util::bool_as_text(aim_target.m_lag_record->m_broke_lc),
										aim_target.m_lag_record->m_sim_ticks - 1
									);

									util::g_notify->print_logo();
									util::g_notify->print_notify(true, false, 0xff998877u, xorstr_("%.4f | %.4f | %.4f | %.4f\n"), aim_target.m_lag_record->m_server_rate * 1000.f, aim_target.m_lag_record->m_negative_rate * 1000.f, aim_target.m_lag_record->m_positive_rate * 1000.f, aim_target.m_lag_record->m_zero_rate * 1000.f);
									util::g_notify->print_logo();
									util::g_notify->print_notify(true, false, 0xff998877u, xorstr_("%.4f | %.4f\n"), aim_target.m_lag_record->m_low_negative_rate * 1000.f, aim_target.m_lag_record->m_low_positive_rate * 1000.f);
								}
#endif
#endif
							}
					}
					else
						hacks::g_shots->add(
							g_context->shoot_pos(), nullptr,
							hacks::g_exploits->next_shift_amount()
						);
				}
			}

			hacks::g_anim_sync->update_local_real(user_cmd);

			hacks::g_eng_pred->restore();

			if (g_context->weapon()) {
				g_context->weapon()->recoil_index() = hacks::g_eng_pred->recoil_index();
				g_context->weapon()->accuracy_penalty() = hacks::g_eng_pred->accuracy_penalty();
			}

			send_packet = !(g_context->flags() & e_context_flags::choke);

			if (!key_handler::check_key(sdk::g_config_system->peek_assistence_key, sdk::g_config_system->peek_assistence_key_style)) {
				g_context->should_return() = false;
				g_context->start_position() = vec3_t{};
			}
			else if (g_context->start_position().x == 0.f
				|| g_context->start_position().y == 0.f
				|| g_context->start_position().z == 0.f
				) {
				g_context->start_position() = valve::g_local_player->abs_origin();

				if (!(hacks::g_eng_pred->local_data().at((user_cmd.m_number) % 150).m_net_vars.m_flags & valve::e_ent_flags::on_ground)) {
					valve::ray_t ray(g_context->start_position(), g_context->start_position() - vec3_t(0.f, 0.f, 1000.f));
					valve::trace_filter_world_only_t filter;
					valve::trace_t trace;

					valve::g_engine_trace->trace_ray(ray, valve::e_mask::solid, &filter, &trace);

					if (trace.m_fraction < 1.f)
						g_context->start_position() = trace.m_end_pos + vec3_t(0.f, 0.f, 2.f);
				}
			}
			else if ((g_context->flags() & e_context_flags::can_shoot
				&& g_context->will_shoot(valve::g_local_player->weapon(), user_cmd)))
				g_context->should_return() = true;
		}
		else {
			send_packet = false;

			user_cmd.m_tick = std::numeric_limits< int >::max();

			hacks::g_anim_sync->setup_local_bones();

			auto& local_data = hacks::g_eng_pred->local_data().at(user_cmd.m_number % 150);

			local_data.init(user_cmd);
			local_data.save(user_cmd.m_number);

			hacks::g_exploits->in_charge() = true;
		}

		hacks::g_eng_pred->local_data().at(user_cmd.m_number % 150).m_move = user_cmd.m_move;

		hacks::g_movement->prev_view_yaw() = hacks::g_eng_pred->old_user_cmd().m_view_angles.y;

		hacks::g_movement->normalize(user_cmd);

		if (!send_packet) {
			auto& net_channel = valve::g_client_state->m_net_channel;
			const auto backup_choked_packets = net_channel->m_choked_packets;
			net_channel->m_choked_packets = 0;
			net_channel->send_datagram(0);
			--net_channel->m_out_seq_number;
			net_channel->m_choked_packets = backup_choked_packets;
		}
		else {
			g_context->broke_lc() = (valve::g_local_player->origin() - g_context->last_sent_origin()).length_2d_sqr() > 4096.f;

			g_context->last_sent_origin() = valve::g_local_player->origin();

			if (!hacks::g_exploits->cur_shift_amount()
				|| (hacks::g_exploits->type() != 2 && hacks::g_exploits->type() != 3))
				g_context->add_packet(user_cmd.m_number);

			if (valve::g_client_state->m_last_cmd_out == hacks::g_exploits->recharge_cmd()) {
				auto& local_data = hacks::g_eng_pred->local_data().at(user_cmd.m_number % 150);
				local_data.m_override_tick_base = true;
				local_data.m_adjusted_tick_base = hacks::g_exploits->adjust_tick_base(
					valve::g_client_state->m_choked_cmds + 1, 1, -valve::g_client_state->m_choked_cmds
				);
			}			
		}

		g_context->last_cmd_number() = user_cmd.m_number;

		if (g_context->flags() & e_context_flags::aim_fire
			|| user_cmd.m_buttons & valve::e_buttons::in_attack)
			hacks::g_anim_sync->local_data().m_last_shot_time = valve::g_global_vars->m_cur_time;

		hacks::g_eng_pred->last_user_cmd() = user_cmd;

		valve::g_local_player->velocity_modifier() = hacks::g_eng_pred->velocity_modifier();

		valve::g_input->m_verified_cmds[seq_number % 150] = { user_cmd, user_cmd.calc_checksum() };
	}

	void __stdcall frame_stage_notify(const valve::e_frame_stage stage) {
		hacks::g_eng_pred->last_frame_stage() = stage;

		if (!valve::g_local_player
			|| !valve::g_engine->in_game()) {
			hacks::g_shots->elements().clear();
			return orig_frame_stage_notify(stage);
		}

		if (stage == valve::e_frame_stage::render_start) {
			hacks::g_visuals->on_render_start();

			if (hacks::g_exploits->type() == 4) {
				auto& var_mapping = valve::g_local_player->var_mapping();

				for (int i{}; i < var_mapping.m_interpolated_entries; ++i)
					var_mapping.m_entries.at(i).m_needs_to_interpolate = false;
			}

			const auto correction_ticks = hacks::g_exploits->calc_correction_ticks();
			for (auto i = 1; i <= valve::g_global_vars->m_max_clients; ++i) {
				const auto player = static_cast<valve::c_player*>(
					valve::g_entity_list->find_entity(i)
					);
				if (!player
					|| player->dormant()
					|| !player->alive()
					|| player == valve::g_local_player)
					continue;

				auto& entry = hacks::g_lag_comp->entry(i - 1);

				if (player->sim_time() == player->old_sim_time()
					|| (entry.m_unk = false, correction_ticks == -1)
					|| std::abs(valve::to_ticks(player->sim_time()) - valve::g_client_state->m_server_tick) <= (correction_ticks + 1)) {
					if (!entry.m_unk)
						continue;
				}
				else
					entry.m_unk = true;

				auto& var_mapping = player->var_mapping();

				for (int j{}; j < var_mapping.m_interpolated_entries; ++j)
					var_mapping.m_entries.at(j).m_needs_to_interpolate = false;
			}
		}

		orig_frame_stage_notify(stage);

		if (stage == valve::e_frame_stage::net_update_end) {
			if (sdk::g_config_system->ragdolls
				|| sdk::g_config_system->disable_rendering_of_ragdolls) {
				for (auto i = 1; i <= valve::g_entity_list->highest_index(); ++i) {
					const auto entity = valve::g_entity_list->find_entity(i);
					if (!entity)
						continue;

					const auto client_class = entity->client_class();
					if (!client_class
						|| client_class->m_class_id != valve::e_class_id::ragdoll)
						continue;

					entity->use_fast_path() = false;
				}
			}

			const auto correction_ticks = hacks::g_exploits->calc_correction_ticks();
			if (correction_ticks == -1)
				hacks::g_exploits->correction_amount() = 0;
			else if (valve::g_local_player->sim_time() > valve::g_local_player->old_sim_time()) {
				const auto delta = valve::to_ticks(valve::g_local_player->sim_time()) - valve::g_client_state->m_server_tick;
				if (std::abs(delta) <= correction_ticks)
					hacks::g_exploits->correction_amount() = delta;
			}

			valve::g_engine->fire_events();

			hacks::g_shots->on_net_update();

			hacks::g_lag_comp->on_net_update();
		}
		else if (stage == valve::e_frame_stage::render_end)
			hacks::g_visuals->on_render_end();
		else if (stage == valve::e_frame_stage::net_update_post_data_update_start)
			hacks::g_visuals->on_post_data_update_start();

		if (const auto view_model = valve::g_local_player->view_model()) {
			static float cycle{}, anim_time{};

			if (stage == valve::e_frame_stage::net_update_post_data_update_start) {
				view_model->cycle() = cycle;
				view_model->anim_time() = anim_time;
			}

			cycle = view_model->cycle();
			anim_time = view_model->anim_time();
		}
			
		hacks::g_visuals->kill_feed();
	}

	void __fastcall update_client_side_anim(valve::c_player* const ecx, const std::uintptr_t edx) {
		if (!ecx->alive()
			|| ecx->index() < 1
			|| ecx->index() > 64
			|| (ecx->friendly()
				&& ecx != valve::g_local_player))
			return orig_update_client_side_anim(ecx, edx);

		if (!g_context->allow_anim_update()) {
			const auto most_recent_model_bone_counter = **reinterpret_cast<unsigned long**>(
				g_context->addresses().m_invalidate_bone_cache + 0xau
				);

			static auto prev_most_recent_model_bone_counter = ecx->most_recent_model_bone_counter();

			if (most_recent_model_bone_counter != prev_most_recent_model_bone_counter)
				hacks::g_anim_sync->setup_local_bones();

			prev_most_recent_model_bone_counter = most_recent_model_bone_counter;

			return;
		}

		for (auto& layer : ecx->anim_layers())
			layer.m_owner = ecx;

		orig_update_client_side_anim(ecx, edx);
	}

	bool __fastcall setup_bones(
		const std::uintptr_t ecx, const std::uintptr_t edx, mat3x4_t* const bones, int max_bones, int mask, float time
	) {		
		const auto player = reinterpret_cast<valve::c_player*>(ecx - sizeof(std::uintptr_t));
		if (!player->alive()
			|| player->index() < 1
			|| player->index() > 64
			|| (player->friendly()
				&& player != valve::g_local_player))
			return orig_setup_bones(ecx, edx, bones, max_bones, mask, time);

		if (!g_context->allow_setup_bones()) {
			if (!bones
				|| max_bones == -1)
				return true;

			if (player == valve::g_local_player) {
				std::memcpy(
					bones, hacks::g_anim_sync->local_data().m_real.m_bones.data(),
					std::min(max_bones, 128) * sizeof(mat3x4_t)
				);
			}
			else {
				const auto& entry = hacks::g_lag_comp->entry(player->index() - 1);

				std::memcpy(
					bones, entry.m_bones.data(),
					std::min(max_bones, 128) * sizeof(mat3x4_t)
				);
			}

			return true;
		}

		if (const auto backup_last_setup_bones_frame = player->last_setup_bones_frame()) {
			const auto backup_frame_count = valve::g_global_vars->m_frame_count;

			const auto index = player->index();

			auto adjusted = false;

			if (valve::g_global_vars->m_frame_time < 0.0033333334) {
				const auto frame_count = index % 3 + 2 * (index % 3) + 1;

				valve::g_global_vars->m_frame_count = frame_count;
				player->last_setup_bones_frame() = frame_count;

				adjusted = true;
			}
			else if (valve::g_global_vars->m_frame_time < 0.0050000004) {
				const auto frame_count = 2 * (index % 2) + 1;

				valve::g_global_vars->m_frame_count = frame_count;
				player->last_setup_bones_frame() = frame_count;

				adjusted = true;
			}
			else if (valve::g_global_vars->m_frame_time < 0.0066666668) {
				const auto frame_count = 3 * (index % 3);

				valve::g_global_vars->m_frame_count = frame_count;
				player->last_setup_bones_frame() = frame_count;

				adjusted = true;
			}

			if (adjusted) {
				const auto ret = orig_setup_bones(ecx, edx, bones, max_bones, mask, time);

				valve::g_global_vars->m_frame_count = backup_frame_count;
				player->last_setup_bones_frame() = backup_last_setup_bones_frame;

				return ret;
			}
		}

		return orig_setup_bones(ecx, edx, bones, max_bones, mask, time);
	}

	void __fastcall do_extra_bones_processing(
		valve::c_player* const ecx, const std::uintptr_t edx, int a0, int a1, int a2, int a3, int a4, int a5
	) {
		if (!ecx->alive()
			|| ecx->index() < 1
			|| ecx->index() > 64
			|| (ecx->friendly()
				&& ecx != valve::g_local_player))
			orig_do_extra_bones_processing(ecx, edx, a0, a1, a2, a3, a4, a5);
	}

	void __fastcall accumulate_layers(
		valve::c_player* const ecx, const std::uintptr_t edx, int a0, int a1, float a2, int a3
	) {
		if (!ecx->alive()
			|| ecx->index() < 1
			|| ecx->index() > 64
			|| (ecx->friendly()
				&& ecx != valve::g_local_player))
			return orig_accumulate_layers(ecx, edx, a0, a1, a2, a3);

		if (const auto anim_state = ecx->anim_state()) {
			const auto backup_first_update = anim_state->m_first_update;

			anim_state->m_first_update = true;

			orig_accumulate_layers(ecx, edx, a0, a1, a2, a3);

			anim_state->m_first_update = backup_first_update;

			return;
		}

		orig_accumulate_layers(ecx, edx, a0, a1, a2, a3);
	}

	void __fastcall standard_blending_rules(
		valve::c_player* const ecx, const std::uintptr_t edx, valve::model_data_t* const mdl_data, int a1, int a2, float a3, int mask
	) {
		if (!ecx->alive()
			|| ecx->index() < 1
			|| ecx->index() > 64
			|| (ecx->friendly()
				&& ecx != valve::g_local_player))
			return orig_standard_blending_rules(ecx, edx, mdl_data, a1, a2, a3, mask);

		if (g_context->force_bone_mask())
			mask = 0x100;

		const auto bone_index = ecx->lookup_bone(xorstr_("lean_root"));
		if (bone_index == -1)
			return orig_standard_blending_rules(ecx, edx, mdl_data, a1, a2, a3, mask);

		auto& bone_flags = mdl_data->m_bone_flags.at(bone_index);

		const auto backup_bone_flags = bone_flags;

		bone_flags = 0u;

		orig_standard_blending_rules(ecx, edx, mdl_data, a1, a2, a3, mask);

		bone_flags = backup_bone_flags;
	}

	void __fastcall packet_start(
		const std::uintptr_t ecx, const std::uintptr_t edx, const int in_seq, const int out_acked
	) {
		if (!valve::g_local_player
			|| !valve::g_local_player->alive())
			return orig_packet_start(ecx, edx, in_seq, out_acked);

		auto& sented_cmds = g_context->sented_cmds();
		if (sented_cmds.empty()
			|| std::find(sented_cmds.rbegin(), sented_cmds.rend(), out_acked) == sented_cmds.rend())
			return;

		sented_cmds.erase(
			std::remove_if(
				sented_cmds.begin(), sented_cmds.end(),
				[&](const int cmd_number) {
					return cmd_number < out_acked;
				}
			),
			sented_cmds.end()
					);

		return orig_packet_start(ecx, edx, in_seq, out_acked);
	}

	void __fastcall packet_end(const std::uintptr_t ecx, const std::uintptr_t edx) {
		if (!valve::g_local_player
			|| valve::g_client_state->m_server_tick != valve::g_client_state->m_delta_tick)
			return orig_packet_end(ecx, edx);

		const auto& local_data = hacks::g_eng_pred->local_data( ).at( valve::g_client_state->m_last_cmd_ack % 150 );
		if (local_data.m_spawn_time == valve::g_local_player->spawn_time()
			&& local_data.m_shift_amount > 0
			&& local_data.m_tick_base > valve::g_local_player->tick_base()
			&& (local_data.m_tick_base - valve::g_local_player->tick_base()) <= 17)
			valve::g_local_player->tick_base() = local_data.m_tick_base + 1;

		orig_packet_end(ecx, edx);
	}

	void __fastcall run_command(const std::uintptr_t ecx, const std::uintptr_t edx, valve::c_player* const player, valve::user_cmd_t* user_cmd, valve::c_move_helper* move_helper) {
		if (!player
			|| !valve::g_local_player
			|| player != valve::g_local_player)
			return orig_run_command(ecx, edx, player, user_cmd, move_helper);		

		orig_run_command(ecx, edx, player, user_cmd, move_helper);
	}

	void __fastcall physics_simulate(valve::c_player* const ecx, const std::uintptr_t edx) {
		if (ecx != valve::g_local_player
			|| !ecx->alive()
			|| ecx->sim_tick() == valve::g_global_vars->m_tick_count)
			return orig_physics_simulate(ecx, edx);

		const auto& user_cmd = ecx->cmd_context().m_user_cmd;
		if (user_cmd.m_tick >= std::numeric_limits< int >::max()) {
			ecx->sim_tick() = valve::g_global_vars->m_tick_count;

			return hacks::g_eng_pred->net_vars().at(user_cmd.m_number % 150).store(user_cmd.m_number);
		}

		if (user_cmd.m_number == (valve::g_client_state->m_last_cmd_ack + valve::g_client_state->m_choked_cmds + 1))
			ecx->velocity_modifier( ) = hacks::g_eng_pred->net_velocity_modifier( );

		hacks::g_eng_pred->net_vars().at((user_cmd.m_number - 1) % 150).restore(user_cmd.m_number - 1);

		const auto backup_tick_base = ecx->tick_base();
		const auto& local_data = hacks::g_eng_pred->local_data().at(user_cmd.m_number % 150);
		if (local_data.m_spawn_time == ecx->spawn_time() && local_data.m_override_tick_base)
			ecx->tick_base() = local_data.m_adjusted_tick_base;

		const auto weapon = ecx->weapon();
		if (weapon
			&& weapon->item_index() == valve::e_item_index::revolver
			&& weapon->postpone_fire_ready_time() == std::numeric_limits< float >::max())
			weapon->postpone_fire_ready_time() = hacks::g_eng_pred->postpone_fire_ready_time();

		orig_physics_simulate(ecx, edx);

		if (weapon
			&& weapon->item_index() == valve::e_item_index::revolver
			&& weapon->postpone_fire_ready_time() == std::numeric_limits< float >::max())
			weapon->postpone_fire_ready_time() = hacks::g_eng_pred->postpone_fire_ready_time();

		if (local_data.m_spawn_time == ecx->spawn_time()
			&& local_data.m_override_tick_base && local_data.m_restore_tick_base)
			ecx->tick_base() = backup_tick_base + ecx->tick_base() - local_data.m_adjusted_tick_base;

		hacks::g_eng_pred->net_vars().at(user_cmd.m_number % 150).store(user_cmd.m_number);
	}

	void __fastcall on_latch_interpolated_vars(
		valve::c_player* const ecx, const std::uintptr_t edx, const int flags
	) {
		if (!valve::g_engine->in_game()
			|| ecx != valve::g_local_player)
			return orig_on_latch_interpolated_vars(ecx, edx, flags);

		const auto backup_sim_time = ecx->sim_time();

		const auto& local_data = hacks::g_eng_pred->local_data( ).at( valve::g_client_state->m_last_cmd_ack % 150 );
		if (local_data.m_spawn_time == valve::g_local_player->spawn_time()
			&& local_data.m_shift_amount > 0)
			ecx->sim_time() += valve::to_time(local_data.m_shift_amount);

		orig_on_latch_interpolated_vars(ecx, edx, flags);

		ecx->sim_time() = backup_sim_time;
	}

	bool __fastcall should_interpolate(valve::c_player* const ecx, const std::uintptr_t edx) {
		if (ecx != valve::g_local_player
			|| valve::g_client_state->m_last_cmd_out != hacks::g_exploits->recharge_cmd())
			return orig_should_interpolate(ecx, edx);

		auto& var_mapping = ecx->var_mapping();

		for (int i{}; i < var_mapping.m_interpolated_entries; ++i)
			var_mapping.m_entries.at(i).m_needs_to_interpolate = false;

		return false;
	}

	bool __fastcall write_user_cmd_delta_to_buffer(
		const std::uintptr_t ecx, const std::uintptr_t edx,
		const int slot, valve::bf_write_t* const buffer,
		int from, int to, const bool is_new_cmd
	) {
		if (!valve::g_local_player)
			return orig_write_user_cmd_delta_to_buffer(ecx, edx, slot, buffer, from, to, is_new_cmd);

		const auto move_msg = reinterpret_cast<valve::move_msg_t*>(
			*reinterpret_cast<std::uintptr_t*>(
				reinterpret_cast<std::uintptr_t>(_AddressOfReturnAddress()) - sizeof(std::uintptr_t)
				) - 0x58u
			);

		if (hacks::g_exploits->cur_shift_amount()
			|| valve::g_client_state->m_last_cmd_out == hacks::g_exploits->recharge_cmd()) {
			if (from == -1) {
				if (valve::g_client_state->m_last_cmd_out == hacks::g_exploits->recharge_cmd()) {
					move_msg->m_new_cmds = 1;
					move_msg->m_backup_cmds = 0;

					const auto next_cmd_number = valve::g_client_state->m_choked_cmds + valve::g_client_state->m_last_cmd_out + 1;

					for (to = next_cmd_number - move_msg->m_new_cmds + 1; to <= next_cmd_number; ++to) {
						if (!orig_write_user_cmd_delta_to_buffer(ecx, edx, slot, buffer, from, to, true))
							break;

						from = to;
					}
				}							
				else
					hacks::g_exploits->handle_other_shift(ecx, edx, slot, buffer, from, to, move_msg);
			}

			return true;
		}

		if (from == -1) {
			const auto v86 = std::min(move_msg->m_new_cmds + hacks::g_exploits->ticks_allowed(), (((*valve::g_game_rules)->is_valve_ds() == true) ? 8 : 16));

			int v69{};

			const auto v70 = v86 - move_msg->m_new_cmds;
			if (v70 >= 0)
				v69 = v70;

			hacks::g_exploits->ticks_allowed() = v69;
		}
	
		return orig_write_user_cmd_delta_to_buffer(ecx, edx, slot, buffer, from, to, is_new_cmd);
	}

	void __cdecl velocity_modifier(valve::recv_proxy_data_t* const data, valve::c_entity* const entity, void* const out) {
		orig_velocity_modifier(data, entity, out);

		if (!valve::g_local_player
			|| entity->index() != valve::g_local_player->index()
			|| data->m_value.m_float == hacks::g_eng_pred->net_velocity_modifier())
			return;

		hacks::g_eng_pred->net_velocity_modifier() = data->m_value.m_float;

		valve::g_prediction->m_prev_ack_had_errors = true;
	}

	void __fastcall modify_eye_pos(valve::anim_state_t* const ecx, const std::uintptr_t edx, vec3_t& eye_pos) {
		const auto backup_smooth_height_valid = ecx->m_smooth_height_valid;
		const auto backup_camera_smooth_height = ecx->m_camera_shooth_height;

		ecx->m_smooth_height_valid = false;

		orig_modify_eye_pos(ecx, edx, eye_pos);

		ecx->m_smooth_height_valid = backup_smooth_height_valid;
		ecx->m_camera_shooth_height = backup_camera_smooth_height;

		eye_pos.z -= ecx->m_player->view_offset().z - std::floor(ecx->m_player->view_offset().z);
	}

	bool __fastcall process_temp_entities(const std::uintptr_t ecx, const std::uintptr_t edx, const std::uintptr_t msg) {
		const auto backup_max_clients = valve::g_client_state->m_max_clients;

		valve::g_client_state->m_max_clients = 1;

		const auto ret = orig_process_temp_entities(ecx, edx, msg);

		valve::g_client_state->m_max_clients = backup_max_clients;

		valve::g_engine->fire_events();

		return ret;
	}

	void __fastcall level_shutdown(const std::uintptr_t ecx, const std::uintptr_t edx) {
		hacks::g_eng_pred->reset_on_spawn();

		hacks::g_shots->elements().clear();

		hacks::g_visuals->next_update() = valve::g_global_vars->m_real_time + 10.f;
				
		hacks::g_exploits->recharge_cmd() = 0;
		hacks::g_exploits->ticks_allowed() = 0;
		hacks::g_exploits->cur_shift_amount() = 0;
		hacks::g_exploits->next_shift_amount() = 0;

		g_context->freeze_time() = false;

		orig_level_shutdown(ecx, edx);
	}

	bool __cdecl glow_effect_spectator(
		valve::c_player* const player, valve::c_player* const local,
		int& style, vec3_t& clr, float& alpha_from, float& alpha_to,
		float& time_from, float& time_to, bool& animate
	) {
		if (!sdk::g_config_system->glow
			|| player->friendly()
			|| player == valve::g_local_player)
			return orig_glow_effect_spectator(
				player, local, style, clr, alpha_from, alpha_to,
				time_from, time_to, animate
			);

		style = 0;

		clr.x = sdk::g_config_system->glow_color[0u];
		clr.y = sdk::g_config_system->glow_color[1u];
		clr.z = sdk::g_config_system->glow_color[2u];

		alpha_to = sdk::g_config_system->glow_color[3u];

		return true;
	}

	void __fastcall get_clr_modulation(
		valve::material_t* const ecx, const std::uintptr_t edx,
		float* const r, float* const g, float* const b
	) {
		orig_get_clr_modulation(ecx, edx, r, g, b);

		const char* texture_group_name = ecx->texture_group();
		if (strstr(texture_group_name, xorstr_("World textures"))) {
			*r = sdk::g_config_system->world_color[0u];
			*g = sdk::g_config_system->world_color[1u];
			*b = sdk::g_config_system->world_color[2u];
		}

		if (strstr(texture_group_name, xorstr_("StaticProp textures"))) {
			*r = sdk::g_config_system->props_color[0u];
			*g = sdk::g_config_system->props_color[1u];
			*b = sdk::g_config_system->props_color[2u];
		}
		
		if (strstr(texture_group_name, xorstr_("SkyBox textures"))) {
			if (sdk::g_config_system->remove_skybox)
				*r = *g = *b = 0.f;
			else {
				*r = sdk::g_config_system->skybox_color[0u];
				*g = sdk::g_config_system->skybox_color[1u];
				*b = sdk::g_config_system->skybox_color[2u];
			}
		}
	}

	float __fastcall get_alpha_modulation(
		valve::material_t* const ecx, const std::uintptr_t edx
	) {		
		const char* texture_group_name = ecx->texture_group();
		if (strstr(texture_group_name, xorstr_("World textures")))		
			return sdk::g_config_system->world_color[3u];

		if (strstr(texture_group_name, xorstr_("StaticProp textures")))
			return sdk::g_config_system->props_color[3u];

		if (strstr(texture_group_name, xorstr_("SkyBox textures")))
			return sdk::g_config_system->skybox_color[3u];

		return orig_get_alpha_modulation(ecx, edx);
	}

	void __fastcall draw_models(
		void* ecx, void* edx,
		int a2, int a3, int a4, int a5, int a6, char a7
	) {
		valve::model_list_by_type_t* model_list_array = (valve::model_list_by_type_t*)a4;
		for (int i = 0; i < a3; ++i)
		{
			valve::model_list_by_type_t& list = model_list_array[i];
			if (!list.count)
				continue;

			for (int j = 0; j < list.count; ++j)
			{
				valve::render_model_info_t* model = &list.render_models[j];

				valve::c_renderable* renderable = (valve::c_renderable*)model->entry.renderable;

				valve::c_entity* entity = (valve::c_entity*)renderable->get_i_unknown_entity()->get_base_entity();
				if (entity)
				{
					valve::e_class_id class_id = entity->client_class()->m_class_id;

					auto valid_entity = [&]
					{
						switch (class_id)
						{
						case valve::e_class_id::ragdoll:
						case valve::e_class_id::chiken:
						case valve::e_class_id::hostage:
						case valve::e_class_id::hostage_carriable_prop:
							return false;
							break;
						}
						return true;
					};

					if (!valid_entity())
						continue;
				}

				model->diffuse_modulation_r = sdk::g_config_system->props_color[0];
				model->diffuse_modulation_g = sdk::g_config_system->props_color[1];
				model->diffuse_modulation_b = sdk::g_config_system->props_color[2];
				model->diffuse_modulation_a = sdk::g_config_system->props_color[3];
			}
		}

		orig_draw_models(ecx, edx, a2, a3, a4, a5, a6, a7);
	}

	bool __stdcall is_using_static_prop_debug_modes() {
		return true;
	}

	int __fastcall list_leaves_in_box(
		const std::uintptr_t ecx, const std::uintptr_t edx,
		const vec3_t& mins, const vec3_t& maxs, const uint16_t* const list, const int max
	) {
		if (!sdk::g_config_system->disable_model_occlusion
			|| *reinterpret_cast<std::uintptr_t*>(_AddressOfReturnAddress()) != g_context->addresses().m_ret_insert_into_tree)
			return orig_list_leaves_in_box(ecx, edx, mins, maxs, list, max);

		auto info = *(valve::renderable_info_t**)((uintptr_t)_AddressOfReturnAddress() + 0x14);

		if (!info)
			return orig_list_leaves_in_box(ecx, edx, mins, maxs, list, max);

		if (!info->m_renderable)
			return orig_list_leaves_in_box(ecx, edx, mins, maxs, list, max);

		const auto entity = info->m_renderable->get_i_unknown_entity()->get_base_entity();
		if (!entity
			|| !entity->is_player())
			return orig_list_leaves_in_box(ecx, edx, mins, maxs, list, max);

		info->m_flags &= ~0x100;
		info->m_flags2 |= 0xC0;

		return orig_list_leaves_in_box(
			ecx, edx,
			vec3_t{ -16384.f, -16384.f, -16384.f },
			vec3_t{ 16384.f, 16384.f, 16384.f },
			list, max
		);
	}

	bool __fastcall override_cfg(
		valve::c_mat_system* const ecx, const std::uintptr_t edx,
		valve::mat_sys_cfg_t* const cfg, const bool update
	) {
		if (sdk::g_config_system->full_bright)
			cfg->m_fullbright = true;

		return orig_override_cfg(ecx, edx, cfg, update);
	}

	const char* __fastcall get_foreign_fallback_font_name(const std::uintptr_t ecx, const std::uintptr_t edx) {
		if (hacks::g_visuals->surface_font_name().empty())
			return orig_get_foreign_fallback_font_name(ecx, edx);

		return hacks::g_visuals->surface_font_name().c_str();
	}

	bool __fastcall fog_enabled(const std::uintptr_t ecx, const std::uintptr_t edx) {
		if (sdk::g_config_system->remove_fog)
			return false;

		return orig_fog_enabled(ecx, edx);
	}

	void __fastcall override_view(
		const std::uintptr_t ecx, const std::uintptr_t edx,
		valve::view_setup_t* const setup
	) {
		hacks::g_visuals->on_override_view(setup);

		orig_override_view(ecx, edx, setup);
	}

	int __fastcall do_post_screen_space_effects(
		const std::uintptr_t ecx, const std::uintptr_t edx,
		valve::view_setup_t* const setup
	) {
		hacks::g_visuals->on_post_screen_effects();

		return orig_do_post_screen_space_effects(ecx, edx, setup);
	}

	int __fastcall weapon_debug_spread_show_get_int(valve::cvar_t* const ecx, const std::uintptr_t edx) {
		if (sdk::g_config_system->crosshair
			&& *reinterpret_cast<std::uintptr_t*>(_AddressOfReturnAddress()) == g_context->addresses().m_crosshair_ret
			&& valve::g_local_player && !valve::g_local_player->scoped())
			return 3;

		return orig_weapon_debug_spread_show_get_int(ecx, edx);
	}

	void __fastcall calc_view(
		valve::c_player* const ecx, const std::uintptr_t edx,
		vec3_t& origin, qangle_t& angles, float& z_near, float& z_far, float& fov
	) {
		if (ecx != valve::g_local_player)
			return orig_calc_view(ecx, edx, origin, angles, z_near, z_far, fov);

		hacks::g_visuals->on_calc_view(ecx, edx, origin, angles, z_near, z_far, fov);
	}

	qangle_t* __fastcall get_eye_angles(valve::c_player* const ecx, const std::uintptr_t edx) {
		if (ecx != valve::g_local_player
			|| *reinterpret_cast<std::uintptr_t*>(_AddressOfReturnAddress()) == g_context->addresses().m_ret_to_fire_bullet
			|| *reinterpret_cast<std::uintptr_t*>(_AddressOfReturnAddress()) == g_context->addresses().m_ret_set_first_person_viewangles)
			return orig_get_eye_angles(ecx, edx);

		return &hacks::g_eng_pred->local_data().at(g_context->last_sent_cmd_number() % 150).m_user_cmd.m_view_angles;
	}

	float __fastcall aspect_ratio(const std::uintptr_t ecx, const std::uintptr_t edx, int width, int height) {
		if (!sdk::g_config_system->aspect_ratio)
			return orig_aspect_ratio(ecx, edx, width, height);

		return orig_aspect_ratio(ecx, edx, sdk::g_config_system->width, sdk::g_config_system->height);
	}

	bool __fastcall svc_msg_voice_data(std::uintptr_t ecx, std::uintptr_t edx, valve::c_svc_msg_voice_data* msg) {
		if (!msg)
			return orig_svc_msg_voice_data(ecx, edx, msg);

		if (!valve::g_local_player
			|| valve::g_engine->local_player_index() == msg->m_client + 1) {
			return orig_svc_msg_voice_data(ecx, edx, msg);
		}

		auto voice_data = msg->get_data();
		const auto raw_data = voice_data.raw_data();
		if (!raw_data)
			return orig_svc_msg_voice_data(ecx, edx, msg);

		const auto& data = *(valve::shared_esp_data_t*)(raw_data);

		if (data.m_unique_key == std::stoi(xorstr_("1469"))) {
			const auto index = data.m_player_idx;

			const auto player = static_cast<valve::c_player*>(
				valve::g_entity_list->find_entity(index)
				);

			if (player
				&& player->alive()
				&& player->dormant()) {
				hacks::g_dormant_esp->m_sound_players[index - 1].m_receive_time = valve::g_global_vars->m_real_time;
				hacks::g_dormant_esp->m_sound_players[index - 1].m_origin = vec3_t(data.m_x, data.m_y, data.m_z);
			}
		}	

		return orig_svc_msg_voice_data(ecx, edx, msg);
	}

	void event_listener_t::fire_game_event(valve::game_event_t* const event) {
		hacks::g_shots->on_new_event(event);
	}
}