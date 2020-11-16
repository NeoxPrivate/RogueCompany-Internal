#pragma once
#include "stdafx.h"

#define PI (3.14159265358979323846264338327950288419716939937510)

class PlayerController;

struct FVector2D {
	float X;
	float Y;

	inline BOOL IsValid() {
		return X != NULL && Y != NULL;
	}

	inline float distance() {
		return sqrtf(this->X * this->X + this->Y * this->Y);
	}
};

struct FVector {
	float X;
	float Y;
	float Z;

	FVector operator-(FVector buffer) {

		return FVector
		(
			{ this->X - buffer.X, this->Y - buffer.Y, this->Z - buffer.Z }
		);
	}

	FVector operator+(FVector buffer) {

		return FVector
		(
			{ this->X + buffer.X, this->Y + buffer.Y, this->Z + buffer.Z }
		);
	}

	inline float distance() {
		return sqrtf(this->X * this->X + this->Y * this->Y + this->Z * this->Z);
	}

	FVector2D ToScreen(PlayerController* Controller);
};

struct FRotator {

	float Pitch;
	float Yaw;
	float Roll;

	FRotator ClampAngles() {

		if (this->Yaw < -180.0f)
			this->Yaw += 360.0f;

		if (this->Yaw > 180.0f)
			this->Yaw -= 360.0f;

		if (this->Pitch < -74.0f)
			this->Pitch = -74.0f;

		if (this->Pitch > 74.0f)
			this->Pitch = 74.0f;

		return *this;
	}

	static FRotator CalcAngle(FVector cameraLocation, FVector boneLocation) {
		
		FRotator result;

		auto delta = boneLocation - cameraLocation;
		auto distance = delta.distance();

		result.Yaw = atan2f(delta.Y, delta.X) * (180.0f / PI);
		result.Pitch = (-((acosf((delta.Z / distance)) * 180.0f / PI) - 90.0f));
		result.Roll = NULL;

		return result;
	}

};

struct FMatrix {
	float M[4][4];
};

struct FBoxSphereBounds {
	FVector Origin;
	FVector BoxExtent;
	float SphereRadius;
};

struct FBox {
	FVector Min;
	FVector Max;

	FBox() {

		Min.X = NULL;
		Min.Y = NULL;
		Min.Z = NULL;

		Max.X = NULL;
		Max.Y = NULL;
		Max.Z = NULL;
	}
};

template<class T>
struct TArray
{
	friend class FString;

public:
	inline TArray()
	{
		Data = NULL;
		Count = Max = 0;
	};

	inline INT Num() const
	{
		return Count;
	};

	inline T& operator[](int i)
	{
		return Data[i];
	};

	inline T& operator[](int i) const
	{
		return Data[i];
	};

	inline BOOL IsValidIndex(int i) const
	{
		return i < Num();
	}

private:
	T* Data;
	INT32 Count;
	INT32 Max;
};


class FString : private TArray<wchar_t>
{
public:
	inline FString() {

	};

	inline int Count() {
		return Num();
	}

	inline bool IsValid() {
		return Data != NULL;
	}

	inline const wchar_t* data() const {
		return Data;
	}

	inline const char* c_str() const {
		return std::string(Data, &Data[wcslen(Data)]).c_str();
	}
};

class USceneComponent {
public:
	char __pad[0x11C];
	FVector RelativeLocation;
	FRotator RelativeRotation;
};

class TeamComponent {
public:
	char __pad0[0x220];
	INT TeamIndex;
};

class AKSPlayerState {
public:
	char __pad[0x300];
	FString PlayerName;
	char __pad0[0x82];
	TeamComponent* TeamComponent;
	char __pad1[0xA0];
	INT EliminatedState;
	char __pad2[0x1FB4];
	float Health;
};

class USkeletalMeshComponent {
public:
	char __pad0[0x450];
	ULONG_PTR SkeletalMesh;
};

class AActor {
public:
	char __pad1[0x130];
	USceneComponent* RootComponent;
	char __pad2[0x108];
	AKSPlayerState* PlayerState;
	char __pad3[0x38];
	USkeletalMeshComponent* Mesh;
};

class ULevel {
public:
	char __pad0[0x98];
	TArray<AActor*> Actors;
};

class PlayerController {
public:
	char __pad0[0x2A0];
	AActor* AcknowledgedPawn;
};

class ULocalPlayer {
public:
	char __pad0[0x30];
	PlayerController* Controller;
};

class GameInstance {
public:
	char __pad0[0x38];
	TArray<ULocalPlayer*> LocalPlayers;
};

class UWorld {
public:
	char __pad1[0x148];
	TArray<ULevel*> Levels;
	char __pad0[0x2A];
	GameInstance* OwningGameInstance;

	UWorld* GetWorld() {
		return (UWorld*)(*(ULONG_PTR*)(this));
	}
};