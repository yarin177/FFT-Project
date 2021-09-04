#include "NeuralNetwork.h"
#include "Training.h"
#include <algorithm>

const int FM = 0;
const int AM = 1;
const int TRAINING_AMOUNT = 6201;
const int TESTING_AMOUNT = 689;
const int INPUTS = 512;
const float OUTPUTS = 2.0;

int main()
{
	readLocalFiles("trainingFM6201.csv", "trainingAM6201.csv", "testingFM689.csv", "testingAM689.csv");
	arrangeFiles(TRAINING_AMOUNT, TESTING_AMOUNT);
	SortedFiles shuffled_files = getshuffledVectors();

	vector<TrainingHandler> trainingData = shuffled_files.trainingData;
	vector<TrainingHandler> testingData = shuffled_files.testingData;

	NeuralNetwork nn(INPUTS, 48, 2); // init a new Neural Network with 512 inputs, 48 hidden nodes and 2 outputs

	std::cout << "Starting to train the Neural Network" << std::endl;
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
	nn.saveModel();

	return 0;
}
