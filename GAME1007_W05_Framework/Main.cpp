#include <SDL_main.h>
#include "Core.h"
#include <vector>

struct Ship
{
	Texture* tex = nullptr;
	Rect rec{ 0.0f, 0.0f, 60.0f, 40.0f };
	float speed = 100.0f;
};

struct Game
{
	std::vector<Ship> ships;
};

void Update(Game& game, float dt)
{
	for (auto& ship : game.ships)
	{
		ship.rec.x += ship.speed * dt;
	}
}

void Render(const Game& game)
{
	for (const auto& ship : game.ships)
	{
		DrawTexture(ship.tex, ship.rec);
	}
}

int main(int argc, char* argv[])
{
	AppInit(1024, 768);

	Game game;
	Texture* shipTexture1 = LoadTexture("../Assets/img/enterprise.png");
	Texture* shipTexture2 = LoadTexture("../Assets/img/Ship2.png");
	Texture* shipTexture3 = LoadTexture("../Assets/img/Ship3.png");

	Ship ship1;
	ship1.tex = shipTexture1;
	ship1.rec.x = 0.0f;
	game.ships.push_back(ship1);

	Ship ship2;
	ship2.tex = shipTexture2;
	ship2.rec.x = 100.0f;
	game.ships.push_back(ship2);

	Ship ship3;
	ship3.tex = shipTexture3;
	ship3.rec.x = 200.0f;
	game.ships.push_back(ship3);

	while (IsRunning())
	{
		Update(game, FrameTime());
		RenderBegin();
		Render(game);
		RenderEnd();
	}

	UnloadTexture(shipTexture1);
	UnloadTexture(shipTexture2);
	UnloadTexture(shipTexture3);
	AppExit();
	return 0;
}
