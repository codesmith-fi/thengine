#pragma once

namespace thengine {

class FpsCounter {
public:
    explicit FpsCounter(float updateIntervalSeconds = 1.0f) noexcept;
    ~FpsCounter() = default;

    // Prevent copying
    FpsCounter(const FpsCounter&) = delete;
    FpsCounter& operator=(const FpsCounter&) = delete;

    void update(float deltaTime) noexcept;

    [[nodiscard]] float getFps() const noexcept { return m_fps; }
    [[nodiscard]] float getUpdateInterval() const noexcept { return m_updateInterval; }
    void setUpdateInterval(float interval) noexcept { m_updateInterval = interval; }

private:
    float m_updateInterval;
    float m_fps;
    float m_timeAccumulator;
    int m_frameCount;
};

} // namespace thengine
