#include "gmock/gmock.h"
#include "wavesim/attribute.h"

#define NAME attribute

using namespace ::testing;

TEST(NAME, construct_default)
{
    attribute_t a = attribute_default();
    EXPECT_THAT(a.reflection, DoubleEq(0.0));
    EXPECT_THAT(a.transmission, DoubleEq(0.0));
    EXPECT_THAT(a.absorption, DoubleEq(1.0));
}

TEST(NAME, set_default)
{
    attribute_t a = attribute(5, 5, 5);
    attribute_set_default(&a);
    EXPECT_THAT(a.reflection, DoubleEq(0.0));
    EXPECT_THAT(a.transmission, DoubleEq(0.0));
    EXPECT_THAT(a.absorption, DoubleEq(1.0));
}

TEST(NAME, construct_with_values)
{
    attribute_t a = attribute(1, 2, 3);
    EXPECT_THAT(a.reflection, DoubleEq(1.0));
    EXPECT_THAT(a.transmission, DoubleEq(2.0));
    EXPECT_THAT(a.absorption, DoubleEq(3.0));
}

TEST(NAME, set_zero)
{
    attribute_t a = attribute(1, 2, 3);
    attribute_set_zero(&a);
    EXPECT_THAT(a.reflection, DoubleEq(0.0));
    EXPECT_THAT(a.transmission, DoubleEq(0.0));
    EXPECT_THAT(a.absorption, DoubleEq(0.0));
}

TEST(NAME, equality)
{
    attribute_t a = attribute(1, 2, 3);
    attribute_t b = attribute(1, 2, 3);
    attribute_t c = attribute(3, 2, 3);
    EXPECT_THAT(attribute_is_same(&a, &b), Eq(true));
    EXPECT_THAT(attribute_is_same(&b, &a), Eq(true));
    EXPECT_THAT(attribute_is_same(&a, &c), Eq(false));
    EXPECT_THAT(attribute_is_same(&b, &c), Eq(false));
}

TEST(NAME, normalize_zeros_yields_default_attribute)
{
    attribute_t a = attribute(0, 0, 0);
    attribute_normalize_rta(&a);
    EXPECT_THAT(a.reflection, DoubleEq(0.0));
    EXPECT_THAT(a.transmission, DoubleEq(0.0));
    EXPECT_THAT(a.absorption, DoubleEq(1.0));
}

TEST(NAME, normalize_one_negative)
{
    attribute_t a = attribute(-1, 1, 1);
    attribute_normalize_rta(&a);
    EXPECT_THAT(a.reflection + a.transmission + a.absorption, DoubleEq(1.0));
    EXPECT_THAT(a.reflection, Ge(0.0));
    EXPECT_THAT(a.transmission, Ge(0.0));
    EXPECT_THAT(a.absorption, Ge(0.0));
    EXPECT_THAT(a.reflection, Le(1.0));
    EXPECT_THAT(a.transmission, Le(1.0));
    EXPECT_THAT(a.absorption, Le(1.0));
}

TEST(NAME, normalize_two_negatives)
{
    attribute_t a = attribute(-1, -1, 1);
    attribute_normalize_rta(&a);
    EXPECT_THAT(a.reflection + a.transmission + a.absorption, DoubleEq(1.0));
    EXPECT_THAT(a.reflection, Ge(0.0));
    EXPECT_THAT(a.transmission, Ge(0.0));
    EXPECT_THAT(a.absorption, Ge(0.0));
    EXPECT_THAT(a.reflection, Le(1.0));
    EXPECT_THAT(a.transmission, Le(1.0));
    EXPECT_THAT(a.absorption, Le(1.0));
}
