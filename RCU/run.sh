export LD_LIBRARY_PATH=/usr/local/lib/

gcc RCU_call_back.c  -lurcu-bp -lpthread  -g -o RCU_call_back
gcc RCU.c  -lurcu-bp -lpthread  -g -o RCU
