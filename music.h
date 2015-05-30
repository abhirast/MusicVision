#include <map>
#include <opencv2/core/core.hpp>

enum InstrumentType {
    XYLO,
    DRUM
};

struct Location {
    int x;
    int y;
};

struct MusicParams {
    double frequency;
    double intensity;
};

class InstrumentModel {
public:
    int rows;
    int cols;
    InstrumentModel(std::string fileName);
    double getFreq(int x, int y);
    double getIntensity(int x, int y);
    void toImage(cv::Mat &m);    
private:
    std::map<Location,MusicParams> model;
    std::vector<std::vector<int> > values;
};


class Detector {
public:
    Detector();
    bool init();
    bool next(MusicParams &params);
    void close();
    void find_calib_locs(cv::Mat &image, std::vector<cv::Point> &locs);
private:
    void find_connected_components(cv::Mat &binary, 
                        std::vector<std::vector<cv::Point> > &blobs);
    cv::Point find_mean(std::vector<cv::Point> blob);
};


class Player {
public:
    Player();
    bool play(MusicParams &params);
    void close(); 
private:
    InstrumentType itype;
};

