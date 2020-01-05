#include <stdafx.h>

#include "edit_text.h"

#include <ui/ui_edit_in_progress.h>
#include <ui/ui_editor.h>
#include <utils/file_helpers.h>
#include <utils/scope_helpers.h>
#include <utils/winapi_error_helpers.h>

namespace
{

/// @remark For cases when modal might be called from another modal
class ConditionalModalScope
{
public:
    ConditionalModalScope( HWND hParent )
        : needsModalScope_( modal_dialog_scope::can_create() )
    {
        if ( needsModalScope_ )
        {
            scope_.initialize( hParent );
        }
    }

    ~ConditionalModalScope()
    {
        scope_.deinitialize();
    }

private:
    modal_dialog_scope scope_;
    bool needsModalScope_;
};

} // namespace

namespace smp
{

void EditTextFileInternal( HWND hParent, const std::filesystem::path& file )
{
    // TODO: handle BOM
    auto text = smp::file::ReadFile( file.u8string(), CP_ACP, true );
    {
        ConditionalModalScope scope( hParent );
        smp::ui::CEditor dlg( text, [&file, &text] { smp::file::WriteFile( file.wstring().c_str(), text ); } );
        dlg.DoModal( hParent );
    }    
}

void EditTextFileExternal( const std::filesystem::path& pathToEditor, const std::filesystem::path& file )
{
    const std::wstring qPath = L"\"" + file.wstring() + L"\"";
    const auto hInstance = ShellExecute( nullptr,
                                         L"open",
                                         pathToEditor.c_str(),
                                         qPath.c_str(),
                                         nullptr,
                                         SW_SHOW );
    smp::error::CheckWinApi( hInstance, "ShellExecute" );
}

void EditTextInternal( HWND hParent, std::u8string& text )
{
    ConditionalModalScope scope( hParent );
    smp::ui::CEditor dlg( text );
    dlg.DoModal( hParent );
}

void EditTextExternal( HWND hParent, const std::filesystem::path& pathToEditor, std::u8string& text )
{
    namespace fs = std::filesystem;

    std::wstring tmpFilePath;
    tmpFilePath.resize( MAX_PATH + 1 );

    DWORD dwRet = GetTempPath( tmpFilePath.size() - 1, tmpFilePath.data() );
    smp::error::CheckWinApi( dwRet && dwRet <= MAX_PATH, "GetTempPath" );

    std::wstring filename;
    filename.resize( MAX_PATH + 1 );
    UINT uRet = GetTempFileName( tmpFilePath.c_str(),
                                 L"smp",
                                 0,
                                 filename.data() ); // buffer for name
    smp::error::CheckWinApi( uRet, "GetTempFileName" );

    fs::path fsTmpFilePath( tmpFilePath );
    fsTmpFilePath /= filename;

    if ( !smp::file::WriteFile( fsTmpFilePath.c_str(), text ) )
    {
        throw SmpException( fmt::format( "Failed to create temporary file: {}", fsTmpFilePath.u8string() ) );
    }
    const smp::utils::final_action autoRemove( [&fsTmpFilePath] { fs::remove( fsTmpFilePath ); } );

    ConditionalModalScope scope( hParent );
    ui::CEditInProgress dlg{ pathToEditor, fsTmpFilePath };
    if ( dlg.DoModal( hParent ) == IDOK )
    {
        text = smp::file::ReadFile( fsTmpFilePath.u8string(), CP_UTF8 );
    }
}

} // namespace smp