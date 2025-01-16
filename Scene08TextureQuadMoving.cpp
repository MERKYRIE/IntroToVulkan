#include"IntroToVulkan.hpp"
#include"Mat4.hpp"
#include"PositionTextureVertex.hpp"
#include"Quadrilateral.hpp"
#include"Renderer.hpp"
#include"Scene08TextureQuadMoving.hpp"

void Scene08TextureQuadMoving::Load(Renderer& renderer) {
    basePath = SDL_GetBasePath();
    vertexShader = renderer.LoadShader(basePath, "TexturedQuadWithMatrix.vert", 0, 1, 0, 0);
    fragmentShader = renderer.LoadShader(basePath, "TexturedQuadWithMultiplyColor.frag", 1, 1, 0, 0);

    SDL_Surface* imageData = renderer.LoadBMPImage(basePath, "ravioli.bmp", 4);
    if (imageData == nullptr) {
        SDL_Log("Could not load image data!");
    }

    // Create the pipeline
    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .vertex_shader = vertexShader,
        .fragment_shader = fragmentShader,
        // This is set up to match the vertex shader layout!
        .vertex_input_state = SDL_GPUVertexInputState {
            .vertex_buffer_descriptions = new SDL_GPUVertexBufferDescription[1] {{
                .slot = 0,
                .pitch = sizeof(PositionTextureVertex),
                .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                .instance_step_rate = 0,
            }},
            .num_vertex_buffers = 1,
            .vertex_attributes = new SDL_GPUVertexAttribute[2] {{
                .location = 0,
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
                .offset = 0
            }, {
                .location = 1,
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
                .offset = sizeof(float) * 3
            }},
            .num_vertex_attributes = 2,
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .target_info = {
            .color_target_descriptions = new SDL_GPUColorTargetDescription[1] {{
                .format = SDL_GetGPUSwapchainTextureFormat(renderer.device,
        renderer.renderWindow),
            .blend_state = {
                .src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
                .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                .color_blend_op = SDL_GPU_BLENDOP_ADD,
                .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
                .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
                .enable_blend = true,
            }
        }},
        .num_color_targets = 1,
        },
    };
    pipelineCreateInfo.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_BACK;
    pipelineCreateInfo.rasterizer_state.front_face = SDL_GPU_FRONTFACE_CLOCKWISE;
    pipeline = renderer.CreateGPUGraphicsPipeline(pipelineCreateInfo);

    // Clean up shader resources
    renderer.ReleaseShader(vertexShader);
    renderer.ReleaseShader(fragmentShader);
}

bool Scene08TextureQuadMoving::Update(float dt) {
    const bool isRunning = ManageInput(inputState);
    time += dt * 2.0F;

    return isRunning;
}

void Scene08TextureQuadMoving::Unload(Renderer& renderer) {
    renderer.ReleaseGraphicsPipeline(pipeline);
}

void Scene08TextureQuadMoving::Draw(Renderer& ARenderer) {
    ARenderer.Begin();
    ARenderer.BindGraphicsPipeline(pipeline);
    auto LBackward
    {
        std::make_shared<NIntroToVulkan::SQuadrilateral>
        (
            &ARenderer , "cube0.bmp"
            ,
            -0.5F , -0.5F , -1.0F
            ,
            -0.5F , +0.5F , -1.0F
            ,
            +0.5F , +0.5F , -1.0F
            ,
            +0.5F , -0.5F , -1.0F
        )
    };
    auto LLeft
    {
        std::make_shared<NIntroToVulkan::SQuadrilateral>
        (
            &ARenderer , "cube1.bmp"
            ,
            -0.5F , -0.5F , -2.0F
            ,
            -0.5F , +0.5F , -2.0F
            ,
            -0.5F , +0.5F , -1.0F
            ,
            -0.5F , -0.5F , -1.0F
        )
    };
    LBackward->IRender(time);
    LLeft->IRender(time);
    ARenderer.End();
}