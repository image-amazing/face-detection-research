CAM_LIBS=-lraspicam -lraspicam_cv -lmmal -lmmal_core -lmmal_util -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect
GL_LIBS=-lGL -lGLU -lglut -lGLEW
LIBS=$(CAM_LIBS) $(GL_LIBS)
LIB_PATH=/opt/vc/lib
INCLUDE_PATH=/usr/local/include/

SRC_DIR=./src/
HEADERS=$(SRC_DIR)camera.h $(SRC_DIR)detection.h $(SRC_DIR)tracking.h $(SRC_DIR)Timer.h
SRCS=$(SRC_DIR)main.cpp $(SRC_DIR)camera.cpp $(SRC_DIR)detection.cpp $(SRC_DIR)tracking.cpp $(SRC_DIR)prediction.cpp
OUTPUT_BIN=research-project

all: build-camera

build-camera: $(OUTPUT_BIN)
run: build-camera
	mkdir -p face_imgs
	./$(OUTPUT_BIN) face_imgs/
clean:
	#	rm face_imgs/*
	rm -f $(OUTPUT_BIN)

$(OUTPUT_BIN): $(HEADERS) $(SRCS)
	g++ -std=c++11 $(SRCS) -o $(OUTPUT_BIN) -L$(LIB_PATH) -I$(INCLUDE_PATH) $(LIBS)
	
