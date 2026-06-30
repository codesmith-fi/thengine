#include "EmberbornGame.h"
#include "core/Constants.h"
#include "core/LevelGenerator.h"
#include "thengine/DebugLogger.h"
#include "thengine/graphics/Shader.h"
#include "thengine/Input.h"
#include "thengine/Renderer.h"
#include "thengine/view/VisibilitySolver.h"
#include "entity/Entity.h"
#include "entity/PlayerEntity.h"
#include "entity/PlayerController.h"
#include "entity/MonsterController.h"
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
		m_entityRenderer.registerTexture(emberborn::EntityType::Player, playerTex);
		LOG_INFO() << "Loaded and registered player texture.";
	} else {
		LOG_ERROR() << "Failed to load player texture.";
	}

	// Load 1x1 pixel primitive texture for line drawing
	m_pixelTex = m_content->load<thengine::Texture>("assets/Primitives/pixel_1x1.png");
	if (m_pixelTex) {
		LOG_INFO() << "Loaded 1x1 pixel primitive texture.";
	} else {
		LOG_ERROR() << "Failed to load 1x1 pixel primitive texture.";
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

	// Find a different floor tile for the monster
	int monsterX = -1, monsterY = -1;
	for (int y = m_tileMap.getHeight() - 1; y >= 0; --y) {
		for (int x = m_tileMap.getWidth() - 1; x >= 0; --x) {
			if (m_tileMap.getTile(x, y).type == emberborn::Tile::TileType::Floor) {
				if (x != startX || y != startY) {
					monsterX = x;
					monsterY = y;
					break;
				}
			}
		}
		if (monsterX != -1) break;
	}

	// Load skeleton texture and register it
	auto skeletonTex = m_content->load<thengine::Texture>("assets/test1.png");
	if (skeletonTex) {
		m_entityRenderer.registerTexture(emberborn::EntityType::Skeleton, skeletonTex);
		LOG_INFO() << "Loaded and registered skeleton texture.";
	} else {
		LOG_ERROR() << "Failed to load skeleton texture.";
	}

	if (monsterX != -1 && monsterY != -1) {
		m_monster = std::make_shared<emberborn::Entity>();
		m_monster->setType(emberborn::EntityType::Skeleton);
		m_monster->setX(monsterX);
		m_monster->setY(monsterY);
		m_monster->setSpeed(0.5f); // slow skeleton
		m_entities.push_back(m_monster);

		m_monsterController = std::make_unique<emberborn::MonsterController>();
		m_monsterController->possess(m_monster.get());
		LOG_INFO() << "Monster spawned at: " << monsterX << ", " << monsterY;
	} else {
		LOG_ERROR() << "Failed to find a valid spawn tile for the monster.";
	}
}

