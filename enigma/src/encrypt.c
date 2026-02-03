#include "../include/config.h"
#include "../include/encrypt.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

static inline int mod26(int x) {
    int r = x % 26;
    if (r < 0) r += 26;
    return r;
}

static int apply_plugboard(const Config* config, int val) {
    if (!config || !config->key) return val;
    const Plugboard* p = &config->key->plugboard_settings;
    int mapped = (int)(*p)[val];
    if (mapped >= 0 && mapped < 26) return mapped;
    return val;
}

static int pass_through_rotors_forward(const Config* config, int val) {
    if (!config || !config->key) return val;
    const Key* k = config->key;
    for (size_t i = 0; i < k->wheel_count; ++i) {
        int rotor_shift = k->wheels[i].m;
        int ring = (int)k->ring_settings[i];
        val = mod26(val + rotor_shift + ring);
    }
    return val;
}

static int pass_through_reflector(const Config* config, int val) {
    if (!config || !config->key || !config->key->reflector) return val;
    int m = config->key->reflector->m;
    return mod26(m - val);
}

static int pass_through_rotors_backward(const Config* config, int val) {
    if (!config || !config->key) return val;
    const Key* k = config->key;
    if (k->wheel_count == 0) return val;
    for (size_t i = k->wheel_count; i-- > 0;) {
        int rotor_shift = k->wheels[i].m;
        int ring = (int)k->ring_settings[i];
        val = mod26(val - rotor_shift - ring);
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
        if (!out) return; /* allocation failed */
        allocated = true;
    }

    for (size_t i = 0; i < len; ++i) {
        char c = in[i];
        if (!isalpha((unsigned char)c)) {
            out[i] = c;
            continue;
        }

        bool is_upper = isupper((unsigned char)c);
        int idx = (toupper((unsigned char)c) - 'A');


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
