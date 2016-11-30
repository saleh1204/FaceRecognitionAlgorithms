/*
 * Copyright (c) 2011. Philipp Wagner <bytefish[at]gmx[dot]de>.
 * Released to public domain under terms of the BSD Simplified license.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the organization nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *   See <http://www.opensource.org/licenses/bsd-license>
 */

#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <ostream>
#include <ctime>

using namespace cv;
using namespace std;

namespace Color {
    enum Code {
	FG_RED      = 31,
        FG_GREEN    = 32,
        FG_BLUE     = 34,
        FG_DEFAULT  = 39,
        BG_RED      = 41,
        BG_GREEN    = 42,
        BG_BLUE     = 44,
        BG_DEFAULT  = 49
    };
    class Modifier {
        Code code;
        public:
               Modifier(Code pCode) : code(pCode) {}
    	       friend std::ostream&
	       operator<<(std::ostream& os, const Modifier& mod) {
               return os << "\033[" << mod.code << "m";
    	       }
	};
}

static Mat norm_0_255(InputArray _src) {
    Mat src = _src.getMat();
    // Create and return normalized image:
    Mat dst;
    switch(src.channels()) {
    case 1:
        cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
        break;
    case 3:
        cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC3);
        break;
    default:
        src.copyTo(dst);
        break;
    }
    return dst;
}

static void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';') {
    std::ifstream file(filename.c_str(), ifstream::in);
    if (!file) {
        string error_message = "No valid input file was given, please check the given filename.";
        CV_Error(CV_StsBadArg, error_message);
    }
    string line, path, classlabel;
    while (getline(file, line)) {
        stringstream liness(line);
        getline(liness, path, separator);
        getline(liness, classlabel);
        if(!path.empty() && !classlabel.empty()) {
            images.push_back(imread(path, 0));
            labels.push_back(atoi(classlabel.c_str()));
        }
    }
}

