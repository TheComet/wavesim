#include "gmock/gmock.h"
#include "wavesim/mesh/attribute.h"

#define NAME attribute

using namespace ::testing;

TEST(NAME, construct_default_solid)
{
    attribute_t a = attribute_default_solid();
    EXPECT_THAT(a.reflection, DoubleEq(0.0));
    EXPECT_THAT(a.transmission, DoubleEq(0.0));
    EXPECT_THAT(a.absorption, DoubleEq(1.0));
    EXPECT_THAT(a.sound_velocity, DoubleEq(2000));
    EXPECT_THAT(vec3_is_zero(a.velocity.xyz), Eq(true));
}

TEST(NAME, construct_default_air)
{
    attribute_t a = attribute_default_solid();
    EXPECT_THAT(a.reflection, DoubleEq(0.0));
    EXPECT_THAT(a.transmission, DoubleEq(1.0));
    EXPECT_THAT(a.absorption, DoubleEq(0.0));
    EXPECT_THAT(a.sound_velocity, DoubleEq(340));
    EXPECT_THAT(vec3_is_zero(a.velocity.xyz), Eq(true));
}

TEST(NAME, set_default_solid)
{
    attribute_t a;
    attribute_set_default_solid(&a);
    EXPECT_THAT(a.reflection, DoubleEq(0.0));
    EXPECT_THAT(a.transmission, DoubleEq(0.0));
    EXPECT_THAT(a.absorption, DoubleEq(1.0));
    EXPECT_THAT(a.sound_velocity, DoubleEq(2000));
    EXPECT_THAT(vec3_is_zero(a.velocity.xyz), Eq(true));
}

TEST(NAME, set_default_air)
{
    attribute_t a;
    attribute_set_default_solid(&a);
    EXPECT_THAT(a.reflection, DoubleEq(0.0));
    EXPECT_THAT(a.transmission, DoubleEq(1.0));
    EXPECT_THAT(a.absorption, DoubleEq(0.0));
    EXPECT_THAT(a.sound_velocity, DoubleEq(340));
    EXPECT_THAT(vec3_is_zero(a.velocity.xyz), Eq(true));
}

TEST(NAME, construct_with_values)
{
    attribute_t a = attribute(1, 2, 3, 200, vec3(4, 5, 6));
    EXPECT_THAT(a.reflection, DoubleEq(1.0));
    EXPECT_THAT(a.transmission, DoubleEq(2.0));
    EXPECT_THAT(a.absorption, DoubleEq(3.0));
    EXPECT_THAT(a.sound_velocity, DoubleEq(200));
    EXPECT_THAT(a.velocity.v.x, DoubleEq(4));
    EXPECT_THAT(a.velocity.v.y, DoubleEq(5));
    EXPECT_THAT(a.velocity.v.z, DoubleEq(6));
}

TEST(NAME, set_zero)
{
    attribute_t a = attribute(1, 2, 3, 200, vec3(4, 5, 6));
    attribute_set_zero(&a);
    EXPECT_THAT(a.reflection, DoubleEq(0.0));
    EXPECT_THAT(a.transmission, DoubleEq(0.0));
    EXPECT_THAT(a.absorption, DoubleEq(0.0));
}

TEST(NAME, equality)
{
    attribute_t a = attribute(1, 2, 3, 200, vec3(4, 5, 6));
    attribute_t b = attribute(1, 2, 3, 200, vec3(4, 5, 6));
    attribute_t c = attribute(3, 2, 3, 200, vec3(4, 5, 6));
    attribute_t d = attribute(1, 1, 3, 200, vec3(4, 5, 6));
    attribute_t e = attribute(1, 2, 2, 200, vec3(4, 5, 6));
    attribute_t f = attribute(1, 2, 3, 400, vec3(4, 5, 6));
    attribute_t g = attribute(1, 2, 3, 200, vec3(1, 5, 6));
    attribute_t h = attribute(1, 2, 3, 200, vec3(4, 1, 6));
    attribute_t i = attribute(1, 2, 3, 200, vec3(4, 5, 1));
    EXPECT_THAT(attribute_is_same(&a, &b), Eq(true));
    EXPECT_THAT(attribute_is_same(&b, &a), Eq(true));
    EXPECT_THAT(attribute_is_same(&a, &c), Eq(false));
    EXPECT_THAT(attribute_is_same(&a, &d), Eq(false));
    EXPECT_THAT(attribute_is_same(&a, &e), Eq(false));
    EXPECT_THAT(attribute_is_same(&a, &f), Eq(false));
    EXPECT_THAT(attribute_is_same(&a, &g), Eq(false));
    EXPECT_THAT(attribute_is_same(&a, &h), Eq(false));
    EXPECT_THAT(attribute_is_same(&a, &i), Eq(false));
}

TEST(NAME, normalize_zeros_yields_default_attribute)
{
    attribute_t a = attribute(0, 0, 0, 200, vec3(4, 5, 6));
    attribute_normalize_rta(&a);
    EXPECT_THAT(a.reflection, DoubleEq(0.0));
    EXPECT_THAT(a.transmission, DoubleEq(0.0));
    EXPECT_THAT(a.absorption, DoubleEq(1.0));
}

TEST(NAME, normalize_one_negative)
{
    attribute_t a = attribute(-1, 1, 1, 200, vec3(4, 5, 6));
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
    attribute_t a = attribute(-1, -1, 1, 200, vec3(4, 5, 6));
    attribute_normalize_rta(&a);
    EXPECT_THAT(a.reflection + a.transmission + a.absorption, DoubleEq(1.0));
    EXPECT_THAT(a.reflection, Ge(0.0));
    EXPECT_THAT(a.transmission, Ge(0.0));
    EXPECT_THAT(a.absorption, Ge(0.0));
    EXPECT_THAT(a.reflection, Le(1.0));
    EXPECT_THAT(a.transmission, Le(1.0));
    EXPECT_THAT(a.absorption, Le(1.0));
}
