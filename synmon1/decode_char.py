from PIL import Image

# PIL accesses images in Cartesian co-ordinates, so it is Image[columns, rows]
img = Image.new( 'RGB', (8,8), "black") # create a new black image
pixels = img.load() # create the pixel map

#for i in range(img.size[0]):    # for every col:
#    for j in range(img.size[1]):    # For every row
#        pixels[i,j] = (i, j, 100) # set the colour accordingly
#
#img.show()


def genchar(ba):
	global pixels
	j = 0
	for raster in ba:
		for i in range(8):
			pixels[i,j] = (0,0,0) if raster&(1<<i)==0 else (200,200,200)
		j+=1



def dump(ba):
	for raster in ba:
		for i in range(8):
			print(f"{' ' if raster&(1<<(8-i))==0 else 'X'}", end='')
		print()



f=open("CHARGEN.ROM","rb")
ch=bytearray(8)
for ch_num in range(256):
	f.readinto(ch)
	genchar(ch)

	#img.show()
	img.save(f'ch_{ch_num:02X}.png','PNG')

f.close()
