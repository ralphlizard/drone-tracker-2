#include "ofApp.h"

void ofApp::setup(){
    int w = ofGetScreenWidth(); int h = ofGetScreenHeight();
    ofSetWindowShape(1400, 500);
    ofSetFullscreen(false);
    
    camW = 1720; camH = 1080;
    cam.setup(camW,camH,0);
    
    vid.setDeviceID(0);
    vid.initGrabber(camW, camH);
    
    artk.setup(camW, camH);
    artk.setThreshold(0);

    ofDisableSmoothing();
     m1.loadImage("joy.png");
     
     scale = 0.4;
}

void ofApp::update(){
    vid.update();
    
     cam.update();
    
     cvImage.setFromPixels(cam.image.getPixelsRef().getChannel(0));
     cvImage.threshold(mouseX);
     cvImage.invert();
     
     contourFinder.setMinArea(300);
     contourFinder.setMaxArea(camW*camH);
     contourFinder.setSortBySize(true);
     
     contourFinder.findContours(ofxCv::toCv(cvImage));
    
    colorImage.setFromPixels(vid.getPixels(), camW, camH);
    grayImage = colorImage;
    
    //grayImage.mirror(false, true); //do this if using mirror
    grayImage.threshold(mouseX);
    artk.update(grayImage.getPixels());
}

void ofApp::draw(){
     // draw raw camera feed and thresholded camera feed
     
     ofSetColor(255,255,255);
     cam.image.draw(camW * scale, 0, camW * scale, camH * scale) ;
     cvImage.draw(0, 0, camW * scale, camH * scale);

//    artk.draw(0, 0, camW * scale, camH * scale);

    int numDetected = artk.getNumDetectedMarkers();
    ofScale (scale, scale, scale);
    
    for(int i=0; i<numDetected; i++) {
        ofPoint pos = artk.getDetectedMarkerCenter(i);
//        float dir = artk.getOrientationQuaternion(i).x();
        float dir = artk.getDetectedMarkerDirection(i);
        vector<ofPoint> corners;
        artk.getDetectedMarkerCorners(i, corners);
        ofDrawBitmapString(ofToString(artk.getMarkerID(i)) + " " + ofToString(corners[0].x) + "," + ofToString(corners[0].y) + " " + ofToString(corners[1].x) + "," + ofToString(corners[1].y) + " " + ofToString(corners[2].x) + "," + ofToString(corners[2].y) + " " + ofToString(corners[3].x) + "," + ofToString(corners[3].y), pos.x, pos.y);
        ofSetColor(255,0,0);
        if (dir == 0)
            ofLine(pos.x, pos.y, pos.x, pos.y + 60);
        if (dir == 1)
            ofLine(pos.x, pos.y, pos.x + 60, pos.y);
        if (dir == 2)
            ofLine(pos.x, pos.y, pos.x, pos.y - 60);
        if (dir == 3)
            ofLine(pos.x, pos.y, pos.x - 60, pos.y);
        ofSetColor(255, 255, 255);
        ofImage drone;
        drone.setFromPixels(vid.getPixelsRef());
        drone.crop(pos.x - 60, pos.y - 60, 120, 120);
        drone.rotate90(dir);
        drone.draw(i * 120, 0);
    }
    
    m1.draw(0, 0);
    
     for(int p = 0; p < contourFinder.getPolylines().size(); p++) {
     
     // get and draw the current polyline
     
     ofSetColor(255,0,0);
     ofPolyline pl = contourFinder.getPolyline(p).getResampledBySpacing(1).getSmoothed(20);
     pl.draw();
     
     // find changes of angle of each segment of the contour
     
     int segmentSize = 15;
     
     for(int i = 0; i < pl.size(); i += segmentSize) {
     
     // draw where this segment of the contour starts
     
     ofSetColor(0,0,255);
     ofPoint tpos = pl.getVertices()[i];
     ofCircle(tpos.x, tpos.y, 2);
     
     ofSetColor(0,255,0);
     
     if(pl.size()-i >= segmentSize) {
     
     // find total change in direction over these points
     
     ofVec3f diff1 =  (pl.getVertices()[i+0]
     -pl.getVertices()[i+1]);
     ofVec3f diff2 =  (pl.getVertices()[i+segmentSize-2]
     -pl.getVertices()[i+segmentSize-1]);
     diff1.normalize();
     diff2.normalize();
     float a1 = atan(diff1.y/diff1.x);
     float a2 = atan(diff2.y/diff2.x);
     float totalChange = (a1 - a2)*57.2957795;
     
     // if the change was big enough, we found a triangle edge
     
     if((diff1-diff2).length() > 1) {
     ofSetColor(0,255,255);
     ofPoint pos = pl.getVertices()[i+4];
     ofCircle(pos.x, pos.y, 5);
     }
     
     // draw the angles (debug)
     
     ofPoint pos = pl.getVertices()[i];
     ofSetColor(0,255,0);
     ofLine(pos.x, pos.y, pos.x+diff1.x*10, pos.y+diff1.y*10);
     ofSetColor(255,0,0);
     ofLine(pos.x, pos.y, pos.x+diff2.x*10, pos.y+diff2.y*10);
     
            }
     
        }
     
     }

}
