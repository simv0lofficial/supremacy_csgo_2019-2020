#include "../../../supremacy.hpp"
#include "../../imgui/settings.hpp"
#include <shellapi.h>

namespace sdk {
	void c_config_system::setup_values()
	{
		//skins::get().setup_values();

		// aimbot.
		g_config_system->push_item(&g_config_system->enabled, "aimbot", "enabled", g_config_system->enabled);
		g_config_system->push_item(&g_config_system->enabled_key, "aimbot", "enabled_key", g_config_system->enabled_key);
		g_config_system->push_item(&g_config_system->enabled_key_style, "aimbot", "enabled_key_style", g_config_system->enabled_key_style);
		g_config_system->push_item(&g_config_system->pistols_target_hitbox[0], "aimbot", "pistols_target_hitbox_0", g_config_system->pistols_target_hitbox[0]);
		g_config_system->push_item(&g_config_system->pistols_target_hitbox[1], "aimbot", "pistols_target_hitbox_1", g_config_system->pistols_target_hitbox[1]);
		g_config_system->push_item(&g_config_system->pistols_target_hitbox[2], "aimbot", "pistols_target_hitbox_2", g_config_system->pistols_target_hitbox[2]);
		g_config_system->push_item(&g_config_system->pistols_target_hitbox[3], "aimbot", "pistols_target_hitbox_3", g_config_system->pistols_target_hitbox[3]);
		g_config_system->push_item(&g_config_system->pistols_target_hitbox[4], "aimbot", "pistols_target_hitbox_4", g_config_system->pistols_target_hitbox[4]);
		g_config_system->push_item(&g_config_system->pistols_target_hitbox[5], "aimbot", "pistols_target_hitbox_5", g_config_system->pistols_target_hitbox[5]);
		g_config_system->push_item(&g_config_system->pistols_multi_point[0], "aimbot", "pistols_multi_point_0", g_config_system->pistols_multi_point[0]);
		g_config_system->push_item(&g_config_system->pistols_multi_point[1], "aimbot", "pistols_multi_point_1", g_config_system->pistols_multi_point[1]);
		g_config_system->push_item(&g_config_system->pistols_multi_point[2], "aimbot", "pistols_multi_point_2", g_config_system->pistols_multi_point[2]);
		g_config_system->push_item(&g_config_system->pistols_multi_point[3], "aimbot", "pistols_multi_point_3", g_config_system->pistols_multi_point[3]);
		g_config_system->push_item(&g_config_system->pistols_multi_point[4], "aimbot", "pistols_multi_point_4", g_config_system->pistols_multi_point[4]);
		g_config_system->push_item(&g_config_system->pistols_multi_point[5], "aimbot", "pistols_multi_point_5", g_config_system->pistols_multi_point[5]);
		g_config_system->push_item(&g_config_system->pistols_static_scale, "aimbot", "pistols_static_scale", g_config_system->pistols_static_scale);
		g_config_system->push_item(&g_config_system->pistols_head_scale, "aimbot", "pistols_head_scale", g_config_system->pistols_head_scale);
		g_config_system->push_item(&g_config_system->pistols_body_scale, "aimbot", "pistols_body_scale", g_config_system->pistols_body_scale);
		g_config_system->push_item(&g_config_system->pistols_automatic_fire, "aimbot", "pistols_automatic_fire", g_config_system->pistols_automatic_fire);
		g_config_system->push_item(&g_config_system->pistols_silent_aim, "aimbot", "pistols_silent_aim", g_config_system->pistols_silent_aim);
		g_config_system->push_item(&g_config_system->pistols_minimum_hitchance, "aimbot", "pistols_minimum_hitchance", g_config_system->pistols_minimum_hitchance);
		g_config_system->push_item(&g_config_system->pistols_minimum_damage, "aimbot", "pistols_minimum_damage", g_config_system->pistols_minimum_damage);
		g_config_system->push_item(&g_config_system->pistols_automatic_penetration, "aimbot", "pistols_automatic_penetration", g_config_system->pistols_automatic_penetration);
		g_config_system->push_item(&g_config_system->pistols_penetration_minimum_damage, "aimbot", "pistols_penetration_minimum_damage", g_config_system->pistols_penetration_minimum_damage);
		g_config_system->push_item(&g_config_system->override_minimum_damage_key, "aimbot", "override_minimum_damage_key", g_config_system->override_minimum_damage_key);
		g_config_system->push_item(&g_config_system->override_minimum_damage_key_style, "aimbot", "override_minimum_damage_key_style", g_config_system->override_minimum_damage_key_style);
		g_config_system->push_item(&g_config_system->pistols_override_minimum_damage, "aimbot", "pistols_override_minimum_damage", g_config_system->pistols_override_minimum_damage);
		g_config_system->push_item(&g_config_system->pistols_automatic_scope, "aimbot", "pistols_automatic_scope", g_config_system->pistols_automatic_scope);
		g_config_system->push_item(&g_config_system->pistols_maximum_fov, "aimbot", "pistols_maximum_fov", g_config_system->pistols_maximum_fov);
		g_config_system->push_item(&g_config_system->log_misses_due_to_spread, "aimbot", "log_misses_due_to_spread", g_config_system->log_misses_due_to_spread);


		g_config_system->push_item(&g_config_system->heavy_pistols_target_hitbox[0], "aimbot", "heavy_pistols_target_hitbox_0", g_config_system->heavy_pistols_target_hitbox[0]);
		g_config_system->push_item(&g_config_system->heavy_pistols_target_hitbox[1], "aimbot", "heavy_pistols_target_hitbox_1", g_config_system->heavy_pistols_target_hitbox[1]);
		g_config_system->push_item(&g_config_system->heavy_pistols_target_hitbox[2], "aimbot", "heavy_pistols_target_hitbox_2", g_config_system->heavy_pistols_target_hitbox[2]);
		g_config_system->push_item(&g_config_system->heavy_pistols_target_hitbox[3], "aimbot", "heavy_pistols_target_hitbox_3", g_config_system->heavy_pistols_target_hitbox[3]);
		g_config_system->push_item(&g_config_system->heavy_pistols_target_hitbox[4], "aimbot", "heavy_pistols_target_hitbox_4", g_config_system->heavy_pistols_target_hitbox[4]);
		g_config_system->push_item(&g_config_system->heavy_pistols_target_hitbox[5], "aimbot", "heavy_pistols_target_hitbox_5", g_config_system->heavy_pistols_target_hitbox[5]);
		g_config_system->push_item(&g_config_system->heavy_pistols_multi_point[0], "aimbot", "heavy_pistols_multi_point_0", g_config_system->heavy_pistols_multi_point[0]);
		g_config_system->push_item(&g_config_system->heavy_pistols_multi_point[1], "aimbot", "heavy_pistols_multi_point_1", g_config_system->heavy_pistols_multi_point[1]);
		g_config_system->push_item(&g_config_system->heavy_pistols_multi_point[2], "aimbot", "heavy_pistols_multi_point_2", g_config_system->heavy_pistols_multi_point[2]);
		g_config_system->push_item(&g_config_system->heavy_pistols_multi_point[3], "aimbot", "heavy_pistols_multi_point_3", g_config_system->heavy_pistols_multi_point[3]);
		g_config_system->push_item(&g_config_system->heavy_pistols_multi_point[4], "aimbot", "heavy_pistols_multi_point_4", g_config_system->heavy_pistols_multi_point[4]);
		g_config_system->push_item(&g_config_system->heavy_pistols_multi_point[5], "aimbot", "heavy_pistols_multi_point_5", g_config_system->heavy_pistols_multi_point[5]);
		g_config_system->push_item(&g_config_system->heavy_pistols_static_scale, "aimbot", "heavy_pistols_static_scale", g_config_system->heavy_pistols_static_scale);
		g_config_system->push_item(&g_config_system->heavy_pistols_head_scale, "aimbot", "heavy_pistols_head_scale", g_config_system->heavy_pistols_head_scale);
		g_config_system->push_item(&g_config_system->heavy_pistols_body_scale, "aimbot", "heavy_pistols_body_scale", g_config_system->heavy_pistols_body_scale);
		g_config_system->push_item(&g_config_system->heavy_pistols_automatic_fire, "aimbot", "heavy_pistols_automatic_fire", g_config_system->heavy_pistols_automatic_fire);
		g_config_system->push_item(&g_config_system->heavy_pistols_silent_aim, "aimbot", "heavy_pistols_silent_aim", g_config_system->heavy_pistols_silent_aim);
		g_config_system->push_item(&g_config_system->heavy_pistols_minimum_hitchance, "aimbot", "heavy_pistols_minimum_hitchance", g_config_system->heavy_pistols_minimum_hitchance);
		g_config_system->push_item(&g_config_system->heavy_pistols_minimum_damage, "aimbot", "heavy_pistols_minimum_damage", g_config_system->heavy_pistols_minimum_damage);
		g_config_system->push_item(&g_config_system->heavy_pistols_automatic_penetration, "aimbot", "heavy_pistols_automatic_penetration", g_config_system->heavy_pistols_automatic_penetration);
		g_config_system->push_item(&g_config_system->heavy_pistols_penetration_minimum_damage, "aimbot", "heavy_pistols_penetration_minimum_damage", g_config_system->heavy_pistols_penetration_minimum_damage);
		g_config_system->push_item(&g_config_system->heavy_pistols_override_minimum_damage, "aimbot", "heavy_pistols_override_minimum_damage", g_config_system->heavy_pistols_override_minimum_damage);
		g_config_system->push_item(&g_config_system->heavy_pistols_automatic_scope, "aimbot", "heavy_pistols_automatic_scope", g_config_system->heavy_pistols_automatic_scope);
		g_config_system->push_item(&g_config_system->heavy_pistols_maximum_fov, "aimbot", "heavy_pistols_maximum_fov", g_config_system->heavy_pistols_maximum_fov);


		g_config_system->push_item(&g_config_system->sub_machine_guns_target_hitbox[0], "aimbot", "sub_machine_guns_target_hitbox_0", g_config_system->sub_machine_guns_target_hitbox[0]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_target_hitbox[1], "aimbot", "sub_machine_guns_target_hitbox_1", g_config_system->sub_machine_guns_target_hitbox[1]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_target_hitbox[2], "aimbot", "sub_machine_guns_target_hitbox_2", g_config_system->sub_machine_guns_target_hitbox[2]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_target_hitbox[3], "aimbot", "sub_machine_guns_target_hitbox_3", g_config_system->sub_machine_guns_target_hitbox[3]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_target_hitbox[4], "aimbot", "sub_machine_guns_target_hitbox_4", g_config_system->sub_machine_guns_target_hitbox[4]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_target_hitbox[5], "aimbot", "sub_machine_guns_target_hitbox_5", g_config_system->sub_machine_guns_target_hitbox[5]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_multi_point[0], "aimbot", "sub_machine_guns_multi_point_0", g_config_system->sub_machine_guns_multi_point[0]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_multi_point[1], "aimbot", "sub_machine_guns_multi_point_1", g_config_system->sub_machine_guns_multi_point[1]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_multi_point[2], "aimbot", "sub_machine_guns_multi_point_2", g_config_system->sub_machine_guns_multi_point[2]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_multi_point[3], "aimbot", "sub_machine_guns_multi_point_3", g_config_system->sub_machine_guns_multi_point[3]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_multi_point[4], "aimbot", "sub_machine_guns_multi_point_4", g_config_system->sub_machine_guns_multi_point[4]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_multi_point[5], "aimbot", "sub_machine_guns_multi_point_5", g_config_system->sub_machine_guns_multi_point[5]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_static_scale, "aimbot", "sub_machine_guns_static_scale", g_config_system->sub_machine_guns_static_scale);
		g_config_system->push_item(&g_config_system->sub_machine_guns_head_scale, "aimbot", "sub_machine_guns_head_scale", g_config_system->sub_machine_guns_head_scale);
		g_config_system->push_item(&g_config_system->sub_machine_guns_body_scale, "aimbot", "sub_machine_guns_body_scale", g_config_system->sub_machine_guns_body_scale);
		g_config_system->push_item(&g_config_system->sub_machine_guns_automatic_fire, "aimbot", "sub_machine_guns_automatic_fire", g_config_system->sub_machine_guns_automatic_fire);
		g_config_system->push_item(&g_config_system->sub_machine_guns_silent_aim, "aimbot", "sub_machine_guns_silent_aim", g_config_system->sub_machine_guns_silent_aim);
		g_config_system->push_item(&g_config_system->sub_machine_guns_minimum_hitchance, "aimbot", "sub_machine_guns_minimum_hitchance", g_config_system->sub_machine_guns_minimum_hitchance);
		g_config_system->push_item(&g_config_system->sub_machine_guns_minimum_damage, "aimbot", "sub_machine_guns_minimum_damage", g_config_system->sub_machine_guns_minimum_damage);
		g_config_system->push_item(&g_config_system->sub_machine_guns_automatic_penetration, "aimbot", "sub_machine_guns_automatic_penetration", g_config_system->sub_machine_guns_automatic_penetration);
		g_config_system->push_item(&g_config_system->sub_machine_guns_penetration_minimum_damage, "aimbot", "sub_machine_guns_penetration_minimum_damage", g_config_system->sub_machine_guns_penetration_minimum_damage);
		g_config_system->push_item(&g_config_system->sub_machine_guns_override_minimum_damage, "aimbot", "sub_machine_guns_override_minimum_damage", g_config_system->sub_machine_guns_override_minimum_damage);
		g_config_system->push_item(&g_config_system->sub_machine_guns_automatic_scope, "aimbot", "sub_machine_guns_automatic_scope", g_config_system->sub_machine_guns_automatic_scope);
		g_config_system->push_item(&g_config_system->sub_machine_guns_maximum_fov, "aimbot", "sub_machine_guns_maximum_fov", g_config_system->sub_machine_guns_maximum_fov);


		g_config_system->push_item(&g_config_system->rifles_target_hitbox[0], "aimbot", "rifles_target_hitbox_0", g_config_system->rifles_target_hitbox[0]);
		g_config_system->push_item(&g_config_system->rifles_target_hitbox[1], "aimbot", "rifles_target_hitbox_1", g_config_system->rifles_target_hitbox[1]);
		g_config_system->push_item(&g_config_system->rifles_target_hitbox[2], "aimbot", "rifles_target_hitbox_2", g_config_system->rifles_target_hitbox[2]);
		g_config_system->push_item(&g_config_system->rifles_target_hitbox[3], "aimbot", "rifles_target_hitbox_3", g_config_system->rifles_target_hitbox[3]);
		g_config_system->push_item(&g_config_system->rifles_target_hitbox[4], "aimbot", "rifles_target_hitbox_4", g_config_system->rifles_target_hitbox[4]);
		g_config_system->push_item(&g_config_system->rifles_target_hitbox[5], "aimbot", "rifles_target_hitbox_5", g_config_system->rifles_target_hitbox[5]);
		g_config_system->push_item(&g_config_system->rifles_multi_point[0], "aimbot", "rifles_multi_point_0", g_config_system->rifles_multi_point[0]);
		g_config_system->push_item(&g_config_system->rifles_multi_point[1], "aimbot", "rifles_multi_point_1", g_config_system->rifles_multi_point[1]);
		g_config_system->push_item(&g_config_system->rifles_multi_point[2], "aimbot", "rifles_multi_point_2", g_config_system->rifles_multi_point[2]);
		g_config_system->push_item(&g_config_system->rifles_multi_point[3], "aimbot", "rifles_multi_point_3", g_config_system->rifles_multi_point[3]);
		g_config_system->push_item(&g_config_system->rifles_multi_point[4], "aimbot", "rifles_multi_point_4", g_config_system->rifles_multi_point[4]);
		g_config_system->push_item(&g_config_system->rifles_multi_point[5], "aimbot", "rifles_multi_point_5", g_config_system->rifles_multi_point[5]);
		g_config_system->push_item(&g_config_system->rifles_static_scale, "aimbot", "rifles_static_scale", g_config_system->rifles_static_scale);
		g_config_system->push_item(&g_config_system->rifles_head_scale, "aimbot", "rifles_head_scale", g_config_system->rifles_head_scale);
		g_config_system->push_item(&g_config_system->rifles_body_scale, "aimbot", "rifles_body_scale", g_config_system->rifles_body_scale);
		g_config_system->push_item(&g_config_system->rifles_automatic_fire, "aimbot", "rifles_automatic_fire", g_config_system->rifles_automatic_fire);
		g_config_system->push_item(&g_config_system->rifles_silent_aim, "aimbot", "rifles_silent_aim", g_config_system->rifles_silent_aim);
		g_config_system->push_item(&g_config_system->rifles_minimum_hitchance, "aimbot", "rifles_minimum_hitchance", g_config_system->rifles_minimum_hitchance);
		g_config_system->push_item(&g_config_system->rifles_minimum_damage, "aimbot", "rifles_minimum_damage", g_config_system->rifles_minimum_damage);
		g_config_system->push_item(&g_config_system->rifles_automatic_penetration, "aimbot", "rifles_automatic_penetration", g_config_system->rifles_automatic_penetration);
		g_config_system->push_item(&g_config_system->rifles_penetration_minimum_damage, "aimbot", "rifles_penetration_minimum_damage", g_config_system->rifles_penetration_minimum_damage);
		g_config_system->push_item(&g_config_system->rifles_override_minimum_damage, "aimbot", "rifles_override_minimum_damage", g_config_system->rifles_override_minimum_damage);
		g_config_system->push_item(&g_config_system->rifles_automatic_scope, "aimbot", "rifles_automatic_scope", g_config_system->rifles_automatic_scope);
		g_config_system->push_item(&g_config_system->rifles_maximum_fov, "aimbot", "rifles_maximum_fov", g_config_system->rifles_maximum_fov);


		g_config_system->push_item(&g_config_system->auto_snipers_target_hitbox[0], "aimbot", "auto_snipers_target_hitbox_0", g_config_system->auto_snipers_target_hitbox[0]);
		g_config_system->push_item(&g_config_system->auto_snipers_target_hitbox[1], "aimbot", "auto_snipers_target_hitbox_1", g_config_system->auto_snipers_target_hitbox[1]);
		g_config_system->push_item(&g_config_system->auto_snipers_target_hitbox[2], "aimbot", "auto_snipers_target_hitbox_2", g_config_system->auto_snipers_target_hitbox[2]);
		g_config_system->push_item(&g_config_system->auto_snipers_target_hitbox[3], "aimbot", "auto_snipers_target_hitbox_3", g_config_system->auto_snipers_target_hitbox[3]);
		g_config_system->push_item(&g_config_system->auto_snipers_target_hitbox[4], "aimbot", "auto_snipers_target_hitbox_4", g_config_system->auto_snipers_target_hitbox[4]);
		g_config_system->push_item(&g_config_system->auto_snipers_target_hitbox[5], "aimbot", "auto_snipers_target_hitbox_5", g_config_system->auto_snipers_target_hitbox[5]);
		g_config_system->push_item(&g_config_system->auto_snipers_multi_point[0], "aimbot", "auto_snipers_multi_point_0", g_config_system->auto_snipers_multi_point[0]);
		g_config_system->push_item(&g_config_system->auto_snipers_multi_point[1], "aimbot", "auto_snipers_multi_point_1", g_config_system->auto_snipers_multi_point[1]);
		g_config_system->push_item(&g_config_system->auto_snipers_multi_point[2], "aimbot", "auto_snipers_multi_point_2", g_config_system->auto_snipers_multi_point[2]);
		g_config_system->push_item(&g_config_system->auto_snipers_multi_point[3], "aimbot", "auto_snipers_multi_point_3", g_config_system->auto_snipers_multi_point[3]);
		g_config_system->push_item(&g_config_system->auto_snipers_multi_point[4], "aimbot", "auto_snipers_multi_point_4", g_config_system->auto_snipers_multi_point[4]);
		g_config_system->push_item(&g_config_system->auto_snipers_multi_point[5], "aimbot", "auto_snipers_multi_point_5", g_config_system->auto_snipers_multi_point[5]);
		g_config_system->push_item(&g_config_system->auto_snipers_static_scale, "aimbot", "auto_snipers_static_scale", g_config_system->auto_snipers_static_scale);
		g_config_system->push_item(&g_config_system->auto_snipers_head_scale, "aimbot", "auto_snipers_head_scale", g_config_system->auto_snipers_head_scale);
		g_config_system->push_item(&g_config_system->auto_snipers_body_scale, "aimbot", "auto_snipers_body_scale", g_config_system->auto_snipers_body_scale);
		g_config_system->push_item(&g_config_system->auto_snipers_automatic_fire, "aimbot", "auto_snipers_automatic_fire", g_config_system->auto_snipers_automatic_fire);
		g_config_system->push_item(&g_config_system->auto_snipers_silent_aim, "aimbot", "auto_snipers_silent_aim", g_config_system->auto_snipers_silent_aim);
		g_config_system->push_item(&g_config_system->auto_snipers_minimum_hitchance, "aimbot", "auto_snipers_minimum_hitchance", g_config_system->auto_snipers_minimum_hitchance);
		g_config_system->push_item(&g_config_system->auto_snipers_minimum_damage, "aimbot", "auto_snipers_minimum_damage", g_config_system->auto_snipers_minimum_damage);
		g_config_system->push_item(&g_config_system->auto_snipers_automatic_penetration, "aimbot", "auto_snipers_automatic_penetration", g_config_system->auto_snipers_automatic_penetration);
		g_config_system->push_item(&g_config_system->auto_snipers_penetration_minimum_damage, "aimbot", "auto_snipers_penetration_minimum_damage", g_config_system->auto_snipers_penetration_minimum_damage);
		g_config_system->push_item(&g_config_system->auto_snipers_override_minimum_damage, "aimbot", "auto_snipers_override_minimum_damage", g_config_system->auto_snipers_override_minimum_damage);
		g_config_system->push_item(&g_config_system->auto_snipers_automatic_scope, "aimbot", "auto_snipers_automatic_scope", g_config_system->auto_snipers_automatic_scope);
		g_config_system->push_item(&g_config_system->auto_snipers_maximum_fov, "aimbot", "auto_snipers_maximum_fov", g_config_system->auto_snipers_maximum_fov);


		g_config_system->push_item(&g_config_system->scout_target_hitbox[0], "aimbot", "scout_target_hitbox_0", g_config_system->scout_target_hitbox[0]);
		g_config_system->push_item(&g_config_system->scout_target_hitbox[1], "aimbot", "scout_target_hitbox_1", g_config_system->scout_target_hitbox[1]);
		g_config_system->push_item(&g_config_system->scout_target_hitbox[2], "aimbot", "scout_target_hitbox_2", g_config_system->scout_target_hitbox[2]);
		g_config_system->push_item(&g_config_system->scout_target_hitbox[3], "aimbot", "scout_target_hitbox_3", g_config_system->scout_target_hitbox[3]);
		g_config_system->push_item(&g_config_system->scout_target_hitbox[4], "aimbot", "scout_target_hitbox_4", g_config_system->scout_target_hitbox[4]);
		g_config_system->push_item(&g_config_system->scout_target_hitbox[5], "aimbot", "scout_target_hitbox_5", g_config_system->scout_target_hitbox[5]);
		g_config_system->push_item(&g_config_system->scout_multi_point[0], "aimbot", "scout_multi_point_0", g_config_system->scout_multi_point[0]);
		g_config_system->push_item(&g_config_system->scout_multi_point[1], "aimbot", "scout_multi_point_1", g_config_system->scout_multi_point[1]);
		g_config_system->push_item(&g_config_system->scout_multi_point[2], "aimbot", "scout_multi_point_2", g_config_system->scout_multi_point[2]);
		g_config_system->push_item(&g_config_system->scout_multi_point[3], "aimbot", "scout_multi_point_3", g_config_system->scout_multi_point[3]);
		g_config_system->push_item(&g_config_system->scout_multi_point[4], "aimbot", "scout_multi_point_4", g_config_system->scout_multi_point[4]);
		g_config_system->push_item(&g_config_system->scout_multi_point[5], "aimbot", "scout_multi_point_5", g_config_system->scout_multi_point[5]);
		g_config_system->push_item(&g_config_system->scout_static_scale, "aimbot", "scout_static_scale", g_config_system->scout_static_scale);
		g_config_system->push_item(&g_config_system->scout_head_scale, "aimbot", "scout_head_scale", g_config_system->scout_head_scale);
		g_config_system->push_item(&g_config_system->scout_body_scale, "aimbot", "scout_body_scale", g_config_system->scout_body_scale);
		g_config_system->push_item(&g_config_system->scout_automatic_fire, "aimbot", "scout_automatic_fire", g_config_system->scout_automatic_fire);
		g_config_system->push_item(&g_config_system->scout_silent_aim, "aimbot", "scout_silent_aim", g_config_system->scout_silent_aim);
		g_config_system->push_item(&g_config_system->scout_minimum_hitchance, "aimbot", "scout_minimum_hitchance", g_config_system->scout_minimum_hitchance);
		g_config_system->push_item(&g_config_system->scout_minimum_damage, "aimbot", "scout_minimum_damage", g_config_system->scout_minimum_damage);
		g_config_system->push_item(&g_config_system->scout_automatic_penetration, "aimbot", "scout_automatic_penetration", g_config_system->scout_automatic_penetration);
		g_config_system->push_item(&g_config_system->scout_penetration_minimum_damage, "aimbot", "scout_penetration_minimum_damage", g_config_system->scout_penetration_minimum_damage);
		g_config_system->push_item(&g_config_system->scout_override_minimum_damage, "aimbot", "scout_override_minimum_damage", g_config_system->scout_override_minimum_damage);
		g_config_system->push_item(&g_config_system->scout_automatic_scope, "aimbot", "scout_automatic_scope", g_config_system->scout_automatic_scope);
		g_config_system->push_item(&g_config_system->scout_maximum_fov, "aimbot", "scout_maximum_fov", g_config_system->scout_maximum_fov);


		g_config_system->push_item(&g_config_system->awp_target_hitbox[0], "aimbot", "awp_target_hitbox_0", g_config_system->awp_target_hitbox[0]);
		g_config_system->push_item(&g_config_system->awp_target_hitbox[1], "aimbot", "awp_target_hitbox_1", g_config_system->awp_target_hitbox[1]);
		g_config_system->push_item(&g_config_system->awp_target_hitbox[2], "aimbot", "awp_target_hitbox_2", g_config_system->awp_target_hitbox[2]);
		g_config_system->push_item(&g_config_system->awp_target_hitbox[3], "aimbot", "awp_target_hitbox_3", g_config_system->awp_target_hitbox[3]);
		g_config_system->push_item(&g_config_system->awp_target_hitbox[4], "aimbot", "awp_target_hitbox_4", g_config_system->awp_target_hitbox[4]);
		g_config_system->push_item(&g_config_system->awp_target_hitbox[5], "aimbot", "awp_target_hitbox_5", g_config_system->awp_target_hitbox[5]);
		g_config_system->push_item(&g_config_system->awp_multi_point[0], "aimbot", "awp_multi_point_0", g_config_system->awp_multi_point[0]);
		g_config_system->push_item(&g_config_system->awp_multi_point[1], "aimbot", "awp_multi_point_1", g_config_system->awp_multi_point[1]);
		g_config_system->push_item(&g_config_system->awp_multi_point[2], "aimbot", "awp_multi_point_2", g_config_system->awp_multi_point[2]);
		g_config_system->push_item(&g_config_system->awp_multi_point[3], "aimbot", "awp_multi_point_3", g_config_system->awp_multi_point[3]);
		g_config_system->push_item(&g_config_system->awp_multi_point[4], "aimbot", "awp_multi_point_4", g_config_system->awp_multi_point[4]);
		g_config_system->push_item(&g_config_system->awp_multi_point[5], "aimbot", "awp_multi_point_5", g_config_system->awp_multi_point[5]);
		g_config_system->push_item(&g_config_system->awp_static_scale, "aimbot", "awp_static_scale", g_config_system->awp_static_scale);
		g_config_system->push_item(&g_config_system->awp_head_scale, "aimbot", "awp_head_scale", g_config_system->awp_head_scale);
		g_config_system->push_item(&g_config_system->awp_body_scale, "aimbot", "awp_body_scale", g_config_system->awp_body_scale);
		g_config_system->push_item(&g_config_system->awp_automatic_fire, "aimbot", "awp_automatic_fire", g_config_system->awp_automatic_fire);
		g_config_system->push_item(&g_config_system->awp_silent_aim, "aimbot", "awp_silent_aim", g_config_system->awp_silent_aim);
		g_config_system->push_item(&g_config_system->awp_minimum_hitchance, "aimbot", "awp_minimum_hitchance", g_config_system->awp_minimum_hitchance);
		g_config_system->push_item(&g_config_system->awp_minimum_damage, "aimbot", "awp_minimum_damage", g_config_system->awp_minimum_damage);
		g_config_system->push_item(&g_config_system->awp_automatic_penetration, "aimbot", "awp_automatic_penetration", g_config_system->awp_automatic_penetration);
		g_config_system->push_item(&g_config_system->awp_penetration_minimum_damage, "aimbot", "awp_penetration_minimum_damage", g_config_system->awp_penetration_minimum_damage);
		g_config_system->push_item(&g_config_system->awp_override_minimum_damage, "aimbot", "awp_override_minimum_damage", g_config_system->awp_override_minimum_damage);
		g_config_system->push_item(&g_config_system->awp_automatic_scope, "aimbot", "awp_automatic_scope", g_config_system->awp_automatic_scope);
		g_config_system->push_item(&g_config_system->awp_maximum_fov, "aimbot", "awp_maximum_fov", g_config_system->awp_maximum_fov);


		g_config_system->push_item(&g_config_system->shotguns_target_hitbox[0], "aimbot", "shotguns_target_hitbox_0", g_config_system->shotguns_target_hitbox[0]);
		g_config_system->push_item(&g_config_system->shotguns_target_hitbox[1], "aimbot", "shotguns_target_hitbox_1", g_config_system->shotguns_target_hitbox[1]);
		g_config_system->push_item(&g_config_system->shotguns_target_hitbox[2], "aimbot", "shotguns_target_hitbox_2", g_config_system->shotguns_target_hitbox[2]);
		g_config_system->push_item(&g_config_system->shotguns_target_hitbox[3], "aimbot", "shotguns_target_hitbox_3", g_config_system->shotguns_target_hitbox[3]);
		g_config_system->push_item(&g_config_system->shotguns_target_hitbox[4], "aimbot", "shotguns_target_hitbox_4", g_config_system->shotguns_target_hitbox[4]);
		g_config_system->push_item(&g_config_system->shotguns_target_hitbox[5], "aimbot", "shotguns_target_hitbox_5", g_config_system->shotguns_target_hitbox[5]);
		g_config_system->push_item(&g_config_system->shotguns_multi_point[0], "aimbot", "shotguns_multi_point_0", g_config_system->shotguns_multi_point[0]);
		g_config_system->push_item(&g_config_system->shotguns_multi_point[1], "aimbot", "shotguns_multi_point_1", g_config_system->shotguns_multi_point[1]);
		g_config_system->push_item(&g_config_system->shotguns_multi_point[2], "aimbot", "shotguns_multi_point_2", g_config_system->shotguns_multi_point[2]);
		g_config_system->push_item(&g_config_system->shotguns_multi_point[3], "aimbot", "shotguns_multi_point_3", g_config_system->shotguns_multi_point[3]);
		g_config_system->push_item(&g_config_system->shotguns_multi_point[4], "aimbot", "shotguns_multi_point_4", g_config_system->shotguns_multi_point[4]);
		g_config_system->push_item(&g_config_system->shotguns_multi_point[5], "aimbot", "shotguns_multi_point_5", g_config_system->shotguns_multi_point[5]);
		g_config_system->push_item(&g_config_system->shotguns_static_scale, "aimbot", "shotguns_static_scale", g_config_system->shotguns_static_scale);
		g_config_system->push_item(&g_config_system->shotguns_head_scale, "aimbot", "shotguns_head_scale", g_config_system->shotguns_head_scale);
		g_config_system->push_item(&g_config_system->shotguns_body_scale, "aimbot", "shotguns_body_scale", g_config_system->shotguns_body_scale);
		g_config_system->push_item(&g_config_system->shotguns_automatic_fire, "aimbot", "shotguns_automatic_fire", g_config_system->shotguns_automatic_fire);
		g_config_system->push_item(&g_config_system->shotguns_silent_aim, "aimbot", "shotguns_silent_aim", g_config_system->shotguns_silent_aim);
		g_config_system->push_item(&g_config_system->shotguns_minimum_hitchance, "aimbot", "shotguns_minimum_hitchance", g_config_system->shotguns_minimum_hitchance);
		g_config_system->push_item(&g_config_system->shotguns_minimum_damage, "aimbot", "shotguns_minimum_damage", g_config_system->shotguns_minimum_damage);
		g_config_system->push_item(&g_config_system->shotguns_automatic_penetration, "aimbot", "shotguns_automatic_penetration", g_config_system->shotguns_automatic_penetration);
		g_config_system->push_item(&g_config_system->shotguns_penetration_minimum_damage, "aimbot", "shotguns_penetration_minimum_damage", g_config_system->shotguns_penetration_minimum_damage);
		g_config_system->push_item(&g_config_system->shotguns_override_minimum_damage, "aimbot", "shotguns_override_minimum_damage", g_config_system->shotguns_override_minimum_damage);
		g_config_system->push_item(&g_config_system->shotguns_automatic_scope, "aimbot", "shotguns_automatic_scope", g_config_system->shotguns_automatic_scope);
		g_config_system->push_item(&g_config_system->shotguns_maximum_fov, "aimbot", "shotguns_maximum_fov", g_config_system->shotguns_maximum_fov);


		g_config_system->push_item(&g_config_system->machine_guns_target_hitbox[0], "aimbot", "machine_guns_target_hitbox_0", g_config_system->machine_guns_target_hitbox[0]);
		g_config_system->push_item(&g_config_system->machine_guns_target_hitbox[1], "aimbot", "machine_guns_target_hitbox_1", g_config_system->machine_guns_target_hitbox[1]);
		g_config_system->push_item(&g_config_system->machine_guns_target_hitbox[2], "aimbot", "machine_guns_target_hitbox_2", g_config_system->machine_guns_target_hitbox[2]);
		g_config_system->push_item(&g_config_system->machine_guns_target_hitbox[3], "aimbot", "machine_guns_target_hitbox_3", g_config_system->machine_guns_target_hitbox[3]);
		g_config_system->push_item(&g_config_system->machine_guns_target_hitbox[4], "aimbot", "machine_guns_target_hitbox_4", g_config_system->machine_guns_target_hitbox[4]);
		g_config_system->push_item(&g_config_system->machine_guns_target_hitbox[5], "aimbot", "machine_guns_target_hitbox_5", g_config_system->machine_guns_target_hitbox[5]);
		g_config_system->push_item(&g_config_system->machine_guns_multi_point[0], "aimbot", "machine_guns_multi_point_0", g_config_system->machine_guns_multi_point[0]);
		g_config_system->push_item(&g_config_system->machine_guns_multi_point[1], "aimbot", "machine_guns_multi_point_1", g_config_system->machine_guns_multi_point[1]);
		g_config_system->push_item(&g_config_system->machine_guns_multi_point[2], "aimbot", "machine_guns_multi_point_2", g_config_system->machine_guns_multi_point[2]);
		g_config_system->push_item(&g_config_system->machine_guns_multi_point[3], "aimbot", "machine_guns_multi_point_3", g_config_system->machine_guns_multi_point[3]);
		g_config_system->push_item(&g_config_system->machine_guns_multi_point[4], "aimbot", "machine_guns_multi_point_4", g_config_system->machine_guns_multi_point[4]);
		g_config_system->push_item(&g_config_system->machine_guns_multi_point[5], "aimbot", "machine_guns_multi_point_5", g_config_system->machine_guns_multi_point[5]);
		g_config_system->push_item(&g_config_system->machine_guns_static_scale, "aimbot", "machine_guns_static_scale", g_config_system->machine_guns_static_scale);
		g_config_system->push_item(&g_config_system->machine_guns_head_scale, "aimbot", "machine_guns_head_scale", g_config_system->machine_guns_head_scale);
		g_config_system->push_item(&g_config_system->machine_guns_body_scale, "aimbot", "machine_guns_body_scale", g_config_system->machine_guns_body_scale);
		g_config_system->push_item(&g_config_system->machine_guns_automatic_fire, "aimbot", "machine_guns_automatic_fire", g_config_system->machine_guns_automatic_fire);
		g_config_system->push_item(&g_config_system->machine_guns_silent_aim, "aimbot", "machine_guns_silent_aim", g_config_system->machine_guns_silent_aim);
		g_config_system->push_item(&g_config_system->machine_guns_minimum_hitchance, "aimbot", "machine_guns_minimum_hitchance", g_config_system->machine_guns_minimum_hitchance);
		g_config_system->push_item(&g_config_system->machine_guns_minimum_damage, "aimbot", "machine_guns_minimum_damage", g_config_system->machine_guns_minimum_damage);
		g_config_system->push_item(&g_config_system->machine_guns_automatic_penetration, "aimbot", "machine_guns_automatic_penetration", g_config_system->machine_guns_automatic_penetration);
		g_config_system->push_item(&g_config_system->machine_guns_penetration_minimum_damage, "aimbot", "machine_guns_penetration_minimum_damage", g_config_system->machine_guns_penetration_minimum_damage);
		g_config_system->push_item(&g_config_system->machine_guns_override_minimum_damage, "aimbot", "machine_guns_override_minimum_damage", g_config_system->machine_guns_override_minimum_damage);
		g_config_system->push_item(&g_config_system->machine_guns_automatic_scope, "aimbot", "machine_guns_automatic_scope", g_config_system->machine_guns_automatic_scope);
		g_config_system->push_item(&g_config_system->machine_guns_maximum_fov, "aimbot", "machine_guns_maximum_fov", g_config_system->machine_guns_maximum_fov);


		g_config_system->push_item(&g_config_system->taser_target_hitbox[0], "aimbot", "taser_target_hitbox_0", g_config_system->taser_target_hitbox[0]);
		g_config_system->push_item(&g_config_system->taser_target_hitbox[1], "aimbot", "taser_target_hitbox_1", g_config_system->taser_target_hitbox[1]);
		g_config_system->push_item(&g_config_system->taser_target_hitbox[2], "aimbot", "taser_target_hitbox_2", g_config_system->taser_target_hitbox[2]);
		g_config_system->push_item(&g_config_system->taser_target_hitbox[3], "aimbot", "taser_target_hitbox_3", g_config_system->taser_target_hitbox[3]);
		g_config_system->push_item(&g_config_system->taser_target_hitbox[4], "aimbot", "taser_target_hitbox_4", g_config_system->taser_target_hitbox[4]);
		g_config_system->push_item(&g_config_system->taser_target_hitbox[5], "aimbot", "taser_target_hitbox_5", g_config_system->taser_target_hitbox[5]);
		g_config_system->push_item(&g_config_system->taser_multi_point[0], "aimbot", "taser_multi_point_0", g_config_system->taser_multi_point[0]);
		g_config_system->push_item(&g_config_system->taser_multi_point[1], "aimbot", "taser_multi_point_1", g_config_system->taser_multi_point[1]);
		g_config_system->push_item(&g_config_system->taser_multi_point[2], "aimbot", "taser_multi_point_2", g_config_system->taser_multi_point[2]);
		g_config_system->push_item(&g_config_system->taser_multi_point[3], "aimbot", "taser_multi_point_3", g_config_system->taser_multi_point[3]);
		g_config_system->push_item(&g_config_system->taser_multi_point[4], "aimbot", "taser_multi_point_4", g_config_system->taser_multi_point[4]);
		g_config_system->push_item(&g_config_system->taser_multi_point[5], "aimbot", "taser_multi_point_5", g_config_system->taser_multi_point[5]);
		g_config_system->push_item(&g_config_system->taser_static_scale, "aimbot", "taser_static_scale", g_config_system->taser_static_scale);
		g_config_system->push_item(&g_config_system->taser_head_scale, "aimbot", "taser_head_scale", g_config_system->taser_head_scale);
		g_config_system->push_item(&g_config_system->taser_body_scale, "aimbot", "taser_body_scale", g_config_system->taser_body_scale);
		g_config_system->push_item(&g_config_system->taser_automatic_fire, "aimbot", "taser_automatic_fire", g_config_system->taser_automatic_fire);
		g_config_system->push_item(&g_config_system->taser_silent_aim, "aimbot", "taser_silent_aim", g_config_system->taser_silent_aim);
		g_config_system->push_item(&g_config_system->taser_minimum_hitchance, "aimbot", "taser_minimum_hitchance", g_config_system->taser_minimum_hitchance);
		g_config_system->push_item(&g_config_system->taser_minimum_damage, "aimbot", "taser_minimum_damage", g_config_system->taser_minimum_damage);
		g_config_system->push_item(&g_config_system->taser_automatic_penetration, "aimbot", "taser_automatic_penetration", g_config_system->taser_automatic_penetration);
		g_config_system->push_item(&g_config_system->taser_penetration_minimum_damage, "aimbot", "taser_penetration_minimum_damage", g_config_system->taser_penetration_minimum_damage);
		g_config_system->push_item(&g_config_system->taser_override_minimum_damage, "aimbot", "taser_override_minimum_damage", g_config_system->taser_override_minimum_damage);
		g_config_system->push_item(&g_config_system->taser_automatic_scope, "aimbot", "taser_automatic_scope", g_config_system->taser_automatic_scope);
		g_config_system->push_item(&g_config_system->taser_maximum_fov, "aimbot", "taser_maximum_fov", g_config_system->taser_maximum_fov);

		// other.
		g_config_system->push_item(&g_config_system->pistols_remove_recoil, "other", "pistols_remove_recoil", g_config_system->pistols_remove_recoil);
		g_config_system->push_item(&g_config_system->pistols_accuracy_boost, "other", "pistols_accuracy_boost", g_config_system->pistols_accuracy_boost);
		g_config_system->push_item(&g_config_system->pistols_quick_stop, "other", "pistols_quick_stop", g_config_system->pistols_quick_stop);
		g_config_system->push_item(&g_config_system->pistols_type, "other", "pistols_type", g_config_system->pistols_type);
		g_config_system->push_item(&g_config_system->pistols_between_shots, "other", "pistols_between_shots", g_config_system->pistols_between_shots);
		g_config_system->push_item(&g_config_system->pistols_lag_compensation, "other", "pistols_lag_compensation", g_config_system->pistols_lag_compensation);
		g_config_system->push_item(&g_config_system->anti_aim_correction, "other", "anti_aim_correction", g_config_system->anti_aim_correction);
		g_config_system->push_item(&g_config_system->pistols_body_aim_conditions[0], "other", "pistols_body_aim_conditions_0", g_config_system->pistols_body_aim_conditions[0]);
		g_config_system->push_item(&g_config_system->pistols_body_aim_conditions[1], "other", "pistols_body_aim_conditions_1", g_config_system->pistols_body_aim_conditions[1]);
		g_config_system->push_item(&g_config_system->pistols_body_aim_conditions[2], "other", "pistols_body_aim_conditions_2", g_config_system->pistols_body_aim_conditions[2]);
		g_config_system->push_item(&g_config_system->pistols_body_aim_conditions[3], "other", "pistols_body_aim_conditions_3", g_config_system->pistols_body_aim_conditions[3]);
		g_config_system->push_item(&g_config_system->pistols_body_aim_conditions[4], "other", "pistols_body_aim_conditions_4", g_config_system->pistols_body_aim_conditions[4]);
		g_config_system->push_item(&g_config_system->pistols_body_aim_conditions[5], "other", "pistols_body_aim_conditions_5", g_config_system->pistols_body_aim_conditions[5]);
		g_config_system->push_item(&g_config_system->pistols_body_aim_conditions[6], "other", "pistols_body_aim_conditions_6", g_config_system->pistols_body_aim_conditions[6]);
		g_config_system->push_item(&g_config_system->pistols_body_aim_conditions[7], "other", "pistols_body_aim_conditions_7", g_config_system->pistols_body_aim_conditions[7]);
		g_config_system->push_item(&g_config_system->pistols_body_aim_conditions[8], "other", "pistols_body_aim_conditions_8", g_config_system->pistols_body_aim_conditions[8]);
		g_config_system->push_item(&g_config_system->body_aim_key, "other", "nbody_aim_keyame", g_config_system->body_aim_key);
		g_config_system->push_item(&g_config_system->body_aim_key_style, "other", "body_aim_key_style", g_config_system->body_aim_key_style);
		g_config_system->push_item(&g_config_system->pistols_max_misses_body_aim, "other", "pistols_max_misses_body_aim", g_config_system->pistols_max_misses_body_aim);
		g_config_system->push_item(&g_config_system->pistols_safe_point_conditions[0], "other", "pistols_safe_point_conditions_0", g_config_system->pistols_safe_point_conditions[0]);
		g_config_system->push_item(&g_config_system->pistols_safe_point_conditions[1], "other", "pistols_safe_point_conditions_1", g_config_system->pistols_safe_point_conditions[1]);
		g_config_system->push_item(&g_config_system->pistols_safe_point_conditions[2], "other", "pistols_safe_point_conditions_2", g_config_system->pistols_safe_point_conditions[2]);
		g_config_system->push_item(&g_config_system->pistols_safe_point_conditions[3], "other", "pistols_safe_point_conditions_3", g_config_system->pistols_safe_point_conditions[3]);
		g_config_system->push_item(&g_config_system->pistols_safe_point_conditions[4], "other", "pistols_safe_point_conditions_4", g_config_system->pistols_safe_point_conditions[4]);
		g_config_system->push_item(&g_config_system->pistols_safe_point_conditions[5], "other", "pistols_safe_point_conditions_5", g_config_system->pistols_safe_point_conditions[5]);
		g_config_system->push_item(&g_config_system->pistols_safe_point_conditions[6], "other", "pistols_safe_point_conditions_6", g_config_system->pistols_safe_point_conditions[6]);
		g_config_system->push_item(&g_config_system->pistols_safe_point_conditions[7], "other", "pistols_safe_point_conditions_7", g_config_system->pistols_safe_point_conditions[7]);
		g_config_system->push_item(&g_config_system->safe_point_key, "other", "safe_point_key", g_config_system->safe_point_key);
		g_config_system->push_item(&g_config_system->safe_point_key_style, "other", "safe_point_key_style", g_config_system->safe_point_key_style);
		g_config_system->push_item(&g_config_system->pistols_safe_point_type, "other", "pistols_safe_point_type", g_config_system->pistols_safe_point_type);
		g_config_system->push_item(&g_config_system->pistols_max_misses_safe_point, "other", "pistols_max_misses_safe_point", g_config_system->pistols_max_misses_safe_point);
		g_config_system->push_item(&g_config_system->pistols_double_tap, "other", "pistols_double_tap", g_config_system->pistols_double_tap);
		g_config_system->push_item(&g_config_system->double_tap_key, "other", "double_tap_key", g_config_system->double_tap_key);
		g_config_system->push_item(&g_config_system->double_tap_key_style, "other", "double_tap_key_style", g_config_system->double_tap_key_style);
		g_config_system->push_item(&g_config_system->pistols_max_shift_amount, "other", "pistols_max_shift_amount", g_config_system->pistols_max_shift_amount);
		

		g_config_system->push_item(&g_config_system->heavy_pistols_remove_recoil, "other", "heavy_pistols_remove_recoil", g_config_system->heavy_pistols_remove_recoil);
		g_config_system->push_item(&g_config_system->heavy_pistols_accuracy_boost, "other", "heavy_pistols_accuracy_boost", g_config_system->heavy_pistols_accuracy_boost);
		g_config_system->push_item(&g_config_system->heavy_pistols_quick_stop, "other", "heavy_pistols_quick_stop", g_config_system->heavy_pistols_quick_stop);
		g_config_system->push_item(&g_config_system->heavy_pistols_type, "other", "heavy_pistols_type", g_config_system->heavy_pistols_type);
		g_config_system->push_item(&g_config_system->heavy_pistols_between_shots, "other", "heavy_pistols_between_shots", g_config_system->heavy_pistols_between_shots);
		g_config_system->push_item(&g_config_system->heavy_pistols_lag_compensation, "other", "heavy_pistols_lag_compensation", g_config_system->heavy_pistols_lag_compensation);
		g_config_system->push_item(&g_config_system->heavy_pistols_body_aim_conditions[0], "other", "heavy_pistols_body_aim_conditions_0", g_config_system->heavy_pistols_body_aim_conditions[0]);
		g_config_system->push_item(&g_config_system->heavy_pistols_body_aim_conditions[1], "other", "heavy_pistols_body_aim_conditions_1", g_config_system->heavy_pistols_body_aim_conditions[1]);
		g_config_system->push_item(&g_config_system->heavy_pistols_body_aim_conditions[2], "other", "heavy_pistols_body_aim_conditions_2", g_config_system->heavy_pistols_body_aim_conditions[2]);
		g_config_system->push_item(&g_config_system->heavy_pistols_body_aim_conditions[3], "other", "heavy_pistols_body_aim_conditions_3", g_config_system->heavy_pistols_body_aim_conditions[3]);
		g_config_system->push_item(&g_config_system->heavy_pistols_body_aim_conditions[4], "other", "heavy_pistols_body_aim_conditions_4", g_config_system->heavy_pistols_body_aim_conditions[4]);
		g_config_system->push_item(&g_config_system->heavy_pistols_body_aim_conditions[5], "other", "heavy_pistols_body_aim_conditions_5", g_config_system->heavy_pistols_body_aim_conditions[5]);
		g_config_system->push_item(&g_config_system->heavy_pistols_body_aim_conditions[6], "other", "heavy_pistols_body_aim_conditions_6", g_config_system->heavy_pistols_body_aim_conditions[6]);
		g_config_system->push_item(&g_config_system->heavy_pistols_body_aim_conditions[7], "other", "heavy_pistols_body_aim_conditions_7", g_config_system->heavy_pistols_body_aim_conditions[7]);
		g_config_system->push_item(&g_config_system->heavy_pistols_body_aim_conditions[8], "other", "heavy_pistols_body_aim_conditions_8", g_config_system->heavy_pistols_body_aim_conditions[8]);
		g_config_system->push_item(&g_config_system->heavy_pistols_max_misses_body_aim, "other", "heavy_pistols_max_misses_body_aim", g_config_system->heavy_pistols_max_misses_body_aim);
		g_config_system->push_item(&g_config_system->heavy_pistols_safe_point_conditions[0], "other", "heavy_pistols_safe_point_conditions_0", g_config_system->heavy_pistols_safe_point_conditions[0]);
		g_config_system->push_item(&g_config_system->heavy_pistols_safe_point_conditions[1], "other", "heavy_pistols_safe_point_conditions_1", g_config_system->heavy_pistols_safe_point_conditions[1]);
		g_config_system->push_item(&g_config_system->heavy_pistols_safe_point_conditions[2], "other", "heavy_pistols_safe_point_conditions_2", g_config_system->heavy_pistols_safe_point_conditions[2]);
		g_config_system->push_item(&g_config_system->heavy_pistols_safe_point_conditions[3], "other", "heavy_pistols_safe_point_conditions_3", g_config_system->heavy_pistols_safe_point_conditions[3]);
		g_config_system->push_item(&g_config_system->heavy_pistols_safe_point_conditions[4], "other", "heavy_pistols_safe_point_conditions_4", g_config_system->heavy_pistols_safe_point_conditions[4]);
		g_config_system->push_item(&g_config_system->heavy_pistols_safe_point_conditions[5], "other", "heavy_pistols_safe_point_conditions_5", g_config_system->heavy_pistols_safe_point_conditions[5]);
		g_config_system->push_item(&g_config_system->heavy_pistols_safe_point_conditions[6], "other", "heavy_pistols_safe_point_conditions_6", g_config_system->heavy_pistols_safe_point_conditions[6]);
		g_config_system->push_item(&g_config_system->heavy_pistols_safe_point_conditions[7], "other", "heavy_pistols_safe_point_conditions_7", g_config_system->heavy_pistols_safe_point_conditions[7]);
		g_config_system->push_item(&g_config_system->heavy_pistols_safe_point_type, "other", "heavy_pistols_safe_point_type", g_config_system->heavy_pistols_safe_point_type);
		g_config_system->push_item(&g_config_system->heavy_pistols_max_misses_safe_point, "other", "heavy_pistols_max_misses_safe_point", g_config_system->heavy_pistols_max_misses_safe_point);
		g_config_system->push_item(&g_config_system->heavy_pistols_double_tap, "other", "heavy_pistols_double_tap", g_config_system->heavy_pistols_double_tap);
		g_config_system->push_item(&g_config_system->heavy_pistols_max_shift_amount, "other", "heavy_pistols_max_shift_amount", g_config_system->heavy_pistols_max_shift_amount);
		

		g_config_system->push_item(&g_config_system->sub_machine_guns_remove_recoil, "other", "sub_machine_guns_remove_recoil", g_config_system->sub_machine_guns_remove_recoil);
		g_config_system->push_item(&g_config_system->sub_machine_guns_accuracy_boost, "other", "sub_machine_guns_accuracy_boost", g_config_system->sub_machine_guns_accuracy_boost);
		g_config_system->push_item(&g_config_system->sub_machine_guns_quick_stop, "other", "sub_machine_guns_quick_stop", g_config_system->sub_machine_guns_quick_stop);
		g_config_system->push_item(&g_config_system->sub_machine_guns_type, "other", "sub_machine_guns_type", g_config_system->sub_machine_guns_type);
		g_config_system->push_item(&g_config_system->sub_machine_guns_between_shots, "other", "sub_machine_guns_between_shots", g_config_system->sub_machine_guns_between_shots);
		g_config_system->push_item(&g_config_system->sub_machine_guns_lag_compensation, "other", "sub_machine_guns_lag_compensation", g_config_system->sub_machine_guns_lag_compensation);
		g_config_system->push_item(&g_config_system->sub_machine_guns_body_aim_conditions[0], "other", "sub_machine_guns_body_aim_conditions_0", g_config_system->sub_machine_guns_body_aim_conditions[0]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_body_aim_conditions[1], "other", "sub_machine_guns_body_aim_conditions_1", g_config_system->sub_machine_guns_body_aim_conditions[1]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_body_aim_conditions[2], "other", "sub_machine_guns_body_aim_conditions_2", g_config_system->sub_machine_guns_body_aim_conditions[2]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_body_aim_conditions[3], "other", "sub_machine_guns_body_aim_conditions_3", g_config_system->sub_machine_guns_body_aim_conditions[3]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_body_aim_conditions[4], "other", "sub_machine_guns_body_aim_conditions_4", g_config_system->sub_machine_guns_body_aim_conditions[4]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_body_aim_conditions[5], "other", "sub_machine_guns_body_aim_conditions_5", g_config_system->sub_machine_guns_body_aim_conditions[5]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_body_aim_conditions[6], "other", "sub_machine_guns_body_aim_conditions_6", g_config_system->sub_machine_guns_body_aim_conditions[6]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_body_aim_conditions[7], "other", "sub_machine_guns_body_aim_conditions_7", g_config_system->sub_machine_guns_body_aim_conditions[7]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_body_aim_conditions[8], "other", "sub_machine_guns_body_aim_conditions_8", g_config_system->sub_machine_guns_body_aim_conditions[8]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_max_misses_body_aim, "other", "sub_machine_guns_max_misses_body_aim", g_config_system->sub_machine_guns_max_misses_body_aim);
		g_config_system->push_item(&g_config_system->sub_machine_guns_safe_point_conditions[0], "other", "sub_machine_guns_safe_point_conditions_0", g_config_system->sub_machine_guns_safe_point_conditions[0]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_safe_point_conditions[1], "other", "sub_machine_guns_safe_point_conditions_1", g_config_system->sub_machine_guns_safe_point_conditions[1]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_safe_point_conditions[2], "other", "sub_machine_guns_safe_point_conditions_2", g_config_system->sub_machine_guns_safe_point_conditions[2]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_safe_point_conditions[3], "other", "sub_machine_guns_safe_point_conditions_3", g_config_system->sub_machine_guns_safe_point_conditions[3]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_safe_point_conditions[4], "other", "sub_machine_guns_safe_point_conditions_4", g_config_system->sub_machine_guns_safe_point_conditions[4]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_safe_point_conditions[5], "other", "sub_machine_guns_safe_point_conditions_5", g_config_system->sub_machine_guns_safe_point_conditions[5]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_safe_point_conditions[6], "other", "sub_machine_guns_safe_point_conditions_6", g_config_system->sub_machine_guns_safe_point_conditions[6]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_safe_point_conditions[7], "other", "sub_machine_guns_safe_point_conditions_7", g_config_system->sub_machine_guns_safe_point_conditions[7]);
		g_config_system->push_item(&g_config_system->sub_machine_guns_safe_point_type, "other", "sub_machine_guns_safe_point_type", g_config_system->sub_machine_guns_safe_point_type);
		g_config_system->push_item(&g_config_system->sub_machine_guns_max_misses_safe_point, "other", "sub_machine_guns_max_misses_safe_point", g_config_system->sub_machine_guns_max_misses_safe_point);
		g_config_system->push_item(&g_config_system->sub_machine_guns_double_tap, "other", "sub_machine_guns_double_tap", g_config_system->sub_machine_guns_double_tap);
		g_config_system->push_item(&g_config_system->sub_machine_guns_max_shift_amount, "other", "sub_machine_guns_max_shift_amount", g_config_system->sub_machine_guns_max_shift_amount);
		

		g_config_system->push_item(&g_config_system->rifles_remove_recoil, "other", "rifles_remove_recoil", g_config_system->rifles_remove_recoil);
		g_config_system->push_item(&g_config_system->rifles_accuracy_boost, "other", "rifles_accuracy_boost", g_config_system->rifles_accuracy_boost);
		g_config_system->push_item(&g_config_system->rifles_quick_stop, "other", "rifles_quick_stop", g_config_system->rifles_quick_stop);
		g_config_system->push_item(&g_config_system->rifles_type, "other", "rifles_type", g_config_system->rifles_type);
		g_config_system->push_item(&g_config_system->rifles_between_shots, "other", "rifles_between_shots", g_config_system->rifles_between_shots);
		g_config_system->push_item(&g_config_system->rifles_lag_compensation, "other", "rifles_lag_compensation", g_config_system->rifles_lag_compensation);
		g_config_system->push_item(&g_config_system->rifles_body_aim_conditions[0], "other", "rifles_body_aim_conditions_0", g_config_system->rifles_body_aim_conditions[0]);
		g_config_system->push_item(&g_config_system->rifles_body_aim_conditions[1], "other", "rifles_body_aim_conditions_1", g_config_system->rifles_body_aim_conditions[1]);
		g_config_system->push_item(&g_config_system->rifles_body_aim_conditions[2], "other", "rifles_body_aim_conditions_2", g_config_system->rifles_body_aim_conditions[2]);
		g_config_system->push_item(&g_config_system->rifles_body_aim_conditions[3], "other", "rifles_body_aim_conditions_3", g_config_system->rifles_body_aim_conditions[3]);
		g_config_system->push_item(&g_config_system->rifles_body_aim_conditions[4], "other", "rifles_body_aim_conditions_4", g_config_system->rifles_body_aim_conditions[4]);
		g_config_system->push_item(&g_config_system->rifles_body_aim_conditions[5], "other", "rifles_body_aim_conditions_5", g_config_system->rifles_body_aim_conditions[5]);
		g_config_system->push_item(&g_config_system->rifles_body_aim_conditions[6], "other", "rifles_body_aim_conditions_6", g_config_system->rifles_body_aim_conditions[6]);
		g_config_system->push_item(&g_config_system->rifles_body_aim_conditions[7], "other", "rifles_body_aim_conditions_7", g_config_system->rifles_body_aim_conditions[7]);
		g_config_system->push_item(&g_config_system->rifles_body_aim_conditions[8], "other", "rifles_body_aim_conditions_8", g_config_system->rifles_body_aim_conditions[8]);
		g_config_system->push_item(&g_config_system->rifles_max_misses_body_aim, "other", "rifles_max_misses_body_aim", g_config_system->rifles_max_misses_body_aim);
		g_config_system->push_item(&g_config_system->rifles_safe_point_conditions[0], "other", "rifles_safe_point_conditions_0", g_config_system->rifles_safe_point_conditions[0]);
		g_config_system->push_item(&g_config_system->rifles_safe_point_conditions[1], "other", "rifles_safe_point_conditions_1", g_config_system->rifles_safe_point_conditions[1]);
		g_config_system->push_item(&g_config_system->rifles_safe_point_conditions[2], "other", "rifles_safe_point_conditions_2", g_config_system->rifles_safe_point_conditions[2]);
		g_config_system->push_item(&g_config_system->rifles_safe_point_conditions[3], "other", "rifles_safe_point_conditions_3", g_config_system->rifles_safe_point_conditions[3]);
		g_config_system->push_item(&g_config_system->rifles_safe_point_conditions[4], "other", "rifles_safe_point_conditions_4", g_config_system->rifles_safe_point_conditions[4]);
		g_config_system->push_item(&g_config_system->rifles_safe_point_conditions[5], "other", "rifles_safe_point_conditions_5", g_config_system->rifles_safe_point_conditions[5]);
		g_config_system->push_item(&g_config_system->rifles_safe_point_conditions[6], "other", "rifles_safe_point_conditions_6", g_config_system->rifles_safe_point_conditions[6]);
		g_config_system->push_item(&g_config_system->rifles_safe_point_conditions[7], "other", "rifles_safe_point_conditions_7", g_config_system->rifles_safe_point_conditions[7]);
		g_config_system->push_item(&g_config_system->rifles_safe_point_type, "other", "rifles_safe_point_type", g_config_system->rifles_safe_point_type);
		g_config_system->push_item(&g_config_system->rifles_max_misses_safe_point, "other", "rifles_max_misses_safe_point", g_config_system->rifles_max_misses_safe_point);
		g_config_system->push_item(&g_config_system->rifles_double_tap, "other", "rifles_double_tap", g_config_system->rifles_double_tap);
		g_config_system->push_item(&g_config_system->rifles_max_shift_amount, "other", "rifles_max_shift_amount", g_config_system->rifles_max_shift_amount);
		

		g_config_system->push_item(&g_config_system->auto_snipers_remove_recoil, "other", "auto_snipers_remove_recoil", g_config_system->auto_snipers_remove_recoil);
		g_config_system->push_item(&g_config_system->auto_snipers_accuracy_boost, "other", "auto_snipers_accuracy_boost", g_config_system->auto_snipers_accuracy_boost);
		g_config_system->push_item(&g_config_system->auto_snipers_quick_stop, "other", "auto_snipers_quick_stop", g_config_system->auto_snipers_quick_stop);
		g_config_system->push_item(&g_config_system->auto_snipers_type, "other", "auto_snipers_type", g_config_system->auto_snipers_type);
		g_config_system->push_item(&g_config_system->auto_snipers_between_shots, "other", "auto_snipers_between_shots", g_config_system->auto_snipers_between_shots);
		g_config_system->push_item(&g_config_system->auto_snipers_lag_compensation, "other", "auto_snipers_lag_compensation", g_config_system->auto_snipers_lag_compensation);
		g_config_system->push_item(&g_config_system->auto_snipers_body_aim_conditions[0], "other", "auto_snipers_body_aim_conditions_0", g_config_system->auto_snipers_body_aim_conditions[0]);
		g_config_system->push_item(&g_config_system->auto_snipers_body_aim_conditions[1], "other", "auto_snipers_body_aim_conditions_1", g_config_system->auto_snipers_body_aim_conditions[1]);
		g_config_system->push_item(&g_config_system->auto_snipers_body_aim_conditions[2], "other", "auto_snipers_body_aim_conditions_2", g_config_system->auto_snipers_body_aim_conditions[2]);
		g_config_system->push_item(&g_config_system->auto_snipers_body_aim_conditions[3], "other", "auto_snipers_body_aim_conditions_3", g_config_system->auto_snipers_body_aim_conditions[3]);
		g_config_system->push_item(&g_config_system->auto_snipers_body_aim_conditions[4], "other", "auto_snipers_body_aim_conditions_4", g_config_system->auto_snipers_body_aim_conditions[4]);
		g_config_system->push_item(&g_config_system->auto_snipers_body_aim_conditions[5], "other", "auto_snipers_body_aim_conditions_5", g_config_system->auto_snipers_body_aim_conditions[5]);
		g_config_system->push_item(&g_config_system->auto_snipers_body_aim_conditions[6], "other", "auto_snipers_body_aim_conditions_6", g_config_system->auto_snipers_body_aim_conditions[6]);
		g_config_system->push_item(&g_config_system->auto_snipers_body_aim_conditions[7], "other", "auto_snipers_body_aim_conditions_7", g_config_system->auto_snipers_body_aim_conditions[7]);
		g_config_system->push_item(&g_config_system->auto_snipers_body_aim_conditions[8], "other", "auto_snipers_body_aim_conditions_8", g_config_system->auto_snipers_body_aim_conditions[8]);
		g_config_system->push_item(&g_config_system->auto_snipers_max_misses_body_aim, "other", "auto_snipers_max_misses_body_aim", g_config_system->auto_snipers_max_misses_body_aim);
		g_config_system->push_item(&g_config_system->auto_snipers_safe_point_conditions[0], "other", "auto_snipers_safe_point_conditions_0", g_config_system->auto_snipers_safe_point_conditions[0]);
		g_config_system->push_item(&g_config_system->auto_snipers_safe_point_conditions[1], "other", "auto_snipers_safe_point_conditions_1", g_config_system->auto_snipers_safe_point_conditions[1]);
		g_config_system->push_item(&g_config_system->auto_snipers_safe_point_conditions[2], "other", "auto_snipers_safe_point_conditions_2", g_config_system->auto_snipers_safe_point_conditions[2]);
		g_config_system->push_item(&g_config_system->auto_snipers_safe_point_conditions[3], "other", "auto_snipers_safe_point_conditions_3", g_config_system->auto_snipers_safe_point_conditions[3]);
		g_config_system->push_item(&g_config_system->auto_snipers_safe_point_conditions[4], "other", "auto_snipers_safe_point_conditions_4", g_config_system->auto_snipers_safe_point_conditions[4]);
		g_config_system->push_item(&g_config_system->auto_snipers_safe_point_conditions[5], "other", "auto_snipers_safe_point_conditions_5", g_config_system->auto_snipers_safe_point_conditions[5]);
		g_config_system->push_item(&g_config_system->auto_snipers_safe_point_conditions[6], "other", "auto_snipers_safe_point_conditions_6", g_config_system->auto_snipers_safe_point_conditions[6]);
		g_config_system->push_item(&g_config_system->auto_snipers_safe_point_conditions[7], "other", "auto_snipers_safe_point_conditions_7", g_config_system->auto_snipers_safe_point_conditions[7]);
		g_config_system->push_item(&g_config_system->auto_snipers_safe_point_type, "other", "auto_snipers_safe_point_type", g_config_system->auto_snipers_safe_point_type);
		g_config_system->push_item(&g_config_system->auto_snipers_max_misses_safe_point, "other", "auto_snipers_max_misses_safe_point", g_config_system->auto_snipers_max_misses_safe_point);
		g_config_system->push_item(&g_config_system->auto_snipers_double_tap, "other", "auto_snipers_double_tap", g_config_system->auto_snipers_double_tap);
		g_config_system->push_item(&g_config_system->auto_snipers_max_shift_amount, "other", "auto_snipers_max_shift_amount", g_config_system->auto_snipers_max_shift_amount);
		

		g_config_system->push_item(&g_config_system->scout_remove_recoil, "other", "scout_remove_recoil", g_config_system->scout_remove_recoil);
		g_config_system->push_item(&g_config_system->scout_accuracy_boost, "other", "scout_accuracy_boost", g_config_system->scout_accuracy_boost);
		g_config_system->push_item(&g_config_system->scout_quick_stop, "other", "scout_quick_stop", g_config_system->scout_quick_stop);
		g_config_system->push_item(&g_config_system->scout_type, "other", "scout_type", g_config_system->scout_type);
		g_config_system->push_item(&g_config_system->scout_between_shots, "other", "scout_between_shots", g_config_system->scout_between_shots);
		g_config_system->push_item(&g_config_system->scout_lag_compensation, "other", "scout_lag_compensation", g_config_system->scout_lag_compensation);
		g_config_system->push_item(&g_config_system->scout_body_aim_conditions[0], "other", "scout_body_aim_conditions_0", g_config_system->scout_body_aim_conditions[0]);
		g_config_system->push_item(&g_config_system->scout_body_aim_conditions[1], "other", "scout_body_aim_conditions_1", g_config_system->scout_body_aim_conditions[1]);
		g_config_system->push_item(&g_config_system->scout_body_aim_conditions[2], "other", "scout_body_aim_conditions_2", g_config_system->scout_body_aim_conditions[2]);
		g_config_system->push_item(&g_config_system->scout_body_aim_conditions[3], "other", "scout_body_aim_conditions_3", g_config_system->scout_body_aim_conditions[3]);
		g_config_system->push_item(&g_config_system->scout_body_aim_conditions[4], "other", "scout_body_aim_conditions_4", g_config_system->scout_body_aim_conditions[4]);
		g_config_system->push_item(&g_config_system->scout_body_aim_conditions[5], "other", "scout_body_aim_conditions_5", g_config_system->scout_body_aim_conditions[5]);
		g_config_system->push_item(&g_config_system->scout_body_aim_conditions[6], "other", "scout_body_aim_conditions_6", g_config_system->scout_body_aim_conditions[6]);
		g_config_system->push_item(&g_config_system->scout_body_aim_conditions[7], "other", "scout_body_aim_conditions_7", g_config_system->scout_body_aim_conditions[7]);
		g_config_system->push_item(&g_config_system->scout_body_aim_conditions[8], "other", "scout_body_aim_conditions_8", g_config_system->scout_body_aim_conditions[8]);
		g_config_system->push_item(&g_config_system->scout_max_misses_body_aim, "other", "scout_max_misses_body_aim", g_config_system->scout_max_misses_body_aim);
		g_config_system->push_item(&g_config_system->scout_safe_point_conditions[0], "other", "scout_safe_point_conditions_0", g_config_system->scout_safe_point_conditions[0]);
		g_config_system->push_item(&g_config_system->scout_safe_point_conditions[1], "other", "scout_safe_point_conditions_1", g_config_system->scout_safe_point_conditions[1]);
		g_config_system->push_item(&g_config_system->scout_safe_point_conditions[2], "other", "scout_safe_point_conditions_2", g_config_system->scout_safe_point_conditions[2]);
		g_config_system->push_item(&g_config_system->scout_safe_point_conditions[3], "other", "scout_safe_point_conditions_3", g_config_system->scout_safe_point_conditions[3]);
		g_config_system->push_item(&g_config_system->scout_safe_point_conditions[4], "other", "scout_safe_point_conditions_4", g_config_system->scout_safe_point_conditions[4]);
		g_config_system->push_item(&g_config_system->scout_safe_point_conditions[5], "other", "scout_safe_point_conditions_5", g_config_system->scout_safe_point_conditions[5]);
		g_config_system->push_item(&g_config_system->scout_safe_point_conditions[6], "other", "scout_safe_point_conditions_6", g_config_system->scout_safe_point_conditions[6]);
		g_config_system->push_item(&g_config_system->scout_safe_point_conditions[7], "other", "scout_safe_point_conditions_7", g_config_system->scout_safe_point_conditions[7]);
		g_config_system->push_item(&g_config_system->scout_safe_point_type, "other", "scout_safe_point_type", g_config_system->scout_safe_point_type);
		g_config_system->push_item(&g_config_system->scout_max_misses_safe_point, "other", "scout_max_misses_safe_point", g_config_system->scout_max_misses_safe_point);
		g_config_system->push_item(&g_config_system->scout_double_tap, "other", "scout_double_tap", g_config_system->scout_double_tap);
		g_config_system->push_item(&g_config_system->scout_max_shift_amount, "other", "scout_max_shift_amount", g_config_system->scout_max_shift_amount);
		

		g_config_system->push_item(&g_config_system->awp_remove_recoil, "other", "awp_remove_recoil", g_config_system->awp_remove_recoil);
		g_config_system->push_item(&g_config_system->awp_accuracy_boost, "other", "awp_accuracy_boost", g_config_system->awp_accuracy_boost);
		g_config_system->push_item(&g_config_system->awp_quick_stop, "other", "awp_quick_stop", g_config_system->awp_quick_stop);
		g_config_system->push_item(&g_config_system->awp_type, "other", "awp_type", g_config_system->awp_type);
		g_config_system->push_item(&g_config_system->awp_between_shots, "other", "awp_between_shots", g_config_system->awp_between_shots);
		g_config_system->push_item(&g_config_system->awp_lag_compensation, "other", "awp_lag_compensation", g_config_system->awp_lag_compensation);
		g_config_system->push_item(&g_config_system->awp_body_aim_conditions[0], "other", "awp_body_aim_conditions_0", g_config_system->awp_body_aim_conditions[0]);
		g_config_system->push_item(&g_config_system->awp_body_aim_conditions[1], "other", "awp_body_aim_conditions_1", g_config_system->awp_body_aim_conditions[1]);
		g_config_system->push_item(&g_config_system->awp_body_aim_conditions[2], "other", "awp_body_aim_conditions_2", g_config_system->awp_body_aim_conditions[2]);
		g_config_system->push_item(&g_config_system->awp_body_aim_conditions[3], "other", "awp_body_aim_conditions_3", g_config_system->awp_body_aim_conditions[3]);
		g_config_system->push_item(&g_config_system->awp_body_aim_conditions[4], "other", "awp_body_aim_conditions_4", g_config_system->awp_body_aim_conditions[4]);
		g_config_system->push_item(&g_config_system->awp_body_aim_conditions[5], "other", "awp_body_aim_conditions_5", g_config_system->awp_body_aim_conditions[5]);
		g_config_system->push_item(&g_config_system->awp_body_aim_conditions[6], "other", "awp_body_aim_conditions_6", g_config_system->awp_body_aim_conditions[6]);
		g_config_system->push_item(&g_config_system->awp_body_aim_conditions[7], "other", "awp_body_aim_conditions_7", g_config_system->awp_body_aim_conditions[7]);
		g_config_system->push_item(&g_config_system->awp_body_aim_conditions[8], "other", "awp_body_aim_conditions_8", g_config_system->awp_body_aim_conditions[8]);
		g_config_system->push_item(&g_config_system->awp_max_misses_body_aim, "other", "awp_max_misses_body_aim", g_config_system->awp_max_misses_body_aim);
		g_config_system->push_item(&g_config_system->awp_safe_point_conditions[0], "other", "awp_safe_point_conditions_0", g_config_system->awp_safe_point_conditions[0]);
		g_config_system->push_item(&g_config_system->awp_safe_point_conditions[1], "other", "awp_safe_point_conditions_1", g_config_system->awp_safe_point_conditions[1]);
		g_config_system->push_item(&g_config_system->awp_safe_point_conditions[2], "other", "awp_safe_point_conditions_2", g_config_system->awp_safe_point_conditions[2]);
		g_config_system->push_item(&g_config_system->awp_safe_point_conditions[3], "other", "awp_safe_point_conditions_3", g_config_system->awp_safe_point_conditions[3]);
		g_config_system->push_item(&g_config_system->awp_safe_point_conditions[4], "other", "awp_safe_point_conditions_4", g_config_system->awp_safe_point_conditions[4]);
		g_config_system->push_item(&g_config_system->awp_safe_point_conditions[5], "other", "awp_safe_point_conditions_5", g_config_system->awp_safe_point_conditions[5]);
		g_config_system->push_item(&g_config_system->awp_safe_point_conditions[6], "other", "awp_safe_point_conditions_6", g_config_system->awp_safe_point_conditions[6]);
		g_config_system->push_item(&g_config_system->awp_safe_point_conditions[7], "other", "awp_safe_point_conditions_7", g_config_system->awp_safe_point_conditions[7]);
		g_config_system->push_item(&g_config_system->awp_safe_point_type, "other", "awp_safe_point_type", g_config_system->awp_safe_point_type);
		g_config_system->push_item(&g_config_system->awp_max_misses_safe_point, "other", "awp_max_misses_safe_point", g_config_system->awp_max_misses_safe_point);
		g_config_system->push_item(&g_config_system->awp_double_tap, "other", "awp_double_tap", g_config_system->awp_double_tap);
		g_config_system->push_item(&g_config_system->awp_max_shift_amount, "other", "awp_max_shift_amount", g_config_system->awp_max_shift_amount);
		

		g_config_system->push_item(&g_config_system->shotguns_remove_recoil, "other", "shotguns_remove_recoil", g_config_system->shotguns_remove_recoil);
		g_config_system->push_item(&g_config_system->shotguns_accuracy_boost, "other", "shotguns_accuracy_boost", g_config_system->shotguns_accuracy_boost);
		g_config_system->push_item(&g_config_system->shotguns_quick_stop, "other", "shotguns_quick_stop", g_config_system->shotguns_quick_stop);
		g_config_system->push_item(&g_config_system->shotguns_type, "other", "shotguns_type", g_config_system->shotguns_type);
		g_config_system->push_item(&g_config_system->shotguns_between_shots, "other", "shotguns_between_shots", g_config_system->shotguns_between_shots);
		g_config_system->push_item(&g_config_system->shotguns_lag_compensation, "other", "shotguns_lag_compensation", g_config_system->shotguns_lag_compensation);
		g_config_system->push_item(&g_config_system->shotguns_body_aim_conditions[0], "other", "shotguns_body_aim_conditions_0", g_config_system->shotguns_body_aim_conditions[0]);
		g_config_system->push_item(&g_config_system->shotguns_body_aim_conditions[1], "other", "shotguns_body_aim_conditions_1", g_config_system->shotguns_body_aim_conditions[1]);
		g_config_system->push_item(&g_config_system->shotguns_body_aim_conditions[2], "other", "shotguns_body_aim_conditions_2", g_config_system->shotguns_body_aim_conditions[2]);
		g_config_system->push_item(&g_config_system->shotguns_body_aim_conditions[3], "other", "shotguns_body_aim_conditions_3", g_config_system->shotguns_body_aim_conditions[3]);
		g_config_system->push_item(&g_config_system->shotguns_body_aim_conditions[4], "other", "shotguns_body_aim_conditions_4", g_config_system->shotguns_body_aim_conditions[4]);
		g_config_system->push_item(&g_config_system->shotguns_body_aim_conditions[5], "other", "shotguns_body_aim_conditions_5", g_config_system->shotguns_body_aim_conditions[5]);
		g_config_system->push_item(&g_config_system->shotguns_body_aim_conditions[6], "other", "shotguns_body_aim_conditions_6", g_config_system->shotguns_body_aim_conditions[6]);
		g_config_system->push_item(&g_config_system->shotguns_body_aim_conditions[7], "other", "shotguns_body_aim_conditions_7", g_config_system->shotguns_body_aim_conditions[7]);
		g_config_system->push_item(&g_config_system->shotguns_body_aim_conditions[8], "other", "shotguns_body_aim_conditions_8", g_config_system->shotguns_body_aim_conditions[8]);
		g_config_system->push_item(&g_config_system->shotguns_max_misses_body_aim, "other", "shotguns_max_misses_body_aim", g_config_system->shotguns_max_misses_body_aim);
		g_config_system->push_item(&g_config_system->shotguns_safe_point_conditions[0], "other", "shotguns_safe_point_conditions_0", g_config_system->shotguns_safe_point_conditions[0]);
		g_config_system->push_item(&g_config_system->shotguns_safe_point_conditions[1], "other", "shotguns_safe_point_conditions_1", g_config_system->shotguns_safe_point_conditions[1]);
		g_config_system->push_item(&g_config_system->shotguns_safe_point_conditions[2], "other", "shotguns_safe_point_conditions_2", g_config_system->shotguns_safe_point_conditions[2]);
		g_config_system->push_item(&g_config_system->shotguns_safe_point_conditions[3], "other", "shotguns_safe_point_conditions_3", g_config_system->shotguns_safe_point_conditions[3]);
		g_config_system->push_item(&g_config_system->shotguns_safe_point_conditions[4], "other", "shotguns_safe_point_conditions_4", g_config_system->shotguns_safe_point_conditions[4]);
		g_config_system->push_item(&g_config_system->shotguns_safe_point_conditions[5], "other", "shotguns_safe_point_conditions_5", g_config_system->shotguns_safe_point_conditions[5]);
		g_config_system->push_item(&g_config_system->shotguns_safe_point_conditions[6], "other", "shotguns_safe_point_conditions_6", g_config_system->shotguns_safe_point_conditions[6]);
		g_config_system->push_item(&g_config_system->shotguns_safe_point_conditions[7], "other", "shotguns_safe_point_conditions_7", g_config_system->shotguns_safe_point_conditions[7]);
		g_config_system->push_item(&g_config_system->shotguns_safe_point_type, "other", "shotguns_safe_point_type", g_config_system->shotguns_safe_point_type);
		g_config_system->push_item(&g_config_system->shotguns_max_misses_safe_point, "other", "shotguns_max_misses_safe_point", g_config_system->shotguns_max_misses_safe_point);
		g_config_system->push_item(&g_config_system->shotguns_double_tap, "other", "shotguns_double_tap", g_config_system->shotguns_double_tap);
		g_config_system->push_item(&g_config_system->shotguns_max_shift_amount, "other", "shotguns_max_shift_amount", g_config_system->shotguns_max_shift_amount);
		

		g_config_system->push_item(&g_config_system->machine_guns_remove_recoil, "other", "machine_guns_remove_recoil", g_config_system->machine_guns_remove_recoil);
		g_config_system->push_item(&g_config_system->machine_guns_accuracy_boost, "other", "machine_guns_accuracy_boost", g_config_system->machine_guns_accuracy_boost);
		g_config_system->push_item(&g_config_system->machine_guns_quick_stop, "other", "machine_guns_quick_stop", g_config_system->machine_guns_quick_stop);
		g_config_system->push_item(&g_config_system->machine_guns_type, "other", "machine_guns_type", g_config_system->machine_guns_type);
		g_config_system->push_item(&g_config_system->machine_guns_between_shots, "other", "machine_guns_between_shots", g_config_system->machine_guns_between_shots);
		g_config_system->push_item(&g_config_system->machine_guns_lag_compensation, "other", "machine_guns_lag_compensation", g_config_system->machine_guns_lag_compensation);
		g_config_system->push_item(&g_config_system->machine_guns_body_aim_conditions[0], "other", "machine_guns_body_aim_conditions_0", g_config_system->machine_guns_body_aim_conditions[0]);
		g_config_system->push_item(&g_config_system->machine_guns_body_aim_conditions[1], "other", "machine_guns_body_aim_conditions_1", g_config_system->machine_guns_body_aim_conditions[1]);
		g_config_system->push_item(&g_config_system->machine_guns_body_aim_conditions[2], "other", "machine_guns_body_aim_conditions_2", g_config_system->machine_guns_body_aim_conditions[2]);
		g_config_system->push_item(&g_config_system->machine_guns_body_aim_conditions[3], "other", "machine_guns_body_aim_conditions_3", g_config_system->machine_guns_body_aim_conditions[3]);
		g_config_system->push_item(&g_config_system->machine_guns_body_aim_conditions[4], "other", "machine_guns_body_aim_conditions_4", g_config_system->machine_guns_body_aim_conditions[4]);
		g_config_system->push_item(&g_config_system->machine_guns_body_aim_conditions[5], "other", "machine_guns_body_aim_conditions_5", g_config_system->machine_guns_body_aim_conditions[5]);
		g_config_system->push_item(&g_config_system->machine_guns_body_aim_conditions[6], "other", "machine_guns_body_aim_conditions_6", g_config_system->machine_guns_body_aim_conditions[6]);
		g_config_system->push_item(&g_config_system->machine_guns_body_aim_conditions[7], "other", "machine_guns_body_aim_conditions_7", g_config_system->machine_guns_body_aim_conditions[7]);
		g_config_system->push_item(&g_config_system->machine_guns_body_aim_conditions[8], "other", "machine_guns_body_aim_conditions_8", g_config_system->machine_guns_body_aim_conditions[8]);
		g_config_system->push_item(&g_config_system->machine_guns_max_misses_body_aim, "other", "machine_guns_max_misses_body_aim", g_config_system->machine_guns_max_misses_body_aim);
		g_config_system->push_item(&g_config_system->machine_guns_safe_point_conditions[0], "other", "machine_guns_safe_point_conditions_0", g_config_system->machine_guns_safe_point_conditions[0]);
		g_config_system->push_item(&g_config_system->machine_guns_safe_point_conditions[1], "other", "machine_guns_safe_point_conditions_1", g_config_system->machine_guns_safe_point_conditions[1]);
		g_config_system->push_item(&g_config_system->machine_guns_safe_point_conditions[2], "other", "machine_guns_safe_point_conditions_2", g_config_system->machine_guns_safe_point_conditions[2]);
		g_config_system->push_item(&g_config_system->machine_guns_safe_point_conditions[3], "other", "machine_guns_safe_point_conditions_3", g_config_system->machine_guns_safe_point_conditions[3]);
		g_config_system->push_item(&g_config_system->machine_guns_safe_point_conditions[4], "other", "machine_guns_safe_point_conditions_4", g_config_system->machine_guns_safe_point_conditions[4]);
		g_config_system->push_item(&g_config_system->machine_guns_safe_point_conditions[5], "other", "machine_guns_safe_point_conditions_5", g_config_system->machine_guns_safe_point_conditions[5]);
		g_config_system->push_item(&g_config_system->machine_guns_safe_point_conditions[6], "other", "machine_guns_safe_point_conditions_6", g_config_system->machine_guns_safe_point_conditions[6]);
		g_config_system->push_item(&g_config_system->machine_guns_safe_point_conditions[7], "other", "machine_guns_safe_point_conditions_7", g_config_system->machine_guns_safe_point_conditions[7]);
		g_config_system->push_item(&g_config_system->machine_guns_safe_point_type, "other", "machine_guns_safe_point_type", g_config_system->machine_guns_safe_point_type);
		g_config_system->push_item(&g_config_system->machine_guns_max_misses_safe_point, "other", "machine_guns_max_misses_safe_point", g_config_system->machine_guns_max_misses_safe_point);
		g_config_system->push_item(&g_config_system->machine_guns_double_tap, "other", "machine_guns_double_tap", g_config_system->machine_guns_double_tap);
		g_config_system->push_item(&g_config_system->machine_guns_max_shift_amount, "other", "machine_guns_max_shift_amount", g_config_system->machine_guns_max_shift_amount);
		

		g_config_system->push_item(&g_config_system->taser_remove_recoil, "other", "taser_remove_recoil", g_config_system->taser_remove_recoil);
		g_config_system->push_item(&g_config_system->taser_accuracy_boost, "other", "taser_accuracy_boost", g_config_system->taser_accuracy_boost);
		g_config_system->push_item(&g_config_system->taser_quick_stop, "other", "taser_quick_stop", g_config_system->taser_quick_stop);
		g_config_system->push_item(&g_config_system->taser_type, "other", "taser_type", g_config_system->taser_type);
		g_config_system->push_item(&g_config_system->taser_between_shots, "other", "taser_between_shots", g_config_system->taser_between_shots);
		g_config_system->push_item(&g_config_system->taser_lag_compensation, "other", "taser_lag_compensation", g_config_system->taser_lag_compensation);
		g_config_system->push_item(&g_config_system->taser_body_aim_conditions[0], "other", "taser_body_aim_conditions_0", g_config_system->taser_body_aim_conditions[0]);
		g_config_system->push_item(&g_config_system->taser_body_aim_conditions[1], "other", "taser_body_aim_conditions_1", g_config_system->taser_body_aim_conditions[1]);
		g_config_system->push_item(&g_config_system->taser_body_aim_conditions[2], "other", "taser_body_aim_conditions_2", g_config_system->taser_body_aim_conditions[2]);
		g_config_system->push_item(&g_config_system->taser_body_aim_conditions[3], "other", "taser_body_aim_conditions_3", g_config_system->taser_body_aim_conditions[3]);
		g_config_system->push_item(&g_config_system->taser_body_aim_conditions[4], "other", "taser_body_aim_conditions_4", g_config_system->taser_body_aim_conditions[4]);
		g_config_system->push_item(&g_config_system->taser_body_aim_conditions[5], "other", "taser_body_aim_conditions_5", g_config_system->taser_body_aim_conditions[5]);
		g_config_system->push_item(&g_config_system->taser_body_aim_conditions[6], "other", "taser_body_aim_conditions_6", g_config_system->taser_body_aim_conditions[6]);
		g_config_system->push_item(&g_config_system->taser_body_aim_conditions[7], "other", "taser_body_aim_conditions_7", g_config_system->taser_body_aim_conditions[7]);
		g_config_system->push_item(&g_config_system->taser_body_aim_conditions[8], "other", "taser_body_aim_conditions_8", g_config_system->taser_body_aim_conditions[8]);
		g_config_system->push_item(&g_config_system->taser_max_misses_body_aim, "other", "taser_max_misses_body_aim", g_config_system->taser_max_misses_body_aim);
		g_config_system->push_item(&g_config_system->taser_safe_point_conditions[0], "other", "taser_safe_point_conditions_0", g_config_system->taser_safe_point_conditions[0]);
		g_config_system->push_item(&g_config_system->taser_safe_point_conditions[1], "other", "taser_safe_point_conditions_1", g_config_system->taser_safe_point_conditions[1]);
		g_config_system->push_item(&g_config_system->taser_safe_point_conditions[2], "other", "taser_safe_point_conditions_2", g_config_system->taser_safe_point_conditions[2]);
		g_config_system->push_item(&g_config_system->taser_safe_point_conditions[3], "other", "taser_safe_point_conditions_3", g_config_system->taser_safe_point_conditions[3]);
		g_config_system->push_item(&g_config_system->taser_safe_point_conditions[4], "other", "taser_safe_point_conditions_4", g_config_system->taser_safe_point_conditions[4]);
		g_config_system->push_item(&g_config_system->taser_safe_point_conditions[5], "other", "taser_safe_point_conditions_5", g_config_system->taser_safe_point_conditions[5]);
		g_config_system->push_item(&g_config_system->taser_safe_point_conditions[6], "other", "taser_safe_point_conditions_6", g_config_system->taser_safe_point_conditions[6]);
		g_config_system->push_item(&g_config_system->taser_safe_point_conditions[7], "other", "taser_safe_point_conditions_7", g_config_system->taser_safe_point_conditions[7]);
		g_config_system->push_item(&g_config_system->taser_safe_point_type, "other", "taser_safe_point_type", g_config_system->taser_safe_point_type);
		g_config_system->push_item(&g_config_system->taser_max_misses_safe_point, "other", "taser_max_misses_safe_point", g_config_system->taser_max_misses_safe_point);
		g_config_system->push_item(&g_config_system->taser_double_tap, "other", "taser_double_tap", g_config_system->taser_double_tap);
		g_config_system->push_item(&g_config_system->taser_max_shift_amount, "other", "taser_max_shift_amount", g_config_system->taser_max_shift_amount);
		
		// anti-aimbot angles.
		g_config_system->push_item(&g_config_system->enabled2, "anti-aimbot angles", "enabled", g_config_system->enabled2);
		g_config_system->push_item(&g_config_system->pitch, "anti-aimbot angles", "pitch", g_config_system->pitch);
		g_config_system->push_item(&g_config_system->yaw_base, "anti-aimbot angles", "yaw_base", g_config_system->yaw_base);
		g_config_system->push_item(&g_config_system->yaw, "anti-aimbot angles", "yaw", g_config_system->yaw);
		g_config_system->push_item(&g_config_system->freestanding, "anti-aimbot angles", "freestanding", g_config_system->freestanding);
		g_config_system->push_item(&g_config_system->freestanding_key, "anti-aimbot angles", "freestanding_key", g_config_system->freestanding_key);
		g_config_system->push_item(&g_config_system->freestanding_key_style, "anti-aimbot angles", "freestanding_key_style", g_config_system->freestanding_key_style);
		g_config_system->push_item(&g_config_system->jitter_range, "anti-aimbot angles", "jitter_range", g_config_system->jitter_range);
		g_config_system->push_item(&g_config_system->jitter_switch, "anti-aimbot angles", "jitter_switch", g_config_system->jitter_switch);
		g_config_system->push_item(&g_config_system->rotate_range, "anti-aimbot angles", "rotate_range", g_config_system->rotate_range);
		g_config_system->push_item(&g_config_system->rotate_speed, "anti-aimbot angles", "rotate_speed", g_config_system->rotate_speed);
		g_config_system->push_item(&g_config_system->body_yaw, "anti-aimbot angles", "body_yaw", g_config_system->body_yaw);
		g_config_system->push_item(&g_config_system->body_yaw_key, "anti-aimbot angles", "body_yaw_key", g_config_system->body_yaw_key);
		g_config_system->push_item(&g_config_system->body_yaw_key_style, "anti-aimbot angles", "body_yaw_key_style", g_config_system->body_yaw_key_style);
		g_config_system->push_item(&g_config_system->yaw_limit, "anti-aimbot angles", "yaw_limit", g_config_system->yaw_limit);
		g_config_system->push_item(&g_config_system->body_lean, "anti-aimbot angles", "body_lean", g_config_system->body_lean);
		g_config_system->push_item(&g_config_system->inverted_body_lean, "anti-aimbot angles", "inverted_body_lean", g_config_system->inverted_body_lean);
		g_config_system->push_item(&g_config_system->on_shot_side, "anti-aimbot angles", "on_shot_side", g_config_system->on_shot_side);
		g_config_system->push_item(&g_config_system->automatic_side_selection, "anti-aimbot angles", "automatic_side_selection", g_config_system->automatic_side_selection);		
		g_config_system->push_item(&g_config_system->manual_left_key, "anti-aimbot angles", "manual_left_key", g_config_system->manual_left_key);
		g_config_system->push_item(&g_config_system->manual_right_key, "anti-aimbot angles", "manual_right_key", g_config_system->manual_right_key);
		g_config_system->push_item(&g_config_system->manual_back_key, "anti-aimbot angles", "manual_back_key", g_config_system->manual_back_key);

		// other.
		g_config_system->push_item(&g_config_system->fake_lag, "other", "fake_lag", g_config_system->fake_lag);
		g_config_system->push_item(&g_config_system->amount, "other", "amount", g_config_system->amount);
		g_config_system->push_item(&g_config_system->conditions[0], "other", "conditions_0", g_config_system->conditions[0]);
		g_config_system->push_item(&g_config_system->conditions[1], "other", "conditions_1", g_config_system->conditions[1]);
		g_config_system->push_item(&g_config_system->conditions[2], "other", "conditions_2", g_config_system->conditions[2]);
		g_config_system->push_item(&g_config_system->factor_limit, "other", "factor_limit", g_config_system->factor_limit);
		g_config_system->push_item(&g_config_system->force_when_peeking, "other", "force_when_peeking", g_config_system->force_when_peeking);
		g_config_system->push_item(&g_config_system->hide_shots, "other", "hide_shots", g_config_system->hide_shots);
		g_config_system->push_item(&g_config_system->hide_shots_key, "other", "hide_shots_key", g_config_system->hide_shots_key);
		g_config_system->push_item(&g_config_system->hide_shots_key_style, "other", "hide_shots_key_style", g_config_system->hide_shots_key_style);

		// player esp.
		g_config_system->push_item(&g_config_system->teammates, "player esp", "teammates", g_config_system->teammates);
		g_config_system->push_item(&g_config_system->dormant, "player esp", "dormant", g_config_system->dormant);
		g_config_system->push_item(&g_config_system->bounding_box, "player esp", "bounding_box", g_config_system->bounding_box);
		g_config_system->push_item(&g_config_system->bounding_box_color[0], "player esp", "bounding_box_color_r", g_config_system->bounding_box_color[0]);
		g_config_system->push_item(&g_config_system->bounding_box_color[1], "player esp", "bounding_box_color_g", g_config_system->bounding_box_color[1]);
		g_config_system->push_item(&g_config_system->bounding_box_color[2], "player esp", "bounding_box_color_b", g_config_system->bounding_box_color[2]);
		g_config_system->push_item(&g_config_system->bounding_box_color[3], "player esp", "bounding_box_color_a", g_config_system->bounding_box_color[3]);
		g_config_system->push_item(&g_config_system->health_bar, "player esp", "health_bar", g_config_system->health_bar);
		g_config_system->push_item(&g_config_system->name, "player esp", "name", g_config_system->name);
		g_config_system->push_item(&g_config_system->name_color[0], "player esp", "name_color_r", g_config_system->name_color[0]);
		g_config_system->push_item(&g_config_system->name_color[1], "player esp", "name_color_g", g_config_system->name_color[1]);
		g_config_system->push_item(&g_config_system->name_color[2], "player esp", "name_color_b", g_config_system->name_color[2]);
		g_config_system->push_item(&g_config_system->name_color[3], "player esp", "name_color_a", g_config_system->name_color[3]);
		g_config_system->push_item(&g_config_system->flags, "player esp", "flags", g_config_system->flags);
		g_config_system->push_item(&g_config_system->weapon_text, "player esp", "weapon_text", g_config_system->weapon_text);
		g_config_system->push_item(&g_config_system->weapon_icon, "player esp", "weapon_icon", g_config_system->weapon_icon);
		g_config_system->push_item(&g_config_system->weapon_color[0], "player esp", "weapon_color_r", g_config_system->weapon_color[0]);
		g_config_system->push_item(&g_config_system->weapon_color[1], "player esp", "weapon_color_g", g_config_system->weapon_color[1]);
		g_config_system->push_item(&g_config_system->weapon_color[2], "player esp", "weapon_color_b", g_config_system->weapon_color[2]);
		g_config_system->push_item(&g_config_system->weapon_color[3], "player esp", "weapon_color_a", g_config_system->weapon_color[3]);
		g_config_system->push_item(&g_config_system->ammo, "player esp", "ammo", g_config_system->ammo);
		g_config_system->push_item(&g_config_system->ammo_color[0], "player esp", "ammo_color_r", g_config_system->ammo_color[0]);
		g_config_system->push_item(&g_config_system->ammo_color[1], "player esp", "ammo_color_g", g_config_system->ammo_color[1]);
		g_config_system->push_item(&g_config_system->ammo_color[2], "player esp", "ammo_color_b", g_config_system->ammo_color[2]);
		g_config_system->push_item(&g_config_system->ammo_color[3], "player esp", "ammo_color_a", g_config_system->ammo_color[3]);
		g_config_system->push_item(&g_config_system->distance, "player esp", "distance", g_config_system->distance);
		g_config_system->push_item(&g_config_system->glow, "player esp", "glow", g_config_system->glow);
		g_config_system->push_item(&g_config_system->glow_color[0], "player esp", "glow_color_r", g_config_system->glow_color[0]);
		g_config_system->push_item(&g_config_system->glow_color[1], "player esp", "glow_color_g", g_config_system->glow_color[1]);
		g_config_system->push_item(&g_config_system->glow_color[2], "player esp", "glow_color_b", g_config_system->glow_color[2]);
		g_config_system->push_item(&g_config_system->glow_color[3], "player esp", "glow_color_a", g_config_system->glow_color[3]);
		g_config_system->push_item(&g_config_system->hit_marker, "player esp", "hit_marker", g_config_system->hit_marker);
		g_config_system->push_item(&g_config_system->hit_marker_sound, "player esp", "hit_marker_sound", g_config_system->hit_marker_sound);
		g_config_system->push_item(&g_config_system->visualize_sounds, "player esp", "visualize_sounds", g_config_system->visualize_sounds);
		g_config_system->push_item(&g_config_system->visualize_sounds_color[0], "player esp", "visualize_sounds_color_r", g_config_system->visualize_sounds_color[0]);
		g_config_system->push_item(&g_config_system->visualize_sounds_color[1], "player esp", "visualize_sounds_color_g", g_config_system->visualize_sounds_color[1]);
		g_config_system->push_item(&g_config_system->visualize_sounds_color[2], "player esp", "visualize_sounds_color_b", g_config_system->visualize_sounds_color[2]);
		g_config_system->push_item(&g_config_system->visualize_sounds_color[3], "player esp", "visualize_sounds_color_a", g_config_system->visualize_sounds_color[3]);
		g_config_system->push_item(&g_config_system->money, "player esp", "money", g_config_system->money);
		g_config_system->push_item(&g_config_system->skeleton, "player esp", "skeleton", g_config_system->skeleton);
		g_config_system->push_item(&g_config_system->skeleton_color[0], "player esp", "skeleton_color_r", g_config_system->skeleton_color[0]);
		g_config_system->push_item(&g_config_system->skeleton_color[1], "player esp", "skeleton_color_g", g_config_system->skeleton_color[1]);
		g_config_system->push_item(&g_config_system->skeleton_color[2], "player esp", "skeleton_color_b", g_config_system->skeleton_color[2]);
		g_config_system->push_item(&g_config_system->skeleton_color[3], "player esp", "skeleton_colorr_a", g_config_system->skeleton_color[3]);
		g_config_system->push_item(&g_config_system->out_of_fow_arrow, "player esp", "out_of_fow_arrow", g_config_system->out_of_fow_arrow);
		g_config_system->push_item(&g_config_system->out_of_fow_arrow_color[0], "player esp", "out_of_fow_arrow_color_r", g_config_system->out_of_fow_arrow_color[0]);
		g_config_system->push_item(&g_config_system->out_of_fow_arrow_color[1], "player esp", "out_of_fow_arrow_color_g", g_config_system->out_of_fow_arrow_color[1]);
		g_config_system->push_item(&g_config_system->out_of_fow_arrow_color[2], "player esp", "out_of_fow_arrow_color_b", g_config_system->out_of_fow_arrow_color[2]);
		g_config_system->push_item(&g_config_system->out_of_fow_arrow_color[3], "player esp", "out_of_fow_arrow_color_a", g_config_system->out_of_fow_arrow_color[3]);

		// colored models.
		g_config_system->push_item(&g_config_system->player, "colored models", "player", g_config_system->player);
		g_config_system->push_item(&g_config_system->player_color[0], "colored models", "player_color_r", g_config_system->player_color[0]);
		g_config_system->push_item(&g_config_system->player_color[1], "colored models", "player_color_g", g_config_system->player_color[1]);
		g_config_system->push_item(&g_config_system->player_color[2], "colored models", "player_color_b", g_config_system->player_color[2]);
		g_config_system->push_item(&g_config_system->player_color[3], "colored models", "player_color_a", g_config_system->player_color[3]);
		g_config_system->push_item(&g_config_system->player_model, "colored models", "player_model", g_config_system->player_model);
		g_config_system->push_item(&g_config_system->player_behind_wall, "colored models", "player_behind_wall", g_config_system->player_behind_wall);
		g_config_system->push_item(&g_config_system->player_behind_wall_color[0], "colored models", "player_behind_wall_color_r", g_config_system->player_behind_wall_color[0]);
		g_config_system->push_item(&g_config_system->player_behind_wall_color[1], "colored models", "player_behind_wall_color_g", g_config_system->player_behind_wall_color[1]);
		g_config_system->push_item(&g_config_system->player_behind_wall_color[2], "colored models", "player_behind_wall_color_b", g_config_system->player_behind_wall_color[2]);
		g_config_system->push_item(&g_config_system->player_behind_wall_color[3], "colored models", "player_behind_wall_color_a", g_config_system->player_behind_wall_color[3]);
		g_config_system->push_item(&g_config_system->player_behind_wall_model, "colored models", "player_behind_wall_model", g_config_system->player_behind_wall_model);
		g_config_system->push_item(&g_config_system->teammate, "colored models", "teammate", g_config_system->teammate);
		g_config_system->push_item(&g_config_system->teammate_color[0], "colored models", "teammate_color_r", g_config_system->teammate_color[0]);
		g_config_system->push_item(&g_config_system->teammate_color[1], "colored models", "teammate_color_g", g_config_system->teammate_color[1]);
		g_config_system->push_item(&g_config_system->teammate_color[2], "colored models", "teammate_color_b", g_config_system->teammate_color[2]);
		g_config_system->push_item(&g_config_system->teammate_color[3], "colored models", "teammate_color_a", g_config_system->teammate_color[3]);
		g_config_system->push_item(&g_config_system->teammate_model, "colored models", "teammate_model", g_config_system->teammate_model);
		g_config_system->push_item(&g_config_system->teammate_behind_wall, "colored models", "teammate_behind_wall", g_config_system->teammate_behind_wall);
		g_config_system->push_item(&g_config_system->teammate_behind_wall_color[0], "colored models", "teammate_behind_wall_color_r", g_config_system->teammate_behind_wall_color[0]);
		g_config_system->push_item(&g_config_system->teammate_behind_wall_color[1], "colored models", "teammate_behind_wall_color_g", g_config_system->teammate_behind_wall_color[1]);
		g_config_system->push_item(&g_config_system->teammate_behind_wall_color[2], "colored models", "teammate_behind_wall_color_b", g_config_system->teammate_behind_wall_color[2]);
		g_config_system->push_item(&g_config_system->teammate_behind_wall_color[3], "colored models", "teammate_behind_wall_color_a", g_config_system->teammate_behind_wall_color[3]);
		g_config_system->push_item(&g_config_system->teammate_behind_wall_model, "colored models", "teammate_behind_wall_model", g_config_system->teammate_behind_wall_model);
		g_config_system->push_item(&g_config_system->local_player, "colored models", "local_player", g_config_system->local_player);
		g_config_system->push_item(&g_config_system->local_player_color[0], "colored models", "local_player_color_r", g_config_system->local_player_color[0]);
		g_config_system->push_item(&g_config_system->local_player_color[1], "colored models", "local_player_color_g", g_config_system->local_player_color[1]);
		g_config_system->push_item(&g_config_system->local_player_color[2], "colored models", "local_player_color_b", g_config_system->local_player_color[2]);
		g_config_system->push_item(&g_config_system->local_player_color[3], "colored models", "local_player_color_a", g_config_system->local_player_color[3]);
		g_config_system->push_item(&g_config_system->local_player_model, "colored models", "local_player_model", g_config_system->local_player_model);
		g_config_system->push_item(&g_config_system->local_player_fake, "colored models", "local_player_fake", g_config_system->local_player_fake);
		g_config_system->push_item(&g_config_system->local_player_fake_color[0], "colored models", "local_player_fake_color_r", g_config_system->local_player_fake_color[0]);
		g_config_system->push_item(&g_config_system->local_player_fake_color[1], "colored models", "local_player_fake_color_g", g_config_system->local_player_fake_color[1]);
		g_config_system->push_item(&g_config_system->local_player_fake_color[2], "colored models", "local_player_fake_color_b", g_config_system->local_player_fake_color[2]);
		g_config_system->push_item(&g_config_system->local_player_fake_color[3], "colored models", "local_player_fake_color_a", g_config_system->local_player_fake_color[3]);
		g_config_system->push_item(&g_config_system->local_player_fake_model, "colored models", "local_player_fake_model", g_config_system->local_player_fake_model);
		g_config_system->push_item(&g_config_system->on_shot, "colored models", "on_shot", g_config_system->on_shot);
		g_config_system->push_item(&g_config_system->on_shot_color[0], "colored models", "on_shot_color_r", g_config_system->on_shot_color[0]);
		g_config_system->push_item(&g_config_system->on_shot_color[1], "colored models", "on_shot_color_g", g_config_system->on_shot_color[1]);
		g_config_system->push_item(&g_config_system->on_shot_color[2], "colored models", "on_shot_color_b", g_config_system->on_shot_color[2]);
		g_config_system->push_item(&g_config_system->on_shot_color[3], "colored models", "on_shot_color_a", g_config_system->on_shot_color[3]);
		g_config_system->push_item(&g_config_system->on_shot_model, "colored models", "on_shot_model", g_config_system->on_shot_model);
		g_config_system->push_item(&g_config_system->on_shot_time, "colored models", "on_shot_time", g_config_system->on_shot_time);
		g_config_system->push_item(&g_config_system->ragdolls, "colored models", "ragdolls", g_config_system->ragdolls);
		g_config_system->push_item(&g_config_system->hands, "colored models", "hands", g_config_system->hands);
		g_config_system->push_item(&g_config_system->hands_color[0], "colored models", "hands_color_r", g_config_system->hands_color[0]);
		g_config_system->push_item(&g_config_system->hands_color[1], "colored models", "hands_color_g", g_config_system->hands_color[1]);
		g_config_system->push_item(&g_config_system->hands_color[2], "colored models", "hands_color_b", g_config_system->hands_color[2]);
		g_config_system->push_item(&g_config_system->hands_color[3], "colored models", "hands_color_a", g_config_system->hands_color[3]);
		g_config_system->push_item(&g_config_system->hands_model, "colored models", "hands_model", g_config_system->hands_model);
		g_config_system->push_item(&g_config_system->weapon_viewmodel, "colored models", "weapon_viewmodel", g_config_system->weapon_viewmodel);
		g_config_system->push_item(&g_config_system->weapon_viewmodel_color[0], "colored models", "weapon_viewmodel_color_r", g_config_system->weapon_viewmodel_color[0]);
		g_config_system->push_item(&g_config_system->weapon_viewmodel_color[1], "colored models", "weapon_viewmodel_color_g", g_config_system->weapon_viewmodel_color[1]);
		g_config_system->push_item(&g_config_system->weapon_viewmodel_color[2], "colored models", "weapon_viewmodel_color_b", g_config_system->weapon_viewmodel_color[2]);
		g_config_system->push_item(&g_config_system->weapon_viewmodel_color[3], "colored models", "weapon_viewmodel_color_a", g_config_system->weapon_viewmodel_color[3]);
		g_config_system->push_item(&g_config_system->weapon_viewmodel_model, "colored models", "weapon_viewmodel_model", g_config_system->weapon_viewmodel_model);
		g_config_system->push_item(&g_config_system->weapons, "colored models", "weapons", g_config_system->weapons);
		g_config_system->push_item(&g_config_system->weapons_color[0], "colored models", "weapons_color_r", g_config_system->weapons_color[0]);
		g_config_system->push_item(&g_config_system->weapons_color[1], "colored models", "weapons_color_g", g_config_system->weapons_color[1]);
		g_config_system->push_item(&g_config_system->weapons_color[2], "colored models", "weapons_color_b", g_config_system->weapons_color[2]);
		g_config_system->push_item(&g_config_system->weapons_color[3], "colored models", "weapons_color_a", g_config_system->weapons_color[3]);
		g_config_system->push_item(&g_config_system->weapons_model, "colored models", "weapons_model", g_config_system->weapons_model);
		g_config_system->push_item(&g_config_system->disable_model_occlusion, "colored models", "disable_model_occlusion", g_config_system->disable_model_occlusion);
		g_config_system->push_item(&g_config_system->shadow, "colored models", "shadow", g_config_system->shadow);
		g_config_system->push_item(&g_config_system->shadow_color[0], "colored models", "shadow_color_r", g_config_system->shadow_color[0]);
		g_config_system->push_item(&g_config_system->shadow_color[1], "colored models", "shadow_color_g", g_config_system->shadow_color[1]);
		g_config_system->push_item(&g_config_system->shadow_color[2], "colored models", "shadow_color_b", g_config_system->shadow_color[2]);
		g_config_system->push_item(&g_config_system->shadow_color[3], "colored models", "shadow_color_a", g_config_system->shadow_color[3]);
		g_config_system->push_item(&g_config_system->shadow_model, "colored models", "shadow_model", g_config_system->shadow_model);

		// other esp.
		g_config_system->push_item(&g_config_system->radar, "other esp", "radar", g_config_system->radar);
		g_config_system->push_item(&g_config_system->dropped_weapons, "other esp", "dropped_weapons", g_config_system->dropped_weapons);
		g_config_system->push_item(&g_config_system->dropped_weapons_color[0], "other esp", "dropped_weapons_color_r", g_config_system->dropped_weapons_color[0]);
		g_config_system->push_item(&g_config_system->dropped_weapons_color[1], "other esp", "dropped_weapons_color_g", g_config_system->dropped_weapons_color[1]);
		g_config_system->push_item(&g_config_system->dropped_weapons_color[2], "other esp", "dropped_weapons_color_b", g_config_system->dropped_weapons_color[2]);
		g_config_system->push_item(&g_config_system->dropped_weapons_color[3], "other esp", "dropped_weapons_color_a", g_config_system->dropped_weapons_color[3]);
		g_config_system->push_item(&g_config_system->grenades, "other esp", "grenades", g_config_system->grenades);
		g_config_system->push_item(&g_config_system->grenades_color[0], "other esp", "grenades_color_r", g_config_system->grenades_color[0]);
		g_config_system->push_item(&g_config_system->grenades_color[1], "other esp", "grenades_color_g", g_config_system->grenades_color[1]);
		g_config_system->push_item(&g_config_system->grenades_color[2], "other esp", "grenades_color_b", g_config_system->grenades_color[2]);
		g_config_system->push_item(&g_config_system->grenades_color[3], "other esp", "grenades_color_a", g_config_system->grenades_color[3]);
		g_config_system->push_item(&g_config_system->inaccuracy_overlay, "other esp", "inaccuracy_overlay", g_config_system->inaccuracy_overlay);
		g_config_system->push_item(&g_config_system->inaccuracy_overlay_color[0], "other esp", "inaccuracy_overlay_color_r", g_config_system->inaccuracy_overlay_color[0]);
		g_config_system->push_item(&g_config_system->inaccuracy_overlay_color[1], "other esp", "inaccuracy_overlay_color_g", g_config_system->inaccuracy_overlay_color[1]);
		g_config_system->push_item(&g_config_system->inaccuracy_overlay_color[2], "other esp", "inaccuracy_overlay_color_b", g_config_system->inaccuracy_overlay_color[2]);
		g_config_system->push_item(&g_config_system->inaccuracy_overlay_color[3], "other esp", "inaccuracy_overlay_color_a", g_config_system->inaccuracy_overlay_color[3]);
		g_config_system->push_item(&g_config_system->recoil_overlay, "other esp", "recoil_overlay", g_config_system->recoil_overlay);
		g_config_system->push_item(&g_config_system->crosshair, "other esp", "crosshair", g_config_system->crosshair);
		g_config_system->push_item(&g_config_system->bomb, "other esp", "bomb", g_config_system->bomb);
		g_config_system->push_item(&g_config_system->bomb_color[0], "other esp", "bomb_color_r", g_config_system->bomb_color[0]);
		g_config_system->push_item(&g_config_system->bomb_color[1], "other esp", "bomb_color_g", g_config_system->bomb_color[1]);
		g_config_system->push_item(&g_config_system->bomb_color[2], "other esp", "bomb_color_b", g_config_system->bomb_color[2]);
		g_config_system->push_item(&g_config_system->bomb_color[3], "other esp", "bomb_color_a", g_config_system->bomb_color[3]);
		g_config_system->push_item(&g_config_system->grenade_trajectory, "other esp", "grenade_trajectory", g_config_system->grenade_trajectory);
		g_config_system->push_item(&g_config_system->grenade_trajectory_color[0], "other esp", "grenade_trajectory_color_r", g_config_system->grenade_trajectory_color[0]);
		g_config_system->push_item(&g_config_system->grenade_trajectory_color[1], "other esp", "grenade_trajectory_color_g", g_config_system->grenade_trajectory_color[1]);
		g_config_system->push_item(&g_config_system->grenade_trajectory_color[2], "other esp", "grenade_trajectory_color_b", g_config_system->grenade_trajectory_color[2]);
		g_config_system->push_item(&g_config_system->grenade_trajectory_color[3], "other esp", "grenade_trajectory_color_a", g_config_system->grenade_trajectory_color[3]);
		g_config_system->push_item(&g_config_system->grenade_proximity_warning, "other esp", "grenade_proximity_warning", g_config_system->grenade_proximity_warning);
		g_config_system->push_item(&g_config_system->spectators, "other esp", "spectators", g_config_system->spectators);
		g_config_system->push_item(&g_config_system->penetration_reticle, "other esp", "penetration_reticle", g_config_system->penetration_reticle);
		g_config_system->push_item(&g_config_system->feature_indicators[0], "other esp", "feature_indicators_0", g_config_system->feature_indicators[0]);
		g_config_system->push_item(&g_config_system->feature_indicators[1], "other esp", "feature_indicators_1", g_config_system->feature_indicators[1]);
		g_config_system->push_item(&g_config_system->feature_indicators[2], "other esp", "feature_indicators_2", g_config_system->feature_indicators[2]);
		g_config_system->push_item(&g_config_system->feature_indicators[3], "other esp", "feature_indicators_3", g_config_system->feature_indicators[3]);
		g_config_system->push_item(&g_config_system->feature_indicators[4], "other esp", "feature_indicators_4", g_config_system->feature_indicators[4]);
		g_config_system->push_item(&g_config_system->feature_indicators[5], "other esp", "feature_indicators_5", g_config_system->feature_indicators[5]);
		g_config_system->push_item(&g_config_system->feature_indicators[6], "other esp", "feature_indicators_6", g_config_system->feature_indicators[6]);
		g_config_system->push_item(&g_config_system->feature_indicators[7], "other esp", "feature_indicators_7", g_config_system->feature_indicators[7]);

		// effects.
		g_config_system->push_item(&g_config_system->remove_flashbang_effects, "effects", "remove_flashbang_effects", g_config_system->remove_flashbang_effects);
		g_config_system->push_item(&g_config_system->remove_smoke_grenades, "effects", "remove_smoke_grenades", g_config_system->remove_smoke_grenades);
		g_config_system->push_item(&g_config_system->remove_fog, "effects", "remove_fog", g_config_system->remove_fog);
		g_config_system->push_item(&g_config_system->remove_skybox, "effects", "remove_skybox", g_config_system->remove_skybox);
		g_config_system->push_item(&g_config_system->visual_recoil_adjustment, "effects", "visual_recoil_adjustment", g_config_system->visual_recoil_adjustment);
		g_config_system->push_item(&g_config_system->world_color[0], "effects", "world_color_r", g_config_system->world_color[0]);
		g_config_system->push_item(&g_config_system->world_color[1], "effects", "world_color_g", g_config_system->world_color[1]);
		g_config_system->push_item(&g_config_system->world_color[2], "effects", "world_color_b", g_config_system->world_color[2]);
		g_config_system->push_item(&g_config_system->world_color[3], "effects", "world_color_a", g_config_system->world_color[3]);
		g_config_system->push_item(&g_config_system->props_color[0], "effects", "props_color_r", g_config_system->props_color[0]);
		g_config_system->push_item(&g_config_system->props_color[1], "effects", "props_color_g", g_config_system->props_color[1]);
		g_config_system->push_item(&g_config_system->props_color[2], "effects", "props_color_b", g_config_system->props_color[2]);
		g_config_system->push_item(&g_config_system->props_color[3], "effects", "props_color_a", g_config_system->props_color[3]);
		g_config_system->push_item(&g_config_system->skybox_color[0], "effects", "skybox_color_r", g_config_system->skybox_color[0]);
		g_config_system->push_item(&g_config_system->skybox_color[1], "effects", "skybox_color_g", g_config_system->skybox_color[1]);
		g_config_system->push_item(&g_config_system->skybox_color[2], "effects", "skybox_color_b", g_config_system->skybox_color[2]);
		g_config_system->push_item(&g_config_system->skybox_color[3], "effects", "skybox_color_a", g_config_system->skybox_color[3]);
		g_config_system->push_item(&g_config_system->full_bright, "effects", "full_bright", g_config_system->full_bright);
		g_config_system->push_item(&g_config_system->remove_scope_overlay, "effects", "remove_scope_overlay", g_config_system->remove_scope_overlay);
		g_config_system->push_item(&g_config_system->disable_post_processing, "effects", "disable_post_processing", g_config_system->disable_post_processing);
		g_config_system->push_item(&g_config_system->force_third_person_alive, "effects", "force_third_person_alive", g_config_system->force_third_person_alive);
		g_config_system->push_item(&g_config_system->third_person_key, "effects", "third_person_key", g_config_system->third_person_key);
		g_config_system->push_item(&g_config_system->third_person_key_style, "effects", "third_person_key_style", g_config_system->third_person_key_style);
		g_config_system->push_item(&g_config_system->third_person_distance, "effects", "third_person_distance", g_config_system->third_person_distance);
		g_config_system->push_item(&g_config_system->force_third_person_dead, "effects", "force_third_person_dead", g_config_system->force_third_person_dead);
		g_config_system->push_item(&g_config_system->disable_rendering_of_teammates, "effects", "disable_rendering_of_teammates", g_config_system->disable_rendering_of_teammates);
		g_config_system->push_item(&g_config_system->disable_rendering_of_ragdolls, "effects", "disable_rendering_of_ragdolls", g_config_system->disable_rendering_of_ragdolls);
		g_config_system->push_item(&g_config_system->bullet_tracers, "effects", "bullet_tracers", g_config_system->bullet_tracers);
		g_config_system->push_item(&g_config_system->bullet_tracers_color[0], "effects", "bullet_tracers_color_r", g_config_system->bullet_tracers_color[0]);
		g_config_system->push_item(&g_config_system->bullet_tracers_color[1], "effects", "bullet_tracers_color_g", g_config_system->bullet_tracers_color[1]);
		g_config_system->push_item(&g_config_system->bullet_tracers_color[2], "effects", "bullet_tracers_color_b", g_config_system->bullet_tracers_color[2]);
		g_config_system->push_item(&g_config_system->bullet_tracers_color[3], "effects", "bullet_tracers_color_a", g_config_system->bullet_tracers_color[3]);
		g_config_system->push_item(&g_config_system->bullet_impacts, "effects", "bullet_impacts", g_config_system->bullet_impacts);

		// settings.
		g_config_system->push_item(&g_config_system->menu_key, "settings", "menu_key", g_config_system->menu_key);
		g_config_system->push_item(&g_config_system->menu_color[0], "settings", "menu_color_r", g_config_system->menu_color[0]);
		g_config_system->push_item(&g_config_system->menu_color[1], "settings", "menu_color_g", g_config_system->menu_color[1]);
		g_config_system->push_item(&g_config_system->menu_color[2], "settings", "menu_color_b", g_config_system->menu_color[2]);
		g_config_system->push_item(&g_config_system->menu_color[3], "settings", "menu_color_a", g_config_system->menu_color[3]);
		g_config_system->push_item(&g_config_system->menu_watermark, "settings", "menu_watermark", g_config_system->menu_watermark);

		// movement.
		g_config_system->push_item(&g_config_system->bunny_hop, "movement", "bunny_hop", g_config_system->bunny_hop);
		g_config_system->push_item(&g_config_system->air_strafe, "movement", "air_strafe", g_config_system->air_strafe);
		g_config_system->push_item(&g_config_system->slide_walk, "movement", "slide_walk", g_config_system->slide_walk);
		g_config_system->push_item(&g_config_system->slow_motion_key, "movement", "slow_motion_key", g_config_system->slow_motion_key);
		g_config_system->push_item(&g_config_system->slow_motion_key_style, "movement", "slow_motion_key_style", g_config_system->slow_motion_key_style);
		g_config_system->push_item(&g_config_system->speed_limit, "movement", "speed_limit", g_config_system->speed_limit);
		g_config_system->push_item(&g_config_system->peek_assistence_key, "movement", "peek_assistence_key", g_config_system->peek_assistence_key);
		g_config_system->push_item(&g_config_system->peek_assistence_key_style, "movement", "peek_assistence_key_style", g_config_system->peek_assistence_key_style);
		g_config_system->push_item(&g_config_system->peek_assistence_color[0], "movement", "peek_assistence_color_r", g_config_system->peek_assistence_color[0]);
		g_config_system->push_item(&g_config_system->peek_assistence_color[1], "movement", "peek_assistence_color_g", g_config_system->peek_assistence_color[1]);
		g_config_system->push_item(&g_config_system->peek_assistence_color[2], "movement", "peek_assistence_color_b", g_config_system->peek_assistence_color[2]);
		g_config_system->push_item(&g_config_system->peek_assistence_color[3], "movement", "peek_assistence_color_a", g_config_system->peek_assistence_color[3]);

		// others.
		g_config_system->push_item(&g_config_system->override_fov, "others", "override_fov", g_config_system->override_fov);
		g_config_system->push_item(&g_config_system->override_fov_amount, "others", "override_fov_amount", g_config_system->override_fov_amount);
		g_config_system->push_item(&g_config_system->override_fov_when_scoped, "others", "override_fov_when_scoped", g_config_system->override_fov_when_scoped);
		g_config_system->push_item(&g_config_system->override_viewmodel_fov, "others", "override_viewmodel_fov", g_config_system->override_viewmodel_fov);
		g_config_system->push_item(&g_config_system->override_viewmodel_fov_amount, "others", "override_viewmodel_fov_amount", g_config_system->override_viewmodel_fov_amount);
		g_config_system->push_item(&g_config_system->aspect_ratio, "others", "aspect_ratio", g_config_system->aspect_ratio);
		g_config_system->push_item(&g_config_system->width, "others", "width", g_config_system->width);
		g_config_system->push_item(&g_config_system->height, "others", "height", g_config_system->height);
		g_config_system->push_item(&g_config_system->clan_tag_spammer, "others", "clan_tag_spammer", g_config_system->clan_tag_spammer);
		g_config_system->push_item(&g_config_system->chat_spammer, "others", "chat_spammer", g_config_system->chat_spammer);
		g_config_system->push_item(&g_config_system->name_spammer, "others", "name_spammer", g_config_system->name_spammer);
		g_config_system->push_item(&g_config_system->share_esp, "others", "share_esp", g_config_system->share_esp);
		g_config_system->push_item(&g_config_system->log_weapon_purchases, "others", "log_weapon_purchases", g_config_system->log_weapon_purchases);
		g_config_system->push_item(&g_config_system->log_damage_dealt, "others", "log_damage_dealt", g_config_system->log_damage_dealt);
		g_config_system->push_item(&g_config_system->persistent_kill_feed, "others", "persistent_kill_feed", g_config_system->persistent_kill_feed);
		g_config_system->push_item(&g_config_system->primary_weapons[0], "others", "primary_weapons_0", g_config_system->primary_weapons[0]);
		g_config_system->push_item(&g_config_system->primary_weapons[1], "others", "primary_weapons_1", g_config_system->primary_weapons[1]);
		g_config_system->push_item(&g_config_system->primary_weapons[2], "others", "primary_weapons_2", g_config_system->primary_weapons[2]);
		g_config_system->push_item(&g_config_system->primary_weapons[3], "others", "primary_weapons_3", g_config_system->primary_weapons[3]);
		g_config_system->push_item(&g_config_system->primary_weapons[4], "others", "primary_weapons_4", g_config_system->primary_weapons[4]);
		g_config_system->push_item(&g_config_system->primary_weapons[5], "others", "primary_weapons_5", g_config_system->primary_weapons[5]);
		g_config_system->push_item(&g_config_system->primary_weapons[6], "others", "primary_weapons_6", g_config_system->primary_weapons[6]);
		g_config_system->push_item(&g_config_system->primary_weapons[7], "others", "primary_weapons_7", g_config_system->primary_weapons[7]);
		g_config_system->push_item(&g_config_system->primary_weapons[8], "others", "primary_weapons_8", g_config_system->primary_weapons[8]);
		g_config_system->push_item(&g_config_system->primary_weapons[9], "others", "primary_weapons_9", g_config_system->primary_weapons[9]);
		g_config_system->push_item(&g_config_system->primary_weapons[10], "others", "primary_weapons_10", g_config_system->primary_weapons[10]);
		g_config_system->push_item(&g_config_system->primary_weapons[11], "others", "primary_weapons_11", g_config_system->primary_weapons[11]);
		g_config_system->push_item(&g_config_system->primary_weapons[12], "others", "primary_weapons_12", g_config_system->primary_weapons[12]);
		g_config_system->push_item(&g_config_system->primary_weapons[13], "others", "primary_weapons_13", g_config_system->primary_weapons[13]);
		g_config_system->push_item(&g_config_system->primary_weapons[14], "others", "primary_weapons_14", g_config_system->primary_weapons[14]);
		g_config_system->push_item(&g_config_system->primary_weapons[15], "others", "primary_weapons_15", g_config_system->primary_weapons[15]);
		g_config_system->push_item(&g_config_system->secondary_weapons[0], "others", "secondary_weapons_0", g_config_system->secondary_weapons[0]);
		g_config_system->push_item(&g_config_system->secondary_weapons[1], "others", "secondary_weapons_1", g_config_system->secondary_weapons[1]);
		g_config_system->push_item(&g_config_system->secondary_weapons[2], "others", "secondary_weapons_2", g_config_system->secondary_weapons[2]);
		g_config_system->push_item(&g_config_system->secondary_weapons[3], "others", "secondary_weapons_3", g_config_system->secondary_weapons[3]);
		g_config_system->push_item(&g_config_system->equipment[0], "others", "equipment_0", g_config_system->equipment[0]);
		g_config_system->push_item(&g_config_system->equipment[1], "others", "equipment_1", g_config_system->equipment[1]);
		g_config_system->push_item(&g_config_system->equipment[2], "others", "equipment_2", g_config_system->equipment[2]);
		g_config_system->push_item(&g_config_system->equipment[3], "others", "equipment_3", g_config_system->equipment[3]);
		g_config_system->push_item(&g_config_system->equipment[4], "others", "equipment_4", g_config_system->equipment[4]);
		g_config_system->push_item(&g_config_system->equipment[5], "others", "equipment_5", g_config_system->equipment[5]);
		g_config_system->push_item(&g_config_system->equipment[6], "others", "equipment_6", g_config_system->equipment[6]);
		g_config_system->push_item(&g_config_system->equipment[7], "others", "equipment_7", g_config_system->equipment[7]);
		g_config_system->push_item(&g_config_system->equipment[8], "others", "equipment_8", g_config_system->equipment[8]);
		g_config_system->push_item(&g_config_system->equipment[9], "others", "equipment_9", g_config_system->equipment[9]);

		// extra.
		g_config_system->push_item(&g_config_system->infinite_duck, "extra", "infinite_duck", g_config_system->infinite_duck);
		g_config_system->push_item(&g_config_system->fake_duck_key, "extra", "fake_duck_key", g_config_system->fake_duck_key);
		g_config_system->push_item(&g_config_system->fake_duck_key_style, "extra", "fake_duck_key_style", g_config_system->fake_duck_key_style);
		g_config_system->push_item(&g_config_system->radio_key, "extra", "radio_key", g_config_system->radio_key);
		g_config_system->push_item(&g_config_system->radio_key_style, "extra", "radio_key_style", g_config_system->radio_key_style);
		g_config_system->push_item(&g_config_system->volume, "extra", "volume", g_config_system->volume);
		g_config_system->push_item(&g_config_system->channel, "extra", "channel", g_config_system->channel);
	}

