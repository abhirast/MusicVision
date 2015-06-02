#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <algorithm>
#include <string>
#include "music.h"

using namespace cv;
using namespace std;


int main(int argc, char** argv) {
    if (argc == 1) cout << "Syntax error" << endl;
    char templ[100];
    char video[100];
    VideoCapture cp;
    InstrumentType itype;
    if (argc > 1) {
        sprintf(templ, "templates/%s", argv[1]);
        if (strcmp(argv[1], "xylo") == 0) itype = XYLO;
        else if (strcmp(argv[1], "piano") == 0) itype = PIANO;
        else if (strcmp(argv[1], "drums") == 0) itype = DRUMS;
        else cout << "Syntax error" << endl;
    }
    if (argc == 3) {
        sprintf(video, "data/%s-%s.avi", argv[1], argv[2]);
        cp.open(video);
    } else {
        cp.open(0);
    }
    // open the video source
    InstrumentModel imodel(templ);
    Detector detector(imodel, cp);
    Player player(itype);
    detector.init();
    Mat image;
    MusicParams mparams;
    while (detector.next(mparams)) {
        player.play(mparams);
        if (waitKey(100) > 0) break;
    }
}