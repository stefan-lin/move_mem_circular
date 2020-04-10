#include <stdio.h>
#include <stdlib.h>

#include "circular.h"

/**
 * Function: construct_cMem
 *
 * initialize the circular memory
 *
 * @cm  (Circular_Mem *): pointer points to circular memory
 */
void construct_cMem(Circular_Mem *cm){
  cm->_mem_end = cm->_mem + MEM_SIZE - 1; // points to the end
  cm->head = cm->_mem;
  cm->tail = cm->_mem;
  cm->_mem_used = 0;
  //cm->value_cache = NULL;
  cm->virtual_addr = START_ADDR;

  int idx = 0;
  for(idx; idx<MEM_SIZE; idx++){
    cm->_mem[idx] = ' ';
  }
}

/**
 * Function: read_mem
 *
 * Dynamically create a copy of request content and return it to user
 *
 * @ptr (void *)        : pointer to the return (dynamically allocated) memory
 * @len (uint32_t)      : the length of content
 * @cm  (Circular_Mem *): pointer to the circular memory
 *
 * return (void *) : dynamically allocated memory
 */
void *read_mem(void *ptr, uint32_t len, Circular_Mem *cm){
  char *cache = (char *)malloc(sizeof(char) * len);
  char *reader = (char *)ptr;
  char *writer = cache;
  uint32_t counter = len;

  while(counter){
    *writer = *reader;
    writer++;
    //reader = (reader - cm->_mem == MEM_SIZE - 1)? cm->_mem: reader + 1;
    //if(reader-cm->_mem == MEM_SIZE-1){
    if(reader == cm->_mem_end){
      // Read the last byte in the memory block
      reader = cm->_mem;
    }
    else{
      reader++;
    }

    counter--;
  } // END WHILE

  return (void *)cache;
}

/**
 * Function: write_mem
 * 
 * Simple write operation which append new content behind the tail pointer
 *
 * @content   (void *)        : pointer points to the head of new content
 * @len       (uint32_t)      : the length of new content
 * @cm        (Circular_Mem *): pointer points to circular memory
 *
 * return : void pointer points to the head of newly written content in circular memory
 */
void *write_mem(void *content, uint32_t len, Circular_Mem *cm){
  if(MEM_SIZE - cm->_mem_used < len){
    // not enough memory
    fprintf(stdout, "[Error] Not Enough Memory...\n"); // may switch to stderr afterwards
    return NULL;
  }

  char *ret = cm->tail;
  uint32_t idx = 0;
  int arr_idx = cm->tail - cm->_mem;
  char *new_content = (char *)content;

  for(idx; idx<len; idx++){
    cm->_mem[arr_idx % MEM_SIZE] = *new_content;
    new_content++;
    arr_idx++;
  }

  if(cm->tail + len > cm->_mem + MEM_SIZE){
    cm->tail = cm->_mem + (cm->tail - cm->_mem + len - MEM_SIZE);
  }
  else{
    cm->tail = cm->tail + len;
  }
  cm->_mem_used += len;

  return (void *)ret;
}

/**
 * Function write_mem_addr
 * 
 * Back door operation, this function would not change head, tail, and count.
 * This function simply just write content to the memory based upon the address given.
 *
 * @content   (void *)        : pointer points to input content
 * @len       (uint32_t)      : the length of input content
 * @req_addr  (uint32_t)      : the memory address requested to write input in
 * @cm        (Circular_Mem *): pointer points to circular memory (access point) 
 */
void *write_mem_addr(void *content, uint32_t len, uint32_t req_addr, Circular_Mem *cm){
  if(MEM_SIZE - cm->_mem_used < len){
    // not enough memory
    fprintf(stdout, "[Error] Not Enough Memory...\n"); // may switch to stderr afterwards
    return NULL;
  }
  if(req_addr > START_ADDR + MEM_SIZE || req_addr < START_ADDR){
    // request address is not valid
    fprintf(stdout, "[Error] Invalid Request Address\n"); // may switch to stderr afterwards
    return NULL;
  }

  char *ret = cm->_mem + (req_addr - START_ADDR);
  uint32_t idx = 0;
  int arr_idx = ret - cm->_mem;
  char *new_content = (char *)content;

  for(idx; idx<len; idx++){
    cm->_mem[arr_idx % MEM_SIZE] = *new_content; // easier to just manipulate the idex instead of
                                             // pointers with lots of if statements
    new_content++;
    arr_idx++;
  }

  //if(cm->tail + len > cm->_mem + MEM_SIZE){
  //  cm->tail = cm->_mem + (cm->tail - cm->_mem + len - MEM_SIZE);
  //}
  //else{
  //  cm->tail = cm->tail + len;
  //}
  //cm->_mem_used += len;

  return (void *)ret;
}

/**
 * Function: move_mem
 *
 * @from   (void *)        : pointer points to the original address
 * @to     (void *)        : pointer points to the address of destination
 * @len    (uint32_t)      : the length of the content
 * @cm     (Circular_Mem *): pointer points to circular memory (access point)
 *
 * return (void *) : the pointer that points to the content which points to
 *                   a new address
 */
/**
 * Approach:
 *  in order to avoid overwrite some bytes of the content that needed to be 
 *  moved, we need to move bytes reversely if the scenario fullfills both
 *  following conditions:
 *    a) the gap between "from" and "to" (pointers) is less than len (the
 *       length of the content that needed to be moved)
 *    b) the address of "to" pointer stores has greater value than the
 *       address of "from" pointer stores
 *  in other scenarios, we can safely move bytes in the direction of moving
 *  bytes in less significant address to greater significant address(
 *  e.g. address 100 to address 102)
 */
void *move_mem(void *from, void *to, uint32_t len, Circular_Mem *cm){
  if(to > cm->_mem + MEM_SIZE || to < cm->_mem){
    // the request destination is an invalid address
    fprintf(stderr, "[Error] invalid address\n");
    return NULL;
  }
  
  char *f = (char *)from;
  char *t = (char *)to;
  int gap = ((t-f))? (t-f): (f-t);
  
  if(gap<len && (t-f)){
    // gap is not big enough, write bytes reversly
    char *f_ = f + len - 1;
    char *t_ = t + len - 1;
    while(len){
      *t_-- = *f_--;
      len--;
    }
  }
  else{
    while(len){
      *t++ = *f++;
      len--;
    }
  }

  return to;
}

/**
 * Function show
 *
 * Simply printing out the array
 */
void show(Circular_Mem *cm){
  int idx = 0;
  for(idx; idx<MEM_SIZE; idx++){
    printf("[%c] ", cm->_mem[idx]);
  }
  printf("\n");

  //printf("_mem_used = %d\n", cm->_mem_used);
  printf("virtual_addr = %d\n", cm->virtual_addr);
}