	inline bool file_exists(const std::string& name) {
		struct stat buffer;
		return (stat(name.c_str(), &buffer) == 0);
	}

	void c_config_system::push_item(bool* pointer, std::string category, std::string name, bool def_value) {
		this->booleans.push_back({ pointer, category, name, def_value });
	}

	void c_config_system::push_item(int* pointer, std::string category, std::string name, int def_value) {
		this->ints.push_back({ pointer, category, name, def_value });
	}

	void c_config_system::push_item(float* pointer, std::string category, std::string name, float def_value) {
		this->floats.push_back({ pointer, category, name, def_value });
	}

	void c_config_system::push_item(util::c_color* pointer, std::string category, std::string name, util::c_color def_value) {
		this->colors.push_back({ pointer, category, name, def_value });
	}

	using nlohmann::json;
	void c_config_system::read(std::string path) {
		path = "c:\\supremacy\\configs\\" + path + ".cfg";

		if (!file_exists(path))
			return;

		std::ifstream reader(path);

		json data;
		reader >> data;
		reader.close();

		auto sanity_checks = [](json& d, std::string& c, std::string& n) -> bool {
			return d.find(c) != d.end() && d[c].find(n) != d[c].end();
		};

		for (auto& b : booleans)
			if (sanity_checks(data, b.category, b.name))
				*b.pointer = data[b.category][b.name].get<bool>();

		for (auto& i : ints)
			if (sanity_checks(data, i.category, i.name))
				*i.pointer = data[i.category][i.name].get<int>();

		for (auto& f : floats)
			if (sanity_checks(data, f.category, f.name))
				*f.pointer = data[f.category][f.name].get<float>();

		for (auto& c : colors) {
			if (sanity_checks(data, c.category, c.name)) {
				auto elem = data[c.category][c.name];
				c.pointer->r = elem["r"].get<int>();
				c.pointer->g = elem["g"].get<int>();
				c.pointer->b = elem["b"].get<int>();
				c.pointer->a = elem["a"].get<int>();
			}
		}

		for (auto& i : keybinds)
			if (sanity_checks(data, i.category, i.name))
				*i.pointer = data[i.category][i.name].get<int>();
	}

