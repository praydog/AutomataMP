#include <chrono>
#include <mutex>
#include <thread>
#include <string_view>
#include <windows.h>
#include <winternl.h>

#include <utility/Module.hpp>
#include <utility/Thread.hpp>
#include <utility/FunctionHook.hpp>

#include "ExceptionHandler.hpp"
#include "AutomataMP.hpp"

HMODULE g_dinput = 0;
std::mutex g_load_mutex{};

void failed() {
    MessageBox(0, "AutomataMP: Unable to load the original dinput8.dll. Please report this to the developer.", "AutomataMP", 0);
    ExitProcess(0);
}

bool load_dinput8() {
    std::scoped_lock _{g_load_mutex};

    if (g_dinput) {
        return true;
    }

    wchar_t buffer[MAX_PATH]{0};
    if (GetSystemDirectoryW(buffer, MAX_PATH) != 0) {
        // Load the original dinput8.dll
        if ((g_dinput = LoadLibraryW((std::wstring{buffer} + L"\\dinput8.dll").c_str())) == NULL) {
            failed();
            return false;
        }

        return true;
    }

    failed();
    return false;
}

extern "C" {
// DirectInput8Create wrapper for dinput8.dll
__declspec(dllexport) HRESULT WINAPI
    direct_input8_create(HINSTANCE hinst, DWORD dw_version, const IID& riidltf, LPVOID* ppv_out, LPUNKNOWN punk_outer) {
// This needs to be done because when we include dinput.h in DInputHook,
// It is a redefinition, so we assign an export by not using the original name
#pragma comment(linker, "/EXPORT:DirectInput8Create=direct_input8_create")

    load_dinput8();
    return ((decltype(direct_input8_create)*)GetProcAddress(g_dinput, "DirectInput8Create"))(hinst, dw_version, riidltf, ppv_out, punk_outer);
}
}

std::unique_ptr<FunctionHook> g_create_event_a_hook{};

// Allows us to launch the game more than once to test multiplayer.
HANDLE WINAPI create_event_a_hook(
  LPSECURITY_ATTRIBUTES lpEventAttributes,
  BOOL                  bManualReset,
  BOOL                  bInitialState,
  LPCSTR                lpName
)
{
    const auto result = g_create_event_a_hook->get_original<decltype(CreateEventA)>()(lpEventAttributes, bManualReset, bInitialState, lpName);

    if (lpName != nullptr) {
        spdlog::info("{}", lpName);

        if (std::string_view{lpName} == "NieR:AutomataEvent") {
            if (GetLastError() == ERROR_ALREADY_EXISTS) {
                spdlog::info("AutomataMP: AutomataEvent already exists...");
                std::this_thread::sleep_for(std::chrono::milliseconds(2000)); // so the game remembers the graphics settings, because it won't be able to access the config in use by the other instance
                return g_create_event_a_hook->get_original<decltype(CreateEventA)>()(lpEventAttributes, bManualReset, bInitialState, "AutomataMPEvent");
            }
        }
    }

    return result;
}

// SetCursorPos hook to prevent the game from taking control of the cursor.
std::unique_ptr<FunctionHook> g_set_cursor_pos_hook{};

BOOL WINAPI set_cursor_pos_hook(int x, int y) {
    return TRUE;
}

void startup_thread(HMODULE automatamp_module) {
    // We will set it once here, then do it continuously
    // every now and then because it gets replaced
    automatamp::setup_exception_handler();

#ifndef NDEBUG
    AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
#endif

    if (load_dinput8()) {
        const auto cva = (decltype(create_event_a_hook)*)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "CreateEventA");
        g_create_event_a_hook = std::make_unique<FunctionHook>(
            cva,
            &create_event_a_hook
        );
        g_create_event_a_hook->create();

        const auto scp = (decltype(set_cursor_pos_hook)*)GetProcAddress(GetModuleHandleW(L"user32.dll"), "SetCursorPos");

        g_set_cursor_pos_hook = std::make_unique<FunctionHook>(
            scp,
            &set_cursor_pos_hook
        );
        g_set_cursor_pos_hook->create();

        g_framework = std::make_unique<AutomataMP>(automatamp_module);
    }
}

BOOL APIENTRY DllMain(HANDLE handle, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)startup_thread, handle, 0, nullptr);
    }

    return TRUE;
}
