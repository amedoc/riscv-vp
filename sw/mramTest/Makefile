compiler = /home/ahmed/Desktop/Work_Productive_4.0/riscv-gnu-toolchain32/bin

all : main.cpp
	$(compiler)/riscv32-unknown-elf-g++ -std=c++14 main.cpp -o mram -march=rv32g -mabi=ilp32d
	
sim: all
	riscv-vp --intercept-syscalls main
	
dump-elf: all
	riscv32-unknown-elf-readelf -a main
	
dump-code: all
	riscv32-unknown-elf-objdump -D main
	
clean:
	rm -f main