	void c_config_system::save(std::string path) {
		path = "c:\\supremacy\\configs\\" + path + ".cfg";

		json ret_data;
		auto sanity_checks = [&](std::string category) -> void {
			if (ret_data.find(category) == ret_data.end())
				ret_data[category] = json();
		};

		for (auto& b : booleans) {
			sanity_checks(b.category);
			ret_data[b.category][b.name] = *b.pointer;
		}

		for (auto& i : ints) {
			sanity_checks(i.category);
			ret_data[i.category][i.name] = *i.pointer;
		}

		for (auto& f : floats) {
			sanity_checks(f.category);
			ret_data[f.category][f.name] = *f.pointer;
		}

		for (auto& c : colors) {
			sanity_checks(c.category);
			json d;
			util::c_color clr = *c.pointer;
			d["r"] = clr.r;
			d["g"] = clr.g;
			d["b"] = clr.b;
			d["a"] = clr.a;
			ret_data[c.category][c.name] = d;
		}

		for (auto& i : keybinds) {
			sanity_checks(i.category);
			ret_data[i.category][i.name] = *i.pointer;
		}

		std::ofstream reader(path);
		reader.clear();
		reader << std::setw(4) << ret_data << std::endl;
		reader.close();
	}

	void c_config_system::refresh() {
		config_files.clear();

		for (const auto& p : std::filesystem::recursive_directory_iterator("c:\\supremacy\\configs\\")) {
			if (!std::filesystem::is_directory(p) &&
				p.path().extension().string() == ".cfg") {
				auto file = p.path().filename().string();
				config_files.push_back(file.substr(0, file.size() - 4));
			}
		}
	}

