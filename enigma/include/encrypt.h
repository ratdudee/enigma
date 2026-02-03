#ifndef ENCRYPT_H
#define ENCRYPT_H

#include "config.h"



/* Encrypt the message in the provided Config. The function writes the result into
   config->out_buffer. If out_buffer is NULL it will be allocated (caller must free).
*/
void encrypt(Config* config);



#endif /* ENCRYPT_H */
