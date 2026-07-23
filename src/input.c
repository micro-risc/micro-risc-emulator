#include "emulator.h"
#include "input.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

void printHelp(const char *prog) {
    fprintf(stderr,
        "Usage: %s [options] <program.hex>\n"
        "  -m, --mem-size <bytes>   total RAM size (default 16MiB)\n"
        "  -l, --load-addr <hex>    address to load program at\n"
        "  -e, --entry <hex>        initial PC (defaults to load-addr)\n"
        "  -c, --max-cycles <n>     stop after n instructions (0 = unlimited)\n"
        "  -s, --step               single-step w/ register dump\n"
        "  -t, --trace              log every executed instruction\n"
        "  -v, --verbose            extra diagnostic output\n"
        "  -h, --help               show this message\n",
        prog);
}

EmuArgs parseArgs(int argc, char **argv) {
    EmuArgs emuArgs = {
        .path   = NULL,
        .memSize        = 16u * 1024 * 1024,
        .loadAddr       = 0,
        .entry          = 0,
        .maxCycles      = 0,
        .step           = false,
        .trace          = false,
        .noTty          = false,
        .rawInput       = false,
        .verbose        = false,
    };
 
    struct option longOpts[] = {
        {"mem-size",   required_argument, NULL, 'm'},
        {"load-addr",  required_argument, NULL, 'l'},
        {"entry",      required_argument, NULL, 'e'},
        {"max-cycles", required_argument, NULL, 'c'},
        {"step",       no_argument,       NULL, 's'},
        {"trace",      no_argument,       NULL, 't'},
        {"verbose",    no_argument,       NULL, 'v'},
        {"help",       no_argument,       NULL, 'h'},
        {0, 0, 0, 0}
    };
 
    int opt;
    int longIdx;
    while ((opt = getopt_long(argc, argv, "m:l:e:c:stvh", longOpts, &longIdx)) != -1) {
        switch (opt) {
            case 'm': 
                emuArgs.memSize   = strtoull(optarg, NULL, 0); 
                break;
            case 'l': // hex
                emuArgs.loadAddr  = strtoul(optarg, NULL, 16); 
                break; 
            case 'e': // hex
                emuArgs.entry      = strtoul(optarg, NULL, 16); 
                break; 
            case 'c': 
                emuArgs.maxCycles = strtoul(optarg, NULL, 0);  
                break;
            case 's': 
                emuArgs.step = true; 
                break;
            case 't': 
                emuArgs.trace = true; 
                break;
            case 'v': 
                emuArgs.verbose = true; 
                break;
            case 'h': 
                printHelp(argv[0]); 
                exit(EXIT_SUCCESS);
            default:  
                printHelp(argv[0]); 
                exit(EXIT_FAILURE);
        }
    }
 
    /* after getopt_long runs, optind points at the first non-option argv entry */
    if (optind >= argc) {
        fprintf(stderr, "error: missing required <program.hex>\n");
        printHelp(argv[0]);
        exit(EXIT_FAILURE);
    }
    emuArgs.path = argv[optind];
 
    if (emuArgs.entry == 0) {
        emuArgs.entry = emuArgs.loadAddr; /* default entry to load address if unset */
    }
 
    return emuArgs;
}