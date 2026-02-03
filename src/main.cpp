// #define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d3d11.h>
#include <dxgi.h>

#include <GL/gl.h>

#include <iostream>
#include <vector>


#pragma comment(lib, "opengl32.lib")

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

void QueryOpenGL()
{
    // 1. Register dummy window class
    WNDCLASS wc = {};
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = "DummyGL";
    RegisterClass(&wc);

    // 2. Create hidden window
    HWND hwnd = CreateWindowA(wc.lpszClassName, "DummyGL",
                              0, 0, 0, 1, 1, nullptr, nullptr,
                              wc.hInstance, nullptr);
    if (!hwnd)
    {
        std::cout << "Failed to create dummy window for OpenGL\n";
        return;
    }

    HDC hdc = GetDC(hwnd);

    // 3. Set pixel format
    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    int pf = ChoosePixelFormat(hdc, &pfd);
    if (pf == 0 || !SetPixelFormat(hdc, pf, &pfd))
    {
        std::cout << "Failed to set pixel format\n";
        ReleaseDC(hwnd, hdc);
        DestroyWindow(hwnd);
        return;
    }

    // 4. Create GL context
    HGLRC glrc = wglCreateContext(hdc);
    if (!glrc || !wglMakeCurrent(hdc, glrc))
    {
        std::cout << "Failed to create OpenGL context\n";
        if (glrc) wglDeleteContext(glrc);
        ReleaseDC(hwnd, hdc);
        DestroyWindow(hwnd);
        return;
    }

    // 5. Query OpenGL version
    const GLubyte* version = glGetString(GL_VERSION);
    //const GLubyte* glslVer = glGetString(GL_SHADING_LANGUAGE_VERSION);

    std::cout << "==================================\n";
    std::cout << "OpenGL Version: " << (version ? (const char*)version : "Unknown") << "\n";
    //std::cout << "GLSL Version: " << (glslVer ? (const char*)glslVer : "Unknown") << "\n";

    // 6. Cleanup
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(glrc);
    ReleaseDC(hwnd, hdc);
    DestroyWindow(hwnd);
}




static const char* FeatureLevelToString(D3D_FEATURE_LEVEL fl)
{
    switch (fl)
    {
    case D3D_FEATURE_LEVEL_9_1:  return "9_1";
    case D3D_FEATURE_LEVEL_9_2:  return "9_2";
    case D3D_FEATURE_LEVEL_9_3:  return "9_3";
    case D3D_FEATURE_LEVEL_10_0: return "10_0";
    case D3D_FEATURE_LEVEL_10_1: return "10_1";
    case D3D_FEATURE_LEVEL_11_0: return "11_0";
    case D3D_FEATURE_LEVEL_11_1: return "11_1";
    default:                     return "Unknown";
    }
}

int QueryOneDXDevice(IDXGIAdapter* adapter) {
    // ------------------------------------------------------------
    // 2. Create DX11 device (feature-level probe)
    // ------------------------------------------------------------
    D3D_FEATURE_LEVEL requestedLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    D3D_FEATURE_LEVEL obtainedLevel = D3D_FEATURE_LEVEL_9_1;

    HRESULT hr = D3D11CreateDevice(
        adapter,                    // default adapter
        D3D_DRIVER_TYPE_UNKNOWN,
        nullptr,
        0,                           // flags
        requestedLevels,
        ARRAYSIZE(requestedLevels),
        D3D11_SDK_VERSION,
        &device,
        &obtainedLevel,
        &context
    );

    if (FAILED(hr))
    {
        std::cout << "Failed to create DX11 device (hr=0x"
                  << std::hex << hr << ")\n";

        // Fallback to software
        hr = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_WARP,
            nullptr,
            0,
            requestedLevels,
            ARRAYSIZE(requestedLevels),
            D3D11_SDK_VERSION,
            &device,
            &obtainedLevel,
            &context
        );

        if (SUCCEEDED(hr))
            std::cout << "Using WARP software device\n";
        else
            return 0;
    }

    std::cout << "DX11 device created\n";
    std::cout << "Feature level: " << FeatureLevelToString(obtainedLevel) << "\n";


    // ------------------------------------------------------------
    // print all level support
    // locating the array index in the requestedLevels
    // then print all from this index to end 
    // ------------------------------------------------------------
    int startIndex = -1;
    for (size_t i = 0; i < ARRAYSIZE(requestedLevels); ++i)
    {
        if (requestedLevels[i] == obtainedLevel)
        {
            startIndex = static_cast<int>(i);
            break;
        }
    }

    if (startIndex >= 0)
    {
        std::cout << "All supported feature levels:\n";
        for (size_t i = startIndex; i < ARRAYSIZE(requestedLevels); ++i)
        {
            std::cout << "  " << FeatureLevelToString(requestedLevels[i]) << "\n";
        }
    }
    else
    {
        std::cout << "Warning: obtained feature level not found in requestedLevels array\n";
    }



    // ------------------------------------------------------------
    // 3. Query adapter & driver info
    // ------------------------------------------------------------
    IDXGIDevice* dxgiDevice = nullptr;
    if (SUCCEEDED(device->QueryInterface(__uuidof(IDXGIDevice),
                                         (void**)&dxgiDevice)))
    {
        IDXGIAdapter* adapter = nullptr;
        if (SUCCEEDED(dxgiDevice->GetAdapter(&adapter)))
        {
            DXGI_ADAPTER_DESC desc;
            if (SUCCEEDED(adapter->GetDesc(&desc)))
            {
                std::wcout << L"Adapter: " << desc.Description << L"\n";
                std::wcout << L"Video memory: "
                           << (desc.DedicatedVideoMemory / (1024 * 1024))
                           << L" MB\n";
            }
            adapter->Release();
        }
        dxgiDevice->Release();
    }

    // ------------------------------------------------------------
    // 3. Cleanup
    // ------------------------------------------------------------
    context->Release();
    device->Release();

    return 0;
}

int QueryDX() {
    // ------------------------------------------------------------
    // 1. Runtime check: is DX11 present?
    // ------------------------------------------------------------
    HMODULE d3d11 = LoadLibraryA("d3d11.dll");
    if (!d3d11)
    {
        std::cout << "DX11 runtime not present\n";
        return 0;
    }

    std::cout << "DX11 runtime found\n";

    // ------------------------------------------------------------
    // 2. list all adapter
    // ------------------------------------------------------------
    IDXGIFactory* factory = nullptr;
    if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory)))
    {
        std::cout << "Failed to create DXGI factory\n";
        return 1;
    }

    UINT index = 0;
    IDXGIAdapter* adapter = nullptr;
    while (factory->EnumAdapters(index, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC desc;
        adapter->GetDesc(&desc);

        std::wcout << L"Adapter " << index << L": " << desc.Description << L"\n";
        std::wcout << L"  VendorId: " << desc.VendorId 
                   << L", DeviceId: " << desc.DeviceId << L"\n";
        std::wcout << L"  Dedicated Video Memory: " 
                   << (desc.DedicatedVideoMemory / (1024*1024)) << L" MB\n";
        std::wcout << L"  Shared System Memory: " 
                   << (desc.SharedSystemMemory / (1024*1024)) << L" MB\n";

        //
        QueryOneDXDevice(adapter);

    }

    return 0;
}

int main()
{    
    QueryDX();

    QueryOpenGL();

    return 0;
}
