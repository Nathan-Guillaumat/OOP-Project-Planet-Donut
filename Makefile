OUT = projet
CXX     = g++
CXXFLAGS = -Wall -std=c++11
LINKING = `pkg-config --cflags gtkmm-3.0`
LDLIBS = `pkg-config --libs gtkmm-3.0`
OFILES = base.o gisement.o message.o robot.o simulation.o projet.o geomod.o gui.o graphic.o

all: $(OUT)

projet.o: projet.cc simulation.h gui.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

gisement.o: gisement.cc gisement.h message.h constantes.h geomod.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

robot.o: robot.cc robot.h gisement.h message.h constantes.h geomod.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

base.o: base.cc base.h robot.h gisement.h message.h constantes.h geomod.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

simulation.o: simulation.cc simulation.h gisement.h base.h geomod.h message.h constantes.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

geomod.o: geomod.cc geomod.h constantes.h graphic.h graphic.cc
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

gui.o : gui.cc gui.h simulation.h graphic.h constantes.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

graphic.o : graphic.cc graphic.h
	$(CXX) $(CXXFLAGS) $(LINKING) -c $< -o $@ $(LINKING)

$(OUT): $(OFILES)
	$(CXX) $(CXXFLAGS) $(LINKING) $(OFILES) -o $@ $(LDLIBS)

clean:
	@echo "Cleaning compilation files"
	@rm *.o $(OUT) *.cc~ *.h~
