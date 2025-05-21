#include "../../../supremacy.hpp"
#include "../dependencies/threading/new_threads.h"

enum Hitboxes_t : int {
	HITBOX_HEAD = 0,
	HITBOX_NECK,
	// HITBOX_LOWER_NECK, // remove this for desync version
	HITBOX_PELVIS,
	HITBOX_STOMACH,
	HITBOX_LOWER_CHEST,
	HITBOX_CHEST,
	HITBOX_UPPER_CHEST,
	HITBOX_R_THIGH,
	HITBOX_L_THIGH,
	HITBOX_R_CALF,
	HITBOX_L_CALF,
	HITBOX_R_FOOT,
	HITBOX_L_FOOT,
	HITBOX_R_HAND,
	HITBOX_L_HAND,
	HITBOX_R_UPPER_ARM,
	HITBOX_R_FOREARM,
	HITBOX_L_UPPER_ARM,
	HITBOX_L_FOREARM,
	HITBOX_MAX
};

namespace supremacy::hacks {
	std::size_t c_aim_bot::calc_points_count(const int hitgroups, const int multi_points) const {
		std::size_t ret{};

		for (std::size_t i{}, fl = 1; i < m_hitgroups.size(); ++i, fl <<= 1) {
			if (hitgroups & fl
				|| (i == 2 && sdk::g_config_system->body_aim_conditions[0]
					|| sdk::g_config_system->body_aim_conditions[1]
					|| sdk::g_config_system->body_aim_conditions[2]
					|| sdk::g_config_system->body_aim_conditions[3]
					|| sdk::g_config_system->body_aim_conditions[4]
					|| sdk::g_config_system->body_aim_conditions[5]
					)) {
				if (!(multi_points & fl))
					++ret;
				else {
					for (auto& hitbox : m_hitgroups.at(i)) {
						if (hitbox == 11 || hitbox == 12)
							ret += 3u;
						else if (hitbox) {
							if (hitbox == 3 || hitbox == 2)
								ret += 4u;
							else if (hitbox == 6 || hitbox == 4 || hitbox == 5)
								ret += 2u;
							else
								ret += 1u;
						}
						else
							ret += 6u;
					}
				}
			}
		}

		return ret;
	}

	void c_aim_bot::scan_point(const aim_target_t& target, aim_point_t& point, const bool ignore_dmg) const {
		const auto view_angles = valve::g_engine->view_angles();
		const auto& shoot_pos = g_context->shoot_pos();
		const auto fov = math::calc_fov(view_angles, shoot_pos, point.m_pos);
		if (fov > sdk::g_config_system->maximum_fov)
			return;

		const auto is_taser = g_context->weapon()->item_index() == valve::e_item_index::taser;
		point.m_pen_data = g_auto_wall->fire_bullet(
			valve::g_local_player, target.m_entry->m_player, g_context->wpn_data(),
			is_taser, shoot_pos, point.m_pos
		);

		if (point.m_pen_data.m_dmg < 1)
			return;

		if (!sdk::g_config_system->automatic_penetration
			&& point.m_pen_data.m_remaining_pen < 4)
			return;

		const auto& pen = point.m_pen_data;
		const auto hp = target.m_entry->m_player->health();

		if (!ignore_dmg) {
			auto min_dmg = pen.m_remaining_pen == 4 ? sdk::g_config_system->minimum_damage : sdk::g_config_system->penetration_minimum_damage;

			if (key_handler::check_key(sdk::g_config_system->override_minimum_damage_key, sdk::g_config_system->override_minimum_damage_key_style))
				min_dmg = sdk::g_config_system->override_minimum_damage;

			min_dmg = std::min(min_dmg, hp + 5);

			if (is_taser)
				min_dmg = hp + 5;

			if (pen.m_dmg < min_dmg)
				return;
		}

		if (target.m_entry->m_player->flags() & valve::e_ent_flags::fake_client) {
			point.m_intersections = 3;
			point.m_valid = true;

			return;
		}

		// todo: simv0l - use TraceToStudioCsgoHitgroupsPriority instead of this math-based shit
		// like in fatality (https://github.com/BulletGG/fatality-source-code/blob/master/internal_hvh/features/wall_penetration.cpp#L536-L543)
		const auto intersect = [](
			const vec3_t& shoot_pos, const vec3_t& point,
			const valve::studio_hitbox_t* const hitbox, const mat3x4_t& matrix
			) {
			vec3_t min{}, max{};

			math::vector_transform(hitbox->m_min, matrix, min);
			math::vector_transform(hitbox->m_max, matrix, max);

			if (hitbox->m_radius > 0.f)
				return math::segment_to_segment(shoot_pos, point, min, max) < hitbox->m_radius;

			math::vector_i_transform(shoot_pos, matrix, min);
			math::vector_i_rotate(point, matrix, max);

			return math::intersect_line_with_bb(min, max, hitbox->m_min, hitbox->m_max);
		};

		const auto& zero_bones = target.m_lag_record->m_sides.at(0).m_bones;
		const auto& negative_bones = target.m_lag_record->m_sides.at(1).m_bones;
		const auto& positive_bones = target.m_lag_record->m_sides.at(2).m_bones;
		const auto& low_negative_bones = target.m_lag_record->m_low_sides.at(0).m_bones;
		const auto& low_positive_bones = target.m_lag_record->m_low_sides.at(1).m_bones;
		const auto hitbox_set = target.m_entry->m_player->mdl_data()->m_studio_hdr->hitbox_set(target.m_entry->m_player->hitbox_set_index());
		const auto hitbox = hitbox_set->hitbox(point.m_hitbox);

		if (zero_bones[hitbox->m_bone].m_matrix[0][3] != negative_bones[hitbox->m_bone].m_matrix[0][3]
			&& zero_bones[hitbox->m_bone].m_matrix[0][3] != positive_bones[hitbox->m_bone].m_matrix[0][3]
			&& zero_bones[hitbox->m_bone].m_matrix[0][3] != low_negative_bones[hitbox->m_bone].m_matrix[0][3]
			&& zero_bones[hitbox->m_bone].m_matrix[0][3] != low_positive_bones[hitbox->m_bone].m_matrix[0][3]
			&& negative_bones[hitbox->m_bone].m_matrix[0][3] != low_negative_bones[hitbox->m_bone].m_matrix[0][3]
			&& negative_bones[hitbox->m_bone].m_matrix[0][3] != low_positive_bones[hitbox->m_bone].m_matrix[0][3]
			&& negative_bones[hitbox->m_bone].m_matrix[0][3] != positive_bones[hitbox->m_bone].m_matrix[0][3]
			&& positive_bones[hitbox->m_bone].m_matrix[0][3] != low_negative_bones[hitbox->m_bone].m_matrix[0][3]
			&& positive_bones[hitbox->m_bone].m_matrix[0][3] != low_positive_bones[hitbox->m_bone].m_matrix[0][3]
			&& low_negative_bones[hitbox->m_bone].m_matrix[0][3] != low_positive_bones[hitbox->m_bone].m_matrix[0][3]) {
			if (intersect(shoot_pos, point.m_pos, hitbox, zero_bones[hitbox->m_bone])) {
				++point.m_intersections;
				++point.m_low_intersections;
			}

			if (intersect(shoot_pos, point.m_pos, hitbox, negative_bones[hitbox->m_bone]))
				++point.m_intersections;

			if (intersect(shoot_pos, point.m_pos, hitbox, positive_bones[hitbox->m_bone]))
				++point.m_intersections;

			if (intersect(shoot_pos, point.m_pos, hitbox, low_negative_bones[hitbox->m_bone]))
				++point.m_low_intersections;

			if (intersect(shoot_pos, point.m_pos, hitbox, low_positive_bones[hitbox->m_bone]))
				++point.m_low_intersections;
		}

		point.m_valid = point.m_intersections >= point.m_needed_intersections;
	}

