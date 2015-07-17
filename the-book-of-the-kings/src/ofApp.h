#pragma once

#include "ofMain.h"
#include "ImageFromCamera.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxARToolkitPlus.h"

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    int camW, camH;
    int closeSize;
    float scale;
    
    ImageFromCamera cam;
    ofVideoGrabber vid;
    
    ofImage m1;
    ofxCvGrayscaleImage cvImage;
    ofxCv::ContourFinder contourFinder;
    
    ofxARToolkitPlus artk;
    
    ofxCvColorImage colorImage;
    ofxCvGrayscaleImage grayImage;
};
