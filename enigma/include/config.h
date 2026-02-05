#ifndef CONFIG_H
#define CONFIG_H
#include <stddef.h>

typedef struct Config Config;
typedef struct Key Key;
typedef struct Rotor Rotor;
typedef struct Reflector Reflector;

/* Enum for the 26 allowed letters A-Z */
typedef enum {
    LETTER_A = 0,  LETTER_B = 1,  LETTER_C = 2,  LETTER_D = 3,  LETTER_E = 4,
    LETTER_F = 5,  LETTER_G = 6,  LETTER_H = 7,  LETTER_I = 8,  LETTER_J = 9,
    LETTER_K = 10, LETTER_L = 11, LETTER_M = 12, LETTER_N = 13, LETTER_O = 14,
    LETTER_P = 15, LETTER_Q = 16, LETTER_R = 17, LETTER_S = 18, LETTER_T = 19,
    LETTER_U = 20, LETTER_V = 21, LETTER_W = 22, LETTER_X = 23, LETTER_Y = 24,
    LETTER_Z = 25
} Letter;

/* Ring setting: A-Z (0-25) */
typedef unsigned char RingSetting;

/* Plugboard: maps each of 26 letters to another (0-25) */
typedef unsigned char Plugboard[26];

/* Rotor: has internal wiring, notch position, and current position (message key) */
typedef struct Rotor {
    unsigned char wiring[26];           // internal wiring permutation
    unsigned char notch;                // position (0-25) where rotor steps
    unsigned char position;             // current position (message key): 0-25
} Rotor;

/* Reflector: has internal wiring that swaps pairs */
typedef struct Reflector {
    unsigned char wiring[26];           // reflection wiring: must be symmetric (wiring[wiring[i]] == i)
} Reflector;

/* Key: enigma key configuration */
typedef struct Key {
    struct Reflector* reflector;        // Umkehrwalze (required)
    struct Rotor* rotors;               // rotors in order (e.g., left to right)
    RingSetting* ring_settings;         // ring settings for each rotor (0-25)
    size_t rotor_count;                 // number of rotors used
    Plugboard plugboard_settings;       // plugboard swaps (0-25 for each of 26 inputs)
} Key;

/* Config: runtime configuration for encryption/decryption */
typedef struct Config {
    char* input;
    struct Key* key;
    char* out_buffer;                   // output buffer (allocated by encrypt/decrypt if NULL)
    char* output_path;                  // optional output filename (allocated) 
} Config;

/* Parse command line arguments into cfg. On success return 0 and set *do_encrypt:
   1 = encrypt, 0 = decrypt. Returns negative on help (-2) or positive on error.
   Caller must free resources with free_config(). */
int load_config_from_args(int argc, char* argv[], Config* cfg, int* do_encrypt);

/* Free all resources allocated in cfg */
void free_config(Config* cfg);



#endif