	void c_aim_bot::scan_center_points(aim_target_t& target, const int hitgroups) const {
		const auto& anim_side = target.m_lag_record->m_side < 3 ? target.m_lag_record->m_sides.at(target.m_lag_record->m_side) : target.m_lag_record->m_low_sides.at(target.m_lag_record->m_side - 3);

		const auto hitbox_set = target.m_entry->m_player->mdl_data()->m_studio_hdr->hitbox_set(target.m_entry->m_player->hitbox_set_index());
		const auto is_taser = g_context->weapon()->item_index() == valve::e_item_index::taser;
		const auto points_count = calc_points_count(hitgroups, 0);

		target.m_points.reserve(points_count);

		for (std::size_t i{}; i < m_hitgroups.size(); ++i) {
			if (!((1 << i) & hitgroups))
				continue;

			for (const auto& index : m_hitgroups.at(i)) {
				const auto hitbox = hitbox_set->hitbox(index);
				if (!hitbox
					|| (is_taser && index != 3))
					continue;

				vec3_t point{};

				math::vector_transform(
					(hitbox->m_min + hitbox->m_max) / 2.f,
					anim_side.m_bones[hitbox->m_bone], point
				);

				target.m_points.emplace_back(std::move(point), true, index, hitbox->m_group, 0);
			}
		}

		target.m_lag_record->restore(target.m_entry->m_player, target.m_lag_record->m_side);

		for (auto& point : target.m_points)
			scan_point(target, point, true);
	}

	void c_aim_bot::calc_capsule_points(
		aim_target_t& target, const valve::studio_hitbox_t* const hitbox,
		const int index, const mat3x4_t& matrix, float scale
	) const {
		vec3_t min{}, max{};

		math::vector_transform(hitbox->m_min, matrix, min);
		math::vector_transform(hitbox->m_max, matrix, max);

		static auto matrix0 = math::vector_matrix({ 0.f, 0.f, 1.f });

		auto matrix1 = math::vector_matrix((max - min).normalized());

		for (const auto& vertices : {
				vec3_t{ 0.95f, 0.f, 0.f },
				vec3_t{ -0.95f, 0.f, 0.f },
				vec3_t{ 0.f, 0.95f, 0.f },
				vec3_t{ 0.f, -0.95f, 0.f },
				vec3_t{ 0.f, 0.f, 0.95f },
				vec3_t{ 0.f, 0.f, -0.95f }
			}) {
			vec3_t point{};

			math::vector_rotate(vertices, matrix0, point);
			math::vector_rotate(point, matrix1, point);

			point *= scale;

			if (vertices.z > 0.f)
				point += min - max;

			target.m_points.emplace_back(point + max, false, index, hitbox->m_group, 0);
		}
	}

	float c_aim_bot::calc_point_scale(
		const float spread, const float max,
		const float dist, const vec3_t& dir,
		const vec3_t& right, const vec3_t& up
	) const
	{
		const auto v1 = g_eng_pred->inaccuracy() + spread;

		auto v28 = right * v1 + dir + up * v1;

		v28.normalize();

		const auto delta = math::angle_diff(math::to_deg(std::atan2(dir.y, dir.x)), math::to_deg(std::atan2(v28.y, v28.x)));

		const auto v23 = max + dist / std::tan(math::to_rad(180.f - (delta + 90.f)));
		if (v23 > max)
			return 1.f;

		float v25{};
		if (v23 >= 0.f)
			v25 = v23;

		return v25 / max;
	}

	void c_aim_bot::calc_multi_points(
		aim_target_t& target, const valve::studio_hitbox_t* const hitbox,
		const int index, const mat3x4_t& matrix, float scale
	) const {
		vec3_t point{};

		const auto center = (hitbox->m_min + hitbox->m_max) / 2.f;

		math::vector_transform(center, matrix, point);

		target.m_points.emplace_back(point, true, index, hitbox->m_group, 0);

		if (!sdk::g_config_system->static_scale) {
			const auto max = (hitbox->m_max - hitbox->m_min).length() * 0.5f + hitbox->m_radius;

			auto dir = (point - g_context->shoot_pos());

			const auto dist = dir.normalize();

			vec3_t right{}, up{};

			if (dir.x == 0.f
				&& dir.y == 0.f) {
				right = { 0.f, -1.f, 0.f };
				up = { -dir.z, 0.f, 0.f };
			}
			else {
				right = dir.cross({ 0.f, 0.f, 1.f }).normalized();
				up = right.cross(dir).normalized();
			}

			scale = calc_point_scale(g_eng_pred->spread(), max, dist, dir, right, up);
			if (scale <= 0.f
				&& g_eng_pred->spread() > g_eng_pred->min_inaccuracy())
				scale = calc_point_scale(g_eng_pred->min_inaccuracy(), max, dist, dir, right, up);
		}

		if (scale <= 0.f)
			return;

		if (hitbox->m_radius <= 0.f) {
			point = { center.x + (hitbox->m_min.x - center.x) * scale, center.y, center.z };

			math::vector_transform(point, matrix, point);

			target.m_points.emplace_back(point, false, index, hitbox->m_group, 0);

			point = { center.x + (hitbox->m_max.x - center.x) * scale, center.y, center.z };

			math::vector_transform(point, matrix, point);

			target.m_points.emplace_back(point, false, index, hitbox->m_group, 0);

			return;
		}

		if (index) {
			if (index == 3) {
				if (scale > 0.9f
					&& !sdk::g_config_system->static_scale)
					scale = 0.9f;
			}
			else {
				if (index != 2
					&& index != 6) {
					if (index == 4
						|| index == 5) {
						if (scale > 0.9f
							&& !sdk::g_config_system->static_scale)
							scale = 0.9f;

						point = { center.x, hitbox->m_max.y - hitbox->m_radius * scale, center.z };

						math::vector_transform(point, matrix, point);

						target.m_points.emplace_back(point, false, index, hitbox->m_group, 0);
					}

					return;
				}

				if (scale > 0.9f
					&& !sdk::g_config_system->static_scale)
					scale = 0.9f;

				if (index == 6) {
					point = { center.x, hitbox->m_max.y - hitbox->m_radius * scale, center.z };

					math::vector_transform(point, matrix, point);

					target.m_points.emplace_back(point, false, index, hitbox->m_group, 0);

					return;
				}
			}

			return calc_capsule_points(target, hitbox, index, matrix, scale);
		}

		point = { hitbox->m_max.x + 0.70710678f * (hitbox->m_radius * scale), hitbox->m_max.y - 0.70710678f * (hitbox->m_radius * scale), hitbox->m_max.z };

		math::vector_transform(point, matrix, point);

		target.m_points.emplace_back(point, false, index, hitbox->m_group, 0);

		point = { hitbox->m_max.x, hitbox->m_max.y, hitbox->m_max.z + hitbox->m_radius * scale };

		math::vector_transform(point, matrix, point);

		target.m_points.emplace_back(point, false, index, hitbox->m_group, 0);

		point = { hitbox->m_max.x, hitbox->m_max.y, hitbox->m_max.z - hitbox->m_radius * scale };

		math::vector_transform(point, matrix, point);

		target.m_points.emplace_back(point, false, index, hitbox->m_group, 0);

		point = { hitbox->m_max.x, hitbox->m_max.y - hitbox->m_radius * scale, hitbox->m_max.z };

		math::vector_transform(point, matrix, point);

		target.m_points.emplace_back(point, false, index, hitbox->m_group, 0);
	}

