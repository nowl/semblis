#include <stdio.h>
#include <time.h>
#include <stdbool.h>

#include "logger.h"
#include "string.h"

static int log_level = LL_DEBUG;
static FILE *fout;
static bool console_output = false;

static const char* log_level_message[] = {
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR",
    "CRITICAL"
};

void logger_init(char *filename)
{
    fout = fopen(filename, "w");
}

void logger_close()
{
    fclose(fout);
}

void logger_set_level(LogLevel level)
{
    log_level = level;
}

void logger_set_console_output(bool setting)
{
    console_output = setting;
}

void logger_log(const char *logger_name, LogLevel level, char *message)
{
    if(level >= log_level) {
        char str[512];
        time_t t;
        char *ascTime;
        char ascTimeFiltered[9];

        // This is the first thing that came to mind to get at the
        // hour/min/sec string.. I'm sure there is an easier way to
        // do this.
        t = time(NULL);
        ascTime = asctime(localtime(&t));
        memcpy(ascTimeFiltered, &ascTime[11], 8);
        ascTimeFiltered[8] = 0;

        snprintf(str, 512, "%s %s [%s]  %s",
                 ascTimeFiltered,
                 logger_name,
                 log_level_message[level],
                 message);
	
        if(console_output)
            printf("%s\n", str);
	
        if(fout)
            fprintf(fout, "%s\n", str);
    }
}
