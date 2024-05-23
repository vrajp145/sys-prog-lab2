/* Author:Vraj Patel
   Date: 2/15/24
    */

/***
 *      ______ .___  ___. .______     _______.  ______        ____    __   __
 *     /      ||   \/   | |   _  \   /       | /      |      |___ \  /_ | /_ |
 *    |  ,----'|  \  /  | |  |_)  | |   (----`|  ,----'        __) |  | |  | |
 *    |  |     |  |\/|  | |   ___/   \   \    |  |            |__ <   | |  | |
 *    |  `----.|  |  |  | |  |   .----)   |   |  `----.       ___) |  | |  | |
 *     \______||__|  |__| | _|   |_______/     \______|      |____/   |_|  |_|
 *
 */

#include "mdadm.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "jbod.h"
#include "util.h"

int check_mount = 0;

uint32_t use_addr(int cmd, int diskVal, int blockVal) {
  return (cmd << 14) | (diskVal << 28) | (blockVal << 20);
}

uint8_t *block = NULL;

int mdadm_mount(void) {
  /* YOUR CODE */
   uint32_t op = use_addr(JBOD_MOUNT, 0, 0);
   if (jbod_operation(op, NULL) == 0){
     check_mount = 1;
     return 1;
   }
   return -1;
}

int mdadm_unmount(void) {
  /* YOUR CODE */
   uint32_t op = use_addr(JBOD_UNMOUNT, 0, 0);
   if (jbod_operation(op, NULL) == 0){
     check_mount = 0;
     return 1;
   }
   return -1;
}

int mdadm_read(uint32_t addr, uint32_t len, uint8_t *buf) {
  /* YOUR CODE */
  if((check_mount == 0)||(len + addr > 1048576)|| (len > 1024) || (len > 0 && buf == NULL)){
    return -1;
    }
  int finish = len + addr; // Calculate final address

 // Loop through each block needed for the read operation
  for(int addr_copy = addr; addr_copy < finish;){

    // Calculate the disk and block numbers based on the current address
    int dB = addr_copy / 65536;
    int new_num = (addr_copy % 65536) / 256;
    int new_value = addr_copy % 256;

    // Calculate the destination address in the buffer and the amount to copy
    uint8_t *address_value = (buf-addr) + addr_copy;
    uint8_t remainder = finish - addr_copy;
    
    uint8_t temporaryBuf[256];
   
   // Seek to the correct disk
    uint32_t op1 = use_addr(JBOD_SEEK_TO_DISK, dB, 0);
    jbod_operation(op1, block);
    
     // Seek to the correct block within the disk
    uint32_t op2 = use_addr(JBOD_SEEK_TO_BLOCK,0, new_num);
    jbod_operation(op2, block);
    
     // Read the current block into the temporary buffer
    uint32_t op3 = use_addr(JBOD_READ_BLOCK, 0, 0);
    jbod_operation(op3, temporaryBuf);
 
    // Determine how many bytes to copy: either the remainder or up to 256 bytes
    int  holder_value = (finish - addr_copy < 256) ? remainder : 256;
    memcpy(address_value, temporaryBuf, holder_value);
     // Move to the next block, adjusting for any initial offset within the block
    addr_copy = (addr_copy-new_value) + 256;

    }

  return len; // Return the total number of bytes intended to read

  }
  
/*CWD /home/cmpsc311/sp24-lab2-vrajp145 */
/*CWD /home/cmpsc311/sp24-lab2-vrajp145 */
/*CWD /home/cmpsc311/sp24-lab2-vrajp145 */
/*CWD /home/cmpsc311/sp24-lab2-vrajp145 */
/*CWD /home/cmpsc311/sp24-lab2-vrajp145 */
