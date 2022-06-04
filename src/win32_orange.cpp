#include <Windows.h>
#include <stdint.h>
#include <cmath>

#define internal_function_static static
#define local_persist_static static
#define global_variable_static static

global_variable_static bool Running;

global_variable_static BITMAPINFO BitmapInfo;
global_variable_static void* BitmapMemory; // LPVOID
global_variable_static int BitmapWidth;
global_variable_static int BitmapHeight;
global_variable_static int BytesPerPixel;

internal_function_static void
RenderGradient(int XOffset, int YOffset)
{
    int Width = BitmapWidth;
    // int Height = BitmapHeight

    int Stride = Width * BytesPerPixel;
    uint8_t* Row = (uint8_t*)BitmapMemory;
    for (int Y = 0; Y < BitmapHeight; ++Y)
    {
        uint32_t* Pixel = (uint32_t*)Row;
        for (int X = 0; X < BitmapWidth; ++X)
        {
            /*
            Bitmaps:
            Pixel in memory: RR GG BB xx
            Little endian architecture
            0x xxBBGGRR

            Windows Bitmaps:
            Pixel in memory: BB GG RR xx
            Little endian architecture
            0x xxRRGGBB
            */

            uint8_t Blue = (X + XOffset);
            uint8_t Green = (Y + YOffset);

            *Pixel++ = ((Green << 8 | Blue));
        }

        Row += Stride;
    }
}


internal_function_static void
Win32ResizeDIBSection(int Width, int Height)
{
    if (BitmapMemory)
    {
        VirtualFree(BitmapMemory, 0, MEM_RELEASE);
    }

    BitmapWidth = Width;
    BitmapHeight = Height;
    BytesPerPixel = 4;

    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = BitmapWidth;
    BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;

    int BitmapMemorySize = (BitmapWidth * BitmapHeight) * BytesPerPixel;
    BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
}

internal_function_static void
Win32UpdateWindow(HDC Hdc, RECT* ClientRect, int X, int Y, int Width, int Height)
{
    int WindowWidth = ClientRect->right - ClientRect->left;
    int WindowHeight = ClientRect->bottom - ClientRect->top;
    StretchDIBits(
        Hdc,
        /*
        X, Y, Width, Height,
        X, Y, Width, Height,
        */
        0, 0, BitmapWidth, BitmapHeight,
        0, 0, WindowWidth, WindowHeight,
        BitmapMemory,
        &BitmapInfo,
        DIB_RGB_COLORS,
        SRCCOPY
    );
}

LRESULT CALLBACK Wind32MainWndProc(
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
            RECT ClientRect;
            GetClientRect(hWnd, &ClientRect);
            int Width = ClientRect.right - ClientRect.left;
            int Height = ClientRect.bottom - ClientRect.top;
            Win32ResizeDIBSection(Width, Height);
        } break;

        case WM_CLOSE:
        {
            Running = false;
        } break;

        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;

        case WM_DESTROY:
        {
            Running = false;
        } break;

        case WM_PAINT:
        {
            PAINTSTRUCT PaintStruct;
            HDC Hdc = BeginPaint(hWnd, &PaintStruct);
            int X = PaintStruct.rcPaint.left;
            int Y = PaintStruct.rcPaint.top;
            int Width = PaintStruct.rcPaint.bottom - PaintStruct.rcPaint.top;
            int Height = PaintStruct.rcPaint.right - PaintStruct.rcPaint.left;

            RECT ClientRect;
            GetClientRect(hWnd, &ClientRect);

            Win32UpdateWindow(Hdc, &ClientRect, X, Y, Width, Height);
            EndPaint(hWnd, &PaintStruct);
        } break;

        default:
        {
            OutputDebugStringA("DefWindowProcW\n");
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
    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = Wind32MainWndProc; // pointer to function for window event response; MainWindowCallback;
    WindowClass.hInstance = hInstance;
    // WindowClass.hIcon;
    WindowClass.lpszClassName = L"orangeWindowClass";

    if (RegisterClass(&WindowClass))
    {
        HWND hWnd =
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
        if (hWnd)
        {
            int XOffset = 0;
            int YOffset = 0;

            Running = true;
            while (Running)
            {
                MSG Message;
                while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
                {
                    // Check if windows sends us WM_Quit.
                    if (Message.message == WM_QUIT)
                    {
                        Running = false;
                    }
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }
                
                RenderGradient(XOffset, YOffset);

                HDC DeviceContext = GetDC(hWnd);
                RECT ClientRect;
                GetClientRect(hWnd, &ClientRect);
                int WindowWidth = ClientRect.right - ClientRect.left;
                int WindowHeight = ClientRect.bottom - ClientRect.top;
                Win32UpdateWindow(DeviceContext, &ClientRect, 0, 0, WindowWidth, WindowHeight);
                ReleaseDC(hWnd, DeviceContext);

                ++XOffset;
                YOffset += 2;
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
