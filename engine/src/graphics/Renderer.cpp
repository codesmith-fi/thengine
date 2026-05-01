#include "thengine/Renderer.h"
#include "thengine/DebugLogger.h"
#include "thengine/graphics/Texture.h"
#include "BasicShaders.h"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_video.h>
#include <cstring>

namespace thengine {

#include "thengine/graphics/Vertex.h"

struct PushConstants {
    float transform[16];
    float r, g, b, a;
};

Renderer::Renderer(SDL_Window* window)
    : m_window(window), m_device(nullptr) {
    
    m_device = SDL_CreateGPUDevice(
        SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL,
        true, // debug_mode
        nullptr // name
    );

    if (!m_device) {
        LOG_ERROR() << "Failed to create SDL_GPUDevice: " << SDL_GetError();
        return;
    }

    if (!SDL_ClaimWindowForGPUDevice(m_device, m_window)) {
        LOG_ERROR() << "Failed to claim window for GPU Device: " << SDL_GetError();
        SDL_DestroyGPUDevice(m_device);
        m_device = nullptr;
        return;
    }

    initPipeline();

    LOG_INFO() << "SDL_GPUDevice initialized and window claimed successfully!";
}

Renderer::~Renderer() {
    cleanupPipeline();

    if (m_device) {
        SDL_ReleaseWindowFromGPUDevice(m_device, m_window);
        SDL_DestroyGPUDevice(m_device);
    }
}

void Renderer::initPipeline() {
    if (!m_device) return;

    SDL_GPUShaderCreateInfo vertInfo = {};
    vertInfo.code = engine_assets_shaders_basic_vert_spv;
    vertInfo.code_size = engine_assets_shaders_basic_vert_spv_len;
    vertInfo.entrypoint = "main";
    vertInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
    vertInfo.stage = SDL_GPU_SHADERSTAGE_VERTEX;
    vertInfo.num_samplers = 0;
    vertInfo.num_storage_textures = 0;
    vertInfo.num_storage_buffers = 0;
    vertInfo.num_uniform_buffers = 1;

    SDL_GPUShader* vertShader = SDL_CreateGPUShader(m_device, &vertInfo);
    if (!vertShader) {
        LOG_ERROR() << "Failed to create vertex shader: " << SDL_GetError();
    }

    SDL_GPUShaderCreateInfo fragInfo = {};
    fragInfo.code = engine_assets_shaders_basic_frag_spv;
    fragInfo.code_size = engine_assets_shaders_basic_frag_spv_len;
    fragInfo.entrypoint = "main";
    fragInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
    fragInfo.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    fragInfo.num_samplers = 1;
    fragInfo.num_storage_textures = 0;
    fragInfo.num_storage_buffers = 0;
    fragInfo.num_uniform_buffers = 0;

    SDL_GPUShader* fragShader = SDL_CreateGPUShader(m_device, &fragInfo);
    if (!fragShader) {
        LOG_ERROR() << "Failed to create fragment shader: " << SDL_GetError();
    }

    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.vertex_shader = vertShader;
    pipelineInfo.fragment_shader = fragShader;
    
    SDL_GPUVertexBufferDescription vertexBufferDesc = {};
    vertexBufferDesc.slot = 0;
    vertexBufferDesc.pitch = sizeof(Vertex);
    vertexBufferDesc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    vertexBufferDesc.instance_step_rate = 0;

    SDL_GPUVertexAttribute vertexAttributes[3] = {};
    vertexAttributes[0].location = 0;
    vertexAttributes[0].buffer_slot = 0;
    vertexAttributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    vertexAttributes[0].offset = offsetof(Vertex, x);

    vertexAttributes[1].location = 1;
    vertexAttributes[1].buffer_slot = 0;
    vertexAttributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    vertexAttributes[1].offset = offsetof(Vertex, u);

    vertexAttributes[2].location = 2;
    vertexAttributes[2].buffer_slot = 0;
    vertexAttributes[2].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
    vertexAttributes[2].offset = offsetof(Vertex, r);

    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = &vertexBufferDesc;
    pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
    pipelineInfo.vertex_input_state.vertex_attributes = vertexAttributes;
    pipelineInfo.vertex_input_state.num_vertex_attributes = 3;

    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

    pipelineInfo.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
    pipelineInfo.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    pipelineInfo.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;

    pipelineInfo.depth_stencil_state.enable_depth_test = false;
    pipelineInfo.depth_stencil_state.enable_depth_write = false;

    SDL_GPUColorTargetDescription colorTargetDesc = {};
    colorTargetDesc.format = SDL_GetGPUSwapchainTextureFormat(m_device, m_window);
    colorTargetDesc.blend_state.enable_blend = true;
    colorTargetDesc.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTargetDesc.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTargetDesc.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTargetDesc.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    colorTargetDesc.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTargetDesc.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;

    pipelineInfo.target_info.color_target_descriptions = &colorTargetDesc;
    pipelineInfo.target_info.num_color_targets = 1;

    m_spritePipeline = SDL_CreateGPUGraphicsPipeline(m_device, &pipelineInfo);
    if (!m_spritePipeline) {
        LOG_ERROR() << "Failed to create graphics pipeline: " << SDL_GetError();
    }

    SDL_ReleaseGPUShader(m_device, vertShader);
    SDL_ReleaseGPUShader(m_device, fragShader);

    SDL_GPUBufferCreateInfo bufferInfo = {};
    bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    bufferInfo.size = sizeof(Vertex) * 4;
    
    m_vertexBuffer = SDL_CreateGPUBuffer(m_device, &bufferInfo);
    
    Vertex quad[4] = {
        {0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f},
        {0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}
    };
    
    SDL_GPUTransferBufferCreateInfo tbInfo = {};
    tbInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    tbInfo.size = sizeof(quad);
    
    SDL_GPUTransferBuffer* tb = SDL_CreateGPUTransferBuffer(m_device, &tbInfo);
    void* map = SDL_MapGPUTransferBuffer(m_device, tb, false);
    if (map) {
        std::memcpy(map, quad, sizeof(quad));
        SDL_UnmapGPUTransferBuffer(m_device, tb);
        
        SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(m_device);
        SDL_GPUCopyPass* copy = SDL_BeginGPUCopyPass(cmd);
        
        SDL_GPUTransferBufferLocation srcLoc = {};
        srcLoc.transfer_buffer = tb;
        srcLoc.offset = 0;
        
        SDL_GPUBufferRegion dstReg = {};
        dstReg.buffer = m_vertexBuffer;
        dstReg.offset = 0;
        dstReg.size = sizeof(quad);
        
        SDL_UploadToGPUBuffer(copy, &srcLoc, &dstReg, false);
        SDL_EndGPUCopyPass(copy);
        SDL_SubmitGPUCommandBuffer(cmd);
    }
    
    SDL_ReleaseGPUTransferBuffer(m_device, tb);

    SDL_GPUSamplerCreateInfo samplerInfo = {};
    samplerInfo.min_filter = SDL_GPU_FILTER_NEAREST;
    samplerInfo.mag_filter = SDL_GPU_FILTER_NEAREST;
    samplerInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
    samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    m_sampler = SDL_CreateGPUSampler(m_device, &samplerInfo);
}

void Renderer::cleanupPipeline() {
    if (!m_device) return;
    
    if (m_sampler) {
        SDL_ReleaseGPUSampler(m_device, m_sampler);
        m_sampler = nullptr;
    }
    if (m_spritePipeline) {
        SDL_ReleaseGPUGraphicsPipeline(m_device, m_spritePipeline);
        m_spritePipeline = nullptr;
    }
    if (m_vertexBuffer) {
        SDL_ReleaseGPUBuffer(m_device, m_vertexBuffer);
        m_vertexBuffer = nullptr;
    }
}

bool Renderer::beginFrame() {
    if (!m_device) return false;
    
    m_cmdBuf = SDL_AcquireGPUCommandBuffer(m_device);
    if (!m_cmdBuf) return false;
    
    if (!SDL_AcquireGPUSwapchainTexture(m_cmdBuf, m_window, &m_swapchainTexture, nullptr, nullptr)) {
        SDL_SubmitGPUCommandBuffer(m_cmdBuf);
        m_cmdBuf = nullptr;
        return false;
    }
    
    if (!m_swapchainTexture) {
        SDL_SubmitGPUCommandBuffer(m_cmdBuf);
        m_cmdBuf = nullptr;
        return false;
    }
    
    SDL_GPUColorTargetInfo colorTarget = {};
    colorTarget.texture = m_swapchainTexture;
    colorTarget.clear_color = {0.39f, 0.58f, 0.93f, 1.0f}; // Cornflower blue
    colorTarget.load_op = SDL_GPU_LOADOP_CLEAR;
    colorTarget.store_op = SDL_GPU_STOREOP_STORE;
    
    m_renderPass = SDL_BeginGPURenderPass(m_cmdBuf, &colorTarget, 1, nullptr);
    return true;
}

void Renderer::endFrame() {
    if (m_renderPass) {
        SDL_EndGPURenderPass(m_renderPass);
        m_renderPass = nullptr;
    }
    if (m_cmdBuf) {
        SDL_SubmitGPUCommandBuffer(m_cmdBuf);
        m_cmdBuf = nullptr;
    }
}

void Renderer::clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    // We already clear in beginFrame.
}

