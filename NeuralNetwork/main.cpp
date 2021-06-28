#include "NeuralNetwork.h"
#include "Training.h"
#include <algorithm>
#include <random>
#include <chrono>       // std::chrono::system_clock

const int SQUARE = 0;
const int SIN = 1;
int main()
{
	DataHandler dh;
	std::cout << "Reading training files..." << std::endl;
	vector<vector<float>> training_square = dh.read_csv("trainingSquare1000.csv");
	vector<vector<float>> training_sin = dh.read_csv("trainingSin1000.csv");

	vector<TrainingHandler> trainingData;
	vector<TrainingHandler> testingData;
	vector<vector<float>> testing_square = dh.read_csv("testingSquare200.csv");
	vector<vector<float>> testing_sin = dh.read_csv("testingSin200.csv");

	for (int i = 0; i < 1000; i++)
	{
		TrainingHandler th(training_square[i],SQUARE);
		trainingData.push_back(th);
	}
	for (int i = 0; i < 1000; i++)
	{
		TrainingHandler th(training_sin[i], SIN);
		trainingData.push_back(th);
	}

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine e(seed);
	std::shuffle(trainingData.begin(), trainingData.end(), e);

	for (int i = 0; i < 200; i++)
	{
		TrainingHandler th(testing_square[i], SQUARE);
		testingData.push_back(th);
	}
	for (int i = 0; i < 200; i++)
	{
		TrainingHandler th(testing_sin[i], SIN);
		testingData.push_back(th);
	}
	std::shuffle(testingData.begin(), testingData.end(), e);

	std::cout << "Starting to train the Neural Network" << std::endl;
	NeuralNetwork nn(256, 48, 1);
	for (int i = 0; i < 2000; i++)
	{
		nn.train(trainingData[i].getInputs(), trainingData[i].getTarget());
	}
	
	std::cout << "Testing Neural Network" << std::endl;

	int correct = 0;
	for (int i = 0; i < 200; i++)
	{
		vector<float> guess = nn.predict(testingData[i].getInputs());
		if (guess[0] >= 0.5 && testingData[i].getTarget()[0] == 1 || guess[0] < 0.5 && testingData[i].getTarget()[0] == 0)
		{
			correct++;
		}
	}
	float perccent = correct / 200.0;
	std::cout << "% correct: " << perccent << std::endl;
	std::cout << "Now training for another epoch" << std::endl;
	std::shuffle(trainingData.begin(), trainingData.end(), e);
	for (int i = 0; i < 2000; i++)
	{
		nn.train(trainingData[i].getInputs(), trainingData[i].getTarget());
	}
	correct = 0;
	for (int i = 0; i < 200; i++)
	{
		vector<float> guess = nn.predict(testingData[i].getInputs());
		if (guess[0] >= 0.5 && testingData[i].getTarget()[0] == 1 || guess[0] < 0.5 && testingData[i].getTarget()[0] == 0)
		{
			correct++;
		}
	}
	perccent = correct / 200.0;
	std::cout << "% correct: " << perccent << std::endl;
	return 0;
}
