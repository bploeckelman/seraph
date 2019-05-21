#include <stdio.h>
#include <stdbool.h>

#define SDL_MAIN_HANDLED
#include "SDL.h"

#include "sprite.h"
#include "animation.h"
#include "assets.h"
#include "doom/doom_utils.h"
#include "camera.h"

#define SCREEN_TITLE "Seraph"
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCREEN_FLAGS (SDL_WINDOW_RESIZABLE)
#define RENDER_FLAGS (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)

SDL_MessageBoxButtonData *msgBoxButtons = NULL;

int mapMinX = INT32_MAX;
int mapMinY = INT32_MAX;
int mapMaxX = INT32_MIN;
int mapMaxY = INT32_MIN;
int mapScale = 8;

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

    struct {
        bool leftDown;
        bool rightDown;
    } mouse;

    struct {
        Camera camera;
    } view;

    map_t *map;
    maplumps_t *maplumps;
    int currentMap;

    Assets *assets;
} Game;

// ----------------------------------------------------------------------------

Game game = {
        .running = false,
        {
                .now = 0,
                .prev = 0,
                .delta = 0.0
        },
        {
                .title = SCREEN_TITLE,
                .width = SCREEN_WIDTH,
                .height = SCREEN_HEIGHT,
                .windowFlags = SCREEN_FLAGS,
                .renderFlags = RENDER_FLAGS,
                .window = NULL,
                .renderer = NULL,
        },
        {
                .sprite = NULL,
                .animIndex = 0,
                .animStateTime = 0.f,
        },
        {
                .leftDown = false,
                .rightDown = false
        },
        .map = NULL,
        .maplumps = NULL,
        .currentMap = -1,
        .assets = NULL
};

// ----------------------------------------------------------------------------

void init();
void initAssets();
void events();
void update();
void updateTimer();
void render();
void shutdown();

void showMapSelectDialog();

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

    game.maplumps = initMapLumps(10);
    readWadMaps("data/doom1.wad", game.maplumps);

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
            // System ---------------------------------
            case SDL_QUIT: game.running = false; break;
            // Keyboard -------------------------------
            case SDL_KEYDOWN: {
                // ...
            } break;
            case SDL_KEYUP: {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    game.running = false;
                }
                if (event.key.keysym.sym == SDLK_SPACE) {
                    game.graphics.animIndex = (game.graphics.animIndex + 1) % game.assets->numAnimations;
                }
                if (event.key.keysym.sym == SDLK_TAB) {
                    showMapSelectDialog();
                }

                if      (event.key.keysym.sym == SDLK_z) { if (++mapScale > 15) mapScale = 15; }
                else if (event.key.keysym.sym == SDLK_x) { if (--mapScale < 1) mapScale = 1; }

                if (event.key.keysym.sym == SDLK_RETURN) {
                    printf("Camera: (%d, %d)\n", game.view.camera.x, game.view.camera.y);
                    printf("Map extents: min(%d, %d) max(%d, %d) scale = %d\n", mapMinX, mapMinY, mapMaxX, mapMaxY, mapScale);
                }
            } break;
            // Mouse ----------------------------------
            case SDL_MOUSEWHEEL: {
                if (event.wheel.y > 0) { if (--mapScale < 1) mapScale = 1; }
                if (event.wheel.y < 0) { if (++mapScale > 15) mapScale = 15; }
            } break;
            case SDL_MOUSEBUTTONDOWN: {
                if (event.button.button == SDL_BUTTON_LEFT)  game.mouse.leftDown  = true;
                if (event.button.button == SDL_BUTTON_RIGHT) game.mouse.rightDown = true;
            } break;
            case SDL_MOUSEBUTTONUP: {
                if (event.button.button == SDL_BUTTON_LEFT)  game.mouse.leftDown  = false;
                if (event.button.button == SDL_BUTTON_RIGHT) game.mouse.rightDown = false;

                if (event.button.button == SDL_BUTTON_RIGHT) {
                    showMapSelectDialog();
                }
            } break;
            case SDL_MOUSEMOTION: {
                if (game.mouse.leftDown) {
                    game.view.camera.x -= event.motion.xrel;
                    game.view.camera.y -= event.motion.yrel;
                }
            } break;
            default: break;
        }
    }
}

