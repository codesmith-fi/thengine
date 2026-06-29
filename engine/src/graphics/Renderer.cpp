#include "thengine/Renderer.h"
#include "thengine/DebugLogger.h"
#include "thengine/graphics/Texture.h"
#include "thengine/graphics/SpriteEffect.h"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_video.h>
#include <cstring>
#include <fstream>
#include <vector>
#include <string>

static std::vector<uint8_t> readBinaryFile(const std::string& path) {
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    return {};
  }
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<uint8_t> buffer(size);
  file.read(reinterpret_cast<char*>(buffer.data()), size);
  return buffer;
}

static SDL_GPUShader* loadShaderFile(SDL_GPUDevice* device, const std::string& basePath, SDL_GPUShaderStage stage) {
  SDL_GPUShaderFormat supportedFormats = SDL_GetGPUShaderFormats(device);
  std::string path = basePath;
  SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;

  if (supportedFormats & SDL_GPU_SHADERFORMAT_SPIRV) {
    path += ".spv";
    format = SDL_GPU_SHADERFORMAT_SPIRV;
  } else if (supportedFormats & SDL_GPU_SHADERFORMAT_DXIL) {
    path += ".dxil";
    format = SDL_GPU_SHADERFORMAT_DXIL;
  } else if (supportedFormats & SDL_GPU_SHADERFORMAT_MSL) {
    path += ".msl";
    format = SDL_GPU_SHADERFORMAT_MSL;
  } else {
    return nullptr;
  }

  std::vector<uint8_t> code = readBinaryFile(path);
  if (code.empty()) {
    return nullptr;
  }

  SDL_GPUShaderCreateInfo info = {};
  info.code = code.data();
  info.code_size = code.size();
  info.entrypoint = "main";
  info.format = format;
  info.stage = stage;
  info.num_samplers = (stage == SDL_GPU_SHADERSTAGE_FRAGMENT) ? 1 : 0;
  info.num_storage_textures = 0;
  info.num_storage_buffers = 0;
  info.num_uniform_buffers = (stage == SDL_GPU_SHADERSTAGE_VERTEX) ? 1 : 0;

  return SDL_CreateGPUShader(device, &info);
}

