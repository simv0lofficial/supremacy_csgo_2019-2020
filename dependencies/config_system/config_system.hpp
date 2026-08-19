#pragma once

namespace sdk {
	template <typename T>
	struct item_t {
		T* pointer;
		std::string category;
		std::string name;
		T default_value;
	};

	class c_config_system {
	private:
		std::vector<item_t<bool>> booleans;
		std::vector<item_t<int>> ints;
		std::vector<item_t<float>> floats;
		std::vector<item_t<util::c_color>> colors;
		std::vector<item_t<int>> keybinds;
	public:
		void push_item(bool* pointer, std::string category, std::string name, bool def_value);
		void push_item(int* pointer, std::string category, std::string name, int def_value);
		void push_item(float* pointer, std::string category, std::string name, float def_value);
		void push_item(util::c_color* pointer, std::string category, std::string name, util::c_color def_value);

		void setup_values();
		void read(std::string path);
		void save(std::string path);
		void remove(std::string path);
		void refresh();
		void reset();
		void menu();
		std::vector<std::string> config_files;

		// aimbot.		
		bool enabled;
		int enabled_key;
		int enabled_key_style;
		int weapon_selection;
		std::unordered_map<int, bool> target_hitbox;
		std::unordered_map<int, bool> multi_point;
		bool static_scale;
		int head_scale;
		int body_scale;
		bool automatic_fire;
		bool silent_aim;
		int minimum_hitchance = 50;
		int minimum_damage = 40;
		bool automatic_penetration;
		int penetration_minimum_damage = 30;
		int override_minimum_damage_key;
		int override_minimum_damage_key_style = 1;
		int override_minimum_damage = 10;
		bool automatic_scope;
		int maximum_fov = 180;
		bool log_misses_due_to_spread;

		std::unordered_map<int, bool> pistols_target_hitbox;
		std::unordered_map<int, bool> pistols_multi_point;
		bool pistols_static_scale;
		int pistols_head_scale;
		int pistols_body_scale;
		bool pistols_automatic_fire;
		bool pistols_silent_aim;
		int pistols_minimum_hitchance = 50;
		int pistols_minimum_damage = 40;
		bool pistols_automatic_penetration;
		int pistols_penetration_minimum_damage = 30;
		int pistols_override_minimum_damage = 10;
		bool pistols_automatic_scope;
		int pistols_maximum_fov = 180;

		std::unordered_map<int, bool> heavy_pistols_target_hitbox;
		std::unordered_map<int, bool> heavy_pistols_multi_point;
		bool heavy_pistols_static_scale;
		int heavy_pistols_head_scale;
		int heavy_pistols_body_scale;
		bool heavy_pistols_automatic_fire;
		bool heavy_pistols_silent_aim;
		int heavy_pistols_minimum_hitchance = 50;
		int heavy_pistols_minimum_damage = 40;
		bool heavy_pistols_automatic_penetration;
		int heavy_pistols_penetration_minimum_damage = 30;
		int heavy_pistols_override_minimum_damage = 10;
		bool heavy_pistols_automatic_scope;
		int heavy_pistols_maximum_fov = 180;

		std::unordered_map<int, bool> sub_machine_guns_target_hitbox;
		std::unordered_map<int, bool> sub_machine_guns_multi_point;
		bool sub_machine_guns_static_scale;
		int sub_machine_guns_head_scale;
		int sub_machine_guns_body_scale;
		bool sub_machine_guns_automatic_fire;
		bool sub_machine_guns_silent_aim;
		int sub_machine_guns_minimum_hitchance = 50;
		int sub_machine_guns_minimum_damage = 40;
		bool sub_machine_guns_automatic_penetration;
		int sub_machine_guns_penetration_minimum_damage = 30;
		int sub_machine_guns_override_minimum_damage = 10;
		bool sub_machine_guns_automatic_scope;
		int sub_machine_guns_maximum_fov = 180;

		std::unordered_map<int, bool> rifles_target_hitbox;
		std::unordered_map<int, bool> rifles_multi_point;
		bool rifles_static_scale;
		int rifles_head_scale;
		int rifles_body_scale;
		bool rifles_automatic_fire;
		bool rifles_silent_aim;
		int rifles_minimum_hitchance = 50;
		int rifles_minimum_damage = 40;
		bool rifles_automatic_penetration;
		int rifles_penetration_minimum_damage = 30;
		int rifles_override_minimum_damage = 10;
		bool rifles_automatic_scope;
		int rifles_maximum_fov = 180;

