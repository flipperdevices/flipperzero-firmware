# changelog

This file contains all changelogs for latest releases, from 1.3 onward.

## v1.4

### Fixed
If received signal is less than 500ms from last decoded signal, we ignore it now.  In the future, we can consider checking the "Key" to see if something in the signal changed, but for now, we just ignore it.

In some firmware, the MF fails to parse because it is mising a \n at the end of the file. This is now fixed.

In some firmware, the SN fails to parse because it is mising from keeloq.c; the application will now use Fix data in that case.

## v1.3

### Added
Added this change log file.

### Fixed
In some firmware, the retry count on KeeLoq was 100 transmissions, which is too much. Now it stops transmitting after 1 second (or the end of the transmissions) whichever comes first.
