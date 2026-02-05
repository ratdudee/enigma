#include "../include/key-parser.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/* Standard Enigma rotor wirings (I-VIII) and their notch positions */
static const unsigned char ROTOR_WIRINGS[8][26] = {
    /* Rotor I:   */   {4, 10, 12, 3, 8, 0, 14, 4, 13, 1, 5, 2, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 25},
    /* Rotor II:  */   {0, 9, 3, 10, 18, 8, 17, 20, 23, 1, 11, 7, 22, 19, 12, 2, 16, 6, 15, 25, 5, 21, 14, 13, 4, 24},
    /* Rotor III: */   {1, 17, 6, 18, 0, 12, 23, 20, 13, 7, 11, 4, 9, 10, 25, 2, 22, 19, 14, 5, 21, 8, 16, 3, 15, 24},
    /* Rotor IV:  */   {4, 18, 14, 21, 15, 25, 9, 0, 24, 16, 20, 8, 17, 7, 23, 13, 11, 26, 3, 10, 19, 5, 2, 12, 1, 22},
    /* Rotor V:   */   {21, 25, 1, 17, 6, 14, 20, 23, 11, 8, 15, 4, 10, 2, 22, 9, 19, 0, 12, 7, 13, 24, 18, 3, 16, 5},
    /* Rotor VI:  */   {9, 15, 3, 13, 14, 0, 25, 5, 7, 10, 16, 2, 27, 22, 4, 11, 17, 8, 19, 23, 12, 18, 1, 24, 20, 6},
    /* Rotor VII: */   {13, 25, 9, 18, 11, 19, 27, 16, 0, 14, 22, 4, 17, 15, 23, 6, 8, 24, 12, 3, 1, 5, 2, 20, 10, 21},
    /* Rotor VIII:*/   {5, 18, 14, 8, 13, 21, 20, 24, 11, 19, 9, 10, 0, 16, 2, 25, 22, 4, 17, 23, 12, 1, 15, 3, 7, 6}
};

/* Notch positions for rotors I-VIII (at which position the next rotor steps) */
static const unsigned char ROTOR_NOTCHES[8] = {
    16,  /* Rotor I notch at Q (position 16) */
    4,   /* Rotor II notch at E (position 4) */
    21,  /* Rotor III notch at V (position 21) */
    9,   /* Rotor IV notch at J (position 9) */
    25,  /* Rotor V notch at Z (position 25) */
    25,  /* Rotor VI notch at Z (position 25) */
    25,  /* Rotor VII notch at Z (position 25) */
    25   /* Rotor VIII notch at Z (position 25) */
};

/* Standard Enigma reflectors (B and C) */
static const unsigned char REFLECTOR_B[26] = {24, 17, 20, 7, 16, 18, 11, 3, 15, 23, 13, 6, 14, 10, 12, 8, 4, 1, 5, 25, 2, 22, 21, 9, 0, 19};
static const unsigned char REFLECTOR_C[26] = {5, 21, 1, 22, 8, 17, 19, 12, 2, 13, 14, 4, 15, 9, 11, 6, 16, 7, 10, 25, 20, 3, 18, 0, 24, 23};

static int letter_to_index(const char* tok) {
    if (!tok || !tok[0]) return -1;
    if (isalpha((unsigned char)tok[0])) {
        char c = toupper((unsigned char)tok[0]);
        return c - 'A';
    }
    char* end = NULL;
    long v = strtol(tok, &end, 10);
    if (end && *end == '\0' && v >= 1 && v <= 26) return (int)(v - 1);
    return -1;
}

static char* trim(char* s) {
    if (!s) return s;
    while (isspace((unsigned char)*s)) ++s;
    char* end = s + strlen(s);
    while (end > s && isspace((unsigned char)*(end-1))) *(--end) = '\0';
    return s;
}

struct Key* parse_key_components(const char* reflector_str, const char* rotors_str, const char* rings_str, const char* plugboard_str) {
    struct Key* k = calloc(1, sizeof(*k));
    if (!k) return NULL;

    /* Initialize default plugboard: identity mapping (A->A, B->B, etc.) */
    for (int i = 0; i < 26; ++i) k->plugboard_settings[i] = (unsigned char)i;

    /* Parse and setup reflector */
    if (reflector_str) {
        int reflector_idx = letter_to_index(reflector_str);
        if (reflector_idx < 0 || reflector_idx >= 2) { 
            fprintf(stderr, "Error: reflector must be 1 (B) or 2 (C), got: %s\n", reflector_str);
            free(k); 
            return NULL; 
        }
        k->reflector = malloc(sizeof(*k->reflector));
        if (!k->reflector) { free(k); return NULL; }
        
        /* Copy appropriate reflector wiring */
        if (reflector_idx == 0) {  /* B */
            memcpy(k->reflector->wiring, REFLECTOR_B, sizeof(REFLECTOR_B));
        } else {  /* C */
            memcpy(k->reflector->wiring, REFLECTOR_C, sizeof(REFLECTOR_C));
        }
    } else {
        /* Default to reflector B */
        k->reflector = malloc(sizeof(*k->reflector));
        if (!k->reflector) { free(k); return NULL; }
        memcpy(k->reflector->wiring, REFLECTOR_B, sizeof(REFLECTOR_B));
    }

