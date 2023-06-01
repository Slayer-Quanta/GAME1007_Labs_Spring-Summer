#include <SDL_main.h>
#include "Core.h"
#include <array>
#include <iostream>
#include "tinyxml2.h"
using namespace tinyxml2;
using namespace std;

struct Game
{
    Texture* shipTex = nullptr;
    Rect shipRec{ 0.0f, 0.0f, 60.0f, 40.0f };
    float shipSpeed = 100.0f;

    Sound* sound1;

    // New members for audio scene
    Sound* sound2;
    Sound* sound3;
    Sound* sound4;
    Sound* sound5;
    Music* music;
    bool musicPaused = false;
};

void Update(Game& game, float dt)
{
    game.shipRec.x += game.shipSpeed * dt;
}

void Render(const Game& game)
{
    DrawTexture(game.shipTex, game.shipRec);
}

void SaveCar()
{
    XMLDocument doc;    
    XMLNode* root = doc.NewElement("CarDefinition");
    doc.InsertEndChild(root);

    XMLElement* acceleration = doc.NewElement("Acceleration");
    acceleration->SetAttribute("a", 20.0f);
    root->InsertEndChild(acceleration);

    XMLElement* wheel = doc.NewElement("Wheel");
    XMLElement* radius = doc.NewElement("Radius");
    radius->SetAttribute("r", 10.0f);
    wheel->InsertEndChild(radius);
    root->InsertEndChild(wheel);

    XMLElement* mass = doc.NewElement("Mass");
    mass->SetAttribute("m", 30.0f);
    root->InsertEndChild(mass);

    doc.SaveFile("Car.xml");
}

void SaveGame(const Game& game)
{
    XMLDocument doc;
    XMLNode* root = doc.NewElement("Game");
    doc.InsertEndChild(root);

    XMLElement* ship = doc.NewElement("Ship");
    ship->SetAttribute("x", 0.0f);
    ship->SetAttribute("y", 0.0f);
    ship->SetAttribute("w", 60.0f);
    ship->SetAttribute("h", 40.0f);
    ship->SetAttribute("speed", 100.0f);
    root->InsertEndChild(ship);

    doc.SaveFile("Game.xml");
}

Uint32 Pause(Uint32 interval, void* param)
{
    cout << (const char*)param << endl;
    PauseMusic();
    return 0;
}

Uint32 Resume(Uint32 interval, void* param)
{
    cout << (const char*)param << endl;
    ResumeMusic();
    return 0;
}

struct Custom
{
    float f = 1.1f;
    int i = 5;
    const char* msg = "hello";
};

Uint32 CustomTimer(Uint32 interval, void* param)
{
    Custom* custom = (Custom*)param;
    cout << custom->f << endl;
    cout << custom->i << endl;
    cout << custom->msg << endl;
    return 0;
}

void OnGui(void* data)
{
    Game& game = *(Game*)data;

    if (ImGui::Button("Fire!"))
    {
        PlaySound(game.sound1);
    }

    // Pause/Resume music button
    if (ImGui::Button(game.musicPaused ? "Resume Music" : "Pause Music"))
    {
        game.musicPaused = !game.musicPaused;
        if (game.musicPaused)
        {
            PauseMusic();
        }
        else
        {
            ResumeMusic();
        }
    }
    if (ImGui::Button("Explosion(warning all sounds are loud as heck)"))
    {
        PlaySound(game.sound2);
    }
    if (ImGui::Button("Aughhhhhhhhhhhhhhhh"))
    {
        PlaySound(game.sound3);
    }
    if (ImGui::Button("Didn't I do it for you"))
    {
        PlaySound(game.sound4);
    }
    if (ImGui::Button("Moist Critical woooo"))
    {
        PlaySound(game.sound5);
    }
}

int main(int argc, char* argv[])
{
    AppInit(1024, 768);
    Music* music = LoadMusic("../Assets/aud/Wings.mp3");
    Sound* sound1 = LoadSound("../Assets/aud/Fire.wav");
    Sound* sound2 = LoadSound("../Assets/aud/explosion.wav");
    Sound* sound3 = LoadSound("../Assets/aud/Aughh.mp3");
    Sound* sound4 = LoadSound("../Assets/aud/do it for you.mp3");
    Sound* sound5 = LoadSound("../Assets/aud/wooo.mp3");

    Game game;
    game.shipTex = LoadTexture("../Assets/img/enterprise.png");
    game.sound1 = sound1;
    game.sound2 = sound2;
    game.sound3 = sound3;
    game.sound4 = sound4;
    game.sound5 = sound5;
    game.music = music;
    SaveGame(game);
    SetGuiCallback(OnGui, &game);
    PlayMusic(music);

    while (IsRunning())
    {
        Update(game, FrameTime());
        RenderBegin();
        Render(game);
        RenderEnd();
    }
    UnloadTexture(game.shipTex);
    UnloadMusic(music);
    UnloadSound(sound1);
    UnloadSound(sound2);
    UnloadSound(sound3);
    UnloadSound(sound4);
    UnloadSound(sound5);

    AppExit();
    return 0;
}
