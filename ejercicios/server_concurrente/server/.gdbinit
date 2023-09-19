set radix 16

dashboard -layout breakpoints registers source expressions variables stack memory
dashboard registers -style list 'r0 r1 r2 r3 sp lr pc'
dashboard registers -output /dev/pts/2
dashboard breakpoints -output /dev/pts/2
dashboard stack -output /dev/pts/2
dashboard expressions -output /dev/pts/2
dashboard memory -output /dev/pts/2


break server.c:21
run
