#include "../../../supremacy.hpp"
#include "../../../dependencies/imgui/settings.hpp"

namespace supremacy::hacks {
	void c_dormant_esp::start()	{
		m_cur_sound_list.m_size = 0;
		valve::g_engine_sound->get_active_sounds(m_cur_sound_list);

		if (!m_cur_sound_list.m_size)
			return;

		for (int i{}; i < m_cur_sound_list.m_size; ++i) {
			auto& sound = m_cur_sound_list.at(i);

			if (sound.m_sound_source < 1
				|| sound.m_sound_source > 64
				|| (sound.m_origin->x == 0 && sound.m_origin->y == 0 && sound.m_origin->z == 0)
				|| !valid_sound(sound))
				continue;

			const auto player = static_cast<valve::c_player*>(valve::g_entity_list->find_entity(sound.m_sound_source));

			if (!player
				|| !player->alive()
				|| player == valve::g_local_player
				|| (player->friendly()
					&& !sdk::g_config_system->teammates))
				continue;

			setup_adjust(player, sound);
		}

		m_sound_buffer = m_cur_sound_list;
	}

	void c_dormant_esp::setup_adjust(valve::c_player* player, valve::sound_info_t& sound)	{
		vec3_t src, dst;
		valve::trace_t tr{};
		valve::trace_filter_simple_t filter{ player, 0 };

		src = *sound.m_origin + vec3_t(0.f, 0.f, 1.f);
		dst = src - vec3_t(0.f, 0.f, 100.f);

		valve::ray_t ray = { src, dst };
		valve::g_engine_trace->trace_ray(ray, (valve::e_mask)(0x1u | 0x2u | 0x8u | 0x4000u | 0x10000u | 0x2000000u),
			reinterpret_cast<valve::trace_filter_t*>(&filter), &tr);

		if (tr.m_all_solid)
			m_sound_players[sound.m_sound_source - 1].m_receive_time = -1;

		*sound.m_origin = tr.m_fraction <= 0.97f ? tr.m_end_pos : *sound.m_origin;

		m_sound_players[sound.m_sound_source - 1].m_flags = player->flags();
		m_sound_players[sound.m_sound_source - 1].m_flags |= (tr.m_fraction < 0.5f ? valve::e_ent_flags::ducking : valve::e_ent_flags::none) | (tr.m_fraction < 1.f ? valve::e_ent_flags::on_ground : valve::e_ent_flags::none);
		m_sound_players[sound.m_sound_source - 1].m_flags &= (tr.m_fraction >= 0.5f ? ~valve::e_ent_flags::ducking : valve::e_ent_flags::none) | (tr.m_fraction >= 1.f ? ~valve::e_ent_flags::on_ground : valve::e_ent_flags::none);
		m_sound_players[sound.m_sound_source - 1].m_receive_time = valve::g_global_vars->m_real_time;
		m_sound_players[sound.m_sound_source - 1].m_origin = *sound.m_origin;
	}

	bool c_dormant_esp::adjust_sound(valve::c_player* entity) {
		auto i = entity->index();
		auto sound_player = m_sound_players[i - 1];

		auto expired = false;

		if (fabs(valve::g_global_vars->m_real_time - sound_player.m_receive_time) < 1.f)
			g_visuals->m_dormant_data.at(i - 1).m_alpha = 160.f;
		else if (fabs(valve::g_global_vars->m_real_time - sound_player.m_receive_time) > 9.f)
			expired = true;

		entity->flags() = sound_player.m_flags;
		entity->set_abs_origin(sound_player.m_origin);

		return !expired;
	}

	bool c_dormant_esp::valid_sound(valve::sound_info_t& sound) {
		for (int i{}; i < m_sound_buffer.m_size; ++i)
			if (m_sound_buffer.at(i).m_guid == sound.m_guid)
				return false;

		return true;
	}

	void c_visuals::grenade_simulation_t::predict(const vec3_t& origin, const vec3_t& velocity, const float throw_time, const int offset) {
		m_origin = origin;
		m_velocity = velocity;
		m_collision_group = 13;

		const auto tick = valve::to_ticks(1.f / 30.f);

		m_last_update_tick = -tick;

		switch (m_index) {
		case valve::e_item_index::smoke_grenade: m_next_think_tick = valve::to_ticks(1.5f); break;
		case valve::e_item_index::decoy: m_next_think_tick = valve::to_ticks(2.f); break;
		case valve::e_item_index::flashbang:
		case valve::e_item_index::he_grenade:
			m_detonate_time = 1.5f;
			m_next_think_tick = valve::to_ticks(0.02f);

			break;
		case valve::e_item_index::molotov:
		case valve::e_item_index::inc_grenade:
			m_detonate_time = g_context->cvars().m_molotov_throw_detonate_time->get_float();
			m_next_think_tick = valve::to_ticks(0.02f);

			break;
		}

		const auto max_sim_amt = valve::to_ticks(60.f);
		for (; m_tick < max_sim_amt; ++m_tick) {
			if (m_next_think_tick <= m_tick)
				think();

			if (m_tick < offset)
				continue;

			if (physics_simulate())
				break;

			if ((m_last_update_tick + tick) > m_tick)
				continue;

			update_path(false);
		}

		if ((m_last_update_tick + tick) <= m_tick)
			update_path(false);

		m_expire_time = throw_time + valve::to_time(m_tick);
	}

	bool c_visuals::grenade_simulation_t::physics_simulate() {
		if (m_detonated)
			return true;

		const auto new_velocity_z = m_velocity.z - (g_context->cvars().m_sv_gravity->get_float() * 0.4f) * valve::g_global_vars->m_interval_per_tick;

		auto move = m_velocity;

		move.z += new_velocity_z;
		move.z /= 2.f;

		move *= valve::g_global_vars->m_interval_per_tick;

		m_velocity.z = new_velocity_z;

		valve::trace_t trace{};

		physics_push_entity(move, trace);

		if (m_detonated)
			return true;

		if (trace.m_fraction != 1.f) {
			update_path(true);

			perform_fly_collision_resolution(trace);
		}

		return false;
	}

	void c_visuals::grenade_simulation_t::physics_trace_entity(
		const vec3_t& src, const vec3_t& dst,
		const valve::e_mask mask, valve::trace_t& trace
	) {
		valve::trace_filter_skip_two_entities_t trace_filter{ m_owner, m_last_breakable, m_collision_group };

		valve::g_engine_trace->trace_ray(
			{ src, dst, { -2.f, -2.f, -2.f }, { 2.f, 2.f, 2.f } }, mask,
			reinterpret_cast<valve::trace_filter_t*>(&trace_filter), &trace
		);

		if (trace.m_start_solid
			&& (trace.m_contents & valve::e_mask::contents_current_90)) {
			trace.clear();

			valve::g_engine_trace->trace_ray(
				{ src, dst, { -2.f, -2.f, -2.f }, { 2.f, 2.f, 2.f } },
				mask & ~valve::e_mask::contents_current_90,
				reinterpret_cast<valve::trace_filter_t*>(&trace_filter), &trace
			);
		}

		if (!trace.hit()
			|| !trace.m_hit_entity
			|| !trace.m_hit_entity->is_player())
			return;

		trace.clear();

		valve::g_engine_trace->trace_ray(
			{ src, dst }, mask,
			reinterpret_cast<valve::trace_filter_t*>(&trace_filter), &trace
		);
	}

	void c_visuals::grenade_simulation_t::physics_push_entity(const vec3_t& push, valve::trace_t& trace) {
		physics_trace_entity(m_origin, m_origin + push,
			m_collision_group == 1
			? (valve::e_mask::solid | valve::e_mask::contents_current_90) & ~valve::e_mask::contents_monster
			: valve::e_mask::solid | valve::e_mask::contents_opaque
			| valve::e_mask::contents_ignore_nodraw_opaque | valve::e_mask::contents_current_90 | valve::e_mask::contents_hitbox,
			trace
		);

		valve::trace_filter_skip_two_entities_t trace_filter{ m_owner, m_last_breakable, m_collision_group };

		if (trace.m_start_solid) {
			m_collision_group = 3;

			valve::g_engine_trace->trace_ray(
				{ m_origin - push, m_origin + push },
				(valve::e_mask::solid | valve::e_mask::contents_current_90) & ~valve::e_mask::contents_monster,
				reinterpret_cast<valve::trace_filter_t*>(&trace_filter), &trace
			);
		}

		if (trace.m_fraction != 0.f)
			m_origin = trace.m_end_pos;

		if (!trace.m_hit_entity
			|| trace.m_hit_entity->is_player()
			|| (m_index != valve::e_item_index::ta_grenade
				&& m_index != valve::e_item_index::molotov && m_index != valve::e_item_index::inc_grenade)
			|| (m_index != valve::e_item_index::ta_grenade
				&& trace.m_plane.m_normal.z < std::cos(math::to_rad(g_context->cvars().m_weapon_molotov_maxdetonateslope->get_float()))))
			return;

		detonate(true);
	}

	void c_visuals::grenade_simulation_t::perform_fly_collision_resolution(valve::trace_t& trace) {
		auto surface_elasticity = 1.f;

		if (trace.m_hit_entity) {
			if (trace.m_hit_entity->breakable()) {
				m_last_breakable = trace.m_hit_entity;

				m_velocity *= 0.4f;

				return;
			}

			const auto is_player = trace.m_hit_entity->is_player();
			if (is_player)
				surface_elasticity = 0.3f;

			if (trace.m_hit_entity->index()) {
				if (is_player
					&& m_last_hit_entity == trace.m_hit_entity) {
					m_collision_group = 1;

					return;
				}

				m_last_hit_entity = trace.m_hit_entity;
			}
		}

		vec3_t velocity{};

		const auto back_off = m_velocity.dot(trace.m_plane.m_normal) * 2.f;

		for (std::size_t i{}; i < 3u; ++i) {
			const auto change = trace.m_plane.m_normal[i] * back_off;

			velocity[i] = m_velocity[i] - change;

			if (std::fabsf(velocity[i]) >= 1.f)
				continue;

			velocity[i] = 0.f;
		}

		velocity *= std::clamp(surface_elasticity * 0.45f, 0.f, 0.9f);

		if (trace.m_plane.m_normal.z > 0.7f) {
			const auto speed_sqr = velocity.length_sqr();
			if (speed_sqr > 96000.f) {
				const auto l = velocity.normalized().dot(trace.m_plane.m_normal);
				if (l > 0.5f)
					velocity *= 1.f - l + 0.5f;
			}

			if (speed_sqr < 400.f)
				m_velocity = {};
			else {
				m_velocity = velocity;

				physics_push_entity(velocity * ((1.f - trace.m_fraction) * valve::g_global_vars->m_interval_per_tick), trace);
			}
		}
		else {
			m_velocity = velocity;

			physics_push_entity(velocity * ((1.f - trace.m_fraction) * valve::g_global_vars->m_interval_per_tick), trace);
		}

		if (m_bounces_count > 20)
			return detonate(false);

		++m_bounces_count;
	}

	void c_visuals::grenade_simulation_t::think() {
		switch (m_index) {
		case valve::e_item_index::smoke_grenade:
			if (m_velocity.length_sqr() <= 0.01f)
				detonate(false);

			break;
		case valve::e_item_index::decoy:
			if (m_velocity.length_sqr() <= 0.04f)
				detonate(false);

			break;
		case valve::e_item_index::flashbang:
		case valve::e_item_index::he_grenade:
		case valve::e_item_index::molotov:
		case valve::e_item_index::inc_grenade:
			if (valve::to_time(m_tick) > m_detonate_time)
				detonate(false);

			break;
		}

		m_next_think_tick = m_tick + valve::to_ticks(0.2f);
	}

	void c_visuals::grenade_simulation_t::detonate(const bool bounced) {
		m_detonated = true;

		update_path(bounced);
	}

	void c_visuals::grenade_simulation_t::update_path(const bool bounced) {
		m_last_update_tick = m_tick;

		m_path.emplace_back(m_origin, bounced);
	}

