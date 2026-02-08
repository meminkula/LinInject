#include "injector.h"
#include <stdio.h>
#include <string.h>

//Atach to process
int attach_to_process(pid_t pid) {
    if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) < 0) return -1;
    waitpid(pid, NULL, 0);
    return 0;
}

//Detach from process
int detach_from_process(pid_t pid) {
    return ptrace(PTRACE_DETACH, pid, NULL, NULL);
}

//Gettin reg values from user_regs_struct
int get_registers(pid_t pid, struct user_regs_struct *regs) {
    return ptrace(PTRACE_GETREGS, pid, NULL, regs);
}

//injecting process:
int inject_data(pid_t pid, long address, void *data, size_t len) {
    size_t i = 0;//count for learn wich byte we are on.
    long word;//create a 8 byte transport boxes.
    
    while (i < len) {
        //cheking if our shellcode is less then 8 byte or not
        if (len - i < sizeof(long)) {//if it is less:
            word = ptrace(PTRACE_PEEKDATA, pid, address + i, NULL);//Read target 8 byte. because we will only change the bytes we need. rest of them must be stay same.
            
            //put our shellcode to box.
            memcpy(&word, (unsigned char *)data + i, len - i);
        } 
        else {
            memcpy(&word, (unsigned char *)data + i, sizeof(long));//directl put 8 bytes package we have into box.
        }
        
        ptrace(PTRACE_POKEDATA, pid, address + i, word);//Write our box to target memory address
        i += sizeof(long);//next block(increasing 8 byte - size of long is 8 bytes)
    }
    return 0;
}

// Memory reading:
//it uses same logic with injection_data. but its for reading(peeking).
int peek_data(pid_t pid, long address, void *dest, size_t len) {
    size_t i = 0;
    long word;
    while (i < len) {
        word = ptrace(PTRACE_PEEKDATA, pid, address + i, NULL);
        if (len - i < sizeof(long)) {
            memcpy((unsigned char *)dest + i, &word, len - i);//We are making casting. 
        } else {
            memcpy((unsigned char *)dest + i, &word, sizeof(long));//Doing same for allow to computer jump 8 bytes correctly.
        }
        i += sizeof(long);
    }
    return 0;
}
