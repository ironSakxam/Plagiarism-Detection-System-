#pragma once

#include <string>

namespace pd {

// Opens a native Windows Open File dialog.
// Filters for C++ source files (*.cpp, *.hpp, *.h) and text files (*.txt).
// Returns the absolute path of the selected file, or an empty string if cancelled.
std::string openFileDialog();

} // namespace pd
