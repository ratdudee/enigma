#include "../include/config.h"
#include "../include/key-parser.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>



int load_config_from_args(int argc, char* argv[], Config* cfg, int* do_encrypt) {
    if (!cfg || !do_encrypt) return 1;
    memset(cfg, 0, sizeof(*cfg));

    const char* infile = NULL;
    const char* outfile = NULL;
    const char* keyfile = NULL;
    const char* reflector = NULL;
    const char* rotors = NULL;
    const char* rings = NULL;
    const char* plugboard = NULL;

    int mode_encrypt = 1; // default: encrypt

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            infile = argv[++i];
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            outfile = argv[++i];
        } else if (strcmp(argv[i], "-k") == 0 && i + 1 < argc) {
            keyfile = argv[++i];
        } else if (strcmp(argv[i], "--reflector") == 0 && i + 1 < argc) {
            reflector = argv[++i];
        } else if (strcmp(argv[i], "--rotors") == 0 && i + 1 < argc) {
            rotors = argv[++i];
        } else if (strcmp(argv[i], "--rings") == 0 && i + 1 < argc) {
            rings = argv[++i];
        } else if (strcmp(argv[i], "--plugboard") == 0 && i + 1 < argc) {
            plugboard = argv[++i];
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--decrypt") == 0) {
            mode_encrypt = 0;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            fprintf(stdout, "Usage: %s -i input.txt (-o output.txt) (-k keyfile | --reflector R --rotors W --rings R) [-d]\n", argv[0]);
            return -2;
        } else {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            return 2;
        }
    }

    if (!infile) {
        fprintf(stderr, "Input file is required (-i).\n");
        return 3;
    }


    FILE* f = fopen(infile, "rb");
    if (!f) { perror("fopen"); return 4; }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz < 0) { fclose(f); return 5; }
    cfg->input = malloc((size_t)sz + 1);
    if (!cfg->input) { fclose(f); return 6; }
    if (fread(cfg->input, 1, (size_t)sz, f) != (size_t)sz) {
        free(cfg->input); cfg->input = NULL; fclose(f); return 7;
    }
    cfg->input[sz] = '\0';
    fclose(f);


    if (keyfile) {
        cfg->key = parse_key_file(keyfile);
        if (!cfg->key) { free_config(cfg); return 8; }
    } else {
        // must at least provide rotors/rings/reflector via args
        cfg->key = parse_key_components(reflector, rotors, rings, plugboard);
        if (!cfg->key) { free_config(cfg); return 9; }
    }

    if (outfile) cfg->output_path = strdup(outfile);

    cfg->out_buffer = NULL;
    *do_encrypt = mode_encrypt;
    return 0;
}

void free_config(Config* cfg) {
    if (!cfg) return;
    if (cfg->input) { free(cfg->input); cfg->input = NULL; }
    if (cfg->out_buffer) { free(cfg->out_buffer); cfg->out_buffer = NULL; }
    if (cfg->output_path) { free(cfg->output_path); cfg->output_path = NULL; }
    if (cfg->key) { free_key(cfg->key); cfg->key = NULL; }
}
