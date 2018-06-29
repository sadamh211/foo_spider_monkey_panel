#include <stdafx.h>

#include "fb_file_info.h"

#include <js_engine/js_to_native_invoker.h>
#include <js_utils/js_error_helper.h>
#include <js_utils/js_object_helper.h>


namespace
{

using namespace mozjs;

JSClassOps jsOps = {
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    JsFinalizeOp<JsFbFileInfo>,
    nullptr,
    nullptr,
    nullptr,
    nullptr
};

JSClass jsClass = {
    "FbFileInfo",
    JSCLASS_HAS_PRIVATE | JSCLASS_FOREGROUND_FINALIZE,
    &jsOps
};

MJS_WRAP_NATIVE_FN( JsFbFileInfo, InfoFind );
MJS_WRAP_NATIVE_FN( JsFbFileInfo, InfoName );
MJS_WRAP_NATIVE_FN( JsFbFileInfo, InfoValue );
MJS_WRAP_NATIVE_FN( JsFbFileInfo, MetaFind );
MJS_WRAP_NATIVE_FN( JsFbFileInfo, MetaName );
MJS_WRAP_NATIVE_FN( JsFbFileInfo, MetaValue );
MJS_WRAP_NATIVE_FN( JsFbFileInfo, MetaValueCount );

const JSFunctionSpec jsFunctions[] = {
    JS_FN( "InfoFind",       InfoFind      , 1, 0 ),
    JS_FN( "InfoName",       InfoName      , 1, 0 ),
    JS_FN( "InfoValue",      InfoValue     , 1, 0 ),
    JS_FN( "MetaFind",       MetaFind      , 1, 0 ),
    JS_FN( "MetaName",       MetaName      , 1, 0 ),
    JS_FN( "MetaValue",      MetaValue     , 2, 0 ),
    JS_FN( "MetaValueCount", MetaValueCount, 1, 0 ),
    JS_FS_END
};

MJS_WRAP_NATIVE_FN( JsFbFileInfo, get_InfoCount );
MJS_WRAP_NATIVE_FN( JsFbFileInfo, get_MetaCount );


const JSPropertySpec jsProperties[] = {
    
    JS_PSG( "get_InfoCount", get_InfoCount , 0 ),
    JS_PSG( "get_MetaCount", get_MetaCount , 0 ),
    JS_PS_END
};

}

namespace mozjs
{

JsFbFileInfo::JsFbFileInfo( JSContext* cx, file_info_impl* pFileInfo )
    : pJsCtx_( cx )
    , fileInfo_( pFileInfo )
{

}

JsFbFileInfo::~JsFbFileInfo()
{
}

JSObject* JsFbFileInfo::Create( JSContext* cx, file_info_impl* pFileInfo )
{
    if ( !pFileInfo )
    {
        JS_ReportErrorASCII( cx, "Internal error: file_info object is null" );
        return nullptr;
    }

    JS::RootedObject jsObj( cx,
                            JS_NewObject( cx, &jsClass ) );
    if ( !jsObj )
    {
        return nullptr;
    }

    if ( !JS_DefineFunctions( cx, jsObj, jsFunctions )
         || !JS_DefineProperties( cx, jsObj, jsProperties ) )
    {
        return nullptr;
    }

    JS_SetPrivate( jsObj, new JsFbFileInfo( cx, pFileInfo ) );

    return jsObj;
}

const JSClass& JsFbFileInfo::GetClass()
{
    return jsClass;
}

std::optional<int32_t> 
JsFbFileInfo::InfoFind( std::string name )
{
    assert( fileInfo_ );

    return fileInfo_->info_find_ex( name.c_str(), name.length() );
}

std::optional<std::string> 
JsFbFileInfo::InfoName( uint32_t index )
{
    assert( fileInfo_ );

    if ( index >= fileInfo_->info_get_count() )
    {
        JS_ReportErrorASCII( pJsCtx_, "Index is out of bounds" );
        return std::nullopt;
    }

    return fileInfo_->info_enum_name( index );
}

std::optional<std::string> 
JsFbFileInfo::InfoValue( uint32_t index )
{
    assert( fileInfo_ );

    if ( index >= fileInfo_->info_get_count() )
    {
        JS_ReportErrorASCII( pJsCtx_, "Index is out of bounds" );
        return std::nullopt;
    }

    return fileInfo_->info_enum_value( index );
}

std::optional<int32_t> 
JsFbFileInfo::MetaFind( std::string name )
{
    assert( fileInfo_ );

    t_size idx = fileInfo_->meta_find_ex( name.c_str(), name.length() );
    if ( idx == pfc_infinite )
    {
        return -1;
    }

    return static_cast<int32_t>( idx );
}

std::optional<std::string> 
JsFbFileInfo::MetaName( uint32_t index )
{
    assert( fileInfo_ );

    if ( index >= fileInfo_->info_get_count() )
    {
        JS_ReportErrorASCII( pJsCtx_, "Index is out of bounds" );
        return std::nullopt;
    }

    return fileInfo_->meta_enum_name( index );
}

std::optional<std::string> 
JsFbFileInfo::MetaValue( uint32_t infoIndex, uint32_t valueIndex )
{
    assert( fileInfo_ );

    if ( infoIndex >= fileInfo_->info_get_count()
         || valueIndex >= fileInfo_->meta_enum_value_count( infoIndex ) )
    {
        JS_ReportErrorASCII( pJsCtx_, "Index is out of bounds" );
        return std::nullopt;
    }

    return fileInfo_->meta_enum_value( infoIndex, valueIndex );
}

std::optional<uint32_t> 
JsFbFileInfo::MetaValueCount( uint32_t index )
{
    assert( fileInfo_ );

    if ( index >= fileInfo_->info_get_count() )
    {
        JS_ReportErrorASCII( pJsCtx_, "Index is out of bounds" );
        return std::nullopt;
    }

    return fileInfo_->meta_enum_value_count( index );
}

std::optional<uint32_t> 
JsFbFileInfo::get_InfoCount()
{
    assert( fileInfo_ );
    return fileInfo_->info_get_count();
}

std::optional<uint32_t> 
JsFbFileInfo::get_MetaCount()
{
    assert( fileInfo_ );
    return fileInfo_->meta_get_count();
}

}
