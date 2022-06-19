#include <Windows.h>
#include <stdint.h>
#include <xinput.h>

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
};

struct win32_window_dimension
{
    int Width;
    int Height;
};

global_variable_static bool GlobalRunning;
global_variable_static win32_offscreen_buffer GlobalBackBuffer;

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
    return(0);
}
global_variable_static x_input_get_state* XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
    return(0);
}
global_variable_static x_input_set_state* XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

internal_function_static void Win32LoadXInput(void)
{
    HMODULE XInputLibrary = LoadLibraryA("xinput1_3.dll");
    if (XInputLibrary)
    {
        XInputGetState = (x_input_get_state*)GetProcAddress(XInputLibrary, "XInputGetState");
        if (!XInputGetState)
        { 
            XInputGetState = XInputGetStateStub;
        }

        XInputSetState = (x_input_set_state*)GetProcAddress(XInputLibrary, "XInputSetState");
        {
            XInputSetState = XInputSetStateStub;
        }
    }
}

internal_function_static win32_window_dimension Win32GetWindowDimension(HWND hWnd)
{
    win32_window_dimension Result;

    RECT ClientRect;
    GetClientRect(hWnd, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;

    return Result;
}

internal_function_static void Win32RenderGradient(
    win32_offscreen_buffer* Buffer, int BlueXOffset, int GreenYOffset)
{
    int Width = Buffer->Width;
    int Height = Buffer->Height;

    uint8_t* Row = (uint8_t*)Buffer->Memory;
    for (int Y = 0; Y < Height; ++Y)
    {
        uint32_t* Pixel = (uint32_t*)Row;
        for (int X = 0; X < Width; ++X)
        {
            uint8_t Blue = (X + BlueXOffset);
            uint8_t Green = (Y + GreenYOffset);

            *Pixel++ = ((Green << 8 | Blue));
        }

        Row += Buffer->Stride;
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

    // Pixels are always 32-bit wide. Memory order BB GG RR XX
    int BytesPerPixel = 4;

    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;

    int BitmapMemorySize =
        (Buffer->Width * Buffer->Height) * BytesPerPixel;

    Buffer->Memory = VirtualAlloc(0, BitmapMemorySize,
        MEM_COMMIT, PAGE_READWRITE);

    // Stride is an uint8_t because that is what we need for row iteration
    Buffer->Stride = Width * BytesPerPixel;
}

internal_function_static void Win32RenderWindow(
    win32_offscreen_buffer* Buffer,
    HDC Hdc, int WindowWidth, int WindowHeight)
{
    StretchDIBits(
        Hdc,
        /*
        X, Y, Width, Height,
        X, Y, Width, Height,
        */
        0, 0, WindowWidth, WindowHeight,
        0, 0, Buffer->Width, Buffer->Height,
        Buffer->Memory,
        &Buffer->Info,
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
            GlobalRunning = false;
        } break;

        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;

        case WM_DESTROY:
        {
            GlobalRunning = false;
        } break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            uint32_t VKCode = wParam;
            #define KeyMessageWasDownBit (1 << 30)
            #define KeyMessageIsDownBit (1 << 31)
            bool WasDown = ((lParam & KeyMessageWasDownBit) != 0);
            bool IsDown = ((lParam & KeyMessageIsDownBit) == 0);
            if (WasDown != IsDown)
            {
                if (VKCode == 'W')
                {

                }
                else if (VKCode == 'A')
                {

                }
                else if (VKCode == 'S')
                {

                }
                else if (VKCode == 'D')
                {

                }
                else if (VKCode == 'Q')
                {

                }
                else if (VKCode == 'E')
                {

                }
                else if (VKCode == VK_UP)
                {

                }
                else if (VKCode == VK_LEFT)
                {

                }
                else if (VKCode == VK_DOWN)
                {

                }
                else if (VKCode == VK_RIGHT)
                {

                }
                else if (VKCode == VK_ESCAPE)
                {
                    OutputDebugStringA("ESCAPE: ");
                    if (IsDown)
                    {
                        OutputDebugStringA("IsDown");
                    }
                    if (WasDown)
                    {
                        OutputDebugStringA("WasDown");
                    }
                    OutputDebugStringA("\n");
                }
                else if (VKCode == VK_SPACE)
                {

                }
            }
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
                &GlobalBackBuffer, DeviceContext,
                Dimension.Width, Dimension.Height);

            EndPaint(hWnd, &PaintStruct);
        } break;

        default:
        {
            // OutputDebugStringA("DefWindowProcW\n");
            Result = DefWindowProcW(hWnd,uMsg,wParam,lParam);
        } break;
    }
    return Result;
}