int main(int argc, const char *argv[]) {

    time_t rawtime;
    struct tm * timeinfo;
    // Sometimes you'll need to get/set internal model data,
    Color::Modifier red(Color::FG_RED);
    Color::Modifier green(Color::FG_GREEN);
    Color::Modifier blue(Color::FG_BLUE);
    Color::Modifier def(Color::FG_DEFAULT);

    // Check for valid command line arguments, print usage
    // if no arguments were given.
    if (argc < 3) {
        cout << "usage: " << argv[0] << " <Training.csv.ext> <Testing.csv.ext> {threshold} {output_folder}\n Default threshold is 50" << endl;
        exit(1);
    }

    double threshold = 50;
    if (argc >= 4)
    {
    	threshold = atof(argv[3]);
    }


    string output_folder = "EigenFacesImages";
    if (argc >= 5) {
        output_folder = string(argv[4]);
    }

    // Get the path to your Training CSV.
    string fn_csv = string(argv[1]);

    // Get the path to your Testing CSV .
    string fn_csv_test = string(argv[2]);

    // These vectors hold the images and corresponding labels.
    vector<Mat> images, imagesTesting;
    vector<int> labels, labelsTesting;
    // Read in the data. This can fail if no valid
    // input filename is given.
	cout << "Reading Training Images" << endl;
    try {
        read_csv(fn_csv, images, labels);
    } catch (cv::Exception& e) {
        cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
        // nothing more we can do
        exit(1);
    }

    // Quit if there are not enough images for this demo.
    if(images.size() <= 1) {
        string error_message = "This demo needs at least 2 images to work. Please add more images to your data set!";
        CV_Error(CV_StsError, error_message);
    }
   // Get the height from the first image. We'll need this
    // later in code to reshape the images to their original
    // size:
    int height = images[0].rows;
    cout << "Reading Testing Images" << endl;
	try {
        	read_csv(fn_csv_test, imagesTesting, labelsTesting);
    }catch (cv::Exception& e) {
        cerr << "Error opening file \"" << fn_csv_test << "\". Reason: " << e.msg << endl;
        // nothing more we can do
        exit(1);
    }

    // The following lines create an Eigenfaces model for
    // face recognition and train it with the images and
    // labels read from the given CSV file.
    // This here is a full PCA, if you just want to keep
    // 10 principal components (read Eigenfaces), then call
    // the factory method like this:
    //
    //      cv::createEigenFaceRecognizer(10);
    //
    // If you want to create a FaceRecognizer with a
    // confidence threshold (e.g. 123.0), call it with:
    //
    //      cv::createEigenFaceRecognizer(10, 123.0);
    //
    // If you want to use _all_ Eigenfaces and have a threshold,
    // then call the method like this:
    //
    //      cv::createEigenFaceRecognizer(0, 123.0);
    //
	cout << "Training The Recognizer with "<< images.size() <<" images \nIt may take some time" << endl;
	// To Calculate how much time Training takes
	clock_t start, end;
	
   	Ptr<FaceRecognizer> model = createEigenFaceRecognizer(15);

	start = time(0);
   	model->train(images, labels);
	end = time(0);
	model->set("threshold",threshold);
	double trainingTime, testingTime;
	trainingTime = (end-start);
	cout << blue << "Time taken for training is " << (trainingTime) << " seconds"<< def << endl;	

	int mispredicted = 0;
	int unpredicted = 0;
	int predictedLabel = 0;
	double confidence = 0;
	
	
	time(&rawtime); 
	timeinfo = localtime(&rawtime);	
	char currentDate[80];		
	strftime(currentDate,80,"%Y-%m-%d_%H:%M",timeinfo);	
	string fout = "./stat/eigen.csv";
	
	ofstream output; 
	
	ifstream testOutput(fout.c_str());
	if (!testOutput){
		// if file does not exist, create one and add the header
		output.open(fout.c_str());
		output << "Date & Time,Threshold,Mispredicted,Unpredicted,Accuracy,Training Time,Testing Time" << endl;
	}
	else {
		// if file already exists, append
		output.open(fout.c_str(), ofstream::out | ofstream::app);
	}
	cout << red << "\tStart testing " << labelsTesting.size() << " images one by one \t It will take a while" << def << endl;
	start = time(0);
	for (int i=0; i<labelsTesting.size();i++)
	{
		model->predict(imagesTesting[i], predictedLabel, confidence);
		if (predictedLabel == -1)
		{
			unpredicted++;
			confidence = -1.0;
		}
		if (predictedLabel != labelsTesting[i] && predictedLabel != -1)
		{
			mispredicted++;
		}

	}
	end = time(0);
	testingTime = (end - start);
	cout << blue << "Time taken for testing is " << (testingTime) << " seconds"<< def << endl;	
	

	cout << "\t\tUnrecognized : " << unpredicted << endl; cout << "\t\tMispredicted : " << mispredicted << endl; 
	double accuracy = double (labelsTesting.size() - unpredicted);
	accuracy = accuracy - mispredicted;
	accuracy = accuracy / (1.0*labelsTesting.size());
	accuracy = accuracy * 100.00;
	string thresholdStr = format("\tThreshold: %.2f", threshold);
	string accStr = format ("\t\tAccuracy: %.3f %%",accuracy);
	cout << green << accStr << blue << thresholdStr << def << endl;
	output << currentDate << "," << threshold << "," << mispredicted << "," << unpredicted << "," << accuracy << "," << trainingTime << "," << testingTime << endl ;
	
	output.close();

    // Here is how to get the eigenvalues of this Eigenfaces model:
    Mat eigenvalues = model->getMat("eigenvalues");
    // And we can do the same to display the Eigenvectors (read Eigenfaces):
    Mat W = model->getMat("eigenvectors");
    // Get the sample mean from the training data
    Mat mean = model->getMat("mean");
    // save eigenfaces
    output_folder = output_folder;
    imwrite(format("%s/%s-mean.png", output_folder.c_str(), currentDate), norm_0_255(mean.reshape(1, images[0].rows)));
   
    // Display or save the Eigenfaces:
    for (int i = 0; i < min(10, W.cols); i++) {
        string msg = format("Eigenvalue #%d = %.5f", i, eigenvalues.at<double>(i));
        cout << msg << endl;
        // get eigenvector #i
        Mat ev = W.col(i).clone();
        // Reshape to original size & normalize to [0...255] for imshow.
        Mat grayscale = norm_0_255(ev.reshape(1, height));
        // Show the image & apply a Jet colormap for better sensing.
        Mat cgrayscale;
        applyColorMap(grayscale, cgrayscale, COLORMAP_JET);
        // save:
        imwrite(format("%s/%s-eigenface_%d.png", output_folder.c_str(), currentDate, i), norm_0_255(cgrayscale));
    }

    // Display or save the image reconstruction at some predefined steps:
    for(int num_components = min(W.cols, 10); num_components < min(W.cols, 300); num_components+=15) {
        // slice the eigenvectors from the model
        Mat evs = Mat(W, Range::all(), Range(0, num_components));
        Mat projection = subspaceProject(evs, mean, images[0].reshape(1,1));
        Mat reconstruction = subspaceReconstruct(evs, mean, projection);
        // Normalize the result:
        reconstruction = norm_0_255(reconstruction.reshape(1, images[0].rows));
        //save:
        imwrite(format("%s/%s-eigenface_reconstruction_%d.png", output_folder.c_str(), currentDate,num_components), reconstruction);

    }
    return 0;
}
