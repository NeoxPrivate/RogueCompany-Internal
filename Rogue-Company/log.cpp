#pragma once
#include "log.h"

namespace Log {

	VOID DebugText(LPCSTR text, ...) {

		va_list(args);
		va_start(args, text);

		CHAR wbuffer[256] = {};
		vsprintf_s(wbuffer, text, args);

		va_end(args);

		std::string buffer = xorstr_("[rogue-company] ") + std::string(wbuffer);

		OutputDebugStringA(buffer.c_str());
	}
}