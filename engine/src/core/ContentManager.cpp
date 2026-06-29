#include "thengine/ContentManager.h"
#include "thengine/graphics/Texture.h"
#include "thengine/graphics/Shader.h"
#include "thengine/Renderer.h"
#include "thengine/DebugLogger.h"
#include "thengine/graphics/SpriteFont.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "graphics/stb_truetype.h"

#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_surface.h>

#include <cstring>
#include <fstream>
#include <vector>

namespace thengine {

ContentManager::ContentManager(Renderer& renderer)
    : m_renderer(renderer) {
}

template<>
std::shared_ptr<Texture> ContentManager::load<Texture>(const std::string& path) {
    auto it = m_cache.find(path);
    if (it != m_cache.end()) {
        try {
            return std::any_cast<std::shared_ptr<Texture>>(it->second);
        } catch (const std::bad_any_cast& e) {
            LOG_ERROR() << "Failed to cast cached resource for: " << path << " to Texture";
        }
    }

    SDL_GPUDevice* device = m_renderer.getDevice();
    if (!device) {
        LOG_ERROR() << "GPU Device is null in Renderer.";
        return nullptr;
    }

    SDL_Surface* originalSurface = IMG_Load(path.c_str());
    if (!originalSurface) {
        LOG_ERROR() << "Failed to load image: " << path << ", error: " << SDL_GetError();
        return nullptr;
    }

    // Convert surface to RGBA32 which maps to R8G8B8A8 byte order in memory
    SDL_Surface* surface = SDL_ConvertSurface(originalSurface, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(originalSurface);
    if (!surface) {
        LOG_ERROR() << "Failed to convert surface to RGBA32: " << SDL_GetError();
        return nullptr;
    }

    SDL_GPUTextureCreateInfo textureInfo = {};
    textureInfo.type = SDL_GPU_TEXTURETYPE_2D;
    textureInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    textureInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
    textureInfo.width = surface->w;
    textureInfo.height = surface->h;
    textureInfo.layer_count_or_depth = 1;
    textureInfo.num_levels = 1;

    SDL_GPUTexture* texture = SDL_CreateGPUTexture(device, &textureInfo);
    if (!texture) {
        LOG_ERROR() << "Failed to create GPU Texture: " << SDL_GetError();
        SDL_DestroySurface(surface);
        return nullptr;
    }

    SDL_GPUTransferBufferCreateInfo transferBufferInfo = {};
    transferBufferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferBufferInfo.size = surface->pitch * surface->h;

    SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferBufferInfo);
    if (!transferBuffer) {
        LOG_ERROR() << "Failed to create GPU Transfer Buffer: " << SDL_GetError();
        SDL_ReleaseGPUTexture(device, texture);
        SDL_DestroySurface(surface);
        return nullptr;
    }

    void* map = SDL_MapGPUTransferBuffer(device, transferBuffer, false);
    if (map) {
        std::memcpy(map, surface->pixels, surface->pitch * surface->h);
        SDL_UnmapGPUTransferBuffer(device, transferBuffer);
    } else {
        LOG_ERROR() << "Failed to map GPU Transfer Buffer: " << SDL_GetError();
        SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
        SDL_ReleaseGPUTexture(device, texture);
        SDL_DestroySurface(surface);
        return nullptr;
    }

    SDL_GPUCommandBuffer* cmdBuf = SDL_AcquireGPUCommandBuffer(device);
    if (!cmdBuf) {
        LOG_ERROR() << "Failed to acquire command buffer: " << SDL_GetError();
        SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
        SDL_ReleaseGPUTexture(device, texture);
        SDL_DestroySurface(surface);
        return nullptr;
    }

    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmdBuf);
    
    SDL_GPUTextureTransferInfo transferInfo = {};
    transferInfo.transfer_buffer = transferBuffer;
    transferInfo.offset = 0; // byte offset

    SDL_GPUTextureRegion region = {};
    region.texture = texture;
    region.w = surface->w;
    region.h = surface->h;
    region.d = 1;

    SDL_UploadToGPUTexture(copyPass, &transferInfo, &region, false);
    
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(cmdBuf);

    int width = surface->w;
    int height = surface->h;

    SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
    SDL_DestroySurface(surface);

    auto texturePtr = std::shared_ptr<Texture>(new Texture(device, texture, width, height, path));
    m_cache[path] = texturePtr;
    return texturePtr;
}

