#include <stdio.h>
#include <stdbool.h>

#define SDL_MAIN_HANDLED
#include "SDL.h"
#include "SDL_image.h"

#define SCREEN_TITLE "Seraph"
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCREEN_FLAGS (SDL_WINDOW_RESIZABLE)
#define RENDER_FLAGS (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)

typedef struct Game {
    bool running;
    struct {
        const char *title;
        unsigned int width;
        unsigned int height;
        unsigned int windowFlags;
        unsigned int renderFlags;
        SDL_Window *window;
        SDL_Renderer *renderer;
        SDL_Texture *texture;
    } screen;
} Game;
Game game = {
        false,
        {
                SCREEN_TITLE,
                SCREEN_WIDTH,
                SCREEN_HEIGHT,
                SCREEN_FLAGS,
                RENDER_FLAGS,
                NULL,
                NULL,
                NULL,
        }
};

void init() {
    Uint32 sdlFlags = SDL_INIT_EVERYTHING;
    if (SDL_Init(sdlFlags)) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        exit(1);
    }

    game.screen.window = SDL_CreateWindow(game.screen.title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          game.screen.width, game.screen.height, game.screen.windowFlags);
    if (game.screen.window == NULL) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        exit(1);
    }

    game.screen.renderer = SDL_CreateRenderer(game.screen.window, -1, game.screen.renderFlags);
    if (game.screen.renderer == NULL) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        exit(1);
    }

    SDL_Surface *surface = IMG_Load("data/test.png");
    if (surface == NULL) {
        SDL_Log("Failed to load image: %s", IMG_GetError());
        exit(1);
    }

    game.screen.texture = SDL_CreateTextureFromSurface(game.screen.renderer, surface);
    if (game.screen.texture == NULL) {
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
    SDL_SetRenderDrawColor(game.screen.renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(game.screen.renderer);
    SDL_RenderCopy(game.screen.renderer, game.screen.texture, NULL, NULL);
    SDL_RenderPresent(game.screen.renderer);
}

void shutdown() {
    SDL_DestroyTexture(game.screen.texture);
    SDL_DestroyRenderer(game.screen.renderer);
    SDL_DestroyWindow(game.screen.window);
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