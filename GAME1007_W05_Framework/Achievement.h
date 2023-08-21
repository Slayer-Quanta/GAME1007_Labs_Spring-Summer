#pragma once

#include <array>

enum Achievement : size_t
{
    TRIGGER_HAPPY,
    EXCAVATOR,
    OVERTIME,
    WARP_DRIVE,
    SPACE_ADMIRAL
};

namespace Achievements
{
    void Initialize(); // Initialize the achievements system
    bool Get(Achievement achievement);
    void Set(Achievement achievement, bool unlocked);
    void Save();
    void Load();
} // namespace Achievements
