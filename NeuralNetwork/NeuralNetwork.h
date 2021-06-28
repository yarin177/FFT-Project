#include <iostream>
#include <Eigen/Dense>
#include <vector>
#include<ctime>

using Eigen::MatrixXf;
using namespace Eigen;
using std::vector;


class NeuralNetwork {

private:
	int input_nodes;
	int hidden_nodes;
	int output_nodes;
	float learning_rare;

	MatrixXf weights_ih;
	MatrixXf weights_ho;
	MatrixXf bias_h;
	MatrixXf bias_o;

public:
	NeuralNetwork(int input_nodes, int hidden_nodes, int output_nodes)
	{
		srand((unsigned int)time(0));

		this->input_nodes = input_nodes;
		this->hidden_nodes = hidden_nodes;
		this->output_nodes = output_nodes;

		this->weights_ih = MatrixXf::Random(this->hidden_nodes, this->input_nodes);
		this->weights_ho = MatrixXf::Random(this->output_nodes, this->hidden_nodes);

		this->bias_h = MatrixXf::Random(this->hidden_nodes, 1);
		this->bias_o = MatrixXf::Random(this->output_nodes, 1);

		learning_rare = 0.1;

	}
	float sigmoid(float x)
	{
		return 1 / (1 + exp(-x));
	}
	float dsigmoid(float y)
	{
		return y * (1 - y);
	}
	void map(MatrixXf& in)
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
	void dmap(MatrixXf& in)
	{
		float val = 0;
		for (int i = 0; i < in.rows(); i++)
		{
			for (int j = 0; j < in.cols(); j++)
			{
				val = in(i, j);
				in(i, j) = dsigmoid(val);
			}
		}
	}
	MatrixXf dmap_return(MatrixXf in)
	{
		MatrixXf out(in.rows(),in.cols());
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
	MatrixXf map_return(MatrixXf& in)
	{
		MatrixXf out(in.rows(), in.cols());
		float val = 0;
		for (int i = 0; i < in.rows(); i++)
		{
			for (int j = 0; j < in.cols(); j++)
			{
				val = in(i, j);
				out(i, j) = sigmoid(val);
			}
		}
		return out;
	}
	MatrixXf vectorToMatrix(vector<float> in)
	{
		MatrixXf m(in.size(), 1);
		for (int i = 0; i < in.size(); i++)
		{
			m(i, 0) = in[i];
		}
		return m;
	}
	vector<float> matrixToVector(MatrixXf in)
	{
		vector<float> output;
		for (int i = 0; i < in.rows(); i++)
		{
			for (int j = 0; j < in.cols(); j++)
			{
				output.push_back(in(i,j));
			}
		}
		return output;
	}
	vector<float> predict(vector<float> inputs_vector) {
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
		return matrixToVector(output);
		
	}
	MatrixXf multiply(MatrixXf in1, MatrixXf in2)
	{
		for (int i = 0; i < in1.rows(); i++) {
			for (int j = 0; j < in1.cols(); j++) {
				in1(i,j) *= in2(i, j);
			}
		}
		return in1;
	}
	void train(vector<float> inputs_vector, vector<float> answers)
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
		gradients *= output_errors;
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
		hidden_gradient = multiply(hidden_gradient, hidden_errors);
		hidden_gradient *= this->learning_rare;

		// Calcuate input->hidden deltas
		MatrixXf inputs_T = inputs.transpose();
		MatrixXf weight_ih_deltas = hidden_gradient * inputs_T;

		this->weights_ih += weight_ih_deltas;
		// Adjust the bias by its deltas (which is just the gradients)
		this->bias_h += hidden_gradient;
	}


};
