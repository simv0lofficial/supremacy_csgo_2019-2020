#include "../../../supremacy.hpp"
#include "../../../dependencies/imgui/settings.hpp"

namespace supremacy::hacks {
	void c_shots::on_net_update() {
		for (auto& shot : m_elements) {
			if (shot.m_processed
				|| !shot.m_process_tick
				|| valve::g_global_vars->m_tick_count < shot.m_process_tick)
				continue;

			if (shot.m_target.m_entry
				&& shot.m_target.m_lag_record
				&& shot.m_target.m_entry->m_player
				&& shot.m_target.m_entry->m_player->alive()) {
				lag_backup_t lag_backup{ shot.m_target.m_entry->m_player };

				if (shot.m_server_info.m_hurt_tick) {
					if (shot.m_target.m_lag_record->m_trying_to_resolve) {
						valve::trace_t trace{};

						auto new_anim_side = shot.m_target.m_lag_record->m_side;
						auto wrong_anim_side = shot.m_target.m_lag_record->m_side;

						if (new_anim_side > 2)
							new_anim_side = 0;

						if (wrong_anim_side > 2)
							wrong_anim_side = 0;

						for (int i{}; ; ) {
							new_anim_side = (new_anim_side + i) % 3;

							shot.m_target.m_lag_record->restore(shot.m_target.m_entry->m_player, new_anim_side, i != 0);

							valve::g_engine_trace->clip_ray_to_entity(
								{ shot.m_src, shot.m_server_info.m_impact_pos },
								valve::e_mask::shot_player, shot.m_target.m_entry->m_player, &trace
							);

							lag_backup.restore(shot.m_target.m_entry->m_player);

							if (trace.m_hit_entity == shot.m_target.m_entry->m_player
								&& trace.m_hitgroup == shot.m_server_info.m_hitgroup)
								break;

							++i;

							if (i >= 3) {
								new_anim_side = shot.m_target.m_lag_record->m_side;

								break;
							}
						}

						if (new_anim_side != shot.m_target.m_lag_record->m_side) {
							shot.m_target.m_entry->m_prev_side = shot.m_target.m_lag_record->m_side = new_anim_side;

							for (auto& lag_record : shot.m_target.m_entry->m_lag_records) {
								if (!lag_record->m_trying_to_resolve
									|| lag_record->m_side != wrong_anim_side)
									continue;

								lag_record->m_side = new_anim_side;
								lag_record->m_priority = std::min(3, lag_record->m_priority);
							}
						}

						shot.m_target.m_lag_record->m_priority = std::min(3, shot.m_target.m_lag_record->m_priority);
					}
				}
				else {
					shot.m_target.m_lag_record->restore(shot.m_target.m_entry->m_player, shot.m_target.m_lag_record->m_side);

					valve::trace_t trace{};

					valve::g_engine_trace->clip_ray_to_entity(
						{ shot.m_src, shot.m_server_info.m_impact_pos },
						valve::e_mask::shot_player, shot.m_target.m_entry->m_player, &trace
					);

					lag_backup.restore(shot.m_target.m_entry->m_player);

					if (!trace.m_hit_entity
						|| !trace.m_hit_entity->is_player()
						|| trace.m_hit_entity != shot.m_target.m_entry->m_player) {
						if (((shot.m_src - shot.m_target.m_point.m_pos).length() - 32.f) > (shot.m_src - shot.m_server_info.m_impact_pos).length()) {
#ifdef ALPHA
							util::g_notify->print_logo();
							util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("missed shot due to "));
							util::g_notify->print_notify(true, true, 0xff60a4f4u, xorstr_("occlusion\n"));
#else
#ifdef _DEBUG
							util::g_notify->print_logo();
							util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("missed shot due to "));
							util::g_notify->print_notify(true, true, 0xff60a4f4u, xorstr_("occlusion\n"));
#else
							if (g_context->debug_build) {
								util::g_notify->print_logo();
								util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("missed shot due to "));
								util::g_notify->print_notify(true, true, 0xff60a4f4u, xorstr_("occlusion\n"));
							}
							else if (sdk::g_config_system->log_misses_due_to_spread) {
								util::g_notify->print_logo();
								util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("missed shot due to "));
								util::g_notify->print_notify(true, true, 0xff60a4f4u, xorstr_("spread\n"));
							}
#endif
#endif
						}
						else if (sdk::g_config_system->log_misses_due_to_spread) {
							util::g_notify->print_logo();
							util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("missed shot due to "));
							util::g_notify->print_notify(true, true, 0xff60a4f4u, xorstr_("spread\n"));
						}
					}
					else {
						++shot.m_target.m_entry->m_misses;

						if ((shot.m_target.m_entry->m_player->flags() & valve::e_ent_flags::fake_client)
							|| shot.m_target.m_point.m_intersections >= 3) {
#ifdef ALPHA
							util::g_notify->print_logo();
							util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("missed shot due to "));
							util::g_notify->print_notify(true, true, 0xff60a4f4u, xorstr_("unknown reason\n"));
#else
#ifdef _DEBUG
							util::g_notify->print_logo();
							util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("missed shot due to "));
							util::g_notify->print_notify(true, true, 0xff60a4f4u, xorstr_("unknown reason\n"));
