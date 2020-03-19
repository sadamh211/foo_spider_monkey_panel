#include <stdafx.h>

#include "sci_prop_sets.h"

#include <utils/array_x.h>
#include <utils/file_helpers.h>
#include <utils/string_helpers.h>

using namespace smp::ui::sci;

namespace
{

constexpr auto DefaultProps = smp::to_array<ScintillaCfg::DefaultPropValue>(
    { { "style.default", "font:Courier New,size:10" },
      { "style.comment", "fore:#008000" },
      { "style.keyword", "bold,fore:#0000ff" },
      { "style.indentifier", "$(style.default)" },
      { "style.string", "fore:#ff0000" },
      { "style.number", "fore:#ff0000" },
      { "style.operator", "$(style.default)" },
      { "style.linenumber", "font:Courier New,size:8,fore:#2b91af" },
      { "style.bracelight", "bold,fore:#000000,back:#ffee62" },
      { "style.bracebad", "bold,fore:#ff0000" },
      { "style.selection.fore", "" },
      { "style.selection.back", "" },
      { "style.selection.alpha", "256" }, // 256 - SC_ALPHA_NOALPHA
      { "style.caret.fore", "" },
      { "style.caret.width", "1" },
      { "style.caret.line.back", "" },
      { "style.caret.line.back.alpha", "256" },
      { "style.wrap.mode", "0" },                 // SC_WRAP_NONE
      { "style.wrap.visualflags", "1" },          // SC_WRAPVISUALFLAG_END
      { "style.wrap.visualflags.location", "0" }, // SC_WRAPVISUALFLAGLOC_DEFAULT
      { "style.wrap.indentmode", "0" },           // SC_WRAPINDENT_FIXED
      { "api.jscript", "$(dir.component)jscript.api;$(dir.component)interface.api" } } );

} // namespace

namespace smp::ui::sci
{

ScintillaCfg::ScintillaCfg( const GUID& p_guid, nonstd::span<const DefaultPropValue> p_default )
    : cfg_var( p_guid )
{
    init_data( p_default );
}

ScintillaPropList& ScintillaCfg::val()
{
    return m_data;
}

const ScintillaPropList& ScintillaCfg::val() const
{
    return m_data;
}

void ScintillaCfg::get_data_raw( stream_writer* p_stream, abort_callback& p_abort )
{
    try
    {
        p_stream->write_lendian_t( m_data.size(), p_abort );
        for ( const auto& prop: m_data )
        {
            smp::pfc_x::WriteString( *p_stream, prop.key, p_abort );
            smp::pfc_x::WriteString( *p_stream, prop.val, p_abort );
        }
    }
    catch ( ... )
    {
    }
}

void ScintillaCfg::set_data_raw( stream_reader* p_stream, t_size, abort_callback& p_abort )
{
    ScintillaPropValues data_map;

    try
    {
        std::u8string key;
        std::u8string val;

        t_size count;
        p_stream->read_lendian_t( count, p_abort );

        for ( t_size i = 0; i < count; ++i )
        {
            key = smp::pfc_x::ReadString( *p_stream, p_abort );
            val = smp::pfc_x::ReadString( *p_stream, p_abort );
            data_map[key] = val;
        }
    }
    catch ( ... )
    {
        // Load default
        init_data( DefaultProps );
        return;
    }
    merge_data( data_map );
}

void ScintillaCfg::reset()
{
    for ( auto& prop: m_data )
    {
        prop.val = prop.defaultval;
    }
}

void ScintillaCfg::export_to_file( const wchar_t* filename )
{
    std::u8string content;

    content = "# Generated by " SMP_NAME "\r\n";
    for ( const auto& prop: m_data )
    {
        content += fmt::format( "{}={}", prop.key, prop.val );
        content += "\r\n";
    }

    smp::file::WriteFile( filename, content );
}

void ScintillaCfg::import_from_file( const char* filename )
{
    const std::u8string text = [&filename] {
        try
        {
            return smp::file::ReadFile( filename, CP_UTF8 );
        }
        catch ( const SmpException& )
        {
            return std::u8string{};
        }
    }();
    if ( text.empty() )
    {
        return;
    }

    ScintillaPropValues data_map;
    for ( const auto& line: smp::string::SplitByLines( text ) )
    {
        if ( line.length() < 3 || line[0] == '#' )
        { // skip comments and lines that are too short
            continue;
        }

        const auto parts = smp::string::Split( line, '=' );
        if ( parts.size() != 2 || parts[0].empty() )
        {
            continue;
        }

        data_map.emplace( std::u8string{ parts[0].data(), parts[0].size() },
                          std::u8string{ parts[1].data(), parts[1].size() } );
    }

    // Merge
    merge_data( data_map );
}

void ScintillaCfg::init_data( nonstd::span<const DefaultPropValue> p_default )
{
    m_data.clear();

    for ( const auto [key, defaultval]: p_default )
    {
        ScintillaProp temp;
        temp.key = key;
        temp.defaultval = defaultval;
        temp.val = temp.defaultval;
        m_data.push_back( temp );
    }
}

void ScintillaCfg::merge_data( const ScintillaPropValues& data_map )
{
    for ( auto& prop: m_data )
    {
        const auto it = data_map.find( prop.key );
        if ( it != data_map.cend() )
        {
            prop.val = it->second;
        }
    }
}

ScintillaCfg g_scintillaCfg( smp::guid::scintilla_props, DefaultProps );

bool ScintillaCfg::StriCmpAscii::operator()( const std::u8string& a, const std::u8string& b ) const
{
    return ( pfc::comparator_stricmp_ascii::compare( a.c_str(), b.c_str() ) < 0 );
}

} // namespace smp::ui::sci
