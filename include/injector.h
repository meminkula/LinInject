#ifndef INJECTOR_H
#define INJECTOR_H

#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <unistd.h>

//Ptarace functions:

int attach_to_process(pid_t pid);
int detach_from_process(pid_t pid);
int get_registers(pid_t pid, struct user_regs_struct *regs);
int inject_data(pid_t pid, long address, void *data, size_t len);
int peek_data(pid_t pid, long address, void *dest, size_t len);

// Analys Modulles

long get_safe_address(pid_t pid);

int is_it_safe(pid_t pid);
double calculate_variance(long long data[], int n);

#endif
