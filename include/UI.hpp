#pragma once

#include <string>

namespace pd {

class UI {
public:
    bool initialize(int width, int height, const std::string& title);
    void render();
    void shutdown();
};

} // namespace pd