		std::unordered_map<int, bool> auto_snipers_target_hitbox;
		std::unordered_map<int, bool> auto_snipers_multi_point;
		bool auto_snipers_static_scale;
		int auto_snipers_head_scale;
		int auto_snipers_body_scale;
		bool auto_snipers_automatic_fire;
		bool auto_snipers_silent_aim;
		int auto_snipers_minimum_hitchance = 50;
		int auto_snipers_minimum_damage = 40;
		bool auto_snipers_automatic_penetration;
		int auto_snipers_penetration_minimum_damage = 30;
		int auto_snipers_override_minimum_damage = 10;
		bool auto_snipers_automatic_scope;
		int auto_snipers_maximum_fov = 180;

		std::unordered_map<int, bool> scout_target_hitbox;
		std::unordered_map<int, bool> scout_multi_point;
		bool scout_static_scale;
		int scout_head_scale;
		int scout_body_scale;
		bool scout_automatic_fire;
		bool scout_silent_aim;
		int scout_minimum_hitchance = 50;
		int scout_minimum_damage = 40;
		bool scout_automatic_penetration;
		int scout_penetration_minimum_damage = 30;
		int scout_override_minimum_damage = 10;
		bool scout_automatic_scope;
		int scout_maximum_fov = 180;

		std::unordered_map<int, bool> awp_target_hitbox;
		std::unordered_map<int, bool> awp_multi_point;
		bool awp_static_scale;
		int awp_head_scale;
		int awp_body_scale;
		bool awp_automatic_fire;
		bool awp_silent_aim;
		int awp_minimum_hitchance = 50;
		int awp_minimum_damage = 40;
		bool awp_automatic_penetration;
		int awp_penetration_minimum_damage = 30;
		int awp_override_minimum_damage = 10;
		bool awp_automatic_scope;
		int awp_maximum_fov = 180;

		std::unordered_map<int, bool> shotguns_target_hitbox;
		std::unordered_map<int, bool> shotguns_multi_point;
		bool shotguns_static_scale;
		int shotguns_head_scale;
		int shotguns_body_scale;
		bool shotguns_automatic_fire;
		bool shotguns_silent_aim;
		int shotguns_minimum_hitchance = 50;
		int shotguns_minimum_damage = 40;
		bool shotguns_automatic_penetration;
		int shotguns_penetration_minimum_damage = 30;
		int shotguns_override_minimum_damage = 10;
		bool shotguns_automatic_scope;
		int shotguns_maximum_fov = 180;

		std::unordered_map<int, bool> machine_guns_target_hitbox;
		std::unordered_map<int, bool> machine_guns_multi_point;
		bool machine_guns_static_scale;
		int machine_guns_head_scale;
		int machine_guns_body_scale;
		bool machine_guns_automatic_fire;
		bool machine_guns_silent_aim;
		int machine_guns_minimum_hitchance = 50;
		int machine_guns_minimum_damage = 40;
		bool machine_guns_automatic_penetration;
		int machine_guns_penetration_minimum_damage = 30;
		int machine_guns_override_minimum_damage = 10;
		bool machine_guns_automatic_scope;
		int machine_guns_maximum_fov = 180;

		std::unordered_map<int, bool> taser_target_hitbox;
		std::unordered_map<int, bool> taser_multi_point;
		bool taser_static_scale;
		int taser_head_scale;
		int taser_body_scale;
		bool taser_automatic_fire;
		bool taser_silent_aim;
		int taser_minimum_hitchance = 50;
		int taser_minimum_damage = 40;
		bool taser_automatic_penetration;
		int taser_penetration_minimum_damage = 30;
		int taser_override_minimum_damage = 10;
		bool taser_automatic_scope;
		int taser_maximum_fov = 180;

		// other.
		bool remove_recoil;
		int accuracy_boost;
		bool quick_stop;
		int type;
		bool between_shots;
		int lag_compensation;
		bool anti_aim_correction;
		std::unordered_map<int, bool> body_aim_conditions;
		int body_aim_key;
		int body_aim_key_style = 1;
		int max_misses_body_aim;
		std::unordered_map<int, bool> safe_point_conditions;
		int safe_point_key;
		int safe_point_key_style = 1;
		int safe_point_type;
		int max_misses_safe_point;
		int double_tap;
		int double_tap_key;
		int double_tap_key_style = 2;
		int max_shift_amount = 6;