	void c_aim_bot::scan_points(
		aim_target_t& target, const int hitgroups, const int multi_points,
		const bool trace, const bool ignore_dmg
	) const {
		const auto& anim_side = target.m_lag_record->m_side < 3 ? target.m_lag_record->m_sides.at(target.m_lag_record->m_side) : target.m_lag_record->m_low_sides.at(target.m_lag_record->m_side - 3);

		const auto hitbox_set = target.m_entry->m_player->mdl_data()->m_studio_hdr->hitbox_set(target.m_entry->m_player->hitbox_set_index());

		const auto points_count = calc_points_count(hitgroups, multi_points);

		target.m_points.reserve(points_count);

		const auto is_taser = g_context->weapon()->item_index() == valve::e_item_index::taser;

		const auto force_safe_point = key_handler::check_key(sdk::g_config_system->safe_point_key, sdk::g_config_system->safe_point_key_style)
			|| (sdk::g_config_system->safe_point_conditions[3] && target.m_lag_record->m_velocity.length() <= 5.f && target.m_lag_record->m_flags & valve::e_ent_flags::on_ground)
			|| (sdk::g_config_system->safe_point_conditions[5] && target.m_entry->m_misses >= sdk::g_config_system->max_misses_safe_point)
			|| (sdk::g_config_system->safe_point_conditions[6] && target.m_lag_record->m_forward)
			|| (sdk::g_config_system->safe_point_conditions[7] && target.m_lag_record->m_sideways);

		for (std::size_t i{}; i < m_hitgroups.size(); ++i) {
			if (!((1 << i) & hitgroups)
				&& (i != 2 || !(sdk::g_config_system->body_aim_conditions[0]
					|| sdk::g_config_system->body_aim_conditions[1]
					|| sdk::g_config_system->body_aim_conditions[2]
					|| sdk::g_config_system->body_aim_conditions[3]
					|| sdk::g_config_system->body_aim_conditions[4]
					|| sdk::g_config_system->body_aim_conditions[5]
					|| (sdk::g_config_system->body_aim_conditions[6] && target.m_entry->m_misses >= sdk::g_config_system->max_misses_body_aim)
					|| (sdk::g_config_system->body_aim_conditions[7] && target.m_lag_record->m_forward)
					|| (sdk::g_config_system->body_aim_conditions[8] && target.m_lag_record->m_sideways)
					)))
				continue;

			if (((1 << i) & multi_points)) {
				const auto scale = (i ? sdk::g_config_system->body_scale : sdk::g_config_system->head_scale) / 100.f;

				for (const auto& index : m_hitgroups.at(i)) {
					const auto hitbox = hitbox_set->hitbox(index);
					if (!hitbox
						|| (is_taser && index != 3))
						continue;

					calc_multi_points(target, hitbox, index, anim_side.m_bones[hitbox->m_bone], scale);
				}
			}
			else {
				for (const auto& index : m_hitgroups.at(i)) {
					const auto hitbox = hitbox_set->hitbox(index);
					if (!hitbox
						|| (is_taser && index != 3))
						continue;

					vec3_t point{};

					math::vector_transform(
						(hitbox->m_min + hitbox->m_max) / 2.f,
						anim_side.m_bones[hitbox->m_bone], point
					);

					target.m_points.emplace_back(std::move(point), true, index, hitbox->m_group, 0);
				}
			}

			for (auto& point : target.m_points) {
				if (is_taser) {
					point.m_needed_intersections = 3;

					continue;
				}

				if (force_safe_point
					|| (sdk::g_config_system->safe_point_conditions[4] && point.m_hitbox > 6)) {
					point.m_needed_intersections = sdk::g_config_system->safe_point_type + 2;

					continue;
				}
			}
		}

		if (!trace)
			return;

		lag_backup_t lag_backup{ target.m_entry->m_player };

		target.m_lag_record->restore(target.m_entry->m_player, target.m_lag_record->m_side);

		for (auto& point : target.m_points)
			scan_point(target, point, ignore_dmg);

		lag_backup.restore(target.m_entry->m_player);
	}

