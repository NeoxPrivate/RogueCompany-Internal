#pragma once
#include "util.h"
#include <fstream>

namespace Util {

	HMODULE moduleBase;
	UWorld* World;

	VOID FreeMemory(PVOID buffer) {

		static PBYTE addr = NULL;
		if (!addr) {
			
			addr = FindPattern(moduleBase,
				xorstr_(""),// hello find the sigs your self
				xorstr_(""));

			if (!addr) {
				Log::DebugText(xorstr_("Unable to find signature for FreeMemory."));
				return;
			}

			Log::DebugText(xorstr_("Free -> 0x%x"), addr - (PBYTE)moduleBase);
		}

		return reinterpret_cast<VOID(*)(PVOID)>(addr)(buffer);
	}

	char* GetObjectName(AActor* Actor) {

		static PBYTE addr = NULL;
		if (!addr) {

			addr = FindPattern(moduleBase,
				xorstr_("),// hello find the sigs your self
				xorstr_(""));
			Log::DebugText(xorstr_("GetObjectName -> 0x%x"), addr - (PBYTE)moduleBase);
		}
		
		if (!addr) Log::DebugText(xorstr_("Unable to find signature for -> GetObjectName."));

		auto GetObjectName = reinterpret_cast<FString * (*)(FString*, AActor*)>(addr);

		FString buffer;
		GetObjectName(&buffer, Actor);

		if (!buffer.c_str()) {
			return {};
		}

		CHAR result[256];
		strcpy_s(result, sizeof(result), buffer.c_str());
		FreeMemory((PVOID)buffer.data());

		return result;
	}

	
	FVector GetBoneMatrix(USkeletalMeshComponent* mesh, INT index) {

		static PBYTE addr = NULL;
		if (!addr) {

			addr = FindPattern(moduleBase,
				xorstr_(""),
				xorstr_(""));// hello find the sigs your self

			if (!addr) {
				Log::DebugText(xorstr_("Failed to find address of GetBoneMatirx()."));
			}

			Log::DebugText(xorstr_("GetBoneMatrix -> 0x%x"), addr - (PBYTE)moduleBase);
		}

		auto GetBoneMatrix = reinterpret_cast<FMatrix*(*)(USkeletalMeshComponent*, FMatrix*, INT)>(addr);

		FMatrix buffer = { 0 };
		GetBoneMatrix(mesh, &buffer, index);

		return FVector({ buffer.M[3][0], buffer.M[3][1], buffer.M[3][2] });
	}

	FBox GetBoundingBox(USkeletalMeshComponent* mesh, FVector RelativeLocation) {

		static PBYTE addr = NULL;
		if (!addr) {

			addr = FindPattern(moduleBase,
				xorstr_(""),// hello find the sigs your self
				xorstr_(""));

			if (!addr) {
				Log::DebugText(xorstr_("Failed to find address of GetBoundingBox."));
			}

			addr = RVA(addr, 5);
			Log::DebugText(xorstr_("GetBoundingBox -> 0x%x"), addr - (PBYTE)moduleBase);
		}

		if (!mesh->SkeletalMesh)
			return {};

		auto GetBoundingBox = reinterpret_cast<VOID(*)(ULONG_PTR, FBoxSphereBounds*)>(addr);

		FBoxSphereBounds buffer = { 0 };
		GetBoundingBox(mesh->SkeletalMesh, &buffer);

		FBox box = {};
		box.Min = RelativeLocation - buffer.BoxExtent;
		box.Max = RelativeLocation + buffer.BoxExtent;
		
		return box;
	}

	BOOL ProjectWorldToScreen(PlayerController* Controller, FVector& WorldPosition, FVector2D* ScreenPosition) {

		static PBYTE addr = NULL;
		if (!addr) {

			addr = FindPattern(moduleBase,
				xorstr_(""),// hello find the sigs your self
				xorstr_(""));

			addr = RVA(addr, 5);
			Log::DebugText(xorstr_("ProjectWorldToScreen -> 0x%x"), addr - (PBYTE)moduleBase);
		}

		if (!addr) Log::DebugText(xorstr_("Unable to find signature for -> ProjectWorldLocationToScreen."));

		return reinterpret_cast<CHAR(*)
			(PlayerController*, FVector, FVector2D*, CHAR)>(addr)
			(Controller, WorldPosition, ScreenPosition, FALSE);
	}

	BOOL LineOfSightTo(PlayerController* Controller, AActor* Actor) {

		static PBYTE addr = NULL;
		if (!addr) {

			addr = FindPattern(moduleBase,
				xorstr_(""),// hello find the sigs your self
				xorstr_(""));

			if (!addr) {
				Log::DebugText(xorstr_("Unable to find signature for LineOfSightTo."));
				return FALSE;
			}

			Log::DebugText(xorstr_("LineOfSightTo -> 0x%x"), addr - (PBYTE)moduleBase);
		}

		auto LineOfSight = reinterpret_cast<BOOL(*)(PlayerController*, AActor*, FVector)>(addr);

		return LineOfSight(Controller, Actor, FVector({ 0, 0, 0 }));
	}

	VOID Init(HMODULE module) {

		if (!module) {
			Log::DebugText(xorstr_("Invalid Image Base."));
			return;
		}

		moduleBase = module;

		Log::DebugText(xorstr_("RogueCompany.exe Image Base -> 0x%x"), module);

		auto addr = FindPattern(module,
			xorstr_(""),
			xorstr_(""));

		if (!addr) {
			Log::DebugText(xorstr_("Unable to find signature for -> UWorld."));
			return;
		}

		addr = RVA(addr, 7);
		Log::DebugText(xorstr_("UWorld -> 0x%x"), addr - (PBYTE)moduleBase);

		World = (UWorld*)addr;
	}
}
