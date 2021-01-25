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

audio_name = 'grabacionAlerta.mp3'

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

array_to_txt(a_samples_left, "alerta_left.txt")
array_to_txt(a_samples_right, "alerta_right.txt")
array_to_txt(a_samples_left_norm, "alerta_left_norm.txt")
array_to_txt(a_samples_right_norm, "alerta_right_norm.txt")