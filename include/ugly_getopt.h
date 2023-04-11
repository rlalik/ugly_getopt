#ifndef UGLY_GETOPT_H
#define UGLY_GETOPT_H

#include <algorithm>
#include <bitset>
#include <functional>
#include <map>
#include <sstream>
#include <vector>

#include <fmt/core.h>

namespace ugly::getopt
{
#include <getopt.h>

using option_handler = std::function<void(const char*)>;
using arguments_handler = std::function<void(const char*)>;

enum class ArgumentType
{
    None,
    Required,
    Optional
};

enum class FlagAction
{
    Set,
    Clear
};

struct parser_option
{
    std::string name;
    ArgumentType arg_type;
    int* flag;
    int val;
    option_handler handler; // function to handle the option
    std::string val_desc;   // describe value kind
    std::string opt_desc;   // describe option

    explicit parser_option() = delete;

    explicit parser_option(std::string name, ArgumentType arg_type)
        : name{std::move(name)}, arg_type{arg_type}, flag{nullptr}, val{0}
    {
    }

    explicit parser_option(std::string name, char val, ArgumentType arg_type)
        : name{std::move(name)}, arg_type{arg_type}, flag{nullptr}, val{val}
    {
    }

    explicit parser_option(std::string name, int val, ArgumentType arg_type)
        : name{std::move(name)}, arg_type{arg_type}, flag{nullptr},
          val{std::numeric_limits<char>::max() + 1 + val}
    {
    }

    explicit parser_option(std::string name, int* flag, FlagAction flag_type)
        : name{std::move(name)}, arg_type{ArgumentType::None}, flag{flag},
          val{flag_type == FlagAction::Set ? 1 : 0}
    {
    }

    explicit parser_option(parser_option&&) = default;
    explicit parser_option(const parser_option&) = default;

    /**
     * Set function handlerb being an void fn(int, const char*)
     * \param h function or lambda
     * \return itself
     */
    auto set_handler(option_handler h) -> parser_option&
    {
        handler = std::move(h);
        return *this;
    }

    /**
     * Set value description
     * \param d value description
     */
    auto set_value_description(std::string d) -> parser_option&
    {
        val_desc = std::move(d);
        return *this;
    }

    /**
     * Set option description
     * \param d option description
     */
    auto set_option_description(std::string d) -> parser_option&
    {
        opt_desc = std::move(d);
        return *this;
    }
};

inline auto make_long_option(const parser_option& opt) -> option
{
    return option{opt.name.c_str(),
                  opt.arg_type == ArgumentType::None       ? no_argument
                  : opt.arg_type == ArgumentType::Required ? required_argument
                                                           : optional_argument,
                  opt.flag, opt.val};
}

inline auto make_short_option(const parser_option& opt) -> std::string
{
    if (not opt.flag and opt.val > 0 and opt.val < std::numeric_limits<char>::max())
    {
        if (opt.arg_type == ArgumentType::Required)
            return fmt::format("{}:", static_cast<char>(opt.val));
        else if (opt.arg_type == ArgumentType::Optional)
            return fmt::format("{}::", static_cast<char>(opt.val));
        else
            return fmt::format("{}", static_cast<char>(opt.val));
    }
    return std::string();
}

class parser
{
public:
    explicit parser() = default;
    ~parser() = default;

    /**
     * Add command line option. See getopt long option example.
     * \param name option name
     * \param value option value
     * \param arg_type whether expect additional arguments
     */
    auto add_option(std::string name, char value, ArgumentType arg_type) -> parser_option&
    {
#if __cplusplus >= 201703L
        return cmdl_options.emplace_back(std::move(name), value, arg_type);
#else
        cmdl_options.emplace_back(std::move(name), value, arg_type);
        return cmdl_options.back();
#endif
    }

    auto add_option(std::string name, ArgumentType arg_type) -> parser_option&
    {
#if __cplusplus >= 201703L
        return cmdl_options.emplace_back(std::move(name), long_only_counter++, arg_type);
#else
        cmdl_options.emplace_back(std::move(name), long_only_counter++, arg_type);
        return cmdl_options.back();
#endif
    }

    auto add_option(parser_option&& opt) -> parser_option&
    {
        cmdl_options.emplace_back(std::move(opt));
        return cmdl_options.back();
    }

    auto add_flag(std::string name, int* flag, FlagAction action = FlagAction::Set)
        -> parser_option&
    {
#if __cplusplus >= 201703L
        return cmdl_options.emplace_back(std::move(name), flag, action);
#else
        cmdl_options.emplace_back(std::move(name), flag, action);
        return cmdl_options.back();
#endif
    }