INT CALLBACK WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ PSTR lpCmdLine,
    _In_ INT nCmdShow)
{
    Win32LoadXInput();

    WNDCLASS WindowClass = {};

    // GlobalBackBuffer gets allocated on heap
    Win32ResizeDIBSection(&GlobalBackBuffer, 1280, 720);

    // complete window redraw; no shared dc;
    WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
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
            // We have specified CS_OWNDC, so are not sharing it an
            // can just get one and keep using it, instead of calling
            // GetDC() every time we are using device context.
            HDC DeviceContext = GetDC(hWnd);

            int XOffset = 0;
            int YOffset = 0;

            GlobalRunning = true;
            while (GlobalRunning)
            {
                MSG Message;
                while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
                {
                    // Check if windows sends us WM_Quit.
                    if (Message.message == WM_QUIT)
                    {
                        GlobalRunning = false;
                    }
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }
                
                for (DWORD ControllerIndex = 0; ControllerIndex < XUSER_MAX_COUNT; ControllerIndex++)
                {
                    XINPUT_STATE ControllerState;
                    ZeroMemory(&ControllerState, sizeof(XINPUT_STATE));

                    if (XInputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS)
                    {
                        // Controller is connected
                        XINPUT_GAMEPAD* GamePad = &ControllerState.Gamepad;

                        bool Up = (GamePad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
                        bool Down = (GamePad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
                        bool Left = (GamePad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                        bool Right = (GamePad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
                        bool Start = (GamePad->wButtons & XINPUT_GAMEPAD_START);
                        bool Back = (GamePad->wButtons & XINPUT_GAMEPAD_BACK);
                        bool LeftShoulder = (GamePad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
                        bool RightShoulder = (GamePad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
                        bool AButton = (GamePad->wButtons & XINPUT_GAMEPAD_A);
                        bool BButton = (GamePad->wButtons & XINPUT_GAMEPAD_B);
                        bool CButton = (GamePad->wButtons & XINPUT_GAMEPAD_X);
                        bool DButton = (GamePad->wButtons & XINPUT_GAMEPAD_Y);

                        int16_t ThumbStickX = GamePad->sThumbLX;
                        int16_t ThumbStickY = GamePad->sThumbLY;

                        // move vertical or horizontal
                        if (Up)
                        {
                            --YOffset;
                        }

                        if (Down)
                        {
                            ++YOffset;
                        }

                        if (Left)
                        {
                            --XOffset;
                        }

                        if (Right)
                        {
                            ++XOffset;
                        }

                        // move diagonal
                        if (Up + Left)
                        {
                            --YOffset;
                            --XOffset;
                        }

                        if (Up + Right)
                        {
                            --YOffset;
                            ++XOffset;
                        }

                        if (Down + Left)
                        {
                            ++YOffset;
                            --XOffset;
                        }

                        if (Down + Right)
                        {
                            ++YOffset;
                            ++XOffset;
                        }

                        // vibration
                        if (AButton)
                        {
                            XINPUT_VIBRATION Vibration;
                            Vibration.wLeftMotorSpeed = 60000;
                            Vibration.wRightMotorSpeed = 60000;
                            XInputSetState(0, &Vibration);
                        }

                        if (BButton)
                        {
                            XINPUT_VIBRATION Vibration;
                            Vibration.wLeftMotorSpeed = 0;
                            Vibration.wRightMotorSpeed = 0;
                            XInputSetState(0, &Vibration);
                        }
                    }
                    else
                    {
                        // Controller is not connected
                    }
                }
                
                Win32RenderGradient(&GlobalBackBuffer, XOffset, YOffset);

                win32_window_dimension Dimension = Win32GetWindowDimension(hWnd);
                Win32RenderWindow(
                    &GlobalBackBuffer, DeviceContext,
                    Dimension.Width, Dimension.Height);

                ReleaseDC(hWnd, DeviceContext);
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
