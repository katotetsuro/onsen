#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "texture_particle.h"

#define NUM_TEXTURES 3072

class testApp : public ofBaseApp{
private:
    static const string GUI_CAM_DIST;
    static const string GUI_SPACE;
    static const string GUI_DEPTH_AMPLIFY;
    static const string GUI_SIZE_ADJUSTER;
    float textureSpace;
    void initValues();

	public:
		void setup();
		void update();
		void draw();
    void exit();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    void guiEvent(ofxUIEventArgs &e);
        
    ofVideoGrabber cam;
    ofEasyCam easyCam;
    ofxUICanvas *gui;
    ofPixels texturePixels;
    
    TextureParticle textures[NUM_TEXTURES];
    
    void audioIn(float * input, int bufferSize, int nChannels); 
	
    vector <float> left;
    vector <float> right;
    vector <float> volHistory;
    
    int 	bufferCounter;
    int 	drawCounter;
    
    float smoothedVol;
    float scaledVol;
    float curVol;
    
    ofSoundStream soundStream;
};
