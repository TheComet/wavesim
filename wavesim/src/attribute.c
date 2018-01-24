#include "wavesim/attribute.h"
#include "wavesim/vec3.h"
#include "string.h"
#include "math.h"

/* ------------------------------------------------------------------------- */
attribute_t
attribute(WS_REAL reflection, WS_REAL transmission, WS_REAL absorption)
{
    attribute_t a;
    a.reflection = reflection;
    a.transmission = transmission;
    a.absorption = absorption;
    return a;
}

/* ------------------------------------------------------------------------- */
attribute_t
attribute_default(void)
{
    attribute_t result;
    attribute_set_default(&result);
    return result;
}

/* ------------------------------------------------------------------------- */
void
attribute_set_default(attribute_t* attribute)
{
    attribute->absorption = 1;
    attribute->reflection = 0;
    attribute->transmission = 0;
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
    WS_REAL sum;

    if (attribute->reflection == 0.0 && attribute->transmission == 0.0 && attribute->absorption == 0.0)
    {
        attribute_set_default(attribute);
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