void Renderer::fillRect(const Vector2& pos, const Vector2& size, const Color& color) {
    // Stubbed
}

void Renderer::drawBatched(std::shared_ptr<Texture> texture, const Vertex* vertices, size_t vertexCount) {
    if (!m_cmdBuf || !texture || vertexCount == 0) return;

    if (m_renderPass) {
        SDL_EndGPURenderPass(m_renderPass);
        m_renderPass = nullptr;
    }

    // 1. Upload dynamic vertex data using a transfer buffer
    SDL_GPUTransferBufferCreateInfo tbInfo = {};
    tbInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    tbInfo.size = vertexCount * sizeof(Vertex);
    SDL_GPUTransferBuffer* tb = SDL_CreateGPUTransferBuffer(m_device, &tbInfo);
    
    void* map = SDL_MapGPUTransferBuffer(m_device, tb, false);
    if (map) {
        std::memcpy(map, vertices, vertexCount * sizeof(Vertex));
        SDL_UnmapGPUTransferBuffer(m_device, tb);
        
        SDL_GPUCopyPass* copy = SDL_BeginGPUCopyPass(m_cmdBuf);
        SDL_GPUTransferBufferLocation srcLoc = {};
        srcLoc.transfer_buffer = tb;
        srcLoc.offset = 0;
        
        SDL_GPUBufferCreateInfo vInfo = {};
        vInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
        vInfo.size = vertexCount * sizeof(Vertex);
        SDL_GPUBuffer* vBuf = SDL_CreateGPUBuffer(m_device, &vInfo);
        
        SDL_GPUBufferRegion dstReg = {};
        dstReg.buffer = vBuf;
        dstReg.offset = 0;
        dstReg.size = vertexCount * sizeof(Vertex);
        
        SDL_UploadToGPUBuffer(copy, &srcLoc, &dstReg, false);
        SDL_EndGPUCopyPass(copy);
        
        // 2. Resume RenderPass with LOADOP_LOAD so we don't clear the screen
        SDL_GPUColorTargetInfo colorTarget = {};
        colorTarget.texture = m_swapchainTexture;
        colorTarget.load_op = SDL_GPU_LOADOP_LOAD; 
        colorTarget.store_op = SDL_GPU_STOREOP_STORE;
        m_renderPass = SDL_BeginGPURenderPass(m_cmdBuf, &colorTarget, 1, nullptr);
        
        // 3. Bindings
        SDL_BindGPUGraphicsPipeline(m_renderPass, m_spritePipeline);
        
        int window_w, window_h;
        SDL_GetWindowSizeInPixels(m_window, &window_w, &window_h);
        float W_screen = static_cast<float>(window_w);
        float H_screen = static_cast<float>(window_h);
        
        SDL_GPUViewport viewport = {0.0f, 0.0f, W_screen, H_screen, 0.0f, 1.0f};
        SDL_SetGPUViewport(m_renderPass, &viewport);
        
        SDL_GPUBufferBinding vertexBinding = {};
        vertexBinding.buffer = vBuf;
        vertexBinding.offset = 0;
        SDL_BindGPUVertexBuffers(m_renderPass, 0, &vertexBinding, 1);
        
        SDL_GPUTextureSamplerBinding samplerBinding = {};
        samplerBinding.texture = texture->m_texture;
        samplerBinding.sampler = m_sampler;
        SDL_BindGPUFragmentSamplers(m_renderPass, 0, &samplerBinding, 1);
        
        // 4. Push constants for Orthographic Projection
        PushConstants pc = {};
        pc.transform[0] = 2.0f / W_screen;
        pc.transform[1] = 0.0f;
        pc.transform[2] = 0.0f;
        pc.transform[3] = 0.0f;
        
        pc.transform[4] = 0.0f;
        pc.transform[5] = -2.0f / H_screen;
        pc.transform[6] = 0.0f;
        pc.transform[7] = 0.0f;
        
        pc.transform[8] = 0.0f;
        pc.transform[9] = 0.0f;
        pc.transform[10] = 1.0f;
        pc.transform[11] = 0.0f;
        
        pc.transform[12] = -1.0f;
        pc.transform[13] = 1.0f;
        pc.transform[14] = 0.0f;
        pc.transform[15] = 1.0f;
        
        pc.r = 1.0f;
        pc.g = 1.0f;
        pc.b = 1.0f;
        pc.a = 1.0f;
        
        SDL_PushGPUVertexUniformData(m_cmdBuf, 0, &pc, sizeof(pc));
        
        // 5. Draw the entire batch
        SDL_DrawGPUPrimitives(m_renderPass, vertexCount, 1, 0, 0);
        
        // 6. Release temporary buffers (SDL_GPU handles deferred destruction safely)
        SDL_ReleaseGPUBuffer(m_device, vBuf);
        SDL_ReleaseGPUTransferBuffer(m_device, tb);
    }
}

} // namespace thengine
