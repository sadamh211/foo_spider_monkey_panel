#include <stdafx.h>
#include "plugin.h"
#include "plugin_interface.h"

#include <optional>

namespace example_plugin
{

using namespace smp_plugin;

bool var_bool = false;
uint8_t var_u8 = 0;
uint32_t var_u32 = 0;
uint64_t var_u64 = 0;
std::wstring var_str;
std::wstring var_json;

template <typename T1, typename T2>
T2 AnyCastIfValid( const std::any& var )
{
    // TODO: add conversion to string and from string
    if constexpr ( std::is_convertible_v<T1, T2> )
    {
        return static_cast<T2>( std::any_cast<T1>( var ) );
    }
    else
    {
        throw SmpPluginException( "Can't convert variable to the required type" );
    }
}

template <typename T>
std::optional<T> GetValueFromAny( const std::wstring type, const std::any& var )
{
    try
    {
        if ( type == L"undefined" )
        {
            return std::nullopt;
        }
        else if ( type == L"bool" )
        {
            return AnyCastIfValid<bool, T>( var );
        }
        else if ( type == L"uint8_t" )
        {
            return AnyCastIfValid<uint8_t, T>( var );
        }
        else if ( type == L"uint32_t" )
        {
            return AnyCastIfValid<uint32_t, T>( var );
        }
        else if ( type == L"uint64_t" )
        {
            return AnyCastIfValid<uint64_t, T>( var );
        }
        else if ( type == L"std::wstring" || type == L"json" )
        {
            return AnyCastIfValid<std::wstring, T>( var );
        }
        else
        {
            throw SmpPluginException( "Unknown variable type" );
        }
    }
    catch ( const std::bad_any_cast& )
    {
        throw SmpPluginException( "Mismatched variable type identifier" );
    }
}

template <typename T>
std::optional<T> GetValueFromAny( const IPlugin::PluginValue& value )
{
    return GetValueFromAny<T>( value.type, value.value );
}

class ExamplePlugin
    : public smp_plugin::IPlugin
{
public:
    std::wstring GetName() override;
    std::wstring GetDescription() override;
    std::wstring GetVersion() override;

    /// @throws SmpPluginException
    PluginValue GetValue( const std::wstring& name ) override;
    /// @throws SmpPluginException
    void SetValue( const std::wstring& name, const PluginValue& value ) override;
    /// @throws SmpPluginException
    PluginValue InvokeMethod( const std::wstring& name, const PluginValue* params, size_t paramsCount ) override;

    /// @throws SmpPluginException
    void RegisterCallback( const std::wstring& name, SmpCallbackFn callbackFn ) override;
    /// @throws SmpPluginException
    void UnregisterCallback( const std::wstring& name, SmpCallbackFn callbackFn ) override;

    void OnSmpInitialize() override;
    void OnSmpFinalize() override;
    void RegisterExitCallback( const std::wstring& name, SmpExitCallbackFn callbackFn ) override;
};

std::wstring ExamplePlugin::GetName()
{
    return L"ExamplePlugin";
}

std::wstring ExamplePlugin::GetDescription()
{
    return L"This is a simple library that demonstrates the usage of SMP plugin API";
}

std::wstring ExamplePlugin::GetVersion()
{
    return L"1.0.0";
}

IPlugin::PluginValue ExamplePlugin::GetValue( const std::wstring& name )
{
    if ( name == L"var_bool" )
    {
        return PluginValue{ L"bool", var_bool };
    }
    else if ( name == L"var_u8" )
    {
        return PluginValue{ L"uint8_t", var_u8 };
    }
    else if ( name == L"var_u32" )
    {
        return PluginValue{ L"uint32_t", var_u32 };
    }
    else if ( name == L"var_u64" )
    {
        return PluginValue{ L"uint64_t", var_u64 };
    }
    else if ( name == L"var_str" )
    {
        return PluginValue{ L"std::wstring", var_str };
    }
    else if ( name == L"var_json" )
    {
        return PluginValue{ L"json", var_json };
    }
    else
    {
        throw SmpPluginException( "Unknown variable name" );
    }
}

void ExamplePlugin::SetValue( const std::wstring& name, const PluginValue& value )
{
    if ( name == L"var_bool" )
    {
        var_bool = GetValueFromAny<bool>( value ).value_or( bool{} );
    }
    else if ( name == L"var_u8" )
    {
        var_u8 = GetValueFromAny<uint8_t>( value ).value_or( uint8_t{} );
    }
    else if ( name == L"var_u32" )
    {
        var_u32 = GetValueFromAny<uint32_t>( value ).value_or( uint32_t{} );
    }
    else if ( name == L"var_u64" )
    {
        var_u64 = GetValueFromAny<uint64_t>( value ).value_or( uint64_t{} );
    }
    else if ( name == L"var_str" )
    {
        var_str = GetValueFromAny<std::wstring>( value ).value_or( std::wstring{} );
    }
    else if ( name == L"var_json" )
    {
        var_json = GetValueFromAny<std::wstring>( value ).value_or( std::wstring{} );
    }
    else
    {
        throw SmpPluginException( "Unknown variable name" );
    }
}

IPlugin::PluginValue ExamplePlugin::InvokeMethod( const std::wstring& name, const PluginValue* params, size_t paramsCount )
{
    if ( name == L"example_method" )
    {
        std::wstring retString;
        if ( paramsCount )
        {
            for ( size_t i = 0; i < paramsCount; ++i )
            {
                retString += GetValueFromAny<std::wstring>( params[i] ).value_or( std::wstring{} );
                retString += L" ";
            }

            retString.resize( retString.size() - 1 );
        }

        return PluginValue{ L"std::wstring", retString };
    }
    else
    {
        throw SmpPluginException( "Unknown method name" );
    }
}

void ExamplePlugin::RegisterCallback( const std::wstring& name, SmpCallbackFn callbackFn )
{
}

void ExamplePlugin::UnregisterCallback( const std::wstring& name, SmpCallbackFn callbackFn )
{
}

void ExamplePlugin::OnSmpInitialize()
{
}

void ExamplePlugin::OnSmpFinalize()
{
}

void ExamplePlugin::RegisterExitCallback( const std::wstring& name, SmpExitCallbackFn callbackFn )
{
}

ExamplePlugin g_plugin;

} // namespace example_plugin

extern "C" SmpPluginApiVersion __cdecl Smp_GetApiVersion()
{
    return SmpPluginApiVersion{
        SMP_PLUGIN_API_VERSION_MAJOR, SMP_PLUGIN_API_VERSION_MINOR, SMP_PLUGIN_API_VERSION_PATCH
    };
}

extern "C" void* __cdecl Smp_GetInterface( const SmpPluginApiVersion* pVersion )
{
    if ( !pVersion
         || pVersion->major != SMP_PLUGIN_API_VERSION_MAJOR
         || pVersion->minor != SMP_PLUGIN_API_VERSION_MINOR )
    {
        return nullptr;
    }

    return &example_plugin::g_plugin;
}