	bool c_visuals::add_grenade_simulation(const grenade_simulation_t& sim, const bool warning) const {
		if (!valve::g_local_player->alive())
			return false;

		const auto points_count = sim.m_path.size();
		if (points_count < 2u
			|| valve::g_global_vars->m_cur_time >= sim.m_expire_time)
			return false;

		const auto& screen_size = ui::GetIO().DisplaySize;
		if (warning) {
			const auto& explode_pos = sim.m_path.back().first;
			auto dist = (valve::g_local_player->origin() - explode_pos).length();

			auto prev_pos = sim.m_path.front().first;

			for (auto i = 1u; i < points_count; ++i) {
				const auto& cur_pos = sim.m_path.at(i).first;

				add_grenade_warning_beam(prev_pos, cur_pos, sdk::g_config_system->grenade_trajectory_color);

				prev_pos = cur_pos;
			}

			vec2_t screen_pos{};
			const auto on_screen = math::to_screen(explode_pos, screen_size, valve::g_engine->w2s_matrix(), screen_pos);
			if (!on_screen
				&& dist > 400.f)
				return true;

			const auto unk = screen_size / 18.f;
			if (!on_screen
				|| screen_pos.x < -unk.x
				|| screen_pos.x >(screen_size.x + unk.x)
				|| screen_pos.y < -unk.y
				|| screen_pos.y >(screen_size.y + unk.y)) {
				vec3_t dir{};
				math::angle_vectors(valve::g_view_render->m_setup.m_angles, &dir);

				dir.z = 0.f;
				dir.normalize();

				const auto radius = 210.f * (screen_size.y / 480.f);

				auto delta = explode_pos - valve::g_view_render->m_setup.m_origin;

				delta.normalize();

				screen_pos.x = radius * -delta.dot(vec3_t{ 0.f, 0.f, 1.f }.cross(dir));
				screen_pos.y = radius * -delta.dot(dir);

				const auto radians = math::to_rad(
					-math::to_deg(std::atan2(screen_pos.x, screen_pos.y) + math::k_pi)
				);

				screen_pos.x = static_cast<int>(screen_size.x / 2.f + radius * std::sin(radians));
				screen_pos.y = static_cast<int>(screen_size.y / 2.f - radius * std::cos(radians));
			}

			add_filled_circle(screen_pos, 23.f, 120, 0xb4000000u | static_cast<std::uint8_t>(255 * (1.f - std::min(dist, 350.f) / 350.f)));

			add_circle(screen_pos, 23.f, 120, 0xb4000000u);

			const auto mod = std::clamp(
				(sim.m_expire_time - valve::g_global_vars->m_cur_time)
				/ valve::to_time(sim.m_tick),
				0.f, 1.f
			);

			std::vector< int > x_points{}, y_points{};

			x_points.reserve(480);
			y_points.reserve(480);

			const auto limit = math::k_pi2 * mod;
			const auto step = limit / 240;

			for (float i{}; i < limit; i += step) {
				x_points.emplace_back(static_cast<int>(screen_pos.x + (23.f * std::cos(i))));
				y_points.emplace_back(static_cast<int>(screen_pos.y + (23.f * std::sin(i))));
			}

			const auto size = x_points.size();
			for (auto i = 1u; i < size; ++i) {
				x_points.emplace_back(x_points.at(size - i));
				y_points.emplace_back(y_points.at(size - i));
			}

			valve::g_surface->set_clr(0xb4ffffffu);
			valve::g_surface->add_poly_line(x_points.data(), y_points.data(), x_points.size());

			screen_pos.y -= 7.f;

			// todo: simv0l - fix flashbang icon.
			add_text(
				{ screen_pos.x + 1.f, screen_pos.y },
				m_fonts.m_icons26, char_code_for_item_index(sim.m_index), 0xb431c0c5u, 3
			);

			screen_pos.y += 11.f;

			constexpr auto k_meters_per_inch = 0.0254f;
			constexpr auto k_feet_per_meter = 3.281f;

			dist *= k_meters_per_inch * k_feet_per_meter;

			add_text(
				screen_pos, m_fonts.m_menu,
				std::format("{} ft", static_cast<int>(dist)).data(),
				0xb4ffffffu, 1
			);

			return true;
		}

		vec2_t prev_screen_pos{};
		auto prev_on_screen = math::to_screen(sim.m_path.front().first,
			screen_size, valve::g_engine->w2s_matrix(), prev_screen_pos
		);

		for (auto i = 1u; i < points_count; ++i) {
			vec2_t cur_screen_pos{};
			const auto cur_on_screen = math::to_screen(sim.m_path.at(i).first,
				screen_size, valve::g_engine->w2s_matrix(), cur_screen_pos
			);

			if (prev_on_screen
				&& cur_on_screen)
				add_line(prev_screen_pos, cur_screen_pos,
					static_cast<std::uint8_t>(sdk::g_config_system->grenade_trajectory_color[0] * 255.f)
					| (static_cast<std::uint8_t>(sdk::g_config_system->grenade_trajectory_color[1] * 255.f) << 8u)
					| (static_cast<std::uint8_t>(sdk::g_config_system->grenade_trajectory_color[2] * 255.f) << 16u)
					| (static_cast<std::uint8_t>(sdk::g_config_system->grenade_trajectory_color[3] * 255.f) << 24u)
				);

			prev_screen_pos = cur_screen_pos;
			prev_on_screen = cur_on_screen;
		}

		return true;
	}

	bool c_visuals::calc_bbox(valve::c_player* const player, bbox_t& bbox) const {
		auto origin = player->origin();

		if (player->alive())
			origin = player->dormant() ? player->abs_origin() : g_lag_comp->entry(player->index() - 1).m_render_origin;
		else
			origin = hacks::g_visuals->m_alive_origin.at(player->index() - 1);

		vec2_t bottom{};
		const auto on_screen = math::to_screen(origin, ui::GetIO().DisplaySize, valve::g_engine->w2s_matrix(), bottom);

		origin.z += player->obb_max().z;

		if (!math::to_screen(origin, ui::GetIO().DisplaySize, valve::g_engine->w2s_matrix(), bbox.m_min) && !on_screen)
			return false;

		bbox.m_size.y = bottom.y - bbox.m_min.y + 6;
		bbox.m_size.x = bbox.m_size.y / 2.f;

		bbox.m_min.x -= bbox.m_size.x / 2.f;

		return true;
	}

	std::optional< valve::bones_t > c_visuals::try_to_lerp_bones(const int index) const {
		const auto& entry = g_lag_comp->entry(index - 1);
		if (entry.m_lag_records.size() < 2u)
			return std::nullopt;

		const auto max_unlag = g_context->cvars().m_sv_maxunlag->get_float();

		const auto net_channel_info = valve::g_engine->net_channel_info();
		const auto total_latency = std::clamp(net_channel_info->avg_latency(0) + net_channel_info->avg_latency(1), 0.f, max_unlag);
		const auto correct = total_latency + g_context->net_info().m_lerp;

		const auto end = entry.m_lag_records.end();
		for (auto it = entry.m_lag_records.begin(); it != end; ++it) {
			const auto current = it->get();

			const auto latest = std::next(it) == end;

			if (current && current->valid()
				&& (!latest && (std::next(it)->get()))) {
				if ((current->m_origin - entry.m_player->origin()).length() < 1.f)
					continue;

				const auto next_origin = latest ? entry.m_player->origin() : std::next(it)->get()->m_origin;
				const auto next_time = latest ? entry.m_player->sim_time() : std::next(it)->get()->m_sim_time;

				const auto add = latest ? 1.f : (next_time - current->m_sim_time);
				const auto delta = (current->m_sim_time + correct + add) - valve::g_global_vars->m_cur_time;

				const auto mult = 1.f / add;
				const auto lerp_amt = std::clamp(delta * mult, 0.f, 1.f);

				const vec3_t lerped_origin{
					std::lerp(next_origin.x, current->m_origin.x, lerp_amt),
					std::lerp(next_origin.y, current->m_origin.y, lerp_amt),
					std::lerp(next_origin.z, current->m_origin.z, lerp_amt)
				};

				const auto origin_delta = lerped_origin - current->m_origin;

				auto lerped_bones = current->m_sides.at(0).m_bones;
				for (std::size_t i{}; i < lerped_bones.size(); ++i) {
					lerped_bones[i][0][3] += origin_delta.x;
					lerped_bones[i][1][3] += origin_delta.y;
					lerped_bones[i][2][3] += origin_delta.z;
				}

				return lerped_bones;
			}
		}

		return std::nullopt;
	}

	valve::material_t* c_visuals::create_material(
		const std::string_view name,
		const std::string_view shader, const std::string_view data
	) const {
		using key_values_t = void(__thiscall*)(const std::uintptr_t, const char* const);

		using load_from_buffer_t = void(__thiscall*)(
			const std::uintptr_t, const char* const, const char* const,
			const std::uintptr_t, const std::uintptr_t, const std::uintptr_t, const std::uintptr_t
			);

		const auto key_values = reinterpret_cast<std::uintptr_t>(std::malloc(36u));

		reinterpret_cast<key_values_t>(g_context->addresses().m_key_values_init)(key_values, shader.data());

		reinterpret_cast<load_from_buffer_t>(g_context->addresses().m_key_values_load_from_buffer)(
			key_values, name.data(), data.data(), 0, 0, 0, 0
			);

		return valve::g_mat_system->create_material(name.data(), key_values);
	}

