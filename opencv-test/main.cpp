
//=================================load/save video=================================================
//
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

const int iHues[4][2]={
    {0,179}, //all colors
    {168,179}, //red
    {77,90}, //green
    {103,132} //blue
};

int iColor = 2;

int iLowH = iHues[iColor][0];
int iHighH = iHues[iColor][1];
int iLowS = 140;
int iHighS = 255;
int iLowV = 135;
int iHighV = 255;

int iCloseSize = 30;
int iOpenSize = 4;

int iLastX = -1;
int iLastY = -1;

Mat imgLines;
Mat imgOriginal;
Mat imgHSV;
Mat imgThresholded;

void processFrame(){
    cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
    
    inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
    
    //morphological closing (removes small holes from the foreground)
    dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(iCloseSize, iCloseSize)), Point(-1,-1) );
    erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(iCloseSize, iCloseSize)), Point(-1,-1) );
    
    //morphological opening (removes small objects from the foreground)
    erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(iOpenSize, iOpenSize)) );
    dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(iOpenSize, iOpenSize)) );
    
    //Calculate the moments of the thresholded image
    Moments oMoments = moments(imgThresholded);
    
    double dM01 = oMoments.m01;
    double dM10 = oMoments.m10;
    double dArea = oMoments.m00;
    
    // if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero
    if (dArea > 10000)
    {
        //calculate the position of the ball
        int posX = dM10 / dArea;
        int posY = dM01 / dArea;
        
        if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
        {
            //Draw a red line from the previous point to the current point
            line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0,0,255), 2);
        }
        
        iLastX = posX;
        iLastY = posY;
    }
    
    imgOriginal = imgOriginal + imgLines; //draw the line
    
    imshow("output", imgThresholded);
    
    //edge detection
//    Mat gray, edge, draw;
//    cvtColor(imgOriginal, gray, CV_BGR2GRAY);
//    
//    Canny( gray, edge, 5, 75, 3);
//    
//    edge.convertTo(draw, CV_8U);
//    imshow("out", draw);
    
}

int main(int argc, char* argv[])
{
    const string file = "green-box.mov";
    VideoCapture cap("input/" + file); // open the video file for reading
    
    if ( !cap.isOpened() )  // if not success, exit program
    {
        cout << "Cannot open the video file" << endl;
        return -1;
    }
    
    double dFps = cap.get(CV_CAP_PROP_FPS); //get the frames per seconds of the video
    cout << "Frame per seconds : " << dFps << endl;
    
    double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
    double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
    cout << "Frame Size = " << dWidth << "x" << dHeight << endl;
    Size frameSize(static_cast<int>(dWidth), static_cast<int>(dHeight));
    
    double dFourCC = cap.get(CV_CAP_PROP_FOURCC); //get the original video's codec code
    cout << "FourCC = " << dFourCC << endl;
    
    double dFrameCount = cap.get(CV_CAP_PROP_FRAME_COUNT);
    cout << "Frame count = " << dFrameCount << endl;

    VideoWriter oVideoWriter ("output/" + file, CV_FOURCC('m','p', '4', 'v'), dFps, frameSize, true); //initialize the VideoWriter object
    
    if ( !oVideoWriter.isOpened() ) //if not initialize the VideoWriter successfully, exit the program
    {
        cout << "ERROR: Failed to write the video" << endl;
        return -1;
    }
    
    int iFrames = 0;
    
    //Create a black image with the size as the camera output
    imgLines = Mat::zeros( frameSize, CV_8UC3 );;
    
    namedWindow("output",WINDOW_NORMAL);
    
    while (1)
    {
        bool bSuccess = cap.read(imgOriginal); // read a new frame from video
        
        iFrames++;
        if (iFrames % 25 == 0){
            cout << "Current frame = " << iFrames << endl;
        }
        
        if (!bSuccess) //if not success, break loop
        {
            cout << "No more frames" << endl;
            break;
        }
        processFrame();
        
        oVideoWriter.write(imgOriginal); //writer the frame into the file
        if (waitKey(1) == 27) //wait for 'esc' key press for 5ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }

    }
    
    return 0;
    
}

