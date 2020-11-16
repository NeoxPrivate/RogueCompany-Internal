#pragma once
#include "stdafx.h"

namespace Core {

	extern AActor* Pawn;

	VOID SetControlRotation(PlayerController* Controller, FRotator NewRotation);
	VOID SetIgnoreLookInput(PlayerController* Controller, BOOL IsLocked);
	VOID Init(HMODULE module);
}