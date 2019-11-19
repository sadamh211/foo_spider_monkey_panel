#pragma once

#include <config.h>

namespace smp::config::binary
{

/// @throw smp::SmpException
PanelSettings LoadSettings( stream_reader& reader, abort_callback& abort );

/// @throw smp::SmpException
PanelProperties LoadProperties( stream_reader& reader, abort_callback& abort );

} // namespace smp::config::binary
