#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <ostream>
#include <string>
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
    	// Check for valid command line arguments, print usage
    	// if no arguments were given.
    	if (argc < 3) {
        	cout << "usage: " << argv[0] << " <csvTraining.ext> <csvTesting.ext> " << endl;
        	exit(1);
    	}

    	// Get the path to your Training CSV.
    	string fn_csv = string(argv[1]);
    	
	// Get the path to your Testing CSV .
    	string fn_csv_test = string(argv[2]);
    	
    	// These vectors hold the images and corresponding labels.
    	vector<Mat> images, imagesTesting;
    	vector<int> labels, labelsTesting;
    	
	cout << "Reading Training Images" << endl;
    	try {
        	read_csv(fn_csv, images, labels);
    	} catch (cv::Exception& e) {
        	cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
        	// nothing more we can do
        	exit(1);
    	}
    	
    	cout << "Reading Testing Images" << endl;
	try {
        	read_csv(fn_csv_test, imagesTesting, labelsTesting);
    	}catch (cv::Exception& e) {
        	cerr << "Error opening file \"" << fn_csv_test << "\". Reason: " << e.msg << endl;
        	// nothing more we can do
        	exit(1);
    	}

    	if(images.size() <= 1) {
        	string error_message = "This demo needs at least 2 images to work. Please add more images to your data set!";
        	CV_Error(CV_StsError, error_message);
    	}

    	double threshold = 1;
    	double finalThreshold = 100;
    	double change = 1.0;
    	if (argc >= 4)
    	{
    		threshold = atof(argv[3]);
    	}
    	if (argc >= 5)
    	{
  		finalThreshold = atof(argv[4]);
    	}
    	if (argc >= 6) 
    	{
    		change = atof(argv[5]); // step size 
    	}
	ofstream output;
	output.open ("output.csv");
	output << "Threshold, Mispredicted, Unpredicted, Accuracy" << endl;


	// Training Stage
	cout << "Training The Recognizer with "<<images.size()<<" images \nIt may take some time" << endl;
	Ptr<FaceRecognizer> model = createLBPHFaceRecognizer();
	model->train(images, labels);

	int mispredicted = 10;
	int unpredicted = 0;


	while (threshold <= finalThreshold)
	{
		// Setting Threshold 
		model->set("threshold",threshold);
	
	
		// Testing Stage
		mispredicted = 0;
		unpredicted = 0;
		int predictedLabel = 0;
		double confidence = 0;
		cout << "Start testing " << labelsTesting.size() << " images one by one with threshold = "<< threshold << endl;
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
		cout << "Unrecognized : " << unpredicted << endl; cout << "Mispredicted : " << mispredicted << endl; 
		double accuracy = double (labelsTesting.size() - unpredicted);
		accuracy = accuracy - mispredicted;
		accuracy = accuracy / (1.0*labelsTesting.size());
		accuracy = accuracy * 100.00;
		output << threshold << "," << mispredicted << "," << unpredicted << "," << accuracy << endl;

		string acc = format("Number of Test Subjects is : %d\tNumber of Test Images is : %d \nAccuracy is %.3f %%", (labelsTesting[labelsTesting.size()-1] + 1),labelsTesting.size(),accuracy); 

		// Sometimes you'll need to get/set internal model data,
		Color::Modifier red(Color::FG_RED);
		Color::Modifier green(Color::FG_GREEN);
		Color::Modifier blue(Color::FG_BLUE);
		Color::Modifier def(Color::FG_DEFAULT);
		cout << green << acc << def << endl;

		threshold = threshold + change;
	}
	output.close();


    cout << "Model Information:" << endl;
    string model_info = format("\tLBPH(radius=%i, neighbors=%i, grid_x=%i, grid_y=%i, threshold=%.2f)",
            model->getInt("radius"),
            model->getInt("neighbors"),
            model->getInt("grid_x"),
            model->getInt("grid_y"),
            model->getDouble("threshold"));
    cout << model_info << endl;
    return 0;
}
