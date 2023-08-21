#pragma once
#include "Core.h"
#include "Achievement.h" 
#include "Scene.h"
#include <vector>
enum Event
{
    SHIP_MOVE,
    SHIP_SHOOT,
    SHIP_TICK,
    ASTEROID_DESTROYED
};
class Entity;
// Forward declaration of Entity class
class Observer
{
public:
    virtual void OnNotify(const Entity& entity, Event event, float shipSpeed) = 0;

    int mShootCount = 0;
    int mDestroyCount = 0;
    float mDistanceTravelled = 0.0f;
    float mTickCount = 0.0f;

    std::array<bool, 5> mUnlocked; // Use std::array instead of array

    Observer(); // Declare the constructor

    void UnlockAchievement(Achievement achievement);

private:
    size_t mId = 0;
    friend class Subject;
};

class Subject
{
public:
    void AddObserver(Observer* observer);
    void RemoveObserver(Observer* observer);
    void Notify(const Entity& entity, Event event, float shipSpeed); // Pass ship speed

private:
    std::vector<Observer*> mObservers;
};