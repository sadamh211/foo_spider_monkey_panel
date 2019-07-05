#pragma once

#define SMP_PLUGIN_API_VERSION_MAJOR 0
#define SMP_PLUGIN_API_VERSION_MINOR 1
#define SMP_PLUGIN_API_VERSION_PATCH 0

#include <any>
#include <array>
#include <string>

namespace smp_plugin
{

class SmpPluginException
    : public std::runtime_error
{
public:
    explicit SmpPluginException( const std::string& errorText )
        : std::runtime_error( errorText )
    {
    }

    virtual ~SmpPluginException() = default;
};

class IPlugin
{
public:
    struct PluginValue
    {
        std::wstring type;
        std::any value;
    };

    static constexpr std::array<const wchar_t*, 7> supportedValueTypes = {
        L"bool",
        L"uint8_t",
        L"uint32_t",
        L"uint64_t",
        L"std::wstring",
        L"undefined",
        L"json"
    };

    /// @throws SmpPluginException
    using SmpCallbackFn = PluginValue( __stdcall* )( const std::wstring&, const PluginValue*, size_t );
    /// @details Does NOT throw
    using SmpExitCallbackFn = SmpCallbackFn;

public:
    virtual ~IPlugin() = default;

    virtual std::wstring GetName() = 0;
    virtual std::wstring GetDescription() = 0;
    virtual std::wstring GetVersion() = 0;

    /// @throws SmpPluginException
    virtual PluginValue GetValue( const std::wstring& name ) = 0;
    /// @throws SmpPluginException
    virtual void SetValue( const std::wstring& name, const PluginValue& value ) = 0;
    /// @throws SmpPluginException
    virtual PluginValue InvokeMethod( const std::wstring& name, const PluginValue* params, size_t paramsCount ) = 0;

    /// @throws SmpPluginException
    virtual void RegisterCallback( const std::wstring& name, SmpCallbackFn callbackFn ) = 0;
    /// @throws SmpPluginException
    virtual void UnregisterCallback( const std::wstring& name, SmpCallbackFn callbackFn ) = 0;

    virtual void OnSmpInitialize() = 0;
    virtual void OnSmpFinalize() = 0;
    virtual void RegisterExitCallback( const std::wstring& name, SmpExitCallbackFn callbackFn ) = 0;
};

} // namespace smp_plugin
