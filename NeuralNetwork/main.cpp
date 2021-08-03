#include "NeuralNetwork.h"
#include "Training.h"
#include <algorithm>
#include <random>
#include <chrono>       // std::chrono::system_clock

const int FM = 0;
const int AM = 1;
const int TRAINING_AMOUNT = 4650;
const int TESTING_AMOUNT = 516;
const float OUTPUTS = 2.0;

int main()
{
	//Load training and testing files
	DataHandler dh;
	std::cout << "Reading training files..." << std::endl;
	vector<vector<float>> training_fm = dh.read_csv("trainingFM4650.csv");
	vector<vector<float>> training_am = dh.read_csv("trainingAM4650.csv");

	vector<TrainingHandler> trainingData;
	vector<TrainingHandler> testingData;
	TrainingHandler data_handler;

	vector<vector<float>> testing_fm = dh.read_csv("testingFM516.csv");
	vector<vector<float>> testing_am = dh.read_csv("testingAM516.csv");

	//arrange all the data in one training vector and one testing vector
	for (int i = 0; i < TRAINING_AMOUNT; i++)
	{
		trainingData.push_back(data_handler.getTrainingHandlerObject(training_fm[i], FM));
		trainingData.push_back(data_handler.getTrainingHandlerObject(training_am[i], AM));
		if (i < TESTING_AMOUNT)
		{
			testingData.push_back(data_handler.getTrainingHandlerObject(testing_fm[i], FM));
			testingData.push_back(data_handler.getTrainingHandlerObject(testing_am[i], AM));
		}
	}

	// shuffle the vectors
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine e(seed);
	std::shuffle(trainingData.begin(), trainingData.end(), e);
	std::shuffle(testingData.begin(), testingData.end(), e);

	std::cout << "Starting to train the Neural Network" << std::endl;
	NeuralNetwork nn(256, 48, 2); // init a new Neural Network with 256 inputs, 48 hidden nodes and 3 outputs
	for (int i = 0; i < TRAINING_AMOUNT * OUTPUTS; i++)
	{
		vector<float> targets = { 0,0 };
		targets[trainingData[i].getTarget()] = 1;
		nn.train(trainingData[i].getInputs(), targets);
	}

	std::cout << "Testing Neural Network" << std::endl;
	float average = 0;
	int correct = 0;
	for (int i = 0; i < TESTING_AMOUNT * OUTPUTS; i++)
	{
		vector<float> guess = nn.predict(testingData[i].getInputs());
		int maxIndex = std::distance(guess.begin(), std::max_element(guess.begin(), guess.end()));
		if (maxIndex == testingData[i].getTarget())
		{
			correct++;
			average += guess[maxIndex];
		}
	}
	float perccent = correct / (TESTING_AMOUNT * OUTPUTS);
	float sure = average / correct;
	std::cout << "% correct: " << perccent << " Average sure: " << sure << std::endl;

	for (int h = 0; h < 3; h++) {
		std::cout << "Now training for another epoch" << std::endl;
		std::shuffle(trainingData.begin(), trainingData.end(), e);
		for (int i = 0; i < TRAINING_AMOUNT * OUTPUTS; i++)
		{
			vector<float> targets = { 0,0 };
			targets[trainingData[i].getTarget()] = 1;
			nn.train(trainingData[i].getInputs(), targets);
		}
		correct = 0;
		average = 0;
		for (int i = 0; i < TESTING_AMOUNT * OUTPUTS; i++)
		{
			vector<float> guess = nn.predict(testingData[i].getInputs());
			int maxIndex = std::distance(guess.begin(), std::max_element(guess.begin(), guess.end()));
			if (maxIndex == testingData[i].getTarget())
			{
				correct++;
				average += guess[maxIndex];
			}
		}
		perccent = correct / (TESTING_AMOUNT * OUTPUTS);
		sure = average / correct;
		std::cout << "% correct: " << perccent << " Average sure: " << sure << std::endl;
	}

	return 0;
}
