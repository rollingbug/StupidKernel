@echo off
echo now building ...
echo ------------------------------------------------------------

rem set obj=build\boot.o build\isr.o build\kernel.o build\basic.o build\interrupt.o build\mm.o build\phy_mem.o
set obj=build\boot.o build\kernel.o build\console.o build\lib.o build\isr.o build\interrupt.o build\mm.o build\physical.o build\segment.o build\paging.o build\kmalloc.o build\dma.o build\vmalloc.o build\sched.o build\floppy.o build\keyboard.o build\fat12.o build\process.o

rem BOOT
build_tools\nasmw.exe -f elf -o build\boot.o boot\boot.asm

rem KERNEL
build_tools\bin\i686-elf-gcc.exe -o build\kernel.o -c kernel.c -nostdlib -fno-builtin

rem CONSOLE
build_tools\bin\i686-elf-gcc.exe -o build\console.o -c console\console.c -nostdlib -fno-builtin

rem LIB
build_tools\bin\i686-elf-gcc.exe -o build\lib.o -c lib\lib.c -nostdlib -fno-builtin

rem INTERRUPT
build_tools\nasmw.exe -f elf -o build\isr.o interrupt\isr.asm
build_tools\bin\i686-elf-gcc.exe -o build\interrupt.o -c interrupt\interrupt.c -nostdlib -fno-builtin

rem MM
build_tools\bin\i686-elf-gcc.exe -o build\mm.o -c mm\mm.c -nostdlib -fno-builtin
build_tools\bin\i686-elf-gcc.exe -o build\physical.o -c mm\physical.c -nostdlib -fno-builtin
build_tools\bin\i686-elf-gcc.exe -o build\segment.o -c mm\segment.c -nostdlib -fno-builtin
build_tools\bin\i686-elf-gcc.exe -o build\paging.o -c mm\paging.c -nostdlib -fno-builtin
build_tools\bin\i686-elf-gcc.exe -o build\kmalloc.o -c mm\kmalloc.c -nostdlib -fno-builtin
build_tools\bin\i686-elf-gcc.exe -o build\vmalloc.o -c mm\vmalloc.c -nostdlib -fno-builtin

rem SCHED
build_tools\bin\i686-elf-gcc.exe -o build\sched.o -c sched\sched.c -nostdlib -fno-builtin
build_tools\bin\i686-elf-gcc.exe -o build\process.o -c sched\process.c -nostdlib -fno-builtin

rem DRIVERS
build_tools\bin\i686-elf-gcc.exe -o build\floppy.o -c drivers\floppy.c -nostdlib -fno-builtin
build_tools\bin\i686-elf-gcc.exe -o build\keyboard.o -c drivers\keyboard.c -nostdlib -fno-builtin
build_tools\bin\i686-elf-gcc.exe -o build\dma.o -c drivers\dma.c -nostdlib -fno-builtin

rem FS
build_tools\bin\i686-elf-gcc.exe -o build\fat12.o -c fs\fat12.c -nostdlib -fno-builtin

rem LINK
build_tools\bin\i686-elf-ld.exe -T linker.ld -o kernel.bin %obj%

pause