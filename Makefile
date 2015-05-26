CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`

CLASSES = Detector.cpp InstrumentModel.cpp Player.cpp


all : source.cpp
	@echo "\nBuilding project\n"
	@g++ $(CFLAGS) -o music $(CLASSES) $< $(LIBS)

clean:
	@rm -f music
