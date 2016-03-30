#include "opencv2/highgui/highgui.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
    VideoCapture cap(0); // open the video camera no. 0
    
    if (!cap.isOpened())  // if not success, exit program
    {
        cout << "Cannot open the video cam" << endl;
        return -1;
    }
    
    double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
    double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
    
    cout << "Frame size : " << dWidth << " x " << dHeight << endl;
    
    namedWindow("MyVideo",CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"
    
    while (1)
    {
        Mat frame;
        
        bool bSuccess = cap.read(frame); // read a new frame from video
        
        if (!bSuccess) //if not success, break loop
        {
            cout << "Cannot read a frame from video stream" << endl;
            break;
        }
        
        imshow("MyVideo", frame); //show the frame in "MyVideo" window
        
        if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break; 
        }
    }
    return 0;
    
}



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
