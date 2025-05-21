#include "../../../supremacy.hpp"

namespace supremacy::hacks {
	void c_lag_comp::on_net_update() {
		const auto tick_rate = valve::to_ticks(1.f);

		for (auto i = 1; i <= valve::g_global_vars->m_max_clients; ++i) {
			auto& entry = m_entries.at(i - 1);

			const auto player = static_cast<valve::c_player*>(
				valve::g_entity_list->find_entity(i)
				);

			if (player == valve::g_local_player) {
				entry.reset();

				g_visuals->send_net_data(player);
				continue;
			}

			if (entry.m_player != player)
				entry.reset();			

			entry.m_player = player;

			if (!player
				|| !player->alive()) {
				entry.reset();

				continue;
			}

			if (player->dormant()) {
				entry.m_try_lby_resolver = true;
				entry.m_try_trace_resolver = true;
				entry.m_try_anim_resolver = true;
				entry.m_misses = entry.m_trace_side = 0;
				entry.m_left_dormancy = true;

				if (entry.m_lag_records.empty()) {
					entry.m_lag_records.emplace_back(
						std::make_shared< lag_record_t >(player)
					);

					continue;
				}

				if (!entry.m_lag_records.back()->m_dormant) {
					entry.m_lag_records.clear();

					entry.m_lag_records.emplace_back(
						std::make_shared< lag_record_t >(player)
					);

					continue;
				}

				continue;
			}

			g_visuals->send_net_data(player);

			const auto anim_state = player->anim_state();
			if (!anim_state) {
				entry.reset();

				continue;
			}

			if (player->sim_time() == player->old_sim_time())
				continue;	

			const auto& cur_alive_loop_cycle = player->anim_layers().at(11).m_cycle;
			if (cur_alive_loop_cycle == entry.m_alive_loop_cycle) {
				player->sim_time() = player->old_sim_time();
				continue;
			}

			entry.m_alive_loop_cycle = cur_alive_loop_cycle;
			entry.m_receive_time = valve::g_global_vars->m_real_time;
			entry.m_render_origin = player->origin();

			if (entry.m_spawn_time != player->spawn_time()) {
				anim_state->reset();

				entry.m_try_lby_resolver = entry.m_try_trace_resolver = entry.m_try_anim_resolver = true;
				entry.m_misses = entry.m_prev_side = entry.m_trace_side = 0;

				entry.m_lag_records.clear();
			}

			entry.m_spawn_time = player->spawn_time();

			lag_record_t* previous = nullptr;
			lag_record_t* penultimate = nullptr;

			if (!entry.m_lag_records.empty()) {
				previous = entry.m_lag_records.back().get();
				if (entry.m_lag_records.size() > 1)
					penultimate = entry.m_lag_records.at(entry.m_lag_records.size() - 2).get();
			}

			const auto current = entry.m_lag_records.emplace_back(
				std::make_shared< lag_record_t >(player)
			).get();

			g_anim_sync->on_net_update(entry, current, previous, penultimate);

			while (entry.m_lag_records.size() > tick_rate)
				entry.m_lag_records.pop_front();

			entry.m_left_dormancy = false;
		}
	}
}