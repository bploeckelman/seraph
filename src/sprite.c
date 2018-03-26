#include <assert.h>

#include "sprite.h"

Sprite *createSprite(TextureRegion *region) {
    assert(region != NULL);

    int x = 0;
    int y = 0;
    int w = region->region.w;
    int h = region->region.h;
    return createSpriteWithBounds(region, x, y, w, h);
}

Sprite *createSpriteWithBounds(TextureRegion *region, int x, int y, int w, int h) {
    assert(region != NULL);

    Sprite *sprite = (Sprite *) calloc(1, sizeof(Sprite));
    sprite->bounds = (SDL_Rect) { x, y, w, h };
    sprite->region = region;
    return sprite;
}

void renderSprite(SDL_Renderer *renderer, const Sprite *sprite) {
    assert(renderer != NULL && sprite != NULL);
    renderTextureRegion(renderer, sprite->region, &sprite->bounds);
}
