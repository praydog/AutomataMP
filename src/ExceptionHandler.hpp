#include <windows.h>

namespace automatamp {
LONG WINAPI global_exception_handler(struct _EXCEPTION_POINTERS* ei);
void setup_exception_handler();
}