	bool c_aim_bot::select_points(aim_target_t& target) const {
		std::array< aim_point_t*, 11u > best_points{};

		const auto hp = target.m_entry->m_player->health();
		const auto is_taser = g_context->weapon()->item_index() == valve::e_item_index::taser;

		for (auto& point : target.m_points) {
			if (!point.m_valid
				|| point.m_pen_data.m_dmg < 1)
				continue;

			auto& best_point = best_points.at(point.m_hitgroup);
			if (!best_point) {
				best_point = &point;

				continue;
			}

			const auto& pen_data = point.m_pen_data;

			auto min_dmg = pen_data.m_remaining_pen == 4 ? sdk::g_config_system->minimum_damage : sdk::g_config_system->penetration_minimum_damage;

			if (key_handler::check_key(sdk::g_config_system->override_minimum_damage_key, sdk::g_config_system->override_minimum_damage_key_style))
				min_dmg = sdk::g_config_system->override_minimum_damage;

			min_dmg = std::min(min_dmg, hp + 5);

			if (is_taser)
				min_dmg = hp + 5;

			if (point.m_intersections == 3) {
				if (pen_data.m_dmg > hp + 5) {
					best_point = &point;
					continue;
				}

				if (sdk::g_config_system->safe_point_conditions[0]
					&& pen_data.m_dmg > min_dmg) {
					best_point = &point;
					continue;
				}

				if (sdk::g_config_system->safe_point_conditions[1]
					&& pen_data.m_dmg > hp) {
					best_point = &point;
					continue;
				}

				if (sdk::g_config_system->safe_point_conditions[2]
					&& pen_data.m_dmg > hp / 2) {
					best_point = &point;
					continue;
				}
			}

			if ((target.m_lag_record->m_flags & valve::e_ent_flags::on_ground)
				&& !target.m_lag_record->m_shot
				&& !target.m_lag_record->m_throw
				&& target.m_lag_record->m_side) {
				if (target.m_lag_record->m_side < 3
					&& point.m_low_intersections == 3) {
					if (pen_data.m_dmg > hp + 5) {
						best_point = &point;
						continue;
					}

					if (sdk::g_config_system->safe_point_conditions[0]
						&& pen_data.m_dmg > min_dmg) {
						best_point = &point;
						continue;
					}

					if (sdk::g_config_system->safe_point_conditions[1]
						&& pen_data.m_dmg > hp) {
						best_point = &point;
						continue;
					}

					if (sdk::g_config_system->safe_point_conditions[2]
						&& pen_data.m_dmg > hp / 2) {
						best_point = &point;
						continue;
					}
				}

				if (point.m_intersections == 2) {
					if (pen_data.m_dmg > hp + 5) {
						best_point = &point;
						continue;
					}

					if (sdk::g_config_system->safe_point_conditions[0]
						&& pen_data.m_dmg > min_dmg) {
						best_point = &point;
						continue;
					}

					if (sdk::g_config_system->safe_point_conditions[1]
						&& pen_data.m_dmg > hp) {
						best_point = &point;
						continue;
					}

					if (sdk::g_config_system->safe_point_conditions[2]
						&& pen_data.m_dmg > hp / 2) {
						best_point = &point;
						continue;
					}
				}

				if (target.m_lag_record->m_side < 3
					&& point.m_low_intersections == 1) {
					if (pen_data.m_dmg > hp + 5) {
						best_point = &point;
						continue;
					}

					if (sdk::g_config_system->safe_point_conditions[0]
						&& pen_data.m_dmg > min_dmg) {
						best_point = &point;
						continue;
					}

					if (sdk::g_config_system->safe_point_conditions[1]
						&& pen_data.m_dmg > hp) {
						best_point = &point;
						continue;
					}

					if (sdk::g_config_system->safe_point_conditions[2]
						&& pen_data.m_dmg > hp / 2) {
						best_point = &point;
						continue;
					}
				}
			}
			else {
				if (point.m_low_intersections == 3) {
					if (pen_data.m_dmg > hp + 5) {
						best_point = &point;
						continue;
					}

					if (sdk::g_config_system->safe_point_conditions[0]
						&& pen_data.m_dmg > min_dmg) {
						best_point = &point;
						continue;
					}

					if (sdk::g_config_system->safe_point_conditions[1]
						&& pen_data.m_dmg > hp) {
						best_point = &point;
						continue;
					}

					if (sdk::g_config_system->safe_point_conditions[2]
						&& pen_data.m_dmg > hp / 2) {
						best_point = &point;
						continue;
					}
				}

				if (point.m_intersections == 2) {
					if (pen_data.m_dmg > hp + 5) {
						best_point = &point;
						continue;
					}

					if (sdk::g_config_system->safe_point_conditions[0]
						&& pen_data.m_dmg > min_dmg) {
						best_point = &point;
						continue;
					}

					if (sdk::g_config_system->safe_point_conditions[1]
						&& pen_data.m_dmg > hp) {
						best_point = &point;
						continue;
					}

					if (sdk::g_config_system->safe_point_conditions[2]
						&& pen_data.m_dmg > hp / 2) {
						best_point = &point;
						continue;
					}
				}

				if (point.m_low_intersections == 2) {
					if (pen_data.m_dmg > hp + 5) {
						best_point = &point;
						continue;
					}

					if (sdk::g_config_system->safe_point_conditions[0]
						&& pen_data.m_dmg > min_dmg) {
						best_point = &point;
						continue;
					}

					if (sdk::g_config_system->safe_point_conditions[1]
						&& pen_data.m_dmg > hp) {
						best_point = &point;
						continue;
					}

					if (sdk::g_config_system->safe_point_conditions[2]
						&& pen_data.m_dmg > hp / 2) {
						best_point = &point;
						continue;
					}
				}
			}

			const auto& best_pen_data = best_point->m_pen_data;
			if (point.m_intersections == best_point->m_intersections) {
				if (point.m_center) {
					if ((best_pen_data.m_hitgroup == pen_data.m_hitgroup && best_pen_data.m_remaining_pen == pen_data.m_remaining_pen)
						|| (best_pen_data.m_remaining_pen == pen_data.m_remaining_pen && std::abs(best_pen_data.m_dmg - pen_data.m_dmg) <= 1)
						|| (best_pen_data.m_dmg > hp && pen_data.m_dmg > hp)) {
						best_point = &point;

						continue;
					}
				}

				if (pen_data.m_dmg > best_pen_data.m_dmg)
					best_point = &point;

				continue;
			}

			if (best_pen_data.m_hitgroup != pen_data.m_hitgroup
				|| best_pen_data.m_remaining_pen != pen_data.m_remaining_pen) {
				if (best_pen_data.m_remaining_pen != pen_data.m_remaining_pen
					|| std::abs(best_pen_data.m_dmg - pen_data.m_dmg) > 1) {
					if (best_pen_data.m_dmg <= hp || pen_data.m_dmg <= hp) {
						if (pen_data.m_dmg > best_pen_data.m_dmg)
							best_point = &point;

						continue;
					}
				}
			}

			if (point.m_intersections > best_point->m_intersections)
				best_point = &point;
		}

		std::vector< aim_point_t > new_points{};

		for (auto& best_point : best_points)
			if (best_point)
				new_points.emplace_back(std::move(*best_point));

		target.m_points = new_points;

		if (target.m_points.empty())
			return false;

		for (auto& point : target.m_points) {
			if (!target.m_best_point)
				target.m_best_point = &point;
			else {
				const auto& best_pen_data = target.m_best_point->m_pen_data;
				const auto& pen_data = point.m_pen_data;

				auto v31 = point.m_intersections > target.m_best_point->m_intersections;
				if (!v31)
					v31 = pen_data.m_dmg > best_pen_data.m_dmg;

				if (v31)
					target.m_best_point = &point;
			}

			if (point.m_hitgroup == 2
				|| point.m_hitgroup == 3) {
				if (!target.m_best_body_point)
					target.m_best_body_point = &point;
				else {
					const auto& best_pen_data = target.m_best_body_point->m_pen_data;
					const auto& pen_data = point.m_pen_data;

					auto v31 = point.m_intersections > target.m_best_body_point->m_intersections;
					if (!v31)
						v31 = pen_data.m_dmg > best_pen_data.m_dmg;

					if (v31)
						target.m_best_body_point = &point;
				}
			}
		}

		return true;
	}

	void c_aim_bot::player_move(extrapolation_data_t& data) const {
		if (data.m_flags & valve::e_ent_flags::on_ground) {
			if (!g_context->cvars().m_sv_enablebunnyhopping->get_bool()) {
				const auto speed = data.m_velocity.length();

				const auto max_speed = data.m_player->max_speed() * 1.1f;
				if (max_speed > 0.f
					&& speed > max_speed)
					data.m_velocity *= (max_speed / speed);
			}

			if (data.m_was_in_air)
				data.m_velocity.z = g_context->cvars().m_sv_jump_impulse->get_float();
		}
		else
			data.m_velocity.z -=
			g_context->cvars().m_sv_gravity->get_float() * valve::g_global_vars->m_interval_per_tick;

		valve::trace_t trace{};
		valve::trace_filter_world_only_t trace_filter{};

		valve::g_engine_trace->trace_ray(
			{
				data.m_origin,
				data.m_origin + data.m_velocity * valve::g_global_vars->m_interval_per_tick,
				data.m_obb_min, data.m_obb_max
			},
			valve::e_mask::contents_solid, &trace_filter, &trace
		);

		if (trace.m_fraction != 1.f) {
			for (int i{}; i < 2; ++i) {
				data.m_velocity -= trace.m_plane.m_normal * data.m_velocity.dot(trace.m_plane.m_normal);

				const auto adjust = data.m_velocity.dot(trace.m_plane.m_normal);
				if (adjust < 0.f)
					data.m_velocity -= trace.m_plane.m_normal * adjust;

				valve::g_engine_trace->trace_ray(
					{
						trace.m_end_pos,
						trace.m_end_pos + (data.m_velocity * (valve::g_global_vars->m_interval_per_tick * (1.f - trace.m_fraction))),
						data.m_obb_min, data.m_obb_max
					},
					valve::e_mask::contents_solid, &trace_filter, &trace
				);

				if (trace.m_fraction == 1.f)
					break;
			}
		}

		data.m_origin = trace.m_end_pos;

		valve::g_engine_trace->trace_ray(
			{
				trace.m_end_pos,
				{ trace.m_end_pos.x, trace.m_end_pos.y, trace.m_end_pos.z - 2.f },
				data.m_obb_min, data.m_obb_max
			},
			valve::e_mask::contents_solid, &trace_filter, &trace
		);

		data.m_flags &= ~valve::e_ent_flags::on_ground;

		if (trace.m_fraction != 1.f
			&& trace.m_plane.m_normal.z > 0.7f)
			data.m_flags |= valve::e_ent_flags::on_ground;
	}

