#ifndef UGLY_GETOPT_H
#define UGLY_GETOPT_H

#include "ugly_getopt/export.h"

#include <functional>

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <getopt.h>

class UGLY_GETOPT_EXPORT ugly_getopt
{
public:
    using option_handler = std::function<void(int, const char*)>;
    using arguments_handler = std::function<bool(const char*)>;
    using multiple_arguments_handler = std::function<bool(int, char* const*, int)>;

    void usage(char** argv);

    /**
     * Add command line option. See getopt long option example.
     * \param name option name
     * \param has_arg whether expect additional arguments
     * \param flag option flag
     * \param val option value
     * \param description description of the option
     */
    void add_option(std::string name, int has_arg, int* flag, int val, option_handler handler,
                    std::string opt_desc = {}, std::string val_desc = "value");
    void add_arguments_handler(arguments_handler f) { cmdl_arguments_handler = f; }
    void add_multiple_arguments_handler(multiple_arguments_handler f) { cmdl_multiple_handler = f; }

    auto configure(int argc, char* const* argv) -> int;
    void usage(int argc, char** argv);

private:
    struct full_option
    {
        std::string name;
        int has_arg;
        int* flag;
        int val;
        option_handler handler; // function to handle the option
        std::string val_desc;   // describe value kind
        std::string opt_desc;   // describe option
    };
    std::vector<std::string> cmdl_names;
    std::map<int, full_option> cmdl_options;

    arguments_handler cmdl_arguments_handler;
    multiple_arguments_handler cmdl_multiple_handler;
};

#endif /* UGLY_GETOPT_H */
