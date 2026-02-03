#include "../include/key-parser.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

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

struct Key* parse_key_components(const char* reflector_str, const char* wheels_str, const char* rings_str, const char* plugboard_str) {
    struct Key* k = calloc(1, sizeof(*k));
    if (!k) return NULL;

    // default plugboard: identity 
    for (int i = 0; i < 26; ++i) k->plugboard_settings[i] = (RingSetting)i;

    // reflector
    if (reflector_str) {
        int idx = letter_to_index(reflector_str);
        if (idx < 0) { free(k); return NULL; }
        k->reflector = malloc(sizeof(*k->reflector));
        if (!k->reflector) { free(k); return NULL; }
        k->reflector->m = idx;
    }

    // wheels
    if (wheels_str) {
        // count commas
        size_t count = 1;
        for (const char* p = wheels_str; *p; ++p) if (*p == ',') ++count;
        k->wheels = calloc(count, sizeof(*k->wheels));
        if (!k->wheels) { free_key(k); return NULL; }
        k->wheel_count = count;
        char* tmp = strdup(wheels_str);
        char* saveptr = NULL;
        char* tok = strtok_r(tmp, ",", &saveptr);
        size_t idxw = 0;
        while (tok && idxw < count) {
            tok = trim(tok);
            int v = letter_to_index(tok);
            if (v < 0) {
                free(tmp); free_key(k); return NULL;
            }
            k->wheels[idxw++].m = v;
            tok = strtok_r(NULL, ",", &saveptr);
        }
        free(tmp);
    }

    // rings
    if (rings_str) {
        /* allocate ring settings equal to wheel_count if present, otherwise inferred */
        size_t count = 1;
        for (const char* p = rings_str; *p; ++p) if (*p == ',') ++count;
        if (k->wheel_count && count != k->wheel_count) {
            free_key(k); return NULL;
        }
        k->ring_settings = calloc(count, sizeof(*(k->ring_settings)));
        if (!k->ring_settings) { free_key(k); return NULL; }
        char* tmp = strdup(rings_str);
        char* saveptr = NULL;
        char* tok = strtok_r(tmp, ",", &saveptr);
        size_t idxr = 0;
        while (tok && idxr < count) {
            tok = trim(tok);
            int v = letter_to_index(tok);
            if (v < 0) { free(tmp); free_key(k); return NULL; }
            k->ring_settings[idxr++] = (RingSetting)v;
            tok = strtok_r(NULL, ",", &saveptr);
        }
        free(tmp);
    }

    // plugboard
    if (plugboard_str) {
        char* tmp = strdup(plugboard_str);
        char* saveptr = NULL;
        char* tok = strtok_r(tmp, ",", &saveptr);
        while (tok) {
            char* t = trim(tok);
            // accept formats: AB or A-B
            char a = t[0];
            char b = '\0';
            if (t[1] == '-' && t[2]) b = t[2]; else if (t[1]) b = t[1];
            if (!isalpha((unsigned char)a) || !isalpha((unsigned char)b)) { free(tmp); free_key(k); return NULL; }
            int ia = toupper((unsigned char)a) - 'A';
            int ib = toupper((unsigned char)b) - 'A';
            // swap in plugboard
            k->plugboard_settings[ia] = (RingSetting)ib;
            k->plugboard_settings[ib] = (RingSetting)ia;
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
    char* wheels = NULL;
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
        else if (strcasecmp(key, "wheels") == 0) wheels = strdup(val);
        else if (strcasecmp(key, "rings") == 0) rings = strdup(val);
        else if (strcasecmp(key, "plugboard") == 0) plugboard = strdup(val);
    }
    fclose(f);
    struct Key* k = parse_key_components(reflector, wheels, rings, plugboard);
    free(reflector); free(wheels); free(rings); free(plugboard);
    return k;
}

void free_key(struct Key* k) {
    if (!k) return;
    if (k->reflector) free(k->reflector);
    if (k->wheels) free(k->wheels);
    if (k->ring_settings) free(k->ring_settings);
    free(k);
}
