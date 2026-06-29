#include "thengine/FpsCounter.h"
#include <algorithm>

namespace thengine {

FpsCounter::FpsCounter(float updateIntervalSeconds) noexcept
    : m_updateInterval(updateIntervalSeconds), m_fps(0.0f), m_timeAccumulator(0.0f), m_frameCount(0) {
}

void FpsCounter::update(float deltaTime) noexcept {
    m_timeAccumulator += deltaTime;
    m_frameCount++;

    if (m_timeAccumulator >= m_updateInterval) {
        if (m_timeAccumulator > 0.0f) {
            m_fps = static_cast<float>(m_frameCount) / m_timeAccumulator;
        } else {
            m_fps = 0.0f;
        }
        m_frameCount = 0;
        m_timeAccumulator = std::max(0.0f, m_timeAccumulator - m_updateInterval);
    }
}

} // namespace thengine
