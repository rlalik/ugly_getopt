#include <gtest/gtest.h>

#include "ugly_getopt.h"

using ugly::getopt::ArgumentType;
using ugly::getopt::FlagAction;
using ugly::getopt::make_long_option;
using ugly::getopt::make_short_option;
using ugly::getopt::parser_option;

class tests_parser_option : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(tests_parser_option, options_types)
{
    int dummy_flag_set = 0;
    int dummy_flag_clear = 1;

    ugly::getopt::parser ugly;

    std::vector<std::tuple<std::string, char, ArgumentType>> short_tests = {
        {"action-short-optional", 'a', ArgumentType::Optional},
        {"action-short-required", 'b', ArgumentType::Required},
        {"action-short-none", 'c', ArgumentType::None}};

    std::vector<std::tuple<std::string, ArgumentType>> long_tests = {
        {"action-long-optional", ArgumentType::Optional},
        {"action-long-required", ArgumentType::Required},
        {"action-long-none", ArgumentType::None}};

    std::vector<std::tuple<std::string, int*, FlagAction>> flag_tests = {
        {"action-flag-set", &dummy_flag_set, FlagAction::Set},
        {"action-flag-clear", &dummy_flag_clear, FlagAction::Clear}};

    for (auto& t : short_tests)
    {
        parser_option o(std::get<0>(t), std::get<1>(t), std::get<2>(t));
        ASSERT_EQ(o.name, std::get<0>(t));
        ASSERT_EQ(o.val, std::get<1>(t));
        ASSERT_EQ(o.arg_type, std::get<2>(t));
        ASSERT_EQ(o.flag, nullptr);

        auto lo = make_long_option(o);
        ASSERT_STREQ(lo.name, std::get<0>(t).c_str());
        ASSERT_EQ(lo.val, std::get<1>(t));
        ASSERT_EQ(lo.has_arg, std::get<2>(t) == ArgumentType::None       ? no_argument
                              : std::get<2>(t) == ArgumentType::Optional ? optional_argument
                                                                         : required_argument);
        ASSERT_EQ(lo.flag, nullptr);

        auto so = make_short_option(o);
        switch (std::get<2>(t))
        {
            case ArgumentType::None:
                ASSERT_EQ(so, std::string(1, std::get<1>(t)));
                break;
            case ArgumentType::Optional:
                ASSERT_EQ(so, std::string(1, std::get<1>(t)) + "::");
                break;
            case ArgumentType::Required:
                ASSERT_EQ(so, std::string(1, std::get<1>(t)) + ":");
                break;
        }

        ugly.add_option(std::move(o));
    }

    for (auto& t : long_tests)
    {
        parser_option o(std::get<0>(t), std::get<1>(t));
        ASSERT_EQ(o.name, std::get<0>(t));
        ASSERT_EQ(o.val, 0);
        ASSERT_EQ(o.arg_type, std::get<1>(t));
        ASSERT_EQ(o.flag, nullptr);

        auto lo = make_long_option(o);
        ASSERT_STREQ(lo.name, std::get<0>(t).c_str());
        ASSERT_EQ(lo.val, 0);
        ASSERT_EQ(lo.has_arg, std::get<1>(t) == ArgumentType::None       ? no_argument
                              : std::get<1>(t) == ArgumentType::Optional ? optional_argument
                                                                         : required_argument);
        ASSERT_EQ(lo.flag, nullptr);

        auto so = make_short_option(o);
        ASSERT_EQ(so, "");

        ugly.add_option(std::move(o));
    }

    for (auto& t : flag_tests)
    {
        parser_option o(std::get<0>(t), std::get<1>(t), std::get<2>(t));
        ASSERT_EQ(o.name, std::get<0>(t));
        ASSERT_EQ(o.val, std::get<2>(t) == FlagAction::Set ? 1 : 0);
        ASSERT_EQ(o.arg_type, ArgumentType::None);
        ASSERT_EQ(o.flag, std::get<1>(t));

        auto lo = make_long_option(o);
        ASSERT_STREQ(lo.name, std::get<0>(t).c_str());
        ASSERT_EQ(lo.val, std::get<2>(t) == FlagAction::Set ? 1 : 0);
        ASSERT_EQ(lo.has_arg, no_argument);
        ASSERT_EQ(lo.flag, std::get<1>(t));

        auto so = make_short_option(o);
        ASSERT_EQ(so, "");

        ugly.add_option(std::move(o));
    }
}

