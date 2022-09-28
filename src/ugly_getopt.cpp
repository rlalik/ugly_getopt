#include "ugly_getopt.h"

#include <cstring>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>

void ugly_getopt::add_option(option opts, std::string desc)
{
    cmdl_options.push_back(opts);
    cmdl_options_descriptions.push_back(std::move(desc));
}

void ugly_getopt::add_option(std::string name, int has_arg, int* flag, int value, std::string desc)
{
    std::unique_ptr<char[]> name_copy(new char[name.length() + 1]);
    std::strncpy(name_copy.get(), name.c_str(), name.length() + 1);

    cmdl_options.push_back({name_copy.get(), has_arg, flag, value});
    cmdl_options_descriptions.push_back(std::move(desc));

    cmdl_names.push_back(std::move(name_copy));
}

void ugly_getopt::add_options_handler(std::function<bool(int, const char*)> f)
{
    cmdl_options_handlers.push_back(std::move(f));
}

auto ugly_getopt::add_options_handler(std::function<int(int, char**, int)> f) -> void
{
    if (cmdl_arguments_handler) throw std::runtime_error("Arguments handler already set");

    cmdl_arguments_handler = std::move(f);
}

int ugly_getopt::configure(int argc, char** argv)
{
    auto tmp_options = cmdl_options;
    tmp_options.push_back({0, 0, 0, 0});

    std::stringstream short_opts;

    for (auto& cmd : cmdl_options)
    {
        if (cmd.val > 0 and cmd.val < std::numeric_limits<char>::max())
        {
            short_opts << static_cast<char>(cmd.val);

            if (cmd.has_arg == optional_argument or cmd.has_arg == required_argument)
                short_opts << ':';
        }
    }

    option* long_options = tmp_options.data();

    int c = 0;
    while (1)
    {
        int option_index = 0;

        c = getopt_long(argc, argv, short_opts.str().c_str(), long_options, &option_index);
        if (c == -1) break;

        if (c == 0)
        {
            /* If this option set a flag, do nothing else now. */
            if (long_options[option_index].flag != 0) break;
            printf("option %s", long_options[option_index].name);
            if (optarg) printf(" with arg %s", optarg);
            printf("\n");
            break;
        }

        bool ret = false;
        for (auto f : cmdl_options_handlers)
        {
            ret = f(c, optarg);
            if (ret) break;
        }

        if (!ret)
            std::cerr << "ERROR: Argument '" << long_options[option_index].name
                      << "' not handled\n";
    }

    if (cmdl_arguments_handler) return cmdl_arguments_handler(argc, argv, optind);

    return optind;
}
