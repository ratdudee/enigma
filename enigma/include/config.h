#ifndef CONFIG_H
#define CONFIG_H
#include <stddef.h>

typedef enum RingSetting RingSetting;
typedef struct Config Config;
typedef struct Key Key;
typedef struct Rotor Rotor;
typedef struct Reflector Reflector;


typedef enum RingSetting {
    A1, B2, C3, D4, E5, F6, G7, H8, I9, J10, K11, L12, M13, N14, O15, P16, Q17, R18, S19, T20, U21, V22, W23, X24, Y25, Z26
} RingSetting;

typedef RingSetting Plugboard[26];

typedef struct Config {
    char* input;
    struct Key* key;
    char* out_buffer;       // output buffer (allocated by encrypt/decrypt if NULL)
    char* output_path;      // optional output filename (allocated) 
} Config;

/* Parse command line arguments into cfg. On success return 0 and set *do_encrypt:
   1 = encrypt, 0 = decrypt. Returns negative on help (-2) or positive on error.
   Caller must free resources with free_config(). */
int load_config_from_args(int argc, char* argv[], Config* cfg, int* do_encrypt);


void free_config(Config* cfg);

typedef struct Key {
    struct Reflector* reflector;        // Umkehrwalze
    struct Rotor* wheels;               // genutzte Walzen in genutzter Reihenfolge
    enum RingSetting* ring_settings;    // jeweilige Ringstellung
    size_t wheel_count;
    Plugboard plugboard_settings;       // Verbindungen auf Steckerbrett
    
} Key;



typedef struct Rotor {
    int m;
} Rotor;


typedef struct Reflector {
    int m;
} Reflector;



#endif