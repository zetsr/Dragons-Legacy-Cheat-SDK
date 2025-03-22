#pragma once
#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

namespace GDI {
    class Renderer {
    public:
        Renderer();
        ~Renderer();
        void Initialize();
        void BeginRender();
        void EndRender();
        void DrawText(const wchar_t* text, float x, float y, Gdiplus::Color color);
        void DrawRect(float x, float y, float width, float height, Gdiplus::Color color);
        void DrawPolygon(const Gdiplus::PointF* points, int count, Gdiplus::Color color);
        HWND GetWindowHandle() const { return m_hwnd; }
        HDC GetDeviceContext() const { return m_memDC; }

    private:
        void CreateOverlayWindow();
        static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

        HWND m_hwnd;
        HDC m_hdc;
        HDC m_memDC;
        HBITMAP m_memBitmap;
        Gdiplus::GdiplusStartupInput m_gdiplusStartupInput;
        ULONG_PTR m_gdiplusToken;
        int m_screenWidth;
        int m_screenHeight;
    };
}