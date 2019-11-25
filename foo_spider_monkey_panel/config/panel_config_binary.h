#pragma once

#include <config/panel_config.h>

namespace smp::config::binary
{

/// @throw smp::SmpException
[[nodiscard]] PanelSettings LoadSettings( stream_reader& reader, abort_callback& abort );

/// @throw smp::SmpException
void SaveSettings( stream_writer& writer, abort_callback& abort, const PanelSettings& settings );

/// @throw smp::SmpException
[[nodiscard]] PanelProperties LoadProperties( stream_reader& reader, abort_callback& abort );

/// @throw smp::SmpException
void SaveProperties( stream_writer& writer, abort_callback& abort, const PanelProperties& properties );

} // namespace smp::config::binary
