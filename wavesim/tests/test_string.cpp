#include "gmock/gmock.h"
#include "wavesim/string.h"

#define NAME string

using namespace ::testing;

TEST(NAME, tokenize_4_parts)
{
    char* saveptr;
    char str[] = "this  is a    test";
    EXPECT_THAT(ws_strtok(str, " ", &saveptr), StrEq("this"));
    EXPECT_THAT(ws_strtok(NULL, " ", &saveptr), StrEq("is"));
    EXPECT_THAT(ws_strtok(NULL, " ", &saveptr), StrEq("a"));
    EXPECT_THAT(ws_strtok(NULL, " ", &saveptr), StrEq("test"));
    EXPECT_THAT(ws_strtok(NULL, " ", &saveptr), IsNull());
    EXPECT_THAT(ws_strtok(NULL, " ", &saveptr), IsNull());
    EXPECT_THAT(ws_strtok(NULL, " ", &saveptr), IsNull());
}

TEST(NAME, tokenize_string_with_no_delimiters)
{
    char* saveptr;
    char str[] = "thisisatest";
    EXPECT_THAT(ws_strtok(str, " ", &saveptr), StrEq("thisisatest"));
    EXPECT_THAT(ws_strtok(NULL, " ", &saveptr), IsNull());
    EXPECT_THAT(ws_strtok(NULL, " ", &saveptr), IsNull());
}

TEST(NAME, tokenize_empty_string)
{
    char* saveptr;
    char str[] = "";
    EXPECT_THAT(ws_strtok(str, " ", &saveptr), IsNull());
    EXPECT_THAT(ws_strtok(NULL, " ", &saveptr), IsNull());
    EXPECT_THAT(ws_strtok(NULL, " ", &saveptr), IsNull());
}

TEST(NAME, tokenize_string_with_only_tokens)
{    char* saveptr;
    char str[] = "             ";
    EXPECT_THAT(ws_strtok(str, " ", &saveptr), IsNull());
    EXPECT_THAT(ws_strtok(NULL, " ", &saveptr), IsNull());
    EXPECT_THAT(ws_strtok(NULL, " ", &saveptr), IsNull());
}
