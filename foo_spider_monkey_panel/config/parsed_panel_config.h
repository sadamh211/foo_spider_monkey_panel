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
    // TODO: handle this
    bool isSample = false;

    [[nodiscard]] static ParsedPanelSettings_File Parse( const PanelSettings_File& settings );
    /// @throw smp::SmpException
    [[nodiscard]] static ParsedPanelSettings_File Parse( const PanelSettings_Sample& settings );
};

struct ParsedPanelSettings_Package
{
    using MenuAction = std::pair<std::string, std::string>;
    using MenuActions = std::vector<MenuAction>;

    std::u8string packagePath;
    std::u8string mainScriptPath;
    // TODO: handle this
    bool isSample;

    std::u8string name;
    std::u8string version;
    std::u8string author;
    std::u8string description;

    bool shouldGrabFocus = true;
    bool enableDragDrop = false;
    MenuActions menuActions;

    /// @throw smp::SmpException
    [[nodiscard]] static ParsedPanelSettings_Package Parse( const PanelSettings_Package& settings );
    /// @throw smp::SmpException
    void Save() const;
};

using ParsedPanelSettings = std::variant<ParsedPanelSettings_InMemory, ParsedPanelSettings_File, ParsedPanelSettings_Package>;

} // namespace smp::config
