# RFID

The spreadsheet is support for the following videos:
- [T5577 overview](https://youtu.be/X6UJI4h9T5I)
- [T5577 user data](https://youtu.be/5DvIBvXBDX8)
- [Create credentials](https://youtu.be/PLMGmRxhBts)


This [lf-rfid excel file](lf-rfid.xlsx) has 4 tabs.

- EM4100 tab : Enter the HEX data in cell H1, like `1A:01:65:73:D1` and you will see all of the data along with the data needed to program a T5577 chip (block 0-2) will be displayed in cells J22-J24.

- H10301 tab : Enter the HEX data in cell H1, like `65:73:D1` and you will see all of the data along with the data needed to program a T5577 chip (block 0-3) will be displayed in cells J18-J21.

- AWID tab : Enter the HEX data in cell H1, like `1A:B2:B9:E8:C0` and you will see all of the data along with the data needed to program a T5577 chip (block 0-3) will be displayed in cells J22-J25.

- Card number tab : Enter the Facility code in cell B2 and the Card number in cell B3.  The H10301 Hex data for that facility/card will be in cell D22.  The EM4100 Hex data for that facility/card will be in cell D27.  The AWID Hex data for that facility/card will be in cell D35.

- Indala tab : Enter the Facility code inb cell B2 and the Card number in cell B5.  Copy E5-T5 into cell E6-T6.  The Indala26 Hex data for that facility/card will be in cell C24.  ** NOTE: AA18, AG18-AJ18 are currently set to 0, but may need to be set to another number?