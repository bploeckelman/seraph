#ifndef SERAPH_SPRITE_H
#define SERAPH_SPRITE_H

#include "texture_region.h"

typedef struct Sprite {
    SDL_Rect bounds;
    TextureRegion *region;
} Sprite;

Sprite *createSprite(TextureRegion *region);
Sprite *createSpriteWithBounds(TextureRegion *region, int x, int y, int w, int h);
void renderSprite(SDL_Renderer *renderer, const Sprite *sprite);

#endif //SERAPH_SPRITE_H