#else
							if (g_context->debug_build) {
								util::g_notify->print_logo();
								util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("missed shot due to "));
								util::g_notify->print_notify(true, true, 0xff60a4f4u, xorstr_("unknown reason\n"));
							}
#endif
#endif
						}
						else {
							if (!shot.m_target.m_lag_record->m_trying_to_resolve) {
#ifdef ALPHA
								util::g_notify->print_logo();
								util::g_notify->print_notify(true, true, 0xff998877u, xorstr_("missed shot due to "));
								util::g_notify->print_notify(true, true, 0xff0045ffu, xorstr_("animation desync\n"));
#else
#ifdef _DEBUG
								util::g_notify->print_logo();
								util::g_notify->print_notify(true, true, 0xff998877u, xorstr_("missed shot due to "));
								util::g_notify->print_notify(true, true, 0xff0045ffu, xorstr_("animation desync\n"));
#else
								if (g_context->debug_build) {
									util::g_notify->print_logo();
									util::g_notify->print_notify(true, true, 0xff998877u, xorstr_("missed shot due to "));
									util::g_notify->print_notify(true, true, 0xff0045ffu, xorstr_("animation desync\n"));
								}
#endif
#endif
							}
							else {
#ifdef ALPHA
								util::g_notify->print_logo();
								util::g_notify->print_notify(true, true, 0xff998877u, xorstr_("missed shot due to "));
								util::g_notify->print_notify(true, true, 0xff0045ffu, xorstr_("bad resolve\n"));
#else
#ifdef _DEBUG
								util::g_notify->print_logo();
								util::g_notify->print_notify(true, true, 0xff998877u, xorstr_("missed shot due to "));
								util::g_notify->print_notify(true, true, 0xff0045ffu, xorstr_("bad resolve\n"));
#else
								if (g_context->debug_build) {
									util::g_notify->print_logo();
									util::g_notify->print_notify(true, true, 0xff998877u, xorstr_("missed shot due to "));
									util::g_notify->print_notify(true, true, 0xff0045ffu, xorstr_("bad resolve\n"));
								}
#endif
#endif
							}							

							if (shot.m_target.m_lag_record->m_trying_to_resolve
								&& shot.m_target.m_point.m_intersections < 3
								&& (shot.m_target.m_lag_record->m_flags & valve::e_ent_flags::on_ground)
								&& !shot.m_target.m_lag_record->m_shot
								&& !shot.m_target.m_lag_record->m_throw) {
								if (shot.m_target.m_lag_record->m_side
									&& shot.m_target.m_entry->m_prev_type == 1)
									shot.m_target.m_entry->m_try_lby_resolver = false;

								if (shot.m_target.m_lag_record->m_side
									&& shot.m_target.m_entry->m_prev_type == 2)
									shot.m_target.m_entry->m_try_trace_resolver = false;

								if (shot.m_target.m_lag_record->m_side
									&& shot.m_target.m_entry->m_prev_type == 3)
									shot.m_target.m_entry->m_try_anim_resolver = false;

								const auto wrong_anim_side = shot.m_target.m_lag_record->m_side;
								auto new_anim_side = shot.m_target.m_lag_record->m_side;

								switch (shot.m_target.m_lag_record->m_side) {
								case 0:
									new_anim_side = 1;
									break;
								case 1:
									new_anim_side = 2;
									break;
								case 2:
									new_anim_side = 1;
									break;
								case 3:
									new_anim_side = 2;
									break;
								case 4:
									new_anim_side = 1;
									break;
								}

								shot.m_target.m_entry->m_prev_side = shot.m_target.m_lag_record->m_side = new_anim_side;
#ifdef ALPHA
								util::g_notify->print_logo();
								util::g_notify->print_notify(true, false, 0xff998877u, xorstr_("corrected side from %d to %d\n"), wrong_anim_side, new_anim_side);
#else
#ifdef _DEBUG
								util::g_notify->print_logo();
								util::g_notify->print_notify(true, false, 0xff998877u, xorstr_("corrected side from %d to %d\n"), wrong_anim_side, new_anim_side);
#else
								if (g_context->debug_build) {
									util::g_notify->print_logo();
									util::g_notify->print_notify(true, false, 0xff998877u, xorstr_("corrected side from %d to %d\n"), wrong_anim_side, new_anim_side);
								}
#endif
#endif
								for (auto& lag_record : shot.m_target.m_entry->m_lag_records) {
									if (!lag_record->m_trying_to_resolve
										|| lag_record->m_side != wrong_anim_side)
										continue;

									lag_record->m_side = new_anim_side;
									lag_record->m_priority = std::min(3, lag_record->m_priority);
								}

								shot.m_target.m_lag_record->m_priority = std::min(3, shot.m_target.m_lag_record->m_priority);
							}
						}
					}
				}
			}

			shot.m_processed = true;
		}

		const auto avg_latency = static_cast<int>(valve::g_engine->net_channel_info()->avg_latency(0) * 1000.f);

		m_elements.erase(std::remove_if(
			m_elements.begin(), m_elements.end(),
			[&](const shot_t& shot) {
				if (shot.m_processed
					|| !shot.m_target.m_entry
					|| !shot.m_target.m_entry->m_player
					|| !shot.m_target.m_entry->m_player->alive()
					|| shot.m_cmd_number == -1
					|| shot.m_process_tick
					|| shot.m_server_info.m_fire_tick
					|| valve::g_client_state->m_last_cmd_ack < (shot.m_cmd_number + avg_latency))
					return false;
#ifdef ALPHA
				if (valve::g_local_player->alive()) {
					util::g_notify->print_logo();
					util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("missed shot due to "));
					util::g_notify->print_notify(true, true, 0xff60a4f4u, xorstr_("ping "));
					util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("(unregistered shot)\n"));
				}
				else {
					util::g_notify->print_logo();
					util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("missed shot due to "));
					util::g_notify->print_notify(true, true, 0xff60a4f4u, xorstr_("ping "));
					util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("(local death)\n"));
				}
