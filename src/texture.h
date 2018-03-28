#ifndef SERAPH_TEXTURE_H
#define SERAPH_TEXTURE_H

#include "SDL.h"

typedef struct Texture {
    const char *name;
    const char *path;
    unsigned int width;
    unsigned int height;
    SDL_Texture *texture;
} Texture;

Texture *createTextureFromFile(SDL_Renderer *renderer, const char *path, const char *name);
Texture *createTextureFromSurface(SDL_Renderer *renderer, SDL_Surface *surface, const char *name);
void renderTexture(SDL_Renderer *renderer, const Texture *texture, const SDL_Rect *src, const SDL_Rect *dest);
void destroyTexture(Texture *texture);

#endif //SERAPH_TEXTURE_H
