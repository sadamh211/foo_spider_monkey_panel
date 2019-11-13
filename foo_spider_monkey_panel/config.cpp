#include <stdafx.h>

#include "config.h"

#include <utils/string_helpers.h>

#include <config_legacy.h>
#include <resource.h>

#include <nlohmann/json.hpp>

namespace
{

constexpr const char kPropJsonConfigVersion[] = "1";
constexpr const char kPropJsonConfigId[] = "properties";

constexpr const char kSettingsJsonConfigVersion[] = "1";
constexpr const char kSettingsJsonConfigId[] = "settings";

enum class ScriptType : uint8_t
{
    Simple = 1,
    Package = 2
};

enum class SettingsType : uint32_t
{
    Binary = 1,
    Json = 2
};

} // namespace

namespace smp::config
{

PanelProperties PanelProperties::LoadJson( stream_reader& reader, abort_callback& abort, bool loadRawString )
{
    try
    {
        std::u8string jsonStr;
        if ( loadRawString )
        {
            jsonStr = smp::pfc_x::ReadRawString( reader, abort );
        }
        else
        {
            jsonStr = smp::pfc_x::ReadString( reader, abort );
        }

        return FromJson( jsonStr );
    }
    catch ( const pfc::exception& e )
    {
        throw SmpException( e.what() );
    }
}

PanelProperties PanelProperties::LoadBinary( stream_reader& reader, abort_callback& abort )
{
    return smp::config::binary::LoadProperties( reader, abort );
}

PanelProperties PanelProperties::LoadCom( stream_reader& reader, abort_callback& abort )
{
    return smp::config::com::LoadProperties( reader, abort );
}

PanelProperties PanelProperties::LoadLegacy( stream_reader& reader, abort_callback& abort )
{
    try
    {
        return smp::config::binary::LoadProperties( reader, abort );
    }
    catch ( const SmpException& )
    {
        return smp::config::com::LoadProperties( reader, abort );
    }   
}

void PanelProperties::SaveJson( stream_writer& writer, abort_callback& abort, bool saveAsRawString ) const
{
    try
    {
        const auto jsonStr = ToJson();
        if ( saveAsRawString )
        {
            pfc_x::WriteStringRaw( writer, jsonStr, abort );
        }
        else
        {
            pfc_x::WriteString( writer, jsonStr, abort );
        }
    }
    catch ( const pfc::exception& e )
    {
        throw SmpException( e.what() );
    }
}

PanelProperties PanelProperties::FromJson( const std::u8string& jsonString )
{
    using json = nlohmann::json;

    PanelProperties properties;

    try
    {
        json jsonMain = json::parse( jsonString );
        if ( !jsonMain.is_object() )
        {
            throw SmpException( "Corrupted serialized properties: not a JSON object" );
        }

        if ( jsonMain.at( "version" ).get<std::string>() != kPropJsonConfigVersion
             || jsonMain.at( "id" ).get<std::string>() != kPropJsonConfigId )
        {
            throw SmpException( "Corrupted serialized properties: version/id mismatch" );
        }

        auto& jsonValues = jsonMain.at( "values" );
        if ( !jsonValues.is_object() )
        {
            throw SmpException( "Corrupted serialized properties: values" );
        }

        for ( auto& [key, value]: jsonValues.items() )
        {
            if ( key.empty() )
            {
                throw SmpException( "Corrupted serialized properties: empty key" );
            }

            mozjs::SerializedJsValue serializedValue;
            if ( value.is_boolean() )
            {
                serializedValue = value.get<bool>();
            }
            else if ( value.is_number_integer() )
            {
                serializedValue = value.get<int32_t>();
            }
            else if ( value.is_number_float() )
            {
                serializedValue = value.get<double>();
            }
            else if ( value.is_string() )
            {
                serializedValue = value.get<std::string>();
            }
            else
            {
                assert( 0 );
                continue;
            }

            properties.values.emplace( smp::unicode::ToWide( key ), std::make_shared<mozjs::SerializedJsValue>( serializedValue ) );
        }

        return properties;
    }
    catch ( const json::exception& e )
    {
        throw SmpException( e.what() );
    }
    catch ( const pfc::exception& e )
    {
        throw SmpException( e.what() );
    }
}

std::u8string PanelProperties::ToJson() const
{
    using json = nlohmann::json;

    try
    {
        json jsonMain = json::object( { { "id", kPropJsonConfigId },
                                        { "version", kPropJsonConfigVersion } } );

        json jsonValues = json::object();
        for ( const auto& [nameW, pValue]: values )
        {
            const auto propertyName = smp::unicode::ToU8( nameW );
            const auto& serializedValue = *pValue;

            std::visit( [&jsonValues, &propertyName]( auto&& arg ) {
                jsonValues.push_back( { propertyName, arg } );
            },
                        serializedValue );
        }

        jsonMain.push_back( { "values", jsonValues } );

        return jsonMain.dump();
    }
    catch ( const json::exception& e )
    {
        throw SmpException( e.what() );
    }
    catch ( const pfc::exception& e )
    {
        throw SmpException( e.what() );
    }
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
    edgeStyle = EdgeStyle::NO_EDGE;
    windowPlacement.length = 0;
    // should not fail
    (void)CoCreateGuid( &guid );
}

PanelSettings PanelSettings::LoadBinary( stream_reader& reader, t_size size, abort_callback& abort )
{
    PanelSettings panelSettings;

    if ( size < sizeof( SettingsType ) )
    { // probably no config at all
        return panelSettings;
    }

    try
    {
        PanelSettings_Simple payload;

        uint32_t version = 0;
        reader.read_object_t( version, abort );
        if ( version != static_cast<uint32_t>( SettingsType::Binary ) )
        {
            throw SmpException( "Not a binary serialized settings" );
        }
        reader.skip_object( sizeof( false ), abort ); // HACK: skip over "delay load"
        reader.read_object_t( panelSettings.guid, abort );
        reader.read_object( &panelSettings.edgeStyle, sizeof( panelSettings.edgeStyle ), abort );
        panelSettings.properties = PanelProperties::LoadBinary( reader, abort );
        reader.skip_object( sizeof( false ), abort ); // HACK: skip over "disable before"
        reader.read_object_t( payload.shouldGrabFocus, abort );
        reader.read_object( &panelSettings.windowPlacement, sizeof( panelSettings.windowPlacement ), abort );
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

PanelSettings PanelSettings::LoadJson( stream_reader& reader, t_size size, abort_callback& abort )
{
    using json = nlohmann::json;

    // TODO: remove, since we don't want to change type probably
    PanelSettings panelSettings;

    if ( size < sizeof( SettingsType ) )
    { // probably no config at all
        return panelSettings;
    }

    try
    {
        uint32_t ver = 0;
        reader.read_object_t( ver, abort );
        if ( ver != static_cast<uint32_t>( SettingsType::Json ) )
        {
            throw SmpException( "Not a JSON serialized settings" );
        }

        const json jsonMain = json::parse( smp::pfc_x::ReadString( reader, abort ) );
        if ( !jsonMain.is_object() )
        {
            throw SmpException( "Corrupted serialized settings: not a JSON object" );
        }

        if ( jsonMain.at( "version" ).get<std::string>() != kSettingsJsonConfigVersion
             || jsonMain.at( "id" ).get<std::string>() != kSettingsJsonConfigId )
        {
            throw SmpException( "Corrupted serialized settings: version/id mismatch" );
        }

        const auto scriptType = jsonMain.at( "scriptType" ).get<ScriptType>();
        const auto jsonPayload = jsonMain.at( "payload" );
        switch ( scriptType )
        {
        case ScriptType::Simple:
        {
            PanelSettings_Simple payload;
            payload.script = jsonPayload.at( "scriptType" ).get<std::string>();
            payload.shouldGrabFocus = jsonPayload.at( "shouldGrabFocus" ).get<bool>();

            panelSettings.payload = payload;
        }
        case ScriptType::Package:
        {
            PanelSettings_Package payload;
            payload.packageName = jsonPayload.at( "packageName" ).get<std::string>();
            payload.location = jsonPayload.at( "location" ).get<int>();

            panelSettings.payload = payload;
        }
        default:
        {
            throw smp::SmpException( "Corrupted serialized settings: unknown script type" );
        }
        }

        panelSettings.properties = PanelProperties::FromJson( jsonMain.at( "properties" ).get<std::string>() );
        panelSettings.edgeStyle = static_cast<EdgeStyle>( jsonMain.value( "properties", static_cast<uint8_t>( EdgeStyle::Default ) ) );
        panelSettings.isPseudoTransparent = jsonMain.value( "isPseudoTransparent", false );

        WINDOWPLACEMENT windowPlacement;
        std::fill( &windowPlacement, &windowPlacement + sizeof( windowPlacement ), 0 );
        if ( jsonMain.contains( "windowPlacement" ) )
        {
            const auto windowPlacementJson = jsonMain.at( "windowPlacement" );
            windowPlacement.length = jsonMain.value( "length", UINT{} );
            windowPlacement.flags = jsonMain.value( "flags", UINT{} );
            windowPlacement.showCmd = jsonMain.value( "showCmd", UINT{} );
            windowPlacement.ptMaxPosition = POINT{ jsonMain.value( "ptMaxPosition.x", LONG{} ), jsonMain.value( "ptMaxPosition.y", LONG{} ) };
            windowPlacement.ptMinPosition = POINT{ jsonMain.value( "ptMinPosition.x", LONG{} ), jsonMain.value( "ptMinPosition.y", LONG{} ) };
            windowPlacement.rcNormalPosition = RECT{ jsonMain.value( "rcNormalPosition.left", LONG{} ),
                                                     jsonMain.value( "rcNormalPosition.top", LONG{} ),
                                                     jsonMain.value( "rcNormalPosition.right", LONG{} ),
                                                     jsonMain.value( "rcNormalPosition.bottom", LONG{} ) };
        }
        panelSettings.windowPlacement = windowPlacement;

        return panelSettings;
    }
    catch ( const json::exception& e )
    {
        throw SmpException( e.what() );
    }
    catch ( const pfc::exception& e )
    {
        throw SmpException( e.what() );
    }
}

void PanelSettings::SaveDefault( stream_writer& writer, abort_callback& abort )
{
    PanelSettings{}.SaveJson( writer, abort );
}

void PanelSettings::SaveJson( stream_writer& writer, abort_callback& abort ) const
{
    using json = nlohmann::json;

    try
    {
        json jsonMain = json::object( { { "id", kSettingsJsonConfigId },
                                        { "version", kSettingsJsonConfigVersion } } );

        const auto scriptType = ( std::holds_alternative<PanelSettings_Simple>( payload ) ? ScriptType::Simple : ScriptType::Package );
        jsonMain.push_back( { "scriptType", static_cast<uint8_t>( scriptType ) } );

        json jsonPayload = json::object();
        switch ( scriptType )
        {
        case ScriptType::Simple:
        {
            auto& simpleConfig = std::get<PanelSettings_Simple>( payload );
            jsonPayload.push_back( { { "script", simpleConfig.script },
                                     { "shouldGrabFocus", simpleConfig.shouldGrabFocus } } );
        }
        case ScriptType::Package:
        {
            auto& packageConfig = std::get<PanelSettings_Package>( payload );
            jsonPayload.push_back( { { "packageName", packageConfig.packageName },
                                     { "location", packageConfig.location } } );
        }
        }

        jsonMain.push_back( { { "payload", jsonPayload },
                              { "properties", properties.ToJson() },
                              { "isPseudoTransparent", isPseudoTransparent } } );

        json jsonWindowPlacement = json::object();
        jsonWindowPlacement.push_back( { { "length", windowPlacement.length },
                                         { "flags", windowPlacement.flags },
                                         { "showCmd", windowPlacement.showCmd },
                                         { "ptMaxPosition.x", windowPlacement.ptMaxPosition.x },
                                         { "ptMaxPosition.y", windowPlacement.ptMaxPosition.y },
                                         { "ptMinPosition.x", windowPlacement.ptMinPosition.x },
                                         { "ptMinPosition.y", windowPlacement.ptMinPosition.y },
                                         { "rcNormalPosition.left", windowPlacement.rcNormalPosition.left },
                                         { "rcNormalPosition.top", windowPlacement.rcNormalPosition.top },
                                         { "rcNormalPosition.right", windowPlacement.rcNormalPosition.right },
                                         { "rcNormalPosition.bottom", windowPlacement.rcNormalPosition.bottom } } );

        jsonMain.push_back( { "windowPlacement", jsonWindowPlacement } );

        writer.write_object_t( static_cast<uint32_t>( SettingsType::Json ), abort );
        pfc_x::WriteString( writer, jsonMain.dump(), abort );
    }
    catch ( const json::exception& e )
    {
        throw SmpException( e.what() );
    }
    catch ( const pfc::exception& e )
    {
        throw SmpException( e.what() );
    }
}

} // namespace smp::config
