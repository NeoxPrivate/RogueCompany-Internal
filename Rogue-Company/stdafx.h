#pragma once
#pragma warning(disable : 4244)
#include <Windows.h>
#include <vector>
#include <iostream>

#include <MinHook.h>
#pragma comment(lib, "MinHook.lib")

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_internal.h>

#include "settings.h"
#include "xorstr.hpp"
#include "structs.h"
#include "log.h"
#include "helper.h"
#include "util.h"
#include "render.h"
#include "core.h"
#include "font.h"

template<typename Ret>
Ret read(PVOID base) {
	return *(Ret*)base;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
