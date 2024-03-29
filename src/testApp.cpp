#include "testApp.h"
//#include <opencv2/opencv.hpp>
//-------------------------------------------------------t-------
void testApp::setup()

{
    ofSetLogLevel(OF_LOG_VERBOSE);

    // enable depth->video image calibration
    kinect.setRegistration(true);

    kinect.init();
    //kinect.init(true); // shows infrared instead of RGB video image
    //kinect.init(false, false); // disable video image (faster fps)

    kinect.open();		// opens first available kinect
    //kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
    //kinect.open("A00362A08602047A");	// open a kinect using it's unique serial #

#ifdef USE_TWO_KINECTS
    kinect2.init();
    kinect2.open();
#endif

    //colorImg.allocate(kinect.width, kinect.height);
    //grayImage.allocate(kinect.width, kinect.height);
    //grayThreshNear.allocate(kinect.width, kinect.height);
    //grayThreshFar.allocate(kinect.width, kinect.height);

    // zero the tilt on startup
    angle = -2;
    kinect.setCameraTiltAngle(angle);

    nearThreshold = 500;
    farThreshold = 920;

    kinect.setDepthClipping( nearThreshold, farThreshold);

    bThreshWithOpenCV = true;

    ofSetFrameRate(60);
    //    cv::waitKey(10);
     //grayMat= cv::zeros(480, 640, CV_8UC1);

    // start from the front
    bDrawPointCloud = false;
    grayOfImage.allocate(480, 640, OF_IMAGE_GRAYSCALE);
    colorOfImage.allocate(480,640, OF_IMAGE_COLOR);
    grayMat.create(480, 640, CV_8UC1);
    temp1.create(480, 640, CV_8UC1);
    temp2.create(480,640, CV_8UC1);
    depthThresh.create(480,640, CV_8UC1);

    colorMat.create(480, 640, CV_8UC3);

    //grayMat = imread("/home/arpit/Pictures/1.png");
    //imshow("hello", grayMat);
}

//--------------------------------------------------------------
void testApp::update()
{

    ofBackground(100, 100, 100);

    kinect.update();

    // there is a new frame and we are connected
    if(kinect.isFrameNew())
    {

        // load grayscale depth image from the kinect source
//        grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height, OF_IMAGE_GRAYSCALE);
   		grayMat=Mat(cv::Size(kinect.width, kinect.height), CV_8UC1, kinect.getDepthPixels());


        //grayMat = cv::Mat::zeros(480,640,CV_8UC1);
        //imread("/home/arpit/Pictures/1.png");

        threshold(grayMat, temp1, 0, 255, 3);
        threshold(temp1, temp2, 254, 255, 4);
        threshold(temp2, depthThresh, 0, 255, 0);


        if(bThreshWithOpenCV)
        {

            blur( depthThresh, depthThresh, cv::Size(3,3) );

    /*        findContours( depthThresh, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0,0) );
            contoursObb.resize(contours.size());

            for(size_t i=0; i<contours.size() ; i++)
            {
                contoursObb[i] = minAreaRect(contours[i]);

                if(contoursObb[i].size() > 100)
               {
                   contoursObb[i].points(obbPoints);
                   for(int i=0; i<4 ; i++)
                   {
                      obbPoints[i].
                   }
               }

            }*/


        }

//
//		// we do two thresholds - one for the far plane and one for the near plane
//		// we then do a cvAnd to get the pixels which are a union of the two thresholds
//		if(bThreshWithOpenCV) {
//			grayThreshNear = grayImage;
//			grayThreshFar = grayImage;
//			grayThreshNear.threshold(nearThreshold, true);
//			grayThreshFar.threshold(farThreshold);
//			cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
//		}

        // update the cv images
//        grayImage.flagImageChanged();

        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
//        contourFinder.findContours(grayImage, 40, (kinect.width*kinect.height)/2, 20, false);

        grayOfImage.setFromPixels((unsigned char *)IplImage(depthThresh).imageData, depthThresh.size().width, depthThresh.size().height, OF_IMAGE_GRAYSCALE);


    }

#ifdef USE_TWO_KINECTS
    kinect2.update();
#endif
}

