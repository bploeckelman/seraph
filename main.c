#include <stdio.h>
#include <stdbool.h>

#define SDL_MAIN_HANDLED
#include "SDL.h"


typedef struct Game {
    bool running;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} Game;
Game game = {
        false,
        NULL,
        NULL,
        NULL,
};

void init() {
    Uint32 sdlFlags = SDL_INIT_EVERYTHING;
    if (SDL_Init(sdlFlags)) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        exit(1);
    }

    SDL_CreateWindowAndRenderer(640, 480, SDL_WINDOW_RESIZABLE, &game.window, &game.renderer);
    if (game.window == NULL) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        exit(1);
    }

    SDL_Surface *surface = SDL_LoadBMP("data/test.bmp");
    if (surface == NULL) {
        SDL_Log("Failed to load bitmap: %s", SDL_GetError());
        exit(1);
    }

    game.texture = SDL_CreateTextureFromSurface(game.renderer, surface);
    if (game.texture == NULL) {
        SDL_Log("Failed to create texture from surface: %s", SDL_GetError());
        exit(1);
    }
    SDL_FreeSurface(surface);

    game.running = true;
}

void events() {
    int x = 0;
    int y = 0;
    SDL_GetMouseState(&x, &y);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: game.running = false; break;
//            case SDL_KEYDOWN: break;
//            case SDL_KEYUP: break;
            default: break;
        }
    }
}

void update() {
}

void render() {
    SDL_SetRenderDrawColor(game.renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(game.renderer);
    SDL_RenderCopy(game.renderer, game.texture, NULL, NULL);
    SDL_RenderPresent(game.renderer);
}

void shutdown() {
    SDL_DestroyTexture(game.texture);
    SDL_DestroyRenderer(game.renderer);
    SDL_DestroyWindow(game.window);
    SDL_Quit();
    game.running = false;
}

int main(int argc, char **argv) {
    init();
    while (game.running) {
        events();
        update();
        render();
    }
    shutdown();
    return 0;
}