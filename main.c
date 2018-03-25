#include <stdio.h>
#include <stdbool.h>

#define SDL_MAIN_HANDLED
#include "SDL.h"

bool running = true;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;

void init() {
    Uint32 sdlFlags = SDL_INIT_VIDEO;
    if (SDL_Init(sdlFlags)) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        exit(1);
    }

    SDL_CreateWindowAndRenderer(640, 480, SDL_WINDOW_RESIZABLE, &window, &renderer);
    if (window == NULL) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        exit(1);
    }

    SDL_Surface *surface = SDL_LoadBMP("data/test.bmp");
    if (surface == NULL) {
        SDL_Log("Failed to load bitmap: %s", SDL_GetError());
        exit(1);
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL) {
        SDL_Log("Failed to create texture from surface: %s", SDL_GetError());
        exit(1);
    }
    SDL_FreeSurface(surface);
}

void events() {
    int x = 0;
    int y = 0;
    SDL_GetMouseState(&x, &y);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: running = false; break;
//            case SDL_KEYDOWN: break;
//            case SDL_KEYUP: break;
            default: break;
        }
    }
}

void update() {
}

void render() {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void shutdown() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char **argv) {
    init();
    while (running) {
        events();
        update();
        render();
    }
    shutdown();
    return 0;
}