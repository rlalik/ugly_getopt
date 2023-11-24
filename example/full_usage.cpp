#include "ugly_getopt/ugly_getopt.hpp"

using ugly::getopt::ArgumentType;
using ugly::getopt::FlagAction;
using ugly::getopt::make_long_option;
using ugly::getopt::make_short_option;
using ugly::getopt::parser_option;

int main(int argc, char** argv)
{
    int dummy_flag_set = 0;
    int dummy_flag_clear = 1;

    ugly::getopt::parser ugly;
    ugly::getopt::add_help_option(&ugly);

    std::vector<std::tuple<std::string, char, ArgumentType>> short_tests = {
        {"action-short-optional", 'a', ArgumentType::Optional},
        {"action-short-required", 'b', ArgumentType::Required},
        {"action-short-none", 'c', ArgumentType::None}};

    std::vector<std::tuple<std::string, ArgumentType>> long_tests = {
        {"action-long-optional", ArgumentType::Optional},
        {"action-long-required", ArgumentType::Required},
        {"action-long-none", ArgumentType::None}};

    std::vector<std::tuple<std::string, int*, FlagAction>> flag_tests = {
        {"action-flag-set", &dummy_flag_set, FlagAction::Set},
        {"action-flag-clear", &dummy_flag_clear, FlagAction::Clear}};

    for (auto& t : short_tests)
    {
        ugly.add_option(std::get<0>(t), std::get<1>(t), std::get<2>(t))
            .set_value_description("value")
            .set_option_description("Value")
            .set_handler(
                [=](const char* optarg)
                {
                    if (optarg) { fmt::print("*{}* = {}\n", std::get<0>(t), optarg); }
                    else { fmt::print("*{}*\n", std::get<0>(t)); }
                });
    }

    for (auto& t : long_tests)
    {
        ugly.add_option(std::get<0>(t), std::get<1>(t))
            .set_value_description("value")
            .set_option_description("Value")
            .set_handler(
                [=](const char* optarg)
                {
                    if (optarg) { fmt::print("*{}* = {}\n", std::get<0>(t), optarg); }
                    else { fmt::print("*{}*\n", std::get<0>(t)); }
                });
    }

    for (auto& t : flag_tests)
    {
        ugly.add_flag(std::get<0>(t), std::get<1>(t), std::get<2>(t))
            .set_value_description("value")
            .set_option_description("Value")
            .set_handler(
                [=](const char* optarg)
                {
                    if (optarg) { fmt::print("*{}* = {}\n", std::get<0>(t), optarg); }
                    else { fmt::print("*{}*\n", std::get<0>(t)); }
                });
    }

    ugly.configure(argc, argv);
    ugly.process_arguments([](const char* arg) { fmt::print("Extra argument: {}\n", arg); });
}
