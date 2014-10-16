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
        $ ./r /dev/ttyS0/ 


  ModularBranch2 of Lab3
==================

client: ttyS4

    run terminal
        $ gcc appLayer.c auxiliar.c linkLayer.c noncanon.c protocol.c main.c -o main
      se estiver a enviar  
        $ ./main client 1
      se estiver a receber
        $ ./main client 0
    
client: ttyS0

    open main.c
    uncomment: //receive_nc(port);
    save main.c
    run terminal
        $ gcc appLayer.c auxiliar.c linkLayer.c noncanon.c protocol.c main.c -o main
        $ ./main /dev/ttyS0/


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