namespace thengine {

#include "thengine/graphics/Vertex.h"

struct PushConstants {
  float transform[16];
  float r, g, b, a;
};

Renderer::Renderer(SDL_Window *window) : m_window(window), m_device(nullptr) {

  m_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV |
                                     SDL_GPU_SHADERFORMAT_DXIL |
                                     SDL_GPU_SHADERFORMAT_MSL,
                                 true,   // debug_mode
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

SDL_GPUGraphicsPipeline* Renderer::createGraphicsPipeline(SDL_GPUShader* vertShader, SDL_GPUShader* fragShader) {
  if (!m_device || !vertShader || !fragShader)
    return nullptr;

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

  pipelineInfo.vertex_input_state.vertex_buffer_descriptions =
      &vertexBufferDesc;
  pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
  pipelineInfo.vertex_input_state.vertex_attributes = vertexAttributes;
  pipelineInfo.vertex_input_state.num_vertex_attributes = 3;

  pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

  pipelineInfo.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
  pipelineInfo.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
  pipelineInfo.rasterizer_state.front_face =
      SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;

  pipelineInfo.depth_stencil_state.enable_depth_test = false;
  pipelineInfo.depth_stencil_state.enable_depth_write = false;

  SDL_GPUColorTargetDescription colorTargetDesc = {};
  colorTargetDesc.format = SDL_GetGPUSwapchainTextureFormat(m_device, m_window);
  colorTargetDesc.blend_state.enable_blend = true;
  colorTargetDesc.blend_state.src_color_blendfactor =
      SDL_GPU_BLENDFACTOR_SRC_ALPHA;
  colorTargetDesc.blend_state.dst_color_blendfactor =
      SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
  colorTargetDesc.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
  colorTargetDesc.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
  colorTargetDesc.blend_state.dst_alpha_blendfactor =
      SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
  colorTargetDesc.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;

  pipelineInfo.target_info.color_target_descriptions = &colorTargetDesc;
  pipelineInfo.target_info.num_color_targets = 1;

  SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(m_device, &pipelineInfo);
  if (!pipeline) {
    LOG_ERROR() << "Failed to create graphics pipeline: " << SDL_GetError();
  }
  return pipeline;
}

void Renderer::initPipeline() {
  if (!m_device)
    return;

  SDL_GPUShader* vertShader = loadShaderFile(m_device, "assets/shaders/basic.vert", SDL_GPU_SHADERSTAGE_VERTEX);
  if (!vertShader) {
    LOG_ERROR() << "Failed to load default vertex shader from file: assets/shaders/basic.vert";
  }

  SDL_GPUShader* fragShader = loadShaderFile(m_device, "assets/shaders/basic.frag", SDL_GPU_SHADERSTAGE_FRAGMENT);
  if (!fragShader) {
    LOG_ERROR() << "Failed to load default fragment shader from file: assets/shaders/basic.frag";
  }

  if (vertShader && fragShader) {
    m_spritePipeline = createGraphicsPipeline(vertShader, fragShader);
  } else {
    LOG_ERROR() << "Could not build the fallback sprite pipeline because default shaders failed to load.";
  }

  if (vertShader) SDL_ReleaseGPUShader(m_device, vertShader);
  if (fragShader) SDL_ReleaseGPUShader(m_device, fragShader);

  SDL_GPUBufferCreateInfo bufferInfo = {};
  bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
  bufferInfo.size = sizeof(Vertex) * MAX_VERTICES;

  m_vertexBuffer = SDL_CreateGPUBuffer(m_device, &bufferInfo);

  SDL_GPUTransferBufferCreateInfo tbInfo = {};
  tbInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
  tbInfo.size = sizeof(Vertex) * MAX_VERTICES;

  m_transferBuffer = SDL_CreateGPUTransferBuffer(m_device, &tbInfo);

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
  if (!m_device)
    return;

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
  if (m_transferBuffer) {
    SDL_ReleaseGPUTransferBuffer(m_device, m_transferBuffer);
    m_transferBuffer = nullptr;
  }
  for (size_t i = 0; i < NUM_FRAMES; ++i) {
    if (m_fences[i]) {
      SDL_ReleaseGPUFence(m_device, m_fences[i]);
      m_fences[i] = nullptr;
    }
  }
}

bool Renderer::beginFrame() {
  if (!m_device)
    return false;

  m_currentFrameIndex = (m_currentFrameIndex + 1) % NUM_FRAMES;

  if (m_fences[m_currentFrameIndex]) {
    SDL_WaitForGPUFences(m_device, true, &m_fences[m_currentFrameIndex], 1);
    SDL_ReleaseGPUFence(m_device, m_fences[m_currentFrameIndex]);
    m_fences[m_currentFrameIndex] = nullptr;
  }

  m_vertexOffset = m_currentFrameIndex * MAX_VERTICES_PER_FRAME;

  m_cmdBuf = SDL_AcquireGPUCommandBuffer(m_device);
  if (!m_cmdBuf)
    return false;

  if (!SDL_AcquireGPUSwapchainTexture(m_cmdBuf, m_window, &m_swapchainTexture,
                                      nullptr, nullptr)) {
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
    m_fences[m_currentFrameIndex] = SDL_SubmitGPUCommandBufferAndAcquireFence(m_cmdBuf);
    m_cmdBuf = nullptr;
  }
}

void Renderer::clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  // We already clear in beginFrame.
}

void Renderer::fillRect(const Vector2 &pos, const Vector2 &size,
                        const Color &color) {
  // Stubbed
}

void Renderer::registerEffect(int id, std::shared_ptr<SpriteEffect> effect) {
  m_Effects[id] = effect;
}

void Renderer::drawBatched(std::shared_ptr<Texture> texture,
                           const Vertex *vertices, size_t vertexCount,
                           std::shared_ptr<SpriteEffect> effect,
                           const Matrix4& transformMatrix) {
  if (!m_cmdBuf || !texture || vertexCount == 0)
    return;

  // Verify that the batch fits in the current frame's slot boundaries
  size_t frameStart = m_currentFrameIndex * MAX_VERTICES_PER_FRAME;
  size_t frameEnd = (m_currentFrameIndex + 1) * MAX_VERTICES_PER_FRAME;
  if (m_vertexOffset + vertexCount > frameEnd) {
    LOG_WARN() << "Vertex buffer streaming limit reached for frame " << m_currentFrameIndex 
               << ", wrapping around! Current offset: " << m_vertexOffset 
               << ", required: " << vertexCount;
    m_vertexOffset = frameStart;
  }

  if (m_renderPass) {
    SDL_EndGPURenderPass(m_renderPass);
    m_renderPass = nullptr;
  }

  // 1. Upload dynamic vertex data using the pre-allocated transfer buffer
  void *map = SDL_MapGPUTransferBuffer(m_device, m_transferBuffer, false);
  if (map) {
    std::memcpy(static_cast<char*>(map) + (m_vertexOffset * sizeof(Vertex)),
                vertices,
                vertexCount * sizeof(Vertex));
    SDL_UnmapGPUTransferBuffer(m_device, m_transferBuffer);

    SDL_GPUCopyPass *copy = SDL_BeginGPUCopyPass(m_cmdBuf);
    SDL_GPUTransferBufferLocation srcLoc = {};
    srcLoc.transfer_buffer = m_transferBuffer;
    srcLoc.offset = m_vertexOffset * sizeof(Vertex);

    SDL_GPUBufferRegion dstReg = {};
    dstReg.buffer = m_vertexBuffer;
    dstReg.offset = m_vertexOffset * sizeof(Vertex);
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
    if (effect && effect->getPipeline()) {
      SDL_BindGPUGraphicsPipeline(m_renderPass, effect->getPipeline());
    } else {
      SDL_BindGPUGraphicsPipeline(m_renderPass, m_spritePipeline);
    }

    int window_w, window_h;
    SDL_GetWindowSizeInPixels(m_window, &window_w, &window_h);
    float W_screen = static_cast<float>(window_w);
    float H_screen = static_cast<float>(window_h);

    SDL_GPUViewport viewport = {0.0f, 0.0f, W_screen, H_screen, 0.0f, 1.0f};
    SDL_SetGPUViewport(m_renderPass, &viewport);

    SDL_GPUBufferBinding vertexBinding = {};
    vertexBinding.buffer = m_vertexBuffer;
    vertexBinding.offset = m_vertexOffset * sizeof(Vertex);
    SDL_BindGPUVertexBuffers(m_renderPass, 0, &vertexBinding, 1);

    SDL_GPUTextureSamplerBinding samplerBinding = {};
    samplerBinding.texture = texture->m_texture;
    samplerBinding.sampler = m_sampler;
    SDL_BindGPUFragmentSamplers(m_renderPass, 0, &samplerBinding, 1);

    // 4. Push constants for Orthographic Projection combined with Camera/Transform
    Matrix4 ortho = Matrix4::createOrthographic(W_screen, H_screen);
    Matrix4 finalTransform = ortho * transformMatrix;

    PushConstants pc = {};
    std::memcpy(pc.transform, finalTransform.m, sizeof(pc.transform));

    pc.r = 1.0f;
    pc.g = 1.0f;
    pc.b = 1.0f;
    pc.a = 1.0f;

    SDL_PushGPUVertexUniformData(m_cmdBuf, 0, &pc, sizeof(pc));

    // 5. Draw the entire batch
    SDL_DrawGPUPrimitives(m_renderPass, vertexCount, 1, 0, 0);

    // Advance offset
    m_vertexOffset += vertexCount;
  }
}

} // namespace thengine
