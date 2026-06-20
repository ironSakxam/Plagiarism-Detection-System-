#include "FileDialog.hpp"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>

namespace pd {

std::string openFileDialog() {
    char szFile[260] = { 0 };
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "C++ & Text Files (*.cpp;*.hpp;*.h;*.txt)\0*.cpp;*.hpp;*.h;*.txt\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    
    // OFN_NOCHANGEDIR is critical to prevent changing the current working directory,
    // which would break relative paths to "data/".
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn) == TRUE) {
        return std::string(ofn.lpstrFile);
    }
    return "";
}

} // namespace pd
