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

    renderTextureRegion(renderer, sprite->keyframe, &sprite->bounds);
}
