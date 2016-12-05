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

void RGB_Range(Mat, Mat&);
void HSV_Range(Mat, Mat&);

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
        Mat frame, frame_HSV;
        Mat RGB, HSV;
        cap.read(frame);
        
        RGB.create(frame.rows, frame.cols, CV_LOAD_IMAGE_GRAYSCALE);
        HSV.create(frame.rows, frame.cols, CV_LOAD_IMAGE_GRAYSCALE);

        cvtColor(frame, frame_HSV, CV_BGR2HSV);
        RGB_Range(frame, RGB);
        HSV_Range(frame_HSV, HSV);
        //blur(frame, frame, Size(5,5));
        //Canny(frame, Contours, 40, 120);
        
        imshow("Frame", frame);
        imshow("HSV", frame_HSV);
        //imshow("RGB", RGB);
        //imshow("HSV", HSV);
    }
    //////////////////////////////////////////
    
    return 0;
}

void RGB_Range(Mat frame, Mat& new_frame)
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
                new_frame.at<uchar>(i, j) = 255;
            }
                
        }
    }
}

void HSV_Range(Mat frame, Mat& new_frame)
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
                new_frame.at<uchar>(i, j) = 255;
        }
    }
}


