#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/config.h"
#include "../include/encrypt.h"

// TODO: arena allocator

int main(int argc, char* argv[]) {
    Config cfg;
    int do_encrypt = 1;
    int r = load_config_from_args(argc, argv, &cfg, &do_encrypt);
    if (r == -2) return 0;  // help
    if (r != 0) return r;

    encrypt(&cfg);

    if (!cfg.out_buffer) {
        fprintf(stderr, "No output produced\n");
        free_config(&cfg);
        return 1;
    }

    const char* outpath = cfg.output_path ? cfg.output_path : (do_encrypt ? "output.enc" : "output.dec");
    FILE* fout = fopen(outpath, "wb");
    if (!fout) { perror("fopen"); free_config(&cfg); return 2; }
    fwrite(cfg.out_buffer, 1, strlen(cfg.out_buffer), fout);
    fclose(fout);

    printf("Wrote result to %s\n", outpath);

    free_config(&cfg);
    return 0;
}
