#ifndef SERAPH_TEXTURE_H
#define SERAPH_TEXTURE_H

#include "SDL.h"

typedef struct Texture {
    unsigned int width;
    unsigned int height;
    SDL_Texture *texture;
} Texture;

Texture *createTextureFromFile(SDL_Renderer *renderer, const char *path);
Texture *createTextureFromSurface(SDL_Renderer *renderer, SDL_Surface *surface);
void renderTexture(SDL_Renderer *renderer, const Texture *texture, const SDL_Rect *dstRegion);
void destroyTexture(Texture *texture);

#endif //SERAPH_TEXTURE_H
