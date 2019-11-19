#pragma once

#include <config.h>

namespace smp::config::com
{

/// @throw smp::SmpException
PanelProperties LoadProperties( stream_reader& reader, abort_callback& abort );

} // namespace smp::config::com