#else
#ifdef _DEBUG
				if (valve::g_local_player->alive()) {
					util::g_notify->print_logo();
					util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("missed shot due to "));
					util::g_notify->print_notify(true, true, 0xff60a4f4u, xorstr_("ping "));
					util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("(unregistered shot)\n"));
				}
				else {
					util::g_notify->print_logo();
					util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("missed shot due to "));
					util::g_notify->print_notify(true, true, 0xff60a4f4u, xorstr_("ping "));
					util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("(local death)\n"));
				}
#else
				if (!g_context->debug_build)
					return true;

				if (valve::g_local_player->alive()) {
					util::g_notify->print_logo();
					util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("missed shot due to "));
					util::g_notify->print_notify(true, true, 0xff60a4f4u, xorstr_("ping "));
					util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("(unregistered shot)\n"));
				}
				else {
					util::g_notify->print_logo();
					util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("missed shot due to "));
					util::g_notify->print_notify(true, true, 0xff60a4f4u, xorstr_("ping "));
					util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("(local death)\n"));
				}
#endif
#endif
				return true;
			}
		),
			m_elements.end()
		);
	}

	void c_shots::on_new_event(valve::game_event_t* const event) {
		if (!valve::g_local_player)
			return;

		m_elements.erase(std::remove_if(m_elements.begin(), m_elements.end(),
			[&](const shot_t& shot) {
				return std::abs(shot.m_time - valve::g_global_vars->m_real_time) > 1.f;
			}
		), m_elements.end());

		if (!strcmp(event->name(), xorstr_("player_footstep"))) {
			if (!sdk::g_config_system->visualize_sounds)
				return;

			const auto entity = valve::g_entity_list->find_entity(valve::g_engine->index_for_uid(event->get_int(xorstr_("userid"))));
			if (!entity
				|| entity->dormant()
				|| entity == valve::g_local_player
				|| (entity->friendly() && !sdk::g_config_system->teammates))
				return;

			g_visuals->player_sounds().emplace_back(entity->origin());
		}

		if (!strcmp(event->name(), xorstr_("player_hurt"))) {
			const auto index = valve::g_engine->index_for_uid(event->get_int(xorstr_("userid")));
			const auto hp = event->get_int(xorstr_("health"));

			hacks::g_visuals->m_dormant_data.at(index - 1).m_health = hp;
			if (valve::g_engine->index_for_uid(event->get_int(xorstr_("attacker"))) != valve::g_local_player->index())
				return;

			const auto victim = valve::g_entity_list->find_entity(index);
			if (!victim)
				return;

			const auto hitgroup = event->get_int(xorstr_("hitgroup"));
			if (hitgroup == 10)
				return;

			if (sdk::g_config_system->log_damage_dealt)
				if (const auto player_info = valve::g_engine->player_info(victim->index()); player_info.has_value()) {
					auto damage = event->get_int(xorstr_("dmg_health"));
					damage = std::clamp(damage, 0, 100);
					const int g = 255 - damage * 2.55;
					const int r = damage * 2.55;

					util::g_notify->print_logo();
					util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("hit %s in %s for "), player_info.value().m_name, util::translate_hitgroup(hitgroup));
					util::g_notify->print_notify(true, true, r | (g << 8u) | (0 << 16u) | (255 << 24u), xorstr_("%d "), event->get_int(xorstr_("dmg_health")));
					util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("damage (%d health remaining)\n"), hp);
				}

			if (sdk::g_config_system->hit_marker_sound)
				valve::g_engine->exec_cmd(xorstr_("play buttons\\arena_switch_press_02.wav"));

			g_visuals->last_hurt_time() = valve::g_global_vars->m_cur_time;

			const auto shot = last_unprocessed();
			if (!shot
				|| (shot->m_target.m_entry && shot->m_target_index != victim->index()))
				return;

			shot->m_server_info.m_hitgroup = hitgroup;
			shot->m_server_info.m_hurt_tick = valve::g_client_state->m_server_tick;
		}

		if (!strcmp(event->name(), xorstr_("weapon_fire"))) {
			const auto index = valve::g_engine->index_for_uid(event->get_int(xorstr_("userid")));

			if (index != valve::g_local_player->index())
				return;

			if (m_elements.empty())
				return;

			const auto shot = std::find_if(
				m_elements.begin(), m_elements.end(),
				[](const shot_t& shot) {
					return shot.m_cmd_number != -1
						&& !shot.m_server_info.m_fire_tick
						&& std::abs(valve::g_client_state->m_cmd_ack - shot.m_cmd_number) <= 17;
				}
			);

			if (shot == m_elements.end())
				return;

			shot->m_process_tick = valve::g_global_vars->m_tick_count + 1;
			shot->m_server_info.m_fire_tick = valve::g_client_state->m_server_tick;
		}

		if (!strcmp(event->name(), xorstr_("bullet_impact"))) {
			const auto index = valve::g_engine->index_for_uid(event->get_int(xorstr_("userid")));
			if (index != valve::g_local_player->index())
				return;

			const vec3_t impact_pos{
				event->get_float(xorstr_("x")),
				event->get_float(xorstr_("y")),
				event->get_float(xorstr_("z"))
			};

			if (const auto shot = last_unprocessed())
				shot->m_server_info.m_impact_pos = impact_pos;

			if (!sdk::g_config_system->bullet_tracers
				&& !sdk::g_config_system->bullet_impacts)
				return;

			auto& vis_impacts = g_visuals->bullet_impacts();

			if (!vis_impacts.empty()
				&& vis_impacts.back().m_time == valve::g_global_vars->m_cur_time)
				vis_impacts.back().m_final = false;

			vis_impacts.emplace_back(
				valve::g_global_vars->m_cur_time,
				valve::g_local_player->origin() + valve::g_local_player->view_offset(),
				impact_pos
			);
		}

		if (!strcmp(event->name(), xorstr_("item_equip"))) {
			const auto index = valve::g_engine->index_for_uid(event->get_int(xorstr_("userid")));
			if (!index
				|| index > 64)
				return;

			hacks::g_visuals->m_dormant_data.at(index - 1).m_weapon_id = event->get_int(xorstr_("defindex"));
			hacks::g_visuals->m_dormant_data.at(index - 1).m_weapon_type = event->get_int(xorstr_("weptype"));
		}

		if (!strcmp(event->name(), xorstr_("item_purchase"))) {
			if (!sdk::g_config_system->log_weapon_purchases)
				return;

			const auto index = valve::g_engine->index_for_uid(event->get_int(xorstr_("userid")));
			if (index == valve::g_local_player->index())
				return;

			const auto purchaser = valve::g_entity_list->find_entity(index);
			if (!purchaser
				|| purchaser->friendly())
				return;

			if (const auto player_info = valve::g_engine->player_info(index); player_info.has_value()) {
				const auto weapon = event->get_str(xorstr_("weapon"));
				if (weapon == xorstr_("weapon_unknown"))
					return;

				util::g_notify->print_logo();
				util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("%s bought %s\n"), player_info.value().m_name, weapon);
			}
		}

		if (!strcmp(event->name(), xorstr_("round_freeze_end"))) {
			g_context->freeze_time() = false;
		}

		if (!strcmp(event->name(), xorstr_("round_prestart"))) {
			g_context->freeze_time() = true;
			g_context->start_position() = vec3_t{};
			g_context->should_auto_buy() = true;

			g_visuals->next_update() = valve::g_global_vars->m_real_time + 10.f;

			for (std::size_t i{}; i < 64u; ++i) {
				g_visuals->m_dormant_data.at(i).m_health = 100;
				g_visuals->m_dormant_data.at(i).m_receive_time = 0.f;
				g_visuals->m_dormant_data.at(i).m_alpha = 0.f;
				g_visuals->m_dormant_data.at(i).m_weapon_id = 0;
				g_visuals->m_dormant_data.at(i).m_weapon_type = -1;
				g_dormant_esp->m_sound_players[i].m_receive_time = 0.f;
				g_dormant_esp->m_sound_players[i].m_origin = {};
				
				auto& entry = g_lag_comp->entry(i);

				entry.m_misses = 0;
				entry.m_try_lby_resolver = true;
				entry.m_try_trace_resolver = true;
				entry.m_try_anim_resolver = true;
			}
		}
	}
}