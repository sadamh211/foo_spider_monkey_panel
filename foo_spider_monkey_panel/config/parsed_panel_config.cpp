#include <stdafx.h>

#include "parsed_panel_config.h"

#include <component_paths.h>

#include <filesystem>

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
    // TODO: impl
    ParsedPanelSettings_Package parsedSettings;
    return parsedSettings;
}

} // namespace smp::config
