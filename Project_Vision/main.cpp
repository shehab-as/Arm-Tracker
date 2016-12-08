////
////  main.cpp
////  Project_Vision
////
////  Created by Shehab Mohamed on 12/5/16.
////  Copyright © 2016 Shehab Mohamed. All rights reserved.
////
#include <iostream>
#include <string>
#include <vector>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/objdetect/objdetect.hpp"

using namespace std;
using namespace cv;

void RGB_Range(Mat&);

int main()
{
    VideoCapture cap(0);
    if(!cap.isOpened())
        cout<<"Webcam is not opened."<<endl;
    
    // 640x480
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    
    //BackgroundSubtractorMOG2 MOG;
    
    Mat frame, frame_HSV, frame_YCRCB;
    Mat Skin_Mask_YC, Skin_Mask_HSV;
    vector<vector<Point>> paths;
    
    while(true)
    {
        cap.read(frame);
        
        //Loulou's paper
//        auto lower_YCRCB = Scalar(0, 150, 100);
//        auto upper_YCRCB = Scalar(255, 200, 150);
        
        
        //From Gesture example (Perfect so far) (2).
        auto lower_YCRCB = Scalar(0, 148, 88);
        auto upper_YCRCB = Scalar(255, 210, 150);
        
        //From Gesture example (Perfect so far).
//        auto lower_YCRCB = Scalar(60, 140, 90);
//        auto upper_YCRCB = Scalar(250, 210, 130);

        
        //Not correct segmentation.
//        auto lower_YCRCB = Scalar(0, 133, 77);
//        auto upper_YCRCB = Scalar(255, 173, 127);
        
        //My guess
//        auto lower_YCRCB = Scalar(0, 140, 80);
//        auto upper_YCRCB = Scalar(255, 180, 130);
        
        //Terrible.
        auto lower_HSV = Scalar(130, 10, 75);
        auto upper_HSV = Scalar(160, 40, 130);
        
////////////////////////////////////////////////////////////////////
        cvtColor(frame, frame_YCRCB, CV_BGR2YCrCb);
        cvtColor(frame, frame_HSV, CV_BGR2HSV);

        vector<Mat> Contours;
        vector<Vec4i> Hierarchy;
        //Mat Contours;
        
        
        // Region of Interest 1/2 Lower Image.
        Mat frame_YCRCB_ROI = frame_YCRCB(Range((int)frame_YCRCB.rows/3, frame_YCRCB.rows), Range(0, frame_YCRCB.cols));
        Mat frame_ROI = frame(Range((int)frame.rows/3, frame.rows), Range(0, frame.cols));
        inRange(frame_YCRCB_ROI, lower_YCRCB, upper_YCRCB, Skin_Mask_YC);
        //inRange(frame_YCRCB_ROI, lower_YCRCB, upper_YCRCB, Skin_Mask_YC);
        inRange(frame_HSV, lower_HSV, upper_HSV, Skin_Mask_HSV);
        
        medianBlur(Skin_Mask_YC, Skin_Mask_YC, 5);
        medianBlur(Skin_Mask_HSV, Skin_Mask_HSV, 5);

        //Mat element = getStructuringElement(MORPH_ELLIPSE, Size(2 * 5 + 1, 2 * 5 + 1), Point(5, 5));
        //dilate(Skin_Mask_YC, Skin_Mask_YC, element);
        //medianBlur(Skin_Mask_RGB, Skin_Mask_RGB, 5);
        //RGB_Range(frame_RGB);
        
        // Applying Region of Interest
        findContours(Skin_Mask_YC, Contours, Hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
        vector<Mat> Better_Contours;
        vector<Point> largest (1);
        
        for(int i=0; i<Contours.size(); i++)
        {
            auto area = contourArea(Contours[i]);
            if(area > 1200 && area < 15000)
            {
                auto M = moments(Contours[i]);
                int cX = M.m10 / M.m00;
                int cY = M.m01 / M.m00;
                Better_Contours.push_back(Contours[i]);
                
                /////////////////////////////////
                
//                if ( contourArea(Contours[i])> contourArea(largest))
//                {
//                    largest = Contours[i];
//                }
                
                /////////////////////////////////
                
                drawContours(frame_ROI, Contours, i, Scalar(0,255,0), 3);
                //Drawing Centroid on Contours.
                circle(frame_ROI, Point(cX, cY), 3, Scalar(255,0,0), -1);
            }
        }
        
        /////////////////////////////////
        //Calculating Paths
        for( int a = 0; a < Better_Contours.size() ; a++)
        {
            auto M = moments(Better_Contours[a]);
            int cX = M.m10 / M.m00;
            int cY = M.m01 / M.m00;
            bool close = false;
            
            if ( cX > 0 && cY > 0)
            {
                for (int b = 0; b < paths.size(); b++)
                {
                    if ( abs(cX - paths[b].back().x) < 30 && abs(cY - paths[b].back().y) < 30)
                    {
                        paths[b].push_back(Point(cX, cY));
                        close = true;
                        break;
                    }
                    else
                    {
                        paths[b].push_back(paths[b].back());
                    }
                }
                
                if (close == false)
                {
                    vector<Point> temp;
                    temp.push_back(Point(cX, cY));
                    paths.push_back(temp);
                    
                }

                cout << "coor " << cX << " " << cY << endl;
            }
            
            
            
            
        }
        
        //Path clean up - removing obsolete paths
        for (int d = 0; d < paths.size(); d++)
        {
            if (paths[d].size() > 5)
            {
                if( paths[d][paths[d].size()-1] == paths[d][paths[d].size()-2] && paths[d][paths[d].size()-2] == paths[d][paths[d].size()-3] && paths[d][paths[d].size()-3] == paths[d][paths[d].size()-4])
                {
                    paths.erase(paths.begin()+ d);
                }
            }
            
            
        }
        
        //Drawing paths
        for (int c = 0; c < paths.size(); c++)
        {
            if( paths[c].size() >  40)
            {
                for ( int j = paths[c].size() - 40; j < paths[c].size(); j++)
                {
                    //                        cout << "paths[i] " << paths[i];
                    //                        int thickness = int( sqrt(64/ float(j+1))*2.5);
                    line(frame_ROI, paths[c][j-1], paths[c][j], Scalar(0,0,0), 3);
                }
            }
        }
        
        
        /////////////////////////////////
        
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        
        vector<vector<Point>> hull(Better_Contours.size());
        
        //Calcuating Convex Hull.
        for(int i=0; i<Better_Contours.size(); i++)
            convexHull(Mat(Better_Contours[i]), hull[i], false);
        
        //Drawing Convex Hull.
        for(int i=0; i<Better_Contours.size(); i++)
        {
            auto area2 = contourArea(hull[i]);
            auto ratio = Contours[i].rows/area2;
            cout<<"Ratio: "<<ratio<<endl;
            drawContours( frame_ROI, hull, i, Scalar(0,0,255), 3, 8, vector<Vec4i>() );
        }
        
        
        // Commented Code Below is for the whole image. Above is for 1/2 Image (ROI)
        
        //Finding Contours Code Block for YCRCB
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//        findContours(Skin_Mask_YC, Contours, Hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
//        vector<Mat> Better_Contours;
//        
//        for(int i=0; i<Contours.size(); i++)
//        {
//            auto area = contourArea(Contours[i]);
//            if(area > 1200 && area < 15000)
//            {
////                auto ratio = Contours[i].rows/area;
////                cout<<"Ratio: "<<ratio<<endl;
//
//                Better_Contours.push_back(Contours[i]);
//                //putText(frame,  to_string(i) + "   ratio " + to_string( Contours[i].rows/ area), Point(frame.cols/2, frame.rows/2), FONT_HERSHEY_COMPLEX_SMALL, 10, Scalar(255,0,0), 1, CV_AA );
//                
//                drawContours(frame, Contours, i, Scalar(0,255,0), 3);
//            }
//        }
//        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//        vector<vector<Point>> hull(Better_Contours.size());
//        
//        for(int i=0; i<Better_Contours.size(); i++)
//            convexHull(Mat(Better_Contours[i]), hull[i], false);
//
//        
//        for(int i=0; i<Better_Contours.size(); i++)
//        {
//            auto area2 = contourArea(hull[i]);
//            auto ratio = Contours[i].rows/area2;
//            cout<<"Ratio: "<<ratio<<endl;
//            //if(ratio <= 0.001)
//            drawContours( frame, hull, i, Scalar(0,0,255), 3, 8, vector<Vec4i>() );
//
//        }
        
        
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        imshow("Frame", frame);
        imshow("Skin Mask (HSV)", Skin_Mask_HSV);
        imshow("Skin Mask (YCRCB)", Skin_Mask_YC);
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
            int R = frame.at<Vec3b>(i, j)[0];
            int G = frame.at<Vec3b>(i, j)[1];
            int B = frame.at<Vec3b>(i, j)[2];

            bool e1 = (R>95) && (G>40) && (B>20) && ((max(R,max(G,B)) - min(R, min(G,B)))>15) && (abs(R-G)>15) && (R>G) && (R>B);
            bool e2 = (R>220) && (G>210) && (B>170) && (abs(R-G)<=15) && (R>B) && (G>B);
            
            if(e1 || e2)
            {
                frame.at<Vec3b>(i, j)[0] = 255;
                frame.at<Vec3b>(i, j)[1] = 255;
                frame.at<Vec3b>(i, j)[2] = 255;
            }
        }
    }
}


//Loulou's Background Subtraction.

//        if (background_frame>0)
//        {
//            if(firstM.empty())
//            {
//                cap.read(firstM);
//                cvtColor(firstM, firstM, CV_RGB2YCrCb);
//            }
//            background_frame--;
//            continue;
//        }
//        else
//            cap.read(frame);
//            cvtColor(frame, frame, CV_RGB2YCrCb);
//
//        vector<Mat> bg, fg;
//        split(firstM, bg);
//        split(frame, fg);
//        Mat distance;
//        distance.create(frame.rows, frame.cols, CV_32F);
//        distance = Scalar(0,0,0);
//        for( int i =0; i< fg.size() ; i++)
//        {
//            Mat temp = abs(fg[i] - bg[i]);
//            temp.convertTo(temp, CV_32F);
//
//            distance += temp;
//        }
//        Mat mask;
//        threshold(distance, mask, 35, 255, THRESH_BINARY);
//
//        Mat kernel5x5 = getStructuringElement(MORPH_RECT, Size(5,5));
//        morphologyEx(mask, mask, MORPH_OPEN, kernel5x5);