void update() {
    updateTimer();

    const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
    const float speed = (float) (200 * game.timer.delta);

//    if (keyboardState[SDL_SCANCODE_LEFT]) {
//        translateSprite(game.graphics.sprite, -speed, 0.f);
//        game.graphics.sprite->facing = LEFT;
//    } else if (keyboardState[SDL_SCANCODE_RIGHT]) {
//        translateSprite(game.graphics.sprite, speed, 0.f);
//        game.graphics.sprite->facing = RIGHT;
//    }
//    if      (keyboardState[SDL_SCANCODE_UP])   translateSprite(game.graphics.sprite, 0.f, -speed);
//    else if (keyboardState[SDL_SCANCODE_DOWN]) translateSprite(game.graphics.sprite, 0.f,  speed);

    if      (keyboardState[SDL_SCANCODE_LEFT])  game.view.camera.x += speed;
    else if (keyboardState[SDL_SCANCODE_RIGHT]) game.view.camera.x -= speed;

    if      (keyboardState[SDL_SCANCODE_UP])   game.view.camera.y += speed;
    else if (keyboardState[SDL_SCANCODE_DOWN]) game.view.camera.y -= speed;

    if      (keyboardState[SDL_SCANCODE_Q]) rotateSprite(game.graphics.sprite, -speed);
    else if (keyboardState[SDL_SCANCODE_E]) rotateSprite(game.graphics.sprite,  speed);
    else if (keyboardState[SDL_SCANCODE_W]) game.graphics.sprite->angle = 0.0;

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

    if (game.map != NULL) {
        // Draw linedefs
        SDL_SetRenderDrawColor(game.screen.renderer, 0xFF, 0x00, 0x00, 0xFF);
        for (int i = 0; i < game.map->numLinedefs; ++i) {
            int x1 = game.map->vertices[game.map->linedefs[i].v1].x / mapScale - game.view.camera.x;
            int y1 = game.map->vertices[game.map->linedefs[i].v1].y / mapScale - game.view.camera.y;
            int x2 = game.map->vertices[game.map->linedefs[i].v2].x / mapScale - game.view.camera.x;
            int y2 = game.map->vertices[game.map->linedefs[i].v2].y / mapScale - game.view.camera.y;
            SDL_RenderDrawLine(game.screen.renderer, x1, y1, x2, y2);
        }
        SDL_SetRenderDrawColor(game.screen.renderer, 0xFF, 0xFF, 0xFF, 0xFF);

        // Draw things
        SDL_Rect rect;
        for (int i = 0; i < game.map->numThings; ++i) {
            const int size = 6;
            rect = (SDL_Rect) {
                    .x = (game.map->things[i].x / mapScale) - (size / 2) - game.view.camera.x,
                    .y = (game.map->things[i].y / mapScale) - (size / 2) - game.view.camera.y,
                    .w = size, .h = size
            };
            SDL_SetRenderDrawColor(game.screen.renderer, 0xFF, 0xFF, 0x00, 0xFF);
            SDL_RenderFillRect(game.screen.renderer, &rect);
            SDL_SetRenderDrawColor(game.screen.renderer, 0x00, 0xFF, 0x00, 0xFF);
            SDL_RenderDrawRect(game.screen.renderer, &rect);
        }
        SDL_SetRenderDrawColor(game.screen.renderer, 0xFF, 0xFF, 0xFF, 0xFF);

        // Draw map bounds rect
        rect = (SDL_Rect) {
                .x = (mapMinX / mapScale) - game.view.camera.x,
                .y = (mapMinY / mapScale) - game.view.camera.y,
                .w = (mapMaxX - mapMinX) / mapScale,
                .h = (mapMaxY - mapMinY) / mapScale
        };
        SDL_SetRenderDrawColor(game.screen.renderer, 0x00, 0x00, 0xFF, 0xFF);
        SDL_RenderDrawRect(game.screen.renderer, &rect);

        // Draw map bounds rect min x,y
        const int size = 10;
        rect = (SDL_Rect) {
                .x = (mapMinX / mapScale) - (size / 2) - game.view.camera.x,
                .y = (mapMinY / mapScale) - (size / 2) - game.view.camera.y,
                .w = size, .h = size
        };
        SDL_SetRenderDrawColor(game.screen.renderer, 0x00, 0x00, 0xFF, 0xFF);
        SDL_RenderFillRect(game.screen.renderer, &rect);

        // Draw map bounds rect center
        rect = (SDL_Rect) {
                .x = rect.x + (((mapMaxX - mapMinX) / mapScale) / 2),
                .y = rect.y + (((mapMaxY - mapMinY) / mapScale) / 2),
                .w = size, .h = size
        };
        SDL_SetRenderDrawColor(game.screen.renderer, 0xAA, 0x00, 0xAA, 0xFF);
        SDL_RenderFillRect(game.screen.renderer, &rect);

        SDL_SetRenderDrawColor(game.screen.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    }

    SDL_RenderPresent(game.screen.renderer);
}

void shutdown() {
    SDL_DestroyRenderer(game.screen.renderer);
    SDL_DestroyWindow(game.screen.window);

    destroyAssets(game.assets);
    freeMap(game.map);
    freeMapLumps(game.maplumps);
    if (msgBoxButtons != NULL) {
        free(msgBoxButtons);
        msgBoxButtons = NULL;
    }

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

// ------------------------------------------------------------------

void showMapSelectDialog() {
    if (msgBoxButtons != NULL) {
        free(msgBoxButtons);
        msgBoxButtons = NULL;
    }
    msgBoxButtons = (SDL_MessageBoxButtonData *) calloc((size_t) game.maplumps->count, sizeof(SDL_MessageBoxButtonData));
    for (int i = 0; i < game.maplumps->count; ++i) {
        msgBoxButtons[i] = (SDL_MessageBoxButtonData) {
                .flags = 0,
                .buttonid = i,
                .text = game.maplumps->lumps[i].name
        };
    }

    const SDL_MessageBoxData messageBoxData = {
            .flags = 0,
            .window = game.screen.window,
            .title = "Map Picker",
            .message = "Pick a map to view",
            .numbuttons = game.maplumps->count,
            .buttons = msgBoxButtons,
            .colorScheme = NULL
    };
    if (SDL_ShowMessageBox(&messageBoxData, &game.currentMap) == 0) {
        printf("\nMap lump selected: %d - %.*s",
               game.currentMap, 8, game.maplumps->lumps[game.currentMap].name);

        if (game.map != NULL) {
            freeMap(game.map);
        }

        game.map = (map_t *) calloc(1, sizeof(map_t));
        loadWadMap("data/doom1.wad", &game.maplumps->lumps[game.currentMap], game.map);

        // Determine map bounds and shift camera so map is in view
        mapMinX = INT32_MAX;
        mapMinY = INT32_MAX;
        mapMaxX = INT32_MIN;
        mapMaxY = INT32_MIN;
        for (int i = 0; i < game.map->numVertexes; ++i) {
            if (game.map->vertices[i].x < mapMinX) mapMinX = game.map->vertices[i].x;
            if (game.map->vertices[i].y < mapMinY) mapMinY = game.map->vertices[i].y;
            if (game.map->vertices[i].x > mapMaxX) mapMaxX = game.map->vertices[i].x;
            if (game.map->vertices[i].y > mapMaxY) mapMaxY = game.map->vertices[i].y;
        }

        int minx = (mapMinX / mapScale) + (SCREEN_WIDTH  / 2) - (((mapMaxX - mapMinX) / mapScale) / 2);
        int miny = (mapMinY / mapScale) + (SCREEN_HEIGHT / 2) - (((mapMaxY - mapMinY) / mapScale) / 2);
        game.view.camera.x = minx;
        game.view.camera.y = miny;

        printf("min (%d, %d)  max(%d, %d)\n", mapMinX, mapMinY, mapMaxX, mapMaxY);
    }
}
