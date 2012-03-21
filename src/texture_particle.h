//
//  texture_particle.h
//  emptyExample
//
//  Created by Tetsuro Kato on 12/03/19.
//  Copyright 2012年 KATO Tetsuro. All rights reserved.
//

#ifndef emptyExample_texture_particle_h
#define emptyExample_texture_particle_h

#include "ofMain.h"

class TextureParticle {
    
public:
    static const int texSize = 10;
    static float depthAmplify;
    ofPoint position;
//    ofTexture texture;
    void update(float vol);
    static ofPixels *pPixels;
    static ofTexture *pTexture;
    static float sizeAdjuster;
    void draw(ofTexture &tex, float space);
};

#endif
