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
	sScenes[LAB_1A] = new Lab1AScene;
	sScenes[LAB_1B] = new Lab1BScene;
	sScenes[LAB_2] = new Lab2Scene;
	sCurrent = LAB_2;
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
	SetGuiCallback(OnTitleGui, this);
}

void TitleScene::OnExit()
{
	SetGuiCallback(nullptr, nullptr);
}

void TitleScene::OnUpdate(float dt)
{
}

void TitleScene::OnRender()
{
	DrawRect(mBackRec, { 0, 0, 0, 255 });
	DrawRect(mFrontRec, { 255, 255, 255, 255 });
}

void OnTitleGui(void* data)
{
	TitleScene& scene = *(TitleScene*)data;
	if (ImGui::Button("Begin!"))
	{
		Scene::Change(Scene::GAME);
	}
}

GameScene::GameScene()
{
	mShipTex = LoadTexture("../Assets/img/enterprise.png");
}

GameScene::~GameScene()
{
	UnloadTexture(mShipTex);
}

void GameScene::OnEnter()
{
	SetGuiCallback(OnGameGui, this);

	XMLDocument doc;
	doc.LoadFile("Game.xml");

	XMLElement* gameData = doc.FirstChildElement();
	XMLElement* shipData = gameData->FirstChildElement();
	shipData->QueryAttribute("x", &mShipRec.x);
	shipData->QueryAttribute("y", &mShipRec.y);
	shipData->QueryAttribute("w", &mShipRec.w);
	shipData->QueryAttribute("h", &mShipRec.h);
	shipData->QueryAttribute("speed", &mShipSpeed);
}

void GameScene::OnExit()
{
	XMLDocument doc;
	XMLNode* root = doc.NewElement("Game");
	doc.InsertEndChild(root);

	XMLElement* ship = doc.NewElement("Ship");
	ship->SetAttribute("x", mShipRec.x);
	ship->SetAttribute("y", mShipRec.y);
	ship->SetAttribute("w", mShipRec.w);
	ship->SetAttribute("h", mShipRec.h);
	ship->SetAttribute("speed", mShipSpeed);
	root->InsertEndChild(ship);

	doc.SaveFile("Game.xml");

	SetGuiCallback(nullptr, nullptr);
}

void GameScene::OnUpdate(float dt)
{
	if (IsKeyDown(SDL_SCANCODE_A))
	{
		mShipRec.x -= mShipSpeed * dt;
	}
	if (IsKeyDown(SDL_SCANCODE_D))
	{
		mShipRec.x += mShipSpeed * dt;
	}
	if (IsKeyDown(SDL_SCANCODE_W))
	{
		mShipRec.y -= mShipSpeed * dt;
	}
	if (IsKeyDown(SDL_SCANCODE_S))
	{
		mShipRec.y += mShipSpeed * dt;
	}
}

void GameScene::OnRender()
{
	DrawTexture(mShipTex, mShipRec);
}

void OnGameGui(void* data)
{
	GameScene& scene = *(GameScene*)data;
	{
		Scene::Change(Scene::TITLE);

	}
}

Lab1AScene::Lab1AScene()
{
	mTexEnterprise = LoadTexture("../Assets/img/enterprise.png");
	mTexD7 = LoadTexture("../Assets/img/d7.png");

	for (size_t i = 0; i < mShips.size(); i++)
	{
		Ship& ship = mShips[i];
		ship.rec.x = SCREEN_WIDTH * 0.1f;

		const float space = SCREEN_HEIGHT / mShips.size();
		ship.rec.y = i * space + space * 0.5f;

		ship.rec.w = 60.0f;
		ship.rec.h = 40.0f;

		ship.dir = { 1.0f, 1.0f };

		if (i % 2 == 0)
			ship.tex = mTexEnterprise;
		else
			ship.tex = mTexD7;
	}
}

Lab1AScene::~Lab1AScene()
{
	UnloadTexture(mTexD7);
	UnloadTexture(mTexEnterprise);
}

void Lab1AScene::OnUpdate(float dt)
{
	const float speed = 500.0f;
	for (Ship& ship : mShips)
	{
		Rect& rec = ship.rec;

		if (rec.x + rec.w >= SCREEN_WIDTH)
			ship.dir.x = -1.0f;
		else if (rec.x <= 0.0f)
			ship.dir.x = 1.0f;

		if (rec.y + rec.h >= SCREEN_HEIGHT)
			ship.dir.y = -1.0f;
		else if (rec.y <= 0.0f)
			ship.dir.y = 1.0f;

		rec.x += ship.dir.x * speed * dt;
		rec.y += ship.dir.y * speed * dt;
	}
}

