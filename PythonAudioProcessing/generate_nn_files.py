import scipy.io.wavfile
import matplotlib.pyplot as plt
import numpy as np
import csv
from scipy.signal import chirp

def plot_graph(t,data):
    plt.plot(t,data)
    plt.xlabel("Time(s)")
    plt.ylabel("Amplitude")
    plt.show()

def generateSignalAM(time_vector,data):

    TWO_PI = 2 * np.pi
    carrier_hz = 20000
    ac = 0.5

    carrier = np.sin(TWO_PI * carrier_hz * time_vector)

    envelope = (1.0 + ac * data)
    modulated = envelope * carrier
    return modulated

def getWritingData(audio_length,sample_rate,data):
    all_freq = []
    temp = []
    count = 0
    for i in range(0,audio_length*sample_rate,sample_rate):
        for j in range(i,i+sample_rate):
            temp.append(data[j])
            count += 1
            if count == 256:
                all_freq.append(temp)
                count = 0
                temp = []
    return all_freq

def writeAudio(file_name, sample_rate, data):
    modulator_ints = np.int16(data * 32767) # increase volume
    scipy.io.wavfile.write(file_name + '.wav', sample_rate, modulator_ints)

def readAudioFile(file_name, sample_for,start_time=0):
    sample_rate, data = scipy.io.wavfile.read(file_name)
    end_time = int(start_time + (sample_rate * sample_for)) # sample for 1 second
    split_data = normalizeAudio(data[start_time:end_time])
    time = np.arange(0,sample_for,1/sample_rate) #time vector
    return time,split_data, sample_rate

def writeCSV(file_name, start_index, end_index, data):
    f = open(file_name + '.csv', 'w', newline='')
    writer = csv.writer(f)
    writer.writerows(data[start_index:end_index])

def normalizeAudio(data):
    return np.float32(data / max(data))

def main():
    SAMPLE_FOR = 30 # in seconds

    time_vector, data, sample_rate = readAudioFile('Recording.wav',SAMPLE_FOR)
    amp = generateSignalAM(time_vector,data)
    writingData = getWritingData(SAMPLE_FOR,sample_rate,amp)
    writeCSV('trainingAM4500',0,4500,writingData)
    
if __name__ == "__main__":
    main()

