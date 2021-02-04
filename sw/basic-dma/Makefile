compiler = /home/ahmed/Desktop/Work_Productive_4.0/riscv-gnu-toolchain32/bin

all : main.c irq.c irq.h bootstrap.S
	$(compiler)/riscv32-unknown-elf-gcc main.c irq.c bootstrap.S -o dma -march=rv32g -mabi=ilp32d -nostartfiles -Wl,--no-relax
	
sim: all
	riscv-vp --intercept-syscalls main
	
dump-elf: all
	riscv32-unknown-elf-readelf -a main
	
dump-code: all
	riscv32-unknown-elf-objdump -D main
	
dump-comment: all
	objdump -s --section .comment main
	
clean:
	rm -f main
