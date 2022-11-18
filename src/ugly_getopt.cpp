#include "ugly_getopt.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>

void ugly_getopt::add_option(std::string name, int has_arg, int* flag, int value,
                             option_handler handler, std::string opt_desc, std::string val_desc)
{
    cmdl_options.insert({value,
                         {std::move(name), has_arg, flag, value, std::move(handler),
                          std::move(val_desc), std::move(opt_desc)}});
}

int ugly_getopt::configure(int argc, char* const* argv)
{
    std::stringstream short_opts;

    const auto s = cmdl_options.size();
    option long_options[s + 1];

    int cnt = 0;
    for (auto& opt : cmdl_options)
    {
        auto& cmd = opt.second;

        long_options[cnt++] = {cmd.name.c_str(), cmd.has_arg, cmd.flag, cmd.val};
        if (cmd.val > 0 and cmd.val < std::numeric_limits<char>::max())
        {
            short_opts << static_cast<char>(cmd.val);

            if (cmd.has_arg == required_argument) short_opts << ':';
            if (cmd.has_arg == optional_argument) short_opts << "::";
        }
    }
    long_options[cnt] = {0, 0, 0, 0};

    int c = 0;
    while (1)
    {
        int option_index = 0;

        c = getopt_long(argc, argv, short_opts.str().c_str(), &long_options[0], &option_index);

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

        auto it = cmdl_options.find(c);
        if (it != cmdl_options.end()) { it->second.handler(c, optarg); }
        else
            std::cerr << "ERROR: Argument '" << long_options[option_index].name
                      << "' not handled\n";
    }

    if (optind < argc and cmdl_multiple_handler)
    {
        return cmdl_multiple_handler(argc, argv, optind);
    }
    if (optind < argc and cmdl_arguments_handler)
    {
        while (optind < argc)
            return cmdl_arguments_handler(argv[optind++]);
    }

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

    for (auto& opt : cmdl_options)
    {
        auto& cmd = opt.second;
        int has_flags = 0x00;
        size_t len = cmd.name.size();
        size_t val_desc_len = 0;

        if (std::isalpha(cmd.val)) { has_flags |= 0x01; }

        if (cmd.has_arg != no_argument)
        {
            val_desc_len = cmd.val_desc.length() + 2; // include '[]'
            has_flags |= 0x02;
            if (cmd.has_arg == optional_argument) has_flags |= 0x10;
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

        if (std::isalpha(cmd.val))
            names.push_back({cmd.val, cmd.name, cmd.val_desc, cmd.opt_desc, has_flags});
        else
            names.push_back({0, cmd.name, cmd.val_desc, cmd.opt_desc, has_flags});
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
