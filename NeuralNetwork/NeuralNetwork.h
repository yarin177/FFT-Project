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

	float sigmoid(float x);
	float dsigmoid(float y);
	void map(MatrixXf& in);
	MatrixXf dmap_return(MatrixXf in);
	MatrixXf vectorToMatrix(vector<float> in);
	vector<float> matrixToVector(MatrixXf in);
	vector<float> predict(vector<float> inputs_vector);
	void train(vector<float> inputs_vector, vector<float> answers);
};
