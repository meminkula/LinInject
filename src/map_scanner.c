#include "injector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//Memory mapping analysis: to find a place that we can execute our code.
long get_safe_address(pid_t pid) {
    char path[256], line[512], permissions[10];
    long start, end;
    sprintf(path, "/proc/%d/maps", pid);//the directory wich have the processes RAM map.
    FILE *f = fopen(path, "r");
    if (!f) {return -1;}

    while (fgets(line, sizeof(line), f)) {
        
        //its for parting and make readable (deximal) to a complicate line
        if (sscanf(line, "%lx-%lx %s", &start, &end, permissions) == 3) {
            if (permissions[0] == 'r' && permissions[2] == 'x') { //we are searching for readable and runnable area on RAM
            
                fclose(f);
                
                return start + 0x2000;// + 8192 (0x2000) because there is ELF Header from the strat to after 8012 bytes.
            }
        }
    }
    
    fclose(f);
    return -1;
}
