#include "ugly_getopt.h"

struct some
{
    some(ugly::getopt::parser& ugly)
    {
        ugly.add_flag("some", &flag_some)
            .set_option_description("Some flag")
            .set_value_description("flag");

        using std::placeholders::_1;
        ugly.add_option("action1", 'a', ugly::getopt::ArgumentType::Optional)
            .set_handler(std::bind(&some::config_handler_1, this, _1))
            .set_option_description("Action 1")
            .set_value_description("action_1");
        ugly.add_option("action2", ugly::getopt::ArgumentType::Optional)
            .set_handler(std::bind(&some::config_handler_2, this, _1))
            .set_option_description("Action 2")
            .set_value_description("action_2");
        ugly.add_option("action3", ugly::getopt::ArgumentType::Required)
            .set_handler(
                [=, this](const char* optarg)
                {
                    if (optarg)
                    {
                        a3 = std::atoi(optarg);
                        printf("Action3 = %d\n", a3);
                    }
                    else { printf("Action3\n"); }
                })
            .set_option_description("Action 3")
            .set_value_description("action_3");
    }

    int a1 = 0;
    int a2 = 0;
    int a3 = 0;
    int flag_some = 0;

    void config_handler_1(const char* optarg)
    {
        if (optarg)
        {
            a1 = std::atoi(optarg);
            printf("Action1 = %d\n", a1);
        }
        else
            printf("Action1\n");
    }

    void config_handler_2(const char* optarg)
    {
        if (optarg)
        {
            a2 = std::atoi(optarg);
            printf("Action2 = %d\n", a2);
        }
        else
            printf("Action2\n");
    }
};

int main(int argc, char** argv)
{
    ugly::getopt::parser ugly;
    ugly::getopt::add_help_option(&ugly);

    some s(ugly);

    ugly.configure(argc, argv);
    printf("a1 = %d\n", s.a1);
    printf("a2 = %d\n", s.a2);
    printf("a3 = %d\n", s.a3);
}