		bool pistols_remove_recoil;
		int pistols_accuracy_boost;
		bool pistols_quick_stop;
		int pistols_type;
		bool pistols_between_shots;
		int pistols_lag_compensation;
		std::unordered_map<int, bool> pistols_body_aim_conditions;
		int pistols_max_misses_body_aim;
		std::unordered_map<int, bool> pistols_safe_point_conditions;
		int pistols_safe_point_type;
		int pistols_max_misses_safe_point;
		int pistols_double_tap;
		int pistols_max_shift_amount = 15;

		bool heavy_pistols_remove_recoil;
		int heavy_pistols_accuracy_boost;
		bool heavy_pistols_quick_stop;
		int heavy_pistols_type;
		bool heavy_pistols_between_shots;
		int heavy_pistols_lag_compensation;
		std::unordered_map<int, bool> heavy_pistols_body_aim_conditions;
		int heavy_pistols_max_misses_body_aim;
		std::unordered_map<int, bool> heavy_pistols_safe_point_conditions;
		int heavy_pistols_safe_point_type;
		int heavy_pistols_max_misses_safe_point;
		int heavy_pistols_double_tap;
		int heavy_pistols_max_shift_amount = 15;

		bool sub_machine_guns_remove_recoil;
		int sub_machine_guns_accuracy_boost;
		bool sub_machine_guns_quick_stop;
		int sub_machine_guns_type;
		bool sub_machine_guns_between_shots;
		int sub_machine_guns_lag_compensation;
		std::unordered_map<int, bool> sub_machine_guns_body_aim_conditions;
		int sub_machine_guns_max_misses_body_aim;
		std::unordered_map<int, bool> sub_machine_guns_safe_point_conditions;
		int sub_machine_guns_safe_point_type;
		int sub_machine_guns_max_misses_safe_point;
		int sub_machine_guns_double_tap;
		int sub_machine_guns_max_shift_amount = 15;

		bool rifles_remove_recoil;
		int rifles_accuracy_boost;
		bool rifles_quick_stop;
		int rifles_type;
		bool rifles_between_shots;
		int rifles_lag_compensation;
		std::unordered_map<int, bool> rifles_body_aim_conditions;
		int rifles_max_misses_body_aim;
		std::unordered_map<int, bool> rifles_safe_point_conditions;
		int rifles_safe_point_type;
		int rifles_max_misses_safe_point;
		int rifles_double_tap;
		int rifles_max_shift_amount = 15;

		bool auto_snipers_remove_recoil;
		int auto_snipers_accuracy_boost;
		bool auto_snipers_quick_stop;
		int auto_snipers_type;
		bool auto_snipers_between_shots;
		int auto_snipers_lag_compensation;
		std::unordered_map<int, bool> auto_snipers_body_aim_conditions;
		int auto_snipers_max_misses_body_aim;
		std::unordered_map<int, bool> auto_snipers_safe_point_conditions;
		int auto_snipers_safe_point_type;
		int auto_snipers_max_misses_safe_point;
		int auto_snipers_double_tap;
		int auto_snipers_max_shift_amount = 15;

		bool scout_remove_recoil;
		int scout_accuracy_boost;
		bool scout_quick_stop;
		int scout_type;
		bool scout_between_shots;
		int scout_lag_compensation;
		std::unordered_map<int, bool> scout_body_aim_conditions;
		int scout_max_misses_body_aim;
		std::unordered_map<int, bool> scout_safe_point_conditions;
		int scout_safe_point_type;
		int scout_max_misses_safe_point;
		int scout_double_tap;
		int scout_max_shift_amount = 15;

		bool awp_remove_recoil;
		int awp_accuracy_boost;
		bool awp_quick_stop;
		int awp_type;
		bool awp_between_shots;
		int awp_lag_compensation;
		std::unordered_map<int, bool> awp_body_aim_conditions;
		int awp_max_misses_body_aim;
		std::unordered_map<int, bool> awp_safe_point_conditions;
		int awp_safe_point_type;
		int awp_max_misses_safe_point;
		int awp_double_tap;
		int awp_max_shift_amount = 15;

		bool shotguns_remove_recoil;
		int shotguns_accuracy_boost;
		bool shotguns_quick_stop;
		int shotguns_type;
		bool shotguns_between_shots;
		int shotguns_lag_compensation;
		std::unordered_map<int, bool> shotguns_body_aim_conditions;
		int shotguns_max_misses_body_aim;
		std::unordered_map<int, bool> shotguns_safe_point_conditions;
		int shotguns_safe_point_type;
		int shotguns_max_misses_safe_point;
		int shotguns_double_tap;
		int shotguns_max_shift_amount = 15;

