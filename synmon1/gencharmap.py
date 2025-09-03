
f=open("./OSI_kbdmap.txt","r")
src=f.read()
f.close()
import re

srch=re.compile('keyval=([0-9]+), hardware_keycode=([0-9]+)\s+([0-9]+)\s+([0-9]+)\s+(.*?)$',re.M)
values=srch.findall(src)


f=open("keydecode.c","w")
f.write("""

#include <stdint.h>
#include "keydecode.h"



VK_t get_vk(uint16_t keyval, uint8_t keycode) {
    VK_t vk_val;
    switch(keycode<<16|keyval) {
""")



for val,code,row,col,vk in values:
    sval=int(code)<<16|int(val)
    keychar=int(row)<<8|int(col)
    print(f"val={val} code={code} combo={hex(sval)}")
    f.write(f"        case {hex(sval)}: vk_val={vk}; break;\n")
    
f.write("    }\n}\n\n")

f.close()