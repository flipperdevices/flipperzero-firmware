# SubGhz Flipper File Formats

## SubGhz Key File (standard preset)

### Example

	Filetype: Flipper SubGhz Key File
	Version: 1
	Frequency: 433920000
	Preset: FuriHalSubGhzPresetOok650Async
	Protocol: Princeton
	Bit: 24
	Key: 00 00 00 00 00 95 D5 D4
	TE: 400

### Description

Mandatory Fields:

- `Filetype` - Filetype of subghz file format, always `Flipper SubGhz Key File`
- `Version` - Version of subghz file format, current version is 1
- `Frequency` - Frequency in Hertz
- `Preset` - Radio preset name (configures modulation, bandwidth, filters and etc...)
- `Protocol` - Protocol name, check protocol registry for full list of protocol names

Optional Fields(Princeton example):

- `Bit: 24` - Princeton payload length
- `Key: 00 00 00 00 00 95 D5 D4` - Princeton payload data
- `TE: 400` - Princeton quantization interval

This file may contain additional fields, more details on used fields can be found in subghz protocols library.

## SubGhz Key File (custom preset)

### Example

Mandatory Fields:

	Filetype: Flipper SubGhz Key File
	Version: 1
	Frequency: 433920000
	Preset: FuriHalSubGhzPresetCustom
	Custom_preset_module: CC1101
	Custom_preset_data: 02 0D 03 07 08 32 0B 06 14 00 13 00 12 30 11 32 10 17 18 18 19 18 1D 91 1C 00 1B 07 20 FB 22 11 21 B6 00 00 00 C0 00 00 00 00 00 00
	Protocol: Princeton
	Bit: 24
	Key: 00 00 00 00 00 95 D5 D4
	TE: 400

### Description

- `Filetype` - Filetype of subghz file format, always `Flipper SubGhz Key File`
- `Version` - Version of subghz file format, current version is 1
- `Frequency` - Frequency in Hertz
- `Preset` - Radio preset name (configures modulation, bandwidth, filters and etc...), in this specific case always `FuriHalSubGhzPresetCustom`
- `Custom_preset_module` - transceiver identifier, `CC1101` for Flipper Zero
- `Custom_preset_data` - transceiver configuration data. For CC1101 data structure: XX YY XX YY .. 00 00 ZZ ZZ ZZ ZZ ZZ ZZ ZZ ZZ, where XX - register address, YY - register values. 00 00 - register block end. ZZ ZZ ZZ ZZ ZZ ZZ ZZ ZZ - 8 byte PA table (Power amplifier ramp table). Detailed information can be found in CC1101 datasheet and furi_hal_subghz code.
- `Protocol` - Protocol name, check protocol registry for full list of protocol names

Optional Fields(Princeton example):

- `Bit: 24` - Princeton payload length
- `Key: 00 00 00 00 00 95 D5 D4` - Princeton payload data
- `TE: 400` - Princeton quantization interval

## SubGhz RAW File (standard preset)

### Example

	Filetype: Flipper SubGhz RAW File
	Version: 1
	Frequency: 433920000
	Preset: FuriHalSubGhzPresetOok650Async
	Protocol: RAW
	RAW_Data: 29262 361 -68 2635 -66 24113 -66 11 ...
	RAW_Data: -424 205 -412 159 -412 381 -240 181 ...
	RAW_Data: -1448 361 -17056 131 -134 233 -1462 131 -166 953 -100 ...

### Description

- `Filetype` - Filetype of subghz file format, always `Flipper SubGhz Key File`
- `Version` - Version of subghz file format, current version is 1
- `Frequency` - Frequency in Hertz
- `Preset` - Radio preset name (configures modulation, bandwidth, filters and etc...)
- `Protocol` - Protocol name, in this case `RAW`
- `RAW_Data` - duration array. In micro seconds. Must be non-zero, start with positive number, change sign(interleave). Up to 512 values per line. Can be used multiple times.

Long payload that doesn't fit into internal buffer and consisting of short duration timings (<10us) may not read fast enough from SD-Card which may cause signal to stop before end of the payload. Ensure that your SD-Card has good performance.

## SubGhz RAW File  (custom preset)

### Example

	Filetype: Flipper SubGhz RAW File
	Version: 1
	Frequency: 433920000
	Preset: FuriHalSubGhzPresetCustom
	Custom_preset_module: CC1101
	Сustom_preset_data: 02 0D 03 07 08 32 0B 06 14 00 13 00 12 30 11 32 10 17 18 18 19 18 1D 91 1C 00 1B 07 20 FB 22 11 21 B6 00 00 00 C0 00 00 00 00 00 00
	Protocol: RAW
	RAW_Data: 29262 361 -68 2635 -66 24113 -66 11 ...
	RAW_Data: -424 205 -412 159 -412 381 -240 181 ...
	RAW_Data: -1448 361 -17056 131 -134 233 -1462 131 -166 953 -100 ...

### Description