	std::optional< aim_record_t > c_aim_bot::extrapolate(const player_entry_t& entry) const {
		const auto& latest = entry.m_lag_records.back();
		const auto time_delta = g_lag_comp->calc_time_delta(latest->m_sim_time);
		if (time_delta < 0.2f) {
			aim_record_t ret{ const_cast<player_entry_t*>(&entry), latest };

			return ret;
		}

		int delay{};
		if (g_movement->should_fake_duck())
			delay = 15 - valve::g_client_state->m_choked_cmds;

		const auto& net_info = g_context->net_info();

		const auto latency_ticks = valve::to_ticks(net_info.m_latency.m_in + net_info.m_latency.m_out);
		const auto delta = (delay + valve::g_client_state->m_server_tick + latency_ticks - latest->m_receive_tick) / latest->m_sim_ticks;
		if (delta <= 0
			|| delta > 17) {
			aim_record_t ret{ const_cast<player_entry_t*>(&entry), latest };

			return ret;
		}

		const auto max = valve::to_ticks(time_delta - 0.2f) / latest->m_sim_ticks;

		auto extrapolate_ticks = std::min(delta, max);
		if (extrapolate_ticks <= 0) {
			aim_record_t ret{ const_cast<player_entry_t*>(&entry), latest };

			return ret;
		}

		latest->m_extrapolate_ticks = extrapolate_ticks;

		extrapolation_data_t data{ entry.m_player, latest.get() };

		do {
			for (int i{}; i < latest->m_sim_ticks; ++i) {
				data.m_sim_time += valve::g_global_vars->m_interval_per_tick;
				player_move(data);
			}
		} while (--extrapolate_ticks);

		aim_record_t ret{ const_cast<player_entry_t*>(&entry), std::make_shared< lag_record_t >() };

		latest->m_extrapolated = true;

		*ret.m_lag_record = *latest;

		ret.m_lag_record->m_sim_time = data.m_sim_time;
		ret.m_lag_record->m_flags = data.m_flags;

		ret.m_lag_record->m_origin = data.m_origin;
		ret.m_lag_record->m_velocity = data.m_velocity;

		const auto origin_delta = data.m_origin - latest->m_origin;

		for (auto& anim_side : ret.m_lag_record->m_sides)
			for (std::size_t i{}; i < anim_side.m_bones_count; ++i) {
				auto& bone = anim_side.m_bones.at(i);

				bone[0][3] += origin_delta.x;
				bone[1][3] += origin_delta.y;
				bone[2][3] += origin_delta.z;
			}

		return ret;
	}

	std::optional< aim_record_t > c_aim_bot::select_latest_record(const player_entry_t& entry) const {
		if (entry.m_lag_records.empty())
			return std::nullopt;

		for (auto i = entry.m_lag_records.rbegin(); i != entry.m_lag_records.rend(); i = std::next(i)) {
			const auto& lag_record = *i;
			if (lag_record->m_dormant
				|| !lag_record->valid())
				continue;

			aim_record_t ret{ const_cast<player_entry_t*>(&entry), lag_record };

			return ret;
		}

		return std::nullopt;
	}

	std::optional< aim_record_t > c_aim_bot::select_oldest_record(const player_entry_t& entry) const {
		if (entry.m_lag_records.empty())
			return std::nullopt;

		for (auto i = entry.m_lag_records.begin(); i != entry.m_lag_records.end(); i = std::next(i)) {
			const auto& lag_record = *i;
			if (lag_record->m_dormant
				|| !lag_record->valid())
				continue;

			aim_record_t ret{ const_cast<player_entry_t*>(&entry), lag_record };

			return ret;
		}

		return std::nullopt;
	}

