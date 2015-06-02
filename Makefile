CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`

CLASSES = Detector.cpp InstrumentModel.cpp Player.cpp


all : source.cpp
	@echo "\nBuilding project\n"
	@g++ --std=c++0x $(CFLAGS) -o music $(CLASSES) $< $(LIBS)

clean:
	@rm -f music
	
% : %.cpp
	g++ --std=c++0x $(CFLAGS) -o $@ $< $(LIBS)