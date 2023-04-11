#include <gtest/gtest.h>

#include "ugly_getopt.h"

class tests_input_params : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

static int result = 0;

inline void handler_function(const char* optarg)
{
    if (optarg) result = atoi(optarg);
}

TEST_F(tests_input_params, handler_function)
{
    ugly::getopt::parser ugly;
    ugly.add_option("action", 'a', ugly::getopt::ArgumentType::Optional)
        .set_handler(handler_function)
        .set_option_description("A-action")
        .set_value_description("action");
    ugly.add_option("bction", 'b', ugly::getopt::ArgumentType::Required)
        .set_handler(handler_function)
        .set_option_description("B-action")
        .set_value_description("bction");

    result = 0;
    {
        char* const argv[] = {"test_app1", "--action=123", "123"};
        ugly.configure(3, argv);
        ASSERT_EQ(result, 123);
    }

    result = 0;
    {
        char* const argv[] = {"test_app2", "-a123"};
        ugly.configure(2, argv);
        ASSERT_EQ(result, 123);
    }

    result = 0;
    {
        char* const argv[] = {"test_app3", "-b", "321"};
        ugly.configure(3, argv);
        ASSERT_EQ(result, 321);
    }

    result = 0;
    {
        char* const argv[] = {"test_app4", "-b321"};
        ugly.configure(2, argv);
        ASSERT_EQ(result, 321);
    }
}
