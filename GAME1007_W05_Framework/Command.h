#pragma once
#include "Core.h"
#include <fstream>
enum Action : size_t
{
    THRUST,
    LEFT,
    RIGHT,
    TELEPORT,
    SHOOT,
    COUNT
};

namespace Bindings
{
    SDL_Scancode Get(Action action);
    void Set(Action action, SDL_Scancode binding);
    void Save();
    void Load();
}
