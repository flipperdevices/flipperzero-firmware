# MCP251XFD
MCP251XFD is a fully hardware independant **Driver** primarily aimed at embedded world

# Presentation
This driver only takes care of configuration and check of the internal registers and the formatting of the communication with the device. That means it does not directly take care of the physical communication, there are functions interfaces to do that. By doing this, the driver can control a MCP2517FD through a I2C to SPI converter without any change, the transformation will be done in the interface functions.
Each driver's functions need a device structure that indicate with which device it must threat and communicate. Each device can have its own configuration.

## Feature

This driver has been designed to:
* Be fully configurable (all features of the MCP2517FD, MCP2518FD and MCP251863 are managed)
* Detect which one of the MCP2517FD or the MCP2518FD (MCP251863 have a MCP2518FD inside) is connected
* Take care of the CRC communication or Safe write if configured for the device
* Manage all Sleep modes
* Have no limit of configuration except the ones imposed by the device
* Manage devices completely independently
* Have an automatic CAN bit time setting
* Prevent all configuration errors

# Usage

## Installation

### Get the sources
Get and add the 4 following files to your project
```
MCP251XFD.c
MCP251XFD.h
Conf_MCP251XFD_Template.h
ErrorsDef.h
```
Copy or rename the file `Conf_MCP251XFD_Template.h` to `Conf_MCP251XFD.h`... Et voila!

## Others directories

### Tests\ directory
The **Tests** folder contains an example of use on the _SAMV71 Xplained Ultra board_
See the `Main_Synchronous.c` for the hardware setup.

### Tests\UnitTest\ directory
_The **Tests\UnitTest** folder contains unit test for Visual Studio 2017 and are not required._

# Configuration
To set up one or more devices in the project, you must:
* Configure the driver (`Conf_MCP251XFD.h`) which will be the same for all devices but modify only its behavior in the project
* Create and define the configuration of as many device structures as there are devices to use
* Create and define controller and CAN controller configuration for each device. Multiple devices can share the same configuration
* Initialize the device with the configuration structure previously defined

**All is explained in the MCP251XFD driver library guide**

# Driver usage

**All is explained in the MCP251XFD driver library guide**