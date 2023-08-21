#include "Observer.h"
#include <iostream>

Observer::Observer()
{
    // Initialize mUnlocked array with false values
    mUnlocked.fill(false);

}

void Observer::UnlockAchievement(Achievement achievement)
{
    // Unlock the achievement and set the corresponding entry in mUnlocked to true
    Achievements::Set(achievement, true);
    mUnlocked[static_cast<size_t>(achievement)] = true;

    // Output a message indicating that the achievement was unlocked
    std::cout << "Achievement unlocked: " << achievement << std::endl;
}
void Observer::OnNotify(const Entity& entity, Event event, float shipSpeed)
{
    switch (event)
    {
    case SHIP_SHOOT:
        mShootCount++;
        if (mShootCount >= 10 && !mUnlocked[TRIGGER_HAPPY])
        {
            UnlockAchievement(TRIGGER_HAPPY);
        }
        break;

    case SHIP_MOVE:
        mDistanceTravelled += shipSpeed;
        if (mDistanceTravelled >= 1000.0f && !mUnlocked[WARP_DRIVE])
        {
            UnlockAchievement(WARP_DRIVE);
        }
        break;

        // Add conditions and logic for other events/achievements here
    case SHIP_TICK:
        mTickCount++;
        if (mTickCount >= 10 && !mUnlocked[OVERTIME])
        {
            UnlockAchievement(OVERTIME);
        }
        break;

    case ASTEROID_DESTROYED:
        mDestroyCount++;
        if (mDestroyCount >= 10 && !mUnlocked[EXCAVATOR])
        {
            UnlockAchievement(EXCAVATOR);
        }
        break;

    default:
        break;
        bool allPreviousAchievementsUnlocked = true;
        for (size_t i = 0; i < mUnlocked.size() - 1; ++i) // Exclude the last achievement
        {
            if (!mUnlocked[i])
            {
                allPreviousAchievementsUnlocked = false;
                break;
            }
        }

        // Unlock the "Space Admiral" achievement if all previous achievements are unlocked
        if (allPreviousAchievementsUnlocked && !mUnlocked[SPACE_ADMIRAL])
        {
            UnlockAchievement(SPACE_ADMIRAL);
        }
    }
}
void Subject::AddObserver(Observer* observer)
{
    assert(observer != nullptr);
    static size_t id = 0; // Initialize id only once
    observer->mId = ++id;
    mObservers.push_back(observer);
}

void Subject::RemoveObserver(Observer* observer)
{
    assert(observer != nullptr);
    for (size_t i = 0; i < mObservers.size(); i++)
    {
        if (mObservers[i]->mId == observer->mId)
        {
            mObservers.erase(mObservers.begin() + i);
            break;
        }
    }
}

void Subject::Notify(const Entity& entity, Event event, float shipSpeed)
{
    for (Observer* observer : mObservers)
    {
        observer->OnNotify(entity, event, shipSpeed);
    }
}
