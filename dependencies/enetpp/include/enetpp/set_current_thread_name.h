#ifndef ENETPP_SET_CURRENT_THREAD_NAME_H_
#define ENETPP_SET_CURRENT_THREAD_NAME_H_

#include "enet/enet.h"

namespace enetpp {

	inline void set_current_thread_name(const char* name) {

#ifdef _WIN32
#ifndef NDEBUG
		//https://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
		const DWORD MS_VC_EXCEPTION = 0x406D1388;

		#pragma pack(push,8)
		typedef struct tagTHREADNAME_INFO {
			DWORD dwType; // Must be 0x1000.
			LPCSTR szName; // Pointer to name (in user addr space).
			DWORD dwThreadID; // Thread ID (-1=caller thread).
			DWORD dwFlags; // Reserved for future use, must be zero.
		} THREADNAME_INFO;
		#pragma pack(pop)

		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = name;
		info.dwThreadID = ::GetCurrentThreadId();
		info.dwFlags = 0;

		__try {
			RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
		}
#endif
#else

		//todo - unknown platform

#endif

	}

}

#endif