//=================================Trace Colors=================================================
//
//#include <iostream>
//#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/imgproc/imgproc.hpp"
//
//using namespace cv;
//using namespace std;
//
//const int iHues[4][2]={
//    {0,179}, //all colors
//    {168,179}, //red
//    {77,90}, //green
//    {103,132} //blue
//};
//
//int iColor = 2;
//
//int iLowH = iHues[iColor][0];
//int iHighH = iHues[iColor][1];
//
//void PrintVal(int iCur, string title)
//{
//    cout << title << ": " << iCur <<endl;
//}
//
//void ColorChange(int val, void *ud){
//    iColor = val;
//    iLowH = iHues[val][0];
//    iHighH = iHues[val][1];
//}
//
//
//int main( int argc, char** argv )
//{
//    VideoCapture cap(0); //capture the video from webcam
//    
//    if ( !cap.isOpened() )  // if not success, exit program
//    {
//        cout << "Cannot open the web cam" << endl;
//        return -1;
//    }
//    
//    namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"
//    
//    int iLowS = 140;
//    int iHighS = 255;
//    int iLowV = 135;
//    int iHighV = 255;
//    
//    int iCloseSize = 50;
//
//
//    
//    //Create trackbars in "Control" window
////    createTrackbar("Color", "Control", &iColor, 3, ColorChange);
//    
//    createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
//    createTrackbar("HighH", "Control", &iHighH, 179);
//    
//    createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
//    createTrackbar("HighS", "Control", &iHighS, 255);
//    
//    createTrackbar("LowV", "Control", &iLowV, 255);//Value (0 - 255)
//    createTrackbar("HighV", "Control", &iHighV, 255);
//    
//    int iLastX = -1;
//    int iLastY = -1;
//    
//    //Capture a temporary image from the camera
//    Mat imgTmp;
//    cap.read(imgTmp);
//    
//    //Create a black image with the size as the camera output
//    Mat imgLines = Mat::zeros( imgTmp.size(), CV_8UC3 );;
//    
//    
//    while (true)
//    {
//        Mat imgOriginal;
//        
//        bool bSuccess = cap.read(imgOriginal); // read a new frame from video
//        
//        
//        
//        if (!bSuccess) //if not success, break loop
//        {
//            cout << "Cannot read a frame from video stream" << endl;
//            break;
//        }
//        
//        Mat imgHSV;
//        
//        cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
//        
//        Mat imgThresholded;
//        Mat imgOpenClosed;
//        
//        inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
//        
//        //morphological opening (removes small objects from the foreground)
////        erode(imgThresholded, imgOpenClosed, getStructuringElement(MORPH_ELLIPSE, Size(iCloseSize, iCloseSize)) );
////        dilate( imgOpenClosed, imgOpenClosed, getStructuringElement(MORPH_ELLIPSE, Size(iCloseSize, iCloseSize)) );
//        
//        //morphological closing (removes small holes from the foreground)
//        dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(iCloseSize*2, iCloseSize)), Point(-1,-1) );
//        erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(iCloseSize*2, iCloseSize)), Point(-1,-1) );
//        
//        //Calculate the moments of the thresholded image
//        Moments oMoments = moments(imgThresholded);
//        
//        double dM01 = oMoments.m01;
//        double dM10 = oMoments.m10;
//        double dArea = oMoments.m00;
//        
//        // if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero
//        if (dArea > 10000)
//        {
//            //calculate the position of the ball
//            int posX = dM10 / dArea;
//            int posY = dM01 / dArea;
//            
//            if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
//            {
//                //Draw a red line from the previous point to the current point
//                line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0,0,255), 2);
//            }
//            
//            iLastX = posX;
//            iLastY = posY;
//        }
//        
//        imshow("Thresholded Image", imgThresholded); //show the thresholded image
////        imshow("OpenClosed Image", imgOpenClosed); //show the thresholded image
//        
//        imgOriginal = imgOriginal + imgLines;
//        imshow("Original", imgOriginal); //show the original image
//
//        int keyPressed = waitKey(30);
//
//        if ( keyPressed == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
//        {
//            cout << "esc key is pressed by user" << endl;
//            PrintVal(iLowH, "Low H");
//            PrintVal(iHighH, "High H");
//            PrintVal(iLowV, "Low V");
//            PrintVal(iHighV, "High V");
//            PrintVal(iLowS, "Low S");
//            PrintVal(iHighS, "High S");
//
//            cout << "esc key is pressed by user" << endl;
//            break;
//        }
//        
//        if ( keyPressed == 13){
//            cout << "enter key is pressed by user clear trace" << endl;
//            
//            //Create a black image with the size as the camera output
//            imgLines = Mat::zeros( imgTmp.size(), CV_8UC3 );
//            
//            //reset previous trace points
//            iLastX = -1;
//            iLastY = -1;
//        }
//        if ( keyPressed == 63232){
//            iLowH++;
//            cout << "Lower hue: " << iLowH << endl;
//        }
//        if ( keyPressed == 63233){
//            iLowH--;
//            cout << "Lower hue: " << iLowH << endl;
//        }
//        if ( keyPressed == 63235){
//            iHighH++;
//            cout << "Upper hue: " << iHighH << endl;
//        }
//        if ( keyPressed == 63234){
//            iHighH--;
//            cout << "Upper hue: " << iHighH << endl;
//        }
//    }
//
//    return 0;
//}
//
//=================================Detect Red=================================================
//
//#include <iostream>
//#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/imgproc/imgproc.hpp"
//
//using namespace cv;
//using namespace std;
//
//void ValueChange(int *iPrev, int *iCur, string title)
//{
//    if (*iCur != *iPrev){
//        *iPrev = *iCur;
//        cout << title << ": " << *iCur <<endl;
//    }
//}
//
//void PrintVal(int iCur, string title)
//{
//    cout << title << ": " << iCur <<endl;
//}
//
//int main( int argc, char** argv )
//{
//    VideoCapture cap(0); //capture the video from web cam
//    
//    if ( !cap.isOpened() )  // if not success, exit program
//    {
//        cout << "Cannot open the web cam" << endl;
//        return -1;
//    }
//    
//    namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"
//    
////    //any color
////    int iLowH = 0;
////    int iHighH = 179;
//    
//    //red
//    int iLowH = 168;
//    int iHighH = 179;
//    
////    //blue
////    int iLowH = 103;
////    int iHighH = 132;
////    
////    //green
////    int iLowH = 77;
////    int iHighH = 90;
//    
//    int iLowS = 140;
//    int iHighS = 255;
//    int iLowV = 135;
//    int iHighV = 255;
//    
//    int iPrevLH = 10;
//    int iPrevHH = 10;
//    int iPrevLS = 10;
//    int iPrevHS = 10;
//    int iPrevLV = 10;
//    int iPrevHV = 10;
//    
//    //Create trackbars in "Control" window
//    cvCreateTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
//    cvCreateTrackbar("HighH", "Control", &iHighH, 179);
//    
//    cvCreateTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
//    cvCreateTrackbar("HighS", "Control", &iHighS, 255);
//    
//    cvCreateTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
//    cvCreateTrackbar("HighV", "Control", &iHighV, 255);
//    
//    while (true)
//    {
//        Mat imgOriginal;
//        
//        bool bSuccess = cap.read(imgOriginal); // read a new frame from video
//        
//        if (!bSuccess) //if not success, break loop
//        {
//            cout << "Cannot read a frame from video stream" << endl;
//            break;
//        }
//        
//        Mat imgHSV;
//        
//        cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
//        
//        Mat imgThresholded;
//        
//        inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
//        
//        //morphological opening (remove small objects from the foreground)
//        erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
//        dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
//        
//        //morphological closing (fill small holes in the foreground)
//        dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
//        erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
//        
//        imshow("Thresholded Image", imgThresholded); //show the thresholded image
//        imshow("Original", imgOriginal); //show the original image
//        
//        ValueChange(&iPrevLH, &iLowH, "Low H");
//        ValueChange(&iPrevHH, &iHighH, "High H");
//        ValueChange(&iPrevLV, &iLowV, "Low V");
//        ValueChange(&iPrevHV, &iHighV, "High V");
//        ValueChange(&iPrevLS, &iLowS, "Low S");
//        ValueChange(&iPrevHS, &iHighS, "High S");
//        
//        if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
//        {
//            cout << "esc key is pressed by user" << endl;
//            PrintVal(iLowH, "Low H");
//            PrintVal(iHighH, "High H");
//            PrintVal(iLowV, "Low V");
//            PrintVal(iHighV, "High V");
//            PrintVal(iLowS, "Low S");
//            PrintVal(iHighS, "High S");
//            break; 
//        }
//    }
//    
//    return 0;
//    
//}

