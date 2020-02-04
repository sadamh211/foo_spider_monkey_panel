#include <stdafx.h>

#include "parsed_panel_config.h"

#include <utils/file_helpers.h>

#include <component_paths.h>

#include <nlohmann/json.hpp>

#include <filesystem>
#include <vector>

namespace smp::config
{

ParsedPanelSettings_InMemory ParsedPanelSettings_InMemory::Parse( const PanelSettings_InMemory& settings )
{
    ParsedPanelSettings_InMemory parsedSettings;
    parsedSettings.script = settings.script;
    return parsedSettings;
}

ParsedPanelSettings_File ParsedPanelSettings_File::Parse( const PanelSettings_File& settings )
{
    ParsedPanelSettings_File parsedSettings;
    parsedSettings.scriptPath = settings.path;
    parsedSettings.isSample = false;
    return parsedSettings;
}

ParsedPanelSettings_File ParsedPanelSettings_File::Parse( const PanelSettings_Sample& settings )
{
    namespace fs = std::filesystem;
    try
    {
        ParsedPanelSettings_File parsedSettings;
        parsedSettings.scriptPath = ( fs::u8path( get_fb2k_component_path() ) / "samples" / settings.sampleName ).u8string();
        parsedSettings.isSample = false;
        return parsedSettings;
    }
    catch ( const fs::filesystem_error& e )
    {
        throw SmpException( e.what() );
    }
}

ParsedPanelSettings_Package ParsedPanelSettings_Package::Parse( const PanelSettings_Package& settings )
{
    namespace fs = std::filesystem;
    using json = nlohmann::json;

    SmpException::ExpectTrue( !settings.folderName.empty(), "Corrupted settings (package): `folderName` is empty" );

    try
    {
        const fs::path packageDir = [&settings] {
            fs::path packageDir;
            switch ( settings.location )
            {
            case PackageLocation::Sample:
                packageDir = fs::u8path( get_fb2k_component_path() ) / "samples" / "packages";
                break;
            case PackageLocation::LocalAppData:
                packageDir = fs::u8path( get_profile_path() ) / SMP_UNDERSCORE_NAME / "packages";
                break;
            case PackageLocation::Fb2k:
                packageDir = fs::u8path( get_fb2k_path() ) / SMP_UNDERSCORE_NAME / "packages";
                break;
            default:
                assert( 0 );
                throw SmpException( "Corrupted settings (package): unexpected `location`" );
            }
            return packageDir / settings.folderName;
        }();

        SmpException::ExpectTrue( fs::exists( packageDir ), "Can't find the required package: `{}`", settings.folderName );

        const auto packageJsonFile = packageDir / "package.json";
        SmpException::ExpectTrue( fs::exists( packageJsonFile ), "Corrupted package: can't find `package.json`" );

        ParsedPanelSettings_Package parsedSettings;

        parsedSettings.packagePath = packageDir.u8string();
        parsedSettings.mainScriptPath = ( packageDir / "main.js" ).u8string();
        parsedSettings.isSample = ( settings.location == PackageLocation::Sample );

        const json jsonMain = json::parse( smp::file::ReadFile( packageJsonFile.u8string(), false ) );
        SmpException::ExpectTrue( jsonMain.is_object(), "Corrupted `package.json`: not a JSON object" );

        parsedSettings.name = jsonMain.at( "name" ).get<std::string>();
        parsedSettings.author = jsonMain.at( "author" ).get<std::string>();
        parsedSettings.version = jsonMain.at( "version" ).get<std::string>();
        parsedSettings.description = jsonMain.value( "description", std::string() );
        parsedSettings.enableDragDrop = jsonMain.value( "enableDragDrop", false );
        parsedSettings.shouldGrabFocus = jsonMain.value( "shouldGrabFocus", true );

        if ( jsonMain.find( "menuActions" ) != jsonMain.end() )
        {
            const auto menuActionsJson = jsonMain.at( "menuActions" );
            SmpException::ExpectTrue( menuActionsJson.is_object(), "Corrupted `package.json`: `menuActions` is not a JSON object" );

            MenuActions menuActions;
            for ( const auto& [key, value]: menuActionsJson.items() )
            {
                SmpException::ExpectTrue( !key.empty(), "Corrupted `package.json`: empty key in `menuActions`" );
                menuActions.emplace_back( key, value.empty() ? std::string() : value.get<std::string>() );
            }

            parsedSettings.menuActions = menuActions;
        }

        return parsedSettings;
    }
    catch ( const fs::filesystem_error& e )
    {
        throw SmpException( e.what() );
    }
    catch ( const json::exception& e )
    {
        throw SmpException( fmt::format( "Corrupted `package.json`: {}", e.what() ) );
    }
}

void ParsedPanelSettings_Package::Save() const
{
    namespace fs = std::filesystem;
    using json = nlohmann::json;

    SmpException::ExpectTrue( !packagePath.empty(), "Corrupted settings: `packagePath_` is empty" );

    try
    {
        auto jsonMain = json::object();
        jsonMain.push_back( { "name", name } );
        jsonMain.push_back( { "author", author } );
        jsonMain.push_back( { "version", version } );
        jsonMain.push_back( { "description", description } );
        jsonMain.push_back( { "enableDragDrop", enableDragDrop } );
        jsonMain.push_back( { "shouldGrabFocus", shouldGrabFocus } );

        if ( !menuActions.empty() )
        {
            json menuActionsJson = json::object();
            for ( const auto& [id, desc]: menuActions )
            {
                menuActionsJson.push_back( { id, desc } );
            }

            jsonMain.push_back( { "menuActions", menuActionsJson } );
        }

        const auto packagePathStd = fs::u8path( packagePath );
        if ( !fs::exists( packagePathStd ) )
        {
            fs::create_directories( packagePathStd );
        }

        const auto packageJsonFile = packagePathStd / "package.json";
        smp::file::WriteFile( packageJsonFile.wstring().c_str(), jsonMain.dump( 2 ) );

        const auto mainScriptPathStd = fs::u8path( mainScriptPath );
        if ( !fs::exists( mainScriptPathStd ) )
        {
            smp::file::WriteFile( mainScriptPathStd.c_str(), PanelSettings_InMemory::GetDefaultScript() );
        }
    }
    catch ( const fs::filesystem_error& e )
    {
        throw SmpException( e.what() );
    }
    catch ( const json::exception& e )
    {
        throw SmpException( fmt::format( "Corrupted settings: {}", e.what() ) );
    }
}

} // namespace smp::config