bool EmberbornGame::onUpdate(float deltaTime) {
	m_fpsCounter.update(deltaTime);

	// Tick player controller
	if (m_playerController) {
		m_playerController->update(deltaTime, m_tileMap);
	}

	// Update torch flicker effect
	m_torchTime += deltaTime * 6.0f;
	float baseNoise = std::sin(m_torchTime) * 8.0f;

	m_crackleTimer += deltaTime;
	if (m_crackleTimer >= 0.1f) {
		m_crackleTimer = 0.0f;
		float randVal = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
		m_targetTorchRadius = 385.0f + randVal * 30.0f;
	}

	float lerpFactor = 15.0f * deltaTime;
	if (lerpFactor > 1.0f) {
		lerpFactor = 1.0f;
	}
	m_currentTorchRadius = m_currentTorchRadius + (m_targetTorchRadius + baseNoise - m_currentTorchRadius) * lerpFactor;

	// Calculate player visibility polygon for torch light using dynamic radius
	if (m_player) {
		thengine::Vector2 playerPos(
			static_cast<float>(m_player->getX()) * emberborn::TILE_SIZE + emberborn::TILE_SIZE * 0.5f,
			static_cast<float>(m_player->getY()) * emberborn::TILE_SIZE + emberborn::TILE_SIZE * 0.5f
		);
		m_playerVisibility = thengine::VisibilitySolver::calculateVisibility(playerPos, m_currentTorchRadius, m_tileMap);
	}

	// Tick monster controller
	if (m_monsterController) {
		m_monsterController->update(deltaTime, m_tileMap);
	}

	if (thengine::Input::isKeyPressed(thengine::Key::Escape)) {
		LOG_INFO() << "ESCAPE pressed, exiting Emberborn.";
		return false;
	}

	// Smooth camera follow player position
	if (m_player) {
		thengine::Vector2 targetPos(
			static_cast<float>(m_player->getX()) * emberborn::TILE_SIZE + emberborn::TILE_SIZE * 0.5f,
			static_cast<float>(m_player->getY()) * emberborn::TILE_SIZE + emberborn::TILE_SIZE * 0.5f
		);
		thengine::Vector2 currentPos = m_camera.getPosition();
		float lerpFactor = 10.0f * deltaTime;
		if (lerpFactor > 1.0f) {
			lerpFactor = 1.0f;
		}
		thengine::Vector2 newPos = currentPos + (targetPos - currentPos) * lerpFactor;
		m_camera.setPosition(newPos);
	}

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
	m_spriteBatch->end();

	// Draw player visibility warm light fan and midnight blue ambient shadow mask overlay
	if (m_player && m_pixelTex && !m_playerVisibility.vertices.empty()) {
		thengine::Vector2 playerPos(
			static_cast<float>(m_player->getX()) * emberborn::TILE_SIZE + emberborn::TILE_SIZE * 0.5f,
			static_cast<float>(m_player->getY()) * emberborn::TILE_SIZE + emberborn::TILE_SIZE * 0.5f
		);

		const auto& vertices = m_playerVisibility.vertices;
		size_t count = vertices.size();

		// 1. Build warm amber torch light triangles (Triangle Fan)
		std::vector<thengine::Vertex> lightVertices;
		lightVertices.reserve(count * 3);

		// Alpha/Intensity Modulation based on radius fluctuation
		float ratio = m_currentTorchRadius / 400.0f;
		float alphaScale = ratio * ratio; // quadratic flicker intensity
		if (alphaScale > 1.5f) {
			alphaScale = 1.5f;
		}

		float cR = 255.0f / 255.0f;
		float cG = 200.0f / 255.0f;
		float cB = 50.0f / 255.0f;
		float cA_center = (100.0f / 255.0f) * alphaScale; // soft warm center

		for (size_t i = 0; i < count; ++i) {
			const auto& vCurrent = vertices[i];
			const auto& vNext = vertices[(i + 1) % count];

			// Calculate distance and attenuation for vCurrent
			float dx1 = vCurrent.x - playerPos.x;
			float dy1 = vCurrent.y - playerPos.y;
			float d1 = std::sqrt(dx1 * dx1 + dy1 * dy1);
			float ratio1 = d1 / m_currentTorchRadius;
			float att1 = (1.0f - ratio1) / 0.85f;
			att1 = std::max(0.0f, std::min(1.0f, att1));
			att1 = att1 * att1 * att1; // cubic smooth curves
			float cA_curr = (100.0f / 255.0f) * att1 * alphaScale;

			// Calculate distance and attenuation for vNext
			float dx2 = vNext.x - playerPos.x;
			float dy2 = vNext.y - playerPos.y;
			float d2 = std::sqrt(dx2 * dx2 + dy2 * dy2);
			float ratio2 = d2 / m_currentTorchRadius;
			float att2 = (1.0f - ratio2) / 0.85f;
			att2 = std::max(0.0f, std::min(1.0f, att2));
			att2 = att2 * att2 * att2; // cubic smooth curves
			float cA_nxt = (100.0f / 255.0f) * att2 * alphaScale;

			thengine::Vertex vCenter = { playerPos.x, playerPos.y, 0.0f, 0.0f, cR, cG, cB, cA_center };
			thengine::Vertex vCurr = { vCurrent.x, vCurrent.y, 0.0f, 0.0f, cR, cG, cB, cA_curr };
			thengine::Vertex vNxt = { vNext.x, vNext.y, 0.0f, 0.0f, cR, cG, cB, cA_nxt };

			lightVertices.push_back(vCenter);
			lightVertices.push_back(vCurr);
			lightVertices.push_back(vNxt);
		}

		getRenderer().drawBatched(m_pixelTex, lightVertices.data(), lightVertices.size(), m_basicEffect, cameraTransform);

		// 2. Build dark ambient shadow triangles (Triangle list between polygon and viewport boundaries)
		float boxSize = 8000.0f;
		float xMin = playerPos.x - boxSize;
		float xMax = playerPos.x + boxSize;
		float yMin = playerPos.y - boxSize;
		float yMax = playerPos.y + boxSize;

		auto projectToBoundary = [](const thengine::Vector2& P, const thengine::Vector2& V, float xMin, float xMax, float yMin, float yMax) {
			thengine::Vector2 D = V - P;
			float tX = (D.x > 0.0f) ? (xMax - P.x) / D.x : (D.x < 0.0f) ? (xMin - P.x) / D.x : 1e10f;
			float tY = (D.y > 0.0f) ? (yMax - P.y) / D.y : (D.y < 0.0f) ? (yMin - P.y) / D.y : 1e10f;
			float t = std::min(tX, tY);
			return P + D * t;
		};

		std::vector<thengine::Vector2> boundaries;
		boundaries.reserve(count);
		for (size_t i = 0; i < count; ++i) {
			boundaries.push_back(projectToBoundary(playerPos, vertices[i], xMin, xMax, yMin, yMax));
		}

		std::vector<thengine::Vertex> shadowVertices;
		shadowVertices.reserve(count * 6);

		// Midnight blue/black ambient darkness overlay
		float sR = 5.0f / 255.0f;
		float sG = 5.0f / 255.0f;
		float sB = 10.0f / 255.0f;
		float sA = 225.0f / 255.0f; // highly opaque shadow

		for (size_t i = 0; i < count; ++i) {
			const auto& vCurrent = vertices[i];
			const auto& vNext = vertices[(i + 1) % count];

			const auto& bCurrent = boundaries[i];
			const auto& bNext = boundaries[(i + 1) % count];

			// Triangle 1: (vCurrent, bCurrent, bNext)
			thengine::Vertex vt1_1 = { vCurrent.x, vCurrent.y, 0.0f, 0.0f, sR, sG, sB, sA };
			thengine::Vertex vt1_2 = { bCurrent.x, bCurrent.y, 0.0f, 0.0f, sR, sG, sB, sA };
			thengine::Vertex vt1_3 = { bNext.x,    bNext.y,    0.0f, 0.0f, sR, sG, sB, sA };

			// Triangle 2: (vCurrent, bNext, vNext)
			thengine::Vertex vt2_1 = { vCurrent.x, vCurrent.y, 0.0f, 0.0f, sR, sG, sB, sA };
			thengine::Vertex vt2_2 = { bNext.x,    bNext.y,    0.0f, 0.0f, sR, sG, sB, sA };
			thengine::Vertex vt2_3 = { vNext.x,    vNext.y,    0.0f, 0.0f, sR, sG, sB, sA };

			shadowVertices.push_back(vt1_1);
			shadowVertices.push_back(vt1_2);
			shadowVertices.push_back(vt1_3);

			shadowVertices.push_back(vt2_1);
			shadowVertices.push_back(vt2_2);
			shadowVertices.push_back(vt2_3);
		}

		getRenderer().drawBatched(m_pixelTex, shadowVertices.data(), shadowVertices.size(), m_basicEffect, cameraTransform);
	}

	// Render dynamic entities on top of the fully lit/darkened world (Layer 3: Entities & Foreground)
	m_spriteBatch->begin(m_basicEffect, cameraTransform);
	m_entityRenderer.render(
		*m_spriteBatch,
		m_entities,
		emberborn::TILE_SIZE
	);
	m_spriteBatch->end();

	// Draw HUD overlay in screen space
	if (m_font) {
		m_spriteBatch->begin(m_basicEffect, thengine::Matrix4::identity());

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

void EmberbornGame::drawLine(
	thengine::SpriteBatch& spriteBatch,
	const thengine::Vector2& start,
	const thengine::Vector2& end,
	const thengine::Color& color
) {
	if (!m_pixelTex) return;

	thengine::Vector2 diff = end - start;
	float length = diff.length();
	if (length < 0.001f) return;

	float rotation = std::atan2(diff.y, diff.x);
	thengine::Vector2 scale(length, 1.5f); // 1.5 pixels thickness

	spriteBatch.draw(
		m_pixelTex,
		start,
		scale,
		rotation,
		{0.0f, 0.5f}, // Left-center origin
		color,
		0.3f // Draw depth slightly above entities
	);
}
