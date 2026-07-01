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
	m_multiplyEffect = std::make_shared<::thengine::SpriteEffect>(getRenderer(), vert, frag, thengine::BlendMode::Multiplicative);
	m_lightmapTexture = thengine::Texture::createRenderTarget(getRenderer(), emberborn::WINDOW_WIDTH, emberborn::WINDOW_HEIGHT);

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

	// Spawn a few static torches across the dungeon
	int torchCount = 0;
	for (int y = 5; y < m_tileMap.getHeight() - 5; y += 15) {
		for (int x = 5; x < m_tileMap.getWidth() - 5; x += 15) {
			if (m_tileMap.getTile(x, y).type == emberborn::Tile::TileType::Floor) {
				// Don't place too close to player spawn
				if (std::abs(x - startX) + std::abs(y - startY) > 5) {
					thengine::Vector2 pos(
						static_cast<float>(x) * emberborn::TILE_SIZE + emberborn::TILE_SIZE * 0.5f,
						static_cast<float>(y) * emberborn::TILE_SIZE + emberborn::TILE_SIZE * 0.5f
					);
					
					thengine::Color col;
					if (torchCount % 4 == 0) col = thengine::Color(255, 150, 50, 255);      // Amber
					else if (torchCount % 4 == 1) col = thengine::Color(50, 150, 255, 255); // Blue
					else if (torchCount % 4 == 2) col = thengine::Color(255, 50, 50, 255);   // Red
					else col = thengine::Color(50, 255, 50, 255);                           // Green
					
					thengine::LightSource light(pos, 350.0f, col, 1.0f, true);
					light.cachedPolygon = thengine::VisibilitySolver::calculateVisibility(pos, 350.0f, m_tileMap);
					light.hasCachedPolygon = true;
					m_lights.push_back(light);
					torchCount++;
					if (torchCount >= 10) break; // Place up to 10 torches
				}
			}
		}
		if (torchCount >= 10) break;
	}
	LOG_INFO() << "Placed " << torchCount << " static dungeon torches.";
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

	// Calculate visibility polygons for all active visible light sources (with CPU-side distance culling)
	m_visibleLights.clear();
	if (m_player) {
		thengine::Vector2 playerPos(
			static_cast<float>(m_player->getX()) * emberborn::TILE_SIZE + emberborn::TILE_SIZE * 0.5f,
			static_cast<float>(m_player->getY()) * emberborn::TILE_SIZE + emberborn::TILE_SIZE * 0.5f
		);

		// Resolve Fog of War for visible tiles in player's light radius
		int radiusTiles = static_cast<int>(std::ceil(m_currentTorchRadius / emberborn::TILE_SIZE));
		int px = m_player->getX();
		int py = m_player->getY();

		int minX_fog = std::max(0, px - radiusTiles);
		int maxX_fog = std::min(m_tileMap.getWidth() - 1, px + radiusTiles);
		int minY_fog = std::max(0, py - radiusTiles);
		int maxY_fog = std::min(m_tileMap.getHeight() - 1, py + radiusTiles);

		for (int ty = minY_fog; ty <= maxY_fog; ++ty) {
			for (int tx = minX_fog; tx <= maxX_fog; ++tx) {
				thengine::Vector2 tilePos(
					static_cast<float>(tx) * emberborn::TILE_SIZE + emberborn::TILE_SIZE * 0.5f,
					static_cast<float>(ty) * emberborn::TILE_SIZE + emberborn::TILE_SIZE * 0.5f
				);
				float d = (tilePos - playerPos).length();
				if (d <= m_currentTorchRadius) {
					if (hasLineOfSight(px, py, tx, ty)) {
						m_tileMap.setExplored(tx, ty, true);
					}
				}
			}
		}

		// Get camera properties for viewport culling
		thengine::Vector2 camCenter = m_camera.getPosition();
		float camWidth = static_cast<float>(emberborn::WINDOW_WIDTH) / m_camera.getZoom();
		float camHeight = static_cast<float>(emberborn::WINDOW_HEIGHT) / m_camera.getZoom();

		// Add player torch as dynamic light source
		std::vector<thengine::LightSource> activeLights;
		activeLights.push_back(thengine::LightSource(playerPos, m_currentTorchRadius, thengine::Color(255, 250, 240, 255), 1.0f));

		// Include static torches
		activeLights.insert(activeLights.end(), m_lights.begin(), m_lights.end());

		// Calculate visibility polygons only for non-culled lights (using cache for static torches)
		static int frameCount = 0;
		frameCount++;
		int solverCalls = 0;

		int playerGridX = m_player->getX();
		int playerGridY = m_player->getY();

		for (const auto& light : activeLights) {
			if (isLightInViewport(light, camCenter, camWidth, camHeight)) {
				// Cull static lights that do not have line-of-sight to the player
				if (light.isStatic) {
					int lx = static_cast<int>(light.position.x / emberborn::TILE_SIZE);
					int ly = static_cast<int>(light.position.y / emberborn::TILE_SIZE);
					if (!hasLineOfSight(playerGridX, playerGridY, lx, ly)) {
						continue; // Hide this light entirely
					}
				}

				if (light.isStatic && light.hasCachedPolygon) {
					m_visibleLights.push_back({ light, light.cachedPolygon });
				} else {
					thengine::VisibilityPolygon poly = thengine::VisibilitySolver::calculateVisibility(light.position, light.currentRadius, m_tileMap);
					m_visibleLights.push_back({ light, std::move(poly) });
					solverCalls++;
				}
			}
		}

		if (frameCount % 60 == 0) {
			LOG_INFO() << "Active lights total: " << activeLights.size() 
			           << ", Solver calls: " << solverCalls 
			           << ", Visible lights: " << m_visibleLights.size();
		}
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
	// Step 1: Draw Lightmap Target
	if (m_lightmapTexture) {
		getRenderer().setRenderTarget(m_lightmapTexture);
		// Clear lightmap to absolute darkness (Fog of War base)
		getRenderer().clear(0, 0, 0, 255);

		// Calculate camera view transformation matrix
		thengine::Matrix4 cameraTransform = m_camera.getTransform(
			static_cast<float>(emberborn::WINDOW_WIDTH),
			static_cast<float>(emberborn::WINDOW_HEIGHT)
		);

		// Draw explored tiles as dim ambient memory shapes in the lightmap
		float camZoom = m_camera.getZoom();
		thengine::Vector2 camPos = m_camera.getPosition();
		float halfW = (emberborn::WINDOW_WIDTH * 0.5f) / camZoom;
		float halfH = (emberborn::WINDOW_HEIGHT * 0.5f) / camZoom;

		int minX = static_cast<int>(std::floor((camPos.x - halfW) / emberborn::TILE_SIZE));
		int maxX = static_cast<int>(std::floor((camPos.x + halfW) / emberborn::TILE_SIZE));
		int minY = static_cast<int>(std::floor((camPos.y - halfH) / emberborn::TILE_SIZE));
		int maxY = static_cast<int>(std::floor((camPos.y + halfH) / emberborn::TILE_SIZE));

		minX = std::max(0, minX);
		maxX = std::min(m_tileMap.getWidth() - 1, maxX);
		minY = std::max(0, minY);
		maxY = std::min(m_tileMap.getHeight() - 1, maxY);

		m_spriteBatch->begin(m_basicEffect, cameraTransform);
		for (int y = minY; y <= maxY; ++y) {
			for (int x = minX; x <= maxX; ++x) {
				const auto& tile = m_tileMap.getTile(x, y);
				if (tile.isExplored) {
					thengine::Vector2 pos(
						static_cast<float>(x) * emberborn::TILE_SIZE,
						static_cast<float>(y) * emberborn::TILE_SIZE
					);
					thengine::Vector2 size(
						static_cast<float>(emberborn::TILE_SIZE),
						static_cast<float>(emberborn::TILE_SIZE)
					);
					m_spriteBatch->draw(
						m_pixelTex,
						pos,
						size,
						0.0f,
						{0.0f, 0.0f},
						thengine::Color(25, 25, 30, 255),
						0.1f
					);
				}
			}
		}
		m_spriteBatch->end();

		// Draw visible dynamic/static light sources directly into the lightmap target texture
		for (size_t idx = 0; idx < m_visibleLights.size(); ++idx) {
			const auto& vl = m_visibleLights[idx];
			const auto& light = vl.light;
			const auto& vertices = vl.polygon.vertices;
			size_t count = vertices.size();
			if (count == 0) continue;

			std::vector<thengine::Vertex> lightVertices;
			lightVertices.reserve(count * 3);

			// Apply player torch flicker if index is 0
			float flicker = 1.0f;
			if (idx == 0) {
				float ratio = m_currentTorchRadius / 400.0f;
				flicker = ratio * ratio;
				if (flicker > 1.5f) {
					flicker = 1.5f;
				}
			}

			float cR = static_cast<float>(light.color.r) / 255.0f;
			float cG = static_cast<float>(light.color.g) / 255.0f;
			float cB = static_cast<float>(light.color.b) / 255.0f;
			
			// Center has attenuation = 1.0f
			float cR_center = cR * flicker * light.intensity;
			float cG_center = cG * flicker * light.intensity;
			float cB_center = cB * flicker * light.intensity;
			float cA_center = 1.0f * flicker * light.intensity;

			const auto& attenuations = vl.polygon.attenuations;

			for (size_t i = 0; i < count; ++i) {
				const auto& vCurrent = vertices[i];
				const auto& vNext = vertices[(i + 1) % count];

				// Read pre-calculated attenuation values
				float att1 = 0.0f;
				if (i < attenuations.size()) {
					att1 = attenuations[i];
				}
				float att2 = 0.0f;
				if (((i + 1) % count) < attenuations.size()) {
					att2 = attenuations[(i + 1) % count];
				}

				float cR_curr = cR * att1 * flicker * light.intensity;
				float cG_curr = cG * att1 * flicker * light.intensity;
				float cB_curr = cB * att1 * flicker * light.intensity;
				float cA_curr = att1 * flicker * light.intensity;

				float cR_nxt = cR * att2 * flicker * light.intensity;
				float cG_nxt = cG * att2 * flicker * light.intensity;
				float cB_nxt = cB * att2 * flicker * light.intensity;
				float cA_nxt = att2 * flicker * light.intensity;

				thengine::Vertex vCenter = { light.position.x, light.position.y, 0.0f, 0.0f, cR_center, cG_center, cB_center, cA_center };
				thengine::Vertex vCurr = { vCurrent.x, vCurrent.y, 0.0f, 0.0f, cR_curr, cG_curr, cB_curr, cA_curr };
				thengine::Vertex vNxt = { vNext.x, vNext.y, 0.0f, 0.0f, cR_nxt, cG_nxt, cB_nxt, cA_nxt };

				lightVertices.push_back(vCenter);
				lightVertices.push_back(vCurr);
				lightVertices.push_back(vNxt);
			}

			getRenderer().drawBatched(m_pixelTex, lightVertices.data(), lightVertices.size(), m_basicEffect, cameraTransform);
		}
	}

	// Step 2: Draw Environment & Compose
	getRenderer().setRenderTarget(nullptr);
	// Clear the main screen target
	getRenderer().clear(5, 5, 8, 255);

	// Calculate camera view transformation matrix
	thengine::Matrix4 cameraTransform = m_camera.getTransform(
		static_cast<float>(emberborn::WINDOW_WIDTH),
		static_cast<float>(emberborn::WINDOW_HEIGHT)
	);

	// 2a. Draw raw TileMap (Layer 1) at full brightness (using white ambient color)
	m_basicEffect->setAmbientColor(thengine::Color(255, 255, 255, 255));
	m_basicEffect->setAmbientIntensity(1.0f);
	m_basicEffect->clearLights();

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

	// 2b. Draw our compiled Lightmap texture over the screen with Multiplicative Blending (Layer 2)
	if (m_lightmapTexture && m_multiplyEffect) {
		m_spriteBatch->begin(m_multiplyEffect, thengine::Matrix4::identity());
		m_spriteBatch->draw(
			m_lightmapTexture,
			{0.0f, 0.0f},
			{1.0f, 1.0f},
			0.0f,
			{0.0f, 0.0f},
			thengine::Color(255, 255, 255, 255),
			0.2f
		);
		m_spriteBatch->end();
	}

	// Step 3: Draw Entities (Layer 3) on top of the composed scene
	std::vector<thengine::Color> entityTints;
	entityTints.reserve(m_entities.size());

	for (const auto& entity : m_entities) {
		if (!entity) {
			entityTints.push_back(thengine::Color(35, 30, 35, 255));
			continue;
		}

		thengine::Vector2 entityPos(
			static_cast<float>(entity->getX()) * emberborn::TILE_SIZE + emberborn::TILE_SIZE * 0.5f,
			static_cast<float>(entity->getY()) * emberborn::TILE_SIZE + emberborn::TILE_SIZE * 0.5f
		);

		float accumR = 35.0f;
		float accumG = 30.0f;
		float accumB = 35.0f;

		for (size_t idx = 0; idx < m_visibleLights.size(); ++idx) {
			const auto& vl = m_visibleLights[idx];
			const auto& light = vl.light;

			float d = (entityPos - light.position).length();
			if (d < light.currentRadius) {
				int lx = static_cast<int>(light.position.x / emberborn::TILE_SIZE);
				int ly = static_cast<int>(light.position.y / emberborn::TILE_SIZE);
				int ex = entity->getX();
				int ey = entity->getY();

				if (hasLineOfSight(lx, ly, ex, ey)) {
					float ratio = d / light.currentRadius;
					if (ratio > 1.0f) ratio = 1.0f;
					float att = 1.0f - ratio;
					att = att * att;

					float flicker = 1.0f;
					if (idx == 0) {
						float r = m_currentTorchRadius / 400.0f;
						flicker = r * r;
						if (flicker > 1.5f) flicker = 1.5f;
					}

					accumR += (static_cast<float>(light.color.r) - 35.0f) * att * flicker * light.intensity;
					accumG += (static_cast<float>(light.color.g) - 30.0f) * att * flicker * light.intensity;
					accumB += (static_cast<float>(light.color.b) - 35.0f) * att * flicker * light.intensity;
				}
			}
		}

		uint8_t r = static_cast<uint8_t>(std::clamp(accumR, 0.0f, 255.0f));
		uint8_t g = static_cast<uint8_t>(std::clamp(accumG, 0.0f, 255.0f));
		uint8_t b = static_cast<uint8_t>(std::clamp(accumB, 0.0f, 255.0f));
		entityTints.push_back(thengine::Color(r, g, b, 255));
	}

	m_basicEffect->setAmbientColor(thengine::Color(255, 255, 255, 255));
	m_basicEffect->setAmbientIntensity(1.0f);
	m_basicEffect->clearLights();

	m_spriteBatch->begin(m_basicEffect, cameraTransform);
	m_entityRenderer.render(
		*m_spriteBatch,
		m_entities,
		entityTints,
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

bool EmberbornGame::isLightInViewport(
	const thengine::LightSource& light,
	const thengine::Vector2& camCenter,
	float camWidth,
	float camHeight
) const {
	if (!light.active) return false;

	// Calculate camera viewport AABB (with extra safety margin for smooth entry/exit)
	float margin = 64.0f;
	float minX = camCenter.x - camWidth * 0.5f - margin;
	float maxX = camCenter.x + camWidth * 0.5f + margin;
	float minY = camCenter.y - camHeight * 0.5f - margin;
	float maxY = camCenter.y + camHeight * 0.5f + margin;

	// Find closest point on AABB to the light
	float closestX = std::max(minX, std::min(light.position.x, maxX));
	float closestY = std::max(minY, std::min(light.position.y, maxY));

	// Distance check using squared length
	float dx = light.position.x - closestX;
	float dy = light.position.y - closestY;
	float distSq = dx * dx + dy * dy;

	return distSq < (light.currentRadius * light.currentRadius);
}

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

bool EmberbornGame::hasLineOfSight(int x1, int y1, int x2, int y2) const {
	int dx = std::abs(x2 - x1);
	int dy = std::abs(y2 - y1);
	int sx = (x1 < x2) ? 1 : -1;
	int sy = (y1 < y2) ? 1 : -1;
	int err = dx - dy;

	int x = x1;
	int y = y1;
	while (true) {
		if (x == x2 && y == y2) return true;
		if ((x != x1 || y != y1) && (x != x2 || y != y2)) {
			if (m_tileMap.getTile(x, y).type == emberborn::Tile::TileType::Wall ||
				m_tileMap.getTile(x, y).type == emberborn::Tile::TileType::Void) {
				return false;
			}
		}
		int e2 = 2 * err;
		if (e2 > -dy) {
			err -= dy;
			x += sx;
		}
		if (e2 < dx) {
			err += dx;
			y += sy;
		}
	}
}
