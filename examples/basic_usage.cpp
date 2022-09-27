#include "ugly_getopt.h"

struct some
{
    some(ugly_getopt& ugly)
    {
        ugly.add_option("some", no_argument, &flag_some, 1, "Some flag");
        ugly.add_option("action1", optional_argument, 0, 10, "Action 1");
        ugly.add_option("action2", optional_argument, 0, 20, "Action 2");
        ugly.add_option("action3", required_argument, 0, 30, "Action 3");

        ugly.add_options_handler(
            std::bind(&some::config_handler, this, std::placeholders::_1, std::placeholders::_2));
    }

    int flag_some = 0;

    bool config_handler(int code, const char* optarg)
    {
        switch (code)
        {
            case 10:
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
                return false;
                break;
        }

        return true;
    }
};

int main(int argc, char** argv)
{

    ugly_getopt ugly;

    some s(ugly);

    ugly.configure(argc, argv);
}
