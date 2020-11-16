#pragma once
#include "stdafx.h"

#define RVA(addr, size) ((PBYTE)(addr + *(DWORD*)(addr + ((size) - 4)) + size))

namespace Util {

	PIMAGE_NT_HEADERS getHeader(HMODULE base);
	PBYTE FindPattern(HMODULE module, LPCSTR pattern, LPCSTR mask);
}