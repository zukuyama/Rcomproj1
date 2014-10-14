Rcomproj1
===========================

  Current Lab2
==================

client: ttyS4

    run terminal
        $ gcc -c s.c -o s
        $ ./s /dev/ttyS4/

client: ttyS0

    run terminal
        $ gcc -c r.c -o r
        $ ./s /dev/ttyS0/ 


  ModularBranch of Lab2
==================

client: ttyS4

    open main.c
    uncomment: //send_nc(port);
    save main.c
    run terminal
        $ gcc -c main.c -o main
        $ ./s /dev/ttyS4/
    
client: ttyS0

    open main.c
    uncomment: //receive_nc(port);
    save main.c
    run terminal
        $ gcc -c main.c -o main
        $ ./s /dev/ttyS0/
