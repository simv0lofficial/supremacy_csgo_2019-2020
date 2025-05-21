#include "../../supremacy.hpp"
#include "../../dependencies/imgui/settings.hpp"

namespace supremacy {
	void c_menu::init(IDirect3DDevice9* const pDevice) {
		if (this->m_initialized)
			return;

		ui::CreateContext();
		auto io = ui::GetIO();
		auto style = &ui::GetStyle();

		style->WindowRounding = 0.f;
		style->AntiAliasedLines = false;
		style->AntiAliasedFill = false;
		style->ScrollbarRounding = 0.f;
		style->ScrollbarSize = 6.f;
		style->WindowPadding = ImVec2(0, 0);
		style->Colors[ImGuiCol_ScrollbarBg] = ImColor(45, 45, 45, 255);
		style->Colors[ImGuiCol_ScrollbarGrab] = ImColor(65, 65, 65, 255);

		settings::m_menu_color[0] = sdk::g_config_system->menu_color[0];
		settings::m_menu_color[1] = sdk::g_config_system->menu_color[1];
		settings::m_menu_color[2] = sdk::g_config_system->menu_color[2];
		settings::m_menu_color[3] = sdk::g_config_system->menu_color[3];
		this->m_is_opened = settings::m_is_opened = false;		

		IDirect3DSwapChain9* pChain = nullptr;
		D3DPRESENT_PARAMETERS pp = {};
		D3DDEVICE_CREATION_PARAMETERS param = {};
		pDevice->GetCreationParameters(&param);
		pDevice->GetSwapChain(0, &pChain);

		if (pChain)
			pChain->GetPresentParameters(&pp);

		ImGui_ImplWin32_Init(param.hFocusWindow);
		ImGui_ImplDX9_Init(pDevice);

		static const ImWchar ranges[] =	{
			0x0020u, 0x00ffu,
			0x0400u, 0x052fu,
			0x2de0u, 0x2dffu,
			0xa640u, 0xa69fu,
			0xe000u, 0xe226u,
			0u,
		};

		ImFontConfig cfg;
		cfg.RasterizerFlags = 1 << 7 | 1 << 4;
		cfg.GlyphRanges = ranges;

		io.Fonts->AddFontFromMemoryTTF(main_font, 291576, 12.f, &cfg, get_font_glyph_ranges(io.Fonts));
		io.Fonts->AddFontFromMemoryTTF(main_font, 291576, 12.f, &cfg, get_font_glyph_ranges(io.Fonts));
		io.Fonts->AddFontFromMemoryTTF(keybinds_font, 25600, 10.f, &cfg, get_font_glyph_ranges(io.Fonts));
		io.Fonts->AddFontDefault();

		ImGuiFreeType::BuildFontAtlas(io.Fonts, 0x00);

		this->m_initialized = true;
	}

	void c_menu::draw_begin(IDirect3DDevice9* const device)	{
		static std::once_flag fl;
		std::call_once(fl, [device]() {
			g_menu->init(device);
		});

		if (!this->m_initialized)
			return;

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ui::NewFrame();
	}

