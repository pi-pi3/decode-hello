/*
 * A parallel method of decrypting "Hello, world!"
 *
 * Copyright © 2018, Szymon Walter
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not
 *    be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <stdlib.h> /* srand, rand */
#include <stdio.h> /* fprintf, fflush, stdout, stderr */
#include <string.h> /* strlen */
#include <time.h> /* time, struct timespec, nanosleep */

/* The author(-s) that made this great feat possible. */
#define AUTHOR "Szymon Walter"
/* Date of last update of this release. */
#define DATE "2018-04-30"
/* Current version. */
#define VERSION "1.1.2"
/* The zlib/libpng license. */
#define LICENSE \
"  Copyright (c) 2018, Szymon Walter\n" \
"  \n" \
"  This software is provided 'as-is', without any express or implied warranty.\n" \
"  In no event will the authors be held liable for any damages arising from\n" \
"  the use of this software.\n" \
"  \n" \
"  Permission is granted to anyone to use this software for any purpose,\n" \
"  including commercial applications, and to alter it and redistribute it\n" \
"  freely, subject to the following restrictions:\n" \
"  \n" \
"  1. The origin of this software must not be misrepresented; you must not\n" \
"     claim that you wrote the original software. If you use this software\n" \
"     in a product, an acknowledgment in the product documentation would be\n" \
"     appreciated but is not required.\n" \
"  2. Altered source versions must be plainly marked as such, and must not\n" \
"     be misrepresented as being the original software.\n" \
"  3. This notice may not be removed or altered from any source distribution."
/* A simple example for an argument that can be printed with this algorithm. */
#define EXAMPLE "'Hello, world!'"

/* 
 * The delay that is needed in each iteration of the algorithm, in order to
 * prevent the cpu from melting.
 *
 * default: 0 seconds, 5000000 nanoseconds
 */
#ifndef DELAY_SEC
#define DELAY_SEC 0
#endif /* ifndef DELAY_SEC */

/* see above */
#ifndef DELAY_NSEC
#define DELAY_NSEC 10000000
#endif /* ifndef DELAY_NSEC */

/*
 * Improve accuracy by randomly ignoring N out of every 256 positive results.
 * We don't want unlucky results, they're definitely false positives.
 *
 * default: 127
 */
#ifndef RANDOM_IGNORE 
#define RANDOM_IGNORE 127
#endif /* ifndef DELAY */

/* Returns a random integer in the range [min; max). */
static inline int randi(int min, int max) {
    return (rand() % (max - min)) + min;
}

/* Program entry. */
int main(int argc, char **argv) {
    int i;
    int j;
    int len;
    int decoded;
    char *target = NULL;
    char *buffer = NULL;
    struct timespec delay;

    if (argc == 1) {
        fprintf(stderr, "usage: %s STRING...\n\n", *argv);
        fprintf(stderr, "EXAMPLE\n  %s %s\n\n", *argv, EXAMPLE);
        fprintf(stderr, "LICENSE\n%s\n\n", LICENSE);
        fprintf(stderr, "AUTHOR(-s)\n  %s\n\n", AUTHOR);
        fprintf(stderr, "UPDATE DATE\n  %s\n\n", DATE);
        fprintf(stderr, "VERSION\n  %s\n", VERSION);
        return 1;
    }

    srand(time(NULL));
    
    delay.tv_sec = DELAY_SEC;
    delay.tv_nsec = DELAY_NSEC;

    for (i = 1; i < argc; i++) {
        target = argv[i];
        len = strlen(target);
        buffer = malloc(len + 1);
        decoded = 0;

        for (j = 0; j < len; j++) {
            buffer[j] = randi(0x20, 0x7f);
        }
        buffer[len] = 0;

        fprintf(stdout, "%s", buffer);
        fflush(stdout);

        while (!decoded) {
            decoded = 1;

            fprintf(stdout, "\033[%dD", len);

            for (j = 0; j < len; j++) {
                if (buffer[j] == target[j]) {
                    fprintf(stdout, "\033[C");
                    continue;
                }

                do {
                    buffer[j] = randi(0x20, 0x7f);
                } while ((rand() & 0xff) < RANDOM_IGNORE);

                fprintf(stdout, "%c", buffer[j]);
                fflush(stdout);

                if (buffer[j] != target[j]) {
                    decoded = 0;
                }
            }

            fflush(stdout);
            nanosleep(&delay, NULL);
        }

        fprintf(stdout, "\n");
        free(buffer);
    }

    return 0;
}