	std::optional< aim_record_t > c_aim_bot::select_record(const player_entry_t& entry) const {
		if (entry.m_lag_records.empty())
			return std::nullopt;

		const auto mdl_data = entry.m_player->mdl_data();
		if (!mdl_data
			|| !mdl_data->m_studio_hdr)
			return std::nullopt;

		const auto hitbox_set = mdl_data->m_studio_hdr->hitbox_set(entry.m_player->hitbox_set_index());
		if (!hitbox_set
			|| hitbox_set->m_hitboxes_count <= 0)
			return std::nullopt;

		const auto& latest = entry.m_lag_records.back();
		if (latest->m_dormant)
			return std::nullopt;

		if (!g_context->cvars().m_cl_lagcompensation->get_bool())
			return extrapolate(entry);		

		if (entry.m_lag_records.size() == 1u
			|| !sdk::g_config_system->lag_compensation)
			return select_latest_record(entry);

		if (latest->m_broke_lc) {
			int delay{};

			if (g_movement->should_fake_duck())
				delay = 15 - valve::g_client_state->m_choked_cmds;			

			const auto v17 = std::clamp(
				valve::to_ticks(
					(
						(valve::to_time(delay) + g_context->net_info().m_latency.m_out)
						+ valve::g_global_vars->m_real_time
						) - entry.m_receive_time
				),
				0, 100
			);

			if ((v17 - valve::to_ticks(latest->m_sim_time - latest->m_old_sim_time)) >= 0)
				return std::nullopt;

			return extrapolate(entry);
		}

		int cfg_hitgroups{};
		for (std::size_t i{}; i < 6u; ++i) {
			if (sdk::g_config_system->target_hitbox[i]) {
				if (sdk::g_config_system->lag_compensation == 1)
					cfg_hitgroups |= (1 << i);
				else if (sdk::g_config_system->lag_compensation == 2)
					cfg_hitgroups = 32 | (1 << i);
			}
			else {
				if (sdk::g_config_system->lag_compensation == 1)
					cfg_hitgroups &= ~(1 << i);
				else if (sdk::g_config_system->lag_compensation == 2)
					cfg_hitgroups &= 32 | ~(1 << i);
			}
		}
		
		lag_backup_t lag_backup{ entry.m_player };		
		std::optional< aim_point_t > best_point{};
		std::shared_ptr< lag_record_t > best_record{};
		std::size_t scanned_count{};

		const auto hp = entry.m_player->health();
		for (auto i = entry.m_lag_records.rbegin(); i != entry.m_lag_records.rend(); i = std::next(i)) {
			const auto& lag_record = *i;

			if (!lag_record->valid())
				continue;			

			if (!lag_record->m_dormant) {
				aim_target_t target{};
				target.m_entry = const_cast<player_entry_t*>(&entry);
				target.m_lag_record = lag_record;

				++scanned_count;

				auto hitgroups = 1;
				if ((scanned_count & 3) == 0)
					hitgroups = cfg_hitgroups;

				scan_center_points(target, hitgroups);

				if (!select_points(target))
					continue;

				if (!best_record
					|| !best_point.has_value()) {
					best_record = lag_record;
					best_point = *target.m_best_point;
					continue;
				}

				if (!target.m_best_point->m_valid
					|| !target.m_best_point->m_pen_data.m_dmg) {
					if ((best_record->m_origin - lag_record->m_origin).length_sqr() > 1.f)
						continue;
				}

				if (lag_record->m_shot != best_record->m_shot) {
					if (lag_record->m_shot) {
						best_record = lag_record;
						best_point = *target.m_best_point;
					}

					continue;
				}

				if (lag_record->m_throw != best_record->m_throw) {
					if (lag_record->m_throw) {
						best_record = lag_record;
						best_point = *target.m_best_point;
					}

					continue;
				}

				if ((lag_record->m_flags & valve::e_ent_flags::on_ground) != (best_record->m_flags & valve::e_ent_flags::on_ground)) {
					if (!(lag_record->m_flags & valve::e_ent_flags::on_ground)) {
						best_record = lag_record;
						best_point = *target.m_best_point;
					}

					continue;
				}

				if (target.m_best_point->m_intersections == 3
					&& target.m_best_point->m_intersections != best_point.value().m_intersections) {
					best_record = lag_record;
					best_point = *target.m_best_point;

					continue;
				}

				if ((lag_record->m_flags & valve::e_ent_flags::on_ground)
					&& !lag_record->m_shot
					&& !lag_record->m_throw
					&& lag_record->m_side
					&& lag_record->m_side < 3) {
					if (target.m_best_point->m_intersections != best_point.value().m_intersections) {
						if (target.m_best_point->m_intersections > best_point.value().m_intersections) {
							best_record = lag_record;
							best_point = *target.m_best_point;
						}

						continue;
					}
				}
				else {
					if (target.m_best_point->m_low_intersections != best_point.value().m_low_intersections) {
						if (target.m_best_point->m_low_intersections > best_point.value().m_low_intersections) {
							best_record = lag_record;
							best_point = *target.m_best_point;
						}

						continue;
					}
				}
			
				const auto& pen_data = target.m_best_point->m_pen_data;
				const auto& best_pen_data = best_point.value().m_pen_data;

				if (std::abs(pen_data.m_dmg - best_pen_data.m_dmg) > 10) {
					if (pen_data.m_dmg <= hp
						|| best_pen_data.m_dmg <= hp) {
						if (pen_data.m_dmg > best_pen_data.m_dmg) {
							best_record = lag_record;
							best_point = *target.m_best_point;

							continue;
						}
					}
				}

				if (pen_data.m_dmg > best_pen_data.m_dmg) {
					best_record = lag_record;
					best_point = *target.m_best_point;
				}
			}
		}

		lag_backup.restore(entry.m_player);

		if (!best_record)
			return std::nullopt;

		aim_record_t ret{ const_cast<player_entry_t*>(&entry), best_record };

		return ret;
	}

	aim_point_t* c_aim_bot::select_point(const aim_target_t& target, const int cmd_number) const {
		const auto force_body_aim = key_handler::check_key(sdk::g_config_system->body_aim_key, sdk::g_config_system->body_aim_key_style);

		if (!target.m_best_body_point
			|| target.m_best_body_point->m_pen_data.m_dmg < 1)
			return force_body_aim ? nullptr : target.m_best_point;

		if (force_body_aim)
			return target.m_best_body_point;

		const auto hp = target.m_entry->m_player->health();
		const auto is_taser = g_context->weapon()->item_index() == valve::e_item_index::taser;
		auto min_dmg = target.m_best_body_point->m_pen_data.m_remaining_pen == 4 ? sdk::g_config_system->minimum_damage : sdk::g_config_system->penetration_minimum_damage;

		if (key_handler::check_key(sdk::g_config_system->override_minimum_damage_key, sdk::g_config_system->override_minimum_damage_key_style))
			min_dmg = sdk::g_config_system->override_minimum_damage;

		min_dmg = std::min(min_dmg, hp + 5);

		if (is_taser)
			min_dmg = hp + 5;

		if (sdk::g_config_system->body_aim_conditions[0]
			&& target.m_best_body_point->m_pen_data.m_dmg >= min_dmg)
			return target.m_best_body_point;

		if (sdk::g_config_system->body_aim_conditions[1]
			&& target.m_best_point->m_intersections < sdk::g_config_system->safe_point_type + 2)
			return target.m_best_body_point;

		if (sdk::g_config_system->body_aim_conditions[2]) {
			const auto& shots = g_shots->elements();
			if (!shots.empty()) {
				const auto& last_shot = shots.back();
				if (!last_shot.m_process_tick
					&& last_shot.m_target.m_entry == target.m_entry
					&& std::abs(last_shot.m_cmd_number - cmd_number) <= 150) {
					const auto after_shot_hp = last_shot.m_target.m_entry->m_player->health() - last_shot.m_dmg;
					if (after_shot_hp > 0
						&& target.m_best_body_point->m_pen_data.m_dmg > after_shot_hp)
						return target.m_best_body_point;
				}
			}

			if (target.m_best_body_point->m_pen_data.m_dmg >= hp)
				return target.m_best_body_point;
		}

		if (sdk::g_config_system->body_aim_conditions[3]
			&& target.m_best_body_point->m_pen_data.m_dmg >= hp / 2)
			return target.m_best_body_point;

		if (sdk::g_config_system->body_aim_conditions[4]) {
			if (g_exploits->type() == 2
				|| g_exploits->type() == 3
				|| g_exploits->type() == 4) {
				if (g_exploits->next_shift_amount()
					&& target.m_best_body_point->m_pen_data.m_dmg >= hp / 2)
					return target.m_best_body_point;

				const auto& shots = g_shots->elements();
				if (!shots.empty()) {
					const auto& last_shot = shots.back();
					if (!last_shot.m_process_tick
						&& last_shot.m_next_shift_amount
						&& last_shot.m_target.m_entry == target.m_entry
						&& std::abs(last_shot.m_cmd_number - cmd_number) <= 150
						&& (target.m_best_body_point->m_pen_data.m_dmg + last_shot.m_dmg) >= hp)
						return target.m_best_body_point;
				}
			}
		}

		if (sdk::g_config_system->body_aim_conditions[5]
			&& target.m_best_point->m_intersections < target.m_best_body_point->m_intersections)
			return target.m_best_body_point;

		if (sdk::g_config_system->body_aim_conditions[6]
			&& target.m_entry->m_misses >= sdk::g_config_system->max_misses_body_aim)
			return target.m_best_body_point;

		return target.m_best_point;
	}

	void c_aim_bot::find_targets() {
		for (auto i = 1; i <= valve::g_global_vars->m_max_clients; ++i) {
			const auto& entry = g_lag_comp->entry(i - 1);
			if (!entry.m_player
				|| entry.m_player->dormant()
				|| !entry.m_player->alive()
				|| entry.m_player->immune()
				|| entry.m_player->friendly())
				continue;

			const auto record = select_record(entry);
			if (!record.has_value())
				continue;

			m_targets.emplace_back(record.value());
		}
	}

