# LinInject: Adaptive Process Injector & Stability Analyzer

LinInject is an advanced core injection tool for linux. Uses `ptrace` and process inheritance. Diffrent thing in Lininject is it can analyze CPU behavior with variance anlysis and waits for the most stable moment to strike.

## Key Features

* Bypassing `ptrace_scope`: By using `fork()` and `exec()` , it creates a "Parent - Child" relationship with target. Linux allows a parent to watch his child (even if security settings are high). 

* The Brain: Inside `brain.c` , I added a variance calculation. It checks RIP and RSP registers (decided this regs because of stability). According to CPU's situation it decides injecting. If CPU is too busy, it waits. Only inject when process is "calm".

* Surgical Injection: It writes data in 8 byte "boxes" (Word-aligment). If the shellcode is not exactly 8 bytes, it peeks at the original memory and patches it carefully. So nothing breaks. 

* Full Recovery: After shellcode finishes, it retstoring the original code and registers. The target is continues its life like nothing happened.

* Safe Exit: If you press `ctrl+C`, the `safe_exit_handler` repairs the process before closing. So your computer will not broke on test like mine :D .

## Project Structure

```text
.
├── Makefile           # Compilation script
├── include/
│   └── injector.h     # Global definitions and shared headers
├── src/
│   ├── main.c         # Process Orchestrator
│   ├── brain.c        # Stability Analysis
│   ├── injector.c     # Memory Operations
│   ├── map_scanner.c  # Memory Mapping
│   └── sniffer.c      # Register Logging and Analysis
├── build/             # Compiled binaries
└── data/              # Storage for register logs (.csv files)
```

## Build And Run

* Build
 When you are in the project folder
```bash
#to compile:
make

#to clean build files:
make clean
``` 

* Run 
 When you are in project folder
 ```bash
 sudo ./build/LinInject <pathway of payload.bin>
```

## How It Works (Step-by-Step)

   1. The program forks. The child calls `PTRACE_TRACEME` and becomes the target.

   2. The parent waits until the child is loaded.

   3. `is_it_safe()` checks the CPU behavior.

   4. Shellcode is injected into the RIP address (where the CPU is currently looking).

   5. `PTRACE_SYSCALL` lets the shellcode run until it hits a system call.

   6. Everything is repairing. Parent detaches. No traces left.

##Disclaimer

This project is for educational purposes and ethical hacking research only. Don't use it on systems you don't own. I wrote `the safe_exit_handler` to protect your stability, but always be careful!
