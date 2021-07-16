#include "NeuralNetwork.h"


float NeuralNetwork::sigmoid(float x)
{
	return 1 / (1 + exp(-x));
}
float NeuralNetwork::dsigmoid(float y)
{
	return y * (1 - y);
}

void NeuralNetwork::map(MatrixXf& in)
{
	float val = 0;
	for (int i = 0; i < in.rows(); i++)
	{
		for (int j = 0; j < in.cols(); j++)
		{
			val = in(i, j);
			in(i, j) = sigmoid(val);
		}
	}
}
MatrixXf NeuralNetwork::dmap_return(MatrixXf in)
{
	MatrixXf out(in.rows(), in.cols());
	float val = 0;
	for (int i = 0; i < in.rows(); i++)
	{
		for (int j = 0; j < in.cols(); j++)
		{
			val = in(i, j);
			out(i, j) = dsigmoid(val);
		}
	}
	return out;
}
MatrixXf NeuralNetwork::vectorToMatrix(vector<float> in)
{
	MatrixXf m(in.size(), 1);
	for (int i = 0; i < in.size(); i++)
	{
		m(i, 0) = in[i];
	}
	return m;
}
vector<float> NeuralNetwork::matrixToVector(MatrixXf in)
{
	vector<float> output;
	for (int i = 0; i < in.rows(); i++)
	{
		for (int j = 0; j < in.cols(); j++)
		{
			output.push_back(in(i, j));
		}
	}
	return output;
}
vector<float> NeuralNetwork::predict(vector<float> inputs_vector) {
	// Generating the Hidden Outputs
	MatrixXf inputs = vectorToMatrix(inputs_vector);
	MatrixXf hidden = this->weights_ih * inputs;
	hidden += this->bias_h;
	// activation function!
	map(hidden);

	// Generating the output's output!
	MatrixXf output = this->weights_ho * hidden;
	output += this->bias_o;
	map(output);
	
	vector<float> results = matrixToVector(output);
	vector<float> softmax;
	float denominator = 0;
	for (int i = 0; i < results.size(); i++)
	{
		denominator += results[i];
	}
	for (int i = 0; i < results.size(); i++)
	{
		softmax.push_back(results[i] / denominator);
	}
	return softmax;

}
void NeuralNetwork::train(vector<float> inputs_vector, vector<float> answers)
{
	// Generating the Hidden Outputs

	MatrixXf inputs = vectorToMatrix(inputs_vector);
	MatrixXf hidden = this->weights_ih * inputs;
	hidden += this->bias_h;
	// activation function!
	map(hidden);

	// Generating the output's output!
	MatrixXf outputs = this->weights_ho * hidden;
	outputs += this->bias_o;
	map(outputs);

	// Convert array to matrix object
	MatrixXf targets = vectorToMatrix(answers);
	// Calculate the error
	// ERROR = TARGETS - OUTPUTS
	MatrixXf output_errors = targets - outputs;
	// let gradient = outputs * (1 - outputs);
	// Calculate gradient
	MatrixXf gradients = dmap_return(outputs);

	ArrayXXf temp1 = gradients;
	ArrayXXf temp2 = output_errors;

	temp1 *= temp2;
	gradients = temp1;
	output_errors = temp2;

	gradients *= this->learning_rare;

	// Calculate deltas
	MatrixXf hidden_T = hidden.transpose();
	MatrixXf weight_ho_deltas = gradients * hidden_T;

	// Adjust the weights by deltas
	this->weights_ho += weight_ho_deltas;

	// Adjust the bias by its deltas (which is just the gradients)
	this->bias_o += gradients;

	// Calculate the hidden layer errors
	MatrixXf who_t = this->weights_ho.transpose();
	MatrixXf hidden_errors = who_t * output_errors;

	// Calculate hidden gradient
	MatrixXf hidden_gradient = dmap_return(hidden);
	temp1 = hidden_gradient;
	temp2 = hidden_errors;
	temp1 *= temp2;

	hidden_gradient = temp1;
	hidden_errors = temp2;
	hidden_gradient *= this->learning_rare;

	// Calcuate input->hidden deltas
	MatrixXf inputs_T = inputs.transpose();
	MatrixXf weight_ih_deltas = hidden_gradient * inputs_T;

	this->weights_ih += weight_ih_deltas;
	// Adjust the bias by its deltas (which is just the gradients)
	this->bias_h += hidden_gradient;
}
