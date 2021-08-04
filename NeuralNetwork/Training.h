#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <iostream>  
#include <vector>
#include <fstream>
#include <string>

using std::vector;
using std::string;
using std::fstream;
using std::cin;
using std::cout;

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