	void scan_targets() {
		int m_hitgroups{};
		int m_multi_points{};

		for (std::size_t i{}; i < 6u; ++i) {
			if (sdk::g_config_system->target_hitbox[i])
				m_hitgroups |= (1 << i);
			else
				m_hitgroups &= ~(1 << i);

			if (sdk::g_config_system->multi_point[i])
				m_multi_points |= (1 << i);
			else
				m_multi_points &= ~(1 << i);
		}
	
		for (auto& target : g_aim_bot->m_targets)
			g_aim_bot->scan_points(target, m_hitgroups, m_multi_points, true, false);

		g_aim_bot->m_targets.erase(
			std::remove_if(
				g_aim_bot->m_targets.begin(), g_aim_bot->m_targets.end(),
				[&](aim_target_t& target) {
			return !g_aim_bot->select_points(target);
		}
			),
			g_aim_bot->m_targets.end()
		);
	}

	aim_target_t* c_aim_bot::select_target(const int cmd_number) {
		if (m_targets.empty())
			return nullptr;

		const auto& shots = g_shots->elements();
		if (!shots.empty()) {
			const auto& last_shot = shots.back();
			if (last_shot.m_target.m_entry
				&& !last_shot.m_process_tick
				&& last_shot.m_target.m_entry->m_player
				&& std::abs(last_shot.m_cmd_number - cmd_number) <= 150
				&& last_shot.m_dmg < last_shot.m_target.m_entry->m_player->health()) {
				const auto target = std::find_if(
					m_targets.begin(), m_targets.end(),
					[&](const aim_target_t& target) {
					return last_shot.m_target.m_entry->m_player == target.m_entry->m_player;
				}
				);

				if (target != m_targets.end())
					return &*target;
			}
		}

		auto best_target = &m_targets.front();

		const auto end = m_targets.end();
		for (auto it = std::next(m_targets.begin()); it != end; it = std::next(it)) {
			const auto hp = it->m_entry->m_player->health();

			const auto& best_lag = best_target->m_lag_record;
			const auto& lag = it->m_lag_record;

			const auto& best_pen_data = best_target->m_best_point->m_pen_data;
			const auto& pen_data = it->m_best_point->m_pen_data;

			if (lag->m_broke_lc == best_lag->m_broke_lc) {
				if (it->m_best_point->m_intersections == best_target->m_best_point->m_intersections) {
					if (lag->m_priority == best_lag->m_priority) {
						if (pen_data.m_dmg > hp)
							best_target = &*it;
						else if (best_pen_data.m_dmg <= hp
							&& pen_data.m_dmg > best_pen_data.m_dmg)
							best_target = &*it;
						else if (pen_data.m_dmg > best_pen_data.m_dmg)
							best_target = &*it;
					}
					else if (lag->m_priority < best_lag->m_priority)
						best_target = &*it;
				}
				else if (it->m_best_point->m_intersections > best_target->m_best_point->m_intersections)
					best_target = &*it;
			}
			else if (!lag->m_broke_lc)
				best_target = &*it;
		}

		return best_target;
	}
	
	vec2_t c_aim_bot::calc_spread_angle(
				const int bullets, const valve::e_item_index item_index,
				const float recoil_index, const std::size_t i
			) const {
				g_context->addresses().m_random_seed(i + 1u);
		
				auto v1 = g_context->addresses().m_random_float(0.f, 1.f);
				auto v2 = g_context->addresses().m_random_float(0.f, math::k_pi2);
		
				float v3{}, v4{};
		
				using fn_t = void(__stdcall*)(valve::e_item_index, int, int, float*, float*);
		
				if (g_context->cvars().m_weapon_accuracy_shotgun_spread_patterns->get_int() > 0)
					reinterpret_cast<fn_t>(g_context->addresses().m_calc_shotgun_spread)(item_index, 0, static_cast<int>(bullets * recoil_index), &v4, &v3);
				else {
					v3 = g_context->addresses().m_random_float(0.f, 1.f);
					v4 = g_context->addresses().m_random_float(0.f, math::k_pi2);
				}
		
				if (recoil_index < 3.f
					&& item_index == valve::e_item_index::negev) {
					for (auto i = 3; i > recoil_index; --i) {
						v1 *= v1;
						v3 *= v3;
					}
		
					v1 = 1.f - v1;
					v3 = 1.f - v3;
				}
		
				const auto inaccuracy = v1 * g_eng_pred->inaccuracy();
				const auto spread = v3 * g_eng_pred->spread();
		
				return {
					std::cos(v2) * inaccuracy + std::cos(v4) * spread,
					std::sin(v2) * inaccuracy + std::sin(v4) * spread
				};
			}
		
	int c_aim_bot::calc_hit_chance(
		const aim_target_t* const target, const qangle_t& angle, const aim_point_t* const aim_point
	) const {
		if (sdk::g_config_system->remove_recoil
			&& g_context->cvars().m_weapon_accuracy_nospread->get_bool())
			return 99;

		const auto hp = target->m_entry->m_player->health();
		const auto is_taser = g_context->weapon()->item_index() == valve::e_item_index::taser;
		auto min_dmg = aim_point->m_pen_data.m_remaining_pen == 4 ? sdk::g_config_system->minimum_damage : sdk::g_config_system->penetration_minimum_damage;

		if (key_handler::check_key(sdk::g_config_system->override_minimum_damage_key, sdk::g_config_system->override_minimum_damage_key_style))
			min_dmg = sdk::g_config_system->override_minimum_damage;

		min_dmg = std::min(min_dmg, hp + 5);

		if (is_taser)
			min_dmg = hp + 5;

		min_dmg = std::clamp(static_cast<int>(min_dmg * (sdk::g_config_system->accuracy_boost / 100.f)), 1, 100);

		vec3_t fwd{}, right{}, up{};
		math::angle_vectors(angle, &fwd, &right, &up);

		const auto item_index = g_context->weapon()->item_index();
		const auto recoil_index = g_context->weapon()->recoil_index();

		int hits{};

		const auto trace_spread = [](
			const vec3_t& fwd, const vec3_t& right, const vec3_t& up,
			const aim_target_t* const target, const int accuracy_boost,
			const valve::e_item_index item_index, const float recoil_index,
			const int min_dmg, const std::size_t i, bool& hit) {
			const auto spread_angle = g_aim_bot->calc_spread_angle(g_context->wpn_data()->m_bullets, item_index, recoil_index, i);

			auto dir = fwd + (right * spread_angle.x) + (up * spread_angle.y);

			dir.normalize();

			const auto pen_data = g_auto_wall->fire_bullet(
				valve::g_local_player, target->m_entry->m_player,
				g_context->wpn_data(), item_index == valve::e_item_index::taser,
				g_context->shoot_pos(), g_context->shoot_pos() + dir * g_context->wpn_data()->m_range
			);

			hit = pen_data.m_dmg > 0 && (accuracy_boost <= 0 || pen_data.m_dmg >= min_dmg);
		};

		constexpr auto k_max_seeds = 128u;

		std::array< bool, k_max_seeds > seeds{};

		for (std::size_t i{}; i < k_max_seeds; ++i)
			trace_spread(fwd, right, up, target, sdk::g_config_system->accuracy_boost, item_index,
				recoil_index, min_dmg, i, std::ref(seeds.at(i)));

		for (auto& hit : seeds)
			if (hit)
				++hits;

		return static_cast<int>((hits / 128.f) * 100.f);
	}
			
