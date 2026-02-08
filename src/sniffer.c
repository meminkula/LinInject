#include "injector.h"
#include <stdio.h>
#include <time.h>

void collect_data(pid_t target_pid, int duration_sec) {
    struct user_regs_struct regs;
    
    //open a new file to keep register logs
    FILE *fp = fopen("data/register_logs.csv", "w");
    if (!fp) { perror("CSV Error"); return; }

    fprintf(fp, "timestamp,rip,rax,rbx,rsp\n");
    
    
    //program starts if attach process is okey
    if (attach_to_process(target_pid) == 0) {
        printf("[*] Sniffer: Collecting data untill %d second...\n", duration_sec);//duration_second from main.c , you can change for diffrent optimization you want.
        time_t start = time(NULL);
        //taking samples untill duration_sec
        while (difftime(time(NULL), start) < duration_sec) {
            
            if (get_registers(target_pid, &regs) == 0) {
                fprintf(fp, "%ld,%llx,%llx,%llx,%llx\n", 
                        clock(), regs.rip, regs.rax, regs.rbx, regs.rsp);
            }
            
            usleep(5000);//you can change it for more or less periods to take samples. 
        }
        
        //close the file. And detach.
        detach_from_process(target_pid);
        fclose(fp);
        printf("[+] Collecting data finish: data/register_logs.csv\n");
    }
}