- `Filetype` - Filetype of subghz file format, always `Flipper SubGhz Key File`
- `Version` - Version of subghz file format, current version is 1
- `Frequency` - Frequency in Hertz
- `Preset` - Radio preset name (configures modulation, bandwidth, filters and etc...), in this specific case always `FuriHalSubGhzPresetCustom`
- `Custom_preset_module` - transceiver identifier, `CC1101` for Flipper Zero
- `Custom_preset_data` - transceiver configuration data. For CC1101 data structure: XX YY XX YY .. 00 00 ZZ ZZ ZZ ZZ ZZ ZZ ZZ ZZ, where XX - register address, YY - register values. 00 00 - register block end. ZZ ZZ ZZ ZZ ZZ ZZ ZZ ZZ - 8 byte PA table (Power amplifier ramp table). Detailed information can be found in CC1101 datasheet and furi_hal_subghz code.
- `Protocol` - Protocol name, in this case `RAW`
- `RAW_Data` - duration array. In micro seconds. Must be non-zero, start with positive number, change sign(interleave). Up to 512 values per line. Can be used multiple times.

Long payload that doesn't fit into internal buffer and consisting of short duration timings (<10us) may not read fast enough from SD-Card which may cause signal to stop before end of the payload. Also ensure that your SD-Card has good performance.

# SubGhz Setting File

## SubGhz keeloq_mfcodes_user file, path \ext\subghz\assets\keeloq_mfcodes_user

### Example

	# to use manual settings and prevent them from being deleted on upgrade, rename *_user.example files to *_user
	# for adding manufacture keys
	# AABBCCDDEEFFAABB:X:NAME
	# AABBCCDDEEFFAABB - man 64 bit
	# X - encryption method:
	# - 0 - iterates over both previous and man in direct and reverse byte sequence
	# - 1 - Simple Learning
	# - 2 - Normal_Learning
	# - 3 - Secure_Learning
	# - 4 - Magic_xor_type1 Learning
	# 
	# NAME - name (string without spaces) max 64 characters long
	Filetype: Flipper SubGhz Keystore File
	Version: 0
	Encryption: 0
	AABBCCDDEEFFAABB:1:Test1
	AABBCCDDEEFFAABB:1:Test2

### Description

- `Filetype` - SubGhz Keystore file format, always `Flipper SubGhz Keystore File`
- `Version` - file format version, 0
- `Encryption` - file encryption: 0 - disable
- list of user manufacture keys. one line - one key. Format as described in header. More information can be found in keeloq decoder. This file will be loaded on subghz application start.

## SubGhz setting_user file, path \ext\subghz\assets\setting_user

### Example

	# to use manual settings and prevent them from being deleted on upgrade, rename *_user.example files to *_user
	Filetype: Flipper SubGhz Setting File
	Version: 1
	# Add Standard frequencies for your region
	Add_standard_frequencies: true
	# Default Frequency: used as default for "Read" and "Read Raw"
	Default_frequency: 433920000
	# Frequencies used for "Read", "Read Raw" and "Frequency Analyzer"
	Frequency: 300000000
	Frequency: 310000000
	Frequency: 320000000
	# Frequencies used for hopping mode (keep this list small or flipper will miss signal)
	Hopper_frequency: 300000000
	Hopper_frequency: 310000000
	Hopper_frequency: 310000000
	# Custom preset
	# format for CC1101 "Custom_preset_data:" XX YY XX YY .. 00 00 ZZ ZZ ZZ ZZ ZZ ZZ ZZ ZZ, where: XX-register, YY - register data, 00 00 - end load register, ZZ - 8 byte Pa table register
	#Custom_preset_name: AM_1
	Custom_preset_module: CC1101
	Custom_preset_data: 02 0D 03 07 08 32 0B 06 14 00 13 00 12 30 11 32 10 17 18 18 19 18 1D 91 1C 00 1B 07 20 FB 22 11 21 B6 00 00 00 C0 00 00 00 00 00 00
	#Custom_preset_name: AM_2
	#Custom_preset_module: CC1101
	#Custom_preset_data: 02 0D 03 07 08 32 0B 06 14 00 13 00 12 30 11 32 10 17 18 18 19 18 1D 91 1C 00 1B 07 20 FB 22 11 21 B6 00 00 00 C0 00 00 00 00 00 00

### Description

- `Add_standard_frequencies` - bool - load default frequencies shipped with firmware. if `false` only frequencies specified in this file will be used.
- `Default_frequency` - int - default frequency used in SubGhz application

Adding more frequencies:

- `Frequency` - int (multi) - add frequency to the subghz application frequency list. Used in Read and Read RAW.

Adding more hopper frequencies:

- `Hopper_frequency` - int (multi) - add frequency to the subghz application hopping frequency list. Used in Frequency Analyzer. Repeating same frequency will cause flipper to listen this frequency more often.

Adding custom preset:

- `Custom_preset_name` - string -preset name that will be shown in SubGHz application 
- `Custom_preset_module` - string - transceiver identifier, CC1101 for Flipper Zero
- `Custom_preset_data` - transceiver configuration data. For CC1101 data structure: XX YY XX YY .. 00 00 ZZ ZZ ZZ ZZ ZZ ZZ ZZ ZZ, where XX - register address, YY - register values. 00 00 - register block end. ZZ ZZ ZZ ZZ ZZ ZZ ZZ ZZ - 8 byte PA table (Power amplifier ramp table). Detailed information can be found in CC1101 datasheet and furi_hal_subghz code.

You can have as many presets as you want, presets are embedded into sub files so another flipper can load them from file directly.