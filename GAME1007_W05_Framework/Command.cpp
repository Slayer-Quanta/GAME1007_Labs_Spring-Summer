#include "Command.h" 

namespace Bindings
{
    static SDL_Scancode sBindings[Action::COUNT] = { SDL_SCANCODE_W ,SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_T, SDL_SCANCODE_SPACE };

    SDL_Scancode Get(Action action)
    {
        return sBindings[action];
    }

    void Set(Action action, SDL_Scancode binding)
    {
        sBindings[action] = binding;
    }

    void Save()
    {
        std::ofstream outFile("Commands.dat", std::ios::binary);
        outFile.write(reinterpret_cast<char*>(sBindings), sizeof(sBindings));
        outFile.close();
    }

    void Load()
    {
        std::ifstream inFile("Commands.dat", std::ios::binary);
        if (inFile.is_open())
        {
            inFile.read(reinterpret_cast<char*>(sBindings), sizeof(sBindings));
            inFile.close();
        }
    }
}
