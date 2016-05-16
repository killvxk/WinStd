/*
    Copyright 1991-2016 Amebis
    Copyright 2016 G�ANT

    This file is part of WinStd.

    Setup is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Setup is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Setup. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "Common.h"

#include <Windows.h>

#include <string>
#include <vector>


///
/// \defgroup WinStdWinAPI Windows API
/// Integrates WinStd classes with Microsoft Windows API
///
/// @{

///
/// Retrieves the fully qualified path for the file that contains the specified module and stores it in a std::string string.
///
/// \sa [GetModuleFileName function](https://msdn.microsoft.com/en-us/library/windows/desktop/ms683197.aspx)
///
inline DWORD GetModuleFileNameA(_In_opt_ HMODULE hModule, _Out_ std::string &sValue)
{
    CHAR szBuffer[WINSTD_STACK_BUFFER_BYTES/sizeof(CHAR)];

    // Try with stack buffer first.
    DWORD dwResult = ::GetModuleFileNameA(hModule, szBuffer, _countof(szBuffer));
    if (dwResult < _countof(szBuffer)) {
        // Copy from stack.
        sValue.assign(szBuffer, dwResult);
        return dwResult;
    } else {
        for (DWORD dwCapacity = 2*WINSTD_STACK_BUFFER_BYTES/sizeof(CHAR);; dwCapacity *= 2) {
            // Allocate on heap and retry.
            auto szBuffer = std::unique_ptr<CHAR[]>(new CHAR[dwCapacity]);
            dwResult = ::GetModuleFileNameA(hModule, szBuffer.get(), dwCapacity);
            if (dwResult < dwCapacity) {
                sValue.assign(szBuffer.get(), dwResult);
                return dwResult;
            }
        }
    }
}


///
/// Retrieves the fully qualified path for the file that contains the specified module and stores it in a std::wstring string.
///
/// \sa [GetModuleFileName function](https://msdn.microsoft.com/en-us/library/windows/desktop/ms683197.aspx)
///
inline DWORD GetModuleFileNameW(_In_opt_ HMODULE hModule, _Out_ std::wstring &sValue)
{
    WCHAR szBuffer[WINSTD_STACK_BUFFER_BYTES/sizeof(WCHAR)];

    // Try with stack buffer first.
    DWORD dwResult = ::GetModuleFileNameW(hModule, szBuffer, _countof(szBuffer));
    if (dwResult < _countof(szBuffer)) {
        // Copy from stack.
        sValue.assign(szBuffer, dwResult);
        return dwResult;
    } else {
        for (DWORD dwCapacity = 2*WINSTD_STACK_BUFFER_BYTES/sizeof(CHAR);; dwCapacity *= 2) {
            // Allocate on heap and retry.
            auto szBuffer = std::unique_ptr<WCHAR[]>(new WCHAR[dwCapacity]);
            dwResult = ::GetModuleFileNameW(hModule, szBuffer.get(), dwCapacity);
            if (dwResult < dwCapacity) {
                sValue.assign(szBuffer.get(), dwResult);
                return dwResult;
            }
        }
    }
}


///
/// Copies the text of the specified window's title bar (if it has one) into a std::string string.
///
/// \sa [GetWindowText function](https://msdn.microsoft.com/en-us/library/windows/desktop/ms633520.aspx)
///
inline int GetWindowTextA(_In_ HWND hWnd, _Out_ std::string &sValue)
{
    int iResult;

    // Query the final string length first.
    iResult = ::GetWindowTextLengthA(hWnd);
    if (iResult > 0) {
        if (++iResult < WINSTD_STACK_BUFFER_BYTES/sizeof(CHAR)) {
            // Read string data to stack.
            CHAR szBuffer[WINSTD_STACK_BUFFER_BYTES/sizeof(CHAR)];
            iResult = ::GetWindowTextA(hWnd, szBuffer, _countof(szBuffer));
            sValue.assign(szBuffer, iResult);
        } else {
            // Allocate buffer on heap and read the string data into it.
            auto szBuffer = std::unique_ptr<CHAR[]>(new CHAR[++iResult]);
            iResult = ::GetWindowTextA(hWnd, szBuffer.get(), iResult);
            sValue.assign(szBuffer.get(), iResult);
        }
        return iResult;
    }

    sValue.clear();
    return 0;
}


///
/// Copies the text of the specified window's title bar (if it has one) into a std::wstring string.
///
/// \sa [GetWindowText function](https://msdn.microsoft.com/en-us/library/windows/desktop/ms633520.aspx)
///
inline int GetWindowTextW(_In_ HWND hWnd, _Out_ std::wstring &sValue)
{
    int iResult;

    // Query the final string length first.
    iResult = ::GetWindowTextLengthW(hWnd);
    if (iResult > 0) {
        if (++iResult < WINSTD_STACK_BUFFER_BYTES/sizeof(WCHAR)) {
            // Read string data to stack.
            WCHAR szBuffer[WINSTD_STACK_BUFFER_BYTES/sizeof(WCHAR)];
            iResult = ::GetWindowTextW(hWnd, szBuffer, _countof(szBuffer));
            sValue.assign(szBuffer, iResult);
        } else {
            // Allocate buffer on heap and read the string data into it.
            auto szBuffer = std::unique_ptr<WCHAR[]>(new WCHAR[++iResult]);
            iResult = ::GetWindowTextW(hWnd, szBuffer.get(), iResult);
            sValue.assign(szBuffer.get(), iResult);
        }
        return iResult;
    }

    sValue.clear();
    return 0;
}


///
/// Retrieves version information for the specified file and stores it in a std::vector buffer.
///
/// \sa [GetFileVersionInfo function](https://msdn.microsoft.com/en-us/library/windows/desktop/ms647003.aspx)
///
inline BOOL GetFileVersionInfoA(_In_ LPCSTR lptstrFilename, __reserved DWORD dwHandle, _Out_ std::vector<unsigned char> &aValue)
{
    // Get version info size.
    DWORD dwVerInfoSize = ::GetFileVersionInfoSizeA(lptstrFilename, &dwHandle);
    if (dwVerInfoSize != 0) {
        // Read version info.
        aValue.resize(dwVerInfoSize);
        return ::GetFileVersionInfoA(lptstrFilename, dwHandle, dwVerInfoSize, aValue.data());
    } else
        return FALSE;
}


///
/// Retrieves version information for the specified file and stores it in a std::vector buffer.
///
/// \sa [GetFileVersionInfo function](https://msdn.microsoft.com/en-us/library/windows/desktop/ms647003.aspx)
///
inline BOOL GetFileVersionInfoW(_In_ LPCWSTR lptstrFilename, __reserved DWORD dwHandle, _Out_ std::vector<unsigned char> &aValue)
{
    // Get version info size.
    DWORD dwVerInfoSize = ::GetFileVersionInfoSizeW(lptstrFilename, &dwHandle);
    if (dwVerInfoSize != 0) {
        // Read version info.
        aValue.resize(dwVerInfoSize);
        return ::GetFileVersionInfoW(lptstrFilename, dwHandle, dwVerInfoSize, aValue.data());
    } else
        return FALSE;
}


///
/// Expands environment-variable strings, replaces them with the values defined for the current user, and stores it in a std::string string.
///
/// \sa [ExpandEnvironmentStrings function](https://msdn.microsoft.com/en-us/library/windows/desktop/ms724265.aspx)
///
inline DWORD ExpandEnvironmentStringsW(_In_ LPCSTR lpSrc, std::string &sValue)
{
    for (DWORD dwSizeOut = (DWORD)strlen(lpSrc) + 0x100;;) {
        DWORD dwSizeIn = dwSizeOut;
        auto szBuffer = std::unique_ptr<CHAR[]>(new CHAR[dwSizeIn + 2]); // Note: ANSI version requires one extra char.
        dwSizeOut = ::ExpandEnvironmentStringsA(lpSrc, szBuffer.get(), dwSizeIn);
        if (dwSizeOut == 0) {
            // Error.
            break;
        } else if (dwSizeOut <= dwSizeIn) {
            // The buffer was sufficient.
            sValue.assign(szBuffer.get(), dwSizeOut);
            return dwSizeOut;
        }
    }

    sValue.clear();
    return 0;
}


///
/// Expands environment-variable strings, replaces them with the values defined for the current user, and stores it in a std::wstring string.
///
/// \sa [ExpandEnvironmentStrings function](https://msdn.microsoft.com/en-us/library/windows/desktop/ms724265.aspx)
///
inline DWORD ExpandEnvironmentStringsW(_In_ LPCWSTR lpSrc, std::wstring &sValue)
{
    for (DWORD dwSizeOut = (DWORD)wcslen(lpSrc) + 0x100;;) {
        DWORD dwSizeIn = dwSizeOut;
        auto szBuffer = std::unique_ptr<WCHAR[]>(new WCHAR[dwSizeIn + 1]);
        dwSizeOut = ::ExpandEnvironmentStringsW(lpSrc, szBuffer.get(), dwSizeIn);
        if (dwSizeOut == 0) {
            // Error.
            break;
        } else if (dwSizeOut <= dwSizeIn) {
            // The buffer was sufficient.
            sValue.assign(szBuffer.get(), dwSizeOut);
            return dwSizeOut;
        }
    }

    sValue.clear();
    return 0;
}


///
/// Formats GUID and stores it in a std::string string.
///
/// \param[in ] lpGuid  Pointer to GUID
/// \param[out] str     String to store the result to
///
inline VOID GuidToString(_In_ LPCGUID lpGuid, _Out_ std::string &str)
{
    sprintf(str, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
        lpGuid->Data1,
        lpGuid->Data2,
        lpGuid->Data3,
        lpGuid->Data4[0], lpGuid->Data4[1],
        lpGuid->Data4[2], lpGuid->Data4[3], lpGuid->Data4[4], lpGuid->Data4[5], lpGuid->Data4[6], lpGuid->Data4[7]);
}


///
/// Formats GUID and stores it in a std::wstring string.
///
/// \param[in ] lpGuid  Pointer to GUID
/// \param[out] str     String to store the result to
///
inline VOID GuidToString(_In_ LPCGUID lpGuid, _Out_ std::wstring &str)
{
    sprintf(str, L"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
        lpGuid->Data1,
        lpGuid->Data2,
        lpGuid->Data3,
        lpGuid->Data4[0], lpGuid->Data4[1],
        lpGuid->Data4[2], lpGuid->Data4[3], lpGuid->Data4[4], lpGuid->Data4[5], lpGuid->Data4[6], lpGuid->Data4[7]);
}


///
/// Queries for a string value in the registry and stores it in a std::string string.
///
/// `REG_EXPAND_SZ` are expanded using `ExpandEnvironmentStrings()` before storing to sValue.
///
/// \param[in ] hReg     A handle to an open registry key. The key must have been opened with the KEY_QUERY_VALUE access right.
/// \param[in ] pszName  The name of the registry value. If lpValueName is NULL or an empty string, "", the function retrieves the type and data for the key's unnamed or default value, if any.
/// \param[out] sValue   String to store the value to
///
/// \return
/// - `ERROR_SUCCESS` when query succeeds;
/// - `ERROR_INVALID_DATA` when the registy value type is not `REG_SZ`, `REG_MULTI_SZ`, or `REG_EXPAND_SZ`;
/// - `ERROR_OUTOFMEMORY` when the memory allocation for the sValue buffer fails;
/// - Error code when query fails. See `RegQueryValueEx()` for the list of error codes.
///
/// \sa [RegQueryValueEx function](https://msdn.microsoft.com/en-us/library/windows/desktop/ms724911.aspx)
/// \sa [ExpandEnvironmentStrings function](https://msdn.microsoft.com/en-us/library/windows/desktop/ms724265.aspx)
///
inline LSTATUS RegQueryStringValue(_In_ HKEY hReg, _In_z_ LPCSTR pszName, _Out_ std::string &sValue)
{
    LSTATUS lResult;
    BYTE aStackBuffer[WINSTD_STACK_BUFFER_BYTES];
    DWORD dwSize = sizeof(aStackBuffer), dwType;

    // Try with stack buffer first.
    lResult = ::RegQueryValueExA(hReg, pszName, NULL, &dwType, aStackBuffer, &dwSize);
    if (lResult == ERROR_SUCCESS) {
        if (dwType == REG_SZ || dwType == REG_MULTI_SZ) {
            // The value is REG_SZ or REG_MULTI_SZ.
            sValue.assign((CHAR*)aStackBuffer, dwSize / sizeof(CHAR));
        } else if (dwType == REG_EXPAND_SZ) {
            // The value is REG_EXPAND_SZ. Expand it from stack buffer.
            if (::ExpandEnvironmentStringsW((const CHAR*)aStackBuffer, sValue) == 0)
                lResult = ::GetLastError();
        } else {
            // The value is not a string type.
            lResult = ERROR_INVALID_DATA;
        }
    } else if (lResult == ERROR_MORE_DATA) {
        if (dwType == REG_SZ || dwType == REG_MULTI_SZ) {
            // The value is REG_SZ or REG_MULTI_SZ. Read it now.
            auto szBuffer = std::unique_ptr<CHAR[]>(new CHAR[dwSize / sizeof(CHAR)]);
            if ((lResult = ::RegQueryValueExA(hReg, pszName, NULL, NULL, (LPBYTE)szBuffer.get(), &dwSize)) == ERROR_SUCCESS)
                sValue.assign(szBuffer.get(), dwSize / sizeof(CHAR));
            else
                sValue.clear();
        } else if (dwType == REG_EXPAND_SZ) {
            // The value is REG_EXPAND_SZ. Read it and expand environment variables.
            auto szBuffer = std::unique_ptr<CHAR[]>(new CHAR[dwSize / sizeof(CHAR)]);
            if ((lResult = ::RegQueryValueExA(hReg, pszName, NULL, NULL, (LPBYTE)szBuffer.get(), &dwSize)) == ERROR_SUCCESS) {
                if (::ExpandEnvironmentStringsW(szBuffer.get(), sValue) == 0)
                    lResult = ::GetLastError();
            } else
                sValue.clear();
        } else {
            // The value is not a string type.
            lResult = ERROR_INVALID_DATA;
        }
    }

    return lResult;
}


///
/// Queries for a string value in the registry and stores it in a std::wstring string.
///
/// `REG_EXPAND_SZ` are expanded using `ExpandEnvironmentStrings()` before storing to sValue.
///
/// \param[in ] hReg     A handle to an open registry key. The key must have been opened with the KEY_QUERY_VALUE access right.
/// \param[in ] pszName  The name of the registry value. If lpValueName is NULL or an empty string, "", the function retrieves the type and data for the key's unnamed or default value, if any.
/// \param[out] sValue   String to store the value to
///
/// \return
/// - `ERROR_SUCCESS` when query succeeds;
/// - `ERROR_INVALID_DATA` when the registy value type is not `REG_SZ`, `REG_MULTI_SZ`, or `REG_EXPAND_SZ`;
/// - `ERROR_OUTOFMEMORY` when the memory allocation for the sValue buffer fails;
/// - Error code when query fails. See `RegQueryValueEx()` for the list of error codes.
///
/// \sa [RegQueryValueEx function](https://msdn.microsoft.com/en-us/library/windows/desktop/ms724911.aspx)
/// \sa [ExpandEnvironmentStrings function](https://msdn.microsoft.com/en-us/library/windows/desktop/ms724265.aspx)
///
inline LSTATUS RegQueryStringValue(_In_ HKEY hReg, _In_z_ LPCWSTR pszName, _Out_ std::wstring &sValue)
{
    LSTATUS lResult;
    BYTE aStackBuffer[WINSTD_STACK_BUFFER_BYTES];
    DWORD dwSize = sizeof(aStackBuffer), dwType;

    // Try with stack buffer first.
    lResult = ::RegQueryValueExW(hReg, pszName, NULL, &dwType, aStackBuffer, &dwSize);
    if (lResult == ERROR_SUCCESS) {
        if (dwType == REG_SZ || dwType == REG_MULTI_SZ) {
            // The value is REG_SZ or REG_MULTI_SZ.
            sValue.assign((WCHAR*)aStackBuffer, dwSize / sizeof(WCHAR));
        } else if (dwType == REG_EXPAND_SZ) {
            // The value is REG_EXPAND_SZ. Expand it from stack buffer.
            if (::ExpandEnvironmentStringsW((const WCHAR*)aStackBuffer, sValue) == 0)
                lResult = ::GetLastError();
        } else {
            // The value is not a string type.
            lResult = ERROR_INVALID_DATA;
        }
    } else if (lResult == ERROR_MORE_DATA) {
        if (dwType == REG_SZ || dwType == REG_MULTI_SZ) {
            // The value is REG_SZ or REG_MULTI_SZ. Read it now.
            auto szBuffer = std::unique_ptr<WCHAR[]>(new WCHAR[dwSize / sizeof(WCHAR)]);
            if ((lResult = ::RegQueryValueExW(hReg, pszName, NULL, NULL, (LPBYTE)szBuffer.get(), &dwSize)) == ERROR_SUCCESS)
                sValue.assign(szBuffer.get(), dwSize / sizeof(WCHAR));
            else
                sValue.clear();
        } else if (dwType == REG_EXPAND_SZ) {
            // The value is REG_EXPAND_SZ. Read it and expand environment variables.
            auto szBuffer = std::unique_ptr<WCHAR[]>(new WCHAR[dwSize / sizeof(WCHAR)]);
            if ((lResult = ::RegQueryValueExW(hReg, pszName, NULL, NULL, (LPBYTE)szBuffer.get(), &dwSize)) == ERROR_SUCCESS) {
                if (::ExpandEnvironmentStringsW(szBuffer.get(), sValue) == 0)
                    lResult = ::GetLastError();
            } else
                sValue.clear();
        } else {
            // The value is not a string type.
            lResult = ERROR_INVALID_DATA;
        }
    }

    return lResult;
}


///
/// Retrieves the type and data for the specified value name associated with an open registry key and stores the data in a std::vector buffer.
///
/// \sa [RegQueryValueEx function](https://msdn.microsoft.com/en-us/library/windows/desktop/ms724911.aspx)
///
inline LSTATUS RegQueryValueExA(_In_ HKEY hKey, _In_opt_ LPCSTR lpValueName, __reserved LPDWORD lpReserved, _Out_opt_ LPDWORD lpType, _Out_ std::vector<unsigned char> &aData)
{
    LSTATUS lResult;
    BYTE aStackBuffer[WINSTD_STACK_BUFFER_BYTES];
    DWORD dwSize = sizeof(aStackBuffer);

    // Try with stack buffer first.
    lResult = RegQueryValueExA(hKey, lpValueName, lpReserved, NULL, aStackBuffer, &dwSize);
    if (lResult == ERROR_SUCCESS) {
        // Allocate buffer on heap, copy from stack buffer.
        aData.resize(dwSize);
        memcpy(aData.data(), aStackBuffer, dwSize);
    } else if (lResult == ERROR_MORE_DATA) {
        // Allocate buffer on heap and retry.
        aData.resize(dwSize);
        if ((lResult = RegQueryValueExA(hKey, lpValueName, lpReserved, lpType, aData.data(), &dwSize)) != ERROR_SUCCESS)
            aData.clear();
    }

    return lResult;
}


///
/// Retrieves the type and data for the specified value name associated with an open registry key and stores the data in a std::vector buffer.
///
/// \sa [RegQueryValueEx function](https://msdn.microsoft.com/en-us/library/windows/desktop/ms724911.aspx)
///
inline LSTATUS RegQueryValueExW(_In_ HKEY hKey, _In_opt_ LPCWSTR lpValueName, __reserved LPDWORD lpReserved, _Out_opt_ LPDWORD lpType, _Out_ std::vector<unsigned char> &aData)
{
    LSTATUS lResult;
    BYTE aStackBuffer[WINSTD_STACK_BUFFER_BYTES];
    DWORD dwSize = sizeof(aStackBuffer);

    // Try with stack buffer first.
    lResult = RegQueryValueExW(hKey, lpValueName, lpReserved, NULL, aStackBuffer, &dwSize);
    if (lResult == ERROR_SUCCESS) {
        // Allocate buffer on heap, copy from stack buffer.
        aData.resize(dwSize);
        memcpy(aData.data(), aStackBuffer, dwSize);
    } else if (lResult == ERROR_MORE_DATA) {
        // Allocate buffer on heap and retry.
        aData.resize(dwSize);
        if ((lResult = RegQueryValueExW(hKey, lpValueName, lpReserved, lpType, aData.data(), &dwSize)) != ERROR_SUCCESS)
            aData.clear();
    }

    return lResult;
}


#if _WIN32_WINNT >= _WIN32_WINNT_VISTA

///
/// Loads the specified string from the specified key and subkey, and stores it in a std::string string.
///
/// \sa [RegLoadMUIString function](https://msdn.microsoft.com/en-us/library/windows/desktop/ms724890.aspx)
///
inline LSTATUS RegLoadMUIStringA(_In_ HKEY hKey, _In_opt_ LPCSTR pszValue, _Out_ std::string &sOut, _In_ DWORD Flags, _In_opt_ LPCSTR pszDirectory)
{
    LSTATUS lResult;
    CHAR szStackBuffer[WINSTD_STACK_BUFFER_BYTES/sizeof(CHAR)];
    DWORD dwSize;

    Flags &= ~REG_MUI_STRING_TRUNCATE;

    // Try with stack buffer first.
    lResult = RegLoadMUIStringA(hKey, pszValue, szStackBuffer, _countof(szStackBuffer), &dwSize, Flags, pszDirectory);
    if (lResult == ERROR_SUCCESS) {
        // Copy from stack buffer.
        sOut.assign(szStackBuffer, dwSize);
    } else if (lResult == ERROR_MORE_DATA) {
        // Allocate buffer on heap and retry.
        auto szBuffer = std::unique_ptr<CHAR[]>(new CHAR[dwSize + 1]);
        sOut.assign(szBuffer.get(), (lResult = RegLoadMUIStringA(hKey, pszValue, szBuffer.get(), dwSize, &dwSize, Flags, pszDirectory)) == ERROR_SUCCESS ? dwSize : 0);
    }

    return lResult;
}


///
/// Loads the specified string from the specified key and subkey, and stores it in a std::wstring string.
///
/// \sa [RegLoadMUIString function](https://msdn.microsoft.com/en-us/library/windows/desktop/ms724890.aspx)
///
inline LSTATUS RegLoadMUIStringW(_In_ HKEY hKey, _In_opt_ LPCWSTR pszValue, _Out_ std::wstring &sOut, _In_ DWORD Flags, _In_opt_ LPCWSTR pszDirectory)
{
    LSTATUS lResult;
    WCHAR szStackBuffer[WINSTD_STACK_BUFFER_BYTES/sizeof(WCHAR)];
    DWORD dwSize;

    Flags &= ~REG_MUI_STRING_TRUNCATE;

    // Try with stack buffer first.
    lResult = RegLoadMUIStringW(hKey, pszValue, szStackBuffer, _countof(szStackBuffer), &dwSize, Flags, pszDirectory);
    if (lResult == ERROR_SUCCESS) {
        // Copy from stack buffer.
        sOut.assign(szStackBuffer, dwSize);
    } else if (lResult == ERROR_MORE_DATA) {
        // Allocate buffer on heap and retry.
        auto szBuffer = std::unique_ptr<WCHAR[]>(new WCHAR[dwSize + 1]);
        sOut.assign(szBuffer.get(), (lResult = RegLoadMUIStringW(hKey, pszValue, szBuffer.get(), dwSize, &dwSize, Flags, pszDirectory)) == ERROR_SUCCESS ? dwSize : 0);
    }

    return lResult;
}

#endif

/// @}


namespace winstd
{
    /// \addtogroup ATLWinAPI
    /// @{

    ///
    /// Module handle wrapper
    ///
    class library : public handle<HMODULE>
    {
    public:
        ///
        /// Frees the module.
        ///
        /// \sa [FreeLibrary](https://msdn.microsoft.com/en-us/library/windows/desktop/ms683152.aspx)
        ///
        virtual ~library()
        {
            if (m_h)
                FreeLibrary(m_h);
        }

        ///
        /// Loads the specified module into the address space of the calling process.
        ///
        /// \sa [LoadLibraryEx](https://msdn.microsoft.com/en-us/library/windows/desktop/ms684179.aspx)
        ///
        inline bool Load(_In_ LPCTSTR lpFileName, __reserved handle_type hFile, _In_ DWORD dwFlags)
        {
            handle_type h = LoadLibraryEx(lpFileName, hFile, dwFlags);
            if (h) {
                attach(h);
                return true;
            } else
                return false;
        }

    protected:
        ///
        /// Frees the module.
        ///
        /// \sa [FreeLibrary](https://msdn.microsoft.com/en-us/library/windows/desktop/ms683152.aspx)
        ///
        virtual void free_internal()
        {
            FreeLibrary(m_h);
        }
    };

    /// @}
}