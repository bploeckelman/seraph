#ifndef SERAPH_SPRITE_H
#define SERAPH_SPRITE_H

#include "texture_region.h"

typedef struct Sprite {
    SDL_Rect bounds;
    TextureRegion *keyframe;
} Sprite;

Sprite *createSprite(TextureRegion *keyframe);
Sprite *createSpriteWithBounds(TextureRegion *keyframe, int x, int y, int w, int h);
void translateSprite(Sprite *sprite, float x, float y);
void renderSprite(SDL_Renderer *renderer, const Sprite *sprite);

#endif //SERAPH_SPRITE_H