    /* Parse rotor specifications */
    if (rotors_str) {
        /* Count comma-separated values */
        size_t count = 1;
        for (const char* p = rotors_str; *p; ++p) if (*p == ',') ++count;
        
        k->rotors = calloc(count, sizeof(*k->rotors));
        if (!k->rotors) { free_key(k); return NULL; }
        k->rotor_count = count;
        
        char* tmp = strdup(rotors_str);
        if (!tmp) { free_key(k); return NULL; }
        
        char* saveptr = NULL;
        char* tok = strtok_r(tmp, ",", &saveptr);
        size_t idxw = 0;
        
        while (tok && idxw < count) {
            tok = trim(tok);
            int rotor_num = letter_to_index(tok);
            if (rotor_num < 0 || rotor_num >= 8) {
                fprintf(stderr, "Error: rotor must be 1-8, got: %s\n", tok);
                free(tmp); 
                free_key(k); 
                return NULL;
            }
            
            /* Initialize rotor with wiring and notch */
            Rotor* rotor = &k->rotors[idxw];
            memcpy(rotor->wiring, ROTOR_WIRINGS[rotor_num], sizeof(rotor->wiring));
            rotor->notch = ROTOR_NOTCHES[rotor_num];
            rotor->position = 0;  /* Default message key position (will be set by ring settings) */
            
            idxw++;
            tok = strtok_r(NULL, ",", &saveptr);
        }
        free(tmp);
    }

    /* Parse ring settings (and message keys if provided) */
    if (rings_str) {
        size_t count = 1;
        for (const char* p = rings_str; *p; ++p) if (*p == ',') ++count;
        
        if (k->rotor_count && count != k->rotor_count) {
            fprintf(stderr, "Error: ring settings count (%zu) != rotor count (%zu)\n", count, k->rotor_count);
            free_key(k); 
            return NULL;
        }
        
        k->ring_settings = calloc(count, sizeof(*(k->ring_settings)));
        if (!k->ring_settings) { free_key(k); return NULL; }
        
        char* tmp = strdup(rings_str);
        if (!tmp) { free_key(k); return NULL; }
        
        char* saveptr = NULL;
        char* tok = strtok_r(tmp, ",", &saveptr);
        size_t idxr = 0;
        
        while (tok && idxr < count) {
            tok = trim(tok);
            int v = letter_to_index(tok);
            if (v < 0 || v >= 26) {
                fprintf(stderr, "Error: ring setting must be A-Z, got: %s\n", tok);
                free(tmp); 
                free_key(k); 
                return NULL;
            }
            k->ring_settings[idxr] = (RingSetting)v;
            
            /* Set rotor message key position from ring setting */
            if (idxr < k->rotor_count) {
                k->rotors[idxr].position = (unsigned char)v;
            }
            
            idxr++;
            tok = strtok_r(NULL, ",", &saveptr);
        }
        free(tmp);
    } else if (k->rotor_count > 0) {
        /* Allocate default ring settings if rotors are specified but rings aren't */
        k->ring_settings = calloc(k->rotor_count, sizeof(*(k->ring_settings)));
        if (!k->ring_settings) { free_key(k); return NULL; }
        for (size_t i = 0; i < k->rotor_count; ++i) {
            k->ring_settings[i] = 0;
            k->rotors[i].position = 0;
        }
    }

    /* Parse plugboard swaps */
    if (plugboard_str) {
        char* tmp = strdup(plugboard_str);
        if (!tmp) { free_key(k); return NULL; }
        
        char* saveptr = NULL;
        char* tok = strtok_r(tmp, ",", &saveptr);
        
        while (tok) {
            char* t = trim(tok);
            /* Accept formats: AB or A-B */
            char a = t[0];
            char b = '\0';
            if (t[1] == '-' && t[2]) b = t[2]; 
            else if (t[1]) b = t[1];
            
            if (!isalpha((unsigned char)a) || !isalpha((unsigned char)b)) { 
                fprintf(stderr, "Error: plugboard pairs must be letters, got: %s\n", t);
                free(tmp); 
                free_key(k); 
                return NULL; 
            }
            
            int ia = toupper((unsigned char)a) - 'A';
            int ib = toupper((unsigned char)b) - 'A';
            
            /* Swap in plugboard (bidirectional) */
            k->plugboard_settings[ia] = (unsigned char)ib;
            k->plugboard_settings[ib] = (unsigned char)ia;
            
            tok = strtok_r(NULL, ",", &saveptr);
        }
        free(tmp);
    }

    return k;
}

struct Key* parse_key_file(const char* path) {
    if (!path) return NULL;
    FILE* f = fopen(path, "r");
    if (!f) return NULL;
    char line[512];
    char* reflector = NULL;
    char* rotors = NULL;
    char* rings = NULL;
    char* plugboard = NULL;
    while (fgets(line, sizeof(line), f)) {
        char* p = line;
        while (isspace((unsigned char)*p)) ++p;
        if (*p == '\0' || *p == '#') continue;
        char* eq = strchr(p, '=');
        if (!eq) continue;
        *eq = '\0';
        char* key = trim(p);
        char* val = trim(eq + 1);
        // remove trailing newline
        char* nl = strchr(val,'\n'); if (nl) *nl = '\0';
        if (strcasecmp(key, "reflector") == 0) reflector = strdup(val);
        else if (strcasecmp(key, "rotors") == 0) rotors = strdup(val);
        else if (strcasecmp(key, "rings") == 0) rings = strdup(val);
        else if (strcasecmp(key, "plugboard") == 0) plugboard = strdup(val);
    }
    fclose(f);
    struct Key* k = parse_key_components(reflector, rotors, rings, plugboard);
    free(reflector); free(rotors); free(rings); free(plugboard);
    return k;
}

void free_key(struct Key* k) {
    if (!k) return;
    if (k->reflector) free(k->reflector);
    if (k->rotors) free(k->rotors);
    if (k->ring_settings) free(k->ring_settings);
    free(k);
}
