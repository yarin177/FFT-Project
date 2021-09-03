from numpy.lib.function_base import append
import scipy.io.wavfile
import matplotlib.pyplot as plt
import numpy as np
import csv
import random
from scipy.fftpack import fft
import scipy.signal as sig

def chunks(lst, n):
    """Yield successive n-sized chunks from lst."""
    for i in range(0, len(lst), n):
        yield lst[i:i + n]

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

def SamplerateConversion(slc):
    fs_audio = 44100
    fs = 176400
    samples = []
    time_vec = np.arange(0,10,1/fs)
    audio_chunks = list(chunks(slc,fs_audio))
    for i,single_chunk in enumerate(audio_chunks):
        for j in range(44100):
            samples.append(single_chunk[j])
            samples.append(0)
            samples.append(0)
            samples.append(0)
    return samples, time_vec

def generateSignalAM(slc,t):
    samples,time_vec = SamplerateConversion(slc)
    w = low_cut_filter(samples,176400,22050)

    TWO_PI = 2 * np.pi
    carrier_hz = 20000
    ac = 0.5

    carrier = np.exp(1j * TWO_PI * carrier_hz * time_vec)
    envelope = (1.0 + ac * w)
    modulated = envelope * carrier

    return modulated

def low_cut_filter(x, fs, cutoff=70):
    """FUNCTION TO APPLY LOW CUT FILTER

    Args:
        x (ndarray): Waveform sequence
        fs (int): Sampling frequency
        cutoff (float): Cutoff frequency of low cut filter

    Return:
        (ndarray): Low cut filtered waveform sequence
    """

    nyquist = fs // 2
    norm_cutoff = cutoff / nyquist

    # low cut filter
    fil = sig.firwin(255, norm_cutoff, pass_zero=True)
    lcf_x = sig.lfilter(fil, 1, x)

    return lcf_x 

def generateSignalFM(slc,t, fc=None, b=.3):
    slc /= (2*np.abs(slc).max())

    fs_audio = 44100
    fs = 176400
    samples = []
    time_vec = np.arange(0,10,1/fs)
    audio_chunks = list(chunks(slc,fs_audio))
    for i,single_chunk in enumerate(audio_chunks):
        for j in range(44100):
            samples.append(single_chunk[j])
            samples.append(0)
            samples.append(0)
            samples.append(0)

    #freq_vec = np.arange(0,fs_audio/2,fs_audio/(fs_audio*10))
    #freq_vec2 = np.arange(0,fs/2,fs/(fs*10))

    #flat_list = [item for sublist in samples for item in sublist]
    #flat_list = np.array(flat_list)

    w = low_cut_filter(samples,fs,22050)

    '''
    fft_out = fft(slc)
    B = fft_out[:len(fft_out)//2]

    fft_out2 = fft(w)
    D = fft_out2[:len(fft_out2)//2]

    fig, axes = plt.subplots(nrows=2, ncols=1)
    axes[0].plot(t, slc)
    axes[1].plot(time_vec, w)
    fig.tight_layout()
    plt.show()
    '''
    N = len(w)
    if fc is None:
        fc = 75000 # arbitrary

    x0 = w[:N]
    # ensure it's [-.5, .5] so diff(phi) is b*[-pi, pi]
    x0 /= (2*np.abs(x0).max())

    # generate phase
    phi0 = 2*np.pi * fc * time_vec
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
    chunks_data = list(chunks(data,200000))
    for i in range(len(chunks_data)):
        chunks_data[i] = chunks_data[i][:len(chunks_data[i])-155968]

    flat_list = [item for sublist in chunks_data for item in sublist]
    lst = list(chunks(flat_list,256))
    return lst

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

def writeCSV(file_name, data,path):
    len_training = int(len(data) * 0.9)
    len_testing = int(len(data) * 0.1)

    training_file_name = 'training' + file_name + str(len_training) + '.csv'
    f = open(path + training_file_name, 'w', newline='')
    writer = csv.writer(f)
    writer.writerows(data[0:len_training])
    f.close()

    testing_file_name = 'testing' + file_name + str(len_testing) + '.csv'
    f = open(path + testing_file_name, 'w', newline='')
    writer = csv.writer(f)
    writer.writerows(data[len_training:len_training+len_testing])
    f.close()

    print("Saved training file to '" +  path + training_file_name + "'")
    print("Saved testing file to '" + path + testing_file_name + "'")

#This function outputs a csv file with N/2 AM signal and N/2 FM signal randomized.
def generate_to_spec(N,writingDataAM,writingDataFM):
    spec_data = []
    length = int(N / 2)
    for i in range(length):
        spec_data.append(writingDataFM[i])
        spec_data.append(writingDataAM[i])
        
    #random.shuffle(spec_data)
    print(len(writingDataAM[0]))
    f = open('final2_samples.csv', 'w', newline='')
    writer = csv.writer(f)
    writer.writerows(spec_data)
    f.close()

def normalizeAudio(data):
    return np.float32((data / max(data)))

def printInfo(SAMPLE_FOR):
    length = SAMPLE_FOR * 176400
    total =  int(length / 256)
    len_training = str(int(total * 0.9))
    len_testing = str(int(total * 0.1))
    total = int(len_training) + int(len_testing)
    print('Dividing ' + str(length) + ' samples to chunks of 256 (' + str(total) + ' chunks total)')
    print(len_training + ' chunks will be used for training')
    print(len_testing + ' chunks will be used for testing\n')

def main():
    path = '../NeuralNetwork/TrainingTestingFiles/'
    print('Saving files to: ' + path)

    SAMPLE_FOR = 10 # in seconds
    samplerate, data = scipy.io.wavfile.read(r'Recording.wav')
    print('Sampling for ' + str(SAMPLE_FOR) + ' seconds with sample rate of ' + str(samplerate))
    data = data[0:int(samplerate*SAMPLE_FOR)]
    time = np.arange(0,SAMPLE_FOR,1/samplerate) #time vector
    data = normalizeAudio(data)
    printInfo(SAMPLE_FOR)

    #fm = generateSignalFM(data,time)
    am = generateSignalAM(data,time)
    
    spec_data = []
    for i in range(256):
        spec_data.append(am[i].real)

    for i in range(256):
        spec_data.append(am[i].imag)

    #random.shuffle(spec_data)
    print(len(spec_data))
    f = open('complex_signal.csv', 'w', newline='')
    writer = csv.writer(f)
    writer.writerows([spec_data])
    f.close()
    

    #writingDataFM = list(chunks(fm,256))
    #writingDataAM = list(chunks(am,256))
    #print(fm[0])
    #generate_to_spec(2,writingDataAM,writingDataFM)
    #writeCSV('FM',writingDataFM,path)
    #writeCSV('AM',writingDataAM,path)
    
if __name__ == "__main__":
    main()
