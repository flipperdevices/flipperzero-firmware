# Integration Tests

Manual integration test cases for Flipper Zero firmware, grouped by feature. Each page lists the steps to verify the corresponding functionality.

## How testing is performed

Testing is **scoped to the subsystem that changed** — you don't run every test case for every change.

- **Test what you touched, plus related regressions.** When you change a subsystem, test the new behavior and re-run a few existing cases for that subsystem (using the steps in the pages below) to confirm nothing broke. For example, when adding a new Sub-GHz protocol, test the new protocol and some of the existing ones.
- **Skip unrelated subsystems.** There's no point testing NFC when only Sub-GHz was changed.
- **Test everything for core-wide changes.** If a change affects something global (e.g. in the core), run all the test cases in order.

### When to run

- Before a release candidate.
- Before accepting new pull requests.

## Test cases

- [General](general_test_cases.md)
- [BadUSB](badusb_test_cases.md)
- [CLI](cli_test_cases.md)
- [GoodFAPs](goodfaps_test_cases.md)
- [GPIO](gpio_test_cases.md)
- [iButton](ibutton_test_cases.md)
- [Infrared](infrared_test_cases.md)
- [NFC](nfc_test_cases.md)
- [RFID](rfid_test_cases.md)
- [Sub-GHz](subghz_test_cases.md)
