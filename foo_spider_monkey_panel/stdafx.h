#pragma once

// clang-format off
// !!! Include order is important here (esp. for Win headers) !!!

// Spider Monkey ESR60 and CUI support only Win7+
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#define WINVER _WIN32_WINNT_WIN7

// Fix std min max conflicts
#define NOMINMAX
#include <algorithm>
namespace Gdiplus
{
using std::min;
using std::max;
};

#include <WinSock2.h>
#include <Windows.h>
#pragma warning( push, 0 )
#   include <GdiPlus.h>
#pragma warning( pop ) 

// COM objects
#include <ActivScp.h>
#include <activdbg.h>
#include <MsHTML.h>
#include <MsHtmHst.h>
#include <ShlDisp.h>
#include <exdisp.h>
#include <shobjidl_core.h>
// Generates wrappers for COM listed above
#include <ComDef.h>

// ATL/WTL
/// atlstr.h (includes atlbase.h) must be included first for CString to LPTSTR conversion to work.
#include <atlstr.h> 
#include <atlapp.h>
#include <atlcom.h>
#include <atlcrack.h>
#include <atlctrls.h>
#include <atlddx.h>
#include <atldlgs.h>
#include <atlfind.h>
#include <atlframe.h>
#include <atltheme.h>
#include <atltypes.h>
#include <atlwin.h>

// foobar2000 SDK
#pragma warning( push, 0 )
#   include <foobar2000/SDK/foobar2000.h>
#pragma warning( pop ) 

// Columns UI SDK
#pragma warning( push, 0 )
#   include <columns_ui-sdk/ui_extension.h>
#pragma warning( pop ) 

#define SMP_MJS_SUPPRESS_WARNINGS_PUSH \
    __pragma( warning( push ) )        \
    __pragma( warning( disable : 4251 ) ) /* dll interface warning */

#define SMP_MJS_SUPPRESS_WARNINGS_POP \
    __pragma( warning( pop ) )

// Mozilla SpiderMonkey
SMP_MJS_SUPPRESS_WARNINGS_PUSH
#include <jsapi.h>
#include <jsfriendapi.h>
SMP_MJS_SUPPRESS_WARNINGS_POP

// fmt
#define FMT_HEADER_ONLY
#include <fmt/format.h>

// range v3
#include <range/v3/all.hpp>

// Some macros defined by windowsx.h should be removed
#ifdef _INC_WINDOWSX
#undef SubclassWindow
#endif

#if not __cpp_char8_t
// Dummy type
#include <string>

using char8_t = char;
namespace std // NOLINT(cert-dcl58-cpp)
{
using u8string = basic_string<char8_t, char_traits<char8_t>, allocator<char8_t>>;
using u8string_view = basic_string_view<char8_t>;
}
#endif

// Additional PFC wrappers
#include <utils/pfc_helpers_cnt.h>
#include <utils/pfc_helpers_stream.h>
#include <utils/pfc_helpers_ui.h>

// Unicode converters
#include <utils/unicode.h>

#include <component_defines.h>
#include <component_guids.h>
#include <smp_exception.h>

// clang-format on
