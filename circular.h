#include <stdint.h>

#define MEM_SIZE 10
#define START_ADDR 100

typedef struct Circular_Mem{
  char _mem[MEM_SIZE];
  char *_mem_end;
  char *head;
  char *tail;
  uint32_t _mem_used;  // unit=byte
  uint32_t virtual_addr;
  char *value_cache;
} Circular_Mem;

void construct_cMem(Circular_Mem *cm);

void *read_mem (void *ptr,     uint32_t len, Circular_Mem *cm);
void *write_mem(void *content, uint32_t len, Circular_Mem *cm);
void *write_mem_addr(void *content, uint32_t len, uint32_t req_addr, Circular_Mem *cm);
void *move_mem (void *from,    void *to,     uint32_t len,      Circular_Mem *cm);

void show(Circular_Mem *cm);

//void _write_op(uint32_t counter, char *reader, char *writer,    Circular_Mem *cm);