    auto configure(int argc, char* const* argv) -> int
    {
        optind = 0;
        this->argc = argc;
        this->argv = argv;

        std::stringstream short_opts;

        const auto s = cmdl_options.size();
        option long_options[s + 1];

        int cnt = 0;
        std::map<int, const option_handler&> opt_reverse_map;
        for (auto& opt : cmdl_options)
        {
            long_options[cnt++] = make_long_option(opt);
            short_opts << make_short_option(opt);
            if (opt.flag == nullptr and opt.val and opt.handler)
                opt_reverse_map.insert({opt.val, opt.handler});
        }
        long_options[cnt] = {0, 0, 0, 0};

        while (1)
        {
            int option_index = 0;
            int c =
                getopt_long(argc, argv, short_opts.str().c_str(), &long_options[0], &option_index);

            if (c == -1) break;

            if (c == 0)
            {
                /* If this option set a flag, do nothing else now. */
                // if (long_options[option_index].flag != 0) continue;
                // printf("option %s", long_options[option_index].name);
                // if (optarg) printf(" with arg %s", optarg);
                // printf("\n");
                continue;
            }

            auto it = opt_reverse_map.find(c);
            if (it != opt_reverse_map.end()) { it->second(optarg); }
            else
                fmt::print(stderr, "Warning: Option '{}' not handled\n",
                           long_options[option_index].name);
        }

        return optind;
    }

    auto process_arguments(arguments_handler ah) -> void
    {
        while (optind < argc)
            ah(argv[optind++]);
    }

    auto usage() const -> void
    {
        auto tmp_options = cmdl_options;

        struct full_desc
        {
            char short_name;
            std::string long_name;
            std::string val_desc;
            std::string desc;
            ulong flag;
        };

        std::vector<full_desc> names;

        size_t max_width = 0;

        for (auto& opt : cmdl_options)
        {
            std::bitset<3> has_flags;
            size_t val_desc_len = 0;

            if (std::isalpha(opt.val)) { has_flags.set(0); }

            if (opt.arg_type != ArgumentType::None)
            {
                val_desc_len = opt.val_desc.length() + 2; // length includes '{}'
                if (opt.arg_type == ArgumentType::Required) has_flags.set(1);
                if (opt.arg_type == ArgumentType::Optional) has_flags.set(2);
            }

            size_t len = opt.name.size();
            switch (has_flags.to_ulong())
            {
                // no value
                case 0b000: // --XXX
                    len += 2;
                    break;
                case 0b001: // -X, --XXX
                    len += 6;
                    break;
                // required value
                case 0b010: // --XXX[=]{...}
                    len += 5 + val_desc_len;
                    break;
                case 0b011: // -X[ ]{...}, --XXX[=]{...}
                    len += 12 + val_desc_len * 2;
                    break;
                // optional value
                case 0b100: // --XXX[={...}]
                    len += 5 + val_desc_len;
                    break;
                case 0b101: // -X[{...}], --XXX[={...}]
                    len += 11 + val_desc_len * 2;
                    break;
            }
            max_width = std::max(max_width, len);

            if (std::isalpha(opt.val))
                names.push_back({static_cast<char>(opt.val), opt.name, opt.val_desc, opt.opt_desc,
                                 has_flags.to_ulong()});
            else
                names.push_back({0, opt.name, opt.val_desc, opt.opt_desc, has_flags.to_ulong()});
        }

        if (sort_usage)
        {
            std::sort(names.begin(), names.end(),
                      [](const full_desc& s1, const full_desc& s2)
                      { return s1.long_name < s2.long_name; });
        }

        fmt::print("Usage:  {} [options] [arguments]\n where options are:\n\n", argv[0]);

        for (auto& name : names)
        {
            std::string buf;
            switch (name.flag & 0xf)
            {
                // no value
                case 0b000:
                    buf = fmt::format("--{}", name.long_name);
                    break;
                case 0b001:
                    buf = fmt::format("-{}, --{}", name.short_name, name.long_name);
                    break;
                // required value
                case 0b010:
                    buf = fmt::format("--{}[=]{{{}}}", name.long_name, name.val_desc);
                    break;
                case 0b011:
                    buf = fmt::format("-{}[ ]{{{}}}, --{}[=]{{{}}}", name.short_name, name.val_desc,
                                      name.long_name, name.val_desc);
                    break;
                // optional value
                case 0b100:
                    buf = fmt::format("--{}[={{{}}}]", name.long_name, name.val_desc);
                    break;
                case 0b101:
                    buf = fmt::format("-{}[{{{}}}], --{}[={{{}}}]", name.short_name, name.val_desc,
                                      name.long_name, name.val_desc);
                    break;
            }

            fmt::print("    {:<{}}: {}\n", buf, max_width + 2, name.desc);
        }
    }

    auto sort_usage_flags(bool do_sorting) -> void { sort_usage = do_sorting; };

private:
    std::vector<parser_option> cmdl_options;

    int long_only_counter{0};
    bool sort_usage = false;
    int argc{0};
    const char* const* argv{nullptr};
};

inline auto add_help_option(parser* g, bool exit_on_call = true) -> void
{
    g->add_option("help", 'h', ArgumentType::None)
        .set_option_description("Print help")
        .set_handler(
            [=](const char*)
            {
                g->usage();
                if (exit_on_call) std::exit(0);
            });
}
};     // namespace ugly::getopt
#endif /* UGLY_GETOPT_H */
