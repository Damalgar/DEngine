#pragma once
#include "IO/Console.h"

class UIConsole {
    public:
    static void Draw();

    inline static bool showInfo = true;
    inline static bool showWarn = true;
    inline static bool showErr = true;
};