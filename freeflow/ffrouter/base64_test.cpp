#include <b64.h>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <stddef.h>

// https://www.base64encode.org/

TEST(Base64, Encoding)
{
    std::string str = "Microsoft";
    char *enc       = b64_encode((const unsigned char *)str.c_str(), str.length());

    str = enc;
    ASSERT_EQ(str, "TWljcm9zb2Z0");

    unsigned char *dec = b64_decode(enc, strlen(enc));
    str                = (char *)dec;
    ASSERT_EQ(str, "Microsoft");

    free(enc);
    free(dec);
}
