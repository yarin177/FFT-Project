#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <iostream>  
#include <vector>
#include <fstream>
#include <string>
#include <chrono>
#include <random>

using std::vector;
using std::string;
using std::fstream;
using std::cin;
using std::cout;

const string TrainingTestingPath = "TrainingTestingFiles/";

class DataHandler {
public:
	vector<vector<float>> read_csv(std::string filename) {
		vector<vector<float>> data;

		// Create an input filestream
		std::ifstream myFile(filename);

		std::string line;
		while (std::getline(myFile, line))
		{
			std::istringstream s(line);
			std::string field;
			vector<float> lineData;
			while (getline(s, field, ',')) {
				lineData.push_back(std::stof(field));
			}
			data.push_back(lineData);
			lineData.clear();
		}
		return data;
	}

};

class TrainingHandler {
private:
	int label;
	vector<float> data;

public:
	void setTrainingHandler(vector<float> data, int label)
	{
		this->data = data;
		this->label = label;
	}
	TrainingHandler getTrainingHandlerObject(vector<float> data, int label)
	{
		TrainingHandler tmp;
		tmp.setTrainingHandler(data, label);
		return tmp;
	}

	vector<float> getInputs() { return data; }
	int getTarget() { return  label; }
};

typedef struct LocalFile
{
	vector<vector<float>> data; // i.e vector with the size 512 data entries of 256
};

struct Files
{
	LocalFile training_fm;
	LocalFile training_am;
	LocalFile testing_fm;
	LocalFile testing_am;
} files;

struct SortedFiles
{
	vector<TrainingHandler> trainingData;
	vector<TrainingHandler> testingData;
} sorted_files;

void readLocalFiles(string training_fm_path, string training_am_path, string testing_fm_path, string testing_am_path) {
	//Load training and testing files
	DataHandler dh;

	LocalFile training_fm;
	LocalFile training_am;
	LocalFile testing_fm;
	LocalFile testing_am;

	std::cout << "Reading training files..." << std::endl;

	training_fm.data = dh.read_csv(TrainingTestingPath + training_fm_path);
	training_am.data = dh.read_csv(TrainingTestingPath + training_am_path);

	testing_fm.data = dh.read_csv(TrainingTestingPath + testing_fm_path);
	testing_am.data = dh.read_csv(TrainingTestingPath + testing_am_path);

	files.training_fm = training_fm;
	files.training_am = training_am;
	files.testing_fm = testing_fm;
	files.testing_am = testing_am;
}

void arrangeFiles(const int TRAINING_AMOUNT, const int TESTING_AMOUNT, const int FM = 0, const int AM = 1) {

	vector<TrainingHandler> trainingData;
	vector<TrainingHandler> testingData;

	TrainingHandler data_handler;

	for (int i = 0; i < TRAINING_AMOUNT; i++)
	{
		trainingData.push_back(data_handler.getTrainingHandlerObject(files.training_fm.data[i], FM));
		trainingData.push_back(data_handler.getTrainingHandlerObject(files.training_am.data[i], AM));
		if (i < TESTING_AMOUNT)
		{
			testingData.push_back(data_handler.getTrainingHandlerObject(files.testing_fm.data[i], FM));
			testingData.push_back(data_handler.getTrainingHandlerObject(files.testing_am.data[i], AM));
		}
	}
	sorted_files.trainingData = trainingData;
	sorted_files.testingData = testingData;

}

SortedFiles getshuffledVectors() {
	// shuffle the vectors
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine e(seed);
	std::shuffle(sorted_files.trainingData.begin(), sorted_files.trainingData.end(), e);
	std::shuffle(sorted_files.testingData.begin(), sorted_files.testingData.end(), e);

	return sorted_files;
}