		bool machine_guns_remove_recoil;
		int machine_guns_accuracy_boost;
		bool machine_guns_quick_stop;
		int machine_guns_type;
		bool machine_guns_between_shots;
		int machine_guns_lag_compensation;
		std::unordered_map<int, bool> machine_guns_body_aim_conditions;
		int machine_guns_max_misses_body_aim;
		std::unordered_map<int, bool> machine_guns_safe_point_conditions;
		int machine_guns_safe_point_type;
		int machine_guns_max_misses_safe_point;
		int machine_guns_double_tap;
		int machine_guns_max_shift_amount = 15;

		bool taser_remove_recoil;
		int taser_accuracy_boost;
		bool taser_quick_stop;
		int taser_type;
		bool taser_between_shots;
		int taser_lag_compensation;
		std::unordered_map<int, bool> taser_body_aim_conditions;
		int taser_max_misses_body_aim;
		std::unordered_map<int, bool> taser_safe_point_conditions;
		int taser_safe_point_type;
		int taser_max_misses_safe_point;
		int taser_double_tap;
		int taser_max_shift_amount = 15;

		// anti-aimbot angles.
		bool enabled2;
		int pitch;
		int yaw_base;
		int yaw;
		bool freestanding;
		int freestanding_key;
		int freestanding_key_style;
		int jitter_range;
		int jitter_switch;
		int rotate_range;
		int rotate_speed;
		int body_yaw;
		int body_yaw_key;
		int body_yaw_key_style = 2;
		int yaw_limit = 60;
		int body_lean;
		int inverted_body_lean;
		int on_shot_side;
		int automatic_side_selection;		
		int manual_left_key;
		int manual_right_key;		
		int manual_back_key;

		// other.
		bool fake_lag;		
		int amount;
		std::unordered_map<int, bool> conditions;
		int factor_limit;
		bool hide_shots;
		int hide_shots_key;
		int hide_shots_key_style = 2;

		// player esp.		
		bool teammates;
		bool dormant;
		bool bounding_box;
		float bounding_box_color[4] = { 250 / 255.f, 250 / 255.f, 250 / 255.f, 0xb4 / 255.f };
		bool health_bar;
		bool name;
		float name_color[4] = { 1.f, 1.f, 1.f, 0xb4 / 255.f };
		bool flags;
		bool weapon_text;
		bool weapon_icon;
		float weapon_color[4] = { 1.f, 1.f, 1.f, 0xb4 / 255.f };
		bool ammo;
		float ammo_color[4] = { 80 / 255.f, 140 / 255.f, 200 / 255.f, 0xb4 / 255.f };
		bool distance;
		bool glow;
		float glow_color[4] = { 180 / 255.f, 60 / 255.f, 120 / 255.f, 150 / 255.f };
		bool hit_marker;
		bool hit_marker_sound;
		bool visualize_sounds;
		float visualize_sounds_color[4] = { 250 / 255.f, 63 / 255.f, 63 / 255.f, 150 / 255.f };
		bool money;
		bool skeleton;
		float skeleton_color[4] = { 1.f, 1.f, 170 / 255.f, 1.f };
		bool out_of_fow_arrow;
		float out_of_fow_arrow_color[4] = { 1.f, 30 / 255.f, 30 / 255.f, 190 / 255.f };

		// colored models.		
		bool player;
		float player_color[4] = { 150 / 255.f, 200 / 255.f, 60 / 255.f, 1.f };
		int player_model;
		bool player_behind_wall;
		float player_behind_wall_color[4] = { 60 / 255.f, 120 / 255.f, 180 / 255.f, 1.f };
		int player_behind_wall_model;
		bool teammate;
		float teammate_color[4] = { 1.f, 1.f, 1.f, 1.f };
		int teammate_model;
		bool teammate_behind_wall;
		float teammate_behind_wall_color[4] = { 1.f, 1.f, 1.f, 1.f };
		int teammate_behind_wall_model;
		bool local_player;
		float local_player_color[4] = { 1.f, 1.f, 1.f, 1.f };
		int local_player_model;
		bool local_player_fake;
		float local_player_fake_color[4] = { 0.f, 0.f, 0.f, 0.7f };
		int local_player_fake_model;		
		bool on_shot;
		float on_shot_color[4] = { 1.f, 1.f, 1.f, 1.f };
		int on_shot_model;
		int on_shot_time = 2;
		bool ragdolls;
		bool hands;
		float hands_color[4] = { 1.f, 1.f, 1.f, 1.f };
		int hands_model;
		bool weapon_viewmodel;
		float weapon_viewmodel_color[4] = { 1.f, 1.f, 1.f, 1.f };
		int weapon_viewmodel_model;
		bool weapons;
		float weapons_color[4] = { 1.f, 1.f, 1.f, 1.f };
		int weapons_model;
		bool disable_model_occlusion;
		bool shadow;
		float shadow_color[4] = { 1.f, 1.f, 1.f, 1.f };
		int shadow_model;

		// other esp.
		bool radar;
		int dropped_weapons;
		float dropped_weapons_color[4] = { 1.f, 1.f, 1.f, 0.78f };
		bool grenades;
		float grenades_color[4] = { 1.f, 1.f, 1.f, 1.f };
		bool inaccuracy_overlay;
		float inaccuracy_overlay_color[4] = { 30 / 255.f, 30 / 255.f, 30 / 255.f, 0.47f };
		bool recoil_overlay;
		bool crosshair;
		bool bomb;
		float bomb_color[4] = { 150 / 255.f, 200 / 255.f, 60 / 255.f, 1.f };
		bool grenade_trajectory;
		float grenade_trajectory_color[4] = { 1.f, 50 / 255.f, 50 / 255.f, 0.7f };
		bool grenade_proximity_warning;
		bool spectators;
		bool penetration_reticle;		
		std::unordered_map<int, bool> feature_indicators;
		float manual_anti_aimbot_angles_color[4] = { 1.f, 45 / 255.f, 0.f, 180 / 255.f };

		// effects.
		bool remove_flashbang_effects;
		bool remove_smoke_grenades;
		bool remove_fog;
		bool remove_skybox;		
		int visual_recoil_adjustment;
		float world_color[4] = { 1.f, 1.f, 1.f, 1.f };
		float props_color[4] = { 1.f, 1.f, 1.f, 1.f };
		float skybox_color[4] = { 1.f, 1.f, 1.f, 1.f };
		bool full_bright;
		bool remove_scope_overlay;
		bool disable_post_processing;
		bool force_third_person_alive;
		int third_person_key;
		int third_person_key_style = 2;
		int third_person_distance = 120;
		bool force_third_person_dead;
		bool disable_rendering_of_teammates;
		bool disable_rendering_of_ragdolls;
		bool bullet_tracers;
		float bullet_tracers_color[4] = { 150 / 255.f, 130 / 255.f, 1.f, 1.f };
		bool bullet_impacts;

		// settings.		
		int menu_key = VK_DELETE;
		float menu_color[4] = { 1.f, 45 / 255.f, 0.f, 180 / 255.f };
		bool menu_watermark = true;

		// movement.		
		bool bunny_hop;
		bool air_strafe;
		bool slide_walk;
		int slow_motion_key;
		int slow_motion_key_style = 1;
		int speed_limit;
		int peek_assistence_key;
		int peek_assistence_key_style = 2;
		float peek_assistence_color[4] = { 150 / 255.f, 130 / 255.f, 1.f, 1.f };

		// others.
		bool override_fov;
		int override_fov_amount = 90;
		bool override_fov_when_scoped;
		bool override_viewmodel_fov;
		int override_viewmodel_fov_amount = 90;
		bool aspect_ratio;
		int width = 16;
		int height = 9;
		bool clan_tag_spammer;
		bool chat_spammer;
		bool name_spammer;
		bool share_esp = true;
		bool log_weapon_purchases;
		bool log_damage_dealt;
		int persistent_kill_feed;
		std::unordered_map<int, bool> primary_weapons;
		std::unordered_map<int, bool> secondary_weapons;
		std::unordered_map<int, bool> equipment;
		int auto_buy_minimal_money = 16000;

		// extra.
		bool infinite_duck;
		int fake_duck_key;
		int fake_duck_key_style = 1;
		int radio_key;
		int radio_key_style = 2;
		int volume = 50;
		int channel;
	
	public:
		struct skin_info {
			int seed = -1;
			int paintkit;
			int rarity;
			std::string tag_name;
			std::string short_name;
			std::string name;
		};
	public:
		std::unordered_map<std::string, std::set<std::string>> weapon_skins;
		std::unordered_map<std::string, skin_info> skin_map;
		std::unordered_map<std::string, std::string> skin_names;

	};

	inline const auto g_config_system = std::make_unique< c_config_system >();
}