#include "EmberbornGame.h"
#include "core/Constants.h"
#include "core/LevelGenerator.h"
#include "thengine/DebugLogger.h"
#include "thengine/graphics/Shader.h"
#include "thengine/Input.h"
#include "thengine/Renderer.h"
#include "entity/Entity.h"
#include "entity/PlayerEntity.h"
#include "entity/PlayerController.h"
#include "entity/EntityRenderer.h"
#include <unordered_map>
#include <format>

namespace emberborn {

// Map defined in the emberborn namespace for clean separation
const std::unordered_map<Tile::TileType, std::string> filesForTiles = {
	{ Tile::TileType::Void,  "assets/Dungeon/void_2.png" },
	{ Tile::TileType::Floor, "assets/Dungeon/floor_1.png" },
	{ Tile::TileType::Wall,  "assets/Dungeon/wall_brown_1.png" },
	{ Tile::TileType::Water, "assets/Dungeon/water_blue_1.png" }
};

} // namespace emberborn

// Level is now initialized to the DEFAULT_MAP_WIDTH and DEFAULT_MAP_HEIGHT from Constants.h
EmberbornGame::EmberbornGame(const std::string &title, int width, int height)
	: thengine::Game(title, width, height),
	  m_tileMap(emberborn::DEFAULT_MAP_WIDTH, emberborn::DEFAULT_MAP_HEIGHT) {}

EmberbornGame::~EmberbornGame() = default;

void EmberbornGame::onInitialize() {
	LOG_INFO() << "EmberbornGame initialized!";
	m_content = std::make_unique<thengine::ContentManager>(getRenderer());
	m_spriteBatch = std::make_unique<thengine::SpriteBatch>(getRenderer());

	// Center the camera in the middle of the generated 100x100 dungeon map
	m_camera.setPosition(thengine::Vector2(
		static_cast<float>(emberborn::DEFAULT_MAP_WIDTH) * emberborn::TILE_SIZE * 0.5f,
		static_cast<float>(emberborn::DEFAULT_MAP_HEIGHT) * emberborn::TILE_SIZE * 0.5f
	));
	m_camera.setZoom(1.0f);
	m_camera.setRotation(0.0f);
}

void EmberbornGame::onLoadContent() {
	auto vert = m_content->load<::thengine::Shader>("assets/shaders/basic.vert");
	auto frag = m_content->load<::thengine::Shader>("assets/shaders/basic.frag");
	m_basicEffect = std::make_shared<::thengine::BasicEffect>(getRenderer(), vert, frag);

	m_font = m_content->loadFont("assets/fonts/DejaVuSansMono.ttf", 36.0f);

	// Load and register textures using the map defined in the emberborn namespace
	for (const auto& [tileType, fileName] : emberborn::filesForTiles) {
		auto texture = m_content->load<thengine::Texture>(fileName);
		if (texture) {
			m_tileRenderer.registerTexture(tileType, texture);
			LOG_INFO() << "Loaded and registered tile texture: " << fileName;
		} else {
			LOG_ERROR() << "Failed to load tile texture: " << fileName;
		}
	}

	// Generate a larger basic dungeon level (10 rooms)
	emberborn::LevelGenerator::generateBasicLayout(m_tileMap, 5, 12, 10);

	// Load player texture and register it
	auto playerTex = m_content->load<thengine::Texture>("assets/player.png");
	if (playerTex) {
		m_entityRenderer.registerTexture("player", playerTex);
		LOG_INFO() << "Loaded and registered player texture.";
	} else {
		LOG_ERROR() << "Failed to load player texture.";
	}

	// Find the first floor tile to spawn the player
	int startX = -1, startY = -1;
	for (int y = 0; y < m_tileMap.getHeight(); ++y) {
		for (int x = 0; x < m_tileMap.getWidth(); ++x) {
			if (m_tileMap.getTile(x, y).type == emberborn::Tile::TileType::Floor) {
				startX = x;
				startY = y;
				break;
			}
		}
		if (startX != -1) break;
	}

	if (startX != -1 && startY != -1) {
		m_player = std::make_shared<emberborn::PlayerEntity>();
		m_player->setX(startX);
		m_player->setY(startY);
		m_entities.push_back(m_player);

		m_playerController = std::make_unique<emberborn::PlayerController>();
		m_playerController->possess(m_player.get());

		// Center camera on the player
		m_camera.setPosition(thengine::Vector2(
			static_cast<float>(startX) * emberborn::TILE_SIZE + emberborn::TILE_SIZE * 0.5f,
			static_cast<float>(startY) * emberborn::TILE_SIZE + emberborn::TILE_SIZE * 0.5f
		));
		LOG_INFO() << "Player spawned at: " << startX << ", " << startY;
	} else {
		LOG_ERROR() << "Failed to find a valid spawn tile for the player.";
	}
}

