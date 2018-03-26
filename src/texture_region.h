#ifndef SERAPH_TEXTURE_REGION_H
#define SERAPH_TEXTURE_REGION_H

#include "texture.h"

typedef struct TextureRegion {
    Texture *texture;
    SDL_Rect region;
} TextureRegion;

TextureRegion *createTextureRegion(Texture *texture, int x, int y, int w, int h);
void renderTextureRegion(SDL_Renderer *renderer, TextureRegion *textureRegion, const SDL_Rect *dest);

#endif //SERAPH_TEXTURE_REGION_H
