# Ugly Getopt

This is a simple and not so pretty c++ overlay for GNU GetOpt (https://www.gnu.org/software/libc/manual/html_node/Getopt.html)

## Usage

See example for details.

```c++
int main(int argc, char ** argv) {
    ugly_getopt ugly;	// step 1

    int flag_some = 0;

    ugly.add_option("some", no_argument, &flag_some, 1, "Some flag");	// step 2
    ugly.add_option("action1", optional_argument, 0, 10, "Action 1");
    ugly.add_option("action2", optional_argument, 0, 20, "Action 2");
    ugly.add_option("action3", required_argument, 0, 30, "Action 3");

    ugly.add_options_handler(std::bind(&some::config_handler, this, std::placeholders::_1, std::placeholders::_2)); // step 3
    ugly.configure(argc, argv); // step 4
}

bool config_handler(int code, const char* optarg) {
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
```
#### Result
```bash
$ example/basic_usage --action1=123 --action2=abc --action3 789
Action1 = 123
Action2 = 0
Argument 30 not handled
```
### Steps
1. Create an `ugly` object:
1. Add getopt-like options using getopt structure:
1. Create function to handle the options:
1. Call `configure` function.
