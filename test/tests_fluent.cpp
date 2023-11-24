#include <gtest/gtest.h>

#include "ugly_getopt/ugly_getopt.hpp"

class tests_fluent : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(tests_fluent, handler_function)
{
    int result = 0;

    auto handler = [&](auto optarg)
    {
        if (optarg) result = atoi(optarg);
    };

    int use_cction = 0;
    ugly::getopt::parser ugly;
    ugly::getopt::add_help_option(&ugly, false);

    ugly.add_option("a-action", 'a', ugly::getopt::ArgumentType::Optional).set_handler(handler);
    ugly.add_option("b-action", ugly::getopt::ArgumentType::Required)
        .set_handler(handler)
        .set_option_description("B-Action")
        .set_value_description("b-action");
    ugly.add_flag("c-action", &use_cction)
        .set_option_description("C-action")
        .set_value_description("c-action");
    ugly.add_option("d-action", 'd', ugly::getopt::ArgumentType::None)
        .set_option_description("B-action")
        .set_value_description("b-action");

    optind = 0;
    result = 0;
    {
        char* const argv[] = {"test_app", "--a-action=123", "123"};
        ugly.configure(3, argv);
        ASSERT_EQ(result, 123);
    }

    optind = 0;
    result = 0;
    {
        char* const argv[] = {"test_app", "-a123"};
        ugly.configure(2, argv);
        ASSERT_EQ(result, 123);
    }

    optind = 0;
    result = 0;
    {
        char* const argv[] = {"test_app", "--b-action", "321"};
        ugly.configure(3, argv);
        ASSERT_EQ(result, 321);
    }

    optind = 0;
    result = 0;
    {
        char* const argv[] = {"test_app", "--c-action", "321"};
        ugly.configure(3, argv);
        ASSERT_EQ(result, 0);
        ASSERT_EQ(use_cction, 1);
    }

    optind = 0;
    result = 0;
    {
        char* const argv[] = {"test_app", "--d-action", "321"};
        ugly.configure(3, argv);
        ASSERT_EQ(result, 0);
        ASSERT_EQ(use_cction, 1);
        ugly.process_arguments([](const char* arg) { fmt::print("Extra argument: {}\n", arg); });
    }

    {
        char* const argv[] = {"test_app", "-h"};
        ugly.configure(2, argv);
    }

    ugly.sort_usage_flags(true);

    {
        char* const argv[] = {"test_app", "-h"};
        ugly.configure(2, argv);
    }
}
