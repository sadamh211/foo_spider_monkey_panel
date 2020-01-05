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

struct PanelSettings_InMemory
{
    std::u8string script = GetDefaultScript();

    [[nodiscard]] static std::u8string GetDefaultScript();
};

struct PanelSettings_File
{
    std::u8string path;
};

struct PanelSettings_Sample
{
    std::u8string sampleName;
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

    using ScriptVariant = std::variant<PanelSettings_InMemory, PanelSettings_File, PanelSettings_Sample, PanelSettings_Package>;
    ScriptVariant payload;

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