	void c_visuals::override_material(
		const int type, const bool ignore_z,
		const float r, const float g, const float b, const float a
	) const {
		static const auto solid_mat = create_material(
			xorstr_("vuln_solid.vmt"),
			xorstr_("UnlitGeneric"),
			xorstr_(
				R"#("UnlitGeneric" {
					"$basetexture" "vgui/white_additive"
					"$ignorez"      "0"
					"$model"		"1"
					"$flat"			"1"
					"$nocull"		"1"
					"$selfillum"	"1"
					"$halflambert"	"1"
					"$nofog"		"1"
					"$wireframe"	"0"
				})#"
			)
		);

		static const auto regular_mat = create_material(
			xorstr_("vuln_regular.vmt"),
			xorstr_("VertexLitGeneric"),
			xorstr_(
				R"#("VertexLitGeneric" {
					"$basetexture" "vgui/white_additive"
					"$ignorez"      "0"
					"$model"		"1"
					"$flat"			"0"
					"$nocull"		"1"
					"$halflambert"	"1"
					"$nofog"		"1"
					"$wireframe"	"0"
				})#"
			)
		);

		static const auto glow_mat = create_material(
			xorstr_("vuln_glow.vmt"),
			xorstr_("VertexLitGeneric"),
			xorstr_(
				R"#("VertexLitGeneric" {
					"$additive"					"1"
					"$ignorez"					"0"
					"$envmap"					"models/effects/cube_white"
					"$envmaptint"				"[1 1 1]"
					"$envmapfresnel"			"1"
					"$envmapfresnelminmaxexp"	"[0 1 2]"
					"$alpha"					"0.8"
				})#"
			)
		);

		static const auto metallic_mat = create_material(
			xorstr_("vuln_metallic.vmt"),
			xorstr_("VertexLitGeneric"),
			xorstr_(
				R"#("VertexLitGeneric" {
					 "$basetexture"					"vgui/white_additive"
					 "$ignorez"						"0"
					 "$envmap"						"env_cubemap"
					 "$normalmapalphaenvmapmask"	"1"
					 "$envmapcontrast"				"1"
					 "$nofog"						"1"
					 "$model"						"1"
					 "$nocull"						"0"
					 "$selfillum"					"1"
					 "$halflambert"					"1"
					 "$znearer"						"0"
					 "$flat"						"1" 
				})#"
			)
		);

		valve::material_t* material{};

		switch (type) {
		case 0: material = regular_mat; break;
		case 1: material = solid_mat; break;
		case 2: material = glow_mat; break;
		case 3: material = metallic_mat; break;
		}

		material->set_alpha(a);
		material->set_clr(r, g, b);
		material->set_flag(1u << 15u, ignore_z);

		if (const auto $envmaptint = material->find_var(xorstr_("$envmaptint"), nullptr, false))
			$envmaptint->set_vec(r, g, b);

		valve::g_studio_render->override_material(material);
	}

	const char* c_visuals::char_code_for_item_index(const valve::e_item_index index) const {
		switch (index) {
		case valve::e_item_index::deagle: return u8"\uE001"; break;
		case valve::e_item_index::elite: return u8"\uE002"; break;
		case valve::e_item_index::fiveseven: return u8"\uE003"; break;
		case valve::e_item_index::glock: return u8"\uE004"; break;
		case valve::e_item_index::ak47: return u8"\uE007"; break;
		case valve::e_item_index::aug: return u8"\uE008"; break;
		case valve::e_item_index::awp: return u8"\uE009"; break;
		case valve::e_item_index::famas: return u8"\uE00A"; break;
		case valve::e_item_index::g3sg1: return u8"\uE00B"; break;
		case valve::e_item_index::galil: return u8"\uE00D"; break;
		case valve::e_item_index::m249: return u8"\uE00E"; break;
		case valve::e_item_index::m4a1: return u8"\uE010"; break;
		case valve::e_item_index::mac10: return u8"\uE011"; break;
		case valve::e_item_index::p90: return u8"\uE013"; break;
		case valve::e_item_index::mp5sd: return u8"\uE017"; break;
		case valve::e_item_index::ump45: return u8"\uE018"; break;
		case valve::e_item_index::xm1014: return u8"\uE019"; break;
		case valve::e_item_index::bizon: return u8"\uE01A"; break;
		case valve::e_item_index::mag7: return u8"\uE01B"; break;
		case valve::e_item_index::negev: return u8"\uE01C"; break;
		case valve::e_item_index::sawedoff: return u8"\uE01D"; break;
		case valve::e_item_index::tec9: return u8"\uE01E"; break;
		case valve::e_item_index::taser: return u8"\uE01F"; break;
		case valve::e_item_index::hkp2000: return u8"\uE020"; break;
		case valve::e_item_index::mp7: return u8"\uE021"; break;
		case valve::e_item_index::mp9: return u8"\uE022"; break;
		case valve::e_item_index::nova: return u8"\uE023"; break;
		case valve::e_item_index::p250: return u8"\uE024"; break;
		case valve::e_item_index::scar20: return u8"\uE026"; break;
		case valve::e_item_index::sg553: return u8"\uE027"; break;
		case valve::e_item_index::ssg08: return u8"\uE028"; break;
		case valve::e_item_index::knife: return u8"\uE02A"; break;
		case valve::e_item_index::flashbang: return u8"\uE02B"; break;
		case valve::e_item_index::he_grenade: return u8"\uE02C"; break;
		case valve::e_item_index::smoke_grenade: return u8"\uE02D"; break;
		case valve::e_item_index::molotov:
		case valve::e_item_index::firebomb:
			return u8"\uE02E";
			break;
		case valve::e_item_index::decoy:
		case valve::e_item_index::diversion:
			return u8"\uE02F";
			break;
		case valve::e_item_index::inc_grenade: return u8"\uE030"; break;
		case valve::e_item_index::c4: return u8"\uE031"; break;
		case valve::e_item_index::healthshot: return u8"\uE039"; break;
		case valve::e_item_index::knife_gg:
		case valve::e_item_index::knife_t:
			return u8"\uE03B";
			break;
		case valve::e_item_index::m4a1_silencer: return u8"\uE03C"; break;
		case valve::e_item_index::usp_silencer: return u8"\uE03D"; break;
		case valve::e_item_index::cz75a: return u8"\uE03F"; break;
		case valve::e_item_index::revolver: return u8"\uE040"; break;
		case valve::e_item_index::ta_grenade: return u8"\uE044"; break;
		case valve::e_item_index::fists: return u8"\uE045"; break;
		case valve::e_item_index::tablet: return u8"\uE048"; break;
		case valve::e_item_index::melee: return u8"\uE04A"; break;
		case valve::e_item_index::axe: return u8"\uE04B"; break;
		case valve::e_item_index::hammer: return u8"\uE04C"; break;
		case valve::e_item_index::spanner: return u8"\uE04E"; break;
		case valve::e_item_index::knife_bayonet: return u8"\uE1F4"; break;
		case valve::e_item_index::knife_css: return u8"\uE1F7"; break;
		case valve::e_item_index::knife_flip: return u8"\uE1F9"; break;
		case valve::e_item_index::knife_gut: return u8"\uE1FA"; break;
		case valve::e_item_index::knife_karambit: return u8"\uE1FB"; break;
		case valve::e_item_index::knife_m9_bayonet: return u8"\uE1FC"; break;
		case valve::e_item_index::knife_tactical: return u8"\uE1FD"; break;
		case valve::e_item_index::knife_falchion: return u8"\uE200"; break;
		case valve::e_item_index::knife_survival_bowie: return u8"\uE202"; break;
		case valve::e_item_index::knife_butterfly: return u8"\uE203"; break;
		case valve::e_item_index::knife_push: return u8"\uE204"; break;
		case valve::e_item_index::knife_cord: return u8"\uE205"; break;
		case valve::e_item_index::knife_canis: return u8"\uE206"; break;
		case valve::e_item_index::knife_ursus: return u8"\uE207"; break;
		case valve::e_item_index::knife_gypsy_jackknife: return u8"\uE208"; break;
		case valve::e_item_index::knife_outdoor: return u8"\uE209"; break;
		case valve::e_item_index::knife_stiletto: return u8"\uE20A"; break;
		case valve::e_item_index::knife_widowmaker: return u8"\uE20B"; break;
		case valve::e_item_index::knife_skeleton: return u8"\uE20D"; break;
		}

		return u8"\u003F";
	}

	void c_visuals::add_line(
		const vec2_t& from, const vec2_t& to, const std::uint32_t clr
	) const {
		valve::g_surface->set_clr(clr);
		valve::g_surface->add_line(from, to);
	}

	void c_visuals::add_rect(
		const vec2_t& pos, const vec2_t& size, const std::uint32_t clr
	) const {
		valve::g_surface->set_clr(clr);
		valve::g_surface->add_rect(pos, pos + size);
	}

	void c_visuals::add_rect_filled(
		const vec2_t& pos, const vec2_t& size, const std::uint32_t clr
	) const {
		valve::g_surface->set_clr(clr);
		valve::g_surface->add_rect_filled(pos, pos + size);
	}

	void c_visuals::add_text(
		vec2_t pos, const unsigned long font,
		const std::string_view str, const std::uint32_t clr, const int flags
	) const {
		const auto wstr = util::to_utf16(str);

		if (flags) {
			int width{}, height{};
			valve::g_font_mgr->calc_text_size(font, wstr.data(), width, height);

			if (flags & 1)
				pos.x -= width / 2;

			if (flags & 2)
				pos.y -= height / 2;

			if (flags & 4)
				pos.x -= width;
		}

		valve::g_surface->set_text_pos(pos);
		valve::g_surface->set_text_clr(clr);
		valve::g_surface->set_text_font(font);

		valve::g_surface->add_text(wstr.data(), wstr.size());
	}

	void c_visuals::add_text(
		vec2_t pos, const unsigned long font,
		const std::wstring_view str, const std::uint32_t clr, const int flags
	) const {
		if (flags) {
			int width{}, height{};
			valve::g_font_mgr->calc_text_size(font, str.data(), width, height);

			if (flags & 1)
				pos.x -= width / 2;

			if (flags & 2)
				pos.y -= height / 2;

			if (flags & 4)
				pos.x -= width;
		}

		valve::g_surface->set_text_pos(pos);
		valve::g_surface->set_text_clr(clr);
		valve::g_surface->set_text_font(font);

		valve::g_surface->add_text(str.data(), str.size());
	}

	void c_visuals::add_vertices(
		const valve::vertex_t* const vertices,
		const std::size_t vertices_count, const std::uint32_t clr
	) const {
		static const auto id = valve::g_surface->create_new_texture(true);
		static const std::uint8_t clr_buffer[4]{ 0xff, 0xff, 0xff, 0xff };

		valve::g_surface->set_texture_rgba(id, clr_buffer, 1, 1);

		valve::g_surface->set_clr(clr);
		valve::g_surface->set_texture(id);

		valve::g_surface->add_textured_polygon(vertices_count, vertices);
	}

	void c_visuals::add_circle(
		const vec2_t& pos, const float radius,
		const int segments_count, const std::uint32_t clr
	) const {
		std::vector< int > x_points{}, y_points{};

		const auto step = math::k_pi2 / segments_count;
		for (auto i = 0.f; i < math::k_pi2; i += step) {
			x_points.emplace_back(pos.x + radius * std::cos(i));
			y_points.emplace_back(pos.y + radius * std::sin(i));
		}

		valve::g_surface->set_clr(clr);
		valve::g_surface->add_poly_line(x_points.data(), y_points.data(), x_points.size());
	}

	void c_visuals::add_filled_circle(
		const vec2_t& pos, const float radius,
		const int segments_count, const std::uint32_t clr
	) const {
		static const auto id = valve::g_surface->create_new_texture(true);
		static const std::uint8_t clr_buffer[4]{ 0xff, 0xff, 0xff, 0xff };

		valve::g_surface->set_texture_rgba(id, clr_buffer, 1, 1);

		valve::g_surface->set_clr(clr);
		valve::g_surface->set_texture(id);

		std::vector< valve::vertex_t > vertices{};

		const auto step = math::k_pi2 / segments_count;
		for (auto i = 0.f; i < math::k_pi2; i += step) {
			auto& vertex = vertices.emplace_back();

			vertex.m_pos = { pos.x + radius * std::cos(i), pos.y + radius * std::sin(i) };
		}

		valve::g_surface->add_textured_polygon(vertices.size(), vertices.data());
	}

	void c_visuals::add_beam_ring(const vec3_t& pos, const float* const clr) const {
		valve::beam_info_t info{};

		info.m_type = 7;
		info.m_center = pos;
		info.m_center.z += 5.f;

		info.m_model_index = valve::g_model_info->lookup_model_index(xorstr_("sprites/purplelaser1.vmt"));
		info.m_model_name = xorstr_("sprites/purplelaser1.vmt");
		info.m_life = 0.5f;
		info.m_width = 7.f;
		info.m_end_width = 7.f;
		info.m_brightness = 255.f;
		info.m_speed = 1.f;
		info.m_renderable = true;
		info.m_red = clr[0] * 255.f;
		info.m_green = clr[1] * 255.f;
		info.m_blue = clr[2] * 255.f;
		info.m_segments = 1;
		info.m_speed = 150.f;
		info.m_fade_length = 1.f;

		info.m_start_radius = 10.f;
		info.m_end_radius = 440.f;

		const auto beam = valve::g_beams->create_beam_ring_point(info);
		if (!beam)
			return;

		valve::g_beams->draw_beam(beam);
	}

	void c_visuals::add_beam(const vec3_t& from, const vec3_t& to, const float* const clr) const {
		valve::beam_info_t info{};

		info.m_start = from;
		info.m_end = to;
		info.m_model_index = valve::g_model_info->lookup_model_index(xorstr_("sprites/purplelaser1.vmt"));
		info.m_model_name = xorstr_("sprites/purplelaser1.vmt");
		info.m_life = 4.f;
		info.m_width = 2.f;
		info.m_end_width = 2.f;
		info.m_fade_length = 0.f;
		info.m_amplitude = 2.f;
		info.m_brightness = 255.f;
		info.m_speed = 0.2f;
		info.m_segments = 2;
		info.m_renderable = true;
		info.m_red = clr[0] * 255.f;
		info.m_green = clr[1] * 255.f;
		info.m_blue = clr[2] * 255.f;
		info.m_flags = 0x100 | 0x200 | 0x8000;

		const auto beam = valve::g_beams->create_beam_points(info);
		if (!beam)
			return;

		valve::g_beams->draw_beam(beam);
	}

	void c_visuals::add_grenade_warning_beam(const vec3_t& from, const vec3_t& to, const float* const clr) const {
		valve::beam_info_t info{};

		info.m_start = from;
		info.m_end = to;
		info.m_model_index = valve::g_model_info->lookup_model_index(xorstr_("sprites/white.vmt"));
		info.m_model_name = xorstr_("sprites/white.vmt");
		info.m_halo_scale = 255.f;
		info.m_life = 0.03f;
		info.m_width = 1.5f;
		info.m_end_width = 1.5f;
		info.m_brightness = 30.f;
		info.m_speed = 0.5f;
		info.m_segments = 4;
		info.m_renderable = true;
		info.m_red = clr[0] * 255.f;
		info.m_green = clr[1] * 255.f;
		info.m_blue = clr[2] * 255.f;

		const auto beam = valve::g_beams->create_beam_points(info);
		if (!beam)
			return;

		valve::g_beams->draw_beam(beam);
	}

	void c_visuals::oof_arrow(const vec3_t& pos) const {
		const auto& screen_size = ui::GetIO().DisplaySize;

		const auto unk = screen_size / 18.f;

		vec2_t screen_pos{};
		if (math::to_screen(pos, screen_size, valve::g_engine->w2s_matrix(), screen_pos)
			&& screen_pos.x >= -unk.x
			&& screen_pos.x <= (screen_size.x + unk.x)
			&& screen_pos.y >= -unk.y
			&& screen_pos.y <= (screen_size.y + unk.y))
			return;

		vec3_t dir{};
		math::angle_vectors(valve::g_view_render->m_setup.m_angles, &dir);

		dir.z = 0.f;
		dir.normalize();

		const auto radius = 200.f * (screen_size.y / 480.f);

		auto delta = pos - valve::g_view_render->m_setup.m_origin;

		delta.normalize();

		screen_pos.x = radius * -delta.dot(vec3_t{ 0.f, 0.f, 1.f }.cross(dir));
		screen_pos.y = radius * -delta.dot(dir);

		const auto radians = math::to_rad(
			-math::to_deg(std::atan2(screen_pos.x, screen_pos.y) + math::k_pi)
		);

		const auto cos = std::cos(radians);
		const auto sin = std::sin(radians);

		screen_pos.x = static_cast<int>(screen_size.x / 2.f + radius * sin);
		screen_pos.y = static_cast<int>(screen_size.y / 2.f - radius * cos);

		std::array< valve::vertex_t, 3u > vertices{};

		vertices.at(0u).m_pos = screen_pos;
		vertices.at(1u).m_pos = { screen_pos.x + 12, screen_pos.y + 24 };
		vertices.at(2u).m_pos = { screen_pos.x - 12, screen_pos.y + 24 };

		for (auto& vertex : vertices) {
			const auto delta = vertex.m_pos - screen_pos;

			vertex.m_pos = {
				screen_pos.x + delta.x * cos - delta.y * sin,
				screen_pos.y + delta.x * sin + delta.y * cos
			};
		}

		add_vertices(vertices.data(), vertices.size(),
			static_cast<uint8_t>(sdk::g_config_system->out_of_fow_arrow_color[0] * 255.f)
			| (static_cast<uint8_t>(sdk::g_config_system->out_of_fow_arrow_color[1] * 255.f) << 8u)
			| (static_cast<uint8_t>(sdk::g_config_system->out_of_fow_arrow_color[2] * 255.f) << 16u)
			| (static_cast<uint8_t>(sdk::g_config_system->out_of_fow_arrow_color[3] * 255.f) << 24u)
		);
	}

	void c_visuals::send_net_data(valve::c_player* const player) {
		if (!sdk::g_config_system->share_esp)
			return;

		valve::shared_esp_data_t data{};		
		data.m_player_idx = static_cast <int16_t> (player->index());
		data.m_unique_key = static_cast <int16_t> (std::stoi(xorstr_("1469")));

		const vec3_t origin = player->origin();
		data.m_x = static_cast <int16_t> (origin.x);
		data.m_y = static_cast <int16_t> (origin.y);
		data.m_z = static_cast <int16_t> (origin.z);

		std::vector<uint8_t> voice{};
		voice.resize(sizeof(data));
		memcpy(voice.data(), &data, sizeof(data));

		valve::c_voice_communication_data new_data{};
		memcpy(new_data.raw_data(), voice.data(), voice.size());

		valve::clc_msg_voice_data_t msg;
		memset(&msg, 0, sizeof(msg));

		reinterpret_cast<uint32_t(__fastcall*)(void*, void*)>(g_context->addresses().m_construct_voice_data_message)(&msg, nullptr);
		msg.set_data(&new_data);

		valve::communication_string_t comm_str{};
		msg.m_data = (uintptr_t)&comm_str;
		msg.m_format = 0;
		msg.m_flags = 63;

		valve::g_client_state->m_net_channel->send_net_msg((uintptr_t)&msg, false, true);
	}

	void c_visuals::handle_player(valve::c_player* const player) {	
		const auto index = player->index();

		if (player == valve::g_local_player
			|| (player->friendly() && !sdk::g_config_system->teammates))
			return;

		const auto dormant = player->dormant();
		bool alive_check{};

		if (!player->alive()) {
			m_dormant_data.at(index - 1).m_alpha -= 255.f / 1.f * valve::g_global_vars->m_frame_time;
			m_dormant_data.at(index - 1).m_alpha = std::clamp(m_dormant_data.at(index - 1).m_alpha, 0.f, 255.f);
			alive_check = true;
		}
		else {
			m_alive_origin.at(index - 1) = g_lag_comp->entry(index - 1).m_render_origin;
			if (!player->dormant()) {
				player->compute_hitbox(&m_alive_mins.at(index - 1), &m_alive_maxs.at(index - 1));
				m_dormant_data.at(index - 1).m_health = player->health();
			}
		}

		if (!m_dormant_data.at(index - 1).m_alpha
			&& alive_check)
			return;

		if (!dormant
			&& sdk::g_config_system->radar)
			player->spotted() = player->spotted_by(valve::g_engine->local_player_index()) = true;

		if (!alive_check) {
			if (dormant) {
				if (!sdk::g_config_system->dormant)
					return;

				g_dormant_esp->adjust_sound(player);
			
				if (player->weapon()) {
					if (m_dormant_data.at(index - 1).m_weapon_id > 0)
						player->weapon()->item_index() = static_cast <valve::e_item_index> (m_dormant_data.at(index - 1).m_weapon_id);

					if (player->weapon()->wpn_data()
						&& m_dormant_data.at(index - 1).m_weapon_type > -1)
							player->weapon()->wpn_data()->m_type = m_dormant_data.at(index - 1).m_weapon_type;
				}

				if (m_dormant_data.at(index - 1).m_alpha < 160.f) {
					m_dormant_data.at(index - 1).m_alpha -= 255.f / 16.f * valve::g_global_vars->m_frame_time;
					m_dormant_data.at(index - 1).m_alpha = std::clamp(m_dormant_data.at(index - 1).m_alpha, 0.f, 160.f);
				}
				else
					m_dormant_data.at(index - 1).m_alpha -= 255.f / 1.f * valve::g_global_vars->m_frame_time;
			}
			else {
				g_dormant_esp->m_sound_players[index - 1].m_receive_time = valve::g_global_vars->m_real_time;
				g_dormant_esp->m_sound_players[index - 1].m_origin = player->abs_origin();
				g_dormant_esp->m_sound_players[index - 1].m_flags = player->flags();

				m_dormant_data.at(index - 1).m_receive_time = 0.f;
				m_dormant_data.at(index - 1).m_alpha += 255.f / 0.68f * valve::g_global_vars->m_frame_time;
				m_dormant_data.at(index - 1).m_alpha = std::clamp(m_dormant_data.at(index - 1).m_alpha, 0.f, 255.f);
				m_dormant_data.at(index - 1).m_weapon_id = 0;
				m_dormant_data.at(index - 1).m_weapon_type = -1;
			}
		}

		const auto weapon = player->weapon();
		const auto wpn_data = weapon ? weapon->wpn_data() : nullptr;

		bbox_t bbox{};
		if (!calc_bbox(player, bbox))
			return;

		if (sdk::g_config_system->bounding_box) {
			add_rect(
				{ bbox.m_min.x + 1, bbox.m_min.y + 1 },
				{ bbox.m_size.x - 2, bbox.m_size.y - 2 },
				0 | (0 << 8u) | (0 << 16u) | (static_cast<std::uint8_t>(std::clamp((int)m_dormant_data[index - 1].m_alpha, 0, 140)) << 24u)
			);

			add_rect(
				bbox.m_min, bbox.m_size,
				static_cast<std::uint8_t>(sdk::g_config_system->bounding_box_color[0] * 255.f)
				| (static_cast<std::uint8_t>(sdk::g_config_system->bounding_box_color[1] * 255.f) << 8u)
				| (static_cast<std::uint8_t>(sdk::g_config_system->bounding_box_color[2] * 255.f) << 16u)
				| (static_cast<std::uint8_t>((int)m_dormant_data.at(index - 1).m_alpha) << 24u)
			);

			add_rect(
				{ bbox.m_min.x - 1, bbox.m_min.y - 1 },
				{ bbox.m_size.x + 2, bbox.m_size.y + 2 },
				0 | (0 << 8u) | (0 << 16u) | (static_cast<std::uint8_t>(std::clamp((int)m_dormant_data[index - 1].m_alpha, 0, 140)) << 24u)
			);
		}

		if (sdk::g_config_system->health_bar) {
			add_rect_filled({ bbox.m_min.x - 6, bbox.m_min.y - 1 }, { 4, bbox.m_size.y + 2 }, 0 | (0 << 8u) | (0 << 16u) | (static_cast<std::uint8_t>(std::clamp((int)m_dormant_data[index - 1].m_alpha, 0, 140)) << 24u));

			auto hp = std::clamp(m_dormant_data.at(index - 1).m_health, 0, 100);

			int red = 150;
			int green = 255;
			int blue = 80;

			if (hp >= 27) {
				if (hp < 57) {
					red = 215;
					green = 200;
					blue = 80;
				}
			}
			else {
				red = 255;
				green = 50;
				blue = 80;
			}
						
			if (settings::m_previous_hp[index - 1] > hp)
				settings::m_previous_hp[index - 1] -= (255.f / 1.f) * valve::g_global_vars->m_frame_time;
			else
				settings::m_previous_hp[index - 1] = hp;

			const auto mod = settings::m_previous_hp[index - 1] / 100.f;
			const auto height = bbox.m_size.y * mod;				

			add_rect_filled({ bbox.m_min.x - 5, bbox.m_min.y + bbox.m_size.y - height }, { 2, height },
				static_cast<std::uint8_t>(red)
				| (static_cast<std::uint8_t>(green) << 8u)
				| (static_cast<std::uint8_t>(blue) << 16u)
				| (static_cast<std::uint8_t>((int)m_dormant_data.at(index - 1).m_alpha) << 24u)
			);

			if (hp > 0
				&& hp < 100)
				add_text({ bbox.m_min.x - 5, bbox.m_min.y + bbox.m_size.y - height - 8 },
					m_fonts.m_keybinds, std::to_string(hp).data(),
					255 | (255 << 8u) | (255 << 16u) | ((int)m_dormant_data.at(index - 1).m_alpha << 24u), 1
				);
		}

		if (sdk::g_config_system->name) {
			const auto player_info = valve::g_engine->player_info(index);
			if (player_info.has_value()) {
				std::string name{ std::string(player_info.value().m_name) };

				if (name.size() > 32) {
					name.resize(32);
					name.append(xorstr_("..."));
				}

				add_text(
					{ bbox.m_min.x + bbox.m_size.x / 2.f, bbox.m_min.y - 13 },
					m_fonts.m_menu, name,
					static_cast<std::uint8_t>(sdk::g_config_system->name_color[0] * 255.f)
					| (static_cast<std::uint8_t>(sdk::g_config_system->name_color[1] * 255.f) << 8u)
					| (static_cast<std::uint8_t>(sdk::g_config_system->name_color[2] * 255.f) << 16u)
					| (static_cast<std::uint8_t>(sdk::g_config_system->name_color[3] * (int)m_dormant_data.at(index - 1).m_alpha) << 24u),
					1
				);
			}
		}

		std::size_t offset{};

		if (sdk::g_config_system->ammo
			&& wpn_data
			&& wpn_data->m_type != 0
			&& wpn_data->m_type < 7
			&& wpn_data->m_max_clip1 != -1) {
			const auto& layer1 = player->anim_layers().at(1u);
			const auto reloading = layer1.m_weight != 0.f && player->lookup_seq_act(layer1.m_sequence) == 967;

			const auto mod = reloading
				? layer1.m_cycle : weapon->ammo() / static_cast<float>(wpn_data->m_max_clip1);

			add_rect_filled({ bbox.m_min.x - 1, bbox.m_min.y + bbox.m_size.y + 2 }, { bbox.m_size.x + 2, 4 }, 0 | (0 << 8u) | (0 << 16u) | (static_cast<std::uint8_t>(std::clamp((int)m_dormant_data[index - 1].m_alpha, 0, 140)) << 24u));

			add_rect_filled({ bbox.m_min.x, bbox.m_min.y + bbox.m_size.y + 3 }, { bbox.m_size.x * mod, 2 },
				static_cast<std::uint8_t>(sdk::g_config_system->ammo_color[0] * 255.f)
				| (static_cast<std::uint8_t>(sdk::g_config_system->ammo_color[1] * 255.f) << 8u)
				| (static_cast<std::uint8_t>(sdk::g_config_system->ammo_color[2] * 255.f) << 16u)
				| (static_cast<std::uint8_t>(sdk::g_config_system->ammo_color[3] * (int)m_dormant_data.at(index - 1).m_alpha) << 24u)
			);

			if (!reloading
				&& weapon->ammo() < wpn_data->m_max_clip1)
				add_text({ bbox.m_min.x + bbox.m_size.x * mod, bbox.m_min.y + bbox.m_size.y },
					m_fonts.m_keybinds, std::to_string(weapon->ammo()).data(),
					255 | (255 << 8u) | (255 << 16u) | ((int)m_dormant_data.at(index - 1).m_alpha << 24u), 0
				);

			offset += 4;
		}

		if (sdk::g_config_system->distance) {
			constexpr auto k_meters_per_inch = 0.0254f;
			constexpr auto k_feet_per_meter = 3.281f;

			const auto dist = static_cast<int>(
				(valve::g_local_player->origin() - player->origin()).length()
				* k_meters_per_inch * k_feet_per_meter
				);

			add_text(
				{ bbox.m_min.x + bbox.m_size.x / 2.f, bbox.m_min.y + bbox.m_size.y + 2 + offset },
				m_fonts.m_keybinds, std::format("{} FT", dist).data(),
				255 | (255 << 8u) | (255 << 16u) | ((int)m_dormant_data.at(index - 1).m_alpha << 24u), 1
			);

			offset += 8u;
		}

		if (wpn_data
			&& wpn_data->m_hud_name
			&& sdk::g_config_system->weapon_text) {
			auto loc_name = util::to_utf8(valve::g_localize->find_safe(wpn_data->m_hud_name));

			std::transform(loc_name.begin(), loc_name.end(), loc_name.begin(), ::toupper);

			add_text(
				{ bbox.m_min.x + bbox.m_size.x / 2.f, bbox.m_min.y + bbox.m_size.y + 2 + offset },
				m_fonts.m_keybinds, loc_name.data(),
				static_cast<std::uint8_t>(sdk::g_config_system->weapon_color[0] * 255.f)
				| (static_cast<std::uint8_t>(sdk::g_config_system->weapon_color[1] * 255.f) << 8u)
				| (static_cast<std::uint8_t>(sdk::g_config_system->weapon_color[2] * 255.f) << 16u)
				| (static_cast<std::uint8_t>(sdk::g_config_system->weapon_color[3] * (int)m_dormant_data.at(index - 1).m_alpha) << 24u),
				1
			);

			offset += 8u;
		}

		if (weapon
			&& sdk::g_config_system->weapon_icon) {
			add_text(
				{ bbox.m_min.x + bbox.m_size.x / 2.f, bbox.m_min.y + bbox.m_size.y + 2 + offset },
				m_fonts.m_icons14, char_code_for_item_index(weapon->item_index()),
				static_cast<std::uint8_t>(sdk::g_config_system->weapon_color[0] * 255.f)
				| (static_cast<std::uint8_t>(sdk::g_config_system->weapon_color[1] * 255.f) << 8u)
				| (static_cast<std::uint8_t>(sdk::g_config_system->weapon_color[2] * 255.f) << 16u)
				| (static_cast<std::uint8_t>(sdk::g_config_system->weapon_color[3] * (int)m_dormant_data.at(index - 1).m_alpha) << 24u),
				1
			);
		}

		offset = 0u;

		if (sdk::g_config_system->money) {
			add_text(
				{ bbox.m_min.x + bbox.m_size.x + 2, bbox.m_min.y },
				m_fonts.m_keybinds, std::format("${}", player->money()).data(),
				150 | (200 << 8u) | (60 << 16u) | ((int)m_dormant_data.at(index - 1).m_alpha << 24u), 0
			);

			offset += 9;
		}

		if (sdk::g_config_system->flags) {
			const char* armor{};
			if (player->armor_value())
				armor = player->has_helmet() ? "HK" : "K";

			if (armor) {
				add_text(
					{ bbox.m_min.x + bbox.m_size.x + 2, bbox.m_min.y + offset },
					m_fonts.m_keybinds, armor,
					255 | (255 << 8u) | (255 << 16u) | ((int)m_dormant_data.at(index - 1).m_alpha << 24u), 0
				);

				offset += 9u;
			}

			if (player->scoped()) {
				add_text(
					{ bbox.m_min.x + bbox.m_size.x + 2, bbox.m_min.y + offset },
					m_fonts.m_keybinds, xorstr_("ZOOM"),
					60 | (180 << 8u) | (225 << 16u) | ((int)m_dormant_data.at(index - 1).m_alpha << 24u), 0
				);

				offset += 9u;
			}

			if (player->has_c4()) {
				add_text(
					{ bbox.m_min.x + bbox.m_size.x + 2, bbox.m_min.y + offset },
					m_fonts.m_keybinds, xorstr_("B"),
					255 | (0 << 8u) | (0 << 16u) | ((int)m_dormant_data.at(index - 1).m_alpha << 24u), 0
				);

				offset += 9u;
			}
		}

		if (dormant
			|| !sdk::g_config_system->skeleton)
			return;

		const auto mdl_data = player->mdl_data();
		if (!mdl_data
			|| !mdl_data->m_studio_hdr)
			return;

		vec2_t screen_pos{}, screen_parent_pos{};

		const auto& cached_bones = player->bone_cache().m_mem.m_ptr;
		const std::uint32_t skeleton_clr = 
			static_cast<std::uint8_t>(sdk::g_config_system->skeleton_color[0] * 255.f)
			| (static_cast<std::uint8_t>(sdk::g_config_system->skeleton_color[1] * 255.f) << 8u)
			| (static_cast<std::uint8_t>(sdk::g_config_system->skeleton_color[2] * 255.f) << 16u)
			| (static_cast<std::uint8_t>(sdk::g_config_system->skeleton_color[3] * (int)m_dormant_data.at(index - 1).m_alpha) << 24u);

		const auto& screen_size = ui::GetIO().DisplaySize;

		for (std::size_t i{}; i < mdl_data->m_studio_hdr->m_bones_count; ++i) {
			const auto bone = mdl_data->m_studio_hdr->bone(i);
			if (!bone
				|| !(bone->m_flags & 0x100)
				|| bone->m_parent == -1)
				continue;

			math::to_screen(
				{
					cached_bones[i][0][3],
					cached_bones[i][1][3],
					cached_bones[i][2][3]
				},
				screen_size,
				valve::g_engine->w2s_matrix(),
				screen_pos
			);

			math::to_screen(
				{
					cached_bones[bone->m_parent][0][3],
					cached_bones[bone->m_parent][1][3],
					cached_bones[bone->m_parent][2][3]
				},
				screen_size,
				valve::g_engine->w2s_matrix(),
				screen_parent_pos
			);

			add_line(screen_pos, screen_parent_pos, skeleton_clr);
		}
	}

	void c_visuals::handle_projectile(valve::c_entity* const entity, const valve::e_class_id class_id) {
		if (sdk::g_config_system->grenade_proximity_warning
			&& class_id != valve::e_class_id::flashbang
			&& (class_id == valve::e_class_id::molotov_projectile || class_id == valve::e_class_id::base_cs_grenade_projectile)) {
			const auto handle = entity->handle();

			if (entity->explode_effect_tick_begin())
				m_throwed_grenades.erase(handle);
			else {
				if (m_throwed_grenades.find(handle) == m_throwed_grenades.end()) {
					m_throwed_grenades.emplace(
						std::piecewise_construct,
						std::forward_as_tuple(handle),
						std::forward_as_tuple(
							static_cast<valve::c_player*>(valve::g_entity_list->find_entity(entity->thrower())),
							class_id == valve::e_class_id::molotov_projectile ? valve::e_item_index::molotov : valve::e_item_index::he_grenade,
							entity->origin(), static_cast<valve::c_player*>(entity)->velocity(),
							entity->grenade_spawn_time(), valve::to_ticks(entity->sim_time() - entity->grenade_spawn_time())
						)
					);
				}

				if (!add_grenade_simulation(m_throwed_grenades.at(handle), true))
					m_throwed_grenades.erase(handle);
			}
		}

		if (!sdk::g_config_system->grenades)
			return;

		vec2_t screen_pos{};
		if (!math::to_screen(entity->origin(), ui::GetIO().DisplaySize, valve::g_engine->w2s_matrix(), screen_pos))
			return;

		switch (class_id) {
		case valve::e_class_id::molotov_projectile:
			add_text(
				screen_pos, m_fonts.m_keybinds, xorstr_("MOLOTOV"),
				static_cast<std::uint8_t>(sdk::g_config_system->grenades_color[0] * 255.f)
				| (static_cast<std::uint8_t>(sdk::g_config_system->grenades_color[1] * 255.f) << 8u)
				| (static_cast<std::uint8_t>(sdk::g_config_system->grenades_color[2] * 255.f) << 16u)
				| (static_cast<std::uint8_t>(sdk::g_config_system->grenades_color[3] * 255.f) << 24u),
				2
			);

			break;
		case valve::e_class_id::decoy_projectile:
			add_text(
				screen_pos, m_fonts.m_keybinds, xorstr_("DECOY"),
				static_cast<std::uint8_t>(sdk::g_config_system->grenades_color[0] * 255.f)
				| (static_cast<std::uint8_t>(sdk::g_config_system->grenades_color[1] * 255.f) << 8u)
				| (static_cast<std::uint8_t>(sdk::g_config_system->grenades_color[2] * 255.f) << 16u)
				| (static_cast<std::uint8_t>(sdk::g_config_system->grenades_color[3] * 255.f) << 24u),
				2
			);

			break;
		case valve::e_class_id::base_cs_grenade_projectile:
			if (const auto model = entity->model()) {
				const std::string_view name{ model->m_name };

				if (name.find(xorstr_("flashbang")) != std::string::npos)
					add_text(
						screen_pos, m_fonts.m_keybinds, xorstr_("FLASH"),
						static_cast<std::uint8_t>(sdk::g_config_system->grenades_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->grenades_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->grenades_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->grenades_color[3] * 255.f) << 24u),
						2
					);
				else if (name.find(xorstr_("fraggrenade")) != std::string::npos)
					add_text(
						screen_pos, m_fonts.m_keybinds, xorstr_("FRAG"),
						static_cast<std::uint8_t>(sdk::g_config_system->grenades_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->grenades_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->grenades_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->grenades_color[3] * 255.f) << 24u),
						2
					);
			}

			break;
		case valve::e_class_id::inferno:
			add_text(
				screen_pos, m_fonts.m_keybinds, xorstr_("FIRE"),
				static_cast<std::uint8_t>(sdk::g_config_system->grenades_color[0] * 255.f)
				| (static_cast<std::uint8_t>(sdk::g_config_system->grenades_color[1] * 255.f) << 8u)
				| (static_cast<std::uint8_t>(sdk::g_config_system->grenades_color[2] * 255.f) << 16u)
				| (static_cast<std::uint8_t>(sdk::g_config_system->grenades_color[3] * 255.f) << 24u),
				2
			);

			//const auto lifetime = ((((float)((uintptr_t)entity + 0x20)) + 7.03125f) - valve::g_global_vars->m_cur_time) / 7.01325f;
			//add_rect_filled(vec2_t(screen_pos.x - 7, screen_pos.y + 5), vec2_t(30 * lifetime, 4),
			//	0xff0000ffu //|
			//	//static_cast<std::uint8_t>(m_cfg->m_other.m_grenades_clr[0] * 255.f)
			//	//| (static_cast<std::uint8_t>(m_cfg->m_other.m_grenades_clr[1] * 255.f) << 8u)
			//	//| (static_cast<std::uint8_t>(m_cfg->m_other.m_grenades_clr[2] * 255.f) << 16u)
			//);
			//add_rect(vec2_t(screen_pos.x - 7, screen_pos.y + 5), vec2_t(30, 4), ImColor(0, 0, 0, 255));
			break;
		case valve::e_class_id::smoke_grenade_projectile:
			add_text(
				screen_pos, m_fonts.m_keybinds, xorstr_("SMOKE"),
				static_cast<std::uint8_t>(sdk::g_config_system->grenades_color[0] * 255.f)
				| (static_cast<std::uint8_t>(sdk::g_config_system->grenades_color[1] * 255.f) << 8u)
				| (static_cast<std::uint8_t>(sdk::g_config_system->grenades_color[2] * 255.f) << 16u)
				| (static_cast<std::uint8_t>(sdk::g_config_system->grenades_color[3] * 255.f) << 24u),
				2
			);

			//const auto lifetime = ((((float)((uintptr_t)entity + 0x20)) + 18.f) - valve::g_global_vars->m_cur_time) / 18.f;

			//add_rect_filled(vec2_t(screen_pos.x - 10, screen_pos.y + 5), vec2_t(46 * lifetime, 4),
			//	0xff0000ffu /*|
			//	static_cast<std::uint8_t>(m_cfg->m_other.m_grenades_clr[0] * 255.f)
			//	| (static_cast<std::uint8_t>(m_cfg->m_other.m_grenades_clr[1] * 255.f) << 8u)
			//	| (static_cast<std::uint8_t>(m_cfg->m_other.m_grenades_clr[2] * 255.f) << 16u)*/
			//);
			//add_rect(vec2_t(screen_pos.x - 10, screen_pos.y + 5), vec2_t(46, 4), ImColor(0, 0, 0, 255));
		
			break;
		}
	}

	void c_visuals::handle_weapon(valve::c_weapon* const weapon) const {
		if (!sdk::g_config_system->dropped_weapons)
			return;

		const auto& origin = weapon->origin();
		if (origin.x == 0.f
			&& origin.y == 0.f
			&& origin.z == 0.f)
			return;

		vec2_t screen_pos{};
		if (!math::to_screen(origin, ui::GetIO().DisplaySize, valve::g_engine->w2s_matrix(), screen_pos))
			return;

		if (sdk::g_config_system->dropped_weapons == 1) {
			const auto wpn_data = weapon->wpn_data();
			if (!wpn_data)
				return;

			auto loc_name = util::to_utf8(valve::g_localize->find_safe(wpn_data->m_hud_name));

			std::transform(loc_name.begin(), loc_name.end(), loc_name.begin(), ::toupper);

			add_text(
				screen_pos, m_fonts.m_keybinds, loc_name.data(),
				static_cast<std::uint8_t>(sdk::g_config_system->dropped_weapons_color[0] * 255.f)
				| (static_cast<std::uint8_t>(sdk::g_config_system->dropped_weapons_color[1] * 255.f) << 8u)
				| (static_cast<std::uint8_t>(sdk::g_config_system->dropped_weapons_color[2] * 255.f) << 16u)
				| (static_cast<std::uint8_t>(sdk::g_config_system->dropped_weapons_color[3] * 255.f) << 24u),
				2
			);

			return;
		}

		add_text(
			screen_pos, m_fonts.m_icons14, char_code_for_item_index(weapon->item_index()),
			static_cast<std::uint8_t>(sdk::g_config_system->dropped_weapons_color[0] * 255.f)
			| (static_cast<std::uint8_t>(sdk::g_config_system->dropped_weapons_color[1] * 255.f) << 8u)
			| (static_cast<std::uint8_t>(sdk::g_config_system->dropped_weapons_color[2] * 255.f) << 16u)
			| (static_cast<std::uint8_t>(sdk::g_config_system->dropped_weapons_color[3] * 255.f) << 24u),
			2
		);
	}

	void c_visuals::init() {
		surface_font_name() = xorstr_("tahoma");
		m_fonts.m_tahoma12 = valve::g_surface->create_font();
		valve::g_font_mgr->set_font_glyph_set(m_fonts.m_tahoma12, surface_font_name().c_str(), 12, 400, 0, 0, 0x080u);

		surface_font_name() = xorstr_("verdana");
		m_fonts.m_verdana26 = valve::g_surface->create_font();
		valve::g_font_mgr->set_font_glyph_set(m_fonts.m_verdana26, surface_font_name().c_str(), 26, 800, 0, 0, 0x010u | 0x080u);

		m_fonts.m_verdana26n = valve::g_surface->create_font();
		valve::g_font_mgr->set_font_glyph_set(m_fonts.m_verdana26n, surface_font_name().c_str(), 26, 600, 0, 0, 0x010u);

		DWORD temp{};
		if (!AddFontMemResourceEx(reinterpret_cast<LPVOID>(main_font), sizeof(main_font), nullptr, &temp))
			return;

		surface_font_name() = xorstr_("main_font");
		m_fonts.m_menu = valve::g_surface->create_font();
		valve::g_font_mgr->set_font_glyph_set(m_fonts.m_menu, surface_font_name().c_str(), 12, 400, 0, 0, 0x080u, 0x20, 0xa69f);

		DWORD temp1{};
		if (!AddFontMemResourceEx(reinterpret_cast<LPVOID>(keybinds_font), sizeof(keybinds_font), nullptr, &temp1))
			return;

		surface_font_name() = xorstr_("keybinds_font");
		m_fonts.m_keybinds = valve::g_surface->create_font();
		valve::g_font_mgr->set_font_glyph_set(m_fonts.m_keybinds, surface_font_name().c_str(), 8, 100, 0, 0, 0x200u, 0x20, 0xa69f);

		DWORD temp2{};
		if (!AddFontMemResourceEx(reinterpret_cast<LPVOID>(qo0_icons), sizeof(qo0_icons), nullptr, &temp2))
			return;

		surface_font_name() = xorstr_("qo0_icons");
		m_fonts.m_icons14 = valve::g_surface->create_font();
		valve::g_font_mgr->set_font_glyph_set(m_fonts.m_icons14, surface_font_name().c_str(), 14, 0, 0, 0, 0x010u | 0x080u, 0xe000, 0xf8ff);

		DWORD temp3{};
		if (!AddFontMemResourceEx(reinterpret_cast<LPVOID>(qo0_icons), sizeof(qo0_icons), nullptr, &temp3))
			return;

		surface_font_name() = xorstr_("qo0_icons");
		m_fonts.m_icons26 = valve::g_surface->create_font();
		valve::g_font_mgr->set_font_glyph_set(m_fonts.m_icons26, surface_font_name().c_str(), 26, 0, 0, 0, 0x010u | 0x080u, 0xe000, 0xf8ff);
	}

	void c_visuals::watermark() {
		static int alpha = 255;
		static int low_alpha = 100;

		if ((!sdk::g_config_system->menu_watermark && alpha == 0) || !g_menu->is_menu_initialized())
			return;

		time_t t = std::time(nullptr);
		std::ostringstream time;
		time << std::put_time(std::localtime(&t), "%H:%M:%S");

		static int rate = (int)std::round(1.f / valve::g_global_vars->m_interval_per_tick);
		std::string logo = "supremacy";
#ifdef ALPHA
		logo.append(" [alpha]");
#else
#ifdef _DEBUG
		logo.append(" [debug]");
#endif
#endif
		if (sdk::g_config_system->menu_watermark) {
			alpha = std::min(alpha + 7, 255);
			low_alpha = std::min(low_alpha + 7, 100);
		}
		else {
			alpha = std::max(alpha - 7, 0);
			low_alpha = std::max(low_alpha - 7, 0);
		}

		std::string text = std::format("{} | {}", logo, time.str().data());

		if (valve::g_engine->in_game()) {
			const auto& net_info = g_context->net_info();

			int ms = std::max(0, (int)std::round(net_info.m_latency.m_out * 1000.f));
			text = std::format("{} | rtt: {}ms | rate: {} | {}", logo, ms, rate, time.str().data());
		}

		int width{}, height{};
		valve::g_font_mgr->calc_text_size(m_fonts.m_tahoma12, util::to_utf16(text).data(), width, height);

		const auto& screen_size = ui::GetIO().DisplaySize;

		add_rect_filled({ screen_size.x - (float)width - 23.f, 5.f }, { (float)width + 15.f, (float)height + 12.f },
			static_cast<std::uint8_t>(5)
			| (static_cast<std::uint8_t>(5) << 8u)
			| (static_cast<std::uint8_t>(5) << 16u)
			| (static_cast<std::uint8_t>(alpha) << 24u)
		);

		add_rect_filled({ screen_size.x - (float)width - 22.f, 6.f }, { (float)width + 13.f, (float)height + 10.f },
			static_cast<std::uint8_t>(75)
			| (static_cast<std::uint8_t>(75) << 8u)
			| (static_cast<std::uint8_t>(75) << 16u)
			| (static_cast<std::uint8_t>(alpha) << 24u)
		);

		add_rect_filled({ screen_size.x - (float)width - 21.f, 7.f }, { (float)width + 11.f, (float)height + 8.f },
			static_cast<std::uint8_t>(24)
			| (static_cast<std::uint8_t>(24) << 8u)
			| (static_cast<std::uint8_t>(24) << 16u)
			| (static_cast<std::uint8_t>(alpha) << 24u)
		);

		add_rect_filled({ screen_size.x - (float)width - 20.f, 8.f }, { (float)width + 9.f, (float)height - 12.f },
			static_cast<std::uint8_t>(sdk::g_config_system->menu_color[0] * 255.f)
			| (static_cast<std::uint8_t>(sdk::g_config_system->menu_color[1] * 255.f) << 8u)
			| (static_cast<std::uint8_t>(sdk::g_config_system->menu_color[2] * 255.f) << 16u)
			| (static_cast<std::uint8_t>(alpha) << 24u)
		);

		add_rect_filled({ screen_size.x - (float)width - 20.f, 9.f }, { (float)width + 9.f, (float)height - 12.f },
			static_cast<std::uint8_t>(sdk::g_config_system->menu_color[0] * 255.f)
			| (static_cast<std::uint8_t>(sdk::g_config_system->menu_color[1] * 255.f) << 8u)
			| (static_cast<std::uint8_t>(sdk::g_config_system->menu_color[2] * 255.f) << 16u)
			| (static_cast<std::uint8_t>(low_alpha) << 24u)
		);

		add_text(
			{ screen_size.x - 15 - width, 12 },
			m_fonts.m_tahoma12, text,
			static_cast<std::uint8_t>(186)
			| (static_cast<std::uint8_t>(186) << 8u)
			| (static_cast<std::uint8_t>(186) << 16u)
			| (static_cast<std::uint8_t>(alpha) << 24u),
			0
		);
	}

	void c_visuals::kill_feed() {
		if (!sdk::g_config_system->persistent_kill_feed)
			return;

		if (m_next_update > valve::g_global_vars->m_real_time)
			return;

		m_next_update = valve::g_global_vars->m_real_time + 2.f;

		using find_t = std::uintptr_t* (__thiscall*)(const std::uintptr_t, const char*);

		if (const auto death_notice = reinterpret_cast<find_t>(g_context->addresses().m_find_hud_element)(
			g_context->addresses().m_hud, xorstr_("CCSGO_HudDeathNotice")
			)) {
			const auto death_notice_panel = (*(valve::c_uipanel**)(*(death_notice + 17) + 4));
			const auto child_panel_count = death_notice_panel->get_child_count();

			for (int i = NULL; i < child_panel_count; ++i) {
				const auto child = death_notice_panel->get_child(i);
				if (!child)
					continue;

				if (child->has_class(xorstr_("DeathNotice_Killer")) && (sdk::g_config_system->persistent_kill_feed != 2 || child->has_class(xorstr_("DeathNoticeHeadShot"))))
					child->set_attribute_float(xorstr_("SpawnTime"), valve::g_global_vars->m_cur_time);
			}
		}
	}

	void c_visuals::on_paint() {
		watermark();

		if (!valve::g_engine->in_game())
			return;

		const auto& screen_size = ui::GetIO().DisplaySize;
		const auto screen_center = screen_size / 2.f;

		static auto last_server_tick = valve::g_client_state->m_server_tick;
		if (!sdk::g_config_system->grenade_proximity_warning
			|| valve::g_client_state->m_server_tick != last_server_tick)
			m_throwed_grenades.clear();

		last_server_tick = valve::g_client_state->m_server_tick;

		valve::c_entity* planted_bomb{};

		g_dormant_esp->start();

		for (int i{}; i <= valve::g_entity_list->highest_index(); ++i) {
			const auto entity = valve::g_entity_list->find_entity(i);
			if (!entity)
				continue;

			if (entity->is_player())
				handle_player(static_cast<valve::c_player* const>(entity));
			else if (const auto client_class = entity->client_class()) {
				if (entity->dormant())
					continue;

				if (entity->is_weapon())
					handle_weapon(static_cast<valve::c_weapon* const>(entity));
				else {
					if (client_class->m_class_id == valve::e_class_id::planted_c4)
						planted_bomb = entity;

					handle_projectile(entity, client_class->m_class_id);
				}
			}
		}

		if (sdk::g_config_system->hit_marker) {
			const auto time_delta = std::abs(valve::g_global_vars->m_cur_time - m_last_hurt_time);
			if (time_delta <= 0.5f) {
				const auto clr = 0x00c8c8c8u | static_cast<std::uint8_t>(240 * (1.f - (time_delta / 0.5f))) << 24u;

				constexpr auto k_size = 6;

				add_line(
					{ screen_center.x - k_size, screen_center.y - k_size },
					{ screen_center.x - (k_size / 2), screen_center.y - (k_size / 2) }, clr
				);
				add_line(
					{ screen_center.x - k_size, screen_center.y + k_size },
					{ screen_center.x - (k_size / 2), screen_center.y + (k_size / 2) }, clr
				);
				add_line(
					{ screen_center.x + k_size, screen_center.y + k_size },
					{ screen_center.x + (k_size / 2), screen_center.y + (k_size / 2) }, clr
				);
				add_line(
					{ screen_center.x + k_size, screen_center.y - k_size },
					{ screen_center.x + (k_size / 2), screen_center.y - (k_size / 2) }, clr
				);
			}
		}

		if (sdk::g_config_system->out_of_fow_arrow) {
			for (auto i = 1u; i <= valve::g_global_vars->m_max_clients; ++i) {
				const auto player = static_cast<valve::c_player*>(valve::g_entity_list->find_entity(i));
				if (!player
					|| player->dormant()
					|| !player->alive()
					|| player == valve::g_local_player
					|| (player->friendly() && !sdk::g_config_system->teammates))
					continue;

				oof_arrow(player->world_space_center());
			}
		}

		if (sdk::g_config_system->grenade_trajectory)
			add_grenade_simulation(m_grenade_trajectory, false);

		struct indicator_t { uint32_t color; std::string text; };
		std::vector< indicator_t > indicators{ };

		if (valve::g_local_player->alive()) {
			if (sdk::g_config_system->inaccuracy_overlay
				&& g_context->wpn_data()
				&& g_context->wpn_data()->m_type != 0
				&& g_context->wpn_data()->m_type < 7) {
				auto radius = (g_eng_pred->inaccuracy() + g_eng_pred->spread()) * 320.f;

				radius /= std::tan(math::to_rad(valve::g_local_player->fov()) * 0.5f) + std::numeric_limits< float >::epsilon();
				radius *= ui::GetIO().DisplaySize.y / 480.f;

				auto pos = screen_center;

				if (sdk::g_config_system->recoil_overlay) {
					const auto d = screen_size / valve::g_local_player->fov();

					pos.x -= d.x * valve::g_local_player->aim_punch().y;
					pos.y += d.y * valve::g_local_player->aim_punch().x;
				}

				add_filled_circle(
					pos, radius,
					std::max(16, static_cast<int>(std::round(radius * 0.75f))),
					static_cast<std::uint8_t>(sdk::g_config_system->inaccuracy_overlay_color[0] * 255.f)
					| (static_cast<std::uint8_t>(sdk::g_config_system->inaccuracy_overlay_color[1] * 255.f) << 8u)
					| (static_cast<std::uint8_t>(sdk::g_config_system->inaccuracy_overlay_color[2] * 255.f) << 16u)
					| (static_cast<std::uint8_t>(sdk::g_config_system->inaccuracy_overlay_color[3] * 255.f) << 24u)
				);
			}

			if (sdk::g_config_system->feature_indicators[0]
				&& (g_context->broke_lc() || valve::g_local_player->velocity().length_2d() > 270.f)
				) {
				indicator_t ind{ };
				ind.color = g_context->broke_lc() ? 0xff3cc896u : 0xff0000ffu;
				ind.text = xorstr_("LC");

				indicators.push_back(ind);
			}

			if (sdk::g_config_system->feature_indicators[5]
				&& key_handler::check_key(sdk::g_config_system->override_minimum_damage_key, sdk::g_config_system->override_minimum_damage_key_style)
				) {
				indicator_t ind{ };
				ind.color = 0xff3cc896u;
				ind.text = std::format("DMG {}", sdk::g_config_system->override_minimum_damage).data();

				indicators.push_back(ind);
			}

			if (sdk::g_config_system->feature_indicators[2]
				&& g_exploits->type() == 1
				) {
				indicator_t ind{ };
				ind.color = 0xff3cc896u;
				ind.text = xorstr_("HS");

				indicators.push_back(ind);
			}

			if (sdk::g_config_system->feature_indicators[1]) {
				if (g_exploits->type() == 2
					|| g_exploits->type() == 3
					|| g_exploits->type() == 4
					) {
					indicator_t ind{ };
					ind.color = g_exploits->dt_ready() ? 0xff3cc896u : 0xff0000ffu;
					ind.text = xorstr_("DT");

					indicators.push_back(ind);
				}
			} {}

			if (sdk::g_config_system->feature_indicators[4]
				&& key_handler::check_key(sdk::g_config_system->safe_point_key, sdk::g_config_system->safe_point_key_style)
				) {
				indicator_t ind{ };
				ind.color = 0xff3cc896u;
				ind.text = xorstr_("SAFE");

				indicators.push_back(ind);
			}

			if (sdk::g_config_system->feature_indicators[3]
				&& key_handler::check_key(sdk::g_config_system->body_aim_key, sdk::g_config_system->body_aim_key_style)
				) {
				indicator_t ind{ };
				ind.color = 0xff3cc896u;
				ind.text = xorstr_("BODY");

				indicators.push_back(ind);
			}

			if (sdk::g_config_system->feature_indicators[6]
				&& key_handler::check_key(sdk::g_config_system->peek_assistence_key, sdk::g_config_system->peek_assistence_key_style)
				) {
				indicator_t ind{ };
				ind.color = g_context->should_return() ? 0xff0000ffu : 0xff3cc896u;
				ind.text = xorstr_("PA");

				indicators.push_back(ind);
			}

			if (sdk::g_config_system->feature_indicators[7]){
				const auto x = screen_size.x / 2;
				const auto y = screen_size.y / 2;

				if (g_anti_aim->manual_side() == 1) {
					add_rect_filled({ x - 50, y - 7 }, { 1, 1 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 50, y + 7 }, { 1, 1 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 51, y - 7 }, { 1, 3 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 51, y + 5 }, { 1, 3 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 52, y - 6 }, { 1, 4 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 52, y + 3 }, { 1, 4 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 53, y - 6 }, { 1, 5 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 53, y + 2 }, { 1, 5 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 54, y - 5 }, { 1, 5 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 54, y + 1 }, { 1, 5 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 55, y - 5 }, { 1, 11 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 56, y - 4 }, { 1, 9 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 57, y - 4 }, { 1, 9 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 58, y - 3 }, { 1, 7 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 59, y - 3 }, { 1, 7 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 60, y - 2 }, { 1, 5 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 61, y - 2 }, { 1, 5 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 62, y - 1 }, { 1, 3 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 63, y - 1 }, { 1, 3 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 64, y }, { 1, 1 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
				}
				else if (g_anti_aim->manual_side() == 2) {
					add_rect_filled({ x + 50, y - 7 }, { 1, 1 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x + 50, y + 7 }, { 1, 1 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x + 51, y - 7 }, { 1, 3 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x + 51, y + 5 }, { 1, 3 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x + 52, y - 6 }, { 1, 4 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x + 52, y + 3 }, { 1, 4 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x + 53, y - 6 }, { 1, 5 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x + 53, y + 2 }, { 1, 5 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x + 54, y - 5 }, { 1, 5 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x + 54, y + 1 }, { 1, 5 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x + 55, y - 5 }, { 1, 11 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x + 56, y - 4 }, { 1, 9 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x + 57, y - 4 }, { 1, 9 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x + 58, y - 3 }, { 1, 7 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x + 59, y - 3 }, { 1, 7 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x + 60, y - 2 }, { 1, 5 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x + 61, y - 2 }, { 1, 5 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x + 62, y - 1 }, { 1, 3 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x + 63, y - 1 }, { 1, 3 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x + 64, y }, { 1, 1 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
				}
				else if (g_anti_aim->manual_side() == 3) {
					add_rect_filled({ x , y + 60 }, { 1, 1 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 1 , y + 59 }, { 3, 1 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 1 , y + 58 }, { 3, 1 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 2 , y + 57 }, { 5, 1 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 2 , y + 56 }, { 5, 1 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 3 , y + 55 }, { 7, 1 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 3 , y + 54 }, { 7, 1 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 4 , y + 53 }, { 9, 1 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 4 , y + 52 }, { 9, 1 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 5 , y + 51 }, { 11, 1 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 5 , y + 50 }, { 5, 1 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 6 , y + 49 }, { 5, 1 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 6 , y + 48 }, { 4, 1 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 7 , y + 47 }, { 3, 1 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x - 7 , y + 46 }, { 1, 1 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x + 1 , y + 50 }, { 5, 1 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x + 2 , y + 49 }, { 5, 1 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x + 3 , y + 48 }, { 4, 1 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x + 5 , y + 47 }, { 3, 1 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
					add_rect_filled({ x + 7 , y + 46 }, { 1, 1 },
						static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[0] * 255.f)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[1] * 255.f) << 8u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[2] * 255.f) << 16u)
						| (static_cast<std::uint8_t>(sdk::g_config_system->manual_anti_aimbot_angles_color[3] * 255.f) << 24u)
					);
				}
			}
		}

		if (planted_bomb
			&& planted_bomb->is_bomb_ticking()
			&& sdk::g_config_system->bomb) {
			constexpr auto k_range = 500.f;
			constexpr auto k_radius = k_range * 3.5f;
			constexpr auto k_sigma = k_radius / 3.f;

			auto dist = (planted_bomb->origin() - valve::g_local_player->origin()).length();
			auto fall_off = std::exp(-dist * dist / (2.f * k_sigma * k_sigma));
			auto dmg = k_range * fall_off;
			auto hp = static_cast<float>(valve::g_local_player->health());

			if (!valve::g_local_player->alive()) {
				const auto target = static_cast<valve::c_player*>(valve::g_entity_list->find_entity(valve::g_local_player->observer_target()));

				if (target
					&& target->index() >= 1
					&& target->index() <= 64)
				{
					dist = (planted_bomb->origin() - target->origin()).length();
					fall_off = std::exp(-dist * dist / (2.f * k_sigma * k_sigma));
					dmg = k_range * fall_off;
					hp = static_cast<float>(target->health());

					g_auto_wall->scale_dmg(target, dmg, 1.f, 0);
				}
			}
			else
				g_auto_wall->scale_dmg(valve::g_local_player, dmg, 1.f, 0);

			const auto mod = std::clamp(dmg / hp, 0.f, 1.f);

			if (hp >= 1.f
				&& dmg >= 1.f) {
				indicator_t ind{ };
				ind.color = 0xb4000000u | static_cast<std::uint8_t>(255.f * mod)
					| (static_cast<std::uint8_t>(255.f * (1.f - mod)) << 8u);
				ind.text = dmg >= hp ? xorstr_("FATAL") : std::format("-{} HP", static_cast<int>(dmg)).data();

				indicators.push_back(ind);
			}

			indicator_t ind{ };
			ind.color = 0xb4ffffffu;

			const auto blow_time = std::max(0.f, planted_bomb->c4_blow() - valve::g_global_vars->m_cur_time);
			if (blow_time)
				ind.text = std::format("{} - {:.1f}s",
					planted_bomb->bomb_site() == 0 ? ("A") : ("B"),
					blow_time
				).data();
			else
				ind.text = std::format("{}",
					planted_bomb->bomb_site() == 0 ? ("A") : ("B")
				).data();		

			indicators.push_back(ind);
		}

		if (!indicators.empty()) {
			for (size_t i{ }; i < indicators.size(); ++i) {
				auto& indicator = indicators[i];

				add_text(
					vec2_t(20, screen_size.y - 80 - (30 * i)), m_fonts.m_verdana26,
					indicator.text, indicator.color,
					0
				);
			}
		}

		if (!valve::g_local_player->alive())
			return;

		if (sdk::g_config_system->spectators) {
			std::vector< std::string > spectators{};

			for (auto i = 1; i <= valve::g_global_vars->m_max_clients; ++i) {
				const auto player = static_cast<valve::c_player*>(valve::g_entity_list->find_entity(i));
				if (!player
					|| player == valve::g_local_player
					|| player->alive()
					|| player->dormant())
					continue;

				const auto target = valve::g_entity_list->find_entity(player->observer_target());
				if (!target
					|| target->index() != valve::g_local_player->index())
					continue;

				const auto info = valve::g_engine->player_info(i);
				if (!info.has_value())
					continue;

				spectators.emplace_back(info.value().m_name);
			}

			int width{}, height{};
			valve::g_font_mgr->calc_text_size(m_fonts.m_menu, util::to_utf16(xorstr_("A")).data(), width, height);

			std::size_t offset{};
			for (const auto& name : spectators) {
				add_text({ screen_size.x - 10, static_cast<float>((sdk::g_config_system->menu_watermark ? 35 : 10) + offset) }, m_fonts.m_menu, name.data(), 0xb4ffffffu, 4);

				offset += (height - 1);
			}
		}

		if (!sdk::g_config_system->penetration_reticle
			|| !m_pen_data.has_value())
			return;

		const auto center = screen_size / 2.f;

		const auto& pen = m_pen_data.value();

		add_rect_filled({ center.x - 1.f, center.y - 1.f }, { 3.f, 3.f },
			pen.m_dmg < 1 ? pen.m_remaining_pen != 4 ? 0xb400ff00u : 0xb40000ffu : 0xb4ff0000u
		);
	}

	void c_visuals::on_render_start() {
		auto& smoke_count = **reinterpret_cast<int**>(g_context->addresses().m_smoke_count);

		m_backup_smoke_count = smoke_count;

		if (sdk::g_config_system->remove_smoke_grenades)
			smoke_count = 0;

		**reinterpret_cast<bool**>(
			g_context->addresses().m_disable_post_processing
			) = sdk::g_config_system->disable_post_processing;

		if (sdk::g_config_system->visualize_sounds)
			for (auto i = m_player_sounds.begin(); i != m_player_sounds.end(); i = m_player_sounds.erase(i))
				add_beam_ring(*i, sdk::g_config_system->visualize_sounds_color);
		else
			m_player_sounds.clear();

		static int last_impacts_count{};

		struct client_hit_verify_t {
			vec3_t	m_pos{};
			float	m_time{};
			float	m_expires{};
		};

		const auto& client_impacts_list = *reinterpret_cast<valve::utl_vec_t< client_hit_verify_t >*>(
			reinterpret_cast<std::uintptr_t>(valve::g_local_player.operator supremacy::valve::c_player * ()) + (TWENTYTWENTY ? 0xbc00u : 0xbbc8u)
			);

		if (sdk::g_config_system->bullet_impacts)
			for (auto i = client_impacts_list.m_size; i > last_impacts_count; --i)
				valve::g_debug_overlay->add_box_overlay(client_impacts_list.at(i - 1).m_pos, { -2.f, -2.f, -2.f }, { 2.f, 2.f, 2.f }, {}, 255, 0, 0, 127, 4.f);

		last_impacts_count = client_impacts_list.m_size;

		if (sdk::g_config_system->bullet_tracers
			|| sdk::g_config_system->bullet_impacts)
			for (auto i = m_bullet_impacts.begin(); i != m_bullet_impacts.end(); i = m_bullet_impacts.erase(i)) {
				if (sdk::g_config_system->bullet_impacts)
					valve::g_debug_overlay->add_box_overlay(i->m_pos, { -2.f, -2.f, -2.f }, { 2.f, 2.f, 2.f }, {}, 0, 0, 255, 127, 4.f);

				if (i->m_final
					&& sdk::g_config_system->bullet_tracers)
					add_beam(i->m_from, i->m_pos, sdk::g_config_system->bullet_tracers_color);
			}
		else
			m_bullet_impacts.clear();

		static const std::array< valve::material_t*, 4u > smoke_materials{
			valve::g_mat_system->find_material(xorstr_("particle/vistasmokev1/vistasmokev1_fire"), nullptr),
			valve::g_mat_system->find_material(xorstr_("particle/vistasmokev1/vistasmokev1_smokegrenade"), nullptr),
			valve::g_mat_system->find_material(xorstr_("particle/vistasmokev1/vistasmokev1_emods"), nullptr),
			valve::g_mat_system->find_material(xorstr_("particle/vistasmokev1/vistasmokev1_emods_impactdust"), nullptr)
		};

		static const std::array< valve::material_t*, 2u > flash_materials{
			valve::g_mat_system->find_material(xorstr_("effects/flashbang"), nullptr),
			valve::g_mat_system->find_material(xorstr_("effects/flashbang_white"), nullptr)
		};

		for (auto& material : smoke_materials)
			if (material)
				material->set_flag(4, sdk::g_config_system->remove_smoke_grenades);

		for (auto& material : flash_materials)
			if (material)
				material->set_flag(4, sdk::g_config_system->remove_flashbang_effects);

		if (valve::g_local_player && valve::g_local_player->alive()) {
			const auto position = supremacy::g_context->start_position();
			static auto alpha = 0.f;
			static auto rad = 0.f;

			if (position.x != 0.f && position.y != 0.f && position.z != 0.f && (key_handler::check_key(sdk::g_config_system->peek_assistence_key, sdk::g_config_system->peek_assistence_key_style) || alpha)) {
				if (key_handler::check_key(sdk::g_config_system->peek_assistence_key, sdk::g_config_system->peek_assistence_key_style)) {
					alpha += 3.f * valve::g_global_vars->m_frame_time;
					rad += 270.f * valve::g_global_vars->m_frame_time;
				}
				else {
					alpha -= 3.f * valve::g_global_vars->m_frame_time;
					rad -= 270.f * valve::g_global_vars->m_frame_time;
				}

				alpha = std::clamp(alpha, 0.f, 1.f);
				rad = std::clamp(rad, 0.f, 90.f);

				auto light = valve::g_effects->cl_alloc_dlight(valve::g_local_player->index());				
				light->die = valve::g_global_vars->m_cur_time + 0.05f;
				light->radius = rad;
				light->r = sdk::g_config_system->peek_assistence_color[0] * 255.f;
				light->g = sdk::g_config_system->peek_assistence_color[1] * 255.f;
				light->b = sdk::g_config_system->peek_assistence_color[2] * 255.f;
				light->exponent = 6;		
				light->key = valve::g_local_player->index();
				light->origin = position;
				light->flags &= 0x1u | 0x2u;
				light->decay = rad;				
				light->outer_angle = 360.f;
				light->style = 1;
			}
		}
	}

	void c_visuals::on_render_end() {
		if (sdk::g_config_system->remove_smoke_grenades)
			**reinterpret_cast<int**>(g_context->addresses().m_smoke_count) = m_backup_smoke_count;
	}

	void c_visuals::on_post_data_update_start() {		
		static auto set_clan_tag = [&](std::string tag) -> void {
			const auto tag_c_str = tag.c_str();
			g_context->addresses().m_set_clan_tag(tag_c_str, tag_c_str);
			};

		static bool reset = false;
		static float last_change_time = 0.f;
		static bool reset_tag = true;
		static int last_clantag_time = 0;

		const auto& net_info = g_context->net_info();
		auto clantag_time = (int)((valve::g_global_vars->m_cur_time * 2.f) + net_info.m_latency.m_out);

		if (sdk::g_config_system->clan_tag_spammer) {			
			if ((*valve::g_game_rules)->game_phase() == 4
				|| (*valve::g_game_rules)->game_phase() == 5) {
				if (clantag_time != last_clantag_time) {
					set_clan_tag(xorstr_("supremacy "));
					last_clantag_time = clantag_time;
				}
			}
			else {
				static int prev_array_stage = 0;
				static char tag_stages[] = {
					0x1u, 0x2u, 0x3u, 0x4u, 0x5u, 0x6u, 0x7u, 0x8u, 0x9u, 0xau, 0xbu, 0xbu,
					0xbu, 0xbu, 0xbu, 0xbu, 0xbu, 0xbu, 0xcu, 0xdu, 0xeu, 0xfu, 0x10u, 0x11u,
					0x12u, 0x13u, 0x14u, 0x15u, 0x16u
				};

				auto time = 0.3f / valve::g_global_vars->m_interval_per_tick;
				auto iter = valve::g_client_state->m_server_tick / static_cast<int> (time + 0.5f) % 30;
				auto new_array_stage = tag_stages[iter];

				if (prev_array_stage != new_array_stage) {
					char tag[40] = { 0 };
					strcpy(tag, xorstr_("             supremacy                "));
					char* current_tag = &tag[new_array_stage];
					current_tag[15] = '\0';
					set_clan_tag(current_tag);
					prev_array_stage = new_array_stage;
				}
			}

			reset_tag = false;
		}
		else if (!reset_tag) {
			if (clantag_time != last_clantag_time) {
				set_clan_tag("");

				reset_tag = true;
				last_clantag_time = clantag_time;
			}
		}
	}

	void c_visuals::on_pre_create_move(const valve::user_cmd_t& cmd) {
		static size_t last_spammed = 0;
		static int counter = 0;

		static std::string chat_spam[] = {
		xorstr_("feel your invulnerability with supremacy"),
		xorstr_("pay only 3$ for domination"),
		xorstr_("still n1 since 2018"),
		xorstr_("t.me/s1legendirl")
		};

		static auto say_in_chat = [&](const char* text) -> void {
			char buffer[250];
			sprintf_s(buffer, xorstr_("say \"%s\""), text);
			valve::g_engine->exec_cmd(buffer);
			};

		if (sdk::g_config_system->chat_spammer) {
			if (GetTickCount() - last_spammed > 800)
			{
				last_spammed = GetTickCount();
				if (++counter > 3)
					counter = 0;

				say_in_chat(chat_spam[counter].c_str());
			}
		}

		if (sdk::g_config_system->name_spammer) {
			const auto voice_loopback = g_context->cvars().m_voice_loopback->get_int();
			static bool flip = false;
			g_context->cvars().m_name->m_callbacks.m_size = 0;
			g_context->cvars().m_name->set_str("\x81 supremacy⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶⁶supremacy");
			g_context->cvars().m_voice_loopback->set_int(flip);
			flip = !flip;	
			g_context->cvars().m_voice_loopback->set_int(voice_loopback);
		}
	}

	void c_visuals::on_create_move(const valve::user_cmd_t& cmd) {		
		m_grenade_trajectory = {};
		m_pen_data = std::nullopt;

		if (!g_context->weapon()
			|| !g_context->wpn_data())
			return;

		qangle_t dir{};

		if (sdk::g_config_system->penetration_reticle
			&& g_context->wpn_data()->m_type != 0
			&& g_context->wpn_data()->m_type < 7) {
			math::angle_vectors(cmd.m_view_angles, &dir);

			m_pen_data = g_auto_wall->fire_bullet(
				valve::g_local_player, nullptr, g_context->wpn_data(),
				g_context->weapon()->item_index() == valve::e_item_index::taser,
				g_context->shoot_pos(), g_context->shoot_pos() + dir * g_context->wpn_data()->m_range
			);
		}

		if (!sdk::g_config_system->grenade_trajectory
			|| g_context->wpn_data()->m_type != 9
			|| (!g_context->weapon()->pin_pulled() && g_context->weapon()->throw_time() == 0.f))
			return;

		m_grenade_trajectory.m_owner = valve::g_local_player;
		m_grenade_trajectory.m_index = g_context->weapon()->item_index();

		auto view_angles = cmd.m_view_angles;

		if (view_angles.x < -90.f)
			view_angles.x += 360.f;
		else if (view_angles.x > 90.f)
			view_angles.x -= 360.f;

		view_angles.x -= (90.f - std::fabsf(view_angles.x)) * 10.f / 90.f;

		math::angle_vectors(view_angles, &dir);

		const auto throw_strength = std::clamp(g_context->weapon()->throw_strength(), 0.f, 1.f);

		auto src = g_context->shoot_pos();

		src.z += throw_strength * 12.f - 12.f;

		valve::trace_t trace{};
		valve::trace_filter_simple_t trace_filter{ valve::g_local_player, 0 };

		valve::g_engine_trace->trace_ray(
			{ src, src + dir * 22.f, { -2.f, -2.f, -2.f }, { 2.f, 2.f, 2.f } },
			valve::e_mask::solid | valve::e_mask::contents_current_90,
			reinterpret_cast<valve::trace_filter_t*>(&trace_filter), &trace
		);

		const auto velocity = std::clamp(
			g_context->wpn_data()->m_throw_velocity * 0.9f, 15.f, 750.f
		) * (throw_strength * 0.7f + 0.3f);

		m_grenade_trajectory.predict(
			trace.m_end_pos - dir * 6.f,
			dir * velocity + valve::g_local_player->velocity() * 1.25f,
			valve::g_global_vars->m_cur_time, 0
		);
	}

	void c_visuals::on_override_view(valve::view_setup_t* const setup) {
		static bool keep_height{};
		static bool once{};

		if (!valve::g_engine->in_game()) {
			keep_height = false;

			return;
		}

		if (valve::g_local_player->alive()) {
			if (g_movement->should_fake_duck()) {
				keep_height = true;

				setup->m_origin.z = valve::g_local_player->abs_origin().z + 64.0625f;
			}
			else if (keep_height) {
				const auto& local_data = g_eng_pred->local_data().at(g_context->last_cmd_number() % 150);
				if (local_data.m_net_vars.m_duck_amount != local_data.m_pred_net_vars.m_duck_amount) {
					if (local_data.m_pred_net_vars.m_duck_amount > 0.05f)
						setup->m_origin.z = valve::g_local_player->abs_origin().z + 64.0625f;
					else
						keep_height = false;
				}
				else
					keep_height = false;
			}
		}
		else
			keep_height = false;

		if (valve::g_local_player->alive()
			&& sdk::g_config_system->force_third_person_alive
			&& key_handler::check_key(sdk::g_config_system->third_person_key, sdk::g_config_system->third_person_key_style)
			) {			
			const auto view_angles = valve::g_engine->view_angles();
			valve::g_input->m_camera_in_third_person = true;
			valve::g_input->m_camera_offset = vec3_t(view_angles.x, view_angles.y, sdk::g_config_system->third_person_distance);

			const auto cam_hull_offset = 12.f + ((sdk::g_config_system->override_fov_amount) / 4.8f - 18.f);
			const vec3_t cam_hull_extents(cam_hull_offset, cam_hull_offset, cam_hull_offset);
			vec3_t cam_forward;
			const auto origin = g_context->shoot_pos();

			math::angle_vectors(qangle_t(valve::g_input->m_camera_offset.x, valve::g_input->m_camera_offset.y, 0.f), &cam_forward);

			valve::trace_t trace;
			valve::trace_filter_world_only_t filter;

			valve::ray_t ray;
			ray.m_delta = origin - origin - (cam_forward * valve::g_input->m_camera_offset.z);
			ray.m_is_swept = (ray.m_delta.length_sqr() != 0.f);
			ray.m_world_axis_transform = NULL;
			ray.m_start_offset = vec3_t(0.f, 0.f, 0.f);
			ray.m_start = origin;
			ray.m_extents = cam_hull_extents;
			ray.m_is_ray = false;

			valve::g_engine_trace->trace_ray(ray, valve::e_mask::solid & ~valve::e_mask::contents_monster, &filter, &trace);
			
			valve::g_input->m_camera_offset.z *= trace.m_fraction;
		}
		else if (valve::g_input->m_camera_in_third_person)		
			valve::g_input->m_camera_in_third_person = false;		
				
		if (valve::g_local_player->alive() 
			|| !sdk::g_config_system->force_third_person_dead
			) {
			once = false;
			return;
		}

		if (once)
			valve::g_local_player->observer_mode() = 5;

		if (valve::g_local_player->observer_mode() == 4)
			once = true;
	}

	void c_visuals::on_post_screen_effects() {
		if (!valve::g_engine->in_game())
			return m_shot_mdls.clear();

		if (sdk::g_config_system->bomb) {
			for (int i{}; i < valve::g_glow_mgr->m_objects.m_size; ++i) {
				auto& object = valve::g_glow_mgr->m_objects.at(i);
				if (!object.m_entity
					|| object.m_next_free_slot != -2)
					continue;

				const auto client_class = object.m_entity->client_class();
				if (!client_class
					|| client_class->m_class_id != valve::e_class_id::planted_c4)
					continue;

				object.m_render_when_occluded = true;
				object.m_render_when_unoccluded = false;
				object.m_red = sdk::g_config_system->bomb_color[0];
				object.m_green = sdk::g_config_system->bomb_color[1];
				object.m_blue = sdk::g_config_system->bomb_color[2];
				object.m_alpha = sdk::g_config_system->bomb_color[3];
			}
		}

		if (!sdk::g_config_system->on_shot
			|| m_shot_mdls.empty())
			return;

		const auto context = valve::g_mat_system->render_context();
		if (!context)
			return;

		m_rendering_shot_mdl = true;

		for (auto i = m_shot_mdls.begin(); i != m_shot_mdls.end(); ) {
			const auto delta = (i->m_time + sdk::g_config_system->on_shot_time) - valve::g_global_vars->m_real_time;
			if (delta <= 0.f 
				|| !i->m_bones.data()
				|| !i->m_world_matrix.m_matrix
				|| !i->m_state.m_renderable) {
				i = m_shot_mdls.erase(i);

				continue;
			}
						
			override_material(
				sdk::g_config_system->on_shot_model, true,
				sdk::g_config_system->on_shot_color[0],
				sdk::g_config_system->on_shot_color[1],
				sdk::g_config_system->on_shot_color[2],
				sdk::g_config_system->on_shot_color[3] * (delta / sdk::g_config_system->on_shot_time)
			);

			valve::g_model_render->draw_model(context, i->m_state, i->m_info, i->m_bones.data());

			valve::g_studio_render->override_material(nullptr);

			i = std::next(i);
		}

		m_rendering_shot_mdl = false;
	}

	bool c_visuals::on_draw_model(
		valve::studio_render_t* const ecx, const std::uintptr_t edx,
		const std::uintptr_t a0, const valve::draw_model_info_t& info,
		mat3x4_t* const bones, float* const a3, float* const a4, const vec3_t& origin, int flags
	) const {
		if (!valve::g_local_player
			|| !valve::g_engine->in_game()
			|| ecx->forced_material_override()
			|| !info.m_renderable || !info.m_studio_hdr)
			return true;

		if (m_rendering_shot_mdl)
			return true;

		const auto entity = reinterpret_cast<valve::c_entity* const>(info.m_renderable - 0x4);
		if (!entity)
			return true;

		const auto client_class = entity->client_class();
		if (!client_class)
			return true;

		const auto move_parent = valve::g_entity_list->find_entity(entity->move_parent());
		const auto move_parent_player = move_parent && move_parent->index() >= 1 && move_parent->index() <= 64;
		const auto player_mdl = *reinterpret_cast<const std::uint32_t*>(info.m_studio_hdr->m_name + 14u) == 'yalp';

		if (sdk::g_config_system->disable_rendering_of_teammates
			&& ((player_mdl && entity->friendly() && entity != valve::g_local_player)
				|| (move_parent_player && move_parent->friendly() && move_parent != valve::g_local_player)))
			return false;

		if (sdk::g_config_system->disable_rendering_of_ragdolls
			&& client_class->m_class_id == valve::e_class_id::ragdoll)
			return false;

		if (player_mdl) {
			if (!sdk::g_config_system->ragdolls
				&& client_class->m_class_id == valve::e_class_id::ragdoll)
				return true;

			const auto friendly = entity->friendly();
			const auto local_player = entity == valve::g_local_player;

			if (!friendly && !local_player) {
				if (entity->is_player()
					&& sdk::g_config_system->shadow
					&& g_context->cvars().m_cl_lagcompensation->get_bool()) {
					auto lerped_bones = try_to_lerp_bones(entity->index());
					if (lerped_bones.has_value()) {
						override_material(
							sdk::g_config_system->shadow_model, true,
							sdk::g_config_system->shadow_color[0],
							sdk::g_config_system->shadow_color[1],
							sdk::g_config_system->shadow_color[2],
							sdk::g_config_system->shadow_color[3]
						);

						hooks::orig_draw_model(
							ecx, edx, a0, info,
							lerped_bones.value().data(), a3, a4, origin, flags
						);

						valve::g_studio_render->override_material(nullptr);
					}
				}

				if (sdk::g_config_system->player_behind_wall) {
					override_material(
						sdk::g_config_system->player_behind_wall_model, true,
						sdk::g_config_system->player_behind_wall_color[0],
						sdk::g_config_system->player_behind_wall_color[1],
						sdk::g_config_system->player_behind_wall_color[2],
						sdk::g_config_system->player_behind_wall_color[3]
					);

					hooks::orig_draw_model(
						ecx, edx, a0, info, bones, a3, a4, origin, flags
					);

					valve::g_studio_render->override_material(nullptr);
				}

				if (sdk::g_config_system->player) {
					override_material(
						sdk::g_config_system->player_model, false,
						sdk::g_config_system->player_color[0],
						sdk::g_config_system->player_color[1],
						sdk::g_config_system->player_color[2],
						sdk::g_config_system->player_color[3]
					);

					hooks::orig_draw_model(
						ecx, edx, a0, info, bones, a3, a4, origin, flags
					);

					valve::g_studio_render->override_material(nullptr);

					return false;
				}
			}
			else if (friendly && !local_player) {
				if (sdk::g_config_system->teammate_behind_wall) {
					override_material(
						sdk::g_config_system->teammate_behind_wall_model, true,
						sdk::g_config_system->teammate_behind_wall_color[0],
						sdk::g_config_system->teammate_behind_wall_color[1],
						sdk::g_config_system->teammate_behind_wall_color[2],
						sdk::g_config_system->teammate_behind_wall_color[3]
					);

					hooks::orig_draw_model(
						ecx, edx, a0, info, bones, a3, a4, origin, flags
					);

					valve::g_studio_render->override_material(nullptr);
				}

				if (sdk::g_config_system->teammate) {
					override_material(
						sdk::g_config_system->teammate_model, false,
						sdk::g_config_system->teammate_color[0],
						sdk::g_config_system->teammate_color[1],
						sdk::g_config_system->teammate_color[2],
						sdk::g_config_system->teammate_color[3]
					);

					hooks::orig_draw_model(
						ecx, edx, a0, info, bones, a3, a4, origin, flags
					);

					valve::g_studio_render->override_material(nullptr);

					return false;
				}
			}
			else if (local_player) {
				if (sdk::g_config_system->local_player_fake) {
					override_material(
						sdk::g_config_system->local_player_fake_model, false,
						sdk::g_config_system->local_player_fake_color[0],
						sdk::g_config_system->local_player_fake_color[1],
						sdk::g_config_system->local_player_fake_color[2],
						sdk::g_config_system->local_player_fake_color[3]
					);

					hooks::orig_draw_model(
						ecx, edx, a0, info,
						g_anim_sync->local_data().m_fake.m_bones.data(),
						a3, a4, origin, flags
					);

					valve::g_studio_render->override_material(nullptr);
				}

				if (sdk::g_config_system->local_player) {
					override_material(
						sdk::g_config_system->local_player_model, false,
						sdk::g_config_system->local_player_color[0],
						sdk::g_config_system->local_player_color[1],
						sdk::g_config_system->local_player_color[2],
						sdk::g_config_system->local_player_color[3]
					);

					hooks::orig_draw_model(
						ecx, edx, a0, info, bones, a3, a4, origin, flags
					);

					valve::g_studio_render->override_material(nullptr);

					return false;
				}
			}
		}
		else if (*reinterpret_cast<const std::uint32_t*>(info.m_studio_hdr->m_name + 17u) == 'smra') {
			if (sdk::g_config_system->hands) {
				override_material(
					sdk::g_config_system->hands_model, false,
					sdk::g_config_system->hands_color[0],
					sdk::g_config_system->hands_color[1],
					sdk::g_config_system->hands_color[2],
					sdk::g_config_system->hands_color[3]
				);

				hooks::orig_draw_model(
					ecx, edx, a0, info, bones, a3, a4, origin, flags
				);

				valve::g_studio_render->override_material(nullptr);

				return false;
			}
		}
		else if (*reinterpret_cast<const std::uint32_t*>(info.m_studio_hdr->m_name) == 'paew'
			&& info.m_studio_hdr->m_name[8] == 'v') {
			if (sdk::g_config_system->weapon_viewmodel) {
				override_material(
					sdk::g_config_system->weapon_viewmodel_model, false,
					sdk::g_config_system->weapon_viewmodel_color[0],
					sdk::g_config_system->weapon_viewmodel_color[1],
					sdk::g_config_system->weapon_viewmodel_color[2],
					sdk::g_config_system->weapon_viewmodel_color[3]
				);

				hooks::orig_draw_model(
					ecx, edx, a0, info, bones, a3, a4, origin, flags
				);

				valve::g_studio_render->override_material(nullptr);

				return false;
			}
		}
		else if (move_parent_player) {
			if (sdk::g_config_system->weapons) {
				override_material(
					sdk::g_config_system->weapons_model, false,
					sdk::g_config_system->weapons_color[0],
					sdk::g_config_system->weapons_color[1],
					sdk::g_config_system->weapons_color[2],
					sdk::g_config_system->weapons_color[3]
				);

				hooks::orig_draw_model(
					ecx, edx, a0, info, bones, a3, a4, origin, flags
				);

				valve::g_studio_render->override_material(nullptr);

				return false;
			}
		}

		return true;
	}

	void c_visuals::on_calc_view(
		valve::c_player* const ecx, const std::uintptr_t edx,
		vec3_t& origin, qangle_t& angles, float& z_near, float& z_far, float& fov
	) {
		const auto backup_aim_punch = valve::g_local_player->aim_punch();
		const auto backup_view_punch = valve::g_local_player->view_punch();
		const auto backup_use_new_anim_state = valve::g_local_player->use_new_anim_state();

		valve::g_local_player->use_new_anim_state() = false;

		if (sdk::g_config_system->visual_recoil_adjustment) {
			switch (sdk::g_config_system->visual_recoil_adjustment) {
			case 1:
				valve::g_local_player->view_punch() = {};
				break;
			case 2:
				valve::g_local_player->aim_punch()
					= valve::g_local_player->view_punch() = {};
				break;
			}
		}

		hooks::orig_calc_view(ecx, edx, origin, angles, z_near, z_far, fov);

		valve::g_local_player->use_new_anim_state() = backup_use_new_anim_state;

		valve::g_local_player->aim_punch() = backup_aim_punch;
		valve::g_local_player->view_punch() = backup_view_punch;
	}

	void c_visuals::add_shot_mdl(valve::c_player* const player, const std::shared_ptr< lag_record_t >& lag_record) {
		const auto model = player->model();
		if (!model)
			return;

		const auto mdl_data = player->mdl_data();
		if (!mdl_data
			|| !mdl_data->m_studio_hdr)
			return;

		auto& shot_mdl = m_shot_mdls.emplace_back();

		shot_mdl.m_time = valve::g_global_vars->m_real_time;
		shot_mdl.m_state.m_studio_hdr = mdl_data->m_studio_hdr;
		shot_mdl.m_state.m_studio_hw_data = valve::g_mdl_cache->lookup_hw_data(model->m_studio);
		shot_mdl.m_state.m_renderable = player->renderable();

		const auto& anim_side = lag_record->m_side < 3 ? lag_record->m_sides.at(lag_record->m_side) : lag_record->m_low_sides.at(lag_record->m_side - 3);

		shot_mdl.m_info.m_model = model;
		shot_mdl.m_info.m_hitbox_set = player->hitbox_set_index();
		shot_mdl.m_info.m_skin = player->skin();
		shot_mdl.m_info.m_body = player->body();
		shot_mdl.m_info.m_index = player->index();
		shot_mdl.m_info.m_origin = lag_record->m_origin;
		shot_mdl.m_info.m_angles.y = anim_side.m_foot_yaw;

		shot_mdl.m_info.m_instance = player->model_instance();
		shot_mdl.m_info.m_flags = 0x1;

		std::memcpy(
			shot_mdl.m_bones.data(), anim_side.m_bones.data(),
			anim_side.m_bones_count * sizeof(mat3x4_t)
		);

		g_context->addresses().m_angle_matrix(shot_mdl.m_info.m_angles, shot_mdl.m_world_matrix);

		shot_mdl.m_world_matrix[0][3] = lag_record->m_origin.x;
		shot_mdl.m_world_matrix[1][3] = lag_record->m_origin.y;
		shot_mdl.m_world_matrix[2][3] = lag_record->m_origin.z;

		shot_mdl.m_info.m_model_to_world = shot_mdl.m_state.m_model_to_world = &shot_mdl.m_world_matrix;
	}
}