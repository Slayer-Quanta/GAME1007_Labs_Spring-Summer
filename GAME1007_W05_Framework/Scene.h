#pragma once
#include "Core.h"
#include <array>
#include <vector>
constexpr int SCREEN_WIDTH = 1024;
constexpr int SCREEN_HEIGHT = 768;

void OnTitleGui(void* data);
void OnGameGui(void* data);
void OnLab1BGui(void* data);

class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	virtual void OnEnter() {}
	virtual void OnExit() {}

	virtual void OnUpdate(float dt) = 0;
	virtual void OnRender() = 0;

	enum Type : size_t
	{
		TITLE,
		GAME,
		LAB_1A,
		LAB_1B,
		LAB_2,
		COUNT
	};

	static void Init();
	static void Exit();

	static void Update(float dt);
	static void Render();

	static void Change(Type type);

private:
	static Type sCurrent;
	static std::array<Scene*, COUNT> sScenes;
};

class TitleScene : public Scene
{
public:
	void OnEnter() final;
	void OnExit() final;

	void OnUpdate(float dt) final;
	void OnRender() final;

private:
	Rect mBackRec = { 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT };
	Rect mFrontRec = { 0.0f, 0.0f, 60.0f, 40.0f };
};

class GameScene : public Scene
{
public:
	GameScene();
	~GameScene() final;

	void OnEnter() final;
	void OnExit() final;

	void OnUpdate(float dt) final;
	void OnRender() final;

	Texture* mShipTex = nullptr;
	Rect mShipRec;
	float mShipSpeed;
	float mTurretX;  // Turret X position
	float mTurretY;  // Turret Y position

};

class Lab1AScene : public Scene
{
public:
	Lab1AScene();
	~Lab1AScene() final;

	void OnUpdate(float dt) final;
	void OnRender() final;

	Texture* mTexEnterprise = nullptr;
	Texture* mTexD7 = nullptr;

	struct Ship
	{
		Rect rec;
		Point dir;
		Texture* tex;
	};

	std::array<Ship, 6> mShips;
};

class Lab1BScene : public Scene
{
public:
	Lab1BScene();
	~Lab1BScene() final;

	void OnEnter() final;
	void OnExit() final;

	void OnUpdate(float dt) final;
	void OnRender() final;

private:
	Sound* mEngine = nullptr;
	Sound* mExplode = nullptr;
	Sound* mFire = nullptr;
	Sound* mTeleport = nullptr;
	Music* mMusic = nullptr;

	bool mMusicPlaying = false;

	friend void OnLab1BGui(void* data);
};

class Lab2Scene : public Scene
{
public:
	Lab2Scene();
	~Lab2Scene() final;

	void OnEnter() final;
	void OnExit() final;

	void OnUpdate(float dt) final;
	void OnRender() final;

	Texture* mTexturret = nullptr;
	Texture* mTexEnemy = nullptr;
	Texture* mTexBullet = nullptr;

	struct Entity
	{
		Rect rec;
	};

	struct Turret : public Entity
	{
		float cooldown = 1.0f;
		int kills = 0;
	};

	struct Enemy : public Entity
	{
		float health = 100.0f;
	};

	struct Bullet : public Entity
	{
		float damage = 10.0f;
		Point direction;
		Turret* parent = nullptr;
		Point velocity;      // New variable for storing the velocity
		Point acceleration;  // New variable for storing the acceleration
	};

	std::vector<Turret> mTurrets;
	std::vector<Enemy> mEnemies;
	std::vector<Enemy*> enemiesToRemove;
	std::vector<Bullet> mBullets;
};