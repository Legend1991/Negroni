#if defined(_WIN32)
#define OS_WINDOWS
#elif defined(__APPLE__)
#define OS_MACOS
#elif defined(__linux__)
#define OS_LINUX
#endif

#include "Lemonade.h"

using namespace LMD;

int main(int, char**)
{
    Lemonade app(1366, 768, L"Lemonade (DX11)");

    return app.exec();
}
