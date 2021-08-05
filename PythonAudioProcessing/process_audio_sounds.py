import scipy.io.wavfile
import matplotlib.pyplot as plt
import numpy as np
import csv

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

def generateSignalAM(data,time_vector):

    TWO_PI = 2 * np.pi
    carrier_hz = 20000
    ac = 0.5

    carrier = np.sin(TWO_PI * carrier_hz * time_vector)

    envelope = (1.0 + ac * data)
    modulated = envelope * carrier
    return modulated

def generateSignalFM(slc,t, fc=None, b=.3):
    N = len(slc)
    if fc is None:
        fc = 75000 # arbitrary

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

def writeCSV(file_name, data):
    len_training = int(len(data) * 0.9)
    len_testing = int(len(data) * 0.1)

    training_file_name = 'training' + file_name + str(len_training) + '.csv'
    f = open(training_file_name, 'w', newline='')
    writer = csv.writer(f)
    writer.writerows(data[0:len_training])
    f.close()

    testing_file_name = 'testing' + file_name + str(len_testing) + '.csv'
    f = open(testing_file_name, 'w', newline='')
    writer = csv.writer(f)
    writer.writerows(data[len_training:len_training+len_testing])
    f.close()

    print("Saved training file as '" + training_file_name + "'")
    print("Saved testing file as '" + testing_file_name + "'")

def normalizeAudio(data):
    return np.float32((data / max(data)))

def main():
    SAMPLE_FOR = 30 # in seconds
    samplerate, data = scipy.io.wavfile.read(r'Recording.wav')
    print('Sampling for ' + str(SAMPLE_FOR) + ' seconds with sample rate of ' + str(samplerate))
    data = data.astype('float64')
    data = data[0:int(samplerate*SAMPLE_FOR)]
    time = np.arange(0,SAMPLE_FOR,1/samplerate) #time vector
    data = normalizeAudio(data)
    data /= (2*np.abs(data).max())
    print('Dividing ' + str(len(data)) + ' samples to chunks of 256 (' + str(int(len(data) / 256)) + ' chunks total)')
    len_training = str(int((len(data) / 256) * 0.9))
    len_testing = str(int((len(data) / 256) * 0.1))
    print(len_training + ' chunks will be used for training')
    print(len_testing + ' chunks will be used for testing\n')
    fm = generateSignalFM(data,time)
    am = generateSignalAM(data,time)
    writingDataFM = getWritingData(SAMPLE_FOR,samplerate,fm)
    writingDataAM = getWritingData(SAMPLE_FOR,samplerate,am)

    writeCSV('FM',writingDataFM)
    writeCSV('AM',writingDataAM)
    
if __name__ == "__main__":
    main()

