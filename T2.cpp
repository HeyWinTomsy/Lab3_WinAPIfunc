#include <windows.h>
#include <vector>

#define IDM_DRAW   1001
#define IDM_COLOR  1002
#define IDM_ZOOM   1003
#define IDM_ABOUT  1004
#define IDM_EXIT   1005

void Rectangle(HDC dc, int l, int t, int r, int b, COLORREF color)
{
    HBRUSH brush = CreateSolidBrush(color);
    HGDIOBJ old = SelectObject(dc, brush);
    Rectangle(dc, l, t, r, b);
    SelectObject(dc, old);
    DeleteObject(brush);
}

void Poly(HDC dc, const POINT* pts, int n, COLORREF color)
{
    HBRUSH brush = CreateSolidBrush(color);
    HGDIOBJ old = SelectObject(dc, brush);
    Polygon(dc, pts, n);
    SelectObject(dc, old);
    DeleteObject(brush);
}

class Cloud
{
public:
    void show(HDC dc, int X, int Y)
    {
        Ellipse(dc, X,      Y + 10, X + 50, Y + 35);
        Ellipse(dc, X + 20, Y,      X + 70, Y + 30);
        Ellipse(dc, X + 45, Y + 10, X + 95, Y + 35);
    }
};

class Lighthouse
{
    static int s(int v, double k) { return (int)(v * k); }
public:
    void show(HDC dc, int X, int Y, double k, COLORREF color)
    {
        POINT tower[4] = { {X - s(25, k), Y}, {X + s(25, k), Y},
                           {X + s(15, k), Y - s(140, k)}, {X - s(15, k), Y - s(140, k)} };
        Poly(dc, tower, 4, RGB(255, 255, 255));

        POINT stripe[4] = { {X - s(19, k), Y - s(60, k)}, {X + s(21, k), Y - s(90, k)},
                            {X + s(19, k), Y - s(120, k)}, {X - s(18, k), Y - s(90, k)} };
        Poly(dc, stripe, 4, color);

        Rectangle(dc, X - s(12, k), Y - s(165, k), X + s(12, k), Y - s(140, k), RGB(255, 240, 80));

        POINT roof[3] = { {X - s(16, k), Y - s(165, k)}, {X + s(16, k), Y - s(165, k)},
                          {X, Y - s(190, k)} };
        Poly(dc, roof, 3, color);
    }
};

class Ship
{
public:
    void show(HDC dc, int X, int Y)
    {
        Rectangle(dc, X - 12, Y - 70, X + 12, Y - 60, RGB(255, 215, 0));
        Rectangle(dc, X - 12, Y - 80, X + 12, Y - 70, RGB(0, 87, 153));

        Rectangle(dc, X + 12, Y - 60, X + 8,  Y - 40, RGB(70, 70, 70));
        Rectangle(dc, X + 24, Y - 75, X + 20, Y - 40, RGB(70, 70, 70));

        Rectangle(dc, X, Y - 40, X + 40, Y - 15, RGB(150, 150, 150));

        POINT hull[4] = { {X - 100, Y - 15}, {X + 100, Y - 20}, {X + 90, Y}, {X - 100, Y} };
        Poly(dc, hull, 4, RGB(70, 70, 70));
    }
};

class Shore
{
public:
    void show(HDC dc, int X, int Y)
    {
        POINT hull[4] = { {X - 80, Y - 15}, {X + 200, Y - 25},
                          {X + 200, Y + 250}, {X + 40, Y + 250} };
        Poly(dc, hull, 4, RGB(194, 178, 128));
    }
};

class Sea
{
public:
    void show(HDC dc, int X, int Y, int width, int height)
    {
        Rectangle(dc, X - 1, Y, X + width + 1, Y + height + 1, RGB(0, 90, 200));
    }
};

int  g_W = 900, g_H = 600;
int  g_shipX = 200;
std::vector<POINT> g_clouds;

bool     g_showImage = false;            // чи показувати рисунок
double   g_scale     = 1.0;              // масштаб маяка
const COLORREF g_palette[] = {           // кольори для "Зміна кольору"
    RGB(220, 30, 30), RGB(30, 160, 60), RGB(150, 40, 190), RGB(255, 140, 0)
};
int      g_colorIdx  = 0;

void DrawScene(HDC dc)
{
    int horizon = g_H * 60 / 100;

    Rectangle(dc, -1, -1, g_W + 1, horizon, RGB(135, 206, 250));

    Cloud cloud;
    for (size_t i = 0; i < g_clouds.size(); i++)
        cloud.show(dc, g_clouds[i].x, g_clouds[i].y);

    Sea sea;
    sea.show(dc, 0, horizon, g_W, g_H - horizon);

    Shore shore;
    shore.show(dc, g_W - 160, horizon);

    Lighthouse lighthouse;
    lighthouse.show(dc, g_W - 160, horizon + 10, g_scale, g_palette[g_colorIdx]);

    Ship ship;
    ship.show(dc, g_shipX, horizon + 70);
    ship.show(dc, g_shipX + 120, horizon + 130);
}

HMENU CreateMainMenu()
{
    HMENU bar = CreateMenu();

    HMENU mImage = CreatePopupMenu();
    AppendMenuW(mImage, MF_STRING, IDM_DRAW, L"Рисунок...");

    HMENU mTransform = CreatePopupMenu();
    AppendMenuW(mTransform, MF_STRING, IDM_COLOR, L"Зміна кольору");
    AppendMenuW(mTransform, MF_STRING, IDM_ZOOM,  L"Збільшення");

    HMENU mInfo = CreatePopupMenu();
    AppendMenuW(mInfo, MF_STRING,    IDM_ABOUT, L"Про програму");
    AppendMenuW(mInfo, MF_SEPARATOR, 0,         nullptr);
    AppendMenuW(mInfo, MF_STRING,    IDM_EXIT,  L"Вихід");

    AppendMenuW(bar, MF_POPUP, (UINT_PTR)mImage,     L"Зображення");
    AppendMenuW(bar, MF_POPUP, (UINT_PTR)mTransform, L"Трансформації");
    AppendMenuW(bar, MF_POPUP, (UINT_PTR)mInfo,      L"Інформація");
    return bar;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        SetMenu(hwnd, CreateMainMenu());

        POINT c1 = { 60, 50 }, c2 = { 330, 90 }, c3 = { 600, 40 };
        g_clouds.push_back(c1);
        g_clouds.push_back(c2);
        g_clouds.push_back(c3);
        return 0;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDM_DRAW:                       // Зображення.Рисунок...
            g_showImage = true;
            g_scale = 1.0;
            InvalidateRect(hwnd, nullptr, TRUE);
            break;

        case IDM_COLOR:                      // Трансформації.Зміна кольору
            g_colorIdx = (g_colorIdx + 1) % (int)(sizeof(g_palette) / sizeof(g_palette[0]));
            InvalidateRect(hwnd, nullptr, TRUE);
            break;

        case IDM_ZOOM:                       // Трансформації.Збільшення
            if (g_scale < 2.5)
                g_scale *= 1.2;
            InvalidateRect(hwnd, nullptr, TRUE);
            break;

        case IDM_ABOUT:                      // Інформація.Про програму
            MessageBoxW(hwnd,
                L"Лабораторна робота №3\nМаяк, кораблі, хмари, море\n"
                L"Варіант 2: дія \"Збільшення\"",
                L"Про програму", MB_OK | MB_ICONINFORMATION);
            break;

        case IDM_EXIT:                       // Інформація.Вихід
            DestroyWindow(hwnd);
            break;

        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
        }
        return 0;

    case WM_SIZE:
        g_W = LOWORD(lParam);
        g_H = HIWORD(lParam);
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        if (g_showImage)
            DrawScene(hdc);
        else
        {
            RECT rc;
            GetClientRect(hwnd, &rc);
            FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
        }
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_LBUTTONDOWN:                     // нова хмара в точці кліку
    {
        POINT p = { (short)LOWORD(lParam), (short)HIWORD(lParam) };
        g_clouds.push_back(p);
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;
    }

    case WM_KEYDOWN:
        if (wParam == VK_LEFT)   g_shipX -= 15;
        if (wParam == VK_RIGHT)  g_shipX += 15;
        if (wParam == VK_ESCAPE) DestroyWindow(hwnd);
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"LighthouseSeaApp";

    WNDCLASSW wc = {};
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = CLASS_NAME;
    if (!RegisterClassW(&wc)) return 0;

    HWND hwnd = CreateWindowExW(0, CLASS_NAME, L"Маяк, кораблі, хмари, море",
        WS_OVERLAPPEDWINDOW, 100, 100, 900, 600,
        nullptr, nullptr, hInstance, nullptr);
    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return (int)msg.wParam;
}