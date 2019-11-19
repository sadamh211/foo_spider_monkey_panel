#include <stdafx.h>

#include "config_json.h"

#include <utils/string_helpers.h>

#include <nlohmann/json.hpp>

namespace
{

enum class ScriptType : uint8_t
{
    Simple = 1,
    Package = 2
};

constexpr const char kPropJsonConfigVersion[] = "1";
constexpr const char kPropJsonConfigId[] = "properties";

constexpr const char kSettingsJsonConfigVersion[] = "1";
constexpr const char kSettingsJsonConfigId[] = "settings";

} // namespace

namespace smp::config::json
{

PanelSettings LoadSettings( stream_reader& reader, abort_callback& abort )
{
    using json = nlohmann::json;

    // TODO: remove, since we don't want to change type probably
    PanelSettings panelSettings;

    try
    {
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
            payload.script = jsonPayload.at( "script" ).get<std::string>();
            payload.shouldGrabFocus = jsonPayload.at( "shouldGrabFocus" ).get<bool>();

            panelSettings.payload = payload;
            break;
        }
        case ScriptType::Package:
        {
            PanelSettings_Package payload;
            payload.packageName = jsonPayload.at( "packageName" ).get<std::string>();
            payload.location = jsonPayload.at( "location" ).get<PackageLocation>();
            // TODO: validate location

            panelSettings.payload = payload;
            break;
        }
        default:
        {
            throw smp::SmpException( "Corrupted serialized settings: unknown script type" );
        }
        }

        panelSettings.properties = PanelProperties::FromJson( jsonMain.at( "properties" ).get<std::string>() );
        panelSettings.edgeStyle = static_cast<EdgeStyle>( jsonMain.value( "edgeStyle", static_cast<uint8_t>( EdgeStyle::Default ) ) );
        panelSettings.isPseudoTransparent = jsonMain.value( "isPseudoTransparent", false );

        WINDOWPLACEMENT windowPlacement{};
        if ( jsonMain.contains( "windowPlacement" ) )
        {
            const auto wndPlcJson = jsonMain.at( "windowPlacement" );
            windowPlacement.length = wndPlcJson.value( "length", UINT{} );
            windowPlacement.flags = wndPlcJson.value( "flags", UINT{} );
            windowPlacement.showCmd = wndPlcJson.value( "showCmd", UINT{} );
            windowPlacement.ptMaxPosition = POINT{ wndPlcJson.value( "ptMaxPosition.x", LONG{} ),
                                                   wndPlcJson.value( "ptMaxPosition.y", LONG{} ) };
            windowPlacement.ptMinPosition = POINT{ wndPlcJson.value( "ptMinPosition.x", LONG{} ),
                                                   wndPlcJson.value( "ptMinPosition.y", LONG{} ) };
            windowPlacement.rcNormalPosition = RECT{ wndPlcJson.value( "rcNormalPosition.left", LONG{} ),
                                                     wndPlcJson.value( "rcNormalPosition.top", LONG{} ),
                                                     wndPlcJson.value( "rcNormalPosition.right", LONG{} ),
                                                     wndPlcJson.value( "rcNormalPosition.bottom", LONG{} ) };
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

void SaveSettings( stream_writer& writer, abort_callback& abort, const PanelSettings& settings )
{
    using json = nlohmann::json;

    try
    {
        auto jsonMain = json::object();
        jsonMain.push_back( { "id", kSettingsJsonConfigId } );
        jsonMain.push_back( { "version", kSettingsJsonConfigVersion } );

        const auto scriptType = ( std::holds_alternative<PanelSettings_Simple>( settings.payload ) ? ScriptType::Simple : ScriptType::Package );
        jsonMain.push_back( { "scriptType", static_cast<uint8_t>( scriptType ) } );

        json jsonPayload = json::object();
        switch ( scriptType )
        {
        case ScriptType::Simple:
        {
            auto& simpleConfig = std::get<PanelSettings_Simple>( settings.payload );
            jsonPayload.push_back( { "script", simpleConfig.script } );
            jsonPayload.push_back( { "shouldGrabFocus", simpleConfig.shouldGrabFocus } );
            break;
        }
        case ScriptType::Package:
        {
            auto& packageConfig = std::get<PanelSettings_Package>( settings.payload );
            jsonPayload.push_back( { "packageName", packageConfig.packageName } );
            jsonPayload.push_back( { "location", packageConfig.location } );
            break;
        }
        }

        jsonMain.push_back( { "payload", jsonPayload } );
        jsonMain.push_back( { "properties", settings.properties.ToJson() } );
        jsonMain.push_back( { "isPseudoTransparent", settings.isPseudoTransparent } );

        auto jsonWindowPlacement = json::object();
        jsonWindowPlacement.push_back( { "length", settings.windowPlacement.length } );
        jsonWindowPlacement.push_back( { "flags", settings.windowPlacement.flags } );
        jsonWindowPlacement.push_back( { "showCmd", settings.windowPlacement.showCmd } );
        jsonWindowPlacement.push_back( { "ptMaxPosition.x", settings.windowPlacement.ptMaxPosition.x } );
        jsonWindowPlacement.push_back( { "ptMaxPosition.y", settings.windowPlacement.ptMaxPosition.y } );
        jsonWindowPlacement.push_back( { "ptMinPosition.x", settings.windowPlacement.ptMinPosition.x } );
        jsonWindowPlacement.push_back( { "ptMinPosition.y", settings.windowPlacement.ptMinPosition.y } );
        jsonWindowPlacement.push_back( { "rcNormalPosition.left", settings.windowPlacement.rcNormalPosition.left } );
        jsonWindowPlacement.push_back( { "rcNormalPosition.top", settings.windowPlacement.rcNormalPosition.top } );
        jsonWindowPlacement.push_back( { "rcNormalPosition.right", settings.windowPlacement.rcNormalPosition.right } );
        jsonWindowPlacement.push_back( { "rcNormalPosition.bottom", settings.windowPlacement.rcNormalPosition.bottom } );

        jsonMain.push_back( { "windowPlacement", jsonWindowPlacement } );

        pfc_x::WriteString( writer, abort, jsonMain.dump() );
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

PanelProperties LoadProperties( stream_reader& reader, abort_callback& abort )
{
    try
    {
        return DeserializeProperties( smp::pfc_x::ReadString( reader, abort ) );
    }
    catch ( const pfc::exception& e )
    {
        throw SmpException( e.what() );
    }
}

void SaveProperties( stream_writer& writer, abort_callback& abort, const PanelProperties& properties )
{
    try
    {
        pfc_x::WriteString( writer, abort, SerializeProperties( properties ) );
    }
    catch ( const pfc::exception& e )
    {
        throw SmpException( e.what() );
    }
}

PanelProperties DeserializeProperties( const std::u8string& str )
{
    using json = nlohmann::json;

    PanelProperties properties;

    try
    {
        json jsonMain = json::parse( str );
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

std::u8string SerializeProperties( const PanelProperties& properties )
{
    using json = nlohmann::json;

    try
    {
        json jsonMain = json::object( { { "id", kPropJsonConfigId },
                                        { "version", kPropJsonConfigVersion } } );

        json jsonValues = json::object();
        for ( const auto& [nameW, pValue]: properties.values )
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

} // namespace smp::config::json
