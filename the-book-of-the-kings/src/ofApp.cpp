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
    sound.loadSound("shutter.wav");
     
    scale = 0.4;
    timeInterval = 5;
    closeSize = 120;
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
    
    //record on interval
    if (ofGetElapsedTimef() - lastEventTime > timeInterval && recording)
    {
        lastEventTime = ofGetElapsedTimef();
        record();
    }
}

void ofApp::draw(){
     // draw raw camera feed and thresholded camera feed
     
     ofSetColor(255,255,255);
     cam.image.draw(camW * scale, 0, camW * scale, camH * scale) ;
     cvImage.draw(0, 0, camW * scale, camH * scale);

    //ARTK DEBUGGING STUFF
    int numDetected = artk.getNumDetectedMarkers();
    ofScale (scale, scale, scale);
    
    for(int i=0; i<numDetected; i++) {
        ofPoint pos = artk.getDetectedMarkerCenter(i);
        // float dir = artk.getOrientationQuaternion(i).x();
        // SWITCH ME OUT COACH
        float dir = artk.getDetectedMarkerDirection(i);
        int id = artk.getMarkerID(i);
        ofImage drone;

        untimedData[ofToString(id)]["position"] = ofToString(pos.x) + ", " + ofToString(pos.y);
        untimedData[ofToString(id)]["rotation"] = ofToString(dir);

        drone.setFromPixels(vid.getPixelsRef());
        drone.crop(pos.x - closeSize /2, pos.y - closeSize / 2, closeSize, closeSize);
        drone.draw(i * 120, 0);
    }

    
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

void ofApp::mouseReleased(int x, int y, int button){
    if (recording)
        record();
    else
        recording = true;
}

void ofApp::record(){
    ofFile newfile(ofToDataPath("coordinates.json"), ofFile::WriteOnly);
    string time = ofToString(ofGetElapsedTimef());
    for(Json::ValueIterator i = untimedData.begin() ; i != untimedData.end(); i++) {
        string id = i.key().asString();
        data[id][time]["position"] = untimedData[id]["position"];
        data[ofToString(id)][time]["rotation"] = untimedData[id]["rotation"];
        cout << data;
    }
    if (data != ofxJSONElement::null)
        newfile << data;
    sound.play();
}

