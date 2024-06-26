#include "test.h"
#include "config.h"
#include "mongoose.h"

TEST(config_test)
{
    int test_n = 0;
    char *test_info;
    extern configuration config;

    // 1: test config.upload_uri_pattern
    test_info = "uri_pattern";

    if (mg_match(mg_str("/"), mg_str(config.upload_uri_pattern), NULL))
    {
        TEST_FAILED(test_info, ++test_n, "'config.upload_uri_pattern' cannot match index page or will cause critical safety issue");
    }
    else
    {
        TEST_OK(test_info, ++test_n)
    }
}