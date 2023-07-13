#include "Scene.h"
#include "tinyxml2.h"
#include <iostream>
#include <cassert>
#include <algorithm>
#include <functional>
using namespace std;
using namespace tinyxml2;

Scene::Type Scene::sCurrent;
std::array<Scene*, Scene::COUNT> Scene::sScenes;

void Scene::Init()
{
	sScenes[TITLE] = new TitleScene;
	sScenes[GAME] = new GameScene;
	sScenes[PAUSE] = new PauseScene; 
	sScenes[EXIT] = new ExitScene;
	sScenes[sCurrent]->OnEnter();
}

void Scene::Exit()
{
	sScenes[sCurrent]->OnExit();
	for (size_t i = 0; i < sScenes.size(); i++)
		delete sScenes[i];
}

void Scene::Update(float dt)
{
	sScenes[sCurrent]->OnUpdate(dt);
}

void Scene::Render()
{
	sScenes[sCurrent]->OnRender();
}

void Scene::Change(Type type)
{
	assert(sCurrent != type);
	sScenes[sCurrent]->OnExit();
	sCurrent = type;
	sScenes[sCurrent]->OnEnter();
}
void TitleScene::OnEnter()
{
	mTitleBackgroundTex = LoadTexture("../Assets/img/Hanger.png");
	mBackgroundRec.x = 0.0f;
	mBackgroundRec.y = 0.0f;
	mBackgroundRec.w = static_cast<float>(SCREEN_WIDTH);
	mBackgroundRec.h = static_cast<float>(SCREEN_HEIGHT);

	mTitleTex = LoadTexture("../Assets/img/title.png");

	mTitleRec.x = static_cast<float>(SCREEN_WIDTH) * 0.5f - 200.0f;
	mTitleRec.y = static_cast<float>(SCREEN_HEIGHT) * 0.3f;
	mTitleRec.w = 400.0f;
	mTitleRec.h = 100.0f;

	TitleMusic = Mix_LoadMUS("../Assets/aud/Menu.mp3");
	if (TitleMusic)
	{
		Mix_PlayMusic(TitleMusic, -1);
	}
}

void TitleScene::OnExit()
{
	UnloadTexture(mTitleTex);
	UnloadTexture(mTitleBackgroundTex);
	Mix_HaltMusic();
	Mix_FreeMusic(TitleMusic);
}

void TitleScene::OnUpdate(float dt)
{
	if (IsKeyDown(SDL_SCANCODE_SPACE))
	{
		Scene::Change(Scene::GAME);
	}
}

void TitleScene::OnRender()
{
	SDL_Rect TitleRec = { static_cast<int>(mTitleRec.x), static_cast<int>(mTitleRec.y), static_cast<int>(mTitleRec.w), static_cast<int>(mTitleRec.h) };
	SDL_Rect BackgroundRect = { static_cast<int>(mBackgroundRec.x), static_cast<int>(mBackgroundRec.y), static_cast<int>(mBackgroundRec.w), static_cast<int>(mBackgroundRec.h) };
	DrawTexture(mTitleBackgroundTex, mBackgroundRec);
	DrawTexture(mTitleTex, mTitleRec);
}

GameScene::GameScene()
{
	mGameBackgroundTex = LoadTexture("../Assets/img/background.png");
	mShipTex = LoadTexture("../Assets/img/F-22 Raptor.png");
	mAsteroidTex = LoadTexture("../Assets/img/AsteroidL.png");
	mAsteroidClusterTex = LoadTexture("../Assets/img/AsteroidCluster.png");
	mBulletTex = LoadTexture("../Assets/img/Bullet.png");
}

GameScene::~GameScene()
{
	UnloadTexture(mGameBackgroundTex);
	UnloadTexture(mShipTex);
	UnloadTexture(mAsteroidTex);
	UnloadTexture(mAsteroidClusterTex);
	UnloadTexture(mBulletTex);
}

