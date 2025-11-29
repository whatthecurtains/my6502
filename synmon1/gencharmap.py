#! /usr/bin/env python


f=open("./OSI_kbdmap.txt","r")
src=f.read()
f.close()
import re

srch=re.compile('keyval=([0-9]+), hardware_keycode=([0-9]+)\s+([0-9]+)\s+([0-9]+)\s+(.*?)$',re.M)
values=srch.findall(src)

vk_dict = {
	  0 :  (" VK_1            ",7,7),
	  1 :  (" VK_2            ",7,6),
	  2 :  (" VK_3            ",7,5),
	  3 :  (" VK_4            ",7,4),
	  4 :  (" VK_5            ",7,3),
	  5 :  (" VK_6            ",7,2),
	  6 :  (" VK_7            ",7,1),
	  7 :  (" VK_8            ",6,7),
	  8 :  (" VK_9            ",6,6),
	  9 :  (" VK_0            ",6,5),
	 10 :  (" VK_COLON        ",6,4),
	 11 :  (" VK_MINUS        ",6,3),
	 12 :  (" VK_BACKSPACE    ",6,2),
	 13 :  (" VK_DOT          ",5,7),
	 14 :  (" VK_L            ",5,6),
	 15 :  (" VK_O            ",5,5),
	 16 :  (" VK_LINEFEED     ",5,4),
	 17 :  (" VK_ENTER        ",5,3),
	 18 :  (" VK_W            ",4,7),
	 19 :  (" VK_E            ",4,6),
	 20 :  (" VK_R            ",4,5),
	 21 :  (" VK_T            ",4,4),
	 22 :  (" VK_Y            ",4,3),
	 23 :  (" VK_U            ",4,2),
	 24 :  (" VK_I            ",4,1),
	 25 :  (" VK_S            ",3,7),
	 26 :  (" VK_D            ",3,6),
	 27 :  (" VK_F            ",3,5),
	 28 :  (" VK_G            ",3,4),
	 29 :  (" VK_H            ",3,3),
	 30 :  (" VK_J            ",3,2),
	 31 :  (" VK_K            ",3,1),
	 32 :  (" VK_X            ",2,7),
	 33 :  (" VK_C            ",2,6),
	 34 :  (" VK_V            ",2,5),
	 35 :  (" VK_B            ",2,4),
	 36 :  (" VK_N            ",2,3),
	 37 :  (" VK_M            ",2,2),
	 38 :  (" VK_COMMA        ",2,1),
	 39 :  (" VK_Q            ",1,7),
	 40 :  (" VK_A            ",1,6),
	 41 :  (" VK_Z            ",1,5),
	 42 :  (" VK_SPACE        ",1,4),
	 43 :  (" VK_SLASH        ",1,3),
	 44 :  (" VK_SEMICOLON    ",1,2),
	 45 :  (" VK_P            ",1,1),
	 46 :  (" VK_REPEAT       ",0,7),
	 47 :  (" VK_LEFTCTRL     ",0,6),
	 48 :  (" VK_ESC          ",0,5),
	 49 :  (" VK_LEFTSHIFT    ",0,2),
	 40 :  (" VK_RIGHSHIFT    ",0,1),
	 51 :  (" VK_CAPSLOCK     ",0,0)
	}


f=open("keydecode.c","w")
f.write("""

#include <stdint.h>
#include "keydecode.h"

uint8_t OSI_keystate[8];

static inline uint8_t keycode_down(uint8_t keycode, uint8_t key) {
    return keycode | (1<<key);
}

static inline uint8_t keycode_up(uint8_t keycode, uint8_t key) {
    return  keycode & ~(1<<key);
}

void update_keystate(bool down, uint16_t val, uint8_t key) {
    uint32_t index=key<<16|val;
    uint8_t  row;
    uint8_t  col;
    switch(index) {
    default: goto skip;    
""")

#
#
#VK_t get_vk(uint16_t keyval, uint16_t keycode) {
#	VK_t vk_val;
#	if (keyval>KEY_MAX) {
#	}
#	else {
#		switch(keyval) {
#""")



for val,code,row,col,vk in values:
	sval=int(val) | int(code)<<16
	print(f"val={val} code={code} combo={hex(sval)}")
	f.write(f"    case {hex(sval)}: row={row}; col={col}; break;\n")

f.write("    }\n")
f.write("""
    if (down) {
        OSI_keystate[row] = keycode_down(OSI_keystate[row],col);
    }
    else {
        OSI_keystate[row] = keycode_up(OSI_keystate[row],col);
    }
skip:
    return;
}

""")

f.close()