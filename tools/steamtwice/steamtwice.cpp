#include <string_view>
#include <windows.h>

#include <SafetyHook.hpp>

std::unique_ptr<safetyhook::InlineHook> g_cpwhook{};

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
        const auto result =  g_cpwhook->call<BOOL>(lpApplicationName, 
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
    auto factory = SafetyHookFactory::init();
    auto builder = factory->acquire(); 

    const auto target = GetProcAddress(GetModuleHandleA("kernel32.dll"), "CreateProcessW");

    g_cpwhook = builder.create_inline(target, cpwhook);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)startup_thread, nullptr, 0, nullptr);
    }

    return TRUE;
}