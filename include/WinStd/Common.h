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

#include <Windows.h>

#include <assert.h>
#include <stdarg.h>

#include <memory>
#include <string>


///
/// Public function calling convention
///
#if defined(WINSTD_DLL)
#define WINSTD_API      __declspec(dllexport)
#elif defined(WINSTD_DLLIMP)
#define WINSTD_API      __declspec(dllimport)
#else
#define WINSTD_API
#endif


#ifndef WINSTD_STACK_BUFFER_BYTES
///
/// Size of the stack buffer in bytes used for initial system function call
///
/// Some system functions with variable length output data fail for
/// insufficient buffer sizes, and return an exact buffer length required.
/// The function helpers use a fixed size stack buffer first. If the stack
/// buffer really prooved sufficient, the helper allocates the exact length
/// output on heap and copies the data without calling the system function
/// again. Otherwise it allocates the exact length output on heap and retries.
///
/// \note
/// Decrease this value in case of stack overflow.
///
#define WINSTD_STACK_BUFFER_BYTES  1024
#endif


// Do not use _vsnprintf_s/_vsnwprintf_s(), since it terminates string by force even when we explicitly want to write unterminated string.
// Threfore turn off compiler warning instead. ;)
#pragma warning(push)
#pragma warning(disable: 4995)
#pragma warning(disable: 4996)

inline int vsnprintf(_Out_z_cap_(capacity) char *str, _In_ size_t capacity, _In_z_ _Printf_format_string_ const char *format, _In_ va_list arg)
{
    return _vsnprintf(str, capacity, format, arg);
}


inline int vsnprintf(_Out_z_cap_(capacity) wchar_t *str, _In_ size_t capacity, _In_z_ _Printf_format_string_ const wchar_t *format, _In_ va_list arg)
{
    return _vsnwprintf(str, capacity, format, arg);
}


///
/// Formats string using `printf()`.
///
/// \param[out] str     Formatted string
/// \param[in ] format  String template using `printf()` style
/// \param[in ] arg     Arguments to `format`
///
/// \returns Number of characters in result.
///
template<class _Elem, class _Traits, class _Ax>
inline int vsprintf(_Out_ std::basic_string<_Elem, _Traits, _Ax> &str, _In_z_ _Printf_format_string_ const _Elem *format, _In_ va_list arg)
{
    _Elem buf[WINSTD_STACK_BUFFER_BYTES/sizeof(_Elem)];

    // Try with stack buffer first.
    int count = vsnprintf(buf, _countof(buf) - 1, format, arg);
    if (count >= 0) {
        // Copy from stack.
        str.assign(buf, count);
    } else {
        for (size_t capacity = 2*WINSTD_STACK_BUFFER_BYTES/sizeof(_Elem);; capacity *= 2) {
            // Allocate on heap and retry.
            auto buf = std::unique_ptr<_Elem[]>(new _Elem[capacity]);
            count = vsnprintf(buf.get(), capacity - 1, format, arg);
            if (count >= 0) {
                str.assign(buf.get(), count);
                break;
            }
        }
    }

    return count;
}

#pragma warning(pop)


///
/// Formats string using `printf()`.
///
/// \param[out] str     Formatted string
/// \param[in ] format  String template using `printf()` style
///
/// \returns Number of characters in result.
///
template<class _Elem, class _Traits, class _Ax>
inline int sprintf(_Out_ std::basic_string<_Elem, _Traits, _Ax> &str, _In_z_ _Printf_format_string_ const _Elem *format, ...)
{
    va_list arg;
    va_start(arg, format);
    int res = vsprintf(str, format, arg);
    va_end(arg);
    return res;
}


namespace winstd
{
#ifdef _UNICODE
    typedef std::wstring tstring;
#else
    typedef std::string tstring;
#endif


    ///
    /// \defgroup WinStdSysHandles System Handles
    /// Simplifies work with object handles of various type
    ///
    /// @{

