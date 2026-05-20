#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <any>

namespace thengine {

class Renderer;
class Texture;
class Shader;

class ContentManager {
public:
    explicit ContentManager(Renderer& renderer);
    ~ContentManager() = default;

    template<typename T>
    std::shared_ptr<T> load(const std::string& path);

private:
    Renderer& m_renderer;
    std::unordered_map<std::string, std::any> m_cache;
};

template<>
std::shared_ptr<Texture> ContentManager::load<Texture>(const std::string& path);

template<>
std::shared_ptr<Shader> ContentManager::load<Shader>(const std::string& path);

} // namespace thengine
