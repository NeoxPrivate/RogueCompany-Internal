#pragma once
#include "stdafx.h"

struct Setting {
	BOOL ShowMenu;
	bool Aimbot;
	float FOV;
	bool ESP;
	bool PlayerName;
};

extern Setting Settings;

namespace playerSettings {

	extern VOID Init();
}