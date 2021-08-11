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

vector<vector<vector<float>>> readLocalFiles(string training_fm_path, string training_am_path, string testing_fm_path, string testing_am_path) {
	//Load training and testing files
	DataHandler dh;
	vector<vector<vector<float>>> data;
	std::cout << "Reading training files..." << std::endl;

	vector<vector<float>> training_fm = dh.read_csv(TrainingTestingPath + training_fm_path);
	vector<vector<float>> training_am = dh.read_csv(TrainingTestingPath + training_am_path);

	vector<vector<float>> testing_fm = dh.read_csv(TrainingTestingPath + testing_fm_path);
	vector<vector<float>> testing_am = dh.read_csv(TrainingTestingPath + testing_am_path);

	data.push_back(training_fm);
	data.push_back(training_am);
	data.push_back(testing_fm);
	data.push_back(testing_am);
	return data;
}

vector<vector<TrainingHandler>> arrangeFiles(vector<vector<vector<float>>> training_testing_data, const int FM = 0, const int AM = 1, const int TRAINING_AMOUNT = 4650, const int TESTING_AMOUNT = 516) {

	vector<TrainingHandler> trainingData;
	vector<TrainingHandler> testingData;
	vector<vector<TrainingHandler>> data;
	TrainingHandler data_handler;

	for (int i = 0; i < TRAINING_AMOUNT; i++)
	{
		trainingData.push_back(data_handler.getTrainingHandlerObject(training_testing_data[0][i], FM));
		trainingData.push_back(data_handler.getTrainingHandlerObject(training_testing_data[1][i], AM));
		if (i < TESTING_AMOUNT)
		{
			testingData.push_back(data_handler.getTrainingHandlerObject(training_testing_data[2][i], FM));
			testingData.push_back(data_handler.getTrainingHandlerObject(training_testing_data[3][i], AM));
		}
	}
	data.push_back(trainingData);
	data.push_back(testingData);
	return data;
}

vector<vector<TrainingHandler>> getshuffledVectors(vector<vector<TrainingHandler>> training_testing_vectors) {
	// shuffle the vectors
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine e(seed);
	std::shuffle(training_testing_vectors[0].begin(), training_testing_vectors[0].end(), e);
	std::shuffle(training_testing_vectors[1].begin(), training_testing_vectors[1].end(), e);

	return training_testing_vectors;
}