void Lab1AScene::OnRender()
{
	for (const Ship& ship : mShips)
		DrawTexture(ship.tex, ship.rec);
}

void OnLab1BGui(void* data)
{
	Lab1BScene& scene = *(Lab1BScene*)data;

	if (scene.mMusicPlaying)
	{
		if (ImGui::Button("Pause"))
		{
			scene.mMusicPlaying = false;
			PauseMusic();
		}
	}
	else
	{
		if (ImGui::Button("Resume"))
		{
			scene.mMusicPlaying = true;
			ResumeMusic();
		}
	}

	if (ImGui::Button("Engine"))
		PlaySound(scene.mEngine);

	if (ImGui::Button("Explode"))
		PlaySound(scene.mExplode);

	if (ImGui::Button("Fire"))
		PlaySound(scene.mFire);

	if (ImGui::Button("Teleport"))
		PlaySound(scene.mTeleport);
}

Lab1BScene::Lab1BScene()
{
	mEngine = LoadSound("../Assets/aud/Engines.wav");
	mExplode = LoadSound("../Assets/aud/Explode.wav");
	mFire = LoadSound("../Assets/aud/Fire.wav");
	mTeleport = LoadSound("../Assets/aud/Teleport.wav");
	mMusic = LoadMusic("../Assets/aud/Wings.mp3");

	// Pause/Resume music() pause/resume the stream, but they don't play.
	// Hence, we need to ensure our music is playing before pausing/resuming!
	PlayMusic(mMusic);
	if (!mMusicPlaying)
		PauseMusic();
}

Lab1BScene::~Lab1BScene()
{
	UnloadMusic(mMusic);
	UnloadSound(mEngine);
	UnloadSound(mExplode);
	UnloadSound(mFire);
	UnloadSound(mTeleport);
}

void Lab1BScene::OnEnter()
{
	SetGuiCallback(OnLab1BGui, this);
}

void Lab1BScene::OnExit()
{
	SetGuiCallback(nullptr, nullptr);
}

void Lab1BScene::OnUpdate(float dt)
{
}

void Lab1BScene::OnRender()
{
}

Lab2Scene::Lab2Scene()
{
}

Lab2Scene::~Lab2Scene()
{
}

void Lab2Scene::OnEnter()
{
	XMLDocument doc;
	doc.LoadFile("Turrets.xml");

	XMLElement* element = doc.FirstChildElement();
	while (element != nullptr)
	{
		int kills = 0;
		float cooldown = 1.0f;
		Rect rec;
		element->QueryAttribute("kills", &kills);
		element->QueryAttribute("cooldown", &cooldown);
		element->QueryAttribute("x", &rec.x);
		element->QueryAttribute("y", &rec.y);
		element->QueryAttribute("w", &rec.w);
		element->QueryAttribute("h", &rec.h);

		Turret turret;
		turret.rec = rec;
		turret.cooldown = cooldown;
		turret.kills = kills;
		mTurrets.push_back(turret);

		element = element->NextSiblingElement();
	}
}


void Lab2Scene::OnExit()
{
	XMLDocument doc;
	for (const Turret& turret : mTurrets)
	{
		XMLElement* element = doc.NewElement("Turret");
		element->SetAttribute("x", turret.rec.x);
		element->SetAttribute("y", turret.rec.y);
		element->SetAttribute("w", turret.rec.w);
		element->SetAttribute("h", turret.rec.h);
		element->SetAttribute("kills", turret.kills);
		element->SetAttribute("cooldown", turret.cooldown);
		doc.InsertEndChild(element);
	}
	doc.SaveFile("Turrets.xml");
}


