#pragma once

#include <config/panel_config.h>

#include <string>
#include <variant>
#include <vector>

namespace smp::config
{

struct ParsedPanelSettings_InMemory
{
    std::u8string script;

    [[nodiscard]] static ParsedPanelSettings_InMemory Parse( const PanelSettings_InMemory& settings );
};

struct ParsedPanelSettings_File
{
    std::u8string scriptPath;
    bool isSample = false;

    [[nodiscard]] static ParsedPanelSettings_File Parse( const PanelSettings_File& settings );
    /// @throw smp::SmpException
    [[nodiscard]] static ParsedPanelSettings_File Parse( const PanelSettings_Sample& settings );
};

struct ParsedPanelSettings_Package
{
    std::u8string packagePath;
    std::u8string mainScriptPath;

    std::u8string name;
    std::u8string version;
    std::u8string author;
    std::u8string description;

    bool shouldGrabFocus = true;
    bool enableDragDrop = false;
    std::vector<std::pair<std::string, std::string>> menuActions;

    /// @throw smp::SmpException
    [[nodiscard]] static ParsedPanelSettings_Package Parse( const PanelSettings_Package& settings );
};

using ParsedPanelSettings = std::variant<ParsedPanelSettings_InMemory, ParsedPanelSettings_File, ParsedPanelSettings_Package>;

} // namespace smp::config