    ///
    /// Base abstract template class to support generic object handle keeping
    ///
    /// It provides basic operators and methods common to all descendands of this class establishing a base to ease the replacement of native object handle type with classes in object-oriented approach.
    ///
    template <class T> class handle
    {
    public:
        ///
        /// Datatype of the object handle this template class handles
        ///
        typedef T handle_type;

        ///
        /// Initializes a new class instance with the object handle set to NULL.
        ///
        inline handle() : m_h(NULL)
        {
        }

        ///
        /// Initializes a new class instance with an available object handle.
        ///
        /// \param[in] h  Initial object handle value
        ///
        inline handle(_In_opt_ handle_type h) : m_h(h)
        {
        }

        /// \name Operators for transparent usage of this class
        /// @{

        ///
        /// Auto-typecasting operator
        ///
        /// \return Object handle
        ///
        inline operator handle_type() const
        {
            return m_h;
        }

        ///
        /// Returns the object handle value when the object handle is a pointer to a value (class, struct, etc.).
        ///
        /// \return Object handle value
        ///
        inline handle_type*& operator*() const
        {
            assert(m_h != NULL);
            return *m_h;
        }

        ///
        /// Returns the object handle reference.
        /// \return Object handle reference
        ///
        inline handle_type* operator&()
        {
            assert(m_h == NULL);
            return &m_h;
        }

        ///
        /// Provides object handle member access when the object handle is a pointer to a class or struct.
        ///
        /// \return Object handle
        ///
        inline handle_type operator->() const
        {
            assert(m_h != NULL);
            return m_h;
        }

        /// @}

        /// \name Comparison operators
        /// @{

        ///
        /// Tests if the object handle is NULL.
        ///
        /// \return
        /// - Non zero when object handle is NULL;
        /// - Zero otherwise.
        ///
        inline bool operator!() const
        {
            return m_h == NULL;
        }

        ///
        /// Is handle less than?
        ///
        /// \param[in] h  Object handle to compare against
        /// \return
        /// - Non zero when object handle is less than h;
        /// - Zero otherwise.
        ///
        inline bool operator<(_In_opt_ handle_type h) const
        {
            return m_h < h;
        }

        ///
        /// Is handle less than or equal to?
        ///
        /// \param[in] h  Object handle to compare against
        /// \return
        /// - Non zero when object handle is less than or equal to h;
        /// - Zero otherwise.
        ///
        inline bool operator<=(_In_opt_ handle_type h) const
        {
            return m_h <= h;
        }

        ///
        /// Is handle greater than or equal to?
        ///
        /// \param[in] h  Object handle to compare against
        /// \return
        /// - Non zero when object handle is greater than or equal to h;
        /// - Zero otherwise.
        ///
        inline bool operator>=(_In_opt_ handle_type h) const
        {
            return m_h >= h;
        }

        ///
        /// Is handle greater than?
        ///
        /// \param[in] h  Object handle to compare against
        /// \return
        /// - Non zero when object handle is greater than h;
        /// - Zero otherwise.
        ///
        inline bool operator>(_In_opt_ handle_type h) const
        {
            return m_h > h;
        }

        ///
        /// Is handle not equal to?
        ///
        /// \param[in] h  Object handle to compare against
        /// \return
        /// - Non zero when object handle is not equal to h;
        /// - Zero otherwise.
        ///
        inline bool operator!=(_In_opt_ handle_type h) const
        {
            return !operator==(h);
        }

        ///
        /// Is handle equal to?
        ///
        /// \param[in] h  Object handle to compare against
        /// \return
        /// - Non zero when object handle is equal to h;
        /// - Zero otherwise.
        ///
        inline bool operator==(_In_opt_ handle_type h) const
        {
            return m_h == h;
        }

        /// @}

        ///
        /// Sets a new object handle for the class
        ///
        /// When the current object handle of the class is non-NULL, the object is destroyed first.
        ///
        /// \param[in] h  New object handle
        ///
        inline void attach(_In_opt_ handle_type h)
        {
            if (m_h)
                free_internal();
            m_h = h;
        }

        ///
        /// Dismisses the object handle from this class
        ///
        /// \return Object handle
        ///
        inline handle_type detach()
        {
            handle_type h = m_h;
            m_h = NULL;
            return h;
        }

        ///
        /// Destroys the object
        ///
        inline void Free()
        {
            if (m_h) {
                free_internal();
                m_h = NULL;
            }
        }

        /// @}

    protected:
        ///
        /// Abstract member function that must be implemented by child classes to do the actual object destruction.
        ///
        virtual void free_internal() = 0;

    protected:
        handle_type m_h; ///< Object handle
    };


