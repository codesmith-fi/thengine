#pragma once

#include "thengine/Game.h"
#include "thengine/ContentManager.h"
#include "thengine/graphics/BasicEffect.h"
#include "thengine/graphics/SpriteBatch.h"
#include "thengine/graphics/SpriteFont.h"
#include "thengine/view/Camera2D.h"
#include "thengine/FpsCounter.h"
#include "core/TileMap.h"
#include "graphics/TileRenderer.h"
#include <memory>
#include <string>
#include <vector>

#include "entity/EntityRenderer.h"

namespace emberborn {
class Entity;
class PlayerEntity;
class PlayerController;
class MonsterController;
}

class EmberbornGame : public thengine::Game {
public:
	EmberbornGame(const std::string &title, int width, int height);
	~EmberbornGame() override;

protected:
	void onInitialize() override;
	void onLoadContent() override;
	bool onUpdate(float deltaTime) override;
	void onRender(float deltaTime) override;
	void onReleaseContent() override;
	void onShutdown() override;

private:
	std::unique_ptr<thengine::SpriteBatch> m_spriteBatch;
	std::unique_ptr<thengine::ContentManager> m_content;
	std::shared_ptr<thengine::BasicEffect> m_basicEffect;
	std::shared_ptr<thengine::SpriteFont> m_font;

	emberborn::TileMap m_tileMap;
	emberborn::TileRenderer m_tileRenderer;
	thengine::Camera2D m_camera;
	thengine::FpsCounter m_fpsCounter;

	emberborn::EntityRenderer m_entityRenderer;
	std::vector<std::shared_ptr<emberborn::Entity>> m_entities;
	std::unique_ptr<emberborn::PlayerController> m_playerController;
	std::shared_ptr<emberborn::PlayerEntity> m_player;
	std::shared_ptr<emberborn::Entity> m_monster;
	std::unique_ptr<emberborn::MonsterController> m_monsterController;
};
