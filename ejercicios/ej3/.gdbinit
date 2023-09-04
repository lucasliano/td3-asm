# GDB Dashboard configuration
set architecture arm
target extended-remote localhost:2159
dashboard registers -style list 'r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 sp lr pc cpsr'
dashboard assembly -style opcodes 'True'
set radix 16
dashboard memory -output /dev/pts/2

dashboard memory watch 0x70000000 96
dashboard memory watch 0x70010000 96
dashboard memory watch 0x70020000 96
dashboard memory watch 0x70030000 96

b* 0x700302d8

b* 0x00000004
b* 0x00000008
b* 0x0000000C
b* 0x00000010
b* 0x00000014
b* 0x00000018
b* 0x0000001C

#x/12x 0x70000000
#dashboard expression watch ((long*)0x70000000)[0]@4

