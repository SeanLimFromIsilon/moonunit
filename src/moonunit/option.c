#define OPTION_SUITE 1
#define OPTION_TEST 2
#define OPTION_ALL 3
#define OPTION_GDB 4
#define OPTION_LOGGER 5
#define OPTION_OPTION 6

#include <moonunit/util.h>
#include <moonunit/logger.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <signal.h>
#include <popt.h>
#include <stdbool.h>

#include "option.h"


static void
StringSet_Append(StringSet* set, const char* _str)
{
    char* str = strdup(_str);

    if (set->size == set->capacity)
    {
        if (set->capacity)
            set->capacity *= 2;
        else
            set->capacity = 16;
        set->value = realloc(set->value, set->capacity);
    }

    set->value[set->size++] = str;
}

static int
error(OptionTable* table, const char* fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);

    table->errormsg = formatv(fmt, ap);

    va_end(ap);

    return -2;
}

static const struct poptOption options[] =
{
    {
        .longName = "suite",
        .shortName = 's',
        .argInfo = POPT_ARG_STRING,
        .arg = NULL,
        .val = OPTION_SUITE,
        .descrip = "Run a specific test suite",
        .argDescrip = "<suite>"
    },
    {
        .longName = "test",
        .shortName = 't',
        .argInfo = POPT_ARG_STRING,
        .arg = NULL,
        .val = OPTION_TEST,
        .descrip = "Run a specific test",
        .argDescrip = "<suite>/<name>"
    },
    {
        .longName = "all",
        .shortName = 'a',
        .argInfo = POPT_ARG_NONE,
        .arg = NULL,
        .val = OPTION_ALL,
        .descrip = "Run all tests in all suites (default)",
        .argDescrip = "<suite>"
    },
    {
        .longName = "gdb",
        .shortName = 'g',
        .argInfo = POPT_ARG_NONE,
        .arg = NULL,
        .val = OPTION_GDB,
        .descrip = "Rerun failed tests in an interactive gdb session",
        .argDescrip = NULL
    },
    {
        .longName = "logger",
        .shortName = 'l',
        .argInfo = POPT_ARG_STRING,
        .arg = NULL,
        .val = OPTION_LOGGER,
        .descrip = "Use a specific result logger (default: console)",
        .argDescrip = "<name>"
    },
    {
        .longName = "option",
        .shortName = 'o',
        .argInfo = POPT_ARG_STRING,
        .arg = NULL,
        .val = OPTION_OPTION,
        .descrip = "Pass an option to a test component (logger or runner)",
        .argDescrip = "<component>.<option name>=<value>"
    },
/*
Not presently implemented
    {
        .longName = "break",
        .shortName = '\0',
        .argInfo = POPT_ARG_STRING,
        .arg = &option_gdb_break,
        .val = 0,
        .descrip = "Specify breakpoint to use for interactive gdb sessions",
        .argDescrip = "< function | line >"
    },
*/
    POPT_AUTOHELP
    POPT_TABLEEND
};

int
Option_Parse(int argc, char** argv, OptionTable* option)
{
    poptContext context = poptGetContext("moonunit", argc, (const char**) argv, options, 0);
    int rc;

    if ((rc = poptReadDefaultConfig(context, 0)))
    {
        rc = error(option, "%s: %s", 
                   poptBadOption(context, 0),
                   poptStrerror(rc));
    } 
    else do
    {
        switch ((rc = poptGetNextOpt(context)))
        {
        case OPTION_SUITE:
        case OPTION_TEST:
        {
            const char* entry = poptGetOptArg(context);

            if (rc == OPTION_SUITE && strchr(entry, '/'))
            {
                rc = error(option, "The --suite option requires an argument without a forward slash");
                break;
            }
            
            if (rc == OPTION_TEST && !strchr(entry, '/'))
            {
                rc = error(option, "The --test option requires an argument with a forward slash");
                break;
            }

            StringSet_Append(&option->tests, entry);
            break;
        }
        case OPTION_ALL:
            option->all = true;
            break;
        case OPTION_GDB:
            option->gdb = true;
            break;
        case OPTION_LOGGER:
            option->logger = strdup(poptGetOptArg(context));
            break;
        case OPTION_OPTION:
        {
            const char* opt = poptGetOptArg(context);
            
            char* dot = strchr(opt, '.');

            if (!dot)
            {
                rc = error(option, "The argument to -o must be of the form component.key=value");
                break;
            }

            *dot = '\0';

            if (!strcmp(opt, "logger"))
            {
                StringSet_Append(&option->logger_options, dot+1);
            }
            else
            {
                rc = error(option, "Unknown component: %s", opt);               
            }

            *dot = '.';
            break;
        }
        case -1:
        {
            const char* file;

            while ((file = poptGetArg(context)))
            {
                StringSet_Append(&option->files, file);
            }
            break;
        }
        case 0:
            break;
        default:
            rc = error(option, "%s: %s",
                       poptBadOption(context, 0),
                       poptStrerror(rc));
        }
    } while (rc > 0);
        
    if (rc == -1 && !option->files.size)
    {
        poptPrintUsage(context, stderr, 0);
        rc = error(option, "Please specify one or more library files");
    }

    poptFreeContext(context);

	return rc != -1;
}

int 
Option_ApplyToLogger(OptionTable* option, struct MoonUnitLogger* logger)
{
    unsigned int index;

    for (index = 0; index < option->logger_options.size; index++)
    {
        char* opt = option->logger_options.value[index];
        char* eq = strchr(opt, '=');

        if (!eq)
            return error(option, "Arguments to --option must be of the form component.key=value");

        *eq = '\0';

        Mu_Logger_SetOptionString(logger, opt, eq+1);
    }

    return 0;
}
