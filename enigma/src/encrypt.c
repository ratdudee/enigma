#include "../include/config.h"
#include "../include/encrypt.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

/* Utility: modulo that handles negative values correctly */
static inline int mod26(int x) {
    int r = x % 26;
    if (r < 0) r += 26;
    return r;
}

/* Apply plugboard transformation (before and after rotor stack) */
static int apply_plugboard(const Config* config, int val) {
    if (!config || !config->key || val < 0 || val >= 26) return val;
    const Plugboard* p = &config->key->plugboard_settings;
    int mapped = (int)(*p)[val];
    if (mapped >= 0 && mapped < 26) return mapped;
    return val;
}

/* 
 * Pass through rotor stack forward (left to right).
 * Each rotor applies its wiring, adjusted for position and ring setting.
 * Position: current rotor position (0-25)
 * Ring setting: offset applied to both entry and exit (0-25)
 */
static int pass_through_rotors_forward(const Config* config, int val) {
    if (!config || !config->key || val < 0 || val >= 26) return val;
    const Key* k = config->key;
    
    for (size_t i = 0; i < k->rotor_count; ++i) {
        const Rotor* rotor = &k->rotors[i];
        int pos = (int)rotor->position;
        int ring = (int)k->ring_settings[i];
        
        /* Adjust for rotor position and ring setting on entry */
        int entry = mod26(val + pos - ring);
        
        /* Apply rotor's internal wiring */
        int output = (int)rotor->wiring[entry];
        
        /* Adjust for rotor position and ring setting on exit */
        val = mod26(output - pos + ring);
    }
    
    return val;
}

/* Pass through reflector (applies symmetric wiring) */
static int pass_through_reflector(const Config* config, int val) {
    if (!config || !config->key || !config->key->reflector || val < 0 || val >= 26) 
        return val;
    const Reflector* refl = config->key->reflector;
    return (int)refl->wiring[val];
}

/* 
 * Pass through rotor stack backward (right to left).
 * Rotors are traversed in reverse order with inverse wiring.
 */
static int pass_through_rotors_backward(const Config* config, int val) {
    if (!config || !config->key || val < 0 || val >= 26) return val;
    const Key* k = config->key;
    
    if (k->rotor_count == 0) return val;
    
    for (size_t i = k->rotor_count; i-- > 0;) {
        const Rotor* rotor = &k->rotors[i];
        int pos = (int)rotor->position;
        int ring = (int)k->ring_settings[i];
        
        /* Adjust for rotor position and ring setting on entry */
        int entry = mod26(val + pos - ring);
        
        /* Find inverse: which input to wiring[x] gives us entry? */
        int output = -1;
        for (int j = 0; j < 26; ++j) {
            if (rotor->wiring[j] == entry) {
                output = j;
                break;
            }
        }
        if (output < 0) output = entry;  // fallback
        
        /* Adjust for rotor position and ring setting on exit */
        val = mod26(output - pos + ring);
    }
    
    return val;
}

void encrypt(Config* config) {
    if (!config || !config->input) return;

    const char* in = config->input;
    size_t len = strlen(in);

    char* out = config->out_buffer;
    bool allocated = false;
    if (!out) {
        out = (char*)malloc(len + 1);
        if (!out) return;
        allocated = true;
    }

    for (size_t i = 0; i < len; ++i) {
        char c = in[i];
        if (c == ' ') c = 'X';                  // spaces to X
        else if (!isalpha((unsigned char)c)) {  // only allow characters
            out[i] = c;
            continue;
        }

        bool is_upper = isupper((unsigned char)c);
        int idx = (toupper((unsigned char)c) - 'A');    // convert ASCII to char 0 - 25


        idx = apply_plugboard(config, idx);


        idx = pass_through_rotors_forward(config, idx);


        idx = pass_through_reflector(config, idx);


        idx = pass_through_rotors_backward(config, idx);


        idx = apply_plugboard(config, idx);

        char outch = (char)('A' + idx);
        if (!is_upper) outch = (char)tolower((unsigned char)outch);
        out[i] = outch;
    }

    out[len] = '\0';

    if (allocated) config->out_buffer = out;
}
