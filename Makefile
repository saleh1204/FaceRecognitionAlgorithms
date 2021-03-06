CFLAGS = `pkg-config --cflags --libs opencv`
CC = g++


all: lbp eigenfaces

lbp: lbpFacesRecognizer.cpp
	$(CC) lbpFacesRecognizer.cpp -fopenmp -o LBP $(CFLAGS)

eigenfaces: eigenFacesRecognizer.cpp
	$(CC) eigenFacesRecognizer.cpp -fopenmp -o EigenFaces $(CFLAGS)

clean:
	rm -rf LBP EigenFaces FisherFaces