template<>
std::shared_ptr<Shader> ContentManager::load<Shader>(const std::string& path) {
    SDL_GPUDevice* device = m_renderer.getDevice();
    if (!device) {
        LOG_ERROR() << "GPU Device is null in Renderer.";
        return nullptr;
    }

    SDL_GPUShaderFormat supportedFormats = SDL_GetGPUShaderFormats(device);
    std::string resolvedPath = path;
    SDL_GPUShaderFormat selectedFormat = SDL_GPU_SHADERFORMAT_INVALID;

    // Check if the path ends with a known shader extension
    bool hasExtension = false;
    if (path.size() >= 4) {
        std::string ext = path.substr(path.size() - 4);
        if (ext == ".spv" || ext == ".msl") {
            hasExtension = true;
            selectedFormat = (ext == ".spv") ? SDL_GPU_SHADERFORMAT_SPIRV : SDL_GPU_SHADERFORMAT_MSL;
        }
    }
    if (path.size() >= 5 && !hasExtension) {
        std::string ext = path.substr(path.size() - 5);
        if (ext == ".dxil") {
            hasExtension = true;
            selectedFormat = SDL_GPU_SHADERFORMAT_DXIL;
        }
    }

    if (!hasExtension) {
        // Resolve extension automatically
        if (supportedFormats & SDL_GPU_SHADERFORMAT_SPIRV) {
            resolvedPath += ".spv";
            selectedFormat = SDL_GPU_SHADERFORMAT_SPIRV;
        } else if (supportedFormats & SDL_GPU_SHADERFORMAT_DXIL) {
            resolvedPath += ".dxil";
            selectedFormat = SDL_GPU_SHADERFORMAT_DXIL;
        } else if (supportedFormats & SDL_GPU_SHADERFORMAT_MSL) {
            resolvedPath += ".msl";
            selectedFormat = SDL_GPU_SHADERFORMAT_MSL;
        } else {
            LOG_ERROR() << "No supported shader format for active GPU device.";
            return nullptr;
        }
    }

    // Check cache using the resolved path
    auto it = m_cache.find(resolvedPath);
    if (it != m_cache.end()) {
        try {
            return std::any_cast<std::shared_ptr<Shader>>(it->second);
        } catch (const std::bad_any_cast& e) {
            LOG_ERROR() << "Failed to cast cached resource for: " << resolvedPath << " to Shader";
        }
    }

    // Load file contents
    std::ifstream file(resolvedPath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        LOG_ERROR() << "Failed to open shader file: " << resolvedPath;
        return nullptr;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        LOG_ERROR() << "Failed to read shader file: " << resolvedPath;
        return nullptr;
    }

    SDL_GPUShaderStage stage;
    uint32_t numSamplers = 0;
    uint32_t numUniformBuffers = 0;

    // Use resolved path for stage inference (since extension might have been added)
    if (resolvedPath.find(".vert") != std::string::npos || resolvedPath.find(".vertex") != std::string::npos) {
        stage = SDL_GPU_SHADERSTAGE_VERTEX;
        numUniformBuffers = 1;
    } else if (resolvedPath.find(".frag") != std::string::npos || resolvedPath.find(".fragment") != std::string::npos) {
        stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
        numSamplers = 1;
    } else {
        LOG_ERROR() << "Could not infer shader stage from path: " << resolvedPath << ". Expected '.vert' or '.frag' in filename.";
        return nullptr;
    }

    SDL_GPUShaderCreateInfo shaderInfo = {};
    shaderInfo.code = buffer.data();
    shaderInfo.code_size = buffer.size();
    shaderInfo.entrypoint = "main";
    shaderInfo.format = selectedFormat;
    shaderInfo.stage = stage;
    shaderInfo.num_samplers = numSamplers;
    shaderInfo.num_storage_textures = 0;
    shaderInfo.num_storage_buffers = 0;
    shaderInfo.num_uniform_buffers = numUniformBuffers;

    SDL_GPUShader* gpuShader = SDL_CreateGPUShader(device, &shaderInfo);
    if (!gpuShader) {
        LOG_ERROR() << "Failed to create GPU Shader from: " << resolvedPath << ", error: " << SDL_GetError();
        return nullptr;
    }

    auto shaderPtr = std::shared_ptr<Shader>(new Shader(device, gpuShader));
    m_cache[resolvedPath] = shaderPtr;
    return shaderPtr;
}

