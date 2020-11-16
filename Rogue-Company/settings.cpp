#pragma once
#include "stdafx.h"


Setting Settings;

namespace playerSettings {

	VOID Init() {

		/*
		* Auto Assign Vars to TRUE/FALSE.
		*/

		Settings.Aimbot = TRUE;
		Settings.ESP = TRUE;
		Settings.FOV = 250.0f;
		Settings.PlayerName = TRUE;
	}
}
