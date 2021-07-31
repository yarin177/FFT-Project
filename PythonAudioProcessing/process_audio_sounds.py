from numpy.lib.shape_base import split
import scipy.io.wavfile
import matplotlib.pyplot as plt
import numpy as np
import csv
from scipy import signal
import scipy.integrate as integrate
import statistics

def plot_graph(t,data):
    plt.plot(t,data)
    plt.xlabel("Time(s)")
    plt.ylabel("Amplitude")
    plt.show()

def plot_graph2d(t1,data1,t2,data2):

    plt.plot(t2,data2)
    plt.xlabel("Time(s)")
    plt.ylabel("Amplitude")
    plt.plot(t1,data1)
    plt.legend(['FM Signal', 'Original Signal'], loc='lower right')
    plt.show()

def generateSignalAM(time_vector,data):

    TWO_PI = 2 * np.pi
    carrier_hz = 20000
    ac = 0.5

    carrier = np.sin(TWO_PI * carrier_hz * time_vector)

    envelope = (1.0 + ac * data)
    modulated = envelope * carrier
    return modulated

def frequency_modulate(slc,t, fc=None, b=.3):
    N = len(slc)
    if fc is None:
        fc = N / 18  # arbitrary

    x0 = slc[:N]
    # ensure it's [-.5, .5] so diff(phi) is b*[-pi, pi]
    x0 /= (2*np.abs(x0).max())

    # generate phase
    phi0 = 2*np.pi * fc * t
    phi1 = 2*np.pi * b * np.cumsum(x0)
    phi = phi0 + phi1
    diffmax  = np.abs(np.diff(phi)).max()
    # `b` correction
    if diffmax >= np.pi:
        diffmax0 = np.abs(np.diff(phi0)).max()
        diffmax1 = np.abs(np.diff(phi1)).max()
        phi1 *= ((np.pi - diffmax0) / diffmax1)
        phi = phi0 + phi1

    # modulate
    x = np.cos(phi)
    return x


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
    start_time = int(start_time * sample_rate)
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

def averageAudio(data):
    return np.float32(data / statistics.mean(data))

def main():
    SAMPLE_FOR = 30 # in seconds

    time_vector, data, sample_rate = readAudioFile('Recording.wav',SAMPLE_FOR,0)
    data /= (2*np.abs(data).max())
    fm = frequency_modulate(data,time_vector,fc=0,b=5)
    plot_graph2d(time_vector,data,time_vector,fm)
    #mod = generateSignalAM(split_time,split_data)
    #plot_graph2d(split_time,split_data,split_time,mod)
    #plot_graph(split_time,fm)
    #amp = generateSignalFM(time_vector,data)
    #writingData = getWritingData(SAMPLE_FOR,sample_rate,amp)
    #writeCSV('trainingAM4500',0,4500,writingData)
    
if __name__ == "__main__":
    main()

