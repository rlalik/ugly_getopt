#ifndef UGLY_GETOPT_H
#define UGLY_GETOPT_H

#include "ugly_getopt/export.h"

#include <functional>

#include <memory>
#include <string>
#include <vector>

#include <getopt.h>

class UGLY_GETOPT_EXPORT ugly_getopt
{
public:
    void usage(char** argv);

    /**
     * Add command line option. See getopt long option example.
     * \param name option name
     * \param has_arg whether expect additional arguments
     * \param flag option flag
     * \param val option value
     * \param description description of the option
     */
    void add_option(std::string name, int has_arg, int* flag, int val, std::string description);
    void add_options_handler(std::function<bool(int, const char*)> f);
    void add_options_handler(std::function<int(int, char**, int)> f);

    int configure(int argc, char** argv);

private:
    size_t s_lo;

    std::vector<std::unique_ptr<char[]>> cmdl_names;
    std::vector<option> cmdl_options;
    std::vector<std::string> cmdl_options_descriptions;
    std::vector<std::function<bool(int, const char*)>> cmdl_options_handlers;
    std::function<int(int, char**, int)> cmdl_arguments_handler;
};

#endif /* UGLY_GETOPT_H */
