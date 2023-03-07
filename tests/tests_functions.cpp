#include <gtest/gtest.h>

#include "ugly_getopt.h"

#include <memory> // for allocator, make_unique
#include <string> // for operator+, string

bool operator==(const ugly_getopt::full_option& o1, const ugly_getopt::full_option& o2)
{
    return o1.name == o2.name and o1.val == o2.val and o1.has_arg == o2.has_arg;
}

class tests_functions : public ::testing::Test
{
protected:
    void SetUp() override {}
    void TearDown() override {}
};

static int code = 0;
static int result = 0;

inline void handler_function(int c, const char* optarg)
{
    code = c;
    if (optarg) result = atoi(optarg);
}

TEST_F(tests_functions, add_and_get)
{
    ugly_getopt ugly;
    auto o1 = ugly.add_option("action", optional_argument, 0, 'a');
    auto o2 = ugly.add_option("bction", required_argument, 0, 'b');

    auto g1 = ugly.get_option('a');
    ASSERT_EQ(g1, o1);
    auto g2 = ugly.get_option("bction");
    ASSERT_EQ(g2, o2);
}
