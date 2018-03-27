#include <assert.h>
#include <stdarg.h>

#include "animation.h"
#include "common.h"

Animation *createAnimation(float frameDuration, unsigned int numKeyFrames, ...) {
    TextureRegion **keyframes = (TextureRegion **) calloc(numKeyFrames, sizeof(TextureRegion));
    va_list args;
    va_start(args, numKeyFrames);
    for (int i = 0; i < numKeyFrames; i++) {
        keyframes[i] = va_arg(args, TextureRegion*);
    }
    va_end(args);

    return createAnimationFromArray(frameDuration, numKeyFrames, keyframes);
}

Animation *createAnimationFromArray(float frameDuration, unsigned int numKeyFrames, TextureRegion *keyframes[]) {
    Animation *animation = (Animation *) calloc(1, sizeof(Animation));
    animation->playMode          = LOOP;
    animation->numKeyFrames      = numKeyFrames;
    animation->lastFrameIndex    = 0;
    animation->lastStateTime     = 0.f;
    animation->frameDuration     = frameDuration;
    animation->animationDuration = frameDuration * numKeyFrames;
    animation->keyframes         = keyframes;
    return animation;
}

TextureRegion *getAnimationKeyFrame(Animation *animation, float stateTime) {
    assert(animation != NULL);
    int frameIndex = getAnimationKeyFrameIndex(animation, stateTime);
    return animation->keyframes[frameIndex];
}

int getAnimationKeyFrameIndex(Animation *animation, float stateTime) {
    assert(animation != NULL);

    if (animation->numKeyFrames == 1) return 0;

    unsigned int frameIndex = (unsigned int) (stateTime / animation->frameDuration);
    switch (animation->playMode) {
        case NORMAL:   frameIndex = MIN(animation->numKeyFrames - 1, frameIndex);     break;
        case REVERSED: frameIndex = MAX(animation->numKeyFrames - frameIndex - 1, 0); break;
        case LOOP:     frameIndex = frameIndex % animation->numKeyFrames;             break;
        case LOOP_REVERSED: {
            frameIndex = animation->numKeyFrames - (frameIndex % animation->numKeyFrames) - 1;
        } break;
        case LOOP_PINGPONG: {
            frameIndex = frameIndex % ((animation->numKeyFrames * 2) - 2);
            if (frameIndex >= animation->numKeyFrames) {
                frameIndex = animation->numKeyFrames - 2 - (frameIndex - animation->numKeyFrames);
            }
        } break;
    }

    animation->lastFrameIndex = frameIndex;
    animation->lastStateTime = stateTime;

    return frameIndex;
}

void destroyAnimation(Animation *animation) {
    assert(animation != NULL && animation->keyframes != NULL);

    free(animation->keyframes);
    free(animation);
    animation = NULL;
}
