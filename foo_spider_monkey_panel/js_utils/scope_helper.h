#pragma once

#include <js_utils/js_error_helper.h>

struct JSContext;

namespace mozjs
{

class JsAutoRealmWithErrorReport
{
public:
    JsAutoRealmWithErrorReport( JSContext* cx, JS::HandleObject global )
        : ar_( cx, global )
        , ajsr_( cx )
    {
    }

    JsAutoRealmWithErrorReport( const JsAutoRealmWithErrorReport& ) = delete;
    JsAutoRealmWithErrorReport& operator=( const JsAutoRealmWithErrorReport& ) = delete;

    void DisableReport()
    {
        ajsr_.Disable();
    }

private:
    JSAutoRealm ar_;
    mozjs::error::AutoJsReport ajsr_;
};

} // namespace mozjs
