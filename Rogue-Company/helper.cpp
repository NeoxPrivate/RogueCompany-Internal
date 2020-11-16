#pragma once
#include "util.h"

namespace Util {

	PIMAGE_NT_HEADERS getHeader(HMODULE module) {
		return (PIMAGE_NT_HEADERS)((PBYTE)module + PIMAGE_DOS_HEADER(module)->e_lfanew);
	}

	PBYTE FindPattern(HMODULE module, LPCSTR pattern, LPCSTR mask) {

		size_t size = getHeader(module)->OptionalHeader.SizeOfImage;

		auto checkMask = [](PBYTE buffer, LPCSTR pattern, LPCSTR mask) -> BOOL
		{
			for (auto x = buffer; *mask; pattern++, mask++, x++) {
				auto addr = *(BYTE*)(pattern);
				if (addr != *x && *mask != '?')
					return FALSE;
			}

			return TRUE;
		};

		for (auto x = 0; x < size - strlen(mask); x++) {

			auto addr = (PBYTE)module + x;
			if (checkMask(addr, pattern, mask))
				return addr;
		}

		return NULL;
	}
}