#include "checksum.h"

unsigned short csum(unsigned short *ptr, int nbytes) {
  register long sum = 0;
  unsigned short oddbyte;
  register short answer;

  while (nbytes > 1) {
    sum += *ptr++;
    nbytes -= 2;
  }
  if (nbytes == 1) {
    oddbyte = 0;
    *((unsigned char *)&oddbyte) = *(unsigned char *)ptr;
    sum += oddbyte;
  }
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  answer = (short)~sum;
  return answer;
}