bool EmberbornGame::onUpdate(float deltaTime) {
	m_fpsCounter.update(deltaTime);

	// Tick player controller
	if (m_playerController) {
		m_playerController->update(deltaTime, m_tileMap);
	}

	if (thengine::Input::isKeyPressed(thengine::Key::Escape)) {
		LOG_INFO() << "ESCAPE pressed, exiting Emberborn.";
		return false;
	}

	// Camera movement controls
	float speed = 400.0f;
	thengine::Vector2 camPos = m_camera.getPosition();
	if (thengine::Input::isKeyPressed(thengine::Key::W)) {
		camPos.y -= speed * deltaTime;
	}
	if (thengine::Input::isKeyPressed(thengine::Key::S)) {
		camPos.y += speed * deltaTime;
	}
	if (thengine::Input::isKeyPressed(thengine::Key::A)) {
		camPos.x -= speed * deltaTime;
	}
	if (thengine::Input::isKeyPressed(thengine::Key::D)) {
		camPos.x += speed * deltaTime;
	}
	m_camera.setPosition(camPos);

	// Camera zoom controls
	if (thengine::Input::isKeyPressed(thengine::Key::Up)) {
		m_camera.setZoom(m_camera.getZoom() + deltaTime * 1.0f);
	}
	if (thengine::Input::isKeyPressed(thengine::Key::Down)) {
		m_camera.setZoom(m_camera.getZoom() - deltaTime * 1.0f);
		if (m_camera.getZoom() < 0.1f) {
			m_camera.setZoom(0.1f);
		}
	}

	// Camera rotation controls
	if (thengine::Input::isKeyPressed(thengine::Key::Left)) {
		m_camera.setRotation(m_camera.getRotation() - deltaTime * 1.0f);
	}
	if (thengine::Input::isKeyPressed(thengine::Key::Right)) {
		m_camera.setRotation(m_camera.getRotation() + deltaTime * 1.0f);
	}

	return true;
}

void EmberbornGame::onRender(float deltaTime) {
	// Clear the screen to a deep, dark atmospheric black/red color
	getRenderer().clear(15, 2, 2, 255);

	// Set ambient light to fully lit (white) and clear point lights so the level is fully bright
	m_basicEffect->setAmbientColor(thengine::Color(255, 255, 255, 255));
	m_basicEffect->setAmbientIntensity(1.0f);
	m_basicEffect->clearLights();

	// Calculate camera view transformation matrix
	thengine::Matrix4 cameraTransform = m_camera.getTransform(
		static_cast<float>(emberborn::WINDOW_WIDTH),
		static_cast<float>(emberborn::WINDOW_HEIGHT)
	);

	// Render the map inside the camera view bounds (frustum culled)
	m_spriteBatch->begin(m_basicEffect, cameraTransform);
	m_tileRenderer.render(
		*m_spriteBatch,
		m_tileMap,
		m_camera,
		static_cast<float>(emberborn::WINDOW_WIDTH),
		static_cast<float>(emberborn::WINDOW_HEIGHT),
		emberborn::TILE_SIZE
	);
	m_entityRenderer.render(
		*m_spriteBatch,
		m_entities,
		emberborn::TILE_SIZE
	);
	m_spriteBatch->end();

	// Draw HUD overlay in screen space
	if (m_font) {
		m_spriteBatch->begin(m_basicEffect, thengine::Matrix4::identity());

		std::string text = "Welcome... to hell";
		thengine::Vector2 size = m_font->measureString(text);
		thengine::Vector2 position(
			(static_cast<float>(emberborn::WINDOW_WIDTH) - size.x) * 0.5f,
			(static_cast<float>(emberborn::WINDOW_HEIGHT) - size.y) * 0.5f
		);

		// Deep red/crimson colored warning text
		m_spriteBatch->drawString(m_font, text, position, thengine::Color(200, 30, 30, 255));

		// Draw FPS in the top right corner
		std::string fpsStr = std::format("FPS: {:.1f}", m_fpsCounter.getFps());
		thengine::Vector2 fpsSize = m_font->measureString(fpsStr);
		thengine::Vector2 fpsPos(
			static_cast<float>(emberborn::WINDOW_WIDTH) - fpsSize.x - 20.0f,
			35.0f
		);
		m_spriteBatch->drawString(m_font, fpsStr, fpsPos, thengine::Color(0, 255, 255, 255));

		m_spriteBatch->end();
	}
}

void EmberbornGame::onReleaseContent() {}

void EmberbornGame::onShutdown() {}
