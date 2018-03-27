#include <stdio.h>
#include <stdbool.h>

#define SDL_MAIN_HANDLED
#include "SDL.h"

#include "sprite.h"
#include "animation.h"

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
        Texture *spritesheet;
        Sprite *sprite;
        Animation *animation;
        float animStateTime;
    } graphics;
} Game;

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
                NULL, NULL, NULL, 0.f,
        },
};

#define size 48
SDL_Rect dest = (SDL_Rect) { 0, 0, size, size };


void updateTimer() {
    game.timer.prev = game.timer.now;
    game.timer.now = SDL_GetPerformanceCounter();
    game.timer.delta = (double) ((game.timer.now - game.timer.prev) * 1000 / SDL_GetPerformanceFrequency()) * 0.001;
}

void init() {
    Uint32 sdlFlags = SDL_INIT_EVERYTHING;
    if (SDL_Init(sdlFlags)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to initialize SDL: %s", SDL_GetError());
        exit(1);
    }

    updateTimer();

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

    game.graphics.spritesheet = createTextureFromFile(game.screen.renderer, "data/oryx_16bit_scifi_creatures_extra_trans.png");

    TextureRegion *spriteRegion = createTextureRegion(game.graphics.spritesheet, 0, 0, 24, 24);
    game.graphics.sprite = createSpriteWithBounds(spriteRegion, 0, 0, 96, 96);

    TextureRegion *keyframe1 = createTextureRegion(game.graphics.spritesheet, 0, 0, 24, 24);
    TextureRegion *keyframe2 = createTextureRegion(game.graphics.spritesheet, 0, 24, 24, 24);
    game.graphics.animation = createAnimation(0.33f, 2, keyframe1, keyframe2);

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
            case SDL_KEYUP: {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    game.running = false;
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
    game.graphics.animStateTime += game.timer.delta;

    const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
    const float speed = (float) (200 * game.timer.delta);
    if (keyboardState[SDL_SCANCODE_LEFT]) {
        if (dest.x - speed >= 0.f) {
            dest.x -= speed;
        }
    } else if (keyboardState[SDL_SCANCODE_RIGHT]) {
        if (dest.x + dest.w + speed <= game.screen.width) {
            dest.x += speed;
        }
    }
    if (keyboardState[SDL_SCANCODE_UP]) {
        if (dest.y - speed >= 0.f) {
            dest.y -= speed;
        }
    } else if (keyboardState[SDL_SCANCODE_DOWN]) {
        if (dest.y + dest.h + speed <= game.screen.height) {
            dest.y += speed;
        }
    }
}

void render() {
    SDL_SetRenderDrawColor(game.screen.renderer, 0xff, 0x00, 0xff, 0x00);
    SDL_RenderClear(game.screen.renderer);

//    renderTexture(game.screen.renderer, game.graphics.texture, NULL, NULL);

//    renderTextureRegion(game.screen.renderer, game.graphics.region1, &dest);
//
//    dest = (SDL_Rect) { 0, size, size, size };
//    renderTextureRegion(game.screen.renderer, game.graphics.region2, &dest);

//    renderSprite(game.screen.renderer, game.graphics.sprite);

    TextureRegion *keyframe = getAnimationKeyFrame(game.graphics.animation, game.graphics.animStateTime);
    if (keyframe != NULL) {
        renderTextureRegion(game.screen.renderer, keyframe, &dest);
    }

    SDL_RenderPresent(game.screen.renderer);
}

void shutdown() {
    destroyAnimation(game.graphics.animation);
    destroyTexture(game.graphics.spritesheet);

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