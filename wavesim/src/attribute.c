#include "wavesim/attribute.h"

/* ------------------------------------------------------------------------- */
attribute_t
attribute(WS_REAL reflection, WS_REAL transmission, WS_REAL absorption)
{
    return (attribute_t){reflection, transmission, absorption};
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
