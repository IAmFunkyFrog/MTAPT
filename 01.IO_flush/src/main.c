#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>

#define HELP \
    "Application to make IO flush experiment\n" \
    "Simply put string STDOUT to stdout and after put string STDERR to stderr\n\n" \
    "\t-e - add endline symbol after each string\n" \
    "\t-r - set count of repetitions STDOUT and STDERR\n" \
    "\t-f - set flush mode. Possible values:\n" \
    "\t\tNO - no flush\n" \
    "\t\tEACH_PRINT - flush after each print on stdout or stderr\n" \
    "\t\tBATCH - flush after all prints on stdout and stderr\n"

enum FlushMode {
    NO,         // Не делать flush
    EACH_PRINT, // Делать flush на каждый вывод строки
    BATCH       // Делать flush после того, как все строки напечатаны в буффер
};

enum FlushMode flush_mode_from_string(const char *str) {
    if (strcmp(str, "EACH_PRINT") == 0) {
        return EACH_PRINT;
    } else if (strcmp(str, "BATCH") == 0) {
        return BATCH;
    }

    return NO;
}

struct ExperimentSettings {
    char endline_symbol;       // Символ в конце каждой строки
    int repetitions_count;     // Количество повторений вывода строки
    enum FlushMode flush_mode;
};

struct ExperimentSettings default_experiment_settings() {
    return (struct ExperimentSettings) {
        .endline_symbol = ' ',
        .repetitions_count = 3,
        .flush_mode = NO,
    };
}

void print_string(const char *str, FILE *out, struct ExperimentSettings settings) {
    for (int i = 0; i < settings.repetitions_count; i++) {
        fputs(str, out);
        if (settings.flush_mode == EACH_PRINT) fflush(out);
    }
    if (settings.flush_mode == BATCH) fflush(out);
}

int main(int argc, char *argv[])
{
    struct ExperimentSettings settings = default_experiment_settings();
    int opt;
    while ((opt = getopt(argc, argv, "er:f:")) != -1) {
        switch (opt) {
        case 'e':
            settings.endline_symbol = '\n';
            break;
        case 'r':
            settings.repetitions_count = atoi(optarg);
            break;
        case 'f':
            settings.flush_mode = flush_mode_from_string(optarg);
            break;
        default:
            fprintf(stderr, HELP);
            return -1;
        }
    }

    char stdout_string[10];
    char stderr_string[10];
    sprintf(stdout_string, "STDOUT%c", settings.endline_symbol);
    sprintf(stderr_string, "STDERR%c", settings.endline_symbol);

    // Тело эксперимента
    print_string(stdout_string, stdout, settings);
    print_string(stderr_string, stderr, settings);

    return 0;
}
