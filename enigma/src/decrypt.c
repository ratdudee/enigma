#include "../include/config.h"
#include "../include/decrypt.h"
#include "../include/encrypt.h"

void decrypt(Config* config) {
    /* Classical Enigma is symmetric: encryption and decryption are identical
       when run with the same key state. This wrapper calls encrypt() for clarity. */
    encrypt(config);
}
