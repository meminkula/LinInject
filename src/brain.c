#include <stdio.h>
#include <math.h>
#include "injector.h"

double calculate_variance(long long data[], int n) {
    double sum = 0, mean, variance = 0;
    
    //taking average of our values 
    for (int i = 0; i < n; i++) sum += data[i];
    mean = sum / n;
    
    //Deviation
    for (int i = 0; i < n; i++) variance += pow(data[i] - mean, 2);
    
    //Variance
    return variance / n;
}

//Deciding is_it_safe
int is_it_safe(pid_t pid) {
    struct user_regs_struct regs;
    //yo can change values. i tried some diffrent versions. this sets are my bests.
    long long rip_history[100], rsp_history[100];
    
    //Sampling: till 100 ms , saving RIP, RSP values.
    for (int i = 0; i < 100; i++) {
        if (get_registers(pid, &regs) != 0) return 0;
        rip_history[i] = (long long)regs.rip;
        rsp_history[i] = (long long)regs.rsp;
        usleep(1000);//also you can change waiting period. as i said they are my sets from test experience
    }
    //analysing as you see:
    double rip_var = calculate_variance(rip_history, 100);
    double rsp_var = calculate_variance(rsp_history, 100);
    printf("[*] Analysis-> RIP Var: %.2f | RSP Var: %.2f\n", rip_var, rsp_var);
    return (rip_var < 5.0 && rsp_var < 0.1);// Also you can change threshold values. 
}