	void c_menu::draw()	{
		this->handle();

		if (!m_is_opened || ui::GetStyle().Alpha == 0.f)
			return;

		const auto& screen_size = ui::GetIO().DisplaySize;

		ui::SetNextWindowSizeConstraints(ImVec2(490 + 8 + 4 + 2 + 3 + 4 + 5, 556 + 7), ImVec2(screen_size.x - 2, screen_size.y - 2));
		ui::Begin("supremacy", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MainWindow);
		ui::TabButton("aimbot", &this->m_current_tab, 0, 6);
		ui::TabButton("anti-aim", &this->m_current_tab, 1, 6);
		ui::TabButton("visuals", &this->m_current_tab, 2, 6);
		ui::TabButton("misc", &this->m_current_tab, 3, 6);
		ui::TabButton("config", &this->m_current_tab, 4, 6);
		ui::TabButton("skins", &this->m_current_tab, 5, 6);

		const auto child_size = ImVec2(ui::GetWindowSize().x / 2 - 36 + 4, ui::GetWindowSize().y - 80 - 27);
		static auto calc_pos = [](int num) -> ImVec2 {
			if (num == 1)
				return ImVec2(ui::GetWindowPos().x - 4 + 26 + (ui::GetWindowSize().x / 2 - 36) * num + 20 * num, ui::GetWindowPos().y + 52 + 27);
			else
				return ImVec2(ui::GetWindowPos().x + 26 + (ui::GetWindowSize().x / 2 - 36) * num + 20 * num, ui::GetWindowPos().y + 52 + 27);
		};

		if (this->m_current_tab == 0) {
			ui::SetNextWindowPos(calc_pos(0));
			ui::BeginChild("aimbot", child_size);
			{
				ui::Checkbox("enabled", &sdk::g_config_system->enabled);
				ui::Keybind("##enabled_key", &sdk::g_config_system->enabled_key, &sdk::g_config_system->enabled_key_style);
				ui::SingleSelect("weapon selection", &sdk::g_config_system->weapon_selection, { "pistols", "heavy pistols", "sub-machine guns", "rifles", "auto snipers", "scout", "awp", "shotguns", "machine guns"/*, "taser"*/ });
				if (sdk::g_config_system->weapon_selection == 0) {
					ui::MultiSelect("target hitbox", &sdk::g_config_system->pistols_target_hitbox, { "head", "chest", "body", "arms", "legs", "feet" });
					ui::MultiSelect("multi-point", &sdk::g_config_system->pistols_multi_point, { "head", "chest", "body", "arms", "legs", "feet" });
					if (sdk::g_config_system->pistols_multi_point[0]
						|| sdk::g_config_system->pistols_multi_point[1]
						|| sdk::g_config_system->pistols_multi_point[2]
						|| sdk::g_config_system->pistols_multi_point[3]
						|| sdk::g_config_system->pistols_multi_point[4]
						|| sdk::g_config_system->pistols_multi_point[5]
						) {
						ui::Checkbox("static scale", &sdk::g_config_system->pistols_static_scale);
						if (sdk::g_config_system->pistols_static_scale) {
							if (sdk::g_config_system->pistols_multi_point[0])
								ui::SliderInt("head scale", &sdk::g_config_system->pistols_head_scale, 0, 100, "%d%%");

							ui::SliderInt("body scale", &sdk::g_config_system->pistols_body_scale, 0, 100, "%d%%");
						}
					}
					ui::Checkbox("automatic fire", &sdk::g_config_system->pistols_automatic_fire);
					ui::Checkbox("silent aim", &sdk::g_config_system->pistols_silent_aim);
					ui::SliderInt("minimum hitchance", &sdk::g_config_system->pistols_minimum_hitchance, 1, 99, "%d%%");
					ui::SliderInt("minimum damage", &sdk::g_config_system->pistols_minimum_damage, 1, 100, "%dhp");
					ui::Checkbox("automatic penetration", &sdk::g_config_system->pistols_automatic_penetration);
					if (sdk::g_config_system->pistols_automatic_penetration)
						ui::SliderInt("penetration minimum damage", &sdk::g_config_system->pistols_penetration_minimum_damage, 1, 100, "%dhp");
					ui::Text("override minimum damage");
					ui::Keybind("##override_minimum_damage_key", &sdk::g_config_system->override_minimum_damage_key, &sdk::g_config_system->override_minimum_damage_key_style);
					ui::SliderInt("##pistols_override_minimum_damage", &sdk::g_config_system->pistols_override_minimum_damage, 1, 100, "%dhp");
					ui::SliderInt("maximum fov", &sdk::g_config_system->pistols_maximum_fov, 1, 180, "%d°%");
				}
				else if (sdk::g_config_system->weapon_selection == 1) {
					ui::MultiSelect("target hitbox", &sdk::g_config_system->heavy_pistols_target_hitbox, { "head", "chest", "body", "arms", "legs", "feet" });
					ui::MultiSelect("multi-point", &sdk::g_config_system->heavy_pistols_multi_point, { "head", "chest", "body", "arms", "legs", "feet" });
					if (sdk::g_config_system->heavy_pistols_multi_point[0]
						|| sdk::g_config_system->heavy_pistols_multi_point[1]
						|| sdk::g_config_system->heavy_pistols_multi_point[2]
						|| sdk::g_config_system->heavy_pistols_multi_point[3]
						|| sdk::g_config_system->heavy_pistols_multi_point[4]
						|| sdk::g_config_system->heavy_pistols_multi_point[5]
						) {
						ui::Checkbox("static scale", &sdk::g_config_system->heavy_pistols_static_scale);
						if (sdk::g_config_system->heavy_pistols_static_scale) {
							if (sdk::g_config_system->heavy_pistols_multi_point[0])
								ui::SliderInt("head scale", &sdk::g_config_system->heavy_pistols_head_scale, 0, 100, "%d%%");

							ui::SliderInt("body scale", &sdk::g_config_system->heavy_pistols_body_scale, 0, 100, "%d%%");
						}
					}
					ui::Checkbox("automatic fire", &sdk::g_config_system->heavy_pistols_automatic_fire);
					ui::Checkbox("silent aim", &sdk::g_config_system->heavy_pistols_silent_aim);
					ui::SliderInt("minimum hitchance", &sdk::g_config_system->heavy_pistols_minimum_hitchance, 1, 99, "%d%%");
					ui::SliderInt("minimum damage", &sdk::g_config_system->heavy_pistols_minimum_damage, 1, 100, "%dhp");
					ui::Checkbox("automatic penetration", &sdk::g_config_system->heavy_pistols_automatic_penetration);
					if (sdk::g_config_system->heavy_pistols_automatic_penetration)
						ui::SliderInt("penetration minimum damage", &sdk::g_config_system->heavy_pistols_penetration_minimum_damage, 1, 100, "%dhp");
					ui::Text("override minimum damage");
					ui::Keybind("##override_minimum_damage_key", &sdk::g_config_system->override_minimum_damage_key, &sdk::g_config_system->override_minimum_damage_key_style);
					ui::SliderInt("##heavy_pistols_override_minimum_damage", &sdk::g_config_system->heavy_pistols_override_minimum_damage, 1, 100, "%dhp");
					ui::SliderInt("maximum fov", &sdk::g_config_system->heavy_pistols_maximum_fov, 1, 180, "%d°%");
				}
				else if (sdk::g_config_system->weapon_selection == 2) {
					ui::MultiSelect("target hitbox", &sdk::g_config_system->sub_machine_guns_target_hitbox, { "head", "chest", "body", "arms", "legs", "feet" });
					ui::MultiSelect("multi-point", &sdk::g_config_system->sub_machine_guns_multi_point, { "head", "chest", "body", "arms", "legs", "feet" });
					if (sdk::g_config_system->sub_machine_guns_multi_point[0]
						|| sdk::g_config_system->sub_machine_guns_multi_point[1]
						|| sdk::g_config_system->sub_machine_guns_multi_point[2]
						|| sdk::g_config_system->sub_machine_guns_multi_point[3]
						|| sdk::g_config_system->sub_machine_guns_multi_point[4]
						|| sdk::g_config_system->sub_machine_guns_multi_point[5]
						) {
						ui::Checkbox("static scale", &sdk::g_config_system->sub_machine_guns_static_scale);
						if (sdk::g_config_system->sub_machine_guns_static_scale) {
							if (sdk::g_config_system->sub_machine_guns_multi_point[0])
								ui::SliderInt("head scale", &sdk::g_config_system->sub_machine_guns_head_scale, 0, 100, "%d%%");

							ui::SliderInt("body scale", &sdk::g_config_system->sub_machine_guns_body_scale, 0, 100, "%d%%");
						}
					}
					ui::Checkbox("automatic fire", &sdk::g_config_system->sub_machine_guns_automatic_fire);
					ui::Checkbox("silent aim", &sdk::g_config_system->sub_machine_guns_silent_aim);
					ui::SliderInt("minimum hitchance", &sdk::g_config_system->sub_machine_guns_minimum_hitchance, 1, 99, "%d%%");
					ui::SliderInt("minimum damage", &sdk::g_config_system->sub_machine_guns_minimum_damage, 1, 100, "%dhp");
					ui::Checkbox("automatic penetration", &sdk::g_config_system->sub_machine_guns_automatic_penetration);
					if (sdk::g_config_system->sub_machine_guns_automatic_penetration)
						ui::SliderInt("penetration minimum damage", &sdk::g_config_system->sub_machine_guns_penetration_minimum_damage, 1, 100, "%dhp");
					ui::Text("override minimum damage");
					ui::Keybind("##override_minimum_damage_key", &sdk::g_config_system->override_minimum_damage_key, &sdk::g_config_system->override_minimum_damage_key_style);
					ui::SliderInt("##sub_machine_guns_override_minimum_damage", &sdk::g_config_system->sub_machine_guns_override_minimum_damage, 1, 100, "%dhp");
					ui::SliderInt("maximum fov", &sdk::g_config_system->sub_machine_guns_maximum_fov, 1, 180, "%d°%");
				}
				else if (sdk::g_config_system->weapon_selection == 3) {
					ui::MultiSelect("target hitbox", &sdk::g_config_system->rifles_target_hitbox, { "head", "chest", "body", "arms", "legs", "feet" });
					ui::MultiSelect("multi-point", &sdk::g_config_system->rifles_multi_point, { "head", "chest", "body", "arms", "legs", "feet" });
					if (sdk::g_config_system->rifles_multi_point[0]
						|| sdk::g_config_system->rifles_multi_point[1]
						|| sdk::g_config_system->rifles_multi_point[2]
						|| sdk::g_config_system->rifles_multi_point[3]
						|| sdk::g_config_system->rifles_multi_point[4]
						|| sdk::g_config_system->rifles_multi_point[5]
						) {
						ui::Checkbox("static scale", &sdk::g_config_system->rifles_static_scale);
						if (sdk::g_config_system->rifles_static_scale) {
							if (sdk::g_config_system->rifles_multi_point[0])
								ui::SliderInt("head scale", &sdk::g_config_system->rifles_head_scale, 0, 100, "%d%%");

							ui::SliderInt("body scale", &sdk::g_config_system->rifles_body_scale, 0, 100, "%d%%");
						}
					}
					ui::Checkbox("automatic fire", &sdk::g_config_system->rifles_automatic_fire);
					ui::Checkbox("silent aim", &sdk::g_config_system->rifles_silent_aim);
					ui::SliderInt("minimum hitchance", &sdk::g_config_system->rifles_minimum_hitchance, 1, 99, "%d%%");
					ui::SliderInt("minimum damage", &sdk::g_config_system->rifles_minimum_damage, 1, 100, "%dhp");
					ui::Checkbox("automatic penetration", &sdk::g_config_system->rifles_automatic_penetration);
					if (sdk::g_config_system->rifles_automatic_penetration)
						ui::SliderInt("penetration minimum damage", &sdk::g_config_system->rifles_penetration_minimum_damage, 1, 100, "%dhp");
					ui::Text("override minimum damage");
					ui::Keybind("##override_minimum_damage_key", &sdk::g_config_system->override_minimum_damage_key, &sdk::g_config_system->override_minimum_damage_key_style);
					ui::SliderInt("##rifles_override_minimum_damage", &sdk::g_config_system->rifles_override_minimum_damage, 1, 100, "%dhp");
					ui::SliderInt("maximum fov", &sdk::g_config_system->rifles_maximum_fov, 1, 180, "%d°%");
				}
				else if (sdk::g_config_system->weapon_selection == 4) {
					ui::MultiSelect("target hitbox", &sdk::g_config_system->auto_snipers_target_hitbox, { "head", "chest", "body", "arms", "legs", "feet" });
					ui::MultiSelect("multi-point", &sdk::g_config_system->auto_snipers_multi_point, { "head", "chest", "body", "arms", "legs", "feet" });
					if (sdk::g_config_system->auto_snipers_multi_point[0]
						|| sdk::g_config_system->auto_snipers_multi_point[1]
						|| sdk::g_config_system->auto_snipers_multi_point[2]
						|| sdk::g_config_system->auto_snipers_multi_point[3]
						|| sdk::g_config_system->auto_snipers_multi_point[4]
						|| sdk::g_config_system->auto_snipers_multi_point[5]
						) {
						ui::Checkbox("static scale", &sdk::g_config_system->auto_snipers_static_scale);
						if (sdk::g_config_system->auto_snipers_static_scale) {
							if (sdk::g_config_system->auto_snipers_multi_point[0])
								ui::SliderInt("head scale", &sdk::g_config_system->auto_snipers_head_scale, 0, 100, "%d%%");

							ui::SliderInt("body scale", &sdk::g_config_system->auto_snipers_body_scale, 0, 100, "%d%%");
						}
					}
					ui::Checkbox("automatic fire", &sdk::g_config_system->auto_snipers_automatic_fire);
					ui::Checkbox("silent aim", &sdk::g_config_system->auto_snipers_silent_aim);
					ui::SliderInt("minimum hitchance", &sdk::g_config_system->auto_snipers_minimum_hitchance, 1, 99, "%d%%");
					ui::SliderInt("minimum damage", &sdk::g_config_system->auto_snipers_minimum_damage, 1, 100, "%dhp");
					ui::Checkbox("automatic penetration", &sdk::g_config_system->auto_snipers_automatic_penetration);
					if (sdk::g_config_system->auto_snipers_automatic_penetration)
						ui::SliderInt("penetration minimum damage", &sdk::g_config_system->auto_snipers_penetration_minimum_damage, 1, 100, "%dhp");
					ui::Text("override minimum damage");
					ui::Keybind("##override_minimum_damage_key", &sdk::g_config_system->override_minimum_damage_key, &sdk::g_config_system->override_minimum_damage_key_style);
					ui::SliderInt("##auto_snipers_override_minimum_damage", &sdk::g_config_system->auto_snipers_override_minimum_damage, 1, 100, "%dhp");
					ui::Checkbox("automatic scope", &sdk::g_config_system->auto_snipers_automatic_scope);
					ui::SliderInt("maximum fov", &sdk::g_config_system->auto_snipers_maximum_fov, 1, 180, "%d°%");
				}
				else if (sdk::g_config_system->weapon_selection == 5) {
					ui::MultiSelect("target hitbox", &sdk::g_config_system->scout_target_hitbox, { "head", "chest", "body", "arms", "legs", "feet" });
					ui::MultiSelect("multi-point", &sdk::g_config_system->scout_multi_point, { "head", "chest", "body", "arms", "legs", "feet" });
					if (sdk::g_config_system->scout_multi_point[0]
						|| sdk::g_config_system->scout_multi_point[1]
						|| sdk::g_config_system->scout_multi_point[2]
						|| sdk::g_config_system->scout_multi_point[3]
						|| sdk::g_config_system->scout_multi_point[4]
						|| sdk::g_config_system->scout_multi_point[5]
						) {
						ui::Checkbox("static scale", &sdk::g_config_system->scout_static_scale);
						if (sdk::g_config_system->scout_static_scale) {
							if (sdk::g_config_system->scout_multi_point[0])
								ui::SliderInt("head scale", &sdk::g_config_system->scout_head_scale, 0, 100, "%d%%");

							ui::SliderInt("body scale", &sdk::g_config_system->scout_body_scale, 0, 100, "%d%%");
						}
					}
					ui::Checkbox("automatic fire", &sdk::g_config_system->scout_automatic_fire);
					ui::Checkbox("silent aim", &sdk::g_config_system->scout_silent_aim);
					ui::SliderInt("minimum hitchance", &sdk::g_config_system->scout_minimum_hitchance, 1, 99, "%d%%");
					ui::SliderInt("minimum damage", &sdk::g_config_system->scout_minimum_damage, 1, 100, "%dhp");
					ui::Checkbox("automatic penetration", &sdk::g_config_system->scout_automatic_penetration);
					if (sdk::g_config_system->scout_automatic_penetration)
						ui::SliderInt("penetration minimum damage", &sdk::g_config_system->scout_penetration_minimum_damage, 1, 100, "%dhp");
					ui::Text("override minimum damage");
					ui::Keybind("##override_minimum_damage_key", &sdk::g_config_system->override_minimum_damage_key, &sdk::g_config_system->override_minimum_damage_key_style);
					ui::SliderInt("##scout_override_minimum_damage", &sdk::g_config_system->scout_override_minimum_damage, 1, 100, "%dhp");
					ui::Checkbox("automatic scope", &sdk::g_config_system->scout_automatic_scope);
					ui::SliderInt("maximum fov", &sdk::g_config_system->scout_maximum_fov, 1, 180, "%d°%");
				}
				else if (sdk::g_config_system->weapon_selection == 6) {
					ui::MultiSelect("target hitbox", &sdk::g_config_system->awp_target_hitbox, { "head", "chest", "body", "arms", "legs", "feet" });
					ui::MultiSelect("multi-point", &sdk::g_config_system->awp_multi_point, { "head", "chest", "body", "arms", "legs", "feet" });
					if (sdk::g_config_system->awp_multi_point[0]
						|| sdk::g_config_system->awp_multi_point[1]
						|| sdk::g_config_system->awp_multi_point[2]
						|| sdk::g_config_system->awp_multi_point[3]
						|| sdk::g_config_system->awp_multi_point[4]
						|| sdk::g_config_system->awp_multi_point[5]
						) {
						ui::Checkbox("static scale", &sdk::g_config_system->awp_static_scale);
						if (sdk::g_config_system->awp_static_scale) {
							if (sdk::g_config_system->awp_multi_point[0])
								ui::SliderInt("head scale", &sdk::g_config_system->awp_head_scale, 0, 100, "%d%%");

							ui::SliderInt("body scale", &sdk::g_config_system->awp_body_scale, 0, 100, "%d%%");
						}
					}
					ui::Checkbox("automatic fire", &sdk::g_config_system->awp_automatic_fire);
					ui::Checkbox("silent aim", &sdk::g_config_system->awp_silent_aim);
					ui::SliderInt("minimum hitchance", &sdk::g_config_system->awp_minimum_hitchance, 1, 99, "%d%%");
					ui::SliderInt("minimum damage", &sdk::g_config_system->awp_minimum_damage, 1, 100, "%dhp");
					ui::Checkbox("automatic penetration", &sdk::g_config_system->awp_automatic_penetration);
					if (sdk::g_config_system->awp_automatic_penetration)
						ui::SliderInt("penetration minimum damage", &sdk::g_config_system->awp_penetration_minimum_damage, 1, 100, "%dhp");
					ui::Text("override minimum damage");
					ui::Keybind("##override_minimum_damage_key", &sdk::g_config_system->override_minimum_damage_key, &sdk::g_config_system->override_minimum_damage_key_style);
					ui::SliderInt("##awp_override_minimum_damage", &sdk::g_config_system->awp_override_minimum_damage, 1, 100, "%dhp");
					ui::Checkbox("automatic scope", &sdk::g_config_system->awp_automatic_scope);
					ui::SliderInt("maximum fov", &sdk::g_config_system->awp_maximum_fov, 1, 180, "%d°%");
				}
				else if (sdk::g_config_system->weapon_selection == 7) {
					ui::MultiSelect("target hitbox", &sdk::g_config_system->shotguns_target_hitbox, { "head", "chest", "body", "arms", "legs", "feet" });
					ui::MultiSelect("multi-point", &sdk::g_config_system->shotguns_multi_point, { "head", "chest", "body", "arms", "legs", "feet" });
					if (sdk::g_config_system->shotguns_multi_point[0]
						|| sdk::g_config_system->shotguns_multi_point[1]
						|| sdk::g_config_system->shotguns_multi_point[2]
						|| sdk::g_config_system->shotguns_multi_point[3]
						|| sdk::g_config_system->shotguns_multi_point[4]
						|| sdk::g_config_system->shotguns_multi_point[5]
						) {
						ui::Checkbox("static scale", &sdk::g_config_system->shotguns_static_scale);
						if (sdk::g_config_system->shotguns_static_scale) {
							if (sdk::g_config_system->shotguns_multi_point[0])
								ui::SliderInt("head scale", &sdk::g_config_system->shotguns_head_scale, 0, 100, "%d%%");

							ui::SliderInt("body scale", &sdk::g_config_system->shotguns_body_scale, 0, 100, "%d%%");
						}
					}
					ui::Checkbox("automatic fire", &sdk::g_config_system->shotguns_automatic_fire);
					ui::Checkbox("silent aim", &sdk::g_config_system->shotguns_silent_aim);
					ui::SliderInt("minimum hitchance", &sdk::g_config_system->shotguns_minimum_hitchance, 1, 99, "%d%%");
					ui::SliderInt("minimum damage", &sdk::g_config_system->shotguns_minimum_damage, 1, 100, "%dhp");
					ui::Checkbox("automatic penetration", &sdk::g_config_system->shotguns_automatic_penetration);
					if (sdk::g_config_system->shotguns_automatic_penetration)
						ui::SliderInt("penetration minimum damage", &sdk::g_config_system->shotguns_penetration_minimum_damage, 1, 100, "%dhp");
					ui::Text("override minimum damage");
					ui::Keybind("##override_minimum_damage_key", &sdk::g_config_system->override_minimum_damage_key, &sdk::g_config_system->override_minimum_damage_key_style);
					ui::SliderInt("##shotguns_override_minimum_damage", &sdk::g_config_system->shotguns_override_minimum_damage, 1, 100, "%dhp");
					ui::SliderInt("maximum fov", &sdk::g_config_system->shotguns_maximum_fov, 1, 180, "%d°%");
				}
				else if (sdk::g_config_system->weapon_selection == 8) {
					ui::MultiSelect("target hitbox", &sdk::g_config_system->machine_guns_target_hitbox, { "head", "chest", "body", "arms", "legs", "feet" });
					ui::MultiSelect("multi-point", &sdk::g_config_system->machine_guns_multi_point, { "head", "chest", "body", "arms", "legs", "feet" });
					if (sdk::g_config_system->machine_guns_multi_point[0]
						|| sdk::g_config_system->machine_guns_multi_point[1]
						|| sdk::g_config_system->machine_guns_multi_point[2]
						|| sdk::g_config_system->machine_guns_multi_point[3]
						|| sdk::g_config_system->machine_guns_multi_point[4]
						|| sdk::g_config_system->machine_guns_multi_point[5]
						) {
						ui::Checkbox("static scale", &sdk::g_config_system->machine_guns_static_scale);
						if (sdk::g_config_system->machine_guns_static_scale) {
							if (sdk::g_config_system->machine_guns_multi_point[0])
								ui::SliderInt("head scale", &sdk::g_config_system->machine_guns_head_scale, 0, 100, "%d%%");

							ui::SliderInt("body scale", &sdk::g_config_system->machine_guns_body_scale, 0, 100, "%d%%");
						}
					}
					ui::Checkbox("automatic fire", &sdk::g_config_system->machine_guns_automatic_fire);
					ui::Checkbox("silent aim", &sdk::g_config_system->machine_guns_silent_aim);
					ui::SliderInt("minimum hitchance", &sdk::g_config_system->machine_guns_minimum_hitchance, 1, 99, "%d%%");
					ui::SliderInt("minimum damage", &sdk::g_config_system->machine_guns_minimum_damage, 1, 100, "%dhp");
					ui::Checkbox("automatic penetration", &sdk::g_config_system->machine_guns_automatic_penetration);
					if (sdk::g_config_system->machine_guns_automatic_penetration)
						ui::SliderInt("penetration minimum damage", &sdk::g_config_system->machine_guns_penetration_minimum_damage, 1, 100, "%dhp");
					ui::Text("override minimum damage");
					ui::Keybind("##override_minimum_damage_key", &sdk::g_config_system->override_minimum_damage_key, &sdk::g_config_system->override_minimum_damage_key_style);
					ui::SliderInt("##machine_guns_override_minimum_damage", &sdk::g_config_system->machine_guns_override_minimum_damage, 1, 100, "%dhp");
					ui::SliderInt("maximum fov", &sdk::g_config_system->machine_guns_maximum_fov, 1, 180, "%d°%");
				}
				else if (sdk::g_config_system->weapon_selection == 9) {
					ui::MultiSelect("target hitbox", &sdk::g_config_system->taser_target_hitbox, { "head", "chest", "body", "arms", "legs", "feet" });
					ui::MultiSelect("multi-point", &sdk::g_config_system->taser_multi_point, { "head", "chest", "body", "arms", "legs", "feet" });
					if (sdk::g_config_system->taser_multi_point[0]
						|| sdk::g_config_system->taser_multi_point[1]
						|| sdk::g_config_system->taser_multi_point[2]
						|| sdk::g_config_system->taser_multi_point[3]
						|| sdk::g_config_system->taser_multi_point[4]
						|| sdk::g_config_system->taser_multi_point[5]
						) {
						ui::Checkbox("static scale", &sdk::g_config_system->taser_static_scale);
						if (sdk::g_config_system->taser_static_scale) {
							if (sdk::g_config_system->taser_multi_point[0])
								ui::SliderInt("head scale", &sdk::g_config_system->taser_head_scale, 0, 100, "%d%%");

							ui::SliderInt("body scale", &sdk::g_config_system->taser_body_scale, 0, 100, "%d%%");
						}
					}
					ui::Checkbox("automatic fire", &sdk::g_config_system->taser_automatic_fire);
					ui::Checkbox("silent aim", &sdk::g_config_system->taser_silent_aim);
					ui::SliderInt("minimum hitchance", &sdk::g_config_system->taser_minimum_hitchance, 1, 99, "%d%%");
					ui::SliderInt("minimum damage", &sdk::g_config_system->taser_minimum_damage, 1, 100, "%dhp");
					ui::Checkbox("automatic penetration", &sdk::g_config_system->taser_automatic_penetration);
					if (sdk::g_config_system->taser_automatic_penetration)
						ui::SliderInt("penetration minimum damage", &sdk::g_config_system->taser_penetration_minimum_damage, 1, 100, "%dhp");
					ui::Text("override minimum damage");
					ui::Keybind("##override_minimum_damage_key", &sdk::g_config_system->override_minimum_damage_key, &sdk::g_config_system->override_minimum_damage_key_style);
					ui::SliderInt("##taser_override_minimum_damage", &sdk::g_config_system->taser_override_minimum_damage, 1, 100, "%dhp");
					ui::SliderInt("maximum fov", &sdk::g_config_system->taser_maximum_fov, 1, 180, "%d°%");
				}

				ui::Checkbox("log misses due to spread", &sdk::g_config_system->log_misses_due_to_spread);
			}
			ui::EndChild();

			ui::SetNextWindowPos(calc_pos(1));
			ui::BeginChild("other", child_size);
			{
				if (sdk::g_config_system->weapon_selection == 0) {
					ui::Checkbox("remove recoil", &sdk::g_config_system->pistols_remove_recoil);
					ui::SliderInt("accuracy boost", &sdk::g_config_system->pistols_accuracy_boost, 0, 100, "%d%%");
					ui::Checkbox("quick stop", &sdk::g_config_system->pistols_quick_stop);
					if (sdk::g_config_system->pistols_quick_stop) {
						ui::SingleSelect("##type", &sdk::g_config_system->pistols_type, { "full", "auto" });
						ui::Checkbox("between shots", &sdk::g_config_system->pistols_between_shots);
					}

					if (ui::Button(g_context->cvars().m_cl_lagcompensation->get_bool() ? "disable client lag compensation" : "enable client lag compensation")) {
						if (valve::g_engine->in_game()
							&& valve::g_local_player) {
							g_context->switch_lagcompensation_state() = true;
						}
						else
							g_context->cvars().m_cl_lagcompensation->set_int(!g_context->cvars().m_cl_lagcompensation->get_bool());
					}

					if (g_context->cvars().m_cl_lagcompensation->get_bool())
						ui::SingleSelect("lag compensation", &sdk::g_config_system->pistols_lag_compensation, { "off", "normal", "high" });
					ui::Checkbox("anti-aim correction", &sdk::g_config_system->anti_aim_correction);
					ui::MultiSelect("body aim conditions", &sdk::g_config_system->pistols_body_aim_conditions, { "prefer", "unsafe", "lethal", "lethal x2", "double shot", "overlapping desync", "max misses", "forward", "sideways" });
					ui::Keybind("##body_aim_key", &sdk::g_config_system->body_aim_key, &sdk::g_config_system->body_aim_key_style);
					if (sdk::g_config_system->pistols_body_aim_conditions[6])
						ui::SliderInt("##max_misses_body_aim", &sdk::g_config_system->pistols_max_misses_body_aim, 1, 5, "%d miss");
					ui::MultiSelect("safe point conditions", &sdk::g_config_system->pistols_safe_point_conditions, { "prefer", "lethal", "lethal x2", "standing", "targeting limbs", "max misses", "forward", "sideways" });
					ui::Keybind("##safe_point_key", &sdk::g_config_system->safe_point_key, &sdk::g_config_system->safe_point_key_style);
					if (sdk::g_config_system->safe_point_key
						|| sdk::g_config_system->pistols_safe_point_conditions[0]
						|| sdk::g_config_system->pistols_safe_point_conditions[1]
						|| sdk::g_config_system->pistols_safe_point_conditions[2]
						|| sdk::g_config_system->pistols_safe_point_conditions[3]
						|| sdk::g_config_system->pistols_safe_point_conditions[4]
						|| sdk::g_config_system->pistols_safe_point_conditions[5]
						|| sdk::g_config_system->pistols_safe_point_conditions[6]
						|| sdk::g_config_system->pistols_safe_point_conditions[7]
						) {
						ui::SingleSelect("type", &sdk::g_config_system->pistols_safe_point_type, { "normal", "strict" });
						if (sdk::g_config_system->pistols_safe_point_conditions[5])
							ui::SliderInt("##max_misses_safe_point", &sdk::g_config_system->pistols_max_misses_safe_point, 1, 5, "%d miss");
					}
					ui::Checkbox("double tap", &sdk::g_config_system->pistols_double_tap);
					ui::Keybind("##double_tap_key", &sdk::g_config_system->double_tap_key, &sdk::g_config_system->double_tap_key_style);
					ui::SliderInt("max shift amount", &sdk::g_config_system->pistols_max_shift_amount, 6, 15, "%d ticks");
				}
				else if (sdk::g_config_system->weapon_selection == 1) {
					ui::Checkbox("remove recoil", &sdk::g_config_system->heavy_pistols_remove_recoil);
					ui::SliderInt("accuracy boost", &sdk::g_config_system->heavy_pistols_accuracy_boost, 0, 100, "%d%%");
					ui::Checkbox("quick stop", &sdk::g_config_system->heavy_pistols_quick_stop);
					if (sdk::g_config_system->heavy_pistols_quick_stop) {
						ui::SingleSelect("##type", &sdk::g_config_system->heavy_pistols_type, { "full", "auto" });
						ui::Checkbox("between shots", &sdk::g_config_system->heavy_pistols_between_shots);
					}

					if (ui::Button(g_context->cvars().m_cl_lagcompensation->get_bool() ? "disable client lag compensation" : "enable client lag compensation")) {
						if (valve::g_engine->in_game()
							&& valve::g_local_player) {
							g_context->switch_lagcompensation_state() = true;
						}
						else
							g_context->cvars().m_cl_lagcompensation->set_int(!g_context->cvars().m_cl_lagcompensation->get_bool());
					}

					if (g_context->cvars().m_cl_lagcompensation->get_bool())
						ui::SingleSelect("lag compensation", &sdk::g_config_system->heavy_pistols_lag_compensation, { "off", "normal", "high" });
					ui::Checkbox("anti-aim correction", &sdk::g_config_system->anti_aim_correction);
					ui::MultiSelect("body aim conditions", &sdk::g_config_system->heavy_pistols_body_aim_conditions, { "prefer", "unsafe", "lethal", "lethal x2", "double shot", "overlapping desync", "max misses", "forward", "sideways" });
					ui::Keybind("##body_aim_key", &sdk::g_config_system->body_aim_key, &sdk::g_config_system->body_aim_key_style);
					if (sdk::g_config_system->heavy_pistols_body_aim_conditions[6])
						ui::SliderInt("##max_misses_body_aim", &sdk::g_config_system->heavy_pistols_max_misses_body_aim, 1, 5, "%d miss");
					ui::MultiSelect("safe point conditions", &sdk::g_config_system->heavy_pistols_safe_point_conditions, { "prefer", "lethal", "lethal x2", "standing", "targeting limbs", "max misses", "forward", "sideways" });
					ui::Keybind("##safe_point_key", &sdk::g_config_system->safe_point_key, &sdk::g_config_system->safe_point_key_style);
					if (sdk::g_config_system->safe_point_key
						|| sdk::g_config_system->heavy_pistols_safe_point_conditions[0]
						|| sdk::g_config_system->heavy_pistols_safe_point_conditions[1]
						|| sdk::g_config_system->heavy_pistols_safe_point_conditions[2]
						|| sdk::g_config_system->heavy_pistols_safe_point_conditions[3]
						|| sdk::g_config_system->heavy_pistols_safe_point_conditions[4]
						|| sdk::g_config_system->heavy_pistols_safe_point_conditions[5]
						|| sdk::g_config_system->heavy_pistols_safe_point_conditions[6]
						|| sdk::g_config_system->heavy_pistols_safe_point_conditions[7]
						) {
						ui::SingleSelect("type", &sdk::g_config_system->heavy_pistols_safe_point_type, { "normal", "strict" });
						if (sdk::g_config_system->heavy_pistols_safe_point_conditions[5])
							ui::SliderInt("##max_misses_safe_point", &sdk::g_config_system->heavy_pistols_max_misses_safe_point, 1, 5, "%d miss");
					}
					ui::Checkbox("double tap", &sdk::g_config_system->heavy_pistols_double_tap);
					ui::Keybind("##double_tap_key", &sdk::g_config_system->double_tap_key, &sdk::g_config_system->double_tap_key_style);
					ui::SliderInt("max shift amount", &sdk::g_config_system->heavy_pistols_max_shift_amount, 6, 15, "%d ticks");
				}
				else if (sdk::g_config_system->weapon_selection == 2) {
					ui::Checkbox("remove recoil", &sdk::g_config_system->sub_machine_guns_remove_recoil);
					ui::SliderInt("accuracy boost", &sdk::g_config_system->sub_machine_guns_accuracy_boost, 0, 100, "%d%%");
					ui::Checkbox("quick stop", &sdk::g_config_system->sub_machine_guns_quick_stop);
					if (sdk::g_config_system->sub_machine_guns_quick_stop) {
						ui::SingleSelect("##type", &sdk::g_config_system->sub_machine_guns_type, { "full", "auto" });
						ui::Checkbox("between shots", &sdk::g_config_system->sub_machine_guns_between_shots);
					}

					if (ui::Button(g_context->cvars().m_cl_lagcompensation->get_bool() ? "disable client lag compensation" : "enable client lag compensation")) {
						if (valve::g_engine->in_game()
							&& valve::g_local_player) {
							g_context->switch_lagcompensation_state() = true;
						}
						else
							g_context->cvars().m_cl_lagcompensation->set_int(!g_context->cvars().m_cl_lagcompensation->get_bool());
					}

					if (g_context->cvars().m_cl_lagcompensation->get_bool())
						ui::SingleSelect("lag compensation", &sdk::g_config_system->sub_machine_guns_lag_compensation, { "off", "normal", "high" });
					ui::Checkbox("anti-aim correction", &sdk::g_config_system->anti_aim_correction);
					ui::MultiSelect("body aim conditions", &sdk::g_config_system->sub_machine_guns_body_aim_conditions, { "prefer", "unsafe", "lethal", "lethal x2", "double shot", "overlapping desync", "max misses", "forward", "sideways" });
					ui::Keybind("##body_aim_key", &sdk::g_config_system->body_aim_key, &sdk::g_config_system->body_aim_key_style);
					if (sdk::g_config_system->sub_machine_guns_body_aim_conditions[6])
						ui::SliderInt("##max_misses_body_aim", &sdk::g_config_system->sub_machine_guns_max_misses_body_aim, 1, 5, "%d miss");
					ui::MultiSelect("safe point conditions", &sdk::g_config_system->sub_machine_guns_safe_point_conditions, { "prefer", "lethal", "lethal x2", "standing", "targeting limbs", "max misses", "forward", "sideways" });
					ui::Keybind("##safe_point_key", &sdk::g_config_system->safe_point_key, &sdk::g_config_system->safe_point_key_style);
					if (sdk::g_config_system->safe_point_key
						|| sdk::g_config_system->sub_machine_guns_safe_point_conditions[0]
						|| sdk::g_config_system->sub_machine_guns_safe_point_conditions[1]
						|| sdk::g_config_system->sub_machine_guns_safe_point_conditions[2]
						|| sdk::g_config_system->sub_machine_guns_safe_point_conditions[3]
						|| sdk::g_config_system->sub_machine_guns_safe_point_conditions[4]
						|| sdk::g_config_system->sub_machine_guns_safe_point_conditions[5]
						|| sdk::g_config_system->sub_machine_guns_safe_point_conditions[6]
						|| sdk::g_config_system->sub_machine_guns_safe_point_conditions[7]
						) {
						ui::SingleSelect("type", &sdk::g_config_system->sub_machine_guns_safe_point_type, { "normal", "strict" });
						if (sdk::g_config_system->sub_machine_guns_safe_point_conditions[5])
							ui::SliderInt("##max_misses_safe_point", &sdk::g_config_system->sub_machine_guns_max_misses_safe_point, 1, 5, "%d miss");
					}
					ui::Checkbox("double tap", &sdk::g_config_system->sub_machine_guns_double_tap);
					ui::Keybind("##double_tap_key", &sdk::g_config_system->double_tap_key, &sdk::g_config_system->double_tap_key_style);
					ui::SliderInt("max shift amount", &sdk::g_config_system->sub_machine_guns_max_shift_amount, 6, 15, "%d ticks");
				}
				else if (sdk::g_config_system->weapon_selection == 3) {
					ui::Checkbox("remove recoil", &sdk::g_config_system->rifles_remove_recoil);
					ui::SliderInt("accuracy boost", &sdk::g_config_system->rifles_accuracy_boost, 0, 100, "%d%%");
					ui::Checkbox("quick stop", &sdk::g_config_system->rifles_quick_stop);
					if (sdk::g_config_system->rifles_quick_stop) {
						ui::SingleSelect("##type", &sdk::g_config_system->rifles_type, { "full", "auto" });
						ui::Checkbox("between shots", &sdk::g_config_system->rifles_between_shots);
					}

					if (ui::Button(g_context->cvars().m_cl_lagcompensation->get_bool() ? "disable client lag compensation" : "enable client lag compensation")) {
						if (valve::g_engine->in_game()
							&& valve::g_local_player) {
							g_context->switch_lagcompensation_state() = true;
						}
						else
							g_context->cvars().m_cl_lagcompensation->set_int(!g_context->cvars().m_cl_lagcompensation->get_bool());
					}

					if (g_context->cvars().m_cl_lagcompensation->get_bool())
						ui::SingleSelect("lag compensation", &sdk::g_config_system->rifles_lag_compensation, { "off", "normal", "high" });
					ui::Checkbox("anti-aim correction", &sdk::g_config_system->anti_aim_correction);
					ui::MultiSelect("body aim conditions", &sdk::g_config_system->rifles_body_aim_conditions, { "prefer", "unsafe", "lethal", "lethal x2", "double shot", "overlapping desync", "max misses", "forward", "sideways" });
					ui::Keybind("##body_aim_key", &sdk::g_config_system->body_aim_key, &sdk::g_config_system->body_aim_key_style);
					if (sdk::g_config_system->rifles_body_aim_conditions[6])
						ui::SliderInt("##max_misses_body_aim", &sdk::g_config_system->rifles_max_misses_body_aim, 1, 5, "%d miss");
					ui::MultiSelect("safe point conditions", &sdk::g_config_system->rifles_safe_point_conditions, { "prefer", "lethal", "lethal x2", "standing", "targeting limbs", "max misses", "forward", "sideways" });
					ui::Keybind("##safe_point_key", &sdk::g_config_system->safe_point_key, &sdk::g_config_system->safe_point_key_style);
					if (sdk::g_config_system->safe_point_key
						|| sdk::g_config_system->rifles_safe_point_conditions[0]
						|| sdk::g_config_system->rifles_safe_point_conditions[1]
						|| sdk::g_config_system->rifles_safe_point_conditions[2]
						|| sdk::g_config_system->rifles_safe_point_conditions[3]
						|| sdk::g_config_system->rifles_safe_point_conditions[4]
						|| sdk::g_config_system->rifles_safe_point_conditions[5]
						|| sdk::g_config_system->rifles_safe_point_conditions[6]
						|| sdk::g_config_system->rifles_safe_point_conditions[7]
						) {
						ui::SingleSelect("type", &sdk::g_config_system->rifles_safe_point_type, { "normal", "strict" });
						if (sdk::g_config_system->rifles_safe_point_conditions[5])
							ui::SliderInt("##max_misses_safe_point", &sdk::g_config_system->rifles_max_misses_safe_point, 1, 5, "%d miss");
					}
					ui::Checkbox("double tap", &sdk::g_config_system->rifles_double_tap);
					ui::Keybind("##double_tap_key", &sdk::g_config_system->double_tap_key, &sdk::g_config_system->double_tap_key_style);
					ui::SliderInt("max shift amount", &sdk::g_config_system->rifles_max_shift_amount, 6, 15, "%d ticks");
				}
				else if (sdk::g_config_system->weapon_selection == 4) {
					ui::Checkbox("remove recoil", &sdk::g_config_system->auto_snipers_remove_recoil);
					ui::SliderInt("accuracy boost", &sdk::g_config_system->auto_snipers_accuracy_boost, 0, 100, "%d%%");
					ui::Checkbox("quick stop", &sdk::g_config_system->auto_snipers_quick_stop);
					if (sdk::g_config_system->auto_snipers_quick_stop) {
						ui::SingleSelect("##type", &sdk::g_config_system->auto_snipers_type, { "full", "auto" });
						ui::Checkbox("between shots", &sdk::g_config_system->auto_snipers_between_shots);
					}

					if (ui::Button(g_context->cvars().m_cl_lagcompensation->get_bool() ? "disable client lag compensation" : "enable client lag compensation")) {
						if (valve::g_engine->in_game()
							&& valve::g_local_player) {
							g_context->switch_lagcompensation_state() = true;
						}
						else
							g_context->cvars().m_cl_lagcompensation->set_int(!g_context->cvars().m_cl_lagcompensation->get_bool());
					}

					if (g_context->cvars().m_cl_lagcompensation->get_bool())
						ui::SingleSelect("lag compensation", &sdk::g_config_system->auto_snipers_lag_compensation, { "off", "normal", "high" });
					ui::Checkbox("anti-aim correction", &sdk::g_config_system->anti_aim_correction);
					ui::MultiSelect("body aim conditions", &sdk::g_config_system->auto_snipers_body_aim_conditions, { "prefer", "unsafe", "lethal", "lethal x2", "double shot", "overlapping desync", "max misses", "forward", "sideways" });
					ui::Keybind("##body_aim_key", &sdk::g_config_system->body_aim_key, &sdk::g_config_system->body_aim_key_style);
					if (sdk::g_config_system->auto_snipers_body_aim_conditions[6])
						ui::SliderInt("##max_misses_body_aim", &sdk::g_config_system->auto_snipers_max_misses_body_aim, 1, 5, "%d miss");
					ui::MultiSelect("safe point conditions", &sdk::g_config_system->auto_snipers_safe_point_conditions, { "prefer", "lethal", "lethal x2", "standing", "targeting limbs", "max misses", "forward", "sideways" });
					ui::Keybind("##safe_point_key", &sdk::g_config_system->safe_point_key, &sdk::g_config_system->safe_point_key_style);
					if (sdk::g_config_system->safe_point_key
						|| sdk::g_config_system->auto_snipers_safe_point_conditions[0]
						|| sdk::g_config_system->auto_snipers_safe_point_conditions[1]
						|| sdk::g_config_system->auto_snipers_safe_point_conditions[2]
						|| sdk::g_config_system->auto_snipers_safe_point_conditions[3]
						|| sdk::g_config_system->auto_snipers_safe_point_conditions[4]
						|| sdk::g_config_system->auto_snipers_safe_point_conditions[5]
						|| sdk::g_config_system->auto_snipers_safe_point_conditions[6]
						|| sdk::g_config_system->auto_snipers_safe_point_conditions[7]
						) {
						ui::SingleSelect("type", &sdk::g_config_system->auto_snipers_safe_point_type, { "normal", "strict" });
						if (sdk::g_config_system->auto_snipers_safe_point_conditions[5])
							ui::SliderInt("##max_misses_safe_point", &sdk::g_config_system->auto_snipers_max_misses_safe_point, 1, 5, "%d miss");
					}
					ui::Checkbox("double tap", &sdk::g_config_system->auto_snipers_double_tap);
					ui::Keybind("##double_tap_key", &sdk::g_config_system->double_tap_key, &sdk::g_config_system->double_tap_key_style);
					ui::SliderInt("max shift amount", &sdk::g_config_system->auto_snipers_max_shift_amount, 6, 15, "%d ticks");
				}
				else if (sdk::g_config_system->weapon_selection == 5) {
					ui::Checkbox("remove recoil", &sdk::g_config_system->scout_remove_recoil);
					ui::SliderInt("accuracy boost", &sdk::g_config_system->scout_accuracy_boost, 0, 100, "%d%%");
					ui::Checkbox("quick stop", &sdk::g_config_system->scout_quick_stop);
					if (sdk::g_config_system->scout_quick_stop) {
						ui::SingleSelect("##type", &sdk::g_config_system->scout_type, { "full", "auto" });
						ui::Checkbox("between shots", &sdk::g_config_system->scout_between_shots);
					}

					if (ui::Button(g_context->cvars().m_cl_lagcompensation->get_bool() ? "disable client lag compensation" : "enable client lag compensation")) {
						if (valve::g_engine->in_game()
							&& valve::g_local_player) {
							g_context->switch_lagcompensation_state() = true;
						}
						else
							g_context->cvars().m_cl_lagcompensation->set_int(!g_context->cvars().m_cl_lagcompensation->get_bool());
					}

					if (g_context->cvars().m_cl_lagcompensation->get_bool())
						ui::SingleSelect("lag compensation", &sdk::g_config_system->scout_lag_compensation, { "off", "normal", "high" });
					ui::Checkbox("anti-aim correction", &sdk::g_config_system->anti_aim_correction);
					ui::MultiSelect("body aim conditions", &sdk::g_config_system->scout_body_aim_conditions, { "prefer", "unsafe", "lethal", "lethal x2", "double shot", "overlapping desync", "max misses", "forward", "sideways" });
					ui::Keybind("##body_aim_key", &sdk::g_config_system->body_aim_key, &sdk::g_config_system->body_aim_key_style);
					if (sdk::g_config_system->scout_body_aim_conditions[6])
						ui::SliderInt("##max_misses_body_aim", &sdk::g_config_system->scout_max_misses_body_aim, 1, 5, "%d miss");
					ui::MultiSelect("safe point conditions", &sdk::g_config_system->scout_safe_point_conditions, { "prefer", "lethal", "lethal x2", "standing", "targeting limbs", "max misses", "forward", "sideways" });
					ui::Keybind("##safe_point_key", &sdk::g_config_system->safe_point_key, &sdk::g_config_system->safe_point_key_style);
					if (sdk::g_config_system->safe_point_key
						|| sdk::g_config_system->scout_safe_point_conditions[0]
						|| sdk::g_config_system->scout_safe_point_conditions[1]
						|| sdk::g_config_system->scout_safe_point_conditions[2]
						|| sdk::g_config_system->scout_safe_point_conditions[3]
						|| sdk::g_config_system->scout_safe_point_conditions[4]
						|| sdk::g_config_system->scout_safe_point_conditions[5]
						|| sdk::g_config_system->scout_safe_point_conditions[6]
						|| sdk::g_config_system->scout_safe_point_conditions[7]
						) {
						ui::SingleSelect("type", &sdk::g_config_system->scout_safe_point_type, { "normal", "strict" });
						if (sdk::g_config_system->scout_safe_point_conditions[5])
							ui::SliderInt("##max_misses_safe_point", &sdk::g_config_system->scout_max_misses_safe_point, 1, 5, "%d miss");
					}
					ui::Checkbox("double tap", &sdk::g_config_system->scout_double_tap);
					ui::Keybind("##double_tap_key", &sdk::g_config_system->double_tap_key, &sdk::g_config_system->double_tap_key_style);
					ui::SliderInt("max shift amount", &sdk::g_config_system->scout_max_shift_amount, 6, 15, "%d ticks");
				}
				else if (sdk::g_config_system->weapon_selection == 6) {
					ui::Checkbox("remove recoil", &sdk::g_config_system->awp_remove_recoil);
					ui::SliderInt("accuracy boost", &sdk::g_config_system->awp_accuracy_boost, 0, 100, "%d%%");
					ui::Checkbox("quick stop", &sdk::g_config_system->awp_quick_stop);
					if (sdk::g_config_system->awp_quick_stop) {
						ui::SingleSelect("##type", &sdk::g_config_system->awp_type, { "full", "auto" });
						ui::Checkbox("between shots", &sdk::g_config_system->awp_between_shots);
					}

					if (ui::Button(g_context->cvars().m_cl_lagcompensation->get_bool() ? "disable client lag compensation" : "enable client lag compensation")) {
						if (valve::g_engine->in_game()
							&& valve::g_local_player) {
							g_context->switch_lagcompensation_state() = true;
						}
						else
							g_context->cvars().m_cl_lagcompensation->set_int(!g_context->cvars().m_cl_lagcompensation->get_bool());
					}

					if (g_context->cvars().m_cl_lagcompensation->get_bool())
						ui::SingleSelect("lag compensation", &sdk::g_config_system->awp_lag_compensation, { "off", "normal", "high" });
					ui::Checkbox("anti-aim correction", &sdk::g_config_system->anti_aim_correction);
					ui::MultiSelect("body aim conditions", &sdk::g_config_system->awp_body_aim_conditions, { "prefer", "unsafe", "lethal", "lethal x2", "double shot", "overlapping desync", "max misses", "forward", "sideways" });
					ui::Keybind("##body_aim_key", &sdk::g_config_system->body_aim_key, &sdk::g_config_system->body_aim_key_style);
					if (sdk::g_config_system->awp_body_aim_conditions[6])
						ui::SliderInt("##max_misses_body_aim", &sdk::g_config_system->awp_max_misses_body_aim, 1, 5, "%d miss");
					ui::MultiSelect("safe point conditions", &sdk::g_config_system->awp_safe_point_conditions, { "prefer", "lethal", "lethal x2", "standing", "targeting limbs", "max misses", "forward", "sideways" });
					ui::Keybind("##safe_point_key", &sdk::g_config_system->safe_point_key, &sdk::g_config_system->safe_point_key_style);
					if (sdk::g_config_system->safe_point_key
						|| sdk::g_config_system->awp_safe_point_conditions[0]
						|| sdk::g_config_system->awp_safe_point_conditions[1]
						|| sdk::g_config_system->awp_safe_point_conditions[2]
						|| sdk::g_config_system->awp_safe_point_conditions[3]
						|| sdk::g_config_system->awp_safe_point_conditions[4]
						|| sdk::g_config_system->awp_safe_point_conditions[5]
						|| sdk::g_config_system->awp_safe_point_conditions[6]
						|| sdk::g_config_system->awp_safe_point_conditions[7]
						) {
						ui::SingleSelect("type", &sdk::g_config_system->awp_safe_point_type, { "normal", "strict" });
						if (sdk::g_config_system->awp_safe_point_conditions[5])
							ui::SliderInt("##max_misses_safe_point", &sdk::g_config_system->awp_max_misses_safe_point, 1, 5, "%d miss");
					}
					ui::Checkbox("double tap", &sdk::g_config_system->awp_double_tap);
					ui::Keybind("##double_tap_key", &sdk::g_config_system->double_tap_key, &sdk::g_config_system->double_tap_key_style);
					ui::SliderInt("max shift amount", &sdk::g_config_system->awp_max_shift_amount, 6, 15, "%d ticks");
				}
				else if (sdk::g_config_system->weapon_selection == 7) {
					ui::Checkbox("remove recoil", &sdk::g_config_system->shotguns_remove_recoil);
					ui::SliderInt("accuracy boost", &sdk::g_config_system->shotguns_accuracy_boost, 0, 100, "%d%%");
					ui::Checkbox("quick stop", &sdk::g_config_system->shotguns_quick_stop);
					if (sdk::g_config_system->shotguns_quick_stop) {
						ui::SingleSelect("##type", &sdk::g_config_system->shotguns_type, { "full", "auto" });
						ui::Checkbox("between shots", &sdk::g_config_system->shotguns_between_shots);
					}

					if (ui::Button(g_context->cvars().m_cl_lagcompensation->get_bool() ? "disable client lag compensation" : "enable client lag compensation")) {
						if (valve::g_engine->in_game()
							&& valve::g_local_player) {
							g_context->switch_lagcompensation_state() = true;
						}
						else
							g_context->cvars().m_cl_lagcompensation->set_int(!g_context->cvars().m_cl_lagcompensation->get_bool());
					}

					if (g_context->cvars().m_cl_lagcompensation->get_bool())
						ui::SingleSelect("lag compensation", &sdk::g_config_system->shotguns_lag_compensation, { "off", "normal", "high" });
					ui::Checkbox("anti-aim correction", &sdk::g_config_system->anti_aim_correction);
					ui::MultiSelect("body aim conditions", &sdk::g_config_system->shotguns_body_aim_conditions, { "prefer", "unsafe", "lethal", "lethal x2", "double shot", "overlapping desync", "max misses", "forward", "sideways" });
					ui::Keybind("##body_aim_key", &sdk::g_config_system->body_aim_key, &sdk::g_config_system->body_aim_key_style);
					if (sdk::g_config_system->shotguns_body_aim_conditions[6])
						ui::SliderInt("##max_misses_body_aim", &sdk::g_config_system->shotguns_max_misses_body_aim, 1, 5, "%d miss");
					ui::MultiSelect("safe point conditions", &sdk::g_config_system->shotguns_safe_point_conditions, { "prefer", "lethal", "lethal x2", "standing", "targeting limbs", "max misses", "forward", "sideways" });
					ui::Keybind("##safe_point_key", &sdk::g_config_system->safe_point_key, &sdk::g_config_system->safe_point_key_style);
					if (sdk::g_config_system->safe_point_key
						|| sdk::g_config_system->shotguns_safe_point_conditions[0]
						|| sdk::g_config_system->shotguns_safe_point_conditions[1]
						|| sdk::g_config_system->shotguns_safe_point_conditions[2]
						|| sdk::g_config_system->shotguns_safe_point_conditions[3]
						|| sdk::g_config_system->shotguns_safe_point_conditions[4]
						|| sdk::g_config_system->shotguns_safe_point_conditions[5]
						|| sdk::g_config_system->shotguns_safe_point_conditions[6]
						|| sdk::g_config_system->shotguns_safe_point_conditions[7]
						) {
						ui::SingleSelect("type", &sdk::g_config_system->shotguns_safe_point_type, { "normal", "strict" });
						if (sdk::g_config_system->shotguns_safe_point_conditions[5])
							ui::SliderInt("##max_misses_safe_point", &sdk::g_config_system->shotguns_max_misses_safe_point, 1, 5, "%d miss");
					}
					ui::Checkbox("double tap", &sdk::g_config_system->shotguns_double_tap);
					ui::Keybind("##double_tap_key", &sdk::g_config_system->double_tap_key, &sdk::g_config_system->double_tap_key_style);
					ui::SliderInt("max shift amount", &sdk::g_config_system->shotguns_max_shift_amount, 6, 15, "%d ticks");
				}
				else if (sdk::g_config_system->weapon_selection == 8) {
					ui::Checkbox("remove recoil", &sdk::g_config_system->machine_guns_remove_recoil);
					ui::SliderInt("accuracy boost", &sdk::g_config_system->machine_guns_accuracy_boost, 0, 100, "%d%%");
					ui::Checkbox("quick stop", &sdk::g_config_system->machine_guns_quick_stop);
					if (sdk::g_config_system->machine_guns_quick_stop) {
						ui::SingleSelect("##type", &sdk::g_config_system->machine_guns_type, { "full", "auto" });
						ui::Checkbox("between shots", &sdk::g_config_system->machine_guns_between_shots);
					}

					if (ui::Button(g_context->cvars().m_cl_lagcompensation->get_bool() ? "disable client lag compensation" : "enable client lag compensation")) {
						if (valve::g_engine->in_game()
							&& valve::g_local_player) {
							g_context->switch_lagcompensation_state() = true;
						}
						else
							g_context->cvars().m_cl_lagcompensation->set_int(!g_context->cvars().m_cl_lagcompensation->get_bool());
					}

					if (g_context->cvars().m_cl_lagcompensation->get_bool())
						ui::SingleSelect("lag compensation", &sdk::g_config_system->machine_guns_lag_compensation, { "off", "normal", "high" });
					ui::Checkbox("anti-aim correction", &sdk::g_config_system->anti_aim_correction);
					ui::MultiSelect("body aim conditions", &sdk::g_config_system->machine_guns_body_aim_conditions, { "prefer", "unsafe", "lethal", "lethal x2", "double shot", "overlapping desync", "max misses", "forward", "sideways" });
					ui::Keybind("##body_aim_key", &sdk::g_config_system->body_aim_key, &sdk::g_config_system->body_aim_key_style);
					if (sdk::g_config_system->machine_guns_body_aim_conditions[6])
						ui::SliderInt("##max_misses_body_aim", &sdk::g_config_system->machine_guns_max_misses_body_aim, 1, 5, "%d miss");
					ui::MultiSelect("safe point conditions", &sdk::g_config_system->machine_guns_safe_point_conditions, { "prefer", "lethal", "lethal x2", "standing", "targeting limbs", "max misses", "forward", "sideways" });
					ui::Keybind("##safe_point_key", &sdk::g_config_system->safe_point_key, &sdk::g_config_system->safe_point_key_style);
					if (sdk::g_config_system->safe_point_key
						|| sdk::g_config_system->machine_guns_safe_point_conditions[0]
						|| sdk::g_config_system->machine_guns_safe_point_conditions[1]
						|| sdk::g_config_system->machine_guns_safe_point_conditions[2]
						|| sdk::g_config_system->machine_guns_safe_point_conditions[3]
						|| sdk::g_config_system->machine_guns_safe_point_conditions[4]
						|| sdk::g_config_system->machine_guns_safe_point_conditions[5]
						|| sdk::g_config_system->machine_guns_safe_point_conditions[6]
						|| sdk::g_config_system->machine_guns_safe_point_conditions[7]
						) {
						ui::SingleSelect("type", &sdk::g_config_system->machine_guns_safe_point_type, { "normal", "strict" });
						if (sdk::g_config_system->machine_guns_safe_point_conditions[5])
							ui::SliderInt("##max_misses_safe_point", &sdk::g_config_system->machine_guns_max_misses_safe_point, 1, 5, "%d miss");
					}
					ui::Checkbox("double tap", &sdk::g_config_system->machine_guns_double_tap);
					ui::Keybind("##double_tap_key", &sdk::g_config_system->double_tap_key, &sdk::g_config_system->double_tap_key_style);
					ui::SliderInt("max shift amount", &sdk::g_config_system->machine_guns_max_shift_amount, 6, 15, "%d ticks");
				}
				else if (sdk::g_config_system->weapon_selection == 9) {
					ui::Checkbox("remove recoil", &sdk::g_config_system->taser_remove_recoil);
					ui::SliderInt("accuracy boost", &sdk::g_config_system->taser_accuracy_boost, 0, 100, "%d%%");
					ui::Checkbox("quick stop", &sdk::g_config_system->taser_quick_stop);
					if (sdk::g_config_system->taser_quick_stop) {
						ui::SingleSelect("##type", &sdk::g_config_system->taser_type, { "full", "auto" });
						ui::Checkbox("between shots", &sdk::g_config_system->taser_between_shots);
					}

					if (ui::Button(g_context->cvars().m_cl_lagcompensation->get_bool() ? "disable client lag compensation" : "enable client lag compensation")) {
						if (valve::g_engine->in_game()
							&& valve::g_local_player) {
							g_context->switch_lagcompensation_state() = true;
						}
						else
							g_context->cvars().m_cl_lagcompensation->set_int(!g_context->cvars().m_cl_lagcompensation->get_bool());
					}

					if (g_context->cvars().m_cl_lagcompensation->get_bool())
						ui::SingleSelect("lag compensation", &sdk::g_config_system->taser_lag_compensation, { "off", "normal", "high" });
					ui::Checkbox("anti-aim correction", &sdk::g_config_system->anti_aim_correction);
					ui::MultiSelect("body aim conditions", &sdk::g_config_system->taser_body_aim_conditions, { "prefer", "unsafe", "lethal", "lethal x2", "double shot", "overlapping desync", "max misses", "forward", "sideways" });
					ui::Keybind("##body_aim_key", &sdk::g_config_system->body_aim_key, &sdk::g_config_system->body_aim_key_style);
					if (sdk::g_config_system->taser_body_aim_conditions[6])
						ui::SliderInt("##max_misses_body_aim", &sdk::g_config_system->taser_max_misses_body_aim, 1, 5, "%d miss");
					ui::MultiSelect("safe point conditions", &sdk::g_config_system->taser_safe_point_conditions, { "prefer", "lethal", "lethal x2", "standing", "targeting limbs", "max misses", "forward", "sideways" });
					ui::Keybind("##safe_point_key", &sdk::g_config_system->safe_point_key, &sdk::g_config_system->safe_point_key_style);
					if (sdk::g_config_system->safe_point_key
						|| sdk::g_config_system->taser_safe_point_conditions[0]
						|| sdk::g_config_system->taser_safe_point_conditions[1]
						|| sdk::g_config_system->taser_safe_point_conditions[2]
						|| sdk::g_config_system->taser_safe_point_conditions[3]
						|| sdk::g_config_system->taser_safe_point_conditions[4]
						|| sdk::g_config_system->taser_safe_point_conditions[5]
						|| sdk::g_config_system->taser_safe_point_conditions[6]
						|| sdk::g_config_system->taser_safe_point_conditions[7]
						) {
						ui::SingleSelect("type", &sdk::g_config_system->taser_safe_point_type, { "normal", "strict" });
						if (sdk::g_config_system->taser_safe_point_conditions[5])
							ui::SliderInt("##max_misses_safe_point", &sdk::g_config_system->taser_max_misses_safe_point, 1, 5, "%d miss");
					}
					ui::Checkbox("double tap", &sdk::g_config_system->taser_double_tap);
					ui::Keybind("##double_tap_key", &sdk::g_config_system->double_tap_key, &sdk::g_config_system->double_tap_key_style);
					ui::SliderInt("max shift amount", &sdk::g_config_system->taser_max_shift_amount, 6, 15, "%d ticks");
				}
			}
			ui::EndChild();
		}

		if (this->m_current_tab == 1) {
			ui::SetNextWindowPos(calc_pos(0));
			ui::BeginChild("anti-aimbot angles", child_size);
			{
				ui::Checkbox("enabled", &sdk::g_config_system->enabled2);
				ui::SliderInt("pitch", &sdk::g_config_system->pitch, -89, 89, "%d°%");
				ui::SingleSelect("yaw base", &sdk::g_config_system->yaw_base, { "local view", "closest", "average", "fov" });
				ui::SliderInt("yaw", &sdk::g_config_system->yaw, -180, 180, "%d°%");
				ui::Checkbox("freestanding", &sdk::g_config_system->freestanding);
				ui::Keybind("##freestanding_key", &sdk::g_config_system->freestanding_key, &sdk::g_config_system->freestanding_key_style);
				ui::SliderInt("jitter range", &sdk::g_config_system->jitter_range, 0, 180, "%d°%");
				ui::SingleSelect("jitter switch", &sdk::g_config_system->jitter_switch, { "sending packet", "random" });
				ui::SliderInt("rotate range", &sdk::g_config_system->rotate_range, 0, 360, "%d°%");
				ui::SliderInt("rotate speed", &sdk::g_config_system->rotate_speed, 0, 100, "%d%%");
				ui::SingleSelect("body yaw", &sdk::g_config_system->body_yaw, { "off", "static", "jitter" });
				ui::Keybind("##body_yaw_key", &sdk::g_config_system->body_yaw_key, &sdk::g_config_system->body_yaw_key_style);
				if (sdk::g_config_system->body_yaw) {
					ui::SliderInt("yaw limit", &sdk::g_config_system->yaw_limit, 0, 58, "%d°%");
					ui::SliderInt("body lean", &sdk::g_config_system->body_lean, -180, 180, "%d°%");
					ui::SliderInt("inverted body lean", &sdk::g_config_system->inverted_body_lean, -180, 180, "%d°%");
					ui::SingleSelect("on shot side", &sdk::g_config_system->on_shot_side, { "off", "left", "right", "opposite", "switch" });
					if (sdk::g_config_system->body_yaw == 1)
						ui::SingleSelect("automatic side selection", &sdk::g_config_system->automatic_side_selection, { "off", "peek fake", "peek real" });
				}								
				ui::Text("manual left");
				ui::Keybind("##manual_left_key", &sdk::g_config_system->manual_left_key);
				ui::Text("manual right");
				ui::Keybind("##manual_right_key", &sdk::g_config_system->manual_right_key);
				ui::Text("manual back");
				ui::Keybind("##manual_back_key", &sdk::g_config_system->manual_back_key);
			}
			ui::EndChild();

			ui::SetNextWindowPos(calc_pos(1));
			ui::BeginChild("other", child_size);
			{
				ui::Checkbox("fake lag", &sdk::g_config_system->fake_lag);
				ui::SingleSelect("amount", &sdk::g_config_system->amount, { "dynamic", "maximal", "switch", "break lc" });
				ui::MultiSelect("conditions", &sdk::g_config_system->conditions, { "always", "on move", "in air" });
				ui::SliderInt("factor limit", &sdk::g_config_system->factor_limit, 0, 14, "%dticks");
				ui::Checkbox("force fake lag when peeking", &sdk::g_config_system->force_when_peeking);
				ui::Checkbox("hide shots", &sdk::g_config_system->hide_shots);
				ui::Keybind("##hide_shots_key", &sdk::g_config_system->hide_shots_key, &sdk::g_config_system->hide_shots_key_style);
			}
			ui::EndChild();
		}
		
		if (this->m_current_tab == 2) {
			ui::SetNextWindowPos(calc_pos(0));
			ui::BeginChild("player esp", ImVec2(ui::GetWindowSize().x / 2 - 36 + 4, ui::GetWindowSize().y / 2 - 80 - 27 + 65 + 31 - 30 - 30));
			{
				ui::Checkbox("teammates", &sdk::g_config_system->teammates);
				ui::Checkbox("dormant", &sdk::g_config_system->dormant);
				ui::Checkbox("bounding box", &sdk::g_config_system->bounding_box);
				color_picker("##bounding_box_color", sdk::g_config_system->bounding_box_color, false);
				ui::Checkbox("health bar", &sdk::g_config_system->health_bar);
				ui::Checkbox("name", &sdk::g_config_system->name);
				color_picker("##name_color", sdk::g_config_system->name_color);
				ui::Checkbox("flags", &sdk::g_config_system->flags);
				ui::Checkbox("weapon text", &sdk::g_config_system->weapon_text);
				ui::Checkbox("weapon icon", &sdk::g_config_system->weapon_icon);
				color_picker("##weapon_color", sdk::g_config_system->weapon_color);
				ui::Checkbox("ammo", &sdk::g_config_system->ammo);
				color_picker("##ammo_color", sdk::g_config_system->ammo_color);
				ui::Checkbox("distance", &sdk::g_config_system->distance);
				ui::Checkbox("glow", &sdk::g_config_system->glow);
				color_picker("##glow_color", sdk::g_config_system->glow_color);
				ui::Checkbox("hit marker", &sdk::g_config_system->hit_marker);
				ui::Checkbox("hit marker sound", &sdk::g_config_system->hit_marker_sound);
				ui::Checkbox("visualize sounds", &sdk::g_config_system->visualize_sounds);
				color_picker("##visualize_sounds_color", sdk::g_config_system->visualize_sounds_color);
				ui::Checkbox("money", &sdk::g_config_system->money);
				ui::Checkbox("skeleton", &sdk::g_config_system->skeleton);
				color_picker("##skeleton_color", sdk::g_config_system->skeleton_color);
				ui::Checkbox("out of fov arrow", &sdk::g_config_system->out_of_fow_arrow);
				color_picker("##out_of_fow_arrow_color", sdk::g_config_system->out_of_fow_arrow_color);
			}
			ui::EndChild();

			ui::SetNextWindowPos(ImVec2(ui::GetWindowPos().x - 4 + 26 + 4 + (ui::GetWindowSize().x / 2 - 36) * 0 + 20 * 0, ui::GetWindowPos().y + (ui::GetWindowSize().y / 2 - 36) + 52 + 27 - 60 + 68 - 30));
			ui::BeginChild("colored models", ImVec2(ui::GetWindowSize().x / 2 - 36 + 4, ui::GetWindowSize().y / 2 - 80 - 27 + 27 + 1 + 30));
			{
				ui::Checkbox("player", &sdk::g_config_system->player);
				color_picker("##player_color", sdk::g_config_system->player_color);
				if (sdk::g_config_system->player)
					ui::SingleSelect("##player_model", &sdk::g_config_system->player_model, { "regular", "solid", "glow", "metallic" });

				ui::Checkbox("player behind wall", &sdk::g_config_system->player_behind_wall);
				color_picker("##player_behind_wall_color", sdk::g_config_system->player_behind_wall_color);
				if (sdk::g_config_system->player_behind_wall)
					ui::SingleSelect("##player_behind_wall_model", &sdk::g_config_system->player_behind_wall_model, { "regular", "solid", "glow", "metallic" });

				ui::Checkbox("teammate", &sdk::g_config_system->teammate);
				color_picker("##teammate_color", sdk::g_config_system->teammate_color);
				if (sdk::g_config_system->teammate)
					ui::SingleSelect("##teammate_model", &sdk::g_config_system->teammate_model, { "regular", "solid", "glow", "metallic" });

				ui::Checkbox("teammate behind wall", &sdk::g_config_system->teammate_behind_wall);
				color_picker("##teammate_behind_wall_color", sdk::g_config_system->teammate_behind_wall_color);
				if (sdk::g_config_system->teammate_behind_wall)
					ui::SingleSelect("##teammate_behind_wall_model", &sdk::g_config_system->teammate_behind_wall_model, { "regular", "solid", "glow", "metallic" });

				ui::Checkbox("local player", &sdk::g_config_system->local_player);
				color_picker("##local_player_color", sdk::g_config_system->local_player_color);
				if (sdk::g_config_system->local_player)
					ui::SingleSelect("##local_player_model", &sdk::g_config_system->local_player_model, { "regular", "solid", "glow", "metallic" });

				ui::Checkbox("local player fake", &sdk::g_config_system->local_player_fake);
				color_picker("##local_player_fake_color", sdk::g_config_system->local_player_fake_color);
				if (sdk::g_config_system->local_player_fake)
					ui::SingleSelect("##local_player_fake_model", &sdk::g_config_system->local_player_fake_model, { "regular", "solid", "glow", "metallic" });
								
				ui::Checkbox("on shot (random crashes)", &sdk::g_config_system->on_shot);
				color_picker("##on_shot_color", sdk::g_config_system->on_shot_color);
				if (sdk::g_config_system->on_shot) {
					ui::SingleSelect("##on_shot_model", &sdk::g_config_system->on_shot_model, { "regular", "solid", "glow", "metallic" });
					ui::SliderInt("##on_shot_time", &sdk::g_config_system->on_shot_time, 0, 5, "%ds");
				}
				ui::Checkbox("ragdolls", &sdk::g_config_system->ragdolls);

				ui::Checkbox("hands", &sdk::g_config_system->hands);
				color_picker("##hands_color", sdk::g_config_system->hands_color);
				if (sdk::g_config_system->hands)
					ui::SingleSelect("##hands_model", &sdk::g_config_system->hands_model, { "regular", "solid", "glow", "metallic" });

				ui::Checkbox("weapon viewmodel", &sdk::g_config_system->weapon_viewmodel);
				color_picker("##weapon_viewmodel_color", sdk::g_config_system->weapon_viewmodel_color);
				if (sdk::g_config_system->weapon_viewmodel_color)
					ui::SingleSelect("##weapon_viewmodel_model", &sdk::g_config_system->weapon_viewmodel_model, { "regular", "solid", "glow", "metallic" });

				ui::Checkbox("weapons", &sdk::g_config_system->weapons);
				color_picker("##weapons_color", sdk::g_config_system->weapons_color);
				if (sdk::g_config_system->weapons)
					ui::SingleSelect("##weapons_model", &sdk::g_config_system->weapons_model, { "regular", "solid", "glow", "metallic" });
				ui::Checkbox("disable model occlusion", &sdk::g_config_system->disable_model_occlusion);
				ui::Checkbox("shadow", &sdk::g_config_system->shadow);
				color_picker("##shadow_color", sdk::g_config_system->shadow_color);
				if (sdk::g_config_system->shadow)
					ui::SingleSelect("##shadow_model", &sdk::g_config_system->shadow_model, { "regular", "solid", "glow", "metallic" });
			}
			ui::EndChild();

			ui::SetNextWindowPos(calc_pos(1));
			ui::BeginChild("other esp", ImVec2(ui::GetWindowSize().x / 2 - 36 + 4, ui::GetWindowSize().y / 2 - 80 - 27 + 65 + 31 - 30));
			{
				ui::Checkbox("radar", &sdk::g_config_system->radar);
				ui::SingleSelect("dropped weapons", &sdk::g_config_system->dropped_weapons, { "none", "text", "icon" });
				color_picker("dropped weapons color", sdk::g_config_system->dropped_weapons_color);
				ui::Checkbox("grenades", &sdk::g_config_system->grenades);
				color_picker("##grenades_color", sdk::g_config_system->grenades_color);
				ui::Checkbox("inaccuracy overlay", &sdk::g_config_system->inaccuracy_overlay);
				color_picker("##inaccuracy_overlay_color", sdk::g_config_system->inaccuracy_overlay_color);
				ui::Checkbox("recoil overlay", &sdk::g_config_system->recoil_overlay);
				ui::Checkbox("crosshair", &sdk::g_config_system->crosshair);
				ui::Checkbox("bomb", &sdk::g_config_system->bomb);
				color_picker("##bomb_color", sdk::g_config_system->bomb_color);
				ui::Checkbox("grenade trajectory", &sdk::g_config_system->grenade_trajectory);
				color_picker("##grenade_trajectory_color", sdk::g_config_system->grenade_trajectory_color);
				ui::Checkbox("grenade proximity warning", &sdk::g_config_system->grenade_proximity_warning);
				ui::Checkbox("spectators", &sdk::g_config_system->spectators);
				ui::Checkbox("penetration reticle", &sdk::g_config_system->penetration_reticle);
				ui::MultiSelect("feature indicators", &sdk::g_config_system->feature_indicators, { "lag comp", "double tap", "hide shots", "force body aim", "force safe point", "override minimum damage", "peek assistance", "manual anti-aimbot angles"});
				if (sdk::g_config_system->feature_indicators[7]) {
					ui::Text("manual anti-aimbot angles color");
					color_picker("##manual_anti_aimbot_angles_color", sdk::g_config_system->manual_anti_aimbot_angles_color);
				}
			}
			ui::EndChild();

			ui::SetNextWindowPos(ImVec2(ui::GetWindowPos().x - 4 + 26 + (ui::GetWindowSize().x / 2 - 36) * 1 + 20 * 1, ui::GetWindowPos().y + (ui::GetWindowSize().y / 2 - 36) + 52 + 27 - 60 + 68));
			ui::BeginChild("effects", ImVec2(ui::GetWindowSize().x / 2 - 36 + 4, ui::GetWindowSize().y / 2 - 80 - 27 + 27 + 1));
			{
				ui::Checkbox("remove flashbang effects", &sdk::g_config_system->remove_flashbang_effects);
				ui::Checkbox("remove smoke grenades", &sdk::g_config_system->remove_smoke_grenades);
				ui::Checkbox("remove fog", &sdk::g_config_system->remove_fog);
				ui::Checkbox("remove skybox", &sdk::g_config_system->remove_skybox);				
				ui::SingleSelect("visual recoil adjustment", &sdk::g_config_system->visual_recoil_adjustment, { "off", "remove shake", "remove all" });
				ui::Text("world color");
				color_picker("##world_color", sdk::g_config_system->world_color);
				ui::Text("props color");
				color_picker("##props_color", sdk::g_config_system->props_color);
				if (!sdk::g_config_system->remove_skybox) {
					ui::Text("skybox color");
					color_picker("##skybox_color", sdk::g_config_system->skybox_color);
				}
				ui::Checkbox("full bright", &sdk::g_config_system->full_bright);
				ui::Checkbox("remove scope overlay", &sdk::g_config_system->remove_scope_overlay);
				ui::Checkbox("disable post processing", &sdk::g_config_system->disable_post_processing);
				ui::Checkbox("force third person (alive)", &sdk::g_config_system->force_third_person_alive);
				ui::Keybind("##third_person_key", &sdk::g_config_system->third_person_key, &sdk::g_config_system->third_person_key_style);
				if (sdk::g_config_system->force_third_person_alive)
					ui::SliderInt("##third_person_distance", &sdk::g_config_system->third_person_distance, 30, 300, "%d");
				ui::Checkbox("force third person (dead)", &sdk::g_config_system->force_third_person_dead);
				ui::Checkbox("disable rendering of teammates", &sdk::g_config_system->disable_rendering_of_teammates);
				ui::Checkbox("disable rendering of ragdolls", &sdk::g_config_system->disable_rendering_of_ragdolls);
				ui::Checkbox("bullet tracers", &sdk::g_config_system->bullet_tracers);
				color_picker("##bullet_tracers_color", sdk::g_config_system->bullet_tracers_color);
				ui::Checkbox("bullet impacts", &sdk::g_config_system->bullet_impacts);
			}
			ui::EndChild();
		}

		if (this->m_current_tab == 3) {
			ui::SetNextWindowPos(ImVec2(ui::GetWindowPos().x + 26 + (ui::GetWindowSize().x / 2 - 36) * 0 + 20 * 0, ui::GetWindowPos().y + 52 + 27));
			ui::BeginChild("settings", ImVec2(ui::GetWindowSize().x / 2 - 36 + 4, ui::GetWindowSize().y / 2 - 80 - 27 + 65 + 31 - 30 - 30 - 120 + 24));
			{
				ui::Text("menu key");
				ui::Keybind("##menu_key", &sdk::g_config_system->menu_key);

				ui::Text("menu color");
				if (color_picker("##menu_color", sdk::g_config_system->menu_color, false)) {
					settings::m_menu_color[0] = sdk::g_config_system->menu_color[0];
					settings::m_menu_color[1] = sdk::g_config_system->menu_color[1];
					settings::m_menu_color[2] = sdk::g_config_system->menu_color[2];
					settings::m_menu_color[3] = sdk::g_config_system->menu_color[3];
				}

				ui::Checkbox("watermark", &sdk::g_config_system->menu_watermark);
			}
			ui::EndChild();
			ui::SetNextWindowPos(ImVec2(ui::GetWindowPos().x + 26 + (ui::GetWindowSize().x / 2 - 36) * 0 + 20 * 0, ui::GetWindowPos().y + (ui::GetWindowSize().y / 2 - 36) + 52 + 27 + 95 - 236 + 24 - 3));
			ui::BeginChild("movement", ImVec2(ui::GetWindowSize().x / 2 - 36 + 4, ui::GetWindowSize().y / 2 - 80 - 27 + 65 + 31 - 30 - 45 - 8 - 4 + 147));
			{
				ui::Checkbox("bunny hop", &sdk::g_config_system->bunny_hop);
				ui::Checkbox("air strafe", &sdk::g_config_system->air_strafe);
				ui::Checkbox("slide walk", &sdk::g_config_system->slide_walk);
				ui::Text("slow motion");
				ui::Keybind("##slow_motion_key", &sdk::g_config_system->slow_motion_key, &sdk::g_config_system->slow_motion_key_style);
				ui::SliderInt("speed limit", &sdk::g_config_system->speed_limit, 0, 260, sdk::g_config_system->speed_limit == 0 ? "auto" : "%du/s");
				ui::Text("peek assistance");
				ui::Keybind("##peek_assistence_key", &sdk::g_config_system->peek_assistence_key, &sdk::g_config_system->peek_assistence_key_style);
				if (sdk::g_config_system->peek_assistence_key) {
					ui::Text("color");
					color_picker("##peek_assistence_color", sdk::g_config_system->peek_assistence_color, false);
				}
			}
			ui::EndChild();

			ui::SetNextWindowPos(calc_pos(1));
			ui::BeginChild("others", ImVec2(ui::GetWindowSize().x / 2 - 36 + 4, ui::GetWindowSize().y / 2 + 27 - 94 + 70 + 37));
			{
				ui::Checkbox("override fov", &sdk::g_config_system->override_fov);
				ui::SliderInt("##override_fov_amount", &sdk::g_config_system->override_fov_amount, 60, 140, "%d°%");
				ui::Checkbox("override fov when scoped", &sdk::g_config_system->override_fov_when_scoped);
				ui::Checkbox("override viewmodel fov", &sdk::g_config_system->override_viewmodel_fov);
				ui::SliderInt("##override_viewmodel_fov_amount", &sdk::g_config_system->override_viewmodel_fov_amount, 60, 140, "%d°%");
				ui::Checkbox("aspect ratio", &sdk::g_config_system->aspect_ratio);
				if (sdk::g_config_system->aspect_ratio) {
					ui::SliderInt("width", &sdk::g_config_system->width, 1, 50, "%d");
					ui::SliderInt("height", &sdk::g_config_system->height, 1, 50, "%d");
				}
				ui::Checkbox("clan tag spammer", &sdk::g_config_system->clan_tag_spammer);
				ui::Checkbox("chat spammer", &sdk::g_config_system->chat_spammer);
				ui::Checkbox("name spammer", &sdk::g_config_system->name_spammer);
				ui::Checkbox("share esp", &sdk::g_config_system->share_esp);
				ui::Checkbox("log weapon purchases", &sdk::g_config_system->log_weapon_purchases);
				ui::Checkbox("log damage dealt", &sdk::g_config_system->log_damage_dealt);
				ui::SingleSelect("persistent kill feed", &sdk::g_config_system->persistent_kill_feed, { "none", "default", "only headshots" });
				ui::Text("auto buy items");
				ui::MultiSelect("##primary_weapons", &sdk::g_config_system->primary_weapons, { "nova", "xm1014", "sawed-off/mag-7", "m249", "negev", "mac-10/mp9", "mp7/mp5", "ump-45", "p90", "bizon", "galil/famas", "ak-47/m4a1-s/m4a4", "ssg 08", "sg 553/aug", "awp", "g3sg1/scar-20" });
				ui::MultiSelect("##secondary_weapons", &sdk::g_config_system->secondary_weapons, { "dual berettas", "p250", "tec-9/five-seven/cz75", "deagle/revolver" });
				ui::MultiSelect("##equipment", &sdk::g_config_system->equipment, { "kevlar", "helmet", "taser", "defuser", "fire", "decoy", "flash", "flash 2", "he", "smoke" });
				ui::SliderInt("auto buy minimal money", &sdk::g_config_system->auto_buy_minimal_money, 50, 16000, "%d$");
			}
			ui::EndChild();

			ui::SetNextWindowPos(ImVec2(ui::GetWindowPos().x - 4 + 26 + (ui::GetWindowSize().x / 2 - 36) * 1 + 20 * 1, ui::GetWindowPos().y + (ui::GetWindowSize().y / 2 - 36) + 52 + 27 - 60 + 68 + 70 - 3 - 24 + 37));
			ui::BeginChild("extra", ImVec2(ui::GetWindowSize().x / 2 - 36 + 4, ui::GetWindowSize().y / 2 - 80 - 27 + 27 + 1 - 50 - 12 - 5 + 24 - 37));
			{
				ui::Checkbox("infinite duck", &sdk::g_config_system->infinite_duck);
				ui::Text("fake duck");
				ui::Keybind("##fake_duck_key", &sdk::g_config_system->fake_duck_key, &sdk::g_config_system->fake_duck_key_style);
				ui::Text("radio");
				ui::Keybind("##radio_key", &sdk::g_config_system->radio_key, &sdk::g_config_system->radio_key_style);
				if (sdk::g_config_system->radio_key) {
					ui::SliderInt("volume", &sdk::g_config_system->volume, 0, 100, "%d%%");
					ui::SingleSelect("channel", &sdk::g_config_system->channel, { "phonk", "chanson", "podrubasik", "club", "house", "8-bit", "lo-fi", "eurobeat", "news"});
				}
			}
			ui::EndChild();
		}

		if (this->m_current_tab == 4) {
			sdk::g_config_system->menu();

			ui::SetNextWindowPos(ImVec2(ui::GetWindowPos().x - 4 + 26 + (ui::GetWindowSize().x / 2 - 36) * 1 + 20 * 1, ui::GetWindowPos().y + 52 + 27));
			ui::BeginChild("scripts", ImVec2(ui::GetWindowSize().x / 2 - 36 + 4, ui::GetWindowSize().y - 80 - 27));
			{
				ui::Text("soon...");
			}
			ui::EndChild();
		}

		if (this->m_current_tab == 5) {
			//skins::get().menu();

			//ui::EndChild();
		}

		ui::End();
	}

	void c_menu::draw_end()	{
		if (!this->m_initialized)
			return;

		ui::EndFrame();
		ui::Render();
		ImGui_ImplDX9_RenderDrawData(ui::GetDrawData());
	}

	void c_menu::handle() {
		if (!this->m_is_opened && ui::GetStyle().Alpha > 0.f) {
			float fc = 255.f / 0.2f * ui::GetIO().DeltaTime;
			ui::GetStyle().Alpha = std::clamp(ui::GetStyle().Alpha - fc / 255.f, 0.f, 1.f);
		}

		if (this->m_is_opened && ui::GetStyle().Alpha < 1.f) {
			float fc = 255.f / 0.2f * ui::GetIO().DeltaTime;
			ui::GetStyle().Alpha = std::clamp(ui::GetStyle().Alpha + fc / 255.f, 0.f, 1.f);
		}
	}

	bool c_menu::is_menu_initialized() {
		return this->m_initialized;
	}

	bool c_menu::is_menu_opened() {
		return this->m_is_opened;
	}

	void c_menu::set_menu_opened(bool v) {
		this->m_is_opened = settings::m_is_opened = v;
	}
}