    ///
    /// Base abstract template class to support object handle keeping for objects that support handle duplication
    ///
    template <class T>
    class dplhandle : public handle<T>
    {
    public:
        ///
        /// Duplicates and returns a new object handle.
        ///
        /// \return Duplicated object handle
        ///
        inline handle_type duplicate() const
        {
            return m_h ? duplicate_internal(m_h) : NULL;
        }

        ///
        /// Duplicates an object handle and sets a new object handle.
        ///
        /// \param[in] h  Object handle of existing object
        ///
        /// \return
        /// - true when duplication succeeds;
        /// - false when duplication fails. In case of failure obtaining the extended error information is object type specific (for example: `GetLastError()`).
        ///
        inline bool attach_duplicated(_In_opt_ handle_type h)
        {
            if (m_h)
                free_internal();

            return h ? (m_h = duplicate_internal(h)) != NULL : (m_h = NULL, true);
        }

        //
        // Do not allow = operators. They are semantically ambigious:
        // Do they attach the class to the existing instance of object, or do they duplicate it?
        // To avoid confusion, user should use attach() and Duplicate() methods explicitly.
        //
        //inline const dplhandle<T>& operator=(_In_ const handle_type src)
        //{
        //    attach(src ? duplicate_internal(src) : NULL);
        //    return *this;
        //}

        //inline const dplhandle<T>& operator=(_In_ const dplhandle<T> &src)
        //{
        //    attach(src.m_h ? duplicate_internal(src.m_h) : NULL);
        //    return *this;
        //}

    protected:
        ///
        /// Abstract member function that must be implemented by child classes to do the actual object handle duplication.
        ///
        /// \param[in] h  Object handle of existing object
        ///
        /// \return Duplicated object handle
        ///
        virtual handle_type duplicate_internal(_In_ handle_type h) const = 0;
    };

    /// @}

    ///
    /// \defgroup WinStdStrFormat String Formatting
    /// Formatted string generation
    ///
    /// \par Example
    /// \code
    /// // Please note the PCSTR typecasting invokes an operator to return
    /// // pointer to formatted buffer rather than class reference itself.
    /// cout << (PCSTR)(string_printf("%i is less than %i.\n", 1, 5));
    /// \endcode
    ///
    /// @{

    ///
    /// Base template class to support string formatting using `printf()` style templates
    ///
    template<class _Elem, class _Traits = char_traits<_Elem>, class _Ax = allocator<_Elem> >
    class basic_string_printf : public std::basic_string<_Elem, _Traits, _Ax>
    {
    public:
        /// \name Initializing string using template in memory
        /// @{

        ///
        /// Initializes a new string and formats its contents using `printf()` style template.
        ///
        /// \param[in] format  String template using `printf()` style
        ///
        basic_string_printf(_In_z_ _Printf_format_string_ const _Elem *format, ...)
        {
            va_list arg;
            va_start(arg, format);
            vsprintf(*this, format, arg);
            va_end(arg);
        }

        /// @}

        /// \name Initializing string using template in resources
        /// @{

        ///
        /// Initializes a new string and formats its contents using `printf()` style template in resources.
        ///
        /// \param[in] hInstance  Resource module handle
        /// \param[in] nFormatID  Resource ID of the string template using `printf()` style
        ///
        basic_string_printf(_In_ HINSTANCE hInstance, _In_ UINT nFormatID, ...)
        {
            _Myt format;
            ATLENSURE(format.LoadString(hInstance, nFormatID));

            va_list arg;
            va_start(arg, nFormatID);
            vsprintf(*this, format, arg);
            va_end(arg);
        }

