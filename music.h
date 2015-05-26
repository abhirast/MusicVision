#include <map>
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
    InstrumentModel(char* fileName);
    double getFreq(int x, int y);
    double getIntensity(int x, int y);    
private:
    std::map<Location,MusicParams> model;
};


class Detector {
public:
    Detector();
    bool init();
    bool next(MusicParams &params);
    void close();
private:
    
};


class Player {
public:
    Player();
    bool play(MusicParams &params);
    void close(); 
private:
    InstrumentType itype;
};

