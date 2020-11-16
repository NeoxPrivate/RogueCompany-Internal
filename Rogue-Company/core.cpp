#pragma once
#include "core.h"

namespace Core {

	AActor* Pawn = NULL;
	HMODULE moduleBase;
	ULONG(__fastcall* GetViewPoint)(PlayerController*, FVector*, FRotator*) = NULL;

	/*
	* AController::SetControlRotation
	* 
	*/
	VOID SetControlRotation(PlayerController* Controller, FRotator NewRotation) {

		auto pController = read<PVOID>(Controller);
		if (!pController) {
			return;
		}

		return reinterpret_cast<VOID(__fastcall*)(ULONG_PTR, FRotator)>
			(read<PVOID>((PBYTE)pController + 0x680))((ULONG_PTR)Controller, NewRotation);
	}

	VOID SetIgnoreLookInput(PlayerController* Controller, BOOL IsLocked) {

		auto pController = read<PVOID>(Controller);
		if (!pController) {
			return;
		}

		return reinterpret_cast<VOID(__fastcall*)(ULONG_PTR, CHAR)>
			(read<PVOID>((PBYTE)pController + 0x740))((ULONG_PTR)Controller, IsLocked);
	}

	ULONG GetPlayerViewPointHook(PlayerController* Controller, FVector* pLocation, FRotator* pRotation) {

		GetViewPoint(Controller, pLocation, pRotation);

		/*
		* You can do whatever you want to the camera location/rotation.
		*/

		if (!Pawn) {
			return GetViewPoint(Controller, pLocation, pRotation);;
		}

		auto headPos = Util::GetBoneMatrix(Pawn->Mesh, 110);

		if (GetAsyncKeyState(VK_RBUTTON)) {

			auto angles = FRotator::CalcAngle(*pLocation, headPos).ClampAngles();
			*pRotation = angles;
			return TRUE;
		}

		return GetViewPoint(Controller, pLocation, pRotation);
	}


	VOID Init(HMODULE module) {

		if (!module) {
			Log::DebugText(xorstr_("Invalid Image Base."));
			return;
		}

		moduleBase = module;

		auto addr = Util::FindPattern(moduleBase,
			xorstr_(""),//hello please ifnd sigs your self
			xorstr_(""));

		if (!addr) {
			Log::DebugText(xorstr_("Failed to get address for GetPlayerViewPoint."));
			return;
		}

		Log::DebugText(xorstr_("GetPlayerViewPoint -> 0x%x"), addr - (PBYTE)moduleBase);

		MH_CreateHook(addr, GetPlayerViewPointHook, (VOID**)&GetViewPoint);
		MH_EnableHook(addr);

		return;
	}

}