std::shared_ptr<SpriteFont> ContentManager::loadFont(const std::string& path, float fontSize, int atlasWidth, int atlasHeight) {
    std::string cacheKey = path + ":" + std::to_string(fontSize);
    auto it = m_cache.find(cacheKey);
    if (it != m_cache.end()) {
        try {
            return std::any_cast<std::shared_ptr<SpriteFont>>(it->second);
        } catch (const std::bad_any_cast& e) {
            LOG_ERROR() << "Failed to cast cached resource for: " << cacheKey << " to SpriteFont";
        }
    }

    SDL_GPUDevice* device = m_renderer.getDevice();
    if (!device) {
        LOG_ERROR() << "GPU Device is null in Renderer.";
        return nullptr;
    }

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        LOG_ERROR() << "Failed to open font file: " << path;
        return nullptr;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> ttfBuffer(size);
    if (!file.read(reinterpret_cast<char*>(ttfBuffer.data()), size)) {
        LOG_ERROR() << "Failed to read font file: " << path;
        return nullptr;
    }
    file.close();

    std::vector<uint8_t> atlasPixels(atlasWidth * atlasHeight, 0);

    stbtt_pack_context packContext;
    stbtt_packedchar packedChars[96];

    if (!stbtt_PackBegin(&packContext, atlasPixels.data(), atlasWidth, atlasHeight, 0, 1, nullptr)) {
        LOG_ERROR() << "Failed to initialize stb_truetype packing context";
        return nullptr;
    }

    stbtt_pack_range range;
    range.font_size = fontSize;
    range.first_unicode_codepoint_in_range = 32;
    range.array_of_unicode_codepoints = nullptr;
    range.num_chars = 96;
    range.chardata_for_range = packedChars;
    range.h_oversample = 1;
    range.v_oversample = 1;

    if (!stbtt_PackFontRanges(&packContext, ttfBuffer.data(), 0, &range, 1)) {
        LOG_ERROR() << "Failed to pack font characters";
        stbtt_PackEnd(&packContext);
        return nullptr;
    }

    stbtt_PackEnd(&packContext);

    std::vector<uint8_t> rgbaPixels(atlasWidth * atlasHeight * 4, 0);
    for (int i = 0; i < atlasWidth * atlasHeight; ++i) {
        rgbaPixels[i * 4 + 0] = 255;
        rgbaPixels[i * 4 + 1] = 255;
        rgbaPixels[i * 4 + 2] = 255;
        rgbaPixels[i * 4 + 3] = atlasPixels[i];
    }

    SDL_GPUTextureCreateInfo textureInfo = {};
    textureInfo.type = SDL_GPU_TEXTURETYPE_2D;
    textureInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    textureInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
    textureInfo.width = atlasWidth;
    textureInfo.height = atlasHeight;
    textureInfo.layer_count_or_depth = 1;
    textureInfo.num_levels = 1;

    SDL_GPUTexture* gpuTexture = SDL_CreateGPUTexture(device, &textureInfo);
    if (!gpuTexture) {
        LOG_ERROR() << "Failed to create GPU Texture for font: " << SDL_GetError();
        return nullptr;
    }

    SDL_GPUTransferBufferCreateInfo transferBufferInfo = {};
    transferBufferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferBufferInfo.size = atlasWidth * atlasHeight * 4;

    SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferBufferInfo);
    if (!transferBuffer) {
        LOG_ERROR() << "Failed to create GPU Transfer Buffer for font: " << SDL_GetError();
        SDL_ReleaseGPUTexture(device, gpuTexture);
        return nullptr;
    }

    void* map = SDL_MapGPUTransferBuffer(device, transferBuffer, false);
    if (map) {
        std::memcpy(map, rgbaPixels.data(), rgbaPixels.size());
        SDL_UnmapGPUTransferBuffer(device, transferBuffer);
    } else {
        LOG_ERROR() << "Failed to map GPU Transfer Buffer for font: " << SDL_GetError();
        SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
        SDL_ReleaseGPUTexture(device, gpuTexture);
        return nullptr;
    }

    SDL_GPUCommandBuffer* cmdBuf = SDL_AcquireGPUCommandBuffer(device);
    if (!cmdBuf) {
        LOG_ERROR() << "Failed to acquire command buffer for font upload: " << SDL_GetError();
        SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
        SDL_ReleaseGPUTexture(device, gpuTexture);
        return nullptr;
    }

    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmdBuf);
    
    SDL_GPUTextureTransferInfo transferInfo = {};
    transferInfo.transfer_buffer = transferBuffer;
    transferInfo.offset = 0;

    SDL_GPUTextureRegion region = {};
    region.texture = gpuTexture;
    region.w = atlasWidth;
    region.h = atlasHeight;
    region.d = 1;

    SDL_UploadToGPUTexture(copyPass, &transferInfo, &region, false);
    
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(cmdBuf);
    SDL_ReleaseGPUTransferBuffer(device, transferBuffer);

    auto texture = std::shared_ptr<Texture>(new Texture(device, gpuTexture, atlasWidth, atlasHeight, path + "_atlas"));

    std::unordered_map<char, GlyphInfo> glyphs;
    for (int i = 0; i < 96; ++i) {
        char c = static_cast<char>(32 + i);
        const auto& pc = packedChars[i];

        GlyphInfo glyph;
        glyph.sourceRect = Rectangle(
            static_cast<float>(pc.x0),
            static_cast<float>(pc.y0),
            static_cast<float>(pc.x1 - pc.x0),
            static_cast<float>(pc.y1 - pc.y0)
        );
        glyph.offsetX = pc.xoff;
        glyph.offsetY = pc.yoff;
        glyph.advanceX = pc.xadvance;

        glyphs[c] = glyph;
    }

    auto fontPtr = std::make_shared<SpriteFont>(texture, fontSize, glyphs);
    m_cache[cacheKey] = fontPtr;
    return fontPtr;
}

} // namespace thengine
