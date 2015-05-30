#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <algorithm>
#include <string>
#include "music.h"

using namespace cv;
using namespace std;

int main() {
    // Define a detector
    Detector detector;
    string tempfile = "templates/template.1";
    InstrumentModel imodel(tempfile);
    Mat tempi;
    imodel.toImage(tempi);
    namedWindow( "template", 0);
    imshow("template", tempi);
    waitKey(0);
    cout << imodel.rows << " " << imodel.cols << endl;
    Mat image;
    image = imread("data/t1_1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    

    vector<Point> caliblocs;
    detector.find_calib_locs(image, caliblocs);

    cout << "Found " << caliblocs.size() << " calib points" << endl;
    for (int i = 0; i  < caliblocs.size(); i++) {
        cout << caliblocs[i] << endl;
    }
    Mat x = Mat::zeros(image.rows, image.cols, CV_8U);
    for (int i = 0; i < caliblocs.size(); i++) {
        x.at<uchar>(caliblocs[i]) = 255;
    }
    Mat y;
    int erosion_size = 3;  
    Mat element = getStructuringElement(MORPH_CROSS,
          Size(2 * erosion_size + 1, 2 * erosion_size + 1),
          Point(0, 0) );
    dilate(x, y, element);
    namedWindow( "Display window", 0);
    imshow("Display window", image);
    namedWindow( "Display window2", 0);
    imshow("Display window2", y);
    waitKey(0);
    return 0;
}
