#pragma once

namespace NIntroToVulkan
{
    struct SQuadrilateral
    {
        Renderer * FRenderer;
        SDL_GPUBuffer * FVertex;
        SDL_GPUBuffer * FIndex;
        SDL_GPUSampler * FSampler;
        SDL_GPUTexture * FTexture;
        
        SQuadrilateral
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
        );
        void IRender(float const& ARotation);
        ~SQuadrilateral();
    };
}