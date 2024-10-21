let tests = require("tests");

tests.assert_eq("compatible", sdkCompatibilityStatus(0, 1));
tests.assert_eq("firmwareTooOld", sdkCompatibilityStatus(1, 0));
tests.assert_eq("firmwareTooNew", sdkCompatibilityStatus(-1, 0));
tests.assert_eq("incompatibleVendor", sdkCompatibilityStatus(0, 1, "Abobus Devices"));

tests.assert_eq(1337, 1337);
tests.assert_eq("hello", "hello");
