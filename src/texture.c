#include <assert.h>

#include "SDL.h"
#include "SDL_image.h"

#include "texture.h"

Texture *createTextureFromFile(SDL_Renderer *renderer, const char *name, const char *path) {
    assert(renderer != NULL && path != NULL);

    SDL_Surface *surface = IMG_Load(path);
    if (surface == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load image '%s': %s", path, IMG_GetError());
        exit(1);
    }
    Texture *texture = createTextureFromSurface(renderer, surface, name);
    texture->path = path;
    SDL_FreeSurface(surface);

    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Loaded texture: '%s'", path);
    return texture;
}

Texture *createTextureFromSurface(SDL_Renderer *renderer, SDL_Surface *surface, const char *name) {
    assert(renderer != NULL && surface != NULL);
    assert(surface->w >= 0 && surface->h >= 0);

    Texture *texture = (Texture *) calloc(1, sizeof(Texture));
    texture->name = name;
    texture->path = NULL;
    texture->width  = (unsigned int) surface->w;
    texture->height = (unsigned int) surface->h;
    texture->texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture->texture == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create texture from surface: %s", SDL_GetError());
        free(texture);
        exit(1);
    }
    return texture;
}

void renderTexture(SDL_Renderer *renderer, const Texture *texture, const SDL_Rect *src, const SDL_Rect *dest) {
    assert(renderer != NULL && texture != NULL);
    if (src != NULL) {
        assert(src->x >= 0 && src->w <= texture->width
            && src->y >= 0 && src->h <= texture->height);
    }
//    SDL_RenderCopy(renderer, texture->texture, src, dest);
    SDL_RenderCopyEx(renderer, texture->texture, src, dest, 0.0, NULL, SDL_FLIP_HORIZONTAL);
}

void destroyTexture(Texture *texture) {
    if (texture == NULL || texture->texture == NULL) return;
    SDL_DestroyTexture(texture->texture);
    free(texture);
}
