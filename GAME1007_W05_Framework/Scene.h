#pragma once
#include "Core.h"
#include <array>
#include <vector>
constexpr int SCREEN_WIDTH = 1024;
constexpr int SCREEN_HEIGHT = 768;
constexpr Rect SCREEN = { 0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };

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
		PAUSE,
		EXIT,
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
	Texture* mTitleBackgroundTex = nullptr;
	Texture* mTitleTex = nullptr;
	Rect mBackgroundRec;
	Rect mTitleRec;
	Music* TitleMusic;
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
	void ResetGame();

	Texture* mShipTex = nullptr;
	Rect mShipRec;
	float mShipSpeed;
	float mShipRotation;  // Add this line
	Texture* mShipSpriteSheetTex = nullptr;
	Texture* mAsteroidTex = nullptr;
	Texture* mAsteroidClusterTex = nullptr;
	Texture* mGameBackgroundTex = nullptr;
	Texture* mBulletTex = nullptr;
	Sound* mBulletSound;
	bool mIsFiring;
	float mBurstTimer;
	Rect mGameBackgroundRec;
	Music* GameMusic;
	struct Timer
	{
		float duration = 0.0f;	
		float elapsed = 0.0f;

		bool Expired() { return elapsed >= duration; }
		void Reset() { elapsed = 0.0f; }
		void Tick(float dt) { elapsed += dt; }
		float Normalize() { return Clamp(elapsed / duration, 0.0f, 1.0f); }
	};

	struct Rigidbody
	{
		Point position{};
		Point velocity{};
		Point acceleration{};
		Point thrust{};
		Point Rotation{};
		Point direction{ 1.0f, 0.0f };
		float angularSpeed = 300.0f;
	};

	struct Entity : public Rigidbody
	{
		float width = 0.0f;
		float height = 0.0f;

		Rect Collider() const
		{
			return { position.x - width * 0.5f, position.y - height * 0.5f, width, height };
		}
	};

	struct Bullet : public Entity
	{
		float damage = 100.0f;
		void Draw() const
		{
			Color bulletColor = { 255, 0, 0, 255 };
			DrawRect(Collider(), bulletColor);
			DrawLine(position, position + direction * 20.0f, bulletColor);
		}
	};

	// Add on to this class if necessary
	struct Asteroid : public Entity
	{
		float health = 100.0f;
		void Draw() const
		{
			Color asteroidColor = { 255, 0, 255, 255 };
			DrawRect(Collider(), asteroidColor);
			DrawLine(position, position + direction * 20.0f, asteroidColor);
		}
	};

	struct Ship : public Entity
	{
		float speed = 100.0f;
		float acceleration;           // Acceleration in the forward direction
		float sidewaysAcceleration;
		float rotation;
		void Draw() const
		{
			DrawRect(Collider(), col);
			DrawTexture(tex, Collider());
			DrawLine(position, position + direction * 100.0f, col);

		}

		Timer bulletCooldown;
		Texture* tex = nullptr;
		Color col{};
	} mShip;

	Color mTestColor{ 255, 255, 255, 255 };
	std::vector<Bullet> mBullets;
	Timer mAsteroidTimer;
	std::vector<Asteroid> mAsteroids;
	std::vector<Asteroid> mAsteroidsLarge;
	std::vector<Asteroid> mAsteroidsMedium;
	std::vector<Asteroid> mAsteroidsSmall;
	const float mSizeLarge = 75.0f;
	const float mSizeMedium = 50.0f;
	const float mSizeSmall = 25.0f;

	Asteroid SpawnAsteroid(float size);
	void Wrap(Entity& entity);

};

class PauseScene : public Scene
{
public:
	void OnEnter() final;
	void OnExit() final;

	void OnUpdate(float dt) final;
	void OnRender() final;

	Texture* mPauseTex = nullptr;
	Texture* PauseTex = nullptr;
	Rect mPauseRec;
};

class ExitScene : public Scene
{
public:
	void OnEnter() final;
	void OnExit() final;

	void OnUpdate(float dt) final;
	void OnRender() final;
};
