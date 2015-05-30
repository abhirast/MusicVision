#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <algorithm>
#include "music.h"

using namespace cv;
using namespace std;


Detector::Detector() {
    CvCapture* capture;
}

bool Detector::init() {
    return false;
}
        
bool Detector::next(MusicParams &params) {
    return false;
}

void Detector::close() {
}

void Detector::find_calib_locs(Mat &image, vector<Point> &locs) {
	// perform graythresh
    Mat bin_image;
    threshold (image, bin_image, 0, 255, THRESH_BINARY | THRESH_OTSU);
    // find the largest connected component
    vector<vector<Point> > blobs;
    find_connected_components(bin_image, blobs);
    vector<int> sizes(blobs.size());
    int index = -1, mx = -1;
    for (int i = 0; i < blobs.size(); i++) {
        if ((int) blobs[i].size() > mx) {
            index = i;
            mx = blobs[i].size();
        }
    }
    Mat page = Mat::zeros(bin_image.rows, bin_image.cols, CV_8U);
    for (int i = 0; i < blobs[index].size(); i++) {
        page.at<uchar>(blobs[index][i]) = 255;
    }
    // find the small connected components in the negative of the page
    page = 255 - page;
    
    // Create a structuring element
    int erosion_size = 3;  
    Mat res;
    Mat element = getStructuringElement(MORPH_CROSS,
          Size(2 * erosion_size + 1, 2 * erosion_size + 1),
          Point(0, 0) );
 
    // Apply erosion or dilation on the image
    erode(page, res, element);
    
    // find_connected_components(page, blobs);
    find_connected_components(res, blobs);
    
    locs.clear();
    for (int i = 0; i < blobs.size(); i++) {
        if (blobs[i].size() > 15 && blobs[i].size() < 1000) {
            locs.push_back(find_mean(blobs[i]));
        }
    }
}

/////////////////////////////////////////////
//		Private methods
/////////////////////////////////////////////


void Detector::find_connected_components(Mat &binary, 
                        vector<vector<Point> > &blobs) {
	blobs.clear();
    Mat labels;
    binary.convertTo(labels, CV_32FC1);
    labels /= 255;
    int label_count = 2; // starts at 2 because 0,1 are used already
    
    for(int y=0; y < binary.rows; y++) {
        for(int x=0; x < binary.cols; x++) {
            if(labels.at<float>(y,x) != 1) {
                continue;
            }
 
            Rect rect;
            floodFill(labels, Point(x,y), Scalar(label_count), 
                                    &rect, Scalar(0), Scalar(0), 4);
 
            vector<Point>  blob;
 
            for(int i=rect.y; i < (rect.y+rect.height); i++) {
                for(int j=rect.x; j < (rect.x+rect.width); j++) {
                    if(labels.at<float>(i, j) != label_count) {
                        continue;
                    }
                    blob.push_back(Point(j,i));
                }
            }
            blobs.push_back(blob);
            label_count++;
        }
    }
}

Point Detector::find_mean(vector<Point> blob) {
	int a = 0, b = 0;
    for (int i = 0; i < blob.size(); i++) {
        a += blob[i].x;
        b += blob[i].y;
    }
    return Point(floor(1.0*a/blob.size()), floor(1.0*b/blob.size()));
}