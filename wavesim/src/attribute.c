#include "wavesim/attribute.h"
#include "wavesim/vec3.h"
#include "string.h"
#include "math.h"

/* ------------------------------------------------------------------------- */
attribute_t
attribute(wsreal_t reflection, wsreal_t transmission, wsreal_t absorption, wsreal_t sound_velocity, vec3_t velocity)
{
    attribute_t a;
    a.reflection = reflection;
    a.transmission = transmission;
    a.absorption = absorption;
    a.sound_velocity = sound_velocity;
    a.velocity = velocity;
    return a;
}

/* ------------------------------------------------------------------------- */
attribute_t
attribute_default_solid(void)
{
    attribute_t result;
    attribute_set_default_solid(&result);
    return result;
}

/* ------------------------------------------------------------------------- */
attribute_t
attribute_default_air(void)
{
    attribute_t result;
    attribute_set_default_air(&result);
    return result;
}

/* ------------------------------------------------------------------------- */
void
attribute_set_default_solid(attribute_t* attribute)
{
    attribute->absorption = 1;
    attribute->reflection = 0;
    attribute->transmission = 0;
    attribute->sound_velocity = 2000;
    vec3_set_zero(attribute->velocity.xyz);
}

/* ------------------------------------------------------------------------- */
void
attribute_set_default_air(attribute_t* attribute)
{
    attribute->absorption = 0;
    attribute->reflection = 0;
    attribute->transmission = 1;
    attribute->sound_velocity = 340;
    vec3_set_zero(attribute->velocity.xyz);
}

/* ------------------------------------------------------------------------- */
void
attribute_set_zero(attribute_t* attribute)
{
    attribute->absorption = 0;
    attribute->reflection = 0;
    attribute->transmission = 0;
}

/* ------------------------------------------------------------------------- */
int
attribute_is_zero(attribute_t* attribute)
{
    return (attribute->reflection == 0.0 &&
            attribute->transmission == 0.0 &&
            attribute->absorption == 0.0);
}

/* ------------------------------------------------------------------------- */
int
attribute_is_same(const attribute_t* a1, const attribute_t* a2)
{
    return (
        a1->absorption == a2->absorption &&
        a1->reflection == a2->reflection &&
        a1->transmission == a2->transmission
    );
}

/* ------------------------------------------------------------------------- */
void
attribute_normalize_rta(attribute_t* attribute)
{
    wsreal_t sum;

    if (attribute->reflection == 0.0 && attribute->transmission == 0.0 && attribute->absorption == 0.0)
    {
        attribute_set_default_solid(attribute);
        return;
    }

    attribute->reflection = fabs(attribute->reflection);
    attribute->transmission = fabs(attribute->transmission);
    attribute->absorption = fabs(attribute->absorption);
    sum = attribute->reflection + attribute->transmission + attribute->absorption;
    sum = 1.0 / sum;
    attribute->reflection *= sum;
    attribute->transmission *= sum;
    attribute->absorption *= sum;
}
