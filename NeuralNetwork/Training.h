#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <iostream>  
#include <vector>
#include <fstream>
#include <string>
#include <chrono>
#include <random>
#include <corecrt_math_defines.h>
#include "Iir.h"

using std::vector;
using std::string;
using std::fstream;
using std::cin;
using std::cout;
using std::complex;

const string TrainingTestingPath = "TrainingTestingFiles/";

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

typedef vector<complex<float>> complexSignal;
typedef vector<vector<float>> trainingChunk; 

void write_csv(std::string filename, vector<TrainingHandler> all_training) {
	std::ofstream myFile(filename);
	//std::ofstream myFileTest("testing_data.csv");
	// Send data to the stream
	for (int i = 0; i < all_training.size(); i++)
	{
		int input = all_training[i].getTarget() + 1;
		vector<float> training_data = all_training[i].getInputs();
		for (int j = 0; j < 512; j++)
		{
			myFile << training_data.at(j) << ",";
		}
		myFile << input << "\n";
	}
	myFile.close();
	std::cout << "Wrote File!" << std::endl;
}

vector<complexSignal> readLocalFile(string file_path) {
	//Reads a local csv file that is located in TrainingTestingPath/<file_path>
	vector<float> data;
	vector<complexSignal> comp;
	// Create an input filestream
	std::ifstream myFile(file_path);
	std::string line;
	int counter = 0;
	int split_size = 1260000; 
	while (std::getline(myFile, line))
	{
		std::istringstream s(line);
		std::string field;
		vector<float> lineData;
		complexSignal temp;

		while (std::getline(s, field, ',')) {
			//data.push_back(std::stof(field));
			float value = std::stof(field);
			if (counter < split_size)
				temp.push_back({ value , 0 });
			else
				temp[counter - split_size].imag(value);
			counter++;
		}
		comp.push_back(temp);
		counter = 0;
		temp.clear();
	}
	return comp;
}
vector<int> getPossibilitiesShuffled()
{
	vector<int> possibilities;
	std::cout << "Generating possibilities" << std::endl;
	for (int i = 0; i < 1260000; i = i + 12600)
	{
		possibilities.push_back(i);
	}
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine e(seed);
	std::shuffle(std::begin(possibilities), std::end(possibilities), e);
	return possibilities;
}
complexSignal frequencyMixer(complexSignal data, const int frequency, const float Fs)
{
	complexSignal chunk(data.size());
	const double  T = 1 / Fs; // At what intervals time points are sampled
	for (int i = 0; i < chunk.size(); i++)
	{
		chunk[i] = { (float)(1 * cos(2 * M_PI * frequency * (i * T))),(float)(1 * sin(2 * M_PI * frequency * (i * T))) };
		data[i] = data[i] * chunk[i];
	}
	return data;
}
vector<double> doFilter(Iir::Butterworth::BandPass<3> f, const complexSignal samples, int N, bool real_part)
{
	//Applies a Bandpass filter to a 1d array with the size of N
	vector<double> to_return;
	float part;
	for (int i = 0; i < N; i++)
	{
		if (real_part)
			part = f.filter(samples[i].real());
		else
			part = f.filter(samples[i].imag());
		to_return.push_back(part);
	}
	return to_return;

}
trainingChunk filterChunk(complexSignal signal, int fcenter)
{
	complexSignal data;
	trainingChunk to_return;

	signal = frequencyMixer(signal, -fcenter + 504000, 1260000);

	int Fs = 1260000;
	int BW = 12600;

	vector<float> tmp;
	vector<float> tmp2;

	Iir::Butterworth::BandPass<3> f_real;
	Iir::Butterworth::BandPass<3> f_imag;

	f_real.setup(Fs, 504000, BW);
	f_imag.setup(Fs, 504000, BW);

	vector<double> real_filtered = doFilter(f_real, signal, 100000, 1);
	vector<double> imag_filtered = doFilter(f_imag, signal, 100000, 0);

	for (int i = 0; i < 100000; i++)
	{
		data.push_back({ (float)real_filtered[i], (float)imag_filtered[i] });
	}

	complexSignal final = frequencyMixer(data, -504000, 1260000);
	for (int j = 0; j < 200; j++)
	{

		for (int i = j * 256; i < (j * 256) + 256; i++)
			tmp.push_back(final[i].real());

		for (int i = j * 256; i < (j * 256) + 256; i++)
			tmp.push_back(final[i].imag());


		to_return.push_back(tmp);
		tmp.clear();
	}
	return to_return;

}
vector<TrainingHandler> prepareTrainingData(complexSignal signal_fm, complexSignal signal_am, vector<int> possibilities)
{
	vector<trainingChunk> training_fm;
	vector<trainingChunk> training_am;
	complexSignal all_channels;
	TrainingHandler temp;

	vector<TrainingHandler> all_training;

	for (int i = 0; i < 50; i++)
	{
		complexSignal fm_temp = frequencyMixer(signal_fm, possibilities[i], 1260000);
		complexSignal am_temp = frequencyMixer(signal_am, possibilities[i + 50], 1260000);

		for (int j = 0; j < 100000; j++)
		{
			all_channels.push_back(fm_temp[j] + am_temp[j]);
		}

		training_fm.push_back(filterChunk(all_channels, possibilities[i]));
		training_am.push_back(filterChunk(all_channels, possibilities[i + 50]));

		all_channels.clear();


		std::cout << "Done: " << i << std::endl;
	}

	for (int i = 0; i < 50; i++)
	{
		for (int j = 0; j < 200; j++)
		{
			temp.setTrainingHandler(training_fm[i][j], 0);
			all_training.push_back(temp);

			temp.setTrainingHandler(training_am[i][j], 1);
			all_training.push_back(temp);
		}
	}
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine e(seed);
	std::shuffle(std::begin(all_training), std::end(all_training), e);
	return all_training;
}
void Train(vector<TrainingHandler> training_data, NeuralNetwork& nn)
{
	std::cout << "Training NeuralNetwork with " << training_data.size() << " chunks" << std::endl;

	for (int j = 0; j < training_data.size(); j++)
	{
		vector<float> targets = { 0,0 };
		targets[training_data[j].getTarget()] = 1;
		nn.train(training_data[j].getInputs(), targets);
	}
}
void Test(vector<TrainingHandler> testing_data, NeuralNetwork& nn)
{
	int correct = 0;
	int average = 0;
	int perccent, sure = 0;
	std::cout << "Testing NeuralNetwork with " << testing_data.size() << " chunks" << std::endl;

	for (int j = 0; j < testing_data.size(); j++)
	{
		vector<float> guess = nn.predict(testing_data[j].getInputs());
		int maxIndex = std::distance(guess.begin(), std::max_element(guess.begin(), guess.end()));
		if (maxIndex == testing_data[j].getTarget())
		{
			correct++;
			average += guess[maxIndex];
		}

	}
	perccent = correct / (float)testing_data.size();
	sure = average / correct;
	std::cout << "% correct: " << perccent << " Average sure: " << sure << std::endl;
	std::cout << "% correct: " << correct << " " << testing_data.size() << std::endl;
}
