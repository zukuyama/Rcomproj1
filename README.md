Rcomproj1
===========================

  Actual
==================

on: ttyS4
    
    run terminal
      make
      -if sending 
        $ bin/main client send
      -if receiving
          $ bin/main client get
    
on: ttyS0

    run terminal
      make
      -if sending 
        $ bin/main server send
      -if receiving
        $ bin/main server get
          

  ModularBranch2 of Lab3
==================

on: ttyS4

    run terminal
        $ gcc appLayer.c auxiliar.c linkLayer.c noncanon.c protocol.c main.c -o main
        -if sending 
          $ ./main client 1
        -if receiving
          $ ./main client 0
    
on: ttyS0

    run terminal
        $ gcc appLayer.c auxiliar.c linkLayer.c noncanon.c protocol.c main.c -o main
        -if sending  
          $ ./main server 1
        -if receiving
          $ ./main server 0


  ModularBranch of Lab2
==================

client: ttyS4

    open main.c
    uncomment: //send_nc(port);
    save main.c
    run terminal
        $ gcc appLayer.c auxiliar.c linkLayer.c noncanon.c protocol.c main.c -o main
        $ ./main /dev/ttyS4/
    
client: ttyS0

    open main.c
    uncomment: //receive_nc(port);
    save main.c
    run terminal
        $ gcc appLayer.c auxiliar.c linkLayer.c noncanon.c protocol.c main.c -o main
        $ ./main /dev/ttyS0/
