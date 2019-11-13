#pragma once

#include <js_utils/serialized_value.h>

#include <optional>

namespace smp::config
{

enum class ScriptType : uint8_t
{
    Simple = 0,
    Package = 1
};

enum class SimpleScriptSource : uint8_t
{
    Sample = 0,
    File = 1,
    InMemory = 2
};

enum class PackageScriptLocation : uint8_t
{
    Sample = 0,
    LocalAppData = 1,
    Fb2k = 2,
};

enum class EdgeStyle : uint8_t
{
    NO_EDGE = 0,
    SUNKEN_EDGE,
    GREY_EDGE,
    Default = NO_EDGE,
};

struct PanelProperties
{
    using PropertyMap = std::unordered_map<std::wstring, std::shared_ptr<mozjs::SerializedJsValue>>;
    PropertyMap values;

public:
    /// @throw smp::SmpException
    static PanelProperties LoadJson( stream_reader& reader, abort_callback& abort, bool loadRawString = false );

    /// @throw smp::SmpException
    static PanelProperties LoadBinary( stream_reader& reader, abort_callback& abort );

    /// @throw smp::SmpException
    static PanelProperties LoadCom( stream_reader& reader, abort_callback& abort );

    /// @throw smp::SmpException
    static PanelProperties LoadLegacy( stream_reader& reader, abort_callback& abort );

    /// @throw smp::SmpException
    void SaveJson( stream_writer& writer, abort_callback& abort, bool saveAsRawString = false ) const;

    /// @throw smp::SmpException
    static PanelProperties FromJson( const std::u8string& jsonString );

    /// @throw smp::SmpException
    std::u8string ToJson() const;
};

struct PanelSettings_Simple
{
    SimpleScriptSource scriptSource;
    std::u8string script;
    bool shouldGrabFocus;

public:
    PanelSettings_Simple();
    void ResetToDefault();

    static std::u8string GetDefaultScript();
};

struct PanelSettings_Package
{
    std::u8string packageName;
    PackageScriptLocation location;

public:
    // TODO: implement
    PanelSettings_Package(){};
    void ResetToDefault(){};
};


struct PanelSettings
{
    GUID guid;
    WINDOWPLACEMENT windowPlacement;
    EdgeStyle edgeStyle;
    bool isPseudoTransparent;
    PanelProperties properties;

    std::variant<PanelSettings_Simple, PanelSettings_Package> payload;

public:
    PanelSettings();

    void ResetToDefault();

    /// @throw smp::SmpException
    static PanelSettings LoadBinary( stream_reader& reader, t_size size, abort_callback& abort );

    /// @throw smp::SmpException
    static PanelSettings LoadJson( stream_reader& reader, t_size size, abort_callback& abort );

    /// @throw smp::SmpException
    void SaveJson( stream_writer& writer, abort_callback& abort ) const;

    /// @throw smp::SmpException
    static void SaveDefault( stream_writer& writer, abort_callback& abort );
};

} // namespace smp::config
