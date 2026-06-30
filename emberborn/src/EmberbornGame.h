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
#include "thengine/view/VisibilityTypes.h"
#include "thengine/view/LightSource.h"

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

	thengine::VisibilityPolygon m_playerVisibility;
	std::shared_ptr<thengine::Texture> m_pixelTex;

	float m_torchTime = 0.0f;
	float m_currentTorchRadius = 400.0f;
	float m_targetTorchRadius = 400.0f;
	float m_crackleTimer = 0.0f;

	std::vector<thengine::LightSource> m_lights;

	struct VisibleLight {
		thengine::LightSource light;
		thengine::VisibilityPolygon polygon;
	};
	std::vector<VisibleLight> m_visibleLights;

	bool isLightInViewport(const thengine::LightSource& light, const thengine::Vector2& camCenter, float camWidth, float camHeight) const;
	bool hasLineOfSight(int x1, int y1, int x2, int y2) const;
	void drawLine(thengine::SpriteBatch& spriteBatch, const thengine::Vector2& start, const thengine::Vector2& end, const thengine::Color& color);
};
