#include"IntroToVulkan.hpp"
#include"Mat4.hpp"
#include"PositionTextureVertex.hpp"
#include"Quadrilateral.hpp"
#include"Renderer.hpp"
#include"Scene08TextureQuadMoving.hpp"

void Scene08TextureQuadMoving::Load(Renderer& ARenderer) {
    basePath = SDL_GetBasePath();
    vertexShader = ARenderer.LoadShader(basePath, "TexturedQuadWithMatrix.vert", 0, 1, 0, 0);
    fragmentShader = ARenderer.LoadShader(basePath, "TexturedQuadWithMultiplyColor.frag", 1, 1, 0, 0);

    SDL_Surface* imageData = ARenderer.LoadBMPImage(basePath, "ravioli.bmp", 4);
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
        .depth_stencil_state
        {
            .compare_op{SDL_GPU_COMPAREOP_LESS}
            ,
            .back_stencil_state
            {
                .fail_op{SDL_GPU_STENCILOP_INVALID}
                ,
                .pass_op{SDL_GPU_STENCILOP_INVALID}
                ,
                .depth_fail_op{SDL_GPU_STENCILOP_INVALID}
                ,
                .compare_op{SDL_GPU_COMPAREOP_INVALID}
            }
            ,
            .front_stencil_state
            {
                .fail_op{SDL_GPU_STENCILOP_INVALID}
                ,
                .pass_op{SDL_GPU_STENCILOP_INVALID}
                ,
                .depth_fail_op{SDL_GPU_STENCILOP_INVALID}
                ,
                .compare_op{SDL_GPU_COMPAREOP_INVALID}
            }
            ,
            .compare_mask{0B00000000}
            ,
            .write_mask{0B00000000}
            ,
            .enable_depth_test{true}
            ,
            .enable_depth_write{false}
            ,
            .enable_stencil_test{false}
        }
        ,
        .target_info = {
            .color_target_descriptions = new SDL_GPUColorTargetDescription[1] {{
                .format = SDL_GetGPUSwapchainTextureFormat(ARenderer.device,
        ARenderer.renderWindow),
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
    pipeline = ARenderer.CreateGPUGraphicsPipeline(pipelineCreateInfo);

    // Clean up shader resources
    ARenderer.ReleaseShader(vertexShader);
    ARenderer.ReleaseShader(fragmentShader);

    SDL_Surface* LSurface = SDL_CreateSurface(540 , 480 , SDL_PIXELFORMAT_RGBA32);
    if(!LSurface)
    {
        SDL_Log("Failed to load the surface!");
    }        
    SDL_GPUSamplerCreateInfo LSamplerInformation
    {
        .min_filter{SDL_GPU_FILTER_NEAREST}
        ,
        .mag_filter{SDL_GPU_FILTER_NEAREST}
        ,
        .mipmap_mode{SDL_GPU_SAMPLERMIPMAPMODE_NEAREST}
        ,
        .address_mode_u{SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE}
        ,
        .address_mode_v{SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE}
        ,
        .address_mode_w{SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE}
    };
    FSampler = ARenderer.CreateSampler(LSamplerInformation);
    SDL_GPUTextureCreateInfo LTextureInformation
    {
        .type{SDL_GPU_TEXTURETYPE_2D}
        ,
        .format{SDL_GPU_TEXTUREFORMAT_D32_FLOAT}
        ,
        .usage{SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET}
        ,
        .width{static_cast<std::uint32_t>(LSurface->w)}
        ,
        .height{static_cast<std::uint32_t>(LSurface->h)}
        ,
        .layer_count_or_depth{1}
        ,
        .num_levels{1}
    };
    FTexture = ARenderer.CreateTexture(LTextureInformation);
    SDL_GPUTransferBufferCreateInfo LTextureTransferInformation{.usage{SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD} , .size{static_cast<std::uint32_t>(LSurface->w * LSurface->h * 4)}};
    SDL_GPUTransferBuffer * LTextureTransfer = ARenderer.CreateTransferBuffer(LTextureTransferInformation);
    auto LTextureData{ARenderer.MapTransferBuffer(LTextureTransfer , false)};
    std::memcpy(LTextureData , LSurface->pixels , LTextureTransferInformation.size);
    ARenderer.UnmapTransferBuffer(LTextureTransfer);
    ARenderer.BeginUploadToBuffer();
    SDL_GPUTextureTransferInfo LTextureLocation{.transfer_buffer{LTextureTransfer} , .offset{0}};
    SDL_GPUTextureRegion LTextureRegion{.texture{FTexture} , .w{static_cast<std::uint32_t>(LSurface->w)} , .h{static_cast<std::uint32_t>(LSurface->h)} , .d{1}};
    ARenderer.UploadToTexture(LTextureLocation , LTextureRegion , false);
    ARenderer.EndUploadToBuffer(LTextureTransfer);
    ARenderer.ReleaseSurface(LSurface);
}

bool Scene08TextureQuadMoving::Update(float dt) {
    const bool isRunning = ManageInput(inputState);
    time += dt * 2.0F;

    return isRunning;
}

void Scene08TextureQuadMoving::Unload(Renderer& ARenderer) {
    ARenderer.ReleaseTexture(FTexture);
    ARenderer.ReleaseSampler(FSampler);
    ARenderer.ReleaseGraphicsPipeline(pipeline);
}

void Scene08TextureQuadMoving::Draw(Renderer& ARenderer) {
    SDL_GPUDepthStencilTargetInfo LInformation
    {
        .texture{FTexture}
        ,
        .clear_depth{1.0F}
        ,
        .load_op{SDL_GPU_LOADOP_CLEAR}
        ,
        .store_op{SDL_GPU_STOREOP_STORE}
        ,
        .stencil_load_op{SDL_GPU_LOADOP_DONT_CARE}
        ,
        .stencil_store_op{SDL_GPU_STOREOP_DONT_CARE}
        ,
        .cycle{false}
        ,
        .clear_stencil{0}
    };
    ARenderer.Begin(&LInformation);
    ARenderer.BindGraphicsPipeline(pipeline);
    auto LBackward
    {
        std::make_shared<NIntroToVulkan::SQuadrilateral>
        (
            &ARenderer , "cube0.bmp"
            ,
            -0.5F , -0.5F , -0.5F
            ,
            -0.5F , +0.5F , -0.5F
            ,
            +0.5F , +0.5F , -0.5F
            ,
            +0.5F , -0.5F , -0.5F
        )
    };
    auto LLeft
    {
        std::make_shared<NIntroToVulkan::SQuadrilateral>
        (
            &ARenderer , "cube1.bmp"
            ,
            -0.5F , -0.5F , -1.5F
            ,
            -0.5F , +0.5F , -1.5F
            ,
            -0.5F , +0.5F , -0.5F
            ,
            -0.5F , -0.5F , -0.5F
        )
    };
    auto LForward
    {
        std::make_shared<NIntroToVulkan::SQuadrilateral>
        (
            &ARenderer , "cube2.bmp"
            ,
            -0.5F , -0.5F , -1.5F
            ,
            -0.5F , +0.5F , -1.5F
            ,
            +0.5F , +0.5F , -1.5F
            ,
            +0.5F , -0.5F , -1.5F
        )
    };
    auto LRight
    {
        std::make_shared<NIntroToVulkan::SQuadrilateral>
        (
            &ARenderer , "cube3.bmp"
            ,
            +0.5F , -0.5F , -1.5F
            ,
            +0.5F , +0.5F , -1.5F
            ,
            +0.5F , +0.5F , -0.5F
            ,
            +0.5F , -0.5F , -0.5F
        )
    };
    auto LDownward
    {
        std::make_shared<NIntroToVulkan::SQuadrilateral>
        (
            &ARenderer , "cube4.bmp"
            ,
            -0.5F , -0.5F , -0.5F
            ,
            -0.5F , -0.5F , -1.5F
            ,
            +0.5F , -0.5F , -1.5F
            ,
            +0.5F , -0.5F , -0.5F
        )
    };
    auto LUpward
    {
        std::make_shared<NIntroToVulkan::SQuadrilateral>
        (
            &ARenderer , "cube5.bmp"
            ,
            -0.5F , +0.5F , -0.5F
            ,
            -0.5F , +0.5F , -1.5F
            ,
            +0.5F , +0.5F , -1.5F
            ,
            +0.5F , +0.5F , -0.5F
        )
    };
    LBackward->IRender(time);
    LLeft->IRender(time);
    LForward->IRender(time);
    LRight->IRender(time);
    LDownward->IRender(time);
    LUpward->IRender(time);
    ARenderer.End();
}