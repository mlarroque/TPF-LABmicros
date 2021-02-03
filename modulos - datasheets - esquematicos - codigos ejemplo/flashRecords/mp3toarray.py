import numpy as np
from pydub import AudioSegment as audioSeg

def array_to_txt(array, output_name):
    np.savetxt(output_name, array, delimiter=',')
    file1 = open(output_name, 'r')
    Lines = file1.readlines()
    output = "{"
    count = 0
    for line in Lines:
        output += line.strip() + ","
        if count % 4 == 0:
            output += "\n"
        count += 1
    print(count)
    output = output[:-1]
    output += "}"
    text_file = open(output_name, "w")
    n = text_file.write(output)



audioSeg.converter = r"C:\\ffmpeg\\bin\\ffmpeg.exe"

#audio_name = "GrabacionEmergencia.aac"
audio_name = 'grabacionAlerta.mp3'

def audio_segment_method(audio_name, channelMode = "stereo", normMode = "unnormal"):


    a = audioSeg.from_mp3(audio_name)
    a_samples = np.array(a.get_array_of_samples())

    print(a_samples.shape)

    a_samples = a_samples.reshape((-1, 2))
    a_samples_left = a_samples[:, 0]
    a_samples_right = a_samples[:, 1]

    a_samples_norm = np.float32(a_samples) / (2**15)
    a_samples_left_norm = a_samples_norm[:, 0]
    a_samples_right_norm = a_samples_norm[:, 1]

    frame_rate = a.frame_rate
    print(f'frame rate: {frame_rate}')
    ret = 0
    if (normMode == "unnormal"):

        if (channelMode == "left"):
            ret = a_samples_left
        elif (channelMode == "right"):
            ret = a_samples_right
        elif (channelMode == "stereo"):
            ret = a_samples

    elif (normMode == "normal"):
        if (channelMode == "left"):
            ret = a_samples_left_norm
        elif (channelMode == "right"):
            ret = a_samples_right_norm
        elif (channelMode == "stereo"):
            ret = a_samples_norm
    y = np.int16(ret)
    #song = audioSeg(y.tobytes(), frame_rate=frame_rate, sample_width=2, channels=2)
    return y


def common_method(audio_name, channelMode, normMode):
    a = np.fromfile(audio_name, dtype = 'uint8', count=-1, sep='', offset=0)
    return a


METHOD = "asFile"
#METHOD = "audio_segment"
fun = 0
if (METHOD == "asFile"):
    fun = common_method
elif (METHOD == "audio_segment"):
    fun = audio_segment_method

array_to_txt(fun(audio_name, "left", "unnormal"), "alerta_left.txt")
array_to_txt(fun(audio_name, "right", "unnormal"), "alerta_right.txt")
array_to_txt(fun(audio_name, "stereo", "unnormal"), "alerta_stereo.txt")
array_to_txt(fun(audio_name, "left", "normal"), "alerta_left_norm.txt")
array_to_txt(fun(audio_name, "right", "normal"), "alerta_right_norm.txt")
array_to_txt(fun(audio_name, "stereo", "normal"), "alerta_stereo_norm.txt")