        ///
        /// Initializes a new string and formats its contents using `printf()` style template in resources.
        ///
        /// \param[in] hInstance    Resource module handle
        /// \param[in] wLanguageID  Resource language
        /// \param[in] nFormatID    Resource ID of the string template using `printf()` style
        ///
        basic_string_printf(_In_ HINSTANCE hInstance, _In_ WORD wLanguageID, _In_ UINT nFormatID, ...)
        {
            _Myt format;
            ATLENSURE(format.LoadString(hInstance, nFormatID, wLanguageID));

            va_list arg;
            va_start(arg, nFormatID);
            vsprintf(*this, format, arg);
            va_end(arg);
        }

        /// }@
    };


    ///
    /// Single-byte character implementation of a class to support string formatting using `printf()` style templates
    ///
    typedef basic_string_printf<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> > string_printf;


    ///
    /// Wide character implementation of a class to support string formatting using `printf()` style templates
    ///
    typedef basic_string_printf<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t>> wstring_printf;


    ///
    /// Base template class to support string formatting using `FormatMessage()` style templates
    ///
    template<class _Elem, class _Traits = char_traits<_Elem>, class _Ax = allocator<_Elem> >
    class basic_string_msg : public std::basic_string<_Elem, _Traits, _Ax>
    {
    public:
        /// \name Initializing string using template in memory
        /// @{

        ///
        /// Initializes a new string and formats its contents using `FormatMessage()` style template.
        ///
        /// \param[in] format  String template using `FormatMessage()` style
        ///
        basic_string_msg(_In_z_ _FormatMessage_format_string_ const _Elem *format, ...)
        {
            va_list arg;
            va_start(arg, format);
            FormatMessageV(format, &arg);
            va_end(arg);
        }

        /// @}

        /// \name Initializing string using template in resources
        /// @{

        ///
        /// Initializes a new string and formats its contents using `FormatMessage()` style template in resources.
        ///
        /// \param[in] hInstance  Resource module handle
        /// \param[in] nFormatID  Resource ID of the string template using `FormatMessage()` style
        ///
        basic_string_msg(_In_ HINSTANCE hInstance, _In_ UINT nFormatID, ...)
        {
            _Myt format(GetManager());
            ATLENSURE(format.LoadString(hInstance, nFormatID));

            va_list arg;
            va_start(arg, nFormatID);
            FormatMessageV(format, &arg);
            va_end(arg);
        }

        ///
        /// Initializes a new string and formats its contents using `FormatMessage()` style template in resources.
        ///
        /// \param[in] hInstance    Resource module handle
        /// \param[in] wLanguageID  Resource language
        /// \param[in] nFormatID    Resource ID of the string template using `FormatMessage()` style
        ///
        basic_string_msg(_In_ HINSTANCE hInstance, _In_ WORD wLanguageID, _In_ UINT nFormatID, ...)
        {
            _Myt format(GetManager());
            ATLENSURE(format.LoadString(hInstance, nFormatID, wLanguageID));

            va_list arg;
            va_start(arg, nFormatID);
            FormatMessageV(format, &arg);
            va_end(arg);
        }

        /// @}
    };

    ///
    /// Single-byte character implementation of a class to support string formatting using `FormatMessage()` style templates
    ///
    typedef basic_string_msg<char, std::char_traits<char>, std::allocator<char> > string_msg;

    ///
    /// Wide character implementation of a class to support string formatting using `FormatMessage()` style templates
    ///
    typedef basic_string_msg<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> > wstring_msg;

    /// @}

    /// \defgroup WinStdMemSanitize Auto-sanitize Memory Management
    /// Sanitizes memory before dismissed
    ///
    /// @{

    ///
    /// An allocator template that sanitizes each memory block before it is destroyed or reallocated
    ///
    /// \note
    /// sanitizing_allocator introduces a performance penalty. However, it provides an additional level of security.
    /// Use for security sensitive data memory storage only.
    ///
    template <class _Ty>
    class sanitizing_allocator : public std::allocator<_Ty> {
    public:
        ///
        /// Sanitizes memory before freeing it
        ///
        /// \param[in] p  Pointer to heap memory block
        ///
        void deallocate(_In_ pointer p, _In_ size_type count)
        {
            // Sanitize then free.
            SecureZeroMemory(p, sizeof(_Ty)*count);
            std::allocator<_Ty>::deallocate(p, count);
        }
    };

    /// @}
}