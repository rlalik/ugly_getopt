#include "ugly_getopt/ugly_getopt.h"

void config_handler_1(const char* optarg)
{
    if (optarg)
        printf("Action1 = %d\n", std::atoi(optarg));
    else
        printf("Action1\n");
}

void config_handler_2(const char* optarg)
{
    if (optarg)
        printf("Action2 = %d\n", std::atoi(optarg));
    else
        printf("Action2\n");
}

void config_handler_3(const char* optarg)
{
    if (optarg)
        printf("Action3 = %d\n", std::atoi(optarg));
    else
        printf("Action3\n");
}

int main(int argc, char** argv)
{
    ugly::getopt::parser ugly;
    ugly::getopt::add_help_option(&ugly);

    int flag_some = 0;
    ugly.add_flag("some", &flag_some)
        .set_option_description("Some flag")
        .set_value_description("flag");

    using std::placeholders::_1;
    ugly.add_option("action1", 'a', ugly::getopt::ArgumentType::Optional)
        .set_handler(config_handler_1)
        .set_option_description("Action 1")
        .set_value_description("action");
    ugly.add_option("action2", ugly::getopt::ArgumentType::Optional)
        .set_handler(config_handler_2)
        .set_option_description("Action 2")
        .set_value_description("action");
    ugly.add_option("action3", ugly::getopt::ArgumentType::Required)
        .set_handler(config_handler_3)
        .set_option_description("Action 3")
        .set_value_description("action");

    ugly.configure(argc, argv);
}
