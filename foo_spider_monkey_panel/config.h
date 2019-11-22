#pragma once

#include <js_utils/serialized_value.h>

#include <optional>

// TODO: rename to panel_config

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

struct PackageData      //< TODO: move to a separate file
{                       // package.json (main script will be in (scripts/ ?)main.js)
    std::u8string name; //< when imported will be used for folder name
    std::u8string version;
    std::u8string author;
    std::u8string description;
    bool shouldGrabFocus;
    bool enableDragDrop;
    std::vector<std::pair<std::string, std::string>> dynamicActions;
};

struct PanelProperties
{
    using PropertyMap = std::unordered_map<std::wstring, std::shared_ptr<mozjs::SerializedJsValue>>;
    PropertyMap values;

public:
    /// @throw smp::SmpException
    [[nodiscard]] static PanelProperties FromJson( const std::u8string& jsonString );

    /// @throw smp::SmpException
    [[nodiscard]] std::u8string ToJson() const;

    /// @throw smp::SmpException
    [[nodiscard]] static PanelProperties Load( stream_reader& reader, abort_callback& abort, SerializationFormat format = SerializationFormat::Json );

    /// @throw smp::SmpException
    void Save( stream_writer& writer, abort_callback& abort ) const;
};

struct PanelSettings_Simple
{
    bool shouldGrabFocus; ///< TODO: remove and place in DefinePanel

    struct InMemoryData
    {
        [[nodiscard]] static std::u8string GetDefaultScript();
        std::u8string script;
    };
    struct FileData
    {
        std::u8string path;
    };
    struct SampleData
    {
        std::u8string sampleName;
    };
    std::variant<InMemoryData, FileData, SampleData> data;

public:
    PanelSettings_Simple();
    void ResetToDefault();
};

struct PanelSettings_Package
{
    std::u8string folderName; // converted from packageName when imported (i.e. remove `'":/\ and etc)
    PackageLocation location;
    PackageData data;

public:
    // TODO: implement
    PanelSettings_Package(){};
    void ResetToDefault(){};
};

struct PanelSettings
{
    GUID guid; ///< TODO: save guid to config
    EdgeStyle edgeStyle;
    bool isPseudoTransparent;
    PanelProperties properties;

    std::variant<PanelSettings_Simple, PanelSettings_Package> payload;

public:
    PanelSettings();

    void ResetToDefault();

    /// @throw smp::SmpException
    [[nodiscard]] static PanelSettings Load( stream_reader& reader, size_t size, abort_callback& abort );

    /// @throw smp::SmpException
    void Save( stream_writer& writer, abort_callback& abort ) const;

    /// @throw smp::SmpException
    static void SaveDefault( stream_writer& writer, abort_callback& abort );
};

} // namespace smp::config
