#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include <SDL_log.h>

#include "json/json.h"

#include "assets.h"
#include "common.h"

const char *keyword_spritesheets = "spritesheets";
const char *keyword_animations = "animations";

void loadSpritesheets(Assets *assets, json_value *jsonValue, SDL_Renderer *renderer);
void loadAnimations(Assets *assets, json_value *jsonValue);

Assets *loadAssets(const char *assetFilePath, SDL_Renderer *renderer) {
    assert(assetFilePath != NULL);

    char *assetsJson = readFileToString(assetFilePath);
    if (assetsJson == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to open asset file '%s'", assetFilePath);
        exit(1);
    } else {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Loading assets from '%s'...", assetFilePath);
    }

    Assets *assets = (Assets *) calloc(1, sizeof(Assets));
    assets->path = assetFilePath;
    {
        json_value *rootJson = json_parse(assetsJson, strlen(assetsJson));
        assert(rootJson->type == json_object);
        assert(rootJson->u.object.length != 0);

        for (int i = 0; i < rootJson->u.object.length; ++i) {
            char *propertyName = rootJson->u.object.values[i].name;
            json_value *propertyValue = rootJson->u.object.values[i].value;

            if (strcmp(propertyName, keyword_spritesheets) == 0) {
                loadSpritesheets(assets, propertyValue, renderer);
            }
            else if (strcmp(propertyName, keyword_animations) == 0) {
                loadAnimations(assets, propertyValue);
            }
            else {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "  Unknown json property '%s'", propertyName);
            }
        }
    }
    free(assetsJson);

    return assets;
}

void loadSpritesheets(Assets *assets, json_value *jsonValue, SDL_Renderer *renderer) {
    assert(assets != NULL && jsonValue != NULL);
    assert(jsonValue->type == json_array);

    size_t numSpritesheets = jsonValue->u.array.length;
    assets->numSpritesheets = numSpritesheets;
    if (numSpritesheets == 0) {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "  No spritesheet definitions found.");
        return;
    } else {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "  Loading %lu spritesheet(s)...", (unsigned long) assets->numSpritesheets);
    }

    assets->spritesheets = (Texture **) calloc(numSpritesheets, sizeof(Texture *));
    for (int i = 0; i < numSpritesheets; ++i) {
        json_value *sheetObject = jsonValue->u.array.values[i];
        assert(sheetObject->type == json_object);

        char *path = NULL;
        char *name = NULL;
        for (int p = 0; p < sheetObject->u.object.length; ++p) {
            char *propertyName = sheetObject->u.object.values[p].name;
            char *propertyValue = sheetObject->u.object.values[p].value->u.string.ptr;

            if      (strcmp(propertyName, "name") == 0) name = propertyValue;
            else if (strcmp(propertyName, "path") == 0) path = propertyValue;
            else {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "    Unknown json property '%s' in spritesheet definition", propertyName);
            }
        }

        Texture *spritesheet = createTextureFromFile(renderer, name, path);
        assets->spritesheets[i] = spritesheet;

        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "    Loaded spritesheet: '%s' @ '%s'", spritesheet->name, spritesheet->path);
    }
}

void loadAnimations(Assets *assets, json_value *jsonValue) {
    assert(assets != NULL && jsonValue != NULL);
    assert(jsonValue->type == json_array);

    size_t numAnimations = jsonValue->u.array.length;
    assets->numAnimations = numAnimations;
    if (numAnimations == 0) {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "  No animation definitions found.");
        return;
    } else {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "  Loading %lu animation(s)...", (unsigned long) assets->numAnimations);
    }

    assets->animations = (Animation **) calloc(numAnimations, sizeof(Animation *));
    for (int i = 0; i < assets->numAnimations; ++i) {
        json_value *animObject = jsonValue->u.array.values[i];
        assert(animObject->type == json_object);

        char *name = NULL;
        char *spritesheet = NULL;
        float frameDuration = 0.15f;
        size_t numKeyframes = 0;
        TextureRegion **keyframes = NULL;
        for (int p = 0; p < animObject->u.object.length; ++p) {
            char *propertyName = animObject->u.object.values[p].name;
            char *propertyValue = animObject->u.object.values[p].value->u.string.ptr;

            if      (strcmp(propertyName, "name")        == 0) name = propertyValue;
            else if (strcmp(propertyName, "duration")    == 0) frameDuration = (float) animObject->u.object.values[p].value->u.dbl;
            else if (strcmp(propertyName, "spritesheet") == 0) spritesheet = propertyValue;
            else if (strcmp(propertyName, "keyframes")   == 0) {
                assert(animObject->u.object.values[p].value->type == json_array);

                Texture *sheetTexture = getSpritesheet(assets, spritesheet);
                if (sheetTexture == NULL) {
                    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to find spritesheet '%s' in assets", name);
                    exit(1);
                }

                numKeyframes = animObject->u.object.values[p].value->u.array.length;
                json_value **keyframesArr = animObject->u.object.values[p].value->u.array.values;

                keyframes = (TextureRegion **) calloc(numKeyframes, sizeof(TextureRegion *));
                for (int k = 0; k < numKeyframes; ++k) {
                    int x,y,w,h;
                    sscanf(keyframesArr[k]->u.string.ptr, "%d %d %d %d", &x, &y, &w, &h);
                    keyframes[k] = createTextureRegion(sheetTexture, x, y, w, h);
                }
            }
            else {
                SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "    Unknown json property '%s' in animation definition", propertyName);
            }
        }

        Animation *animation = createAnimationFromArray(frameDuration, (unsigned int) numKeyframes, keyframes);
        animation->name = name;
        assets->animations[i] = animation;

        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "    Loaded animation: '%s' @ '%s'", animation->name, spritesheet);
    }
}

Texture *getSpritesheet(Assets *assets, const char *name) {
    assert(assets != NULL && name != NULL);

    Texture *spritesheet = NULL;
    for (int i = 0; i < assets->numSpritesheets; ++i) {
        if (strcmp(assets->spritesheets[i]->name, name) == 0) {
            spritesheet = assets->spritesheets[i];
            break;
        }
    }
    return spritesheet;
}

Animation *getAnimation(Assets *assets, const char *name) {
    assert(assets != NULL && name != NULL);

    Animation *animation = NULL;
    for (int i = 0; i < assets->numSpritesheets; ++i) {
        if (strcmp(assets->animations[i]->name, name) == 0) {
            animation = assets->animations[i];
            break;
        }
    }
    return animation;
}

void destroyAssets(Assets *assets) {
    assert(assets != NULL);
    if (assets->spritesheets != NULL) {
        for (int i = 0; i < assets->numSpritesheets; ++i) {
            destroyTexture(assets->spritesheets[i]);
        }
        free(assets->spritesheets);
    }
    if (assets->animations != NULL) {
        for (int i = 0; i < assets->numAnimations; ++i) {
            destroyAnimation(assets->animations[i]);
        }
        free(assets->animations);
    }
    free(assets);
}
