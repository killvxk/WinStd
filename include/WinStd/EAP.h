﻿/*
    Copyright 1991-2016 Amebis
    Copyright 2016 GÉANT

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

#include "Common.h"

#include <Windows.h>
#include <eaphostpeerconfigapis.h>
#include <eaptypes.h> // Must include after <Windows.h>

#include <memory>

namespace winstd
{
    ///
    /// \defgroup WinStdEAPAPI Extensible Authentication Protocol API
    /// Integrates WinStd classes with Microsoft EAP API
    ///
    /// @{

    ///
    /// EAP method numbers
    ///
    /// \sa [Extensible Authentication Protocol (EAP) Registry (Chapter: Method Types)](https://www.iana.org/assignments/eap-numbers/eap-numbers.xhtml#eap-numbers-4)
    ///
    enum eap_type_t;

    ///
    /// Deleter for unique_ptr using EapHostPeerFreeMemory
    ///
    struct WINSTD_API EapHostPeerFreeMemory_delete;

    ///
    /// EapHost BLOB wrapper class
    ///
    typedef std::unique_ptr<BYTE[], EapHostPeerFreeMemory_delete> WINSTD_API eap_blob;

    ///
    /// Deleter for unique_ptr to EAP_ERROR using EapHostPeerFreeEapError
    ///
    struct WINSTD_API EapHostPeerFreeErrorMemory_delete;

    ///
    /// EAP_ERROR wrapper class
    ///
    typedef std::unique_ptr<EAP_ERROR, EapHostPeerFreeErrorMemory_delete> WINSTD_API eap_error;

    ///
    /// EAP_ATTRIBUTE wrapper class
    ///
    class WINSTD_API eap_attr;

    ///
    /// EAP_METHOD_PROPERTY wrapper class
    ///
    class WINSTD_API eap_method_prop;

    ///
    /// EapPacket wrapper class
    ///
    class WINSTD_API eap_packet;

    ///
    /// EAP_METHOD_INFO_ARRAY wrapper class
    ///
    class WINSTD_API eap_method_info_array;

    /// @}
}

#pragma once

#include <eapmethodtypes.h>


namespace winstd
{
    #pragma warning(suppress: 4480)
    enum eap_type_t : unsigned char {
        eap_type_undefined       =   0,    ///< Undefined EAP type
        eap_type_identity        =   1,    ///< Identity
        eap_type_notification    =   2,    ///< Notification
        eap_type_nak             =   3,    ///< Legacy Nak
        eap_type_md5_challenge   =   4,    ///< MD5-Challenge
        eap_type_otp             =   5,    ///< One-Time Password (OTP)
        eap_type_gtc             =   6,    ///< Generic Token Card (GTC)
        eap_type_tls             =  13,    ///< EAP-TLS
        eap_type_ttls            =  21,    ///< EAP-TTLS
        eap_type_peap            =  25,    ///< EAP-PEAP
        eap_type_mschapv2        =  29,    ///< EAP-MSCHAPv2

        eap_type_legacy_pap      = 192,    ///< PAP (Not actually an EAP method; Moved to the Unassigned area)
        eap_type_legacy_mschapv2 = 193,    ///< MSCHAPv2 (Not actually an EAP method; Moved to the Unassigned area)

        eap_type_start           =   1,    ///< Start of EAP methods
        eap_type_end             = 192,    ///< End of EAP methods (non-inclusive)
        eap_type_noneap_start    = 192,    ///< Start of non-EAP methods
        eap_type_noneap_end      = 254,    ///< End of non-EAP methods (non-inclusive)
    };


    struct WINSTD_API EapHostPeerFreeMemory_delete
    {
        ///
        /// Default constructor
        ///
        EapHostPeerFreeMemory_delete() {}

        ///
        /// Delete a pointer
        ///
        void operator()(BYTE *_Ptr) const
        {
            EapHostPeerFreeMemory(_Ptr);
        }
    };


    struct WINSTD_API EapHostPeerFreeErrorMemory_delete
    {
        ///
        /// Default constructor
        ///
        EapHostPeerFreeErrorMemory_delete() {}

        ///
        /// Delete a pointer
        ///
        void operator()(EAP_ERROR *_Ptr) const
        {
            EapHostPeerFreeErrorMemory(_Ptr);
        }
    };


    class WINSTD_API __declspec(novtable) eap_attr : public EAP_ATTRIBUTE
    {
    public:
        ///
        /// Initializes a new EAP attribute set to eatReserved.
        ///
        inline eap_attr()
        {
            eaType   = eatReserved;
            dwLength = 0;
            pValue   = NULL;
        }

        ///
        /// Copies an existing EAP attribute.
        ///
        inline eap_attr(_In_ const EAP_ATTRIBUTE &a)
        {
            eaType   = a.eaType;
            dwLength = a.dwLength;
            if (a.dwLength) {
                pValue = new BYTE[a.dwLength];
                assert(pValue);
                memcpy(pValue, a.pValue, a.dwLength);
            } else
                pValue = NULL;
        }

        ///
        /// Moves an existing EAP attribute.
        ///
        inline eap_attr(_Inout_ eap_attr &&a)
        {
            eaType   = a.eaType;
            dwLength = a.dwLength;
            if (a.dwLength) {
                pValue     = a.pValue;
                a.dwLength = 0;
                a.pValue   = NULL;
            } else
                pValue = NULL;
        }

        ///
        /// Destroys the EAP attribute.
        ///
        ~eap_attr();

        ///
        /// Copies an existing EAP attribute.
        ///
        inline eap_attr& operator=(_In_ const EAP_ATTRIBUTE &a)
        {
            if (this != &a) {
                eaType   = a.eaType;
                dwLength = a.dwLength;
                if (a.dwLength) {
                    BYTE *pValueNew = new BYTE[a.dwLength];
                    if (pValueNew) {
                        if (pValue)
                            delete [] pValue;
                        memcpy(pValueNew, a.pValue, a.dwLength);
                        pValue = pValueNew;
                    } else
                        assert(0); // Could not allocate memory
                } else
                    pValue = NULL;
            }
            return *this;
        }

        ///
        /// Moves an existing EAP attribute.
        ///
        inline eap_attr& operator=(_Inout_ eap_attr &&a)
        {
            if (this != &a) {
                eaType   = a.eaType;
                dwLength = a.dwLength;
                if (pValue)
                    delete [] pValue;
                if (a.dwLength) {
                    pValue     = a.pValue;
                    a.dwLength = 0;
                    a.pValue   = NULL;
                } else
                    pValue = NULL;
            }
            return *this;
        }

        ///
        /// Creates MS-MPPE-Send-Key or MS-MPPE-Recv-Key
        ///
        /// \sa [RADIUS Vendor-Specific](https://tools.ietf.org/html/rfc2865#section-5.26)
        /// \sa [MS-MPPE-Send-Key](https://tools.ietf.org/html/rfc2548#section-2.4.2)
        /// \sa [MS-MPPE-Recv-Key](https://tools.ietf.org/html/rfc2548#section-2.4.3)
        ///
        void create_ms_mppe_key(_In_ BYTE bVendorType, _In_count_(nKeySize) LPCBYTE pbKey, _In_ BYTE nKeySize);

    public:
        static const EAP_ATTRIBUTE blank;
    };


    class WINSTD_API __declspec(novtable) eap_method_prop : public EAP_METHOD_PROPERTY
    {
    public:
        ///
        /// Constructs a BOOL method property
        ///
        /// \param[in] type   EAP method property type
        /// \param[in] value  Property value
        ///
        inline eap_method_prop(_In_ EAP_METHOD_PROPERTY_TYPE type, _In_ BOOL value)
        {
            eapMethodPropertyType                  = type;
            eapMethodPropertyValueType             = empvtBool;
            eapMethodPropertyValue.empvBool.length = sizeof(BOOL);
            eapMethodPropertyValue.empvBool.value  = value;
        }


        ///
        /// Constructs a DWORD method property
        ///
        /// \param[in] type   EAP method property type
        /// \param[in] value  Property value
        ///
        inline eap_method_prop(_In_ EAP_METHOD_PROPERTY_TYPE type, _In_ DWORD value)
        {
            eapMethodPropertyType                   = type;
            eapMethodPropertyValueType              = empvtDword;
            eapMethodPropertyValue.empvDword.length = sizeof(DWORD);
            eapMethodPropertyValue.empvDword.value  = value;
        }


        ///
        /// Constructs a Unicode string method property
        ///
        /// \param[in] type   EAP method property type
        /// \param[in] value  Property value
        ///
        inline eap_method_prop(_In_ EAP_METHOD_PROPERTY_TYPE type, _In_z_ LPCWSTR value)
        {
            eapMethodPropertyType                    = type;
            eapMethodPropertyValueType               = empvtString;
            eapMethodPropertyValue.empvString.length = (DWORD)(sizeof(WCHAR)*(wcslen(value) + 1));
            eapMethodPropertyValue.empvString.value  = (BYTE*)value;
        }
    };


    class WINSTD_API eap_packet : public dplhandle<EapPacket*>
    {
        DPLHANDLE_IMPL(eap_packet)

    public:
        ///
        /// Destroys the EAP packet.
        ///
        virtual ~eap_packet();


        ///
        /// Create new EAP packet
        ///
        /// \param[in] code  EAP code (one of EapCode enum values)
        /// \param[in] id    Packet ID
        /// \param[in] size  Initial packet size. Must be at least 4.
        ///
        /// \note Packet data (beyond first 4B) is not initialized.
        ///
        /// \return
        /// - true when creation succeeds;
        /// - false when creation fails. For extended error information, call `GetLastError()`.
        ///
        inline bool create(_In_ EapCode code, _In_ BYTE id, _In_ WORD size)
        {
            assert(size >= 4); // EAP packets must contain at least 4B.

            handle_type h = (handle_type)HeapAlloc(GetProcessHeap(), 0, size);
            if (h) {
                        h->Code   = (BYTE)      code ;
                        h->Id     =             id   ;
                *(WORD*)h->Length =       htons(size);

                attach(h);
                return true;
            } else {
                SetLastError(ERROR_OUTOFMEMORY);
                return false;
            }
        }


        ///
        /// Returns total EAP packet size in bytes.
        ///
        inline WORD size() const
        {
            return m_h ? ntohs(*(WORD*)m_h->Length) : 0;
        }


    protected:
        ///
        /// Destroys the EAP packet.
        ///
        virtual void free_internal();

        ///
        /// Duplicates the EAP packet.
        ///
        virtual handle_type duplicate_internal(_In_ handle_type h) const;
    };


    class WINSTD_API __declspec(novtable) eap_method_info_array : public EAP_METHOD_INFO_ARRAY
    {
        WINSTD_NONCOPYABLE(eap_method_info_array)

    public:
        ///
        /// Constructs an empty array
        ///
        inline eap_method_info_array()
        {
            dwNumberOfMethods = 0;
            pEapMethods       = NULL;
        }

        ///
        /// Move constructor
        ///
        /// \param[inout] other  A rvalue reference of another object
        ///
        inline eap_method_info_array(_Inout_ eap_method_info_array &&other)
        {
            dwNumberOfMethods       = other.dwNumberOfMethods;
            pEapMethods             = other.pEapMethods;
            other.dwNumberOfMethods = 0;
            other.pEapMethods       = NULL;
        }

        ///
        /// Destructor
        ///
        ~eap_method_info_array();

        ///
        /// Move assignment
        ///
        /// \param[inout] other  A rvalue reference of another object
        ///
        inline eap_method_info_array& operator=(_Inout_ eap_method_info_array &&other)
        {
            if (this != std::addressof(other)) {
                if (pEapMethods)
                    free_internal();
                dwNumberOfMethods       = other.dwNumberOfMethods;
                pEapMethods             = other.pEapMethods;
                other.dwNumberOfMethods = 0;
                other.pEapMethods       = NULL;
            }
            return *this;
        }

    protected:
        void free_internal();
        static void free_internal(_In_ EAP_METHOD_INFO *pMethodInfo);
    };
}