TEST_F(tests_parser_option, inputs_parsing)
{
    std::string l1, l2;
    l1.reserve(100);
    l2.reserve(100);
    ugly::getopt::parser ugly;

    std::vector<std::tuple<std::string, char, ArgumentType>> short_tests = {
        {"action-a", 'a', ArgumentType::Optional},
        {"action-b", 'b', ArgumentType::Required},
        {"action-c", 'c', ArgumentType::None}};

    for (auto& t : short_tests)
    {
        parser_option o(std::get<0>(t), std::get<1>(t), std::get<2>(t));
        o.set_handler([&](const char* optarg)
                      { l1 = fmt::format("{} = {}", std::get<0>(t), optarg ? optarg : "(null)"); });
        ugly.add_option(std::move(o));
    }

    std::vector<std::tuple<std::string, ArgumentType>> long_tests = {
        {"action-long-a", ArgumentType::Optional},
        {"action-long-b", ArgumentType::Required},
        {"action-long-c", ArgumentType::None}};

    for (auto& t : long_tests)
    {
        parser_option o(std::get<0>(t), std::get<1>(t));
        o.set_option_description("some desc")
            .set_value_description("some desc")
            .set_handler([](const char*) {});
        ugly.add_option(std::move(o));
    }

    ugly::getopt::add_help_option(&ugly, false);

    // optional
    char* a1[] = {"test_app", "-a123", "456"};           // OK
    char* a2[] = {"test_app", "-a", "123"};              // NOT OK
    char* a3[] = {"test_app", "-a=123", "456"};          // NOT OK
    char* la1[] = {"test_app", "--action-a=123", "456"}; // OK
    char* la2[] = {"test_app", "--action-a", "123"};     // NOT OK
    // required
    char* b1[] = {"test_app", "-b123", "456"};           // OK
    char* b2[] = {"test_app", "-b", "123"};              // OK
    char* b3[] = {"test_app", "-b=123", "456"};          // NOT OK
    char* lb1[] = {"test_app", "--action-b=123", "456"}; // OK
    char* lb2[] = {"test_app", "--action-b", "123"};     // OK
    // none
    char* c1[] = {"test_app", "-c123", "456"};           // NOT OK
    char* c2[] = {"test_app", "-c", "123"};              // OK
    char* c3[] = {"test_app", "-c=123", "456"};          // NOT OK
    char* lc1[] = {"test_app", "--action-c=123", "456"}; // NOT OK
    char* lc2[] = {"test_app", "--action-c", "123"};     // OK

    std::vector<std::tuple<char**, char*, char*>> cmdl_test = {
        {a1, "action-a = 123", "Extra argument: 456"},
        {a2, "action-a = (null)", "Extra argument: 123"},
        {a3, "action-a = =123", "Extra argument: 456"},
        {la1, "action-a = 123", "Extra argument: 456"},
        {la2, "action-a = (null)", "Extra argument: 123"},

        {b1, "action-b = 123", "Extra argument: 456"},
        {b2, "action-b = 123", ""},
        {b3, "action-b = =123", "Extra argument: 456"},
        {lb1, "action-b = 123", "Extra argument: 456"},
        {lb2, "action-b = 123", ""},

        {c1, "action-c = (null)", "Extra argument: 456"},
        {c2, "action-c = (null)", "Extra argument: 123"},
        {c3, "action-c = (null)", "Extra argument: 456"},
        {lc1, "", "Extra argument: 456"},
        {lc2, "action-c = (null)", "Extra argument: 123"},
    };

    char* const help_demo[] = {"help_test", "-h"};
    ugly.configure(2, help_demo);

    for (auto& t : cmdl_test)
    {
        l1.clear();
        l2.clear();
        fmt::print("Input = {} {} {}  Exp output = {:s}  Args = {:s}\n", std::get<0>(t)[0],
                   std::get<0>(t)[1], std::get<0>(t)[2], std::get<1>(t), std::get<2>(t));
        ugly.configure(3, std::get<0>(t));
        ugly.process_arguments([&](const char* arg)
                               { l2 = fmt::format("Extra argument: {}", arg ? arg : "(null)"); });
        ASSERT_EQ(l1, std::get<1>(t));
        ASSERT_EQ(l2, std::get<2>(t));
    }
}
