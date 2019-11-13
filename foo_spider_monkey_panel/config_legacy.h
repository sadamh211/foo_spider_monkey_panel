#pragma once

#include <config.h>

namespace smp::config
{

namespace binary
{

/// @throw smp::SmpException
PanelProperties LoadProperties( stream_reader& reader, abort_callback& abort );

} // namespace binary

namespace com
{

/// @throw smp::SmpException
PanelProperties LoadProperties( stream_reader& reader, abort_callback& abort );

} // namespace com

} // namespace smp::config
