/*
 * Tests if expected names are generated when M_FLATTEN is used.
 */

#include <stdio.h>
#include "unittests.h"
#include "flatten.pb.h"

int main()
{
    TopLevelMessage msg = {0};
    NestedMessage nmsg = msg.nested;
    NestedLevel2 nmsg2 = nmsg.nested;
    NestedLevel3 nmsg3 = nmsg2.nested;
    nmsg3.nothing = 42;

    msg.short_if_none = ShortIfNone_IfNone_A;
    msg.short_if_strip_package = ShortIfStripPackage_IfPackage_A;
    msg.short_if_flatten = IfFlatten_A;

    return nmsg3.nothing; /* this sets `nmsg3` as used, to prevent warning */
}
