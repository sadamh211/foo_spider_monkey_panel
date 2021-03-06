#pragma once

#include <js_objects/object_base.h>

#include <optional>

class JSObject;
struct JSContext;
struct JSClass;

namespace mozjs
{

class JsMeasureStringInfo
    : public JsObjectBase<JsMeasureStringInfo>
{
public:
    static constexpr bool HasProto = true;
    static constexpr bool HasGlobalProto = false;
    static constexpr bool HasProxy = false;
    static constexpr bool HasPostCreate = false;

    static const JSClass JsClass;
    static const JSFunctionSpec* JsFunctions;
    static const JSPropertySpec* JsProperties;
    static const JsPrototypeId PrototypeId;

public:
    ~JsMeasureStringInfo() override = default;

    static std::unique_ptr<JsMeasureStringInfo> CreateNative( JSContext* cx, float x, float y, float w, float h, uint32_t l, uint32_t c );
    static size_t GetInternalSize( float x, float y, float w, float h, uint32_t l, uint32_t c );

public:
    uint32_t get_Chars();
    float get_Height();
    uint32_t get_Lines();
    float get_Width();
    float get_X();
    float get_Y();

private:
    JsMeasureStringInfo( JSContext* cx, float x, float y, float w, float h, uint32_t l, uint32_t c );

private:
    [[maybe_unused]] JSContext* pJsCtx_ = nullptr;

    float x_, y_, w_, h_;
    int lines_, characters_;
};

} // namespace mozjs
