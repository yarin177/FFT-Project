import scipy
import numpy as np
#import matplotlib.pyplot as plt
import scipy.io.wavfile
from scipy.fftpack import fft
from scipy.signal import butter, lfilter, freqz
import scipy.signal as sig
import csv
import wave, struct, math

def normalizeAudio(data):
    normalized = np.complex128((data / max(data)))
    return normalized

def normalizeAudio_real(data):
    normalized = np.float16((data / max(data)))
    return normalized

def chunks(lst, n):
    """Yield successive n-sized chunks from lst."""
    for i in range(0, len(lst), n):
        yield lst[i:i + n]

def compute_hcf(x, y):
# choose the smaller number
    if x > y:
        smaller = y
    else:
        smaller = x
    for i in range(1, smaller+1):
        if((x % i == 0) and (y % i == 0)):
            hcf = i 
    return hcf

def generateSignalFM(slc, time_vec):
    b = 0.001
    slc /= (2*np.abs(slc).max())
    N = len(slc)
    fc = 0

    x0 = slc[:N]
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
    x = np.exp(1j * phi)
    #normilized_fm = normalizeAudio(x)

    return x

def generateSignalAM(samples):
    Z = np.zeros(len(samples), dtype=complex)

    for i in range(len(samples)):
        value = samples[i] + 1
        Z[i] = value + 0j 

    #normilized_am = normalizeAudio(Z)
    return Z

def SamplerateConversion(samples, new_fs, old_fs):
    L = int(new_fs / 100)
    M = int(old_fs / 100)

    hcf = compute_hcf(L,M)
    L = int(L / hcf)
    M = int(M / hcf)

    if L > 1 and M == 1:
        inter = interpolation(samples,L,old_fs)
        w = low_cut_filter(inter,new_fs,22050)
        return w
    elif L == 1 and M > 1:
        dec = decimation(samples,M,old_fs)
        return dec
    elif L > 1 and M > 1:
        inter = interpolation(samples,L,old_fs)
        filtered_inter = low_cut_filter(inter,old_fs * L,22050)
        dec = decimation(filtered_inter,M)
        return dec

def interpolation(samples,num, old_fs):
    inter_samples = []

    chunks_samp = list(chunks(samples,old_fs))
    for chunk in chunks_samp:
        for j in chunk:
            inter_samples.append(j)
            for h in range(num-1):
                inter_samples.append(0)
    return inter_samples

def decimation(samples,num):
    inter_samples = []
    i = num - 1
    for j in samples:
        if i == num-1:
            inter_samples.append(j)
            i = 0
        else:
            i += 1
    return inter_samples

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

def ComplexToList(type, Fs):
    """Function to split complex array into a list of <type>

    Args:
        complex_arr (ndarray): The complex arr, should contain complex sampels
        type (string): The type part that should be returned. real\imag

    Return:
        (list): 1D list of the desired type
    """

    max_val = int(Fs / 1260000) * 1260000
    lst = []
    tmp = []
    for i in range(0,max_val,1260000):
        for j in range(1260000):
            tmp.append(type[j+i].real)
        for j in range(1260000):
            tmp.append(type[j+i].imag)
        lst.append(tmp)
        tmp = []

    return lst

def ComplexToList_nn(type, Fs):
    #THIS FUNCTION IS ONLY USED TO GENERATE 
    #TRAINING AND TESTING DATA FOR THE NN

    max_val = int(Fs / 256) * 256
    lst = []
    tmp = []
    for i in range(0,max_val,256):
        for j in range(256):
            tmp.append(type[j+i].real)
        for j in range(256):
            tmp.append(type[j+i].imag)
        lst.append(tmp)
        tmp = []

    return lst


def fm_generate_rewrite(lst):
    FM_CARRIER = 0.0
    MAX_DEVIATION = 1000.0 # Hz
    filtered_sound = []
    phase = 0

    for n in range(0, 44100):
        # rush or drag phase accordingly to input signal
        # this is analog to integrating
        inputsgn = lst[n]

        # translate input into a phase change that changes frequency
        # up to MAX_DEVIATION Hz 
        phase += inputsgn * math.pi * MAX_DEVIATION / 44100
        phase %= 2 * math.pi

        # calculate quadrature I/Q
        i = math.cos(phase)
        q = math.sin(phase)

        carrier = 2 * math.pi * FM_CARRIER * (n / 44100.0)
        output = i * math.cos(carrier) - q * math.sin(carrier)

        filtered_sound.append(output)

    f = sig.resample(filtered_sound, 1260000)
    f = np.exp(1j * f)
    return f
    

