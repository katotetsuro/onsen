//
//  texture_particle.cpp
//  emptyExample
//
//  Created by Tetsuro Kato on 12/03/19.
//  Copyright 2012年 KATO Tetsuro. All rights reserved.
//

#include "texture_particle.h"

float TextureParticle::depthAmplify = 1.f;
ofPixels * TextureParticle::pPixels = NULL;
ofTexture * TextureParticle::pTexture = NULL;
float TextureParticle::sizeAdjuster = 1.f;

void TextureParticle::update(float vol) {
    int v = 0;
    for (int y=0; y<texSize; ++y) {
        for(int x=0; x<texSize; ++x) {
            ofColor pix = pPixels->getColor(position.x+x, position.y+y);
            v = pix.r*0.299 + pix.g* 0.587 + pix.b*0.114;
        }
    }
    float newZ = (v + vol) * depthAmplify;
    position.z = newZ;
//    position.z = position.z * ofNoise(position.x, position.y, position.z);
}

void TextureParticle::draw(ofTexture &tex, float space) {
    int nx = position.x / texSize;
    int ny = position.y / texSize;
    float offsetX = (nx-1) * space;
    float offsetY = (ny-1) * space;
    float sizeCorrection =  exp(-position.z/1000)*sizeAdjuster;
    tex.bind();
    glBegin(GL_QUADS);
    glTexCoord2f(position.x, position.y);
    glVertex3f(position.x+offsetX-sizeCorrection, position.y+offsetY-sizeCorrection, position.z);
    glTexCoord2f(position.x, position.y+texSize);
    glVertex3f(position.x+offsetX-sizeCorrection, position.y+texSize+offsetY+sizeCorrection, position.z);
    glTexCoord2f(position.x+texSize, position.y+texSize);
    glVertex3f(position.x+texSize+offsetX+sizeCorrection, position.y+texSize+offsetY+sizeCorrection, position.z);
    glTexCoord2f(position.x+texSize, position.y);
    glVertex3f(position.x+texSize+offsetX+sizeCorrection, position.y+offsetY-sizeCorrection, position.z);
    glEnd();
    tex.unbind();  
}
