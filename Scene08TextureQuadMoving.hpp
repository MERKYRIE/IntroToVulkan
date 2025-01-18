#ifndef SCENE08TEXTUREQUADMOVING_HPP
#define SCENE08TEXTUREQUADMOVING_HPP

#include <SDL3/SDL_gpu.h>
#include "Scene.hpp"
#include <array>
#include <string>

using std::array;
using std::string;

typedef struct FragMultiplyUniform
{
    float r, g, b, a;
} fragMultiplyUniform;

class Scene08TextureQuadMoving : public Scene {
public:
    void Load(Renderer& ARenderer) override;
    bool Update(float dt) override;
    void Draw(Renderer& ARenderer) override;
    void Unload(Renderer& ARenderer) override;

private:
    InputState inputState;
    const char* basePath {nullptr};
    SDL_GPUShader* vertexShader {nullptr};
    SDL_GPUShader* fragmentShader {nullptr};
    
    SDL_GPUGraphicsPipeline* pipeline {nullptr};
    float time {0};

    public : SDL_GPUSampler * FSampler;
    public : SDL_GPUTexture * FTexture;
};

#endif //SCENE08TEXTUREQUADMOVING_HPP