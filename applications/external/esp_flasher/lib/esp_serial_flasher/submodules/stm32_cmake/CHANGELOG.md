# Change log
All notable changes to this project will be documented in this file.
This project adheres to [Semantic Versioning](http://semver.org/).

## v2.1.0 (2021/08/09)

#### Enhancements:

- Add support for U5 family ([PR #249](https://github.com/ObKo/stm32-cmake/pull/249))
- Add support for WB and WL families ([PR #235](https://github.com/ObKo/stm32-cmake/pull/235))
- Add support for L5 family ([PR #172](https://github.com/ObKo/stm32-cmake/pull/172))
- Add support for new devices ([PR #165](https://github.com/ObKo/stm32-cmake/pull/165))
- Add support for CMSIS RTOS ([PR #253](https://github.com/ObKo/stm32-cmake/pull/253))
- Add support for Newlib-Nano ([Issue #179](https://github.com/ObKo/stm32-cmake/issues/179) and [Issue #208](https://github.com/ObKo/stm32-cmake/issues/208) and [Issue #213](https://github.com/ObKo/stm32-cmake/issues/213))
- Allow setting toolchain PATH through env vars ([PR #233](https://github.com/ObKo/stm32-cmake/pull/233))
- Allow setting ST repositories PATH through env vars ([PR #226](https://github.com/ObKo/stm32-cmake/pull/226) and [PR #244](https://github.com/ObKo/stm32-cmake/pull/244))
- Add function `stm32_print_devices_by_family` ([PR #205](https://github.com/ObKo/stm32-cmake/pull/205))
- Generate binary files with `.elf` extension ([PR #163](https://github.com/ObKo/stm32-cmake/pull/163) and [PR #174](https://github.com/ObKo/stm32-cmake/pull/174))
- Add functions to generate `hex` and `bin`files ([PR #221](https://github.com/ObKo/stm32-cmake/pull/221))
- Handle package version on find_package, for HAL and CMSIS ([Issue #177](https://github.com/ObKo/stm32-cmake/issues/177))
- Update ST repositories versions for fetch ([Issue #217](https://github.com/ObKo/stm32-cmake/issues/217) and [PR #255](https://github.com/ObKo/stm32-cmake/pull/255))
- Enhance continuous integration by running GitHub Actions on each PR ([PR #220](https://github.com/ObKo/stm32-cmake/pull/220))
- Add issue template to repository ([Issue #196](https://github.com/ObKo/stm32-cmake/issues/196))
- Clarify documentation for H7 and dual-core MCUs usage ([Issue #155](https://github.com/ObKo/stm32-cmake/issues/155))
- Code refactoring ([PR #161](https://github.com/ObKo/stm32-cmake/pull/161) and [PR #225](https://github.com/ObKo/stm32-cmake/pull/225))

#### Bug Fixes:

- Add dependency on liker script in `stm32_add_linker_script` ([PR #192](https://github.com/ObKo/stm32-cmake/pull/192))
- Fix `find_package(BSP)` ([Issue #198](https://github.com/ObKo/stm32-cmake/issues/198))
- Fix `find_package(FreeRTOS)` ([Issue #150](https://github.com/ObKo/stm32-cmake/issues/150) and [Issue #153](https://github.com/ObKo/stm32-cmake/issues/153) and [PR #202](https://github.com/ObKo/stm32-cmake/pull/202) and [PR #260](https://github.com/ObKo/stm32-cmake/pull/260))
- Check and correct RAM and CCRAM management for all families ([Issue #164](https://github.com/ObKo/stm32-cmake/issues/164))
- Correct minimum CMake version to be 3.16 ([PR #245](https://github.com/ObKo/stm32-cmake/pull/245))
- Correct build flags ([Issue #228](https://github.com/ObKo/stm32-cmake/issues/228))
- Restore function to print binary size at build ([Issue #166](https://github.com/ObKo/stm32-cmake/issues/166) and [PR #207](https://github.com/ObKo/stm32-cmake/pull/207))
- Correct errors in README.md file ([PR #167](https://github.com/ObKo/stm32-cmake/pull/167) and [PR #175](https://github.com/ObKo/stm32-cmake/pull/175) and [PR #210](https://github.com/ObKo/stm32-cmake/pull/210))

---

## v2.0 (2020/11/01)
*Initial realease using moden cmake approach*
