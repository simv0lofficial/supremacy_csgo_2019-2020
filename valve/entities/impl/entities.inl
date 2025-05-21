#pragma once

#include "../entities.hpp"

namespace supremacy::valve {
	__forceinline std::uintptr_t c_entity::renderable( ) {
		return reinterpret_cast< std::uintptr_t >( this ) + sizeof( std::uintptr_t );
	}

	__forceinline std::uintptr_t c_entity::networkable( ) {
		return reinterpret_cast< std::uintptr_t >( this ) + sizeof( std::uintptr_t ) * 2u;
	}

	__forceinline int c_entity::index( ) {
		using fn_t = int( __thiscall* )( const std::uintptr_t );

		return ( *reinterpret_cast< fn_t** >( networkable( ) ) )[ 10u ]( networkable( ) );
	}

	__forceinline e_ent_handle c_entity::handle( ) {
		using fn_t = e_ent_handle( __thiscall* )( decltype( this ) );

		return ( *reinterpret_cast< fn_t** >( this ) )[ 2u ]( this );
	}

	__forceinline bool c_entity::dormant( ) {
		using fn_t = bool( __thiscall* )( const std::uintptr_t );

		return ( *reinterpret_cast< fn_t** >( networkable( ) ) )[ 9u ]( networkable( ) );
	}

	__forceinline client_class_t* c_entity::client_class( ) {
		using fn_t = client_class_t*( __thiscall* )( const std::uintptr_t );
	
		return ( *reinterpret_cast< fn_t** >( networkable( ) ) )[ 2u ]( networkable( ) );
	}

	__forceinline bool c_entity::setup_bones(
		mat3x4_t* const bones, const int max_bones,
		const int mask, const float time
	) {
		using fn_t = bool( __thiscall* )(
			const std::uintptr_t, mat3x4_t* const,
			const int, const int, const float
		);

		return ( *reinterpret_cast< fn_t** >( renderable( ) ) )[ 13u ](
			renderable( ), bones, max_bones, mask, time
		);
	}

