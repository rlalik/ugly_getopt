#include "ugly_getopt.h"

struct some
{
    int a3;

    some(ugly_getopt& ugly)
    {
        ugly.add_option("some", no_argument, &flag_some, 1, {}, "Some flag", "flag");

        using std::placeholders::_1;
        using std::placeholders::_2;
        ugly.add_option("action1", optional_argument, 0, 'a')
            .set_handler(std::bind(&some::config_handler, this, _1, _2))
            .set_option_description("Action 1")
            .set_value_description("action");
        ugly.add_option("action2", optional_argument, 0, 20)
            .set_handler(std::bind(&some::config_handler, this, _1, _2))
            .set_option_description("Action 2");
        ugly.add_option("action3", required_argument, 0, 30)
            .set_handler(
                [=](int code, const char* optarg)
                {
                    if (optarg)
                    {
                        a3 = std::atoi(optarg);
                        printf("Action3 = %d\n", a3);
                    }
                    else { printf("Action3\n"); }
                })
            .set_option_description("Action 3")
            .set_value_description("action");
    }

    int flag_some = 0;

    void config_handler(int code, const char* optarg)
    {
        switch (code)
        {
            case 'a':
                if (optarg)
                    printf("Action1 = %d\n", std::atoi(optarg));
                else
                    printf("Action1\n");
                break;
            case 20:
                if (optarg)
                    printf("Action2 = %d\n", std::atoi(optarg));
                else
                    printf("Action2\n");
                break;
            default:
                break;
        }
    }
};

int main(int argc, char** argv)
{
    ugly_getopt ugly;

    some s(ugly);

    ugly.configure(argc, argv);
    printf("a3 = %d\n", s.a3);
    ugly.usage(argc, argv);
}
