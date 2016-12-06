////
////  main.cpp
////  Project_Vision
////
////  Created by Shehab Mohamed on 12/5/16.
////  Copyright © 2016 Shehab Mohamed. All rights reserved.
////
#include <iostream>
#include <string>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/objdetect/objdetect.hpp"

using namespace std;
using namespace cv;

void RGB_Range(Mat&);
void HSV_Range(Mat&);

int main()
{
    VideoCapture cap(0);
    if(!cap.isOpened())
        cout<<"Webcam is not opened."<<endl;
    
    // 640x480
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    
    
    while(true)
    {
        Mat frame, frame_HSV, frame_YCRCB;
        Mat Skin_Mask1, Skin_Mask2;
        Mat Contours, Hierarchy;
        cap.read(frame);
        
        
        auto lower_HSV = Scalar(0, 48, 80);
        auto upper_HSV = Scalar(20, 255, 255);
        
        //Loulou's paper2
//        auto lower_HSV = Scalar(0, 38, 0);
//        auto upper_HSV = Scalar(45, 230, 255);
        
        //Loulou's paper
        auto lower_YCRCB = Scalar(0, 150, 100);
        auto upper_YCRCB = Scalar(255, 200, 150);
        
        //My guess
//        auto lower_YCRCB = Scalar(0, 140, 80);
//        auto upper_YCRCB = Scalar(255, 180, 130);

        cvtColor(frame, frame_HSV, CV_BGR2HSV);
        cvtColor(frame, frame_YCRCB, CV_BGR2YCrCb);

        inRange(frame_HSV, lower_HSV, upper_HSV, Skin_Mask1);
        inRange(frame_YCRCB, lower_YCRCB, upper_YCRCB, Skin_Mask2);
        
        
        //RGB.create(frame.rows, frame.cols, CV_LOAD_IMAGE_GRAYSCALE);
        //HSV.create(frame.rows, frame.cols, CV_LOAD_IMAGE_GRAYSCALE);

        //RGB_Range(frame);
        //HSV_Range(frame_HSV, HSV);
        //blur(Skin_Mask2, Skin_Mask2, Size(3,3));
        //Canny(frame, Contours, 40, 120);
        
        imshow("Frame", frame);
        //imshow("HSV", frame_HSV);
        //imshow("YCRCB", frame_YCRCB);
        imshow("Skin Mask (HSV)", Skin_Mask1);
        imshow("Skin Mask (YCRCB)", Skin_Mask2);
    }
    //////////////////////////////////////////
    
    return 0;
}

void RGB_Range(Mat& frame)
{
    for(int i=0; i<frame.rows; i++)
    {
        for(int j=0; j<frame.cols; j++)
        {
            int Red = frame.at<Vec3b>(i, j)[2];
            int Green = frame.at<Vec3b>(i, j)[1];
            int Blue = frame.at<Vec3b>(i, j)[0];
            //TODO
            if(Red >= 100 && Green >= 100 && Blue >= 100)
            {
                frame.at<Vec3b>(i, j)[2] = 255;
                frame.at<Vec3b>(i, j)[1] = 255;
                frame.at<Vec3b>(i, j)[0] = 255;
            }
                
        }
    }
}

void HSV_Range(Mat& frame)
{
    for(int i=0; i<frame.rows; i++)
    {
        for(int j=0; j<frame.cols; j++)
        {
            int H = frame.at<Vec3b>(i, j)[0];
            int S = frame.at<Vec3b>(i, j)[1];
            int V = frame.at<Vec3b>(i, j)[2];
            //TODO
            if(H >= 100 && S >= 100 && V >= 100)
            {
                frame.at<Vec3b>(i, j)[0] = 255;
                frame.at<Vec3b>(i, j)[1] = 255;
                frame.at<Vec3b>(i, j)[2] = 255;
            }
        }
    }
}



//#include "opencv2/opencv.hpp"
//
//using namespace cv;
//
//int main(int, char**)
//{
//    VideoCapture cap(0); // open the default camera
//    //VideoCapture cap("/home/mohamed/temp/sample.dino.avi");
//    if(!cap.isOpened())  // check if we succeeded
//        return -1;
//    std::cerr << cap.get(CV_CAP_PROP_FRAME_WIDTH);
//    std::cerr << cap.get(CV_CAP_PROP_FRAME_HEIGHT);
//    
//    //create Background Subtractor object
//    BackgroundSubtractorMOG2 MoG; //MOG approach
//    
//    for(;;)
//    {
//        Mat frame,foregroundMask;
//        cap >> frame; // get a new frame from camera
//        imshow("frame",frame);
//        MoG(frame, foregroundMask,0.01);
//        imshow("Mask",foregroundMask);
//        
//        if(waitKey(30) >= 0) break;
//    }
//    
//    // the camera will be deinitialized automatically in VideoCapture destructor
//    return 0;
//}