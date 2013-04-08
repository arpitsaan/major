#pragma once

#include "ofMain.h"
//#include "ofxOpenCv.h"
#include "ofxKinect.h"

#include<iostream>
#include <opencv2/opencv.hpp>


using namespace cv;
// uncomment this to read from two kinects simultaneously
//#define USE_TWO_KINECTS

class testApp : public ofBaseApp {
public:

	void setup();
	void update();
	void draw();
	void exit();

	void drawPointCloud();

	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);

	ofxKinect kinect;

#ifdef USE_TWO_KINECTS
	ofxKinect kinect2;
#endif
/*
	ofxCvColorImage colorImg;

	ofxCvGrayscaleImage grayImage; // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar; // the far thresholded image

    ofxCvContourFinder contourFinder;
*/

    ofImage grayOfImage;
    ofImage colorOfImage;

    Mat grayMat;

    Mat temp1;
    Mat temp2;

    Mat depthThresh;
    Mat colorMat;

    vector <vector <cv::Point> > contours;
    vector <Vec4i> hierarchy;

    vector <cv::RotatedRect> contoursObb;
    Point2f obbPoints[4];

	bool bThreshWithOpenCV;
	bool bDrawPointCloud;

	int nearThreshold;
	int farThreshold;

	int angle;

	int mousex;
	int mousey;
	int mouseDepth;

	// used for viewing the point cloud
	ofEasyCam easyCam;
};
