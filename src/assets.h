#ifndef SERAPH_ASSETS_H
#define SERAPH_ASSETS_H

#include "texture.h"
#include "animation.h"

typedef struct Assets {
    const char *path;
    size_t numSpritesheets;
    size_t numAnimations;
    Texture **spritesheets;
    Animation **animations;
} Assets;

Assets *loadAssets(const char *assetFilePath, SDL_Renderer *renderer);
Texture *getSpritesheet(Assets *assets, const char *name);
Animation *getAnimation(Assets *assets, const char *name);
void destroyAssets(Assets *assets);

#endif //SERAPH_ASSETS_H
