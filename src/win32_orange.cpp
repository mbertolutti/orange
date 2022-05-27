#include <Windows.h>
#include <WinUser.h>

LRESULT CALLBACK MainWndProc(
    HWND hWnd,        // handle to window
    UINT uMsg,        // message identifier
    WPARAM wParam,    // first message parameter
    LPARAM lParam)    // second message parameter
{
    LRESULT Result = 0;

    switch (uMsg)
    {
        case WM_SIZE:
        {
            OutputDebugStringA("WM_SIZE\n");
        } break;

        case WM_DESTROY:
        {
            OutputDebugStringA("WM_DESTROY\n");
        } break;

        case WM_CLOSE:
        {
            OutputDebugStringA("WM_CLOSE\n");
        } break;

        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;

        case WM_PAINT:
        {
            PAINTSTRUCT paintStruct;
            HDC hdc = BeginPaint(hWnd, &paintStruct);
            int x = paintStruct.rcPaint.left;
            int y = paintStruct.rcPaint.top;
            int width = paintStruct.rcPaint.bottom - paintStruct.rcPaint.top;
            int height = paintStruct.rcPaint.right - paintStruct.rcPaint.left;
            static DWORD operation = WHITENESS;
            if (operation == WHITENESS)
            {
                operation = BLACKNESS;
            }
            else
            {
                operation = WHITENESS;
            }
            PatBlt(hdc, x, y, width, height, operation);
            EndPaint(hWnd, &paintStruct);
        } break;

        default:
        {
            OutputDebugStringA("OutputDebugStringA\n");
            Result = DefWindowProcW(hWnd,uMsg,wParam,lParam);
        } break;
    }
    return Result;
}

INT CALLBACK
WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ PSTR lpCmdLine,
    _In_ INT nCmdShow)
{
    WNDCLASS WindowClass = {};
    WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = MainWndProc; // pointer to function for window event response; MainWindowCallback;
    WindowClass.hInstance = hInstance;
    // WindowClass.hIcon;
    WindowClass.lpszClassName = L"orangeWindowClass";

    if (RegisterClass(&WindowClass))
    {
        HWND hwnd =
            CreateWindowEx(
                0,
                WindowClass.lpszClassName,
                L"orange",
                WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                CW_USEDEFAULT, // x
                CW_USEDEFAULT, // y
                CW_USEDEFAULT, // nWidth
                CW_USEDEFAULT, // nHeight
                0,
                0,
                hInstance,
                0
            );
        if (hwnd != NULL)
        {
            for (;;)
            {
                MSG msg;
                BOOL bRet = GetMessage(&msg, NULL, 0, 0);
                if (bRet > 0)
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            // logging
        }
    }
    else
    {
        // logging
    }

    return 0;
}
