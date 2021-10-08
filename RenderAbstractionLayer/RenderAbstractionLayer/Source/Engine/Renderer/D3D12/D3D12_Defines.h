/*****************************************************************//**
 * \file   D3D12_Defines.h
 * \brief  DirectX12’è‹`
 * 
 * \author USAMI KOSHI
 * \date   2021/10/08
 *********************************************************************/
#ifndef _D3D12_DEFINES_
#define _D3D12_DEFINES_

#include <comdef.h>
#include <Windows.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#define D3D12_VERSION_0

#ifdef D3D12_VERSION_0
#include <d3d12.h>
#include <dxgi1_6.h>
#endif

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#define CHECK_FAILED(hr)													\
    if (FAILED(hr)) {                                                                                                                 \
        static char szResult[256];                                                                                                    \
        _com_error  err(hr);                                                                                                          \
        LPCTSTR     errMsg = err.ErrorMessage();                                                                                      \
        sprintf_s(szResult, sizeof(szResult), "[ERROR] HRESULT: %08X %s#L%d\n%s", static_cast<UINT>(hr), __FILE__, __LINE__, errMsg); \
        MessageBox(nullptr, szResult, "ERROR", MB_OK | MB_ICONERROR);                                                                 \
        throw std::exception(szResult);                                                                                               \
    }

#endif // !_D3D12_DEFINES_