def writeCSV(file_name, data,path): 
    len_training = int(len(data) * 0.9)
    len_testing = int(len(data) * 0.1)
    f = open(path + file_name + '_signal.csv', 'w', newline='')
    writer = csv.writer(f)
    writer.writerows([data[0]])
    f.close()

    """
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
    """
def writeCSV_nn(file_name, data,path): 
    len_training = int(len(data) * 0.9)
    len_testing = int(len(data) * 0.1)

    training_file_name = 'training' + file_name.upper() + str(len_training) + '.csv'
    f = open(path + training_file_name, 'w', newline='')
    writer = csv.writer(f)
    writer.writerows(data[0:len_training])
    f.close()

    testing_file_name = 'testing' + file_name.upper() + str(len_testing) + '.csv'
    f = open(path + testing_file_name, 'w', newline='')
    writer = csv.writer(f)
    writer.writerows(data[len_training:len_training+len_testing])
    f.close()

    print("Saved training file to '" +  path + training_file_name + "'")
    print("Saved testing file to '" + path + testing_file_name + "'")


def main():
    path = '../NeuralNetwork/TrainingTestingFiles/'

    SAMPLE_FOR = 1 # in seconds
    samplerate, data = scipy.io.wavfile.read(r'slave.wav')
    time = np.arange(0,SAMPLE_FOR,1/samplerate) #time vector
    data_real = normalizeAudio_real(data[0:int(samplerate*SAMPLE_FOR)])
    data = normalizeAudio(data[0:int(samplerate*SAMPLE_FOR)])

    time_vec = np.arange(0, 1, 1 / 1260000)
    #Set the Bandwidth to 12.6KHz
    BW = 6300
    w = low_cut_filter(data,samplerate,BW)
    w_real = low_cut_filter(data_real,samplerate,BW)

    samples_fm = fm_generate_rewrite(w_real)

    #Set new Sampeling rate to 1.26MHz
    f = sig.resample(w, 1260000)
    normalized_f = normalizeAudio(f)

    time_old = np.arange(0,SAMPLE_FOR,1/1260000)

    #f = SamplerateConversion(data,1260000,44100)

    #AM Modulation (Zero-IF)
    samples_am = generateSignalAM(normalized_f)

    #plt.plot(time_old[0:1260000], f[0:1260000])


    #FM Modulation (Zero-IF)
    #samples_fm = fm_generate_rewrite()
    #samples_fm = generateSignalFM(normalized_f,time_vec)


    sine = 2 * np.pi * 310000 * time_vec

    #fft_out = np.fft.fft(fm_filtered[0:100])
    #freq_vector = np.arange(0, 1260000, 1260000 / 100)
    #plt.plot(freq_vector, np.abs(fft_out))
    #for i in range(100):
    #    print(np.abs(fft_out[i]))
    #plt.show()
    #exit(0)
    #norm_am = normalizeAudio(samples_am)
    #norm_fm = normalizeAudio(fm_filtered)

    
    am = ComplexToList(samples_am,1260000)
    fm = ComplexToList(samples_fm,1260000)

    #f = open('fm_signal.csv', 'w', newline='')
    #writer = csv.writer(f)
    #writer.writerows([fm[0]])
    #f.close()

    #am = ComplexToList(samples_am,1260000)
    #fm = ComplexToList(samples_fm,1260000)

    #writeCSV_nn('am',am,'')
    #writeCSV_nn('fm',fm,'')
    
    #Save to files
    writeCSV('FM',fm,path)
    writeCSV('AM',am,path)
    #Save to files
    print("Wrote files!")
    #writeCSV('FM',fm,path)
    #writeCSV('AM',am,path
    
    '''
    fft_out = np.fft.fft(samples_am[0:12600])
    freq_vector = np.arange(0, 1260000, 1260000 / 12600)
    plt.plot(freq_vector, np.abs(fft_out))
    plt.show()
    '''
main()
