# Ugly Getopt

This is a simple and not so pretty c++ overlay for GNU GetOpt (https://www.gnu.org/software/libc/manual/html_node/Getopt.html)

## Usage

See example for details.

```c++
#include "ugly_getopt/ugly_getopt.h"
#include <fmt/core.h>

void config_handler_1(const char*) { fmt::print("Action1\n"); }

void config_handler_2(const char* optarg)
{
    if (optarg) { fmt::print("Action2 = {}\n", optarg); }
    else { fmt::print("Action2\n"); }
}

int main(int argc, char** argv)
{
    ugly::getopt::parser ugly; // step 1

    int flag_some = 0;

    // step 2 -- add options

    // a flag
    ugly.add_flag("some", &flag_some, ugly::getopt::FlagAction::Set)
        .set_option_description("Set some flag");

    // a short+long option -a, --ation1 w/o value
    ugly.add_option("action1", 'a', ugly::getopt::ArgumentType::None)
        .set_handler(config_handler_1)
        .set_option_description("Action 1");

    // a long-only option --action2 w/ optional value
    ugly.add_option("action2", ugly::getopt::ArgumentType::Optional)
        .set_handler(config_handler_2)
        .set_option_description("Action 2");

    // a long-only option --acion3 w/ required value with lambda capture
    ugly.add_option("action3", ugly::getopt::ArgumentType::Required)
        .set_handler([](const char* optarg) { fmt::print("Action3 = {}\n", optarg); })
        .set_option_description("Action 3");

    ugly::getopt::add_help_option(&ugly);

    ugly.configure(argc, argv); // step 4 -- call configure

    // step 5 -- process remaining arguments
    ugly.process_arguments([&](const char* arg) { fmt::print("Extra argument: {}\n", arg); });
}
```
#### Result
```bash
$ example/example_readme --action1 --action2=abc --action3 789 qwe rty
Action1
Action2 = abc
Action3 = 789
Extra argument: qwe
Extra argument: rty
$ example/basic_usage --help
Usage:  examples/example_readme [options] [arguments]
 where options are:

    --some          : Set some flag
    -a, --action1   : Action 1
    --action2[={}]  : Action 2
    --action3[=]{}  : Action 3
    -h, --help      : Print help

```
### Steps
1. Create an `ugly::getopt::parser` object:
1. Add getopt-like options using getopt-like structure:
1. Create function handler for the options:
1. Call `configure` function.

# Building and installing

See the [BUILDING](BUILDING.md) document.

# Contributing

See the [CONTRIBUTING](CONTRIBUTING.md) document.

# Licensing

Creative Commons Legal Code

See the [LICENSE](LICENSE) document.
