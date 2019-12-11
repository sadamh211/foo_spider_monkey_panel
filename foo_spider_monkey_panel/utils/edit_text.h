#pragma once

#include <filesystem>

namespace smp
{

/// @throw smp::SmpException
void EditTextFileInternal( HWND hParent, const std::filesystem::path& file );

/// @throw smp::SmpException
void EditTextFileExternal( const std::filesystem::path& pathToEditor, const std::filesystem::path& file );

/// @throw smp::SmpException
void EditTextInternal( HWND hParent, std::u8string& text );

/// @throw smp::SmpException
void EditTextExternal( HWND hParent, const std::filesystem::path& pathToEditor, std::u8string& text );

} // namespace smp
