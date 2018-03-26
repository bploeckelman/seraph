#include <assert.h>

#include "texture_region.h"

TextureRegion *createTextureRegion(Texture *texture, int x, int y, int w, int h) {
    assert(texture != NULL);
    assert(w > 0 && h >= 0);

    TextureRegion *textureRegion = (TextureRegion *) calloc(1, sizeof(TextureRegion));
    textureRegion->texture = texture;
    textureRegion->region = (SDL_Rect) { x, y, w, h };
    return textureRegion;
}

void renderTextureRegion(SDL_Renderer *renderer, TextureRegion *textureRegion, const SDL_Rect *dest) {
    assert(textureRegion != NULL && textureRegion->texture != NULL && textureRegion->texture->texture != NULL);
    renderTexture(renderer, textureRegion->texture, &textureRegion->region, dest);
}
