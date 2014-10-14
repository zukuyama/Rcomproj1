Rcomproj1
===========================


==================
  Current
==================

client: ttyS4

  run terminal
  use:
    gcc -c s.c -o s
    
client: ttyS0

  run terminal
  use:
    gcc -c r.c -o r


==================
  ModularBranch of Lab2
==================

client: ttyS4

  open main.c
  uncomment: //send_nc(port);
  save main.c

  run terminal
  use:
    gcc -c main.c -o main
    
client: ttyS0

  open main.c
  uncomment: //receive_nc(port);
  save main.c

  run terminal
  use:
    gcc -c main.c -o main
