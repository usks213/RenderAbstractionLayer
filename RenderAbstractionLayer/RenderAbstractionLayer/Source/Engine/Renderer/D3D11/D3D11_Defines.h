/*****************************************************************//**
 * \file   D3D11_Defines.h
 * \brief  DirectX11’è‹`
 * 
 * \author USAMI KOSHI
 * \date   2021/10/04
 *********************************************************************/
#ifndef _D3D11_DEFINES_
#define _D3D11_DEFINES_

#include <comdef.h>
#include <Windows.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#define D3D11_VERSION_1

#ifdef D3D11_VERSION_1
#include <d3d11_1.h>
#endif
#ifdef D3D11_VERSION_2
#include <d3d11_2.h>
#endif
#ifdef D3D11_VERSION_3
#include <d3d11_3.h>
#endif
#ifdef D3D11_VERSION_4
#include <d3d11_4.h>
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

#endif // !_D3D11_DEFINES_

