






# Neural Network Installation process:

Start by cloning this git repository by running the following command in the terminal:

**`git clone https://github.com/yarin177/FFT-Project.git`**

This project uses [Eigen library](https://eigen.tuxfamily.org/) for linear algebra calculations, to install it run the following command in the terminal:

**`sudo apt install libeigen3-dev`**

Head over to [PythonAudioProcessing](https://github.com/yarin177/FFT-Project/tree/main/PythonAudioProcessing) folder through the terminal (`cd FFT-Project/PythonAudioProcessing`)

And install python script dependencies by running:

**`pip install -r requirements.txt`**

**Note**: **Python >=3.6** is required for this project.

Lastly, check that you have CMake version >3.8 installed, if not run the following line:

**`sudo apt install cmake`**

## Quick Start:
### Generating Training and Testing files
Now it's time to run the script by running the following command in the [PythonAudioProcessing](https://github.com/yarin177/FFT-Project/tree/main/PythonAudioProcessing) folder:

**`python process_audio_sounds.py`**

This script reads an audio wav file and performs Amplitude Modulation & Frequency Modulation, then it splits the data into 4  CSV files which would be used for training and testing the Neural Network.


These files will be saved in the default path location which is the [TrainingTestingFiles](https://github.com/yarin177/FFT-Project/tree/main/NeuralNetwork/TrainingTestingFiles) folder.

### Training and Testing the Neural Network
Now we are all set to compile and run the Neural Network! :)
Head over to the [NeuralNetwork](https://github.com/yarin177/FFT-Project/tree/main/NeuralNetwork) folder.

Make sure that the **TrainingTestingFiles** folder includes 4 csv files:
 - testingAM516.csv 
 - testingFM516.csv  
 - trainingAM4650.csv 
 - trainingFM4650.csv

These are the training and testing files we generated earlier.

If you use **Linux**, change **NeuralNetwork.cpp** second line to include the right path to the Eigen folder we installed earlier, **default path** for Linux system is **/usr/include/eigen3/Eigen/Dense**.

To compile and run the program with CMake run the following line:

**`cmake --build build && ./build/main`**
# Spectrogram Installation 

The project uses **Qt framework**  library  to output a  spectrogram.
Install it on **ubuntu** by running this line in the terminal:

**`$ sudo apt-get install qt5-default`**

<img src="https://upload.wikimedia.org/wikipedia/commons/thumb/0/0b/Qt_logo_2016.svg/1200px-Qt_logo_2016.svg.png" alt="drawing" width="200"/>

## Compiling and running 
#### Assuming you have these  3 files in your current directory:

>fft.h

>main.cpp

>main.pro

### FIrstly, run:

**`$ qmake`** 

#### **"Makefile"** file should be created.

#### Then, you can simply compile and run the program by running this line each time:

**`$ make && ./spectrogram`**

### End
