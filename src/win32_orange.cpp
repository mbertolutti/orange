#include <Windows.h>

INT WINAPI
WinMain(HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        PSTR lpCmdLine,
        INT nCmdShow)
{
    MessageBox(
        0,
        "orange, here  we go!",
        "orange",
        0x00000000L
    );
    return 0;
}