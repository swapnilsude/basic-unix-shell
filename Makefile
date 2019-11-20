all: assign1_swapnil.c
		gcc shell_swapnil.c -o shell_swapnil -lreadline
packages:
		sudo apt-get install libreadline-dev    
clean:
		rm assign1_swapnil
