#include <Windows.h>
#include <stdint.h>
#include <cmath>

#define internal_function_static static
#define local_persist_static static
#define global_variable_static static

struct win32_offscreen_buffer
{
    BITMAPINFO Info;
    void* Memory; // LPVOID
    int Width;
    int Height;
    int Stride;
    int BytesPerPixel;
};

global_variable_static bool Running;
global_variable_static win32_offscreen_buffer GlobalBackBuffer;

struct win32_window_dimension
{
    int Width;
    int Height;
};

win32_window_dimension Win32GetWindowDimension(HWND hWnd)
{
    win32_window_dimension Result;

    RECT ClientRect;
    GetClientRect(hWnd, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;

    return Result;
}

internal_function_static void RenderGradient(
    win32_offscreen_buffer Buffer, int BlueXOffset, int GreenYOffset)
{
    int Width = Buffer.Width;
    int Height = Buffer.Height;

    uint8_t* Row = (uint8_t*)Buffer.Memory;
    for (int Y = 0; Y < Height; ++Y)
    {
        uint32_t* Pixel = (uint32_t*)Row;
        for (int X = 0; X < Width; ++X)
        {
            uint8_t Blue = (X + BlueXOffset);
            uint8_t Green = (Y + GreenYOffset);

            *Pixel++ = ((Green << 8 | Blue));
        }

        Row += Buffer.Stride;
    }
}

internal_function_static void Win32ResizeDIBSection(
    win32_offscreen_buffer* Buffer, int Width, int Height)
{
    if (Buffer->Memory)
    {
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }

    Buffer->Width = Width;
    Buffer->Height = Height;
    Buffer->BytesPerPixel = 4;

    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;

    int BitmapMemorySize =
        (Buffer->Width * Buffer->Height) * Buffer->BytesPerPixel;

    Buffer->Memory = VirtualAlloc(0, BitmapMemorySize,
        MEM_COMMIT, PAGE_READWRITE);


    Buffer->Stride = Width * Buffer->BytesPerPixel;
}

internal_function_static void Win32RenderWindow(
    HDC Hdc,
    int WindowWidth, int WindowHeight,
    win32_offscreen_buffer Buffer,
    int X, int Y, int Width, int Height)
{
    StretchDIBits(
        Hdc,
        /*
        X, Y, Width, Height,
        X, Y, Width, Height,
        */
        0, 0, WindowWidth, WindowHeight,
        0, 0, Buffer.Width, Buffer.Height,
        Buffer.Memory,
        &Buffer.Info,
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
            HDC DeviceContext = BeginPaint(hWnd, &PaintStruct);
            int X = PaintStruct.rcPaint.left;
            int Y = PaintStruct.rcPaint.top;
            int Width = PaintStruct.rcPaint.bottom - PaintStruct.rcPaint.top;
            int Height = PaintStruct.rcPaint.right - PaintStruct.rcPaint.left;

            win32_window_dimension Dimension = Win32GetWindowDimension(hWnd);

            Win32RenderWindow(
                DeviceContext,
                Dimension.Width, Dimension.Height,
                GlobalBackBuffer,
                X, Y, Width, Height);

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

    Win32ResizeDIBSection(&GlobalBackBuffer, 1280, 720);

    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    // pointer to function for window event response; MainWindowCallback;
    WindowClass.lpfnWndProc = Wind32MainWndProc;
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
                
                RenderGradient(GlobalBackBuffer, XOffset, YOffset);

                HDC DeviceContext = GetDC(hWnd);

                win32_window_dimension Dimension = Win32GetWindowDimension(hWnd);
                Win32RenderWindow(
                    DeviceContext,
                    Dimension.Width, Dimension.Height,
                    GlobalBackBuffer,
                    0, 0, Dimension.Width, Dimension.Height);

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
