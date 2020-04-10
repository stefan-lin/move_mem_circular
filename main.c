#include "circular.h"

int main(){
  Circular_Mem cm;
  char input[5] = "ABCDE";

  construct_cMem(&cm);
  void *from = write_mem_addr(input, 5, 100, &cm);
  printf("[inital memory] : ");
  void *to = (void *)(cm._mem + 2); // addr = 102
  show(&cm);
  void *ret = move_mem(from, to, 5, &cm);
  printf("from address = 100\n");
  printf("to adress = 102\n");
  printf("[moved memory]  : ");
  show(&cm);
  printf("\n");

  construct_cMem(&cm);
  void *f = write_mem_addr(input, 5, 104, &cm);
  printf("[inital memory] : ");
  void *t = (void *)(cm._mem + 2);
  show(&cm);
  void *r = move_mem(f, t, 5, &cm);
  printf("from address = 104\n");
  printf("to adress = 102\n");
  printf("[moved memory]  : ");
  show(&cm);
  printf("\n");

  construct_cMem(&cm);
  show(&cm);
  f = write_mem_addr(input, 5, 100, &cm);
  printf("[inital memory] : ");
  t = (void *)(cm._mem+5);
  r = move_mem(f, t, 5, &cm);
  printf("from address = 100\n");
  printf("to adress = 105\n");
  printf("[moved memory]  : ");
  show(&cm);

  return 0;
}
