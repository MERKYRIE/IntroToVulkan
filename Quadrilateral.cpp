#include"IntroToVulkan.hpp"
#include"Mat4.hpp"
#include"PositionTextureVertex.hpp"
#include"Renderer.hpp"
#include"Scene08TextureQuadMoving.hpp"

namespace NIntroToVulkan
{
    SQuadrilateral::SQuadrilateral
    (
        Renderer *const& ARenderer , std::string const& ATexture
        ,
        float const& AAX , float const& AAY , float const& AAZ
        ,
        float const& ABX , float const& ABY , float const& ABZ
        ,
        float const& ACX , float const& ACY , float const& ACZ
        ,
        float const& ADX , float const& ADY , float const& ADZ
    )
    {
        FRenderer = ARenderer;
        SDL_Surface* LSurface = FRenderer->LoadBMPImage(SDL_GetBasePath() , ATexture.c_str() , 4);
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
        FSampler = FRenderer->CreateSampler(LSamplerInformation);
        SDL_GPUBufferCreateInfo LVertexInformation{.usage{SDL_GPU_BUFFERUSAGE_VERTEX} , .size{sizeof(PositionTextureVertex) * 4}};
        SDL_GPUBufferCreateInfo LIndexInformation{.usage{SDL_GPU_BUFFERUSAGE_INDEX} , .size{sizeof(std::uint16_t) * 6}};
        FVertex = FRenderer->CreateBuffer(LVertexInformation);
        FIndex = FRenderer->CreateBuffer(LIndexInformation);
        SDL_GPUTextureCreateInfo LTextureInformation
        {
            .type{SDL_GPU_TEXTURETYPE_2D}
            ,
            .format{SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM}
            ,
            .usage{SDL_GPU_TEXTUREUSAGE_SAMPLER}
            ,
            .width{static_cast<std::uint32_t>(LSurface->w)}
            ,
            .height{static_cast<std::uint32_t>(LSurface->h)}
            ,
            .layer_count_or_depth{1}
            ,
            .num_levels{1}
        };
        FTexture = FRenderer->CreateTexture(LTextureInformation);
        SDL_GPUTransferBufferCreateInfo LBufferTransferInformation{.usage{SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD} , .size{LVertexInformation.size + LIndexInformation.size}};
        SDL_GPUTransferBuffer * LBufferTransfer{FRenderer->CreateTransferBuffer(LBufferTransferInformation)};
        auto LVertexData{static_cast<PositionTextureVertex *>(FRenderer->MapTransferBuffer(LBufferTransfer , false))};
        LVertexData[0] = {AAX , AAY , AAZ , 0.0F , 1.0F};
        LVertexData[1] = {ABX , ABY , ABZ , 0.0F , 0.0F};
        LVertexData[2] = {ACX , ACY , ACZ , 1.0F , 0.0F};
        LVertexData[3] = {ADX , ADY , ADZ , 1.0F , 1.0F};
        auto LIndexData{reinterpret_cast<Uint16 *>(&LVertexData[4])};
        LIndexData[0] = 0;
        LIndexData[1] = 1;
        LIndexData[2] = 2;
        LIndexData[3] = 0;
        LIndexData[4] = 2;
        LIndexData[5] = 3;
        FRenderer->UnmapTransferBuffer(LBufferTransfer);
        SDL_GPUTransferBufferCreateInfo LTextureTransferInformation{.usage{SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD} , .size{static_cast<std::uint32_t>(LSurface->w * LSurface->h * 4)}};
        SDL_GPUTransferBuffer * LTextureTransfer = FRenderer->CreateTransferBuffer(LTextureTransferInformation);
        auto LTextureData{FRenderer->MapTransferBuffer(LTextureTransfer , false)};
        std::memcpy(LTextureData , LSurface->pixels , LTextureTransferInformation.size);
        FRenderer->UnmapTransferBuffer(LTextureTransfer);
        FRenderer->BeginUploadToBuffer();
        SDL_GPUTransferBufferLocation LVertexLocation{.transfer_buffer{LBufferTransfer} , .offset{0}};
        SDL_GPUTransferBufferLocation LIndexLocation{.transfer_buffer{LBufferTransfer} , .offset{LVertexInformation.size}};        
        SDL_GPUBufferRegion LVertexRegion{.buffer{FVertex} , .offset{0}, .size{LVertexInformation.size}};
        SDL_GPUBufferRegion LIndexRegion{.buffer{FIndex} , .offset{0}, .size{LIndexInformation.size}};
        SDL_GPUTextureTransferInfo LTextureLocation{.transfer_buffer{LTextureTransfer} , .offset{0}};
        SDL_GPUTextureRegion LTextureRegion{.texture{FTexture} , .w{static_cast<std::uint32_t>(LSurface->w)} , .h{static_cast<std::uint32_t>(LSurface->h)} , .d{1}};
        FRenderer->UploadToBuffer(LVertexLocation , LVertexRegion , false);
        FRenderer->UploadToBuffer(LIndexLocation , LIndexRegion , false);
        FRenderer->UploadToTexture(LTextureLocation , LTextureRegion , false);
        FRenderer->EndUploadToBuffer(LBufferTransfer);
        FRenderer->ReleaseTransferBuffer(LTextureTransfer);
        FRenderer->ReleaseSurface(LSurface);
    }
    
    void SQuadrilateral::IRender(float const& ARotation)
    {
        FRenderer->BindVertexBuffers(0 , {.buffer{FVertex} , .offset{0}} , 1);
        FRenderer->BindIndexBuffer({.buffer{FIndex} , .offset{0}} , SDL_GPU_INDEXELEMENTSIZE_16BIT);
        FRenderer->BindFragmentSamplers(0 , {.texture{FTexture} , .sampler{FSampler}} , 1);

        /*
        auto LTransform{std::make_shared<Mat4>(Mat4::CreateRotationZ(ARotation) * Mat4::CreateTranslation(AX , AY , 0.0F))};
        FRenderer->PushVertexUniformData(0 , LTransform.get() , sizeof(Mat4));
        auto LColor{std::make_shared<FragMultiplyUniform>(1.0F , 0.5F + AColor , 1.0F , 1.0F)};
        FRenderer->PushFragmentUniformData(0 , LColor.get() , sizeof(FragMultiplyUniform));
        */
        auto LTransform{std::make_shared<Mat4>(Mat4::CreatePerspectiveFieldOfView(90.0F , 16.0F / 9.0F , 0.0625F , 1000.0F) * Mat4::CreateRotationZ(ARotation))};
        FRenderer->PushVertexUniformData(0 , LTransform.get() , sizeof(Mat4));
        auto LColor{std::make_shared<FragMultiplyUniform>(1.0F , 1.0F , 1.0F , 1.0F)};
        FRenderer->PushFragmentUniformData(0 , LColor.get() , sizeof(FragMultiplyUniform));

        FRenderer->DrawIndexedPrimitives(6 , 1 , 0 , 0 , 0);
    }

    SQuadrilateral::~SQuadrilateral()
    {
        FRenderer->ReleaseTexture(FTexture);
        FRenderer->ReleaseSampler(FSampler);
        FRenderer->ReleaseBuffer(FIndex);
        FRenderer->ReleaseBuffer(FVertex);
    }
}