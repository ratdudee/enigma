#ifndef DECRYPT_H
#define DECRYPT_H

#include "config.h"


/* Decrypt the message in the provided Config. Writes the result into
   config->out_buffer. If out_buffer is NULL it will be allocated (caller must free).
   For a classical Enigma-like implementation, encryption and decryption are symmetric,
   but this function is provided for API clarity.
*/
void decrypt(Config* config);


#endif /* DECRYPT_H */
