#include "testApp.h"

const string testApp::GUI_CAM_DIST = "cam_distance";
const string testApp::GUI_SPACE = "texture_space";
const string testApp::GUI_DEPTH_AMPLIFY = "depth_amplify";
const string testApp::GUI_SIZE_ADJUSTER = "size_adjuster";

//--------------------------------------------------------------
void testApp::setup(){
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    glEnable(GL_DEPTH_TEST);
    ofSetWindowShape(1280, 720);
    ofSetLogLevel(OF_LOG_NOTICE);
    
    cam.initGrabber(640, 480);
    cam.setAnchorPercent(0.5, 0.5);
    easyCam.setDistance(100);
    texturePixels.allocate(640, 480, 3);
    for (int i=0; i<NUM_TEXTURES; ++i) {
        textures[i].position.x = (i % 64) * 10;
        textures[i].position.y = (i / 64) * 10;
    }
    textureSpace = 15.f;
    
    gui = new ofxUICanvas(0, 0, 400, 600);
    gui->addWidgetDown(new ofxUILabel(GUI_CAM_DIST, OFX_UI_FONT_MEDIUM));
    gui->addWidgetRight(new ofxUISlider(95, 16, 0, 2000, 100, GUI_CAM_DIST));
    gui->addWidgetDown(new ofxUILabel(GUI_SPACE, OFX_UI_FONT_MEDIUM));
    gui->addWidgetRight(new ofxUISlider(95, 16, 0.0, 100.0, 15.0, GUI_SPACE));
    gui->addWidgetDown(new ofxUILabel(GUI_DEPTH_AMPLIFY, OFX_UI_FONT_MEDIUM));
    gui->addWidgetRight(new ofxUISlider(95, 16, 0.0, 10.0, 15.0, GUI_DEPTH_AMPLIFY));
    gui->addWidgetDown(new ofxUILabel(GUI_SIZE_ADJUSTER, OFX_UI_FONT_MEDIUM));
    gui->addWidgetRight(new ofxUISlider(95, 16, 0.0, 3.0, 15.0, GUI_SIZE_ADJUSTER));
    
    gui->addWidgetDown(new ofxUIFPS(OFX_UI_FONT_MEDIUM));
    gui->loadSettings("GUI/guiSettings.xml");
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    
    initValues();
    
    soundStream.listDevices();
	
	//if you want to set a different device id 
	//soundStream.setDeviceID(0); //bear in mind the device id corresponds to all audio devices, including  input-only and output-only devices.
	
	int bufferSize = 256;
	
	
	left.assign(bufferSize, 0.0);
	right.assign(bufferSize, 0.0);
	volHistory.assign(400, 0.0);
	
	bufferCounter	= 0;
	drawCounter		= 0;
	smoothedVol     = 0.0;
	scaledVol		= 0.0;
    
	soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
}

//--------------------------------------------------------------
void testApp::update(){
    
    //lets scale the vol up to a 0-1 range 
	scaledVol = ofMap(smoothedVol, 0.0, 0.17, 1.0, 10.0, true);
    
	//lets record the volume into an array
	volHistory.push_back( scaledVol );
	
	//if we are bigger the the size we want to record - lets drop the oldest value
	if( volHistory.size() >= 400 ){
		volHistory.erase(volHistory.begin(), volHistory.begin()+1);
	}
    
    cam.update();
    if (cam.isFrameNew()) {
        ofTexture texture = cam.getTextureReference();
        textures[0].pTexture = &texture;
        textures[0].pPixels = &cam.getPixelsRef();
        
        for (int y=0; y<48; ++y) {
            for (int x=0; x<64; ++x) {
                textures[y*64+x].update(smoothedVol);
            }
        }
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(ofColor::black);
    easyCam.begin();
    ofRotateZ(180.f);
    ofTranslate(-ofGetWidth()/2, -ofGetHeight()/2);
    for (int y=0; y<48; ++y) {
        for (int x=0; x<64; ++x) {
            textures[y*64+x].draw(cam.getTextureReference(), textureSpace);
        }
    }
    easyCam.end();
}

//--------------------------------------------------------------
void testApp::exit(){
    gui->saveSettings("GUI/guiSettings.xml");
    delete gui;
}

void testApp::guiEvent(ofxUIEventArgs &e) {
    string selectedWidget = e.widget->getName();
    if(selectedWidget == GUI_CAM_DIST)	
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;    
        int newDistance = slider->getScaledValue();
        easyCam.setDistance(newDistance);
    } else if (selectedWidget == GUI_SPACE) {
        ofxUISlider *slider = (ofxUISlider *) e.widget;    
        float newSpace = slider->getScaledValue();
        textureSpace = newSpace;
    } else if (selectedWidget == GUI_DEPTH_AMPLIFY) {
        ofxUISlider *slider = (ofxUISlider *) e.widget;    
        float newAmplify = slider->getScaledValue();
        textures[0].depthAmplify = newAmplify;
    } else if (selectedWidget == GUI_SIZE_ADJUSTER) {
        ofxUISlider *slider = (ofxUISlider *) e.widget;    
        float newAdjuster = slider->getScaledValue();
        textures[0].sizeAdjuster = newAdjuster;
    }
}

void testApp::initValues() {
    {
        ofxUIWidget *widget = gui->getWidget(GUI_CAM_DIST);
        ofxUISlider *slider = (ofxUISlider *) widget;    
        int newDistance = slider->getScaledValue();
        easyCam.setDistance(newDistance);
    }
    {
        ofxUIWidget *widget = gui->getWidget(GUI_SPACE);
        ofxUISlider *slider = (ofxUISlider *) widget;    
        float newSpace = slider->getScaledValue();
        textureSpace = newSpace;
    }
    {
        ofxUIWidget *widget = gui->getWidget(GUI_DEPTH_AMPLIFY);
        ofxUISlider *slider = (ofxUISlider *) widget;
        float newAmplify = slider->getScaledValue();
        textures[0].depthAmplify = newAmplify;
    }
    {
        ofxUIWidget *widget = gui->getWidget(GUI_SIZE_ADJUSTER);
        ofxUISlider *slider = (ofxUISlider *) widget;
        float newAdjuster = slider->getScaledValue();
        textures[0].sizeAdjuster = newAdjuster;
    }
}

//--------------------------------------------------------------
void testApp::audioIn(float * input, int bufferSize, int nChannels){	
	
	curVol = 0.0;
	
	// samples are "interleaved"
	int numCounted = 0;	
    
	//lets go through each sample and calculate the root mean square which is a rough way to calculate volume	
	for (int i = 0; i < bufferSize; i++){
		left[i]		= input[i*2]*0.5;
		right[i]	= input[i*2+1]*0.5;
        
		curVol += left[i] * left[i];
		curVol += right[i] * right[i];
		numCounted+=2;
	}
	
	//this is how we get the mean of rms :) 
	curVol /= (float)numCounted;
	
	// this is how we get the root of rms :) 
	curVol = sqrt( curVol );
    curVol *= 1000;
	
	smoothedVol *= 0.93;
	smoothedVol += 0.07 * curVol;
	
	bufferCounter++;
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if (key=='s') {
        ofImage capture;
        capture.grabScreen(0,0,ofGetWidth(), ofGetHeight());
        capture.saveImage(ofGetTimestampString()+".jpg");
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}