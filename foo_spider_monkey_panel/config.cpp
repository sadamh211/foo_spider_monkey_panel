#include <stdafx.h>

#include "config.h"

#include <utils/string_helpers.h>

#include <config_binary.h>
#include <config_com.h>
#include <config_json.h>
#include <resource.h>

namespace
{

enum class SettingsType : uint32_t
{
    Binary = 1,
    Json = 2
};

} // namespace

namespace smp::config
{

PanelProperties PanelProperties::FromJson( const std::u8string& jsonString )
{
    return smp::config::json::DeserializeProperties( jsonString );
}

std::u8string PanelProperties::ToJson() const
{
    return smp::config::json::SerializeProperties( *this );
}

PanelProperties PanelProperties::Load( stream_reader& reader, abort_callback& abort, SerializationFormat format )
{
    switch ( format )
    {
    case smp::config::SerializationFormat::Com:
        return smp::config::com::LoadProperties( reader, abort );
    case smp::config::SerializationFormat::Binary:
        return smp::config::binary::LoadProperties( reader, abort );
    case smp::config::SerializationFormat::Json:
        return smp::config::json::LoadProperties( reader, abort );
    default:
    {
        assert( false );
        throw SmpException( "Internal error: unknown serialization format" );
    }
    }
}

void PanelProperties::Save( stream_writer& writer, abort_callback& abort ) const
{
    smp::config::json::SaveProperties( writer, abort, *this );
}

PanelSettings_Simple::PanelSettings_Simple()
{
    ResetToDefault();
}

void PanelSettings_Simple::ResetToDefault()
{
    shouldGrabFocus = true;
    script = GetDefaultScript();
}

std::u8string PanelSettings_Simple::GetDefaultScript()
{
    puResource puRes = uLoadResource( core_api::get_my_instance(), uMAKEINTRESOURCE( IDR_SCRIPT ), "SCRIPT" );
    if ( puRes )
    {
        return std::u8string{ static_cast<const char*>( puRes->GetPointer() ), puRes->GetSize() };
    }
    else
    {
        return std::u8string{};
    }
}

PanelSettings::PanelSettings()
{
    ResetToDefault();
}

void PanelSettings::ResetToDefault()
{
    payload = PanelSettings_Simple();
    isPseudoTransparent = false;
    edgeStyle = EdgeStyle::NoEdge;
    windowPlacement.length = 0;
    // should not fail
    (void)CoCreateGuid( &guid );
}

PanelSettings PanelSettings::Load( stream_reader& reader, size_t size, abort_callback& abort )
{
    try
    {
        // TODO: remove, since we don't want to change type probably
        PanelSettings panelSettings;

        if ( size < sizeof( SettingsType ) )
        { // probably no config at all
            return panelSettings;
        }

        uint32_t ver;
        reader.read_object_t( ver, abort );

        switch ( static_cast<SettingsType>( ver ) )
        {
        case SettingsType::Binary:
            return smp::config::binary::LoadSettings( reader, abort );
        case SettingsType::Json:
            return smp::config::json::LoadSettings( reader, abort );
        default:
            throw SmpException( fmt::format( "Unexpected panel settings format: {}", ver ) );
        }
    }
    catch ( const pfc::exception& e )
    {
        throw SmpException( e.what() );
    }
}

void PanelSettings::Save( stream_writer& writer, abort_callback& abort ) const
{
    writer.write_object_t( static_cast<uint32_t>( SettingsType::Json ), abort );
    smp::config::json::SaveSettings( writer, abort, *this );
}

void PanelSettings::SaveDefault( stream_writer& writer, abort_callback& abort )
{
    PanelSettings{}.Save( writer, abort );
}

} // namespace smp::config