//--------------------------------------------------------------
void testApp::draw()
{

    ofSetColor(255, 255, 255);

    if(bDrawPointCloud)
    {
        easyCam.begin();
        drawPointCloud();
        easyCam.end();
    }
    else
    {
        // draw from the live kinect
        kinect.drawDepth(10, 10, 400, 300);
        kinect.draw(420, 10, 400, 300);
        //imshow("kinect Data",grayMat);
        //waitKey(1);
        grayOfImage.draw(10, 320, 400, 300);
        //contourFinder.draw(10, 320, 400, 300);

#ifdef USE_TWO_KINECTS
        kinect2.draw(420, 320, 400, 300);
#endif
    }

    // draw instructions
    ofSetColor(255, 255, 255);
    stringstream reportStream;
    reportStream << "accel is: " << ofToString(kinect.getMksAccel().x, 2) << " / "
    << ofToString(kinect.getMksAccel().y, 2) << " / "
    << ofToString(kinect.getMksAccel().z, 2) << endl
    << "press p to switch between images and point cloud, rotate the point cloud with the mouse" << endl
    << "using threshold = " << bThreshWithOpenCV <<" (press spacebar)" << endl
    << "set near threshold " << nearThreshold << " (press: A S) (precise: a s)" << endl
    << "set far threshold " << farThreshold //<< " (press: Z X) (precise: z x) num blobs found " << contourFinder.nBlobs
    << ", fps: " << ofGetFrameRate() << endl
    << "press c to close the connection and o to open it again, connection is: " << kinect.isConnected() << endl
    << "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl
    << "press 1-5 & 0 to change the led mode (mac/linux only)" << endl;
    ofDrawBitmapString(reportStream.str(),20,652);
}

void testApp::drawPointCloud()
{
    int w = 640;
    int h = 480;
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_POINTS);
    int step = 2;
    for(int y = 0; y < h; y += step)
    {
        for(int x = 0; x < w; x += step)
        {
            if(kinect.getDistanceAt(x, y) > 0)
            {
                mesh.addColor(kinect.getColorAt(x,y));
                mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
            }
        }
    }
    glPointSize(3);
    ofPushMatrix();
    // the projected points are 'upside down' and 'backwards'
    ofScale(1, -1, -1);
    ofTranslate(0, 0, -1000); // center the points a bit
    glEnable(GL_DEPTH_TEST);
    mesh.drawVertices();
    glDisable(GL_DEPTH_TEST);
    ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::exit()
{
    //kinect.setCameraTiltAngle(0); // zero the tilt on exit
    kinect.close();

#ifdef USE_TWO_KINECTS
    kinect2.close();
#endif
}

//--------------------------------------------------------------
void testApp::keyPressed (int key)
{
    switch (key)
    {
    case ' ':
        bThreshWithOpenCV = !bThreshWithOpenCV;
        break;

    case'p':
        bDrawPointCloud = !bDrawPointCloud;
        break; // TODO : remove this point cloud

        //thresholding the depth
        //by a factor of 10mm
    case 'S':
        farThreshold +=10;
        if (farThreshold > 4000) farThreshold = 4000;
        kinect.setDepthClipping( nearThreshold, farThreshold);
        break;

    case 'A':
        if (farThreshold > nearThreshold + 10) farThreshold -=10;
        kinect.setDepthClipping( nearThreshold, farThreshold);
        break;

    case 'X':
        if (nearThreshold < farThreshold -10) nearThreshold +=10;;
        kinect.setDepthClipping( nearThreshold, farThreshold);
        break;

    case 'Z':
        nearThreshold -=10;
        if (nearThreshold <= 500) nearThreshold = 500;
        kinect.setDepthClipping( nearThreshold, farThreshold);
        break;

        //by a factor of 1mm(precise)
    case 's':
        farThreshold++;
        if (farThreshold > 4000) farThreshold = 4000;
        kinect.setDepthClipping( nearThreshold, farThreshold);
        break;

    case 'a':
        if (farThreshold > nearThreshold ) farThreshold--;
        kinect.setDepthClipping( nearThreshold, farThreshold);
        break;

    case 'x':
        if (nearThreshold < farThreshold) nearThreshold++;;
        kinect.setDepthClipping( nearThreshold, farThreshold);
        break;

    case 'z':
        nearThreshold --;
        if (nearThreshold <= 500) nearThreshold = 500;
        kinect.setDepthClipping( nearThreshold, farThreshold);
        break;

    case 'w':
        kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
        break;

    case 'o':
        kinect.setCameraTiltAngle(angle); // go back to prev tilt
        kinect.open();
        break;

    case 'c':
        kinect.setCameraTiltAngle(0); // zero the tilt
        kinect.close();
        break;

    case '1':
        kinect.setLed(ofxKinect::LED_GREEN);
        break;

    case '2':
        kinect.setLed(ofxKinect::LED_YELLOW);
        break;

    case '3':
        kinect.setLed(ofxKinect::LED_RED);
        break;

    case '4':
        kinect.setLed(ofxKinect::LED_BLINK_GREEN);
        break;

    case '5':
        kinect.setLed(ofxKinect::LED_BLINK_YELLOW_RED);
        break;

    case '0':
        kinect.setLed(ofxKinect::LED_OFF);
        break;

    case OF_KEY_UP:
        angle++;
        if(angle>30) angle=30;
        kinect.setCameraTiltAngle(angle);
        break;

    case OF_KEY_DOWN:
        angle--;
        if(angle<-30) angle=-30;
        kinect.setCameraTiltAngle(angle);
        break;
    }
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{}
