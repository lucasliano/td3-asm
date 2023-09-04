# GDB Dashboard configuration
set architecture arm
target extended-remote localhost:2159
dashboard registers -style list 'r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 sp lr pc cpsr'
dashboard assembly -style opcodes 'True'
set radix 16
dashboard memory -output /dev/pts/2