	__forceinline float& c_entity::sim_time( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseEntity->m_flSimulationTime" ) );

		return *reinterpret_cast< float* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline bool c_entity::compute_hitbox(vec3_t* obb_min, vec3_t* obb_max) {
		using fn_t = bool(__thiscall*)(decltype(this), vec3_t*, vec3_t*);

		return reinterpret_cast<fn_t>(
			g_context->addresses().m_compute_hitbox_surround_box
			)(this, obb_min, obb_max);
	}

	__forceinline float& c_entity::old_sim_time( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseEntity->m_flSimulationTime" ) );

		return *reinterpret_cast< float* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var ) + 0x4u
		);
	}

	__forceinline vec3_t& c_entity::origin( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseEntity->m_vecOrigin" ) );

		return *reinterpret_cast< vec3_t* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline qangle_t& c_entity::abs_angles( ) {
		return *reinterpret_cast< qangle_t* >(
			reinterpret_cast< std::uintptr_t >( this ) + 0xc4u
		);
	}

	__forceinline vec3_t& c_entity::abs_origin( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "m_vecAbsOrigin" ) );

		return *reinterpret_cast< vec3_t* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline vec3_t& c_entity::abs_velocity( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "m_vecAbsVelocity" ) );

		return *reinterpret_cast< vec3_t* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline void c_entity::set_abs_angles( const qangle_t& abs_angles ) {
		using fn_t = void( __thiscall* )( decltype( this ), const qangle_t& );

		return reinterpret_cast< fn_t >(
			g_context->addresses( ).m_set_abs_angles
		)( this, abs_angles );
	}

	__forceinline void c_entity::set_abs_origin( const vec3_t& abs_origin ) {
		using fn_t = void( __thiscall* )( decltype( this ), const vec3_t& );

		return reinterpret_cast< fn_t >(
			g_context->addresses( ).m_set_abs_origin
		)( this, abs_origin );
	}

	__forceinline e_ent_flags& c_entity::flags( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBasePlayer->m_fFlags" ) );

		return *reinterpret_cast< e_ent_flags* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline std::uint32_t& c_entity::eflags( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "m_iEFlags" ) );

		return *reinterpret_cast< std::uint32_t* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline model_t* c_entity::model( ) {
		using fn_t = model_t*( __thiscall* )( const std::uintptr_t );

		return ( *reinterpret_cast< fn_t** >( renderable( ) ) )[ 8u ]( renderable( ) );
	}

	__forceinline model_data_t*& c_entity::mdl_data( ) {
		return *reinterpret_cast< model_data_t** >(
			reinterpret_cast< std::uintptr_t >( this ) + 0x294cu
		);
	}

	__forceinline studio_hdr_t* c_entity::studio_hdr() {
		return *reinterpret_cast<studio_hdr_t**>(
			reinterpret_cast<std::uintptr_t>(this) + 0x294cu
			);
	}

	__forceinline studio_hdr_t* c_entity::mdl_ptr() {
		using fn_t = void(__thiscall*)(decltype(this));

		if (!studio_hdr())
			reinterpret_cast<fn_t>(
				g_context->addresses().m_lock_studio_hdr
				)(this);

		return studio_hdr();
	}

	__forceinline std::uint32_t& c_entity::effects( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "m_fEffects" ) );

		return *reinterpret_cast< std::uint32_t* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline std::uint32_t& c_entity::occlusion_flags( ) {
		return *reinterpret_cast< std::uint32_t* >(
			reinterpret_cast< std::uintptr_t >( this ) + 0xa28u
		);
	}

	__forceinline int& c_entity::occlusion_frame( ) {
		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + 0xa30u
		);
	}

	__forceinline ik_context_t*& c_entity::inverse_kinematics() {
		return *reinterpret_cast<ik_context_t**>(
			reinterpret_cast<std::uintptr_t>(this) + 0x2670u
			);
	}

	__forceinline int& c_entity::last_setup_bones_frame( ) {
		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + 0xa68u
		);
	}

	__forceinline float& c_entity::last_bone_setup_time( ) {
		return *reinterpret_cast< float* >(
			reinterpret_cast< std::uintptr_t >( this ) + 0x2924u
		);
	}

	__forceinline unsigned long& c_entity::most_recent_model_bone_counter( ) {
		return *reinterpret_cast< unsigned long* >(
			reinterpret_cast< std::uintptr_t >( this ) + 0x2690u
		);
	}

	__forceinline bone_accessor_t& c_entity::bone_accessor( ) {
		return *reinterpret_cast< bone_accessor_t* >(
			reinterpret_cast< std::uintptr_t >( this ) + 0x26a4u
		);
	}

	__forceinline utl_vec_t< mat3x4_t >& c_entity::bone_cache( ) {
		return *reinterpret_cast< utl_vec_t< mat3x4_t >* >(
			reinterpret_cast< std::uintptr_t >( this ) + 0x2910u
		);
	}

	__forceinline std::uint8_t& c_entity::ent_client_effects( ) {
		return *reinterpret_cast< std::uint8_t* >(
			reinterpret_cast< std::uintptr_t >( this ) + 0x68u
		);
	}

	__forceinline bool c_entity::breakable() {
		using fn_t = bool(__thiscall*)(decltype(this));

		if (reinterpret_cast<fn_t>(g_context->addresses().m_breakable)(this))
			return true;

		const auto cc = client_class();
		if (!cc)
			return false;

		return (*reinterpret_cast<const std::uint32_t*>(cc->m_network_name) == 'erBC'
			&& *reinterpret_cast<const std::uint32_t*>(cc->m_network_name + 7) == 'Selb')
			|| (*reinterpret_cast<const std::uint32_t*>(cc->m_network_name) == 'saBC'
				&& *reinterpret_cast<const std::uint32_t*>(cc->m_network_name + 7) == 'ytit');
	}

	__forceinline int& c_entity::team( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseEntity->m_iTeamNum" ) );

		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline bool c_entity::friendly( ) {
		if (team() != valve::g_local_player->team())
			return false;

		else if (g_context->cvars().m_mp_teammates_are_enemies->get_bool())
			return false;

		return true;
	}

	__forceinline int& c_entity::sim_tick( ) {
		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + 0x2acu
		);
	}

	__forceinline vec3_t& c_entity::obb_min( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseEntity->m_vecMins" ) );

		return *reinterpret_cast< vec3_t* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline vec3_t& c_entity::obb_max( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseEntity->m_vecMaxs" ) );

		return *reinterpret_cast< vec3_t* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline void c_entity::set_collision_bounds(
		const vec3_t& obb_min, const vec3_t& obb_max
	) {
		using collideable_fn_t = std::uintptr_t( __thiscall* )( decltype( this ) );

		const auto collideable = ( *reinterpret_cast< collideable_fn_t** >( this ) )[ 3u ]( this );
		if ( !collideable )
			return;

		using fn_t = void( __thiscall* )( const std::uintptr_t, const vec3_t&, const vec3_t& );

		reinterpret_cast< fn_t >(
			g_context->addresses( ).m_set_collision_bounds
		)( collideable, obb_min, obb_max );
	}

	__forceinline const vec3_t& c_entity::world_space_center( ) {
		using fn_t = const vec3_t&( __thiscall* )( decltype( this ) );

		return ( *reinterpret_cast< fn_t** >( this ) )[ 78u ]( this );
	}

	__forceinline bool c_entity::is_player( ) {
		using fn_t = bool( __thiscall* )( decltype( this ) );

		return ( *reinterpret_cast< fn_t** >( this ) )[ 157u ]( this );
	}

	__forceinline bool c_entity::is_weapon( ) {
		using fn_t = bool( __thiscall* )( decltype( this ) );

		return ( *reinterpret_cast< fn_t** >( this ) )[ 165u ]( this );
	}

	__forceinline var_mapping_t& c_entity::var_mapping( ) {
		return *reinterpret_cast< var_mapping_t* >(
			reinterpret_cast< std::uintptr_t >( this ) + 0x24u
		);
	}

	__forceinline float& c_entity::cycle( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CCSPlayer->m_flCycle" ) );

		return *reinterpret_cast< float* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline float& c_entity::anim_time( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseEntity->m_flAnimTime" ) );

		return *reinterpret_cast< float* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline e_ent_handle& c_entity::move_parent( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseEntity->moveparent" ) );

		return *reinterpret_cast< e_ent_handle* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline bool& c_entity::spotted( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseEntity->m_bSpotted" ) );

		return *reinterpret_cast< bool* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline bool& c_entity::spotted_by(int index) {
		static const auto& net_var = g_net_vars->entry(xorstr_("CBaseEntity->m_bSpotted"));

		return *reinterpret_cast<bool*>(
			reinterpret_cast<std::uintptr_t>(this) + std::get< std::uint32_t >(net_var) + 0x1u + index
			);
	}

	__forceinline e_ent_handle& c_entity::thrower( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseGrenade->m_hThrower" ) );

		return *reinterpret_cast< e_ent_handle* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline int& c_entity::explode_effect_tick_begin( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseCSGrenadeProjectile->m_nExplodeEffectTickBegin" ) );

		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline float& c_entity::grenade_spawn_time( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseCSGrenadeProjectile->m_vecExplodeEffectOrigin" ) );

		return *reinterpret_cast< float* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var ) + 0xcu
		);
	}

	__forceinline bool& c_entity::use_fast_path( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseAnimating->m_flFrozen" ) );

		return *reinterpret_cast< bool* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var ) + 0x4u
		);
	}

	__forceinline int& c_entity::bomb_site( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CPlantedC4->m_nBombSite" ) );

		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline bool& c_entity::is_bomb_ticking( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CPlantedC4->m_bBombTicking" ) );

		return *reinterpret_cast< bool* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline float& c_entity::c4_blow( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CPlantedC4->m_flC4Blow" ) );

		return *reinterpret_cast< float* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline std::uint16_t c_entity::model_instance( ) {
		using fn_t = std::uint16_t( __thiscall* )( const std::uintptr_t );

		return ( *reinterpret_cast< fn_t** >( renderable( ) ) )[ 30u ]( renderable( ) );
	}

	__forceinline int& c_entity::model_index( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseEntity->m_nModelIndex" ) );

		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline e_ent_handle& c_entity::world_model_handle( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseCombatWeapon->m_hWeaponWorldModel" ) );

		return *reinterpret_cast< e_ent_handle* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline int& c_entity::sequence( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseViewModel->m_nSequence" ) );

		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline e_ent_handle& c_entity::view_model_weapon_handle( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseViewModel->m_hWeapon" ) );

		return *reinterpret_cast< e_ent_handle* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline e_ent_handle& c_entity::view_model_owner( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseViewModel->m_hOwner" ) );

		return *reinterpret_cast< e_ent_handle* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline void c_entity::set_model_index( const int index ) {
		using fn_t = void( __thiscall* )( decltype( this ), const int );

		return ( *reinterpret_cast< fn_t** >( this ) )[ 75u ]( this, index );
	}

	__forceinline float c_weapon::max_speed( ) {
		using fn_t = float( __thiscall* )( decltype( this ) );

		return ( *reinterpret_cast< fn_t** >( this ) )[(TWENTYTWENTY ? 441u : 438u) ]( this );
	}

	__forceinline e_item_index& c_weapon::item_index( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseAttributableItem->m_iItemDefinitionIndex" ) );

		return *reinterpret_cast< e_item_index* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline float& c_weapon::postpone_fire_ready_time( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CWeaponCSBase->m_flPostponeFireReadyTime" ) );

		return *reinterpret_cast< float* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var  )
		);
	}

	__forceinline float& c_weapon::accuracy_penalty( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CWeaponCSBase->m_fAccuracyPenalty" ) );

		return *reinterpret_cast< float* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline float& c_weapon::last_shot_time( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CWeaponCSBase->m_fLastShotTime" ) );

		return *reinterpret_cast< float* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline float& c_weapon::recoil_index( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CWeaponCSBase->m_flRecoilIndex" ) );

		return *reinterpret_cast< float* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline int& c_weapon::ammo( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseCombatWeapon->m_iClip1" ) );

		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline float& c_weapon::next_primary_attack( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseCombatWeapon->m_flNextPrimaryAttack" ) );

		return *reinterpret_cast< float* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline float& c_weapon::next_secondary_attack( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseCombatWeapon->m_flNextSecondaryAttack" ) );

		return *reinterpret_cast< float* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline float& c_weapon::throw_time( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseCSGrenade->m_fThrowTime" ) );

		return *reinterpret_cast< float* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline float& c_weapon::throw_strength( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseCSGrenade->m_flThrowStrength" ) );

		return *reinterpret_cast< float* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline bool& c_weapon::pin_pulled( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseCSGrenade->m_bPinPulled" ) );

		return *reinterpret_cast< bool* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline float c_weapon::spread( ) {
		using fn_t = float( __thiscall* )( decltype( this ) );

		return ( *reinterpret_cast< fn_t** >( this ) )[(TWENTYTWENTY ? 452u : 449u) ]( this );
	}

	__forceinline float c_weapon::inaccuracy( ) {
		using fn_t = float( __thiscall* )( decltype( this ) );

		return ( *reinterpret_cast< fn_t** >( this ) )[(TWENTYTWENTY ? 482u:479u )]( this );
	}

	__forceinline void c_weapon::update_inaccuracy( ) {
		using fn_t = void( __thiscall* )( decltype( this ) );

		return ( *reinterpret_cast< fn_t** >( this ) )[(TWENTYTWENTY ? 483u:480u) ]( this );
	}

	__forceinline weapon_data_t* c_weapon::wpn_data( ) {
		using fn_t = weapon_data_t*( __thiscall* )(decltype(this));
		return ( *reinterpret_cast< fn_t** >( this ) )[(TWENTYTWENTY ? 460u:457u) ]( this );
	}

	__forceinline bool& c_weapon::in_reload( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "m_bInReload" ) );

		return *reinterpret_cast< bool* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline int& c_weapon::zoom_lvl( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CWeaponCSBaseGun->m_zoomLevel" ) );

		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline int& c_weapon::item_id_high( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseAttributableItem->m_iItemIDHigh" ) );

		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline int& c_weapon::account_id( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseAttributableItem->m_iAccountID" ) );

		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline int& c_weapon::fallback_paint_kit( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseAttributableItem->m_nFallbackPaintKit" ) );

		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline int& c_weapon::item_quality( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseAttributableItem->m_iEntityQuality" ) );

		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline int& c_weapon::fallback_stat_trak( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseAttributableItem->m_nFallbackStatTrak" ) );

		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline float& c_weapon::fallback_wear( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseAttributableItem->m_flFallbackWear" ) );

		return *reinterpret_cast< float* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline int& c_weapon::owner_xuid_low( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseAttributableItem->m_OriginalOwnerXuidLow" ) );

		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline int& c_weapon::owner_xuid_high( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseAttributableItem->m_OriginalOwnerXuidHigh" ) );

		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline bool& c_weapon::custom_material_initialized( ) {
		return *reinterpret_cast< bool* > (
			reinterpret_cast< std::uintptr_t >( this ) + 0x3370u
		);
	}

	__forceinline utl_vec_t< ret_counted_t* >& c_weapon::custom_materials( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseCombatWeapon->m_Item" ) );

		return *reinterpret_cast< utl_vec_t< ret_counted_t* >* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var ) + 0x14u
		);
	}

	__forceinline utl_vec_t< ret_counted_t* >& c_weapon::custom_materials2( ) {
		return *reinterpret_cast< utl_vec_t< ret_counted_t* >* >(
			reinterpret_cast< std::uintptr_t >( this ) + 0x9dcu
		);
	}

	__forceinline utl_vec_t< ret_counted_t* >& c_weapon::visuals_data_processors( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseCombatWeapon->m_Item" ) );

		return *reinterpret_cast< utl_vec_t< ret_counted_t* >* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var ) + 0x230u
		);
	}

	__forceinline void c_player::set_local_view_angles( const qangle_t& angle ) {
		using fn_t = void( __thiscall* )( decltype( this ), const qangle_t& );

		return ( *reinterpret_cast< fn_t** >( this ) )[ 372u ]( this, angle );
	}

	__forceinline int& c_player::tick_base( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBasePlayer->m_nTickBase" ) );

		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline e_life_state& c_player::life_state( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBasePlayer->m_lifeState" ) );

		return *reinterpret_cast< e_life_state* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline int& c_player::health( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBasePlayer->m_iHealth" ) );

		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline bool c_player::alive( ) {
		return ( health( ) > 0 ) && ( life_state( ) == e_life_state::alive );
	}

	__forceinline pose_params_t& c_player::pose_params( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CCSPlayer->m_flPoseParameter" ) );

		return *reinterpret_cast< pose_params_t* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline float& c_player::spawn_time( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CCSPlayer->m_iAddonBits" ) );

		return *reinterpret_cast< float* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var ) - 0x4u
		);
	}

	__forceinline anim_layers_t& c_player::anim_layers( ) {
		return **reinterpret_cast< anim_layers_t** >(
			reinterpret_cast< std::uintptr_t >( this ) + 0x2980u
		);
	}

	__forceinline anim_state_t*& c_player::anim_state( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CCSPlayer->m_bIsScoped" ) );

		return *reinterpret_cast< anim_state_t** >(
			reinterpret_cast< std::uintptr_t >( this )
			+ std::get< std::uint32_t >( net_var ) - 0x14u
		);
	}

	__forceinline int& c_player::final_predicted_tick() {
		static const auto& net_var = g_net_vars->entry(xorstr_("CBasePlayer->m_nTickBase"));

		return *reinterpret_cast<int*>(
			reinterpret_cast<std::uintptr_t>(this) + std::get< std::uint32_t >(net_var) + 0x4u
		);
	}

	__forceinline bool& c_player::client_side_anim( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CCSPlayer->m_bClientSideAnimation" ) );

		return *reinterpret_cast< bool* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline float c_player::get_sequence_cycle_rate(studio_hdr_t* studio_hdr, int sequence) {
		using fn_t = float(__thiscall*)(decltype(this), studio_hdr_t*, int);

		return (*reinterpret_cast<fn_t**>(this))[221u](this, studio_hdr, sequence);
	}

	__forceinline void c_player::get_sequence_linear_motion(studio_hdr_t* studio_hdr, int sequence, pose_params_t pose_params, vec3_t* out) {
		using fn_t = int(__fastcall*)(studio_hdr_t*, int, float*, vec3_t*);

		reinterpret_cast<fn_t>(
			g_context->addresses().m_get_sequence_linear_motion
			)(studio_hdr, sequence, pose_params.data(), out);

		__asm add esp, 8u
	}

	__forceinline float c_player::get_sequence_move_dist(studio_hdr_t* studio_hdr, int sequence) {
		vec3_t ret;
		get_sequence_linear_motion(studio_hdr, sequence, pose_params(), &ret);
		return ret.length();
	}

	__forceinline float c_player::get_layer_sequence_cycle_rate(anim_layer_t* layer, int sequence) {
		using fn_t = float(__thiscall*)(decltype(this), anim_layer_t*, int);

		return (*reinterpret_cast<fn_t**>(this))[222u](this, layer, sequence);
	}

	__forceinline void c_player::setup_movement() {
		using fn_t = void(__thiscall*)(anim_state_t*);

		return reinterpret_cast<fn_t>(
			g_context->addresses().m_setup_movement
			)(anim_state());
	}

	__forceinline void c_player::update_client_side_anim( ) {
		using fn_t = void( __thiscall* )( decltype( this ) );

		return ( *reinterpret_cast< fn_t** >( this ) )[ 223u ]( this );
	}

	__forceinline qangle_t& c_player::eye_angles( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CCSPlayer->m_angEyeAngles" ) );

		return *reinterpret_cast< qangle_t* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline vec3_t& c_player::velocity( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBasePlayer->m_vecVelocity[0]" ) );

		return *reinterpret_cast< vec3_t* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline float& c_player::duck_amount( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "m_flDuckAmount" ) );

		return *reinterpret_cast< float* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline e_ent_handle& c_player::wpn_handle( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseCombatCharacter->m_hActiveWeapon" ) );

		return *reinterpret_cast< e_ent_handle* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline e_ent_handle& c_player::ground_entity( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "m_hGroundEntity" ) );

		return *reinterpret_cast< e_ent_handle* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline c_weapon* c_player::weapon( ) {
		const auto handle = wpn_handle( );
		if ( handle == e_ent_handle::invalid )
			return nullptr;

		return static_cast< c_weapon* >( g_entity_list->find_entity( handle ) );
	}

	__forceinline int& c_player::hitbox_set_index( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseAnimating->m_nHitboxSet" ) );

		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline int& c_player::skin( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "m_nSkin" ) );

		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline int& c_player::body( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "m_nBody" ) );

		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline float& c_player::lby( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CCSPlayer->m_flLowerBodyYawTarget" ) );

		return *reinterpret_cast< float* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline int c_player::lookup_seq_act( const int sequence ) {
		const auto model_data = mdl_data( );
		if ( !model_data
			|| !model_data->m_studio_hdr )
			return -1;

		using fn_t = int( __fastcall* )( decltype( this ), studio_hdr_t* const, const int );

		return reinterpret_cast< fn_t >(
			g_context->addresses( ).m_lookup_seq_act
		)( this, model_data->m_studio_hdr, sequence );
	}

	__forceinline vec3_t& c_player::view_offset( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBasePlayer->m_vecViewOffset[0]" ) );

		return *reinterpret_cast< vec3_t* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline qangle_t& c_player::aim_punch( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBasePlayer->m_aimPunchAngle" ) );

		return *reinterpret_cast< qangle_t* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline vec3_t& c_player::aim_punch_vel( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBasePlayer->m_aimPunchAngleVel" ) );

		return *reinterpret_cast< vec3_t* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline float& c_player::next_attack( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "m_flNextAttack" ) );

		return *reinterpret_cast< float* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline bool& c_player::ducking( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBasePlayer->m_bDucking" ) );

		return *reinterpret_cast< bool* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline bool& c_player::ducked( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBasePlayer->m_bDucked" ) );

		return *reinterpret_cast< bool* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline bool& c_player::walking( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CCSPlayer->m_bIsWalking" ) );

		return *reinterpret_cast< bool* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline bool& c_player::strafing( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CCSPlayer->m_bStrafing" ) );

		return *reinterpret_cast< bool* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline e_move_type& c_player::move_type( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "m_MoveType" ) );

		return *reinterpret_cast< e_move_type* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline float& c_player::velocity_modifier( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CCSPlayer->m_flVelocityModifier" ) );

		return *reinterpret_cast< float* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline float& c_player::fall_velocity() {
		static const auto& net_var = g_net_vars->entry(xorstr_("CBasePlayer->m_flFallVelocity"));

		return *reinterpret_cast<float*>(
			reinterpret_cast<std::uintptr_t>(this) + std::get< std::uint32_t >(net_var)
			);
	}

	__forceinline user_cmd_t*& c_player::cur_user_cmd( ) {
		return *reinterpret_cast< user_cmd_t** >(
			reinterpret_cast< std::uintptr_t >( this ) + 0x3338u
		);
	}

	__forceinline user_cmd_t& c_player::last_user_cmd( ) {
		return *reinterpret_cast< user_cmd_t* >(
			reinterpret_cast< std::uintptr_t >( this ) + 0x3288u
		);
	}

	__forceinline cmd_context_t& c_player::cmd_context( ) {
		return *reinterpret_cast< cmd_context_t* >(
			reinterpret_cast< std::uintptr_t >( this ) + (TWENTYTWENTY ? 0x34fcu : 0x34f8u)
		);
	}

	__forceinline bool& c_player::scoped( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CCSPlayer->m_bIsScoped" ) );

		return *reinterpret_cast< bool* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline qangle_t& c_player::view_punch( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBasePlayer->m_viewPunchAngle" ) );

		return *reinterpret_cast< qangle_t* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline float& c_player::duck_speed( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBasePlayer->m_flDuckSpeed" ) );

		return *reinterpret_cast< float* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline int& c_player::move_state( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CCSPlayer->m_iMoveState" ) );

		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline float& c_player::third_person_recoil( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CCSPlayer->m_flThirdpersonRecoil" ) );

		return *reinterpret_cast< float* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline int c_player::lookup_bone( const char* name ) {
		using fn_t = int( __thiscall* )( decltype( this ), const char* );

		return reinterpret_cast< fn_t >( g_context->addresses( ).m_lookup_bone )( this, name );
	}

	__forceinline int c_player::lookup_sequence(const char* name) {
		using fn_t = int(__thiscall*)(decltype(this), const char*);

		return reinterpret_cast<fn_t>(g_context->addresses().m_lookup_sequence)(this, name);
	}

	__forceinline int& c_player::armor_value( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CCSPlayer->m_ArmorValue" ) );

		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline bool& c_player::has_helmet( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CCSPlayer->m_bHasHelmet" ) );

		return *reinterpret_cast< bool* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline bool& c_player::has_heavy_armor( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CCSPlayer->m_bHasHeavyArmor" ) );

		return *reinterpret_cast< bool* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline float& c_player::max_speed( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBasePlayer->m_flMaxspeed" ) );

		return *reinterpret_cast< float* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline float& c_player::surface_friction( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "m_surfaceFriction" ) );

		return *reinterpret_cast< float* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline bool& c_player::immune( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CCSPlayer->m_bGunGameImmunity" ) );

		return *reinterpret_cast< bool* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline e_ent_handle c_player::view_model_handle( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBasePlayer->m_hViewModel[0]" ) );

		return *reinterpret_cast< e_ent_handle* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline c_entity* c_player::view_model( ) {
		return g_entity_list->find_entity( view_model_handle( ) );
	}

	__forceinline int& c_player::money( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CCSPlayer->m_iAccount" ) );

		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline float c_player::fov( ) {
		using fn_t = float( __thiscall* )( decltype( this ) );

		return ( *reinterpret_cast< fn_t** >( this ) )[ 331u ]( this );
	}

	__forceinline bool c_player::has_c4( ) {
		using fn_t = bool( __thiscall* )( decltype( this ) );

		return reinterpret_cast< fn_t >( g_context->addresses( ).m_has_c4 )( this );
	}

	__forceinline int& c_player::observer_mode( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBasePlayer->m_iObserverMode" ) );

		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline e_ent_handle& c_player::observer_target( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBasePlayer->m_hObserverTarget" ) );

		return *reinterpret_cast< e_ent_handle* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline bool& c_player::use_new_anim_state( ) {
		return *reinterpret_cast<bool*>(
			reinterpret_cast<std::uintptr_t>(this) + (TWENTYTWENTY ? 0x3ac8u : 0x3ab4u)
			);
	}

	__forceinline void c_player::invalidate_physics_recursive(int bits) {
		using fn_t = void(__thiscall*)(void*, int);

		reinterpret_cast<fn_t>(
			g_context->addresses().m_invalidate_physics_recursive
			)(this, bits);
	}

	__forceinline e_ent_handle* c_player::weapons( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CBaseCombatCharacter->m_hMyWeapons" ) );

		return reinterpret_cast< e_ent_handle* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline float& c_player::flash_duration( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CCSPlayer->m_flFlashDuration" ) );

		return *reinterpret_cast< float* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline int& c_player::shots_fired( ) {
		static const auto& net_var = g_net_vars->entry( xorstr_( "CCSPlayer->m_iShotsFired" ) );

		return *reinterpret_cast< int* >(
			reinterpret_cast< std::uintptr_t >( this ) + std::get< std::uint32_t >( net_var )
		);
	}

	__forceinline c_local_player::operator bool( ) const {
		return !!*m_local;
	}

	__forceinline c_local_player::operator c_player* ( ) const {
		return *m_local;
	}

	__forceinline c_player* c_local_player::operator ->( ) const {
		return *m_local;
	}
}