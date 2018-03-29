#include <assert.h>

#include "sprite.h"

Sprite *createSprite(TextureRegion *keyframe) {
    assert(keyframe != NULL);

    int x = 0;
    int y = 0;
    int w = keyframe->region.w;
    int h = keyframe->region.h;
    return createSpriteWithBounds(keyframe, x, y, w, h);
}

Sprite *createSpriteWithBounds(TextureRegion *keyframe, int x, int y, int w, int h) {
    assert(keyframe != NULL);

    Sprite *sprite = (Sprite *) calloc(1, sizeof(Sprite));
    sprite->facing = RIGHT;
    sprite->bounds = (SDL_Rect) { x, y, w, h };
    sprite->keyframe = keyframe;
    return sprite;
}

void translateSprite(Sprite *sprite, float dx, float dy) {
    assert(sprite != NULL);

    sprite->bounds.x += dx;
    sprite->bounds.y += dy;
}

void renderSprite(SDL_Renderer *renderer, const Sprite *sprite) {
    assert(renderer != NULL && sprite != NULL);

    SDL_Texture *texture        =  sprite->keyframe->texture->texture;
    SDL_Rect *srcRect           = &sprite->keyframe->region;
    const SDL_Rect *destRect    = &sprite->bounds;
    const double angle          = 0.0;
    const SDL_Point *origin     = NULL;
    const SDL_RendererFlip flip = (sprite->facing == LEFT) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

    SDL_RenderCopyEx(renderer, texture, srcRect, destRect, angle, origin, flip);
}
