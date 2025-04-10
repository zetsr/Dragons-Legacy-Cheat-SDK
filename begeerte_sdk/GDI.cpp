#include "GDI.h"
#include <algorithm>

namespace GDI {
    Renderer::Renderer() : m_hwnd(nullptr), m_hdc(nullptr), m_memDC(nullptr), m_memBitmap(nullptr),
        m_screenWidth(0), m_screenHeight(0) {
        Gdiplus::GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, nullptr);
    }

    Renderer::~Renderer() {
        if (m_memBitmap) DeleteObject(m_memBitmap);
        if (m_memDC) DeleteDC(m_memDC);
        if (m_hdc) ReleaseDC(m_hwnd, m_hdc);
        if (m_hwnd) DestroyWindow(m_hwnd);
        Gdiplus::GdiplusShutdown(m_gdiplusToken);
    }

    void Renderer::Initialize() {
        m_screenWidth = GetSystemMetrics(SM_CXSCREEN);
        m_screenHeight = GetSystemMetrics(SM_CYSCREEN);
        CreateOverlayWindow();

        m_hdc = GetDC(m_hwnd);
        m_memDC = CreateCompatibleDC(m_hdc);
        m_memBitmap = CreateCompatibleBitmap(m_hdc, m_screenWidth, m_screenHeight);
        SelectObject(m_memDC, m_memBitmap);
    }

    void Renderer::CreateOverlayWindow() {
        WNDCLASSEX wc = { 0 };
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WndProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpszClassName = L"OverlayWindow";
        RegisterClassEx(&wc);

        m_hwnd = CreateWindowEx(
            WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
            L"OverlayWindow",
            L"Begeerte",
            WS_POPUP,
            0, 0, m_screenWidth, m_screenHeight,
            nullptr, nullptr, GetModuleHandle(nullptr), nullptr
        );

        SetLayeredWindowAttributes(m_hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
        ShowWindow(m_hwnd, SW_SHOW);
        UpdateWindow(m_hwnd);
    }

    void Renderer::BeginRender() {
        HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
        RECT rect = { 0, 0, m_screenWidth, m_screenHeight };
        FillRect(m_memDC, &rect, brush);
        DeleteObject(brush);
    }

    void Renderer::EndRender() {
        BitBlt(m_hdc, 0, 0, m_screenWidth, m_screenHeight, m_memDC, 0, 0, SRCCOPY);
    }

    void Renderer::DrawText(const wchar_t* text, float x, float y, Gdiplus::Color color) {
        Gdiplus::Graphics graphics(m_memDC);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeNone);
        graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintSingleBitPerPixel);
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeNone);

        Gdiplus::Font font(L"Verdana", 10, Gdiplus::FontStyleBold);
        Gdiplus::SolidBrush brush(color);
        Gdiplus::PointF point(x, y);
        graphics.DrawString(text, -1, &font, point, &brush);
    }

    void Renderer::DrawRect(float x, float y, float width, float height, Gdiplus::Color color) {
        Gdiplus::Graphics graphics(m_memDC);
        Gdiplus::SolidBrush brush(color);
        Gdiplus::RectF rect(x, y, width, height);
        graphics.FillRectangle(&brush, rect);
    }

    void Renderer::DrawPolygon(const Gdiplus::PointF* points, int count, Gdiplus::Color color) {
        Gdiplus::Graphics graphics(m_memDC);
        Gdiplus::SolidBrush brush(color);
        graphics.FillPolygon(&brush, points, count);
    }

    LRESULT CALLBACK Renderer::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
        return 0;
    }
}