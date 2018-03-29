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
    *sprite = (Sprite) {
            .facing = RIGHT,
            .angle  = 0.0,
            .bounds = (SDL_Rect) { x, y, w, h },
            .keyframe = keyframe
    };
    return sprite;
}

void translateSprite(Sprite *sprite, float dx, float dy) {
    assert(sprite != NULL);

    sprite->bounds.x += dx;
    sprite->bounds.y += dy;
}

void rotateSprite(Sprite *sprite, float da) {
    assert(sprite != NULL);

    sprite->angle += da;
}

void renderSprite(SDL_Renderer *renderer, const Sprite *sprite) {
    assert(renderer != NULL && sprite != NULL);

    SDL_Texture *texture        =  sprite->keyframe->texture->texture;
    SDL_Rect *srcRect           = &sprite->keyframe->region;
    const SDL_Rect *destRect    = &sprite->bounds;
    const double angle          =  sprite->angle;
    const SDL_Point *origin     = NULL; // defaults to (w/2, h/2)
    const SDL_RendererFlip flip = (sprite->facing == LEFT) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

    SDL_RenderCopyEx(renderer, texture, srcRect, destRect, angle, origin, flip);
}