void GameScene::OnEnter()
{
	Mix_Music* GameMusic = Mix_LoadMUS("../Assets/aud/Wings.mp3");
	if (GameMusic)
	{
		Mix_PlayMusic(GameMusic, -1);
	}
	mBulletSound = Mix_LoadWAV("../Assets/aud/Ciws .mp3");
	XMLDocument doc;
	if (doc.LoadFile("Game.xml") == XML_SUCCESS)
	{
		XMLElement* gameData = doc.FirstChildElement();
		XMLElement* shipData = gameData->FirstChildElement();
		shipData->QueryAttribute("x", &mShipRec.x);
		shipData->QueryAttribute("y", &mShipRec.y);
		shipData->QueryAttribute("w", &mShipRec.w);
		shipData->QueryAttribute("h", &mShipRec.h);
		shipData->QueryAttribute("speed", &mShipSpeed);

		mShip.position = { SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
		mShip.width = mShip.height = 50.0f;
		mShip.height = mShipRec.h;
		mShip.speed = 200.0f;

		XMLElement* mediumAsteroidsData = gameData->FirstChildElement("MediumAsteroids");
		if (mediumAsteroidsData)
		{
			for (XMLElement* asteroidData = mediumAsteroidsData->FirstChildElement("Asteroid"); asteroidData; asteroidData = asteroidData->NextSiblingElement("Asteroid"))
			{
				Asteroid asteroid;
				asteroid.position.x = asteroidData->FloatAttribute("x");
				asteroid.position.y = asteroidData->FloatAttribute("y");
				mAsteroidsMedium.push_back(asteroid);
			}
		}

		XMLElement* smallAsteroidsData = gameData->FirstChildElement("SmallAsteroids");
		if (smallAsteroidsData)
		{
			for (XMLElement* asteroidData = smallAsteroidsData->FirstChildElement("Asteroid"); asteroidData; asteroidData = asteroidData->NextSiblingElement("Asteroid"))
			{
				Asteroid asteroid;
				asteroid.position.x = asteroidData->FloatAttribute("x");
				asteroid.position.y = asteroidData->FloatAttribute("y");
				mAsteroidsSmall.push_back(asteroid);
			}
		}
	}
	else
	{
		mShip.position = Point{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
		mShip.width = mShip.height = 50.0f;
		mShip.speed = 800.0f;
	}
}

void GameScene::OnExit()
{
	Mix_HaltMusic();
	Mix_FreeMusic(GameMusic);
	Mix_FreeChunk(mBulletSound);
	XMLDocument doc;
	XMLNode* root = doc.NewElement("Game");
	doc.InsertEndChild(root);

	XMLElement* ship = doc.NewElement("Ship");
	ship->SetAttribute("x", mShip.position.x);
	ship->SetAttribute("y", mShip.position.y);
	ship->SetAttribute("w", mShip.width);
	ship->SetAttribute("h", mShip.height);
	ship->SetAttribute("speed", mShip.speed);
	root->InsertEndChild(ship);

	XMLElement* mediumAsteroidsData = doc.NewElement("MediumAsteroids");
	root->InsertEndChild(mediumAsteroidsData);
	for (const Asteroid& asteroid : mAsteroidsMedium)
	{
		XMLElement* asteroidData = doc.NewElement("Asteroid");
		asteroidData->SetAttribute("x", asteroid.position.x);
		asteroidData->SetAttribute("y", asteroid.position.y);

		mediumAsteroidsData->InsertEndChild(asteroidData);
	}

	XMLElement* smallAsteroidsData = doc.NewElement("SmallAsteroids");
	root->InsertEndChild(smallAsteroidsData);
	for (const Asteroid& asteroid : mAsteroidsSmall)
	{
		XMLElement* asteroidData = doc.NewElement("Asteroid");
		asteroidData->SetAttribute("x", asteroid.position.x);
		asteroidData->SetAttribute("y", asteroid.position.y);

		smallAsteroidsData->InsertEndChild(asteroidData);
	}

	doc.SaveFile("Game.xml");
}

void GameScene::OnUpdate(float dt)
{
	mShip.velocity.x *= 0.99f;
	mShip.velocity.y *= 0.99f;
	mShip.acceleration = 1000.0f;
	mShip.sidewaysAcceleration = 800.0f;
	mShipRotation = 0.0f;  // Initialize ship rotation angle


	if (IsKeyDown(SDL_SCANCODE_A))
	{
		mShip.velocity = mShip.velocity - mShip.direction * mShip.acceleration * dt;
		float minSpeed = 0.0f;
		float currentSpeed = Length(mShip.velocity);
		if (currentSpeed < minSpeed)
		{
			mShip.velocity = Normalize(mShip.velocity) * minSpeed;
		}
		
	}

	if (IsKeyDown(SDL_SCANCODE_D))
	{
		mShip.velocity = mShip.velocity + mShip.direction * mShip.acceleration * dt;
		float maxSpeed = 800.0f;
		float currentSpeed = Length(mShip.velocity);
		if (currentSpeed > maxSpeed)
		{
			mShip.velocity = Normalize(mShip.velocity) * maxSpeed;
		}
		
	}

	if (IsKeyDown(SDL_SCANCODE_W))
	{
		mShip.direction = Rotate(mShip.direction, -mShip.angularSpeed * dt);
		Point perpendicular = Point{ mShip.direction.y, -mShip.direction.x };
		Point sidewaysThrust = perpendicular * mShip.sidewaysAcceleration * dt;
		mShip.velocity = mShip.velocity + sidewaysThrust;
	}

	if (IsKeyDown(SDL_SCANCODE_S))
	{
		mShip.direction = Rotate(mShip.direction, -mShip.angularSpeed * dt);
		Point perpendicular = Point{ -mShip.direction.y, mShip.direction.x };
		Point sidewaysThrust = perpendicular * mShip.sidewaysAcceleration * dt;
		mShip.velocity = mShip.velocity + sidewaysThrust;
	}

	mShip.position = mShip.position + mShip.velocity * dt;

	Wrap(mShip);

	if (IsKeyDown(SDL_SCANCODE_P))
	{
		Scene::Change(Scene::PAUSE);
	}

	bool isFiring = IsKeyDown(SDL_SCANCODE_SPACE);
	if (isFiring && !mIsFiring && mShip.bulletCooldown.Expired())
	{
		mShip.bulletCooldown.Reset();
		if (mBulletSound)
		{
			Mix_PlayChannel(-1, mBulletSound, 0);
		}
		Bullet bullet;
		bullet.width = bullet.height = 10.0f;
		bullet.position = mShip.position + mShip.direction * sqrtf(powf(mShip.width * 0.5f + bullet.width * 0.5f, 2.0f));
		bullet.velocity = mShip.direction * 100.0f;
		bullet.direction = mShip.direction;
		mBullets.push_back(bullet);
	}

	mIsFiring = isFiring;
	mShip.bulletCooldown.Tick(dt);
	if (IsKeyDown(SDL_SCANCODE_E))
	{
		// Rotate the ship clockwise
		mShipRotation += mShip.angularSpeed * dt;
	}

	if (IsKeyDown(SDL_SCANCODE_Q))
	{
		// Rotate the ship counterclockwise
		mShipRotation -= mShip.angularSpeed * dt;
	}


	for (const Asteroid& asteroid : mAsteroidsLarge)
	{
		Rect asteroidRect = asteroid.Collider();
		Rect shipRect = mShip.Collider();

		if (SDL_HasIntersectionF(&asteroidRect, &shipRect))
		{
			ResetGame();
			return;
		}
	}

	for (Bullet& bullet : mBullets)
	{
		bullet.position = bullet.position + bullet.velocity * dt;
		Rect bulletRect = bullet.Collider();

		for (Asteroid& asteroid : mAsteroidsLarge)
		{
			Rect asteroidRect = asteroid.Collider();
			if (SDL_HasIntersectionF(&bulletRect, &asteroidRect))
			{
				asteroid.health -= bullet.damage;

				Asteroid medasteroid1, medasteroid2;
				medasteroid1.position = medasteroid2.position = asteroid.position;
				medasteroid1.width = medasteroid2.width = mSizeMedium;
				medasteroid1.height = medasteroid2.height = mSizeMedium;

				float r = Random(30.0f, 45.0f) * DEG2RAD;
				float v = Random(20.0f, 200.0f);
				Point direction = Normalize(bullet.velocity);
				Point direction1 = Rotate(direction, r);
				Point direction2 = Rotate(direction, -r);
				medasteroid1.velocity = direction1 * v;
				medasteroid2.velocity = direction2 * v;

				medasteroid1.position = medasteroid1.position + direction1 * mSizeLarge;
				medasteroid2.position = medasteroid2.position + direction2 * mSizeLarge;

				mAsteroidsMedium.push_back(medasteroid1);
				mAsteroidsMedium.push_back(medasteroid2);
			}
		}

		for (Asteroid& asteroid : mAsteroidsMedium)
		{
			Rect asteroidRect = asteroid.Collider();
			if (SDL_HasIntersectionF(&bulletRect, &asteroidRect))
			{
				asteroid.health -= bullet.damage;

				Asteroid asteroid1, asteroid2;
				asteroid1.position = asteroid2.position = asteroid.position;
				asteroid1.width = asteroid2.width = mSizeSmall;
				asteroid1.height = asteroid2.height = mSizeSmall;

				float r = Random(30.0f, 45.0f) * DEG2RAD;
				float v = Random(20.0f, 200.0f);
				Point direction = Normalize(bullet.velocity);
				Point direction1 = Rotate(direction, r);
				Point direction2 = Rotate(direction, -r);
				asteroid1.velocity = direction1 * v;
				asteroid2.velocity = direction2 * v;

				asteroid1.position = asteroid1.position + direction1 * mSizeMedium;
				asteroid2.position = asteroid2.position + direction2 * mSizeMedium;

				mAsteroidsSmall.push_back(asteroid1);
				mAsteroidsSmall.push_back(asteroid2);
			}
		}
	}

	for (Asteroid& asteroid : mAsteroidsLarge)
	{
		asteroid.position = asteroid.position + asteroid.velocity * dt;
		Wrap(asteroid);
	}

	for (Asteroid& asteroid : mAsteroidsMedium)
	{
		asteroid.position = asteroid.position + asteroid.velocity * dt;
		Wrap(asteroid);
	}

	for (Asteroid& asteroid : mAsteroidsSmall)
	{
		asteroid.position = asteroid.position + asteroid.velocity * dt;
		Wrap(asteroid);
	}

	if (mAsteroidsLarge.size() < 2 && mAsteroidTimer.Expired())
	{
		mAsteroidTimer.Reset();
		mAsteroidsLarge.push_back(SpawnAsteroid(mSizeLarge));
	}
	mAsteroidTimer.Tick(dt);

	mBullets.erase(remove_if(mBullets.begin(), mBullets.end(), [this](Bullet& bullet)
		{
			Rect bulletRect = bullet.Collider();
			if (!SDL_HasIntersectionF(&bulletRect, &SCREEN)) return true;

			for (Asteroid& asteroid : mAsteroidsLarge)
			{
				Rect asteroidRect = asteroid.Collider();
				if (SDL_HasIntersectionF(&asteroidRect, &bulletRect))
				{
					asteroid.health -= bullet.damage;
					return true;
				}
			}

			for (Asteroid& asteroid : mAsteroidsMedium)
			{
				Rect asteroidRect = asteroid.Collider();
				if (SDL_HasIntersectionF(&asteroidRect, &bulletRect))
				{
					asteroid.health -= bullet.damage;
					return true;
				}
			}

			for (Asteroid& asteroid : mAsteroidsSmall)
			{
				Rect asteroidRect = asteroid.Collider();
				if (SDL_HasIntersectionF(&asteroidRect, &bulletRect))
				{
					asteroid.health -= bullet.damage;
					return true;
				}
			}

			return false;
		}), mBullets.end());

	mAsteroidsLarge.erase(remove_if(mAsteroidsLarge.begin(), mAsteroidsLarge.end(), [this](const Asteroid& asteroid)
		{
			return asteroid.health <= 0.0f;
		}), mAsteroidsLarge.end());

	mAsteroidsMedium.erase(remove_if(mAsteroidsMedium.begin(), mAsteroidsMedium.end(), [this](const Asteroid& asteroid)
		{
			return asteroid.health <= 0.0f;
		}), mAsteroidsMedium.end());

	mAsteroidsSmall.erase(remove_if(mAsteroidsSmall.begin(), mAsteroidsSmall.end(), [this](const Asteroid& asteroid)
		{
			return asteroid.health <= 0.0f;
		}), mAsteroidsSmall.end());
}

void GameScene::OnRender()
{
	DrawTexture(mGameBackgroundTex, { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT });

	for (const Asteroid& asteroid : mAsteroidsLarge)
	{
		float angle = atan2(asteroid.velocity.y, asteroid.velocity.x) * RAD2DEG;
		DrawTexture(mAsteroidTex, { asteroid.position.x - asteroid.width * 0.5f, asteroid.position.y - asteroid.height * 0.5f, asteroid.width, asteroid.height }, angle);
	}

	for (const Asteroid& asteroid : mAsteroidsMedium)
	{
		float angle = atan2(asteroid.velocity.y, asteroid.velocity.x) * RAD2DEG;
		DrawTexture(mAsteroidClusterTex, { asteroid.position.x - asteroid.width * 0.5f, asteroid.position.y - asteroid.height * 0.5f, asteroid.width, asteroid.height }, angle);
	}

	for (const Asteroid& asteroid : mAsteroidsSmall)
	{
		float angle = atan2(asteroid.velocity.y, asteroid.velocity.x) * RAD2DEG;
		DrawTexture(mAsteroidClusterTex, { asteroid.position.x - asteroid.width * 0.5f, asteroid.position.y - asteroid.height * 0.5f, asteroid.width, asteroid.height }, angle);
	}

	for (const Bullet& bullet : mBullets)
	{
		DrawTexture(mBulletTex, { bullet.position.x - bullet.width * 0.5f, bullet.position.y - bullet.height * 0.5f, bullet.width, bullet.height });
	}

	float increasedSize = 1.2f;
	float shipWidth = mShip.width * increasedSize;
	float shipHeight = mShip.height * increasedSize;
	float shipAngle = mShipRotation;  // Use the ship's rotation angle
	DrawTexture(mShipTex, { mShip.position.x - shipWidth * 0.5f, mShip.position.y - shipHeight * 0.5f, shipWidth, shipHeight }, shipAngle);
}



void GameScene::ResetGame()
{
	mShip.position = Point{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f };
	mShip.velocity = Point{ 0.0f, 0.0f };
	mAsteroidsLarge.clear();
	mAsteroidsMedium.clear();
	mAsteroidsSmall.clear();
	mBullets.clear();
	mAsteroidsLarge.push_back(SpawnAsteroid(mSizeLarge));
}

void GameScene::Wrap(Entity& entity)
{
	float halfWidth = entity.width * 0.5f;
	float halfHeight = entity.height * 0.5f;

	if (entity.position.x + halfWidth < 0.0f)
		entity.position.x = SCREEN_WIDTH + halfWidth;
	else if (entity.position.x - halfWidth > SCREEN_WIDTH)
		entity.position.x = -halfWidth;

	if (entity.position.y + halfHeight < 0.0f)
		entity.position.y = SCREEN_HEIGHT + halfHeight;
	else if (entity.position.y - halfHeight > SCREEN_HEIGHT)
		entity.position.y = -halfHeight;
}
GameScene::Asteroid GameScene::SpawnAsteroid(float size)
{
	Asteroid asteroid;
	asteroid.width = asteroid.height = size;

	int spawnSide = Random(0, 3);
	float x = 0.0f, y = 0.0f;
	switch (spawnSide)
	{
	case 0:
		x = 0.0f;
		y = Random(0.0f, static_cast<float>(SCREEN_HEIGHT));
		break;
	case 1:
		x = static_cast<float>(SCREEN_WIDTH);
		y = Random(0.0f, static_cast<float>(SCREEN_HEIGHT));
		break;
	case 2:
		x = Random(0.0f, static_cast<float>(SCREEN_WIDTH));
		y = 0.0f;
		break;
	case 3:
		x = Random(0.0f, static_cast<float>(SCREEN_WIDTH));
		y = static_cast<float>(SCREEN_HEIGHT);
		break;
	}

	asteroid.position = { x, y };

	Point toCenter = Normalize(Point{ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f } - asteroid.position);
	asteroid.velocity = toCenter * Random(20.0f, 200.0f);

	return asteroid;
}

void PauseScene::OnEnter()
{
	mPauseTex = LoadTexture("../Assets/img/Hanger.png"); 
	PauseTex = LoadTexture("../Assets/img/Pause.png");

	mPauseRec.x = static_cast<float>(SCREEN_WIDTH) * 0.5f - 200.0f;
	mPauseRec.y = static_cast<float>(SCREEN_HEIGHT) * 0.3f;
	mPauseRec.w = 400.0f;
	mPauseRec.h = 100.0f;
}

void PauseScene::OnExit()
{
	UnloadTexture(mPauseTex);
	UnloadTexture(PauseTex);
}

void PauseScene::OnUpdate(float dt)
{
	if (IsKeyDown(SDL_SCANCODE_G))
	{
		Scene::Change(Scene::GAME);
	}
	if (IsKeyDown(SDL_SCANCODE_E))
	{
		Scene::Change(Scene::EXIT);
	}
}

void PauseScene::OnRender()
{
	DrawTexture(mPauseTex, { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT });
	SDL_Rect PauseRec = { static_cast<int>(mPauseRec.x), static_cast<int>(mPauseRec.y), static_cast<int>(mPauseRec.w), static_cast<int>(mPauseRec.h) };
	DrawTexture(PauseTex, mPauseRec);
}


void ExitScene::OnEnter()
{
	SDL_Quit();
}

void ExitScene::OnExit()
{
}

void ExitScene::OnUpdate(float dt)
{
}

void ExitScene::OnRender()
{
	//dont need it cause we are closing the game
}
