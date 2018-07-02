#pragma once

#pragma warning( push )  
#pragma warning( disable : 4251 ) // dll interface warning
#pragma warning( disable : 4996 ) // C++17 deprecation warning
#include <jsapi.h>
#pragma warning( pop )  

namespace mozjs
{

// TODO: think about adding JS_DefineFunctionsWithHelp()
// TODO: nargs == args before the first default

bool DefineConsole( JSContext* cx, JS::HandleObject global );

}
