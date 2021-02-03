filename = "GrabacionEmergencia"

data = open(filename + '.wav', 'rb').read()

print(f"El archivo {filename} tiene {len(data)} bytes")

out_str = "uint8_t "+filename+"_wavarray[BUFF_LEN_WAV]={ \n"

columns = 35

i =0

for elem in data:
	hex_byte = "0x" + format(elem, '02x')
	out_str += hex_byte + ','
	if((i-(columns-1)) % columns == 0):
		out_str += "\n"
	i +=1


out_str = out_str[:-1]
out_str += "};"

        
f = open(filename+"_wavarray.h",'w')
f.write("#define BUFF_LEN_WAV "+str(len(data))+"\n")
f.write(out_str)

f.close()