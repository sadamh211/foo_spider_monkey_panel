#include <stdafx.h>

#include "config_binary.h"

#include <utils/string_helpers.h>

namespace
{

enum class JsValueType : uint32_t
{ // Take care changing this: used in config
    pt_boolean = 0,
    pt_int32 = 1,
    pt_double = 2,
    pt_string = 3,
};

}

namespace smp::config::binary
{

PanelSettings LoadSettings( stream_reader& reader, abort_callback& abort )
{
    try
    {
        PanelSettings panelSettings;
        PanelSettings_Simple payload;

        reader.skip_object( sizeof( false ), abort ); // skip "delay load"
        reader.read_object_t( panelSettings.guid, abort );
        reader.read_object( &panelSettings.edgeStyle, sizeof( panelSettings.edgeStyle ), abort );
        panelSettings.properties = LoadProperties( reader, abort );
        reader.skip_object( sizeof( false ), abort ); // skip "disable before"
        reader.read_object_t( payload.shouldGrabFocus, abort );
        reader.skip_object( sizeof( WINDOWPLACEMENT ), abort ); // skip WINDOWPLACEMENT
        payload.script = smp::pfc_x::ReadString( reader, abort );
        reader.read_object_t( panelSettings.isPseudoTransparent, abort );
        panelSettings.payload = payload;

        return panelSettings;
    }
    catch ( const pfc::exception& e )
    {
        throw SmpException( e.what() );
    }
}

PanelProperties LoadProperties( stream_reader& reader, abort_callback& abort )
{
    try
    {
        PanelProperties properties;

        uint32_t count;
        reader.read_lendian_t( count, abort );

        for ( uint32_t i = 0; i < count; ++i )
        {
            mozjs::SerializedJsValue serializedValue;

            const std::u8string u8PropName = smp::pfc_x::ReadString( reader, abort );

            uint32_t valueType;
            reader.read_lendian_t( valueType, abort );

            switch ( static_cast<JsValueType>( valueType ) )
            {
            case JsValueType::pt_boolean:
            {
                bool value;
                reader.read_lendian_t( value, abort );
                serializedValue = value;
                break;
            }
            case JsValueType::pt_int32:
            {
                int32_t value;
                reader.read_lendian_t( value, abort );
                serializedValue = value;
                break;
            }
            case JsValueType::pt_double:
            {
                double value;
                reader.read_lendian_t( value, abort );
                serializedValue = value;
                break;
            }
            case JsValueType::pt_string:
            {
                serializedValue = smp::pfc_x::ReadString( reader, abort );
                break;
            }
            default:
            {
                assert( 0 );
                continue;
            }
            }

            properties.values.emplace( smp::unicode::ToWide( u8PropName ), std::make_shared<mozjs::SerializedJsValue>( serializedValue ) );
        }

        return properties;
    }
    catch ( const pfc::exception& e )
    {
        throw SmpException( e.what() );
    }
}

} // namespace smp::config::binary
