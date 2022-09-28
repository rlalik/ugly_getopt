#include "ugly_getopt.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>

void ugly_getopt::add_option(option opts, std::string opt_desc, std::string val_desc)
{
    cmdl_options.push_back({opts, std::move(val_desc), std::move(opt_desc)});
}

void ugly_getopt::add_option(std::string name, int has_arg, int* flag, int value,
                             std::string opt_desc, std::string val_desc)
{
    std::unique_ptr<char[]> name_copy(new char[name.length() + 1]);
    std::strncpy(name_copy.get(), name.c_str(), name.length() + 1);

    cmdl_options.push_back(
        {{name_copy.get(), has_arg, flag, value}, std::move(val_desc), std::move(opt_desc)});

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
    std::vector<option> tmp_options;
    std::stringstream short_opts;

    for (auto& cmd : cmdl_options)
    {
        tmp_options.push_back(cmd.opt);
        if (cmd.opt.val > 0 and cmd.opt.val < std::numeric_limits<char>::max())
        {
            short_opts << static_cast<char>(cmd.opt.val);

            if (cmd.opt.has_arg == optional_argument or cmd.opt.has_arg == required_argument)
                short_opts << ':';
        }
    }
    tmp_options.push_back({0, 0, 0, 0});

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
            if (long_options[option_index].flag != 0) continue;
            printf("option %s", long_options[option_index].name);
            if (optarg) printf(" with arg %s", optarg);
            printf("\n");
            continue;
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

template <class T> struct sort_options
{
    bool operator()(T& s1, T& s2) const { return s1.long_name < s2.long_name; }
};

void ugly_getopt::usage(int argc, char** argv)
{
    auto tmp_options = cmdl_options;

    struct full_desc
    {
        int short_name;
        std::string long_name;
        std::string val_desc;
        std::string desc;
        int flag;
    };

    std::vector<full_desc> names;

    size_t max_width = 0;

    for (auto& cmd : cmdl_options)
    {
        int has_flags = 0x00;
        size_t len = std::strlen(cmd.opt.name);
        size_t val_desc_len = 0;

        if (std::isalpha(cmd.opt.val)) { has_flags |= 0x01; }

        if (cmd.opt.has_arg != no_argument)
        {
            val_desc_len = cmd.val_desc.length() + 2; // include '[]'
            has_flags |= 0x02;
            if (cmd.opt.has_arg == optional_argument) has_flags |= 0x10;
        }

        switch (has_flags & 0xf)
        {
            case 0x00: // --XXX
                len += 2;
                break;
            case 0x01: // -X, --XXX
                len += 6;
                break;
            case 0x02: // --XXX=[...]
                len += 3 + val_desc_len;
                break;
            case 0x03: // -X [...], --XXX=[...]
                len += 8 + val_desc_len * 2;
                break;
            default:
                break;
        }
        max_width = std::max(max_width, len);

        if (std::isalpha(cmd.opt.val))
            names.push_back({cmd.opt.val, cmd.opt.name, cmd.val_desc, cmd.opt_desc, has_flags});
        else
            names.push_back({0, cmd.opt.name, cmd.val_desc, cmd.opt_desc, has_flags});
    }

    sort_options<full_desc> so;
    std::sort(names.begin(), names.end(), so);

    std::cout << "Usage:  " << argv[0] << " [options] [arguments]\n where options are:\n\n";

    char buff[max_width + 2];

    for (auto& name : names)
    {
        switch (name.flag & 0xf)
        {
            case 0x00:
                sprintf(buff, "--%s", name.long_name.c_str());
                break;
            case 0x01:
                sprintf(buff, "-%c, --%s", name.short_name, name.long_name.c_str());
                break;
            case 0x02:
                sprintf(buff, "--%s%s%s]", name.long_name.c_str(), name.flag & 0x10 ? "[=" : "=[",
                        name.val_desc.c_str());
                break;
            case 0x03:
                sprintf(buff, "-%c [%s], --%s%s%s]", name.short_name, name.val_desc.c_str(),
                        name.long_name.c_str(), name.flag & 0x10 ? "[=" : "=[",
                        name.val_desc.c_str());
                break;
            default:
                break;
        }

        std::cout << "    " << std::setw(max_width + 2) << std::left << buff << ": " << name.desc
                  << '\n';
    }
}