	void c_aim_bot::on_create_move(valve::user_cmd_t& user_cmd) {
		m_last_target.m_lag_record.reset();

		if (!g_context->weapon() || !g_context->wpn_data())
			return;

		m_is_peeking = peek_correction();

		const auto item_index = g_context->weapon()->item_index();
		if (!(sdk::g_config_system->enabled
			&& key_handler::check_key(sdk::g_config_system->enabled_key, sdk::g_config_system->enabled_key_style)
			) || g_context->weapon()->in_reload()
			|| (static_cast<std::size_t>(g_context->wpn_data()->m_unk_type - 2) > 5
				&& item_index != valve::e_item_index::taser)
			|| valve::g_local_player->move_type() == valve::e_move_type::ladder
			|| valve::g_local_player->move_type() == valve::e_move_type::noclip)
			return;

		find_targets();
#if 0
		auto thread_task = thread_pool->add_task(scan_targets);
		thread_pool->wait(thread_task);
#else
		scan_targets();
#endif
		const auto target = select_target(user_cmd.m_number);
		if (!target)
			return m_targets.clear();

		g_context->flags() |= e_context_flags::has_target;

		const auto point = select_point(*target, user_cmd.m_number);
		if (!point
			|| !point->m_valid)
			return m_targets.clear();

		g_context->flags() |= e_context_flags::has_target;

		lag_backup_t lag_backup{ target->m_entry->m_player };

		target->m_lag_record->restore(target->m_entry->m_player, target->m_lag_record->m_side);

		qangle_t angle{};

		math::vector_angles(point->m_pos - g_context->shoot_pos(), angle);

		if (const auto anim_state = valve::g_local_player->anim_state()) {
			if (anim_state->m_player
				&& (anim_state->m_landing || anim_state->m_duck_amount || valve::g_local_player->ground_entity() == valve::e_ent_handle::invalid)) {
				const auto backup_pose_param = valve::g_local_player->pose_params().at(12u);

				valve::g_local_player->pose_params().at(12u) = (std::clamp(std::remainder(
					angle.x
					- valve::g_local_player->aim_punch().x * g_context->cvars().m_weapon_recoil_scale->get_float()
					+ valve::g_local_player->third_person_recoil(), 360.f
				), -90.f, 90.f) + 90.f) / 180.f;

				g_eng_pred->update_shoot_pos();

				valve::g_local_player->pose_params().at(12u) = backup_pose_param;

				math::vector_angles(point->m_pos - g_context->shoot_pos(), angle);
			}
		}

		if (valve::g_global_vars->m_cur_time >= valve::g_local_player->next_attack()
			&& sdk::g_config_system->quick_stop)
			g_movement->stop_type() = ((g_context->flags() & e_context_flags::can_shoot) || sdk::g_config_system->between_shots) ? sdk::g_config_system->type + 1 : 0;

		if (g_context->flags() & e_context_flags::can_shoot) {
			if (sdk::g_config_system->automatic_scope
				&& (item_index == valve::e_item_index::g3sg1
					|| item_index == valve::e_item_index::scar20
					|| item_index == valve::e_item_index::awp
					|| item_index == valve::e_item_index::aug
					|| item_index == valve::e_item_index::sg553
					|| item_index == valve::e_item_index::ssg08
					)
				&& !valve::g_local_player->scoped()
				&& valve::g_local_player->flags() & valve::e_ent_flags::on_ground
				&& !(g_eng_pred->old_user_cmd().m_buttons & valve::e_buttons::in_jump))
				user_cmd.m_buttons |= valve::e_buttons::in_zoom;
			else if (const auto hit_chance = std::min(calc_hit_chance(target, angle, point), 99); hit_chance >= (item_index == valve::e_item_index::taser ? 80 : sdk::g_config_system->minimum_hitchance)) {
				user_cmd.m_view_angles = angle;

				if (sdk::g_config_system->remove_recoil)
					user_cmd.m_view_angles -= valve::g_local_player->aim_punch() * g_context->cvars().m_weapon_recoil_scale->get_float();

				user_cmd.m_view_angles.x = std::remainder(user_cmd.m_view_angles.x, 360.f);
				user_cmd.m_view_angles.y = std::remainder(user_cmd.m_view_angles.y, 360.f);
				user_cmd.m_view_angles.z = std::remainder(user_cmd.m_view_angles.z, 360.f);

				user_cmd.m_view_angles.x = std::clamp(user_cmd.m_view_angles.x, -89.f, 89.f);
				user_cmd.m_view_angles.y = std::clamp(user_cmd.m_view_angles.y, -180.f, 180.f);
				user_cmd.m_view_angles.z = std::clamp(user_cmd.m_view_angles.z, -90.f, 90.f);

				if (!sdk::g_config_system->silent_aim)
					valve::g_engine->set_view_angles(user_cmd.m_view_angles);

				if (sdk::g_config_system->automatic_fire)
					user_cmd.m_buttons |= valve::e_buttons::in_attack;

				if (user_cmd.m_buttons & valve::e_buttons::in_attack) {
					user_cmd.m_tick = valve::to_ticks(target->m_lag_record->m_sim_time + g_context->net_info().m_lerp);

					m_last_target.m_entry = target->m_entry;
					m_last_target.m_lag_record = target->m_lag_record;
					m_last_target.m_hitbox = point->m_hitbox;
					m_last_target.m_dmg = point->m_pen_data.m_dmg;
					m_last_target.m_hit_chance = hit_chance;
					m_last_target.m_point = *point;

					g_context->flags() |= e_context_flags::aim_fire;
				}
#if 0
				g_visuals->add_shot_mdl(target->m_entry->m_player, target->m_lag_record);
#endif
			}
		}

		lag_backup.restore(target->m_entry->m_player);

		m_targets.clear();
	}

	bool c_aim_bot::peek_correction() const {
		bool player_found = false;
		vec3_t backup_shoot_pos = g_context->shoot_pos();
		g_context->shoot_pos() += valve::g_local_player->velocity() * valve::g_global_vars->m_interval_per_tick * 3.f;

		for (auto i = 0; i < valve::g_global_vars->m_max_clients; ++i) {
			const auto& entry = g_lag_comp->entry(i);

			if (!entry.m_player || entry.m_player->dormant() || !entry.m_player->alive() || entry.m_player->immune() || entry.m_player->friendly())
				continue;

			if (entry.m_lag_records.empty())
				continue;

			auto& lag_record = entry.m_lag_records.back();

			if (!lag_record)
				continue;

			aim_target_t target{};

			target.m_entry = const_cast<player_entry_t*>(&entry);
			target.m_lag_record = lag_record;

			scan_points(target, (1 << 1 | 1 << 3 | 1 << 4), (1 << 1 | 1 << 3 | 1 << 4), true, true);

			if (g_aim_bot->select_points(target)) {
				player_found = true;
				break;
			}
		}

		g_context->shoot_pos() = backup_shoot_pos;
		return player_found;
	}
}