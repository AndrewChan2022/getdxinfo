// #define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d3d11.h>
#include <dxgi.h>

#include <iostream>
#include <vector>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

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

int main()
{
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
        nullptr,                    // default adapter
        D3D_DRIVER_TYPE_HARDWARE,
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
    // Cleanup
    // ------------------------------------------------------------
    context->Release();
    device->Release();

    return 0;
}
