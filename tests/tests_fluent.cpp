#include <gtest/gtest.h>

#include "ugly_getopt.h"

#include <memory> // for allocator, make_unique
#include <string> // for operator+, string

class tests_fluent : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(tests_fluent, handler_function)
{
    int code = 0;
    int result = 0;

    auto handler = [&](auto c, auto optarg)
    {
        code = c;
        if (optarg) result = atoi(optarg);
    };

    int use_cction;
    ugly_getopt ugly;
    ugly.add_option("action", optional_argument, 0, 'a').set_handler(handler);
    ugly.add_option("bction", required_argument, 0, 'b')
        .set_handler(handler)
        .set_option_description("Bction")
        .set_value_description("bction");
    ugly.add_option("cction", required_argument, &use_cction, 'b')
        .set_option_description("Cction")
        .set_value_description("cction");

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
