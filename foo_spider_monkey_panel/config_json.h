#pragma once

#include <config.h>

namespace smp::config::json
{

/// @throw smp::SmpException
PanelSettings LoadSettings( stream_reader& reader, abort_callback& abort );

/// @throw smp::SmpException
void SaveSettings( stream_writer& writer, abort_callback& abort, const PanelSettings& settings );

/// @throw smp::SmpException
PanelProperties LoadProperties( stream_reader& reader, abort_callback& abort );

/// @throw smp::SmpException
void SaveProperties( stream_writer& writer, abort_callback& abort, const PanelProperties& properties );

/// @throw smp::SmpException
PanelProperties DeserializeProperties( const std::u8string& str );

/// @throw smp::SmpException
std::u8string SerializeProperties( const PanelProperties& properties );


} // namespace smp::config::json
