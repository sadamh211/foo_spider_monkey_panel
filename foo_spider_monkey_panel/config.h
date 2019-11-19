#pragma once

#include <js_utils/serialized_value.h>

#include <optional>

namespace smp::config
{

enum class SerializationFormat : uint8_t
{
    Com,
    Binary,
    Json
};

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

enum class PackageLocation : uint8_t
{
    Sample = 0,
    LocalAppData = 1,
    Fb2k = 2,
};

enum class EdgeStyle : uint8_t
{
    NoEdge = 0,
    SunkenEdge,
    GreyEdge,
    Default = NoEdge,
};

struct PanelProperties
{
    using PropertyMap = std::unordered_map<std::wstring, std::shared_ptr<mozjs::SerializedJsValue>>;
    PropertyMap values;

public:
    /// @throw smp::SmpException
    static PanelProperties FromJson( const std::u8string& jsonString );

    /// @throw smp::SmpException
    std::u8string ToJson() const;

    /// @throw smp::SmpException
    static PanelProperties Load( stream_reader& reader, abort_callback& abort, SerializationFormat format = SerializationFormat::Json );

    /// @throw smp::SmpException
    void Save( stream_writer& writer, abort_callback& abort ) const;
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
    PackageLocation location;

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
    static PanelSettings Load( stream_reader& reader, size_t size, abort_callback& abort );

    /// @throw smp::SmpException
    void Save( stream_writer& writer, abort_callback& abort ) const;

    /// @throw smp::SmpException
    static void SaveDefault( stream_writer& writer, abort_callback& abort );
};

} // namespace smp::config
