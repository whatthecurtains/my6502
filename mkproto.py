
f=open("instruction_list.txt","r")
instr=f.readlines()
f.close()

#f=open("protos.h","w")
#for i in instr:
#    i=i[:-1]
#    f.write(f"#define {i}(mode,cc) \\\n\tall_regs_t* {i}_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);\n")
#    f.write(f'#include "{i}_def.c"\n\n')
#    
#f.close()


def gen_all_instr(instr):
    f=open("./instr/all_instr.c","w")
    for i in instr:
        i=i[:-1]
        f.write(f'#include "{i}_def.c"\n')
    f.close()
        

def split_instr(instr):
    for i in instr:
        i=i[:-1]
        print(f"Splitting files for {i}")
        f1=open(f"./{i}.c","r")
        f2=open(f"./instr/{i}.c","w")
        f3=open(f"./instr/_{i}_def.c","w")
        split=False
        for line in f1:
            if split:
                f3.write(line)
            else:
                if line=='//include\n':
                    split = True
                    f2.write(f'#include "{i}_def.c"\n') 
                else:
                    f2.write(line)
        f1.close()
        f2.close()
        f3.close()

def transform_defs(instr):
    import re
    spec=re.compile('(.{3})\\((.*?),(.*)\\)')
    for i in instr:
        i=i[:-1]
        print(f"Transforming {i}")
        f1=open(f"./instr/_{i}_def.c","r")
        f2=open(f"./instr/{i}_def.c","w")
        for line in f1:
            m=spec.search(line)
            if m!=None:
                f2.write(f'INSTR({m.group(1)},{m.group(2)},{m.group(3)},0x,0)\n')
    
        f1.close()
        f2.close()
        

def gen_protos(instr):    
    #proto=open("protos.h","w")
    #for i in instr:
    #    i=i[:-1]
    #    proto.write(f"#define {i}(mode,cc) \\\n\tall_regs_t* {i}_##mode(all_regs_t* reg, uint8_t low, uint8_t high, uint64_t* cyc);\n")
        #f.write(f'#include "{i}_def.c"\n\n')
        
    #f.close()
  
    import re
    spec=re.compile('(.{3})\\((.*?),(.*)\\)')
    proto.write("\n\n#define INSTR(inst,mode,cc,hex)  \\\n\tinst(mode,cc)\n\n")
    for i in instr:
        i=i[:-1]
        print(f"Generating files for {i}")
        f3=open(f"./instr/{i}_def.c","r")
        for line in f3:
            m=spec.search(line)
            if m!=None:
                proto.write(f'INSTR({m.group(1)},{m.group(2)},{m.group(3)},0x,0)\n')
        f3.close()
        proto.write("\n\n")
    proto.write('#undef INSTR\n\n')
    proto.close()
    

def make_SOURCES(instr):
    for file in instr:
        file=file[:-1]
        print(f"./instr/{file}.c\t\t\t\\")
        print(f"./instr/{file}_def.c\t\t\t\\")
        

    
if __name__=="__main__":
    #split_instr(instr)
    #gen_protos(instr)
    #transform_defs(instr)
    #gen_all_instr(instr)
    make_SOURCES(instr)