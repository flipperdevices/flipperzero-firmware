/*
 * Tests if expected names are generated when M_STRIP_PACKAGE is used.
 */

#include <stdio.h>
#include "unittests.h"
#include "strip_package.pb.h"

int main()
{
    TopLevelMessage msg = {0};
    TopLevelMessage_NestedMessage_NestedLevel2_NestedLevel3 nmsg = msg.nested.nested.nested;

    msg.short_if_none = TopLevelMessage_ShortIfNone_IfNone_A;
    msg.short_if_strip_package = TopLevelMessage_IfPackage_A;
    msg.short_if_flatten = TopLevelMessage_ShortIfFlatten_IfFlatten_A;

    return nmsg.nothing; /* marks nmsg as used */
}
