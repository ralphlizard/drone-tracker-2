#pragma once

#include "ofMain.h"
#include "ImageFromCamera.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxARToolkitPlus.h"
#include "ofxJSONElement.h"

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void mouseReleased(int x, int y, int button);
    void record();
    
    int camW, camH;
    int closeSize;
    bool recording;
    float scale;
    float lastEventTime;
    float timeInterval;
    
    ImageFromCamera cam;
    ofVideoGrabber vid;
    
    ofImage m1;
    ofSoundPlayer sound;
    ofxCvGrayscaleImage cvImage;
    ofxCv::ContourFinder contourFinder;
    
    ofxARToolkitPlus artk;
    ofxJSONElement data;

    ofxCvColorImage colorImage;
    ofxCvGrayscaleImage grayImage;
};
