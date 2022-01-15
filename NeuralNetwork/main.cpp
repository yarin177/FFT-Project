#include "NeuralNetwork.h"
#include "Training.h"
#include <algorithm>

const int FM = 0;
const int AM = 1;
const int INPUTS = 512;
const float OUTPUTS = 2.0;

int main()
{
	NeuralNetwork nn(INPUTS, 48, 2); // init a new Neural Network with 512 inputs, 48 hidden nodes and 2 outputs
	//NeuralNetwork nn;

	std::cout << "Reading training files... " << std::endl;

	vector<complexSignal> fm_filtered = readLocalFile("FM_signal.csv");
	vector<complexSignal> am_filtered = readLocalFile("AM_signal.csv");

	vector<int> possibilities = getPossibilitiesShuffled();

	vector<TrainingHandler> all_training = prepareTrainingData(fm_filtered[0], am_filtered[0], possibilities);
	Train(all_training, nn);
	Test(all_training, nn);
	 

	char ans;
	cout << "Save model? (y/n): ";
	cin >> ans;

	if (ans == 'y')
		nn.saveModel("../Model.txt");
	
	return 0;
	
}
