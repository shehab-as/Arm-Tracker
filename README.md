# Vision-Project
### Course Project for CS463: Fundamentals of Computer Vision
#### Contributors: Alia Hassan and Shehab Abdel-Salam, The American University in Cairo.

Arm-Tracker is a program that detects Hands in a ROI (lower 2/3) of the frame, masks out the background using our color-space for the skin tone. After masking out, it draws its contours within our certain threshold to remove noise, and then draws the convex hull around the extreme points to have a better representation of the hand. It calculates the centroid of the contours of the detected object and tracks its path within the surrounding pixels, leaving a fading tracking line.

### Skin-tone Masked Out:
![Preview](https://github.com/ShehabCE/Vision-Project/blob/master/Screen%20Shot%202016-12-10%20at%205.16.31%20PM.png)


