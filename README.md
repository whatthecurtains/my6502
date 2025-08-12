# my6502 #
## This is a simple 6502 ##
This repo contains a simple 6502 emulator plus the ROM images for an OSI Challenger C2 4P (tape based).
The ROMS are 4 2K BASIC images, 3 system images and the character generator ROM for a 540 video board.

## Building the system ##
There are symlinks in the build directory to the individual character maps as .png files. These were extracted
with the decode_char.py script in synmon1 directory. The decoded character .png file are in v540/resources.

There are two programs that need to be built in order to run the simulator. The first is the video hardware emulator
in v540.

This can be built as follows:
    cd v540/build
    ../configure
    make video
    cd -

This should compile the video executable.

Next do the following:
    
    cd build
    ../configure
    make c2_4p

You will need to export the following environment variable as follows:
    
    export OSI_DISPLAY=gtk

This will tell the simulator to launch the video executable for video output (and in the future keyboard input)

If you want to just debug the simulator without launching the video app, you can set:
    
    export OSI_DISPLAY=NONE

This causes the executable to skip the video output.

With the environment properly set, you run the executable as follows:
    
    ./c2_4p -f ../rommap.map -m simulate -t map

# Developer information #

This is a very early work in progress. The basic design is does the following:
- main.c
    * main decodes the command line arguments -f indicates the image name -t is the image type (binary | map | monitor<=depricated) and calls either boot or disassemble
    * boot calls load_image to initialize the memory map and reserve space for the registers. The memory map is most often loaded as a map file
        * the map file contains a number of lines in increasing address order with the following fields:
            * binary_image_file_name start_address size
- hw_c2-4p.c
    *defines the interface functions that emulate the OSI hardware. These interface functions are based on the prototypes in include/hw_model.h
        * B540_init function, if the environment is properly configured, creates a shared memory file containing the entire 2K of video memory, and  a simple mailbox to communicate changes to the video application.




