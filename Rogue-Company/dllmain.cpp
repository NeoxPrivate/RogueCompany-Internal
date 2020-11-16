#pragma once
#include "stdafx.h"


FVector2D FVector::ToScreen(PlayerController* Controller) {

	if (!Controller)
		return {};

	FVector2D resultPosition = { 0 };
	Util::ProjectWorldToScreen(Controller, *this, &resultPosition);
	return resultPosition;

}

VOID Init() {

	auto status = MH_Initialize();

	if (status == MH_OK) {
		Log::DebugText(xorstr_("MH_Initialize was sucessfull. Time -> %s"), xorstr_(__TIME__));
	}
	else {
		Log::DebugText(xorstr_("MH_ERROR = %d"), status);
	}

	playerSettings::Init();

	auto base = GetModuleHandle(0); // -> the first module is **always** the process.
	if (!base) {
		Log::DebugText(xorstr_("Failed to get Image Base of process."));
		return;
	}

	Util::Init(base);
	Core::Init(base);

	base = GetModuleHandle(xorstr_(L"dxgi.dll"));
	if (!base) {
		Log::DebugText(xorstr_("dxgi.dll not loaded, manually loading... (this shouldn't even be happening unless this isn't the game......lol)"));
		base = LoadLibrary(xorstr_(L"dxgi.dll"));
	}

	Render::Init(base);
}


BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {


	if (reason == DLL_PROCESS_ATTACH) {
		Init();
	}

	return TRUE;
}