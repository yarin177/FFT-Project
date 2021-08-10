#include "NeuralNetwork.h"
#include "Training.h"
#include <algorithm>

const int FM = 0;
const int AM = 1;
const int TRAINING_AMOUNT = 4650;
const int TESTING_AMOUNT = 516;
const float OUTPUTS = 2.0;

int main()
{
	vector<vector<vector<float>>> files = readLocalFiles("trainingFM4650.csv", "trainingAM4650.csv", "testingFM516.csv", "testingAM516.csv");
	vector<vector<TrainingHandler>> training_testing_vectors = arrangeFiles(files);
	vector<vector<TrainingHandler>> shuffled_training_testing_vectors = getshuffledVectors(training_testing_vectors);

	vector<TrainingHandler> trainingData = shuffled_training_testing_vectors[0];
	vector<TrainingHandler> testingData = shuffled_training_testing_vectors[1];

	NeuralNetwork nn(256, 48, 2); // init a new Neural Network with 256 inputs, 48 hidden nodes and 3 outputs

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
