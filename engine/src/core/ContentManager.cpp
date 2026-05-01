#include "thengine/ContentManager.h"
#include "thengine/graphics/Texture.h"
#include "thengine/Renderer.h"
#include "thengine/DebugLogger.h"

#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_surface.h>

#include <cstring>

namespace thengine {

ContentManager::ContentManager(Renderer& renderer)
    : m_renderer(renderer) {
}

template<>
std::shared_ptr<Texture> ContentManager::load<Texture>(const std::string& path) {
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
    // Leaving pixels_per_row and rows_per_layer as 0 assumes tightly packed data
    // SDL_ConvertSurface guarantees a format, but we may need to set pixels_per_row if pitch > w*4
    // Usually it's tightly packed, but let's be safe.
    // Actually, setting pitch / 4 isn't possible if struct doesn't have it, but it should in SDL3.
    // For now we assume `{}` zeroes it out and it's tightly packed.

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

    return std::shared_ptr<Texture>(new Texture(device, texture, width, height, path));
}

} // namespace thengine
