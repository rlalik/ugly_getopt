#include <gtest/gtest.h>

#include "ugly_getopt.h"

#include <memory> // for allocator, make_unique
#include <string> // for operator+, string

class tests_input_params : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

int code = 0;
int result = 0;

void handler_function(int c, const char* optarg)
{
    code = c;
    if (optarg) result = atoi(optarg);
}

TEST_F(tests_input_params, handler_function)
{
    ugly_getopt ugly;
    ugly.add_option("action", optional_argument, 0, 'a', handler_function, "Action", "action");
    ugly.add_option("bction", required_argument, 0, 'b', handler_function, "Bction", "bction");

    optind = 0;
    code = 0;
    result = 0;
    {
        char* const argv[] = {"test_app", "--action=123", "123"};
        ugly.configure(3, argv);
        ASSERT_EQ(code, 'a');
        ASSERT_EQ(result, 123);
    }

    optind = 0;
    code = 0;
    result = 0;
    {
        char* const argv[] = {"test_app", "-a123"};
        ugly.configure(3, argv);
        ASSERT_EQ(code, 'a');
        ASSERT_EQ(result, 123);
    }

    optind = 0;
    code = 0;
    result = 0;
    {
        char* const argv[] = {"test_app", "-b", "321"};
        ugly.configure(3, argv);
        ASSERT_EQ(code, 'b');
        ASSERT_EQ(result, 321);
    }
}