//=================================Load Webcam=================================================
//
//#include "opencv2/highgui/highgui.hpp"
//#include <iostream>
//
//using namespace cv;
//using namespace std;
//
//int main(int argc, char* argv[])
//{
//    VideoCapture cap(0); // open the video camera no. 0
//    
//    if (!cap.isOpened())  // if not success, exit program
//    {
//        cout << "Cannot open the video cam" << endl;
//        return -1;
//    }
//    
//    double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
//    double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
//    
//    cout << "Frame size : " << dWidth << " x " << dHeight << endl;
//    
//    namedWindow("MyVideo",CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"
//    
//    while (1)
//    {
//        Mat frame;
//        
//        bool bSuccess = cap.read(frame); // read a new frame from video
//        
//        if (!bSuccess) //if not success, break loop
//        {
//            cout << "Cannot read a frame from video stream" << endl;
//            break;
//        }
//        
//        imshow("MyVideo", frame); //show the frame in "MyVideo" window
//        
//        if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
//        {
//            cout << "esc key is pressed by user" << endl;
//            break; 
//        }
//    }
//    return 0;
//    
//}

//=================================Load Image=================================================
//
//#include "opencv2/highgui/highgui.hpp"
//#include <iostream>
//
//using namespace cv;
//using namespace std;
//
//int main( int argc, const char** argv )
//{
//    Mat img = imread("Pic.JPG", CV_LOAD_IMAGE_UNCHANGED); //read the image data in the file "MyPic.JPG" and store it in 'img'
//    
//    if (img.empty()) //check whether the image is loaded or not
//    {
//        cout << "Error : Image cannot be loaded..!!" << endl;
//        //system("pause"); //wait for a key press
//        return -1;
//    }
//    
//    namedWindow("MyWindow", CV_WINDOW_AUTOSIZE); //create a window with the name "MyWindow"
//    imshow("MyWindow", img); //display the image which is stored in the 'img' in the "MyWindow" window
//    
//    waitKey(0); //wait infinite time for a keypress
//    
//    destroyWindow("MyWindow"); //destroy the window with the name, "MyWindow"
//    
//    return 0;
//}
