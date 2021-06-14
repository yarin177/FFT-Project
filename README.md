### Project description

##### At the moment, this projects calculates a complex sine wave and outputs a spectrogram.
**Note:** This project is currently being developed.


## Installation process:

- Make sure you cloned the git repository as it is needed to compile the program.
- The project uses **Qt framework**  library  to output a  spectrogram.
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