void Lab2Scene::OnUpdate(float dt)
{
	if (IsKeyPressed(SDL_SCANCODE_T))
	{
		Turret turret;
		turret.rec.x = rand() % SCREEN_WIDTH;
		turret.rec.y = rand() % SCREEN_HEIGHT;
		turret.rec.w = 100.0f;
		turret.rec.h = 100.0f;
		mTexturret = LoadTexture("../Assets/img/Turret.png");
		mTurrets.push_back(turret);
	}

	if (IsKeyPressed(SDL_SCANCODE_R))
		mTurrets.pop_back();

	if (IsKeyPressed(SDL_SCANCODE_E))
	{
		Enemy enemy;
		enemy.rec.x = rand() % SCREEN_WIDTH;
		enemy.rec.y = rand() % SCREEN_HEIGHT;
		enemy.rec.w = 60.0f;
		enemy.rec.h = 40.0f;
		mTexEnemy = LoadTexture("../Assets/img/Eaburr.png");
		mEnemies.push_back(enemy);
	}

	for (Turret& turret : mTurrets)
	{
		turret.cooldown -= dt;
		if (turret.cooldown <= 0.0f)
		{
			turret.cooldown = 1.0f;

			// Find the nearest enemy to shoot at:
			float nearestDistance = FLT_MAX;
			const Enemy* nearestEnemy = nullptr;

			for (const Enemy& enemy : mEnemies)
			{
				float distance = Distance({ turret.rec.x, turret.rec.y }, { enemy.rec.x, enemy.rec.y });
				if (distance < nearestDistance)
				{
					nearestDistance = distance;
					nearestEnemy = &enemy;
				}
			}

			if (nearestEnemy != nullptr)
			{
				// AB = B - A
				Bullet bullet;
				bullet.rec.w = 30.0f;
				bullet.rec.h = 20.0f;
				bullet.direction = Normalize({ nearestEnemy->rec.x - turret.rec.x, nearestEnemy->rec.y - turret.rec.y });
				bullet.rec.x = turret.rec.x + turret.rec.w * bullet.direction.x;
				bullet.rec.y = turret.rec.y + turret.rec.h * bullet.direction.y;
				bullet.parent = &turret;
				bullet.velocity.x = 0.0f;
				bullet.velocity.y = 0.0f;
				bullet.acceleration.x = 0.0f;
				bullet.acceleration.y = 0.0f;
				mTexBullet = LoadTexture("../Assets/img/Bullet.png");
				mBullets.push_back(bullet);
			}
		}

		for (Bullet& bullet : mBullets)
		{
			float acceleration = 10.0f;
			float maxSpeed = 200.0f;
			float dtSquared = dt * dt;

			// Update velocity based on acceleration
			bullet.velocity.x += bullet.direction.x * acceleration * dt;
			bullet.velocity.y += bullet.direction.y * acceleration * dt;

			// Clamp velocity to the maximum speed
			float velocityMagnitude = std::sqrt(bullet.velocity.x * bullet.velocity.x + bullet.velocity.y * bullet.velocity.y);
			if (velocityMagnitude > maxSpeed)
			{
				bullet.velocity.x = (bullet.velocity.x / velocityMagnitude) * maxSpeed;
				bullet.velocity.y = (bullet.velocity.y / velocityMagnitude) * maxSpeed;
			}

			// Update position based on velocity
			bullet.rec.x += bullet.velocity.x * dt + 0.5f * bullet.acceleration.x * dtSquared;
			bullet.rec.y += bullet.velocity.y * dt + 0.5f * bullet.acceleration.y * dtSquared;
		}
	}
		const float damageMultiplier = 10.0f;

		mBullets.erase(remove_if(mBullets.begin(), mBullets.end(),
			[this, damageMultiplier](Bullet& bullet)
			{
				Rect screen{ 0.0f, 0.0f, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT) };
				if (!SDL_HasIntersectionF(&bullet.rec, &screen))
					return true;

				for (Enemy& enemy : mEnemies)
				{
					if (SDL_HasIntersectionF(&bullet.rec, &enemy.rec))
					{
						enemy.health -= bullet.damage * damageMultiplier;
						if (enemy.health <= 0.0f)
							bullet.parent->kills++;
						return true;
					}
				}

				return false;
			}),
			mBullets.end());

	// Remove dead enemies
	mEnemies.erase(remove_if(mEnemies.begin(), mEnemies.end(),
		[](const Enemy& enemy)
		{
			return enemy.health <= 0.0f;
		}),
		mEnemies.end());

	static float timer = 1.0f;
	timer -= dt;
	if (timer <= 0.0f)
	{
		timer = 1.0f;
		cout << "Enemies: " << mBullets.size() << endl;
		cout << "Bullets: " << mBullets.size() << endl;
	}
}

void Lab2Scene::OnRender()
{
	
	for (const Turret& turret : mTurrets)
		DrawTexture(mTexturret, turret.rec);

	for (const Enemy& enemy : mEnemies)
		DrawTexture(mTexEnemy, enemy.rec);

	for (const Bullet& bullet : mBullets)
		DrawTexture(mTexBullet, bullet.rec);
}