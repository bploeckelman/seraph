#include <stdio.h>
#include <stdbool.h>

#define SDL_MAIN_HANDLED
#include "SDL.h"

#include "sprite.h"
#include "animation.h"
#include "assets.h"

#define SCREEN_TITLE "Seraph"
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCREEN_FLAGS (SDL_WINDOW_RESIZABLE)
#define RENDER_FLAGS (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)

typedef struct Game {
    bool running;

    struct {
        Uint64 now;
        Uint64 prev;
        double delta;
    } timer;

    struct {
        const char *title;
        unsigned int width;
        unsigned int height;
        unsigned int windowFlags;
        unsigned int renderFlags;
        SDL_Window *window;
        SDL_Renderer *renderer;
    } screen;

    struct {
        Sprite *sprite;
        size_t animIndex;
        float animStateTime;
    } graphics;

    Assets *assets;
} Game;

// ----------------------------------------------------------------------------

Game game = {
        false,
        {
                0, 0, 0.0
        },
        {
                SCREEN_TITLE,
                SCREEN_WIDTH,
                SCREEN_HEIGHT,
                SCREEN_FLAGS,
                RENDER_FLAGS,
                NULL,
                NULL,
        },
        {
                NULL, 0, 0.f,
        },
        NULL,
};

// ----------------------------------------------------------------------------

void init();
void initAssets();
void events();
void update();
void updateTimer();
void render();
void shutdown();

// ----------------------------------------------------------------------------

void init() {
    atexit(shutdown);

    Uint32 sdlFlags = SDL_INIT_EVERYTHING;
    if (SDL_Init(sdlFlags)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to initialize SDL: %s", SDL_GetError());
        exit(1);
    }

    game.screen.window = SDL_CreateWindow(game.screen.title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          game.screen.width, game.screen.height, game.screen.windowFlags);
    if (game.screen.window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create window: %s", SDL_GetError());
        exit(1);
    }

    game.screen.renderer = SDL_CreateRenderer(game.screen.window, -1, game.screen.renderFlags);
    if (game.screen.renderer == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create renderer: %s", SDL_GetError());
        exit(1);
    }

    initAssets();
    updateTimer();

    game.running = true;
}

void initAssets() {
    game.assets = loadAssets("data/assets.json", game.screen.renderer);

    TextureRegion *spriteRegion = createTextureRegion(game.assets->spritesheets[0], 0, 0, 24, 24);
    game.graphics.sprite = createSpriteWithBounds(spriteRegion, 0, 0, 96, 96);
}

void events() {
    int x = 0;
    int y = 0;
    SDL_GetMouseState(&x, &y);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: game.running = false; break;
            case SDL_KEYUP: {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    game.running = false;
                }
                if (event.key.keysym.sym == SDLK_SPACE) {
                    game.graphics.animIndex = (game.graphics.animIndex + 1) % game.assets->numAnimations;
                }
            } break;
            case SDL_KEYDOWN: {
//                if (event.key.keysym.sym == SDLK_) {
//                }
            } break;
            default: break;
        }
    }
}

void update() {
    updateTimer();

    const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
    const float speed = (float) (200 * game.timer.delta);

    if (keyboardState[SDL_SCANCODE_LEFT]) {
        translateSprite(game.graphics.sprite, -speed, 0.f);
        game.graphics.sprite->facing = LEFT;
    } else if (keyboardState[SDL_SCANCODE_RIGHT]) {
        translateSprite(game.graphics.sprite, speed, 0.f);
        game.graphics.sprite->facing = RIGHT;
    }

    if (keyboardState[SDL_SCANCODE_UP]) {
        translateSprite(game.graphics.sprite, 0.f, -speed);
    } else if (keyboardState[SDL_SCANCODE_DOWN]) {
        translateSprite(game.graphics.sprite, 0.f, speed);
    }

    game.graphics.animStateTime += game.timer.delta;
    TextureRegion *keyframe = getAnimationKeyFrame(game.assets->animations[game.graphics.animIndex], game.graphics.animStateTime);
    if (keyframe != NULL) {
        game.graphics.sprite->keyframe = keyframe;
    }
}

void updateTimer() {
    game.timer.prev = game.timer.now;
    game.timer.now = SDL_GetPerformanceCounter();
    game.timer.delta = (double) ((game.timer.now - game.timer.prev) * 1000 / SDL_GetPerformanceFrequency()) * 0.001;
}

void render() {
    SDL_SetRenderDrawColor(game.screen.renderer, 0xd3, 0xd3, 0xd3, 0x00);
    SDL_RenderClear(game.screen.renderer);

    renderSprite(game.screen.renderer, game.graphics.sprite);

    SDL_RenderPresent(game.screen.renderer);
}

void shutdown() {
    destroyAssets(game.assets);

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
    exit(0);
}
