#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXT 25
#define NSIG 128   /* one slot per character: signal names are single letters */

/* gate table, indexed by the output signal's letter */
static int  is_gate[NSIG];
static char op_of[NSIG][5];
static int  in1[NSIG], in2[NSIG];

/* is_input[s]: s is an input signal; inp[s][k]: its value in cycle k */
static int is_input[NSIG];
static int inp[NSIG][MAXT];

/* val[s][k]: value of signal s in cycle k (0-based cycles) */
static int val[NSIG][MAXT];

static int apply(const char *op, int x, int y) {
    if (strcmp(op, "AND")  == 0) return x & y;
    if (strcmp(op, "OR")   == 0) return x | y;
    if (strcmp(op, "NAND") == 0) return !(x & y);
    if (strcmp(op, "NOR")  == 0) return !(x | y);
    if (strcmp(op, "XOR")  == 0) return x ^ y;
    return 0;
}

/* Read the next non-empty line, upper-cased. Returns 0 at end of input. */
static int next_line(char *buf, int size) {
    while (fgets(buf, size, stdin)) {
        for (char *p = buf; *p; p++) *p = (char)toupper((unsigned char)*p);
        for (char *p = buf; *p; p++)
            if (!isspace((unsigned char)*p)) return 1;
    }
    return 0;
}

int main(void) {
    char line[512];
    int n, t;

    if (!next_line(line, sizeof line)) return 0;
    sscanf(line, "%d", &n);

    /* Gate lines like "C=AND(A, B)": keep only letters, then the line is
       OUT, OP..., IN1, IN2 with the op name in the middle. */
    for (int i = 0; i < n; i++) {
        next_line(line, sizeof line);
        char letters[64]; int m = 0;
        for (char *p = line; *p && m < 63; p++)
            if (isalpha((unsigned char)*p)) letters[m++] = *p;
        letters[m] = '\0';
        /* letters = OUT + OPNAME + IN1 + IN2 */
        int out = letters[0];
        int a   = letters[m - 2];
        int b   = letters[m - 1];
        int oplen = m - 3;
        if (oplen > 4) oplen = 4;              /* longest op name is NAND */
        memcpy(op_of[out], letters + 1, oplen);
        op_of[out][oplen] = '\0';
        is_gate[out] = 1;
        in1[out] = a;
        in2[out] = b;
    }

    next_line(line, sizeof line);
    sscanf(line, "%d", &t);

    /* Remaining lines: input signals, then the target gate name (last line). */
    char lines[64][512];
    int cnt = 0;
    while (cnt < 64 && next_line(lines[cnt], sizeof lines[cnt])) cnt++;

    for (int i = 0; i < cnt - 1; i++) {
        char *p = lines[i];
        while (isspace((unsigned char)*p)) p++;
        int s = *p++;
        is_input[s] = 1;
        for (int k = 0; k < t; k++) {
            int bit;
            if (sscanf(p, "%d", &bit) != 1) break;
            inp[s][k] = bit;
            while (isspace((unsigned char)*p)) p++;   /* skip to after this number */
            while (*p && !isspace((unsigned char)*p)) p++;
        }
    }
    char *tp = lines[cnt - 1];
    while (isspace((unsigned char)*tp)) tp++;
    int target = *tp;

    /* Cycle 1: every gate outputs 0. Cycle k: gate = op(inputs at cycle k-1). */
    for (int s = 0; s < NSIG; s++) val[s][0] = is_input[s] ? inp[s][0] : 0;
    for (int k = 1; k < t; k++) {
        for (int s = 0; s < NSIG; s++) {
            if (is_input[s])      val[s][k] = inp[s][k];
            else if (is_gate[s])  val[s][k] = apply(op_of[s], val[in1[s]][k - 1], val[in2[s]][k - 1]);
        }
    }

    for (int k = 0; k < t; k++) printf(k ? " %d" : "%d", val[target][k]);
    printf("\n");
    return 0;
}
