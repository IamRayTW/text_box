#ifndef _RUSER_INTERFACE_H_
#define _RUSER_INTERFACE_H_

#include <windows.h>
#include <GL/gl.h>
#include <cstdio>

class RUserInterface
{
public:
    RUserInterface()
    {
        int width =640;
        int height = 480;
        WNDCLASS wc;
        wc.style = CS_OWNDC;
        wc.lpfnWndProc = StaticWindowProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = GetModuleHandle(NULL);
        wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
        wc.hCursor = LoadCursor (NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
        wc.lpszMenuName = NULL;
        wc.lpszClassName = "RUI";
        RegisterClass (&wc);

        hWnd_ = CreateWindowA (
            "RUI", "Ray UI", 
            //WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,
            WS_VISIBLE | WS_POPUPWINDOW | WS_OVERLAPPEDWINDOW | WS_MAXIMIZE,
            CW_USEDEFAULT , CW_USEDEFAULT , width, height,
            NULL, NULL, wc.hInstance, NULL);

        SetWindowLong(hWnd_, GWLP_USERDATA, (long)this);
        EnableOpenGL (hWnd_, &hDC, &hRC_);
    }

    ~RUserInterface()
    {
        DisableOpenGL(hWnd_, hDC, hRC_);
        DestroyWindow(hWnd_);
    }

    int RunUI(unsigned int width, unsigned int height)
    {
        BOOL bQuit = FALSE;
        MSG msg;
        int result = 0;

        while (!bQuit)
        {
            if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                {
                    bQuit = TRUE;
                }
                else
                {
                    TranslateMessage (&msg);
                    DispatchMessage (&msg);
                }
            }

            result = Work();

            if (0 == result)
            {
                wglMakeCurrent(hDC, hRC_);
                SwapBuffers(hDC);
                //wglMakeCurrent(NULL, NULL);
            }
        }

        return static_cast<int>(msg.wParam);
    }

    int WT(unsigned int width, unsigned int height)
    {
        if (FALSE == SetWindowPos(
            hWnd_, 0, 0, 0, width+3, height+25, SWP_NOMOVE))
        {
            return -1;
        }

        RECT rc;
        SetRect(&rc, 0, 0, width, height);
        InvalidateRect(hWnd_, &rc, FALSE);

        return 0;
    }

    virtual int Work() = 0;

    enum MouseState
    {
    };

    virtual int OnResize()
    {
        return 0;
    }

    virtual int OnKey(unsigned int key, unsigned int state)
    {
        return 0;
    }

    virtual int OnMouse(short x, short y, unsigned char is_down, int bits)
    {
        return 0;
    }

    virtual int OnResize(unsigned short width, unsigned short height)
    {
        return 0;
    }

    void GetWidthHeight(unsigned int& width, unsigned int& height)
    {
        RECT rect;
        GetClientRect(hWnd_, &rect);
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;
    }

private:
    static LRESULT CALLBACK StaticWindowProc(
        HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        RUserInterface* object = 
            (RUserInterface*)GetWindowLong(hWnd, GWLP_USERDATA);

        if (NULL == object)
            return DefWindowProc(hWnd, message, wParam, lParam);

        return object->WindowProc(hWnd, message, wParam, lParam);

    }

    LRESULT CALLBACK WindowProc(
        HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_KEYDOWN:
        case WM_CHAR:
            {
                int tt = 0;
                if (message == WM_KEYDOWN)
                {
                    tt = 1;
                    if (   (35 <= wParam && wParam <= 40)
                        || 46 == wParam) //del
                    {
                        OnKey(static_cast<int>(wParam), tt);
                    }
                }
                else
                    OnKey(static_cast<int>(wParam), tt);
            }
            break;
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONDOWN:
            {
                SetCapture(hWnd);
                unsigned short x = static_cast<unsigned short>(lParam & 0xffff);
                unsigned short y = static_cast<unsigned short>(lParam >> 16);
                OnMouse(x, y, 1, static_cast<int>(wParam));
            }
            break;
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
        case WM_LBUTTONUP:
            {
                ReleaseCapture();
                unsigned short x = static_cast<unsigned short>(lParam & 0xffff);
                unsigned short y = static_cast<unsigned short>(lParam >> 16);
                OnMouse(x, y, 0, static_cast<int>(wParam));
            }
            break;
        case WM_MOUSEMOVE:
            {
                unsigned short x = static_cast<unsigned short>(lParam & 0xffff);
                unsigned short y = static_cast<unsigned short>(lParam >> 16);
                OnMouse(x, y, 2, static_cast<int>(wParam));
            }
            break;
        case WM_SIZE:
            {
                unsigned short width = static_cast<unsigned short>(lParam & 0xffff);
                unsigned short height = static_cast<unsigned short>(lParam >> 16);
                OnResize(width, height);
            }
            break;
        case WM_CLOSE:
            PostQuitMessage (0);
            return 0;
        case WM_DESTROY:
            return 0;
        default:
            return DefWindowProc (hWnd, message, wParam, lParam);
        }

        return DefWindowProc (hWnd, message, wParam, lParam);
    }

    void EnableOpenGL(HWND hWnd, HDC* hDC, HGLRC* hRC_)
    {
        PIXELFORMATDESCRIPTOR pfd;
        int iFormat;
        *hDC = GetDC(hWnd);
        ZeroMemory(&pfd, sizeof(pfd));
        pfd.nSize = sizeof(pfd); 
        pfd.nVersion = 1;
        pfd.dwFlags = 
            PFD_DRAW_TO_WINDOW | 
            PFD_SUPPORT_OPENGL |
            PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 24;
        pfd.cDepthBits = 16;
        pfd.iLayerType = PFD_MAIN_PLANE;
        iFormat = ChoosePixelFormat(*hDC, &pfd);
        SetPixelFormat(*hDC, iFormat, &pfd);
        *hRC_ = wglCreateContext(*hDC);
    }

    void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC_)
    {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(hRC_);
        ReleaseDC(hWnd, hDC);
    }

private:
    HWND hWnd_;
    HDC hDC;
    HGLRC hRC_;
};

#endif //_RUSER_INTERFACE_H_