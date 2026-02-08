#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include "injector.h"

pid_t g_pid = 0;
unsigned char *g_backup = NULL;
long g_addr = 0;
size_t g_sc_len = 0;
struct user_regs_struct g_old_regs;

//it is for ctrl + c situatins. we are putting back our backup codes taked from process. I add this func. on testing. I think it was a good idea :D
void safe_exit_handler(int sig) {
    if (g_pid > 0 && g_backup) {
        printf("\n[!] Signal %d: Restoration and safe detach...\n", sig);
        inject_data(g_pid, g_addr, g_backup, g_sc_len);
        ptrace(PTRACE_SETREGS, g_pid, NULL, &g_old_regs);
        detach_from_process(g_pid);
    }
    exit(sig);
}


int main(int argc, char *argv[]) {
    signal(SIGINT, safe_exit_handler); // Repeating: very important. i nearly loose my computer :D ...
    signal(SIGTERM, safe_exit_handler);

    if (argc != 3) {
        printf("Usage: sudo %s <program_directory> <payload.bin>\n", argv[0]); //Yes, if you dont know how  to run, you must run it false to learn.
        return 1;
    }

    char *target_path = argv[1];
    char *payload_path = argv[2];
    
    //read payload, print error if you have.
    FILE *f = fopen(payload_path, "rb");
    if (!f) { perror("[-] Payload error"); return 1; }
    
    
    fseek(f, 0, SEEK_END); g_sc_len = ftell(f); fseek(f, 0, SEEK_SET);//go to and, tell me where are you, turn back (its for learning size).
    unsigned char *shellcode = malloc(g_sc_len);//allocate empty room up to size of our code
    fread(shellcode, 1, g_sc_len, f); fclose(f);//write our code, close.
    
    
    //Taking target process to our child. it because ptrace_scop:
    
    pid_t child = fork(); // sperated our program to child and parent

    if (child == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);// Telling to kernel: my parent can trace me.
        
        if (strstr(target_path, "firefox")) {
            execl(target_path, target_path, "--new-instance", "--no-remote", NULL);
        }// Yes i tried this on firefox. So i needed "--new-instance" and "--no-remote". if you want to try thin on another spesified app, you can create a new situation like this down here:(its not necesery)
         
        
        
        //process of any other process:
        else { 
            execl(target_path, target_path, NULL);
        }
        exit(1);
    } else {
        g_pid = child;
        int status;
        waitpid(child, &status, 0); // waiting for child to change their name and start a new page of his life.
        
        printf("[+] Bypassing. PID: %d\n", child);

        if (is_it_safe(child)) {
            get_registers(child, &g_old_regs);//taking a photo of current regs. And save them on g_old_regs
            g_addr = g_old_regs.rip;//Taking address of rip
            
            //allocating memory enough to our code. Take the codes şn there and keep it for put it back. 
            g_backup = malloc(g_sc_len);
            peek_data(child, g_addr, g_backup, g_sc_len);
            
            //And finally, Injection.
            inject_data(child, g_addr, shellcode, g_sc_len);
            
            //Using PTARCE_SYSCALL for taking entry and exit news. when it comes, it will be an "exit" on our situation, putting back process will start.
            printf("[*] PTRACE_SYSCALL...\n");
            ptrace(PTRACE_SYSCALL, child, NULL, NULL);
            waitpid(child, &status, 0);
            
            //Putting back process
            inject_data(child, g_addr, g_backup, g_sc_len);
            ptrace(PTRACE_SETREGS, child, NULL, &g_old_regs);
            printf("[+] Repairing complated. System stability %%100 protected.\n");// i hope :D
        }
        //detach and cleanup
        ptrace(PTRACE_DETACH, child, NULL, NULL);
        free(shellcode);
        if (g_backup) {free(g_backup);}
    }
    return 0;
}
