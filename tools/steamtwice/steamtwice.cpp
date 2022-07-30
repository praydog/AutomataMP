#include <string_view>
#include <windows.h>

#include <MinHook.h>

decltype(CreateProcessW)* g_original_cpw{nullptr};

BOOL WINAPI cpwhook(
  LPCWSTR               lpApplicationName,
  LPWSTR                lpCommandLine,
  LPSECURITY_ATTRIBUTES lpProcessAttributes,
  LPSECURITY_ATTRIBUTES lpThreadAttributes,
  BOOL                  bInheritHandles,
  DWORD                 dwCreationFlags,
  LPVOID                lpEnvironment,
  LPCWSTR               lpCurrentDirectory,
  LPSTARTUPINFOW        lpStartupInfo,
  LPPROCESS_INFORMATION lpProcessInformation
)
{
    auto launch = [&](const wchar_t* custom_command_line = nullptr) {
        const auto result =  g_original_cpw(lpApplicationName, 
                    custom_command_line != nullptr ? (LPWSTR)custom_command_line : lpCommandLine, 
                    lpProcessAttributes, 
                    lpThreadAttributes, 
                    bInheritHandles, 
                    dwCreationFlags, 
                    lpEnvironment, 
                    lpCurrentDirectory, 
                    lpStartupInfo, 
                    lpProcessInformation);

        if (lpCommandLine != nullptr && std::wstring_view{lpCommandLine}.find(L"NieRAutomata") != std::wstring_view::npos) {
            // Remove the process handle so Steam can't close it.
            CloseHandle(lpProcessInformation->hProcess);
            lpProcessInformation->hProcess = nullptr;

            // Remove the thread handle so Steam can't close it.
            CloseHandle(lpProcessInformation->hThread);
            lpProcessInformation->hThread = nullptr;
        }

        return result;
    };

    if (lpCommandLine != nullptr && std::wstring_view{lpCommandLine}.find(L"NieRAutomata.exe") != std::wstring_view::npos) {
        // Launch it again. Remove the suspended flag.
        dwCreationFlags &= ~CREATE_SUSPENDED;

        launch();
    }

    const auto result = launch();

    return result;
}

void startup_thread() {
    MH_Initialize();

    const auto target = GetProcAddress(GetModuleHandleA("kernel32.dll"), "CreateProcessW");

    MH_CreateHook(
        target,
        cpwhook,
        (LPVOID*)&g_original_cpw
    );

    MH_EnableHook(target);
}

// dllmain
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)startup_thread, nullptr, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}