	void c_config_system::remove(std::string path) {
		path = "c:\\supremacy\\configs\\" + path + ".cfg";

		std::filesystem::remove(path);
		refresh();
	}

	void c_config_system::reset() {
		for (auto& b : booleans)
			*b.pointer = b.default_value;
		for (auto& i : ints)
			*i.pointer = i.default_value;
		for (auto& c : colors)
			*c.pointer = c.default_value;
		for (auto& f : floats)
			*f.pointer = f.default_value;
		for (auto& i : keybinds)
			*i.pointer = i.default_value;

	}

	static std::string selected_cfg = "";
	bool confirm_opened = false;
	bool confirmed = false;
	bool delete_cfg = false;

	void confirm_save(bool deleting)
	{
		confirm_opened = true;
		delete_cfg = deleting;
	}

	void final_save()
	{
		if (delete_cfg)
		{
			g_config_system->remove(selected_cfg);

			std::stringstream ss;
			ss << "deleted config '" << selected_cfg << "'\n";

			util::g_notify->print_logo();
			util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("%s"), ss.str().c_str());
		}
		else
		{
			g_config_system->save(selected_cfg);

			std::stringstream ss;
			ss << "saved config '" << selected_cfg << "'\n";
			
			util::g_notify->print_logo();
			util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("%s"), ss.str().c_str());
		}


		confirm_opened = false;
		confirmed = true;
	}

	void no_confirm()
	{
		confirm_opened = false;
		confirmed = false;

		util::g_notify->print_logo();
		delete_cfg ? util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("deleting cancelled\n")) : util::g_notify->print_notify(false, true, 0xffc0c0c0u, xorstr_("saving cancelled\n"));
	}

	void window()
	{
		if (!confirm_opened && settings::m_stored_alpha > 0.f) {
			float fc = 255.f / 0.2f * ui::GetIO().DeltaTime;
			settings::m_stored_alpha = std::clamp(settings::m_stored_alpha - fc / 255.f, 0.f, 1.f);
		}

		if (confirm_opened && settings::m_stored_alpha < 1.f) {
			float fc = 255.f / 0.2f * ui::GetIO().DeltaTime;
			settings::m_stored_alpha = std::clamp(settings::m_stored_alpha + fc / 255.f, 0.f, 1.f);
		}

		if (!confirm_opened && settings::m_stored_alpha == 0.f)
			return;

		ui::Begin("confirmation", 0, ImVec2(0, 0), 1.f, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_Confirmation);
		ui::SetWindowSize(ImVec2(240, 145));
		ui::SetCursorPosY(39);

		if (confirm_opened)
			delete_cfg ? ui::Text("are you sure want to delete this config?") : ui::Text("are you sure want to save this config?");

		ui::SetCursorPosX(25 - 21);
		ui::SetCursorPosY(76 - 15);

		if (confirm_opened && ui::Button(delete_cfg ? "delete" : "save", ImVec2(ui::GetWindowSize().x - 58 + 6, 0)))
			final_save();

		ui::SetCursorPosY(116 - 30);
		ui::SetCursorPosX(25 - 21);

		if (confirm_opened && ui::Button("cancel", ImVec2(ui::GetWindowSize().x - 58 + 6, 0)))
			no_confirm();

		ui::End();
	}

	void c_config_system::menu()
	{
		refresh();

		static char cfg_name[26];

		static auto posDouble = [](int num) -> ImVec2 {
			if (num == 1)
				return ImVec2(ui::GetWindowPos().x - 4 + 26 + (ui::GetWindowSize().x / 2 - 36) * num + 20 * num, ui::GetWindowPos().y + 52 + 27);
			else
				return ImVec2(ui::GetWindowPos().x + 26 + (ui::GetWindowSize().x / 2 - 36) * num + 20 * num, ui::GetWindowPos().y + 52 + 27);
		};

		ui::SetNextWindowPos(posDouble(0));
		ui::BeginChild("presets", ImVec2(ui::GetWindowSize().x / 2 - 36 + 4, ui::GetWindowSize().y - 80 - 27)); {

			ui::ListBoxHeader("##cfglist", ImVec2(-1, -1 - 50 + 31 - 145 - 20 - 20));

			for (auto cfg : config_files)
				if (ui::Selectable(cfg.c_str(), cfg == selected_cfg))
					selected_cfg = cfg;

			ui::ListBoxFooter();
			ui::SetCursorPosY(ui::GetWindowSize().y - 153 - 20 - 20);
			if (ui::InputText("", cfg_name, 26)) selected_cfg = std::string(cfg_name);
			if (std::string(cfg_name) != "")
			{
				ui::SetCursorPosX(4);
				if (ui::Button("create", ImVec2(ui::GetWindowSize().x - 58 + 6, 0))) {

					if (selected_cfg.empty()) {
						util::g_notify->print_logo();
						util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("failed to create config\n"));
					}
					else
					{
						save(selected_cfg);

						std::stringstream ss;
						ss << "created & saved config '" << selected_cfg << "'\n";
						util::g_notify->print_logo();
						util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("%s"), ss.str().c_str());
					}

					refresh();
				}
			}
			ui::SetCursorPosX(4);
			if (ui::Button("load", ImVec2(ui::GetWindowSize().x - 58 + 6, 0))) {
				if (selected_cfg.empty()) {
					util::g_notify->print_logo();
					util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("failed to load config\n"));
				}
				else {
					read(selected_cfg);
					settings::m_menu_color[0] = g_config_system->menu_color[0];
					settings::m_menu_color[1] = g_config_system->menu_color[1];
					settings::m_menu_color[2] = g_config_system->menu_color[2];
					settings::m_menu_color[3] = g_config_system->menu_color[3];

					std::stringstream ss;
					ss << "loaded config '" << selected_cfg << "'\n";

					util::g_notify->print_logo();
					util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("%s"), ss.str().c_str());
				}

				refresh();
			}
			ui::SetCursorPosX(4);
			if (ui::Button("save", ImVec2(ui::GetWindowSize().x - 58 + 6, 0))) {
				if (selected_cfg.empty()) {
					util::g_notify->print_logo();
					util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("failed to save config\n"));
				}
				else				
					confirm_save(false);
				

				refresh();
			}

			window();

			ui::SetCursorPosX(4);
			if (ui::Button("delete", ImVec2(ui::GetWindowSize().x - 58 + 6, 0))) {
				if (selected_cfg.empty()) {
					util::g_notify->print_logo();
					util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("failed to remove config\n"));
				}
				else				
					confirm_save(true);
				
				refresh();
			}
			ui::SetCursorPosX(4);
			if (ui::Button("reset", ImVec2(ui::GetWindowSize().x - 58 + 6, 0))) {
				reset();
				util::g_notify->print_logo();
				util::g_notify->print_notify(true, true, 0xffc0c0c0u, xorstr_("successfully resetted menu state\n"));
			}
			ui::SetCursorPosX(4);
			if (ui::Button("open folder", ImVec2(ui::GetWindowSize().x - 58 + 6, 0)))
				ShellExecute(0, "open", "c:/supremacy/configs", NULL, NULL, SW_NORMAL);			
		};
		ui::EndChild();
	}
}