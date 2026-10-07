#include <windows.h>
#include <string>
//ідентифікаія
#define ID_EDIT_X        101
#define ID_EDIT_Y        102
#define ID_EDIT_Z        103
#define ID_BUTTON        104
#define ID_STATIC_RESULT 105
//функія обробки поідомлень вікна
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
//головна функія
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    //опис класу
    const wchar_t CLASS_NAME[] = L"FunctionVar2Class";

    WNDCLASSEXW wc = {};
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = CLASS_NAME;
    if (!RegisterClassExW(&wc)) return 0;

    HWND hwnd = CreateWindowExW(0, CLASS_NAME,
        L"f(x, y, z) = x*(z+1) - 2*y*z",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        400, 250, 460, 300,
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
//обробка повідомлень
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hEditX, hEditY, hEditZ, hResult;

    switch (message)
    {
    case WM_CREATE:
    {
        HINSTANCE hInst = ((LPCREATESTRUCTW)lParam)->hInstance;
        DWORD editStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_RIGHT;

        CreateWindowExW(0, L"STATIC", L"Введіть значення x, y, z:",
            WS_CHILD | WS_VISIBLE, 30, 20, 300, 20, hwnd, nullptr, hInst, nullptr);

        CreateWindowExW(0, L"STATIC", L"x", WS_CHILD | WS_VISIBLE,
            30, 52, 15, 20, hwnd, nullptr, hInst, nullptr);
        hEditX = CreateWindowExW(0, L"EDIT", L"0", editStyle,
            45, 50, 80, 24, hwnd, (HMENU)ID_EDIT_X, hInst, nullptr);

        CreateWindowExW(0, L"STATIC", L"y", WS_CHILD | WS_VISIBLE,
            145, 52, 15, 20, hwnd, nullptr, hInst, nullptr);
        hEditY = CreateWindowExW(0, L"EDIT", L"0", editStyle,
            160, 50, 80, 24, hwnd, (HMENU)ID_EDIT_Y, hInst, nullptr);

        CreateWindowExW(0, L"STATIC", L"z", WS_CHILD | WS_VISIBLE,
            260, 52, 15, 20, hwnd, nullptr, hInst, nullptr);
        hEditZ = CreateWindowExW(0, L"EDIT", L"0", editStyle,
            275, 50, 80, 24, hwnd, (HMENU)ID_EDIT_Z, hInst, nullptr);

        CreateWindowExW(0, L"BUTTON", L"Розрахувати",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
            30, 100, 120, 30, hwnd, (HMENU)ID_BUTTON, hInst, nullptr);

        CreateWindowExW(0, L"STATIC", L"Результат:", WS_CHILD | WS_VISIBLE,
            30, 160, 80, 20, hwnd, nullptr, hInst, nullptr);
        hResult = CreateWindowExW(0, L"STATIC", L"0", WS_CHILD | WS_VISIBLE,
            115, 160, 280, 20, hwnd, (HMENU)ID_STATIC_RESULT, hInst, nullptr);

        // Системний шрифт для всіх дочірніх елементів
        EnumChildWindows(hwnd, [](HWND h, LPARAM) -> BOOL {
            SendMessageW(h, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);
            return TRUE;
        }, 0);
        return 0;
    }
    //обробка команди
    case WM_COMMAND:
        if (LOWORD(wParam) == ID_BUTTON)
        {
            wchar_t bx[64] = {}, by[64] = {}, bz[64] = {};
            GetWindowTextW(hEditX, bx, 64);
            GetWindowTextW(hEditY, by, 64);
            GetWindowTextW(hEditZ, bz, 64);

            try
            {
                double x = std::stod(bx);
                double y = std::stod(by);
                double z = std::stod(bz);

                double f = x * (z + 1) - 2 * y * z;

                wchar_t out[64];
                swprintf_s(out, L"%g", f);
                SetWindowTextW(hResult, out);
            }
            catch (...)
            {
                SetWindowTextW(hResult, L"Помилка введення");
            }
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}