#pragma once

#include <config.h>

namespace smp::config::binary
{

/// @throw smp::SmpException
[[nodiscard]] PanelSettings LoadSettings( stream_reader& reader, abort_callback& abort );

/// @throw smp::SmpException
[[nodiscard]] PanelProperties LoadProperties( stream_reader& reader, abort_callback& abort );

} // namespace smp::config::binary
