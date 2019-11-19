#include <stdafx.h>

#include "config_com.h"

#include <utils/string_helpers.h>
#include <utils/type_traits_x.h>

namespace smp::config::com
{

PanelProperties LoadProperties( stream_reader& reader, abort_callback& abort )
{
    PanelProperties properties;

    try
    {
        t_size count;
        reader.read_lendian_t( count, abort );

        for ( t_size i = 0; i < count; ++i )
        {
            const std::u8string u8propName = smp::string::Trim<char8_t>( smp::pfc_x::ReadString( reader, abort ) );

            VARTYPE vt;
            reader.read_lendian_t( vt, abort );

            mozjs::SerializedJsValue serializedValue;
            switch ( vt )
            {
            case VT_UI1:
            case VT_I1:
            {
                int8_t val;
                reader.read( &val, sizeof( val ), abort );
                serializedValue = static_cast<int32_t>( val );
                break;
            }
            case VT_I2:
            case VT_UI2:
            {
                int16_t val;
                reader.read( &val, sizeof( val ), abort );
                serializedValue = static_cast<int32_t>( val );
                break;
            }

            case VT_BOOL:
            {
                int16_t val;
                reader.read( &val, sizeof( val ), abort );
                serializedValue = !!val;
                break;
            }
            case VT_I4:
            case VT_UI4:
            case VT_INT:
            case VT_UINT:
            {
                int32_t val;
                reader.read( &val, sizeof( val ), abort );
                serializedValue = val;
                break;
            }
            case VT_R4:
            {
                float val;
                reader.read( &val, sizeof( val ), abort );
                serializedValue = static_cast<double>( val );
                break;
            }
            case VT_I8:
            case VT_UI8:
            {
                int64_t val;
                reader.read( &val, sizeof( val ), abort );
                serializedValue = static_cast<int32_t>( val );
                break;
            }
            case VT_R8:
            case VT_CY:
            case VT_DATE:
            {
                double val;
                reader.read( &val, sizeof( val ), abort );
                serializedValue = val;
                break;
            }
            case VT_BSTR:
            {
                serializedValue = smp::pfc_x::ReadString( reader, abort );
                break;
            }
            default:
            {
                continue;
            }
            }

            properties.values.emplace( smp::unicode::ToWide( u8propName ), std::make_shared<mozjs::SerializedJsValue>( serializedValue ) );
        }

        return properties;
    }
    catch ( const pfc::exception& e )
    {
        throw SmpException( e.what() );
    }
}

} // namespace smp::config::com
