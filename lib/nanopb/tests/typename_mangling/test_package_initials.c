/*
 * Tests if expected names are generated when M_STRIP_PACKAGE is used.
 */

#include <stdio.h>
#include "unittests.h"
#include "package_initials.pb.h"

int main()
{
    cen_TopLevelMessage msg = {0};
    cen_TopLevelMessage_NestedMessage_NestedLevel2_NestedLevel3 nmsg = msg.nested.nested.nested;

    msg.short_if_none = cen_TopLevelMessage_ShortIfNone_IfNone_A;
    msg.short_if_strip_package = cen_TopLevelMessage_ShortIfStripPackage_IfPackage_A;
    msg.short_if_flatten = cen_TopLevelMessage_ShortIfFlatten_IfFlatten_A;

    return nmsg.nothing; /* marks nmsg as used */
}
