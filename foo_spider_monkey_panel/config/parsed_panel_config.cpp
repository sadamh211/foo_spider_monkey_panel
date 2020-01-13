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
                packageDir = fs::u8path( get_profile_path() ) / "foo_spider_monkey_panel" / "packages";
                break;
            case PackageLocation::Fb2k:
                packageDir = fs::u8path( get_fb2k_path() ) / "foo_spider_monkey_panel" / "packages";
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

            std::vector<std::pair<std::string, std::string>> menuActions;
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

} // namespace smp::config
