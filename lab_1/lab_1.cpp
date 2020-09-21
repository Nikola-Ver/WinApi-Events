#include <Windows.h>

#define SPEED 1
#define HEIGHT 64
#define WIDTH 64
#define TIMER_ID 1

RECT rcSize;
HDC hdcBackBuffer, hdcSprite;
int spriteX = 0, spriteY = 0;
int width = 0, height = 0;
int coeffSpeed = 16;
bool up = false, down = false, right = false, left = false;
bool isPicture = false;
void ResizeWnd(HWND hWnd);
void SpriteMovement(HWND hWnd)
{
    InvalidateRect(hWnd, &rcSize, true);
    spriteX += 10;
}

void ResizeWnd(HWND hWnd)
{
    HDC hdcWindow = GetDC(hWnd);    

    GetClientRect(hWnd, &rcSize);
    InvalidateRect(hWnd, &rcSize, true);
    width = rcSize.right - rcSize.left;
    height = rcSize.bottom - rcSize.top;
    if (spriteX > width - WIDTH)
        spriteX = width - WIDTH;
    if (spriteY > height - HEIGHT)
        spriteY = height - HEIGHT;

    if (hdcBackBuffer) DeleteDC(hdcBackBuffer);
    hdcBackBuffer = CreateCompatibleDC(hdcWindow);
    HBITMAP hbmBackBuffer = CreateCompatibleBitmap(hdcBackBuffer, rcSize.right - rcSize.left, rcSize.bottom - rcSize.top);
    SelectObject(hdcBackBuffer, hbmBackBuffer);
    DeleteObject(hbmBackBuffer);

    if (hdcSprite) DeleteDC(hdcSprite);
    hdcSprite = CreateCompatibleDC(hdcWindow);
    HBITMAP hbmSprite;
    if (isPicture)
        hbmSprite = (HBITMAP)LoadImage(NULL, L"image.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    else hbmSprite = CreateCompatibleBitmap(hdcSprite, WIDTH, HEIGHT);
    SelectObject(hdcSprite, hbmSprite);
    DeleteObject(hbmSprite);
    if (!isPicture)
    {
        RECT rcSprite;
        SetRect(&rcSprite, 0, 0, WIDTH, HEIGHT);
        FillRect(hdcSprite, &rcSprite, (HBRUSH)GetStockObject(WHITE_BRUSH));
    }

    ReleaseDC(hWnd, hdcWindow);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static PAINTSTRUCT ps;

    switch (msg)
    {
        case WM_TIMER:
        {
            SpriteMovement(hWnd);
            return 0;
        }
        case WM_CREATE:
        {
            SetTimer(hWnd, TIMER_ID, 100, NULL);
            return 0;
        }
        case WM_MOUSEWHEEL:
        {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (delta > 0) delta = 1;
            else delta = -1;
            if (GET_KEYSTATE_WPARAM(wParam) != MK_SHIFT)
            {
                spriteY += delta * SPEED * coeffSpeed;
                if (spriteY < 0)
                    spriteY = 0;
                if (spriteY > height - HEIGHT)
                    spriteY = height - HEIGHT;
            }
            else 
            {
                spriteX += delta * SPEED * coeffSpeed;
                if (spriteX < 0 )
                    spriteX = 0;
                if (spriteX > width - WIDTH)
                    spriteX = width - WIDTH;
            }
            return 0;
        }
        case WM_SIZE:
        {
            ResizeWnd(hWnd);
            return 0;
        }
        case WM_KEYUP:
        {
            switch (wParam)
            {
                case 65:
                    left = false;
                    break;
                case 68:
                    right = false;
                    break;
                case 87:
                    up = false;
                    break;
                case 83:
                    down = false;
                    break;
            }
            return 0;
        }
        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case 65:
                    left = true;
                    break;
                case 68:
                    right = true;
                    break;
                case 87:
                    up = true;
                    break;
                case 83:
                    down = true;
                    break;
                case 187:
                    coeffSpeed *= 2;
                    break;
                case 189:
                    if (coeffSpeed >= 2)
                        coeffSpeed /= 2;
                    break;
                case 67:
                    isPicture = !isPicture;
                    ResizeWnd(hWnd);
                    break;
            }

            if (left)
                if (spriteX >= SPEED * coeffSpeed)
                    spriteX -= SPEED * coeffSpeed;
                else
                {
                    left = false;
                    spriteX = 0;
                }
            if (right)
                if (spriteX + SPEED * coeffSpeed <= width - WIDTH)
                    spriteX += SPEED * coeffSpeed;
                else 
                { 
                    right = false;
                    spriteX = width - WIDTH; 
                }
            if (up)
                if (spriteY >= SPEED * coeffSpeed)
                    spriteY -= SPEED * coeffSpeed;
                else
                {
                    up = false;
                    spriteY = 0;
                }
            if (down)
                if (spriteY + SPEED * coeffSpeed <= height - HEIGHT)
                    spriteY += SPEED * coeffSpeed;
                else
                {
                    down = false;
                    spriteY = height - HEIGHT;
                }
            return 0;
        }
        case WM_GETMINMAXINFO:
        {
            LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
            lpMMI->ptMinTrackSize.x = WIDTH + 20;
            lpMMI->ptMinTrackSize.y = HEIGHT + 40;
            return 0;
        }
        case WM_PAINT:
        {
            BeginPaint(hWnd, &ps);
            FillRect(hdcBackBuffer, &rcSize, (HBRUSH)GetStockObject(BLACK_BRUSH));
            BitBlt(hdcBackBuffer, spriteX, spriteY, WIDTH, HEIGHT, hdcSprite, 0, 0, SRCCOPY);
            BitBlt(ps.hdc, 0, 0, rcSize.right - rcSize.left, rcSize.bottom - rcSize.top, hdcBackBuffer, 0, 0, SRCCOPY);
            EndPaint(hWnd, &ps);
            return 0;
        }
        case WM_DESTROY:
        {
            KillTimer(hWnd, TIMER_ID);
            PostQuitMessage(0);
            return 0;
        }
        default:
        {
            return DefWindowProc(hWnd, msg, wParam, lParam);
        }
    }
}

int WINAPI WinMain(HINSTANCE hPrevInstance, HINSTANCE hInstance, LPSTR lpCmdLine, int nShowCmd)
{
    static TCHAR className[] = TEXT("GameClass");
    static TCHAR windowName[] = TEXT("WinApi");

    WNDCLASSEX wcex;

    wcex.cbClsExtra = 0;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.cbWndExtra = 0;
    wcex.hbrBackground = NULL;
    wcex.hCursor = LoadCursor(hInstance, IDC_ARROW);
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hIconSm = NULL;
    wcex.hInstance = hInstance;
    wcex.lpfnWndProc = WndProc;
    wcex.lpszClassName = className;
    wcex.lpszMenuName = NULL;
    wcex.style = 0;

    if (!RegisterClassEx(&wcex))
        return 0;

    HWND hWnd = CreateWindow(className, windowName, WS_OVERLAPPEDWINDOW, 0, 0, 800, 800, NULL, NULL, hInstance, NULL);
    if (!hWnd)
        return 0;

    ShowWindow(hWnd, nShowCmd);
    UpdateWindow(hWnd);

    MSG msg;
    while(true)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                break;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        InvalidateRect(hWnd, NULL, FALSE);
    }

    return msg.wParam;
}