#ifndef SERAPH_ANIMATION_H
#define SERAPH_ANIMATION_H

#include "texture_region.h"

enum PlayMode { NORMAL, REVERSED, LOOP, LOOP_REVERSED, LOOP_PINGPONG };

typedef struct Animation {
    enum PlayMode playMode;
    unsigned int numKeyFrames;
    unsigned int lastFrameIndex;
    float lastStateTime;
    float frameDuration;
    float animationDuration;
    TextureRegion **keyframes;
} Animation;

Animation *createAnimation(float frameDuration, unsigned int numKeyFrames, ...);
TextureRegion *getAnimationKeyFrame(Animation *animation, float stateTime);
int getAnimationKeyFrameIndex(Animation *animation, float stateTime);
void destroyAnimation(Animation *animation);

#endif //SERAPH_ANIMATION_H
