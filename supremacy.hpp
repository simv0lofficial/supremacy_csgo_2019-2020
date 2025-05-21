#pragma once

#define TWENTYTWENTY TRUE
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define _HAS_EXCEPTIONS 0

#define IMGUI_DEFINE_MATH_OPERATORS

#define IM_VEC2_CLASS_EXTRA \
        __forceinline float dot( const ImVec2& rhs ) const { return rhs.x * x + rhs.y * y; } \
        __forceinline float length_sqr( ) const { return dot( *this ); } \
        __forceinline float length( ) const { return std::sqrt( length_sqr( ) ); } \
        __forceinline float normalize( ) { const auto len = length( ); if ( len ) { x /= len; y /= len; } return len; } \
        __forceinline ImVec2 normalized( ) { auto ret = *this; ret.normalize( ); return ret; }

#define IM_VEC3_CLASS_EXTRA \
        __forceinline ImVec3 cross( const ImVec3& rhs ) const { return { ( y * rhs.z ) - ( z * rhs.y ), ( z * rhs.x ) - ( x * rhs.z ), ( x * rhs.y ) - ( y * rhs.x ) }; } \
	    __forceinline float dot( const float* const rhs ) const { return ( x * rhs[ 0u ] + y * rhs[ 1u ] + z * rhs[ 2u ] ); } \
        __forceinline float dot( const ImVec3& rhs ) const { return rhs.x * x + rhs.y * y + rhs.z * z; } \
        __forceinline float length_sqr( ) const { return dot( *this ); } \
        __forceinline float length_2d_sqr( ) const { return x * x + y * y; } \
        __forceinline float length( ) const { return std::sqrt( length_sqr( ) ); } \
        __forceinline bool is_valid( ) const { return std::isfinite( x ) && std::isfinite( y ) && std::isfinite( z ); } \
        __forceinline float length_2d( ) const { return std::sqrt( length_2d_sqr( ) ); } \
        __forceinline float normalize( ) { const auto len = length( ); if ( len ) { x /= len; y /= len; z /= len; } return len; } \
        __forceinline ImVec3 normalized( ) { auto ret = *this; ret.normalize( ); return ret; }

#if __clang__ && __cplusplus >= 201907l
#define __cpp_lib_format
#endif

#include <windows.h>
#include <intrin.h>
#include <wincrypt.h>
#include <algorithm>
#include <thread>
#include <array>
#include <deque>
#include <unordered_map>
#include <format>
#include <mutex>
#include <filesystem>
#include <set>
#include <type_traits>
#include <fstream>
#include <semaphore>
#include <future>
#include <tlhelp32.h>
#include <psapi.h>
#include <signal.h>
#pragma comment (lib, "urlmon.lib")

#include <d3d9.h>
#include <d3dx9.h>

#include "dependencies/hash.hpp"
#include "dependencies/xorstr.hpp"
#include "dependencies/defs.hpp"
#include "dependencies/syscall.hpp"

#define JSON_NOEXCEPTION

#include "dependencies/json.hpp"

#include "dependencies/minhook/minhook.h"
#include "dependencies/bass/API.h"
#include "dependencies/imgui/imgui.h"
#include "dependencies/imgui/imgui_freetype.h"
#include "dependencies/imgui/imgui_internal.h"
#include "dependencies/imgui/imgui_impl_dx9.h"
#include "dependencies/imgui/imgui_impl_win32.h"
IMGUI_IMPL_API LRESULT  ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#define SOL_NO_EXCEPTIONS 1

#include "dependencies/fonts.hpp"
#include "dependencies/icons.hpp"
#include "dependencies/key_handler.hpp"

#include "math/math.hpp"
#include "util/util.hpp"
#include "dependencies/config_system/config_system.hpp"
#include "context/context.hpp"
#include "valve/valve.hpp"
#include "hacks/hacks.hpp"

#include "menu/menu.hpp"

#include "hooks/hooks.hpp"