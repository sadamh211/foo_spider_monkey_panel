#pragma once

#include <nonstd/span.hpp>

#include <optional>
#include <string>
#include <vector>

namespace smp::file
{

/// @throw smp::SmpException
std::u8string ReadFile( const std::u8string& path, UINT codepage, bool checkFileExistense = true );

/// @throw smp::SmpException
std::wstring ReadFileW( const std::u8string& path, UINT codepage, bool checkFileExistense = true );

bool WriteFile( const wchar_t* path, const std::u8string& content, bool write_bom = true );

UINT DetectFileCharset( const std::u8string& path );

struct FileDialogOptions
{
    std::vector<COMDLG_FILTERSPEC> filterSpec{ { L"All files", L"*.*" } };
    std::wstring defaultExtension = L"";
    std::wstring defaultFilename = L"";
    bool rememberLocation = true;
};

std::wstring FileDialog( const std::wstring& title,
                         bool saveFile,
                         const FileDialogOptions& options = {} );

} // namespace smp::file
