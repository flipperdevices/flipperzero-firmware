import serial.tools.list_ports as list_ports


# Returns a valid port or None, if it cannot be found
def resolve_port(logger, portname: str = "auto"):
    if portname != "auto":
        return portname
    # Try guessing
    flippers = list(list_ports.grep("flip_", include_links=True))
    logger.info(f"Found following ports {list_ports}")
    for port in list_ports.comports():
        if port.vid is not None and port.pid is not None:
            logger.info("Port={},VID={:#06x},PID={:#06x}".format(port.device, port.vid, port.pid))
    if len(flippers) == 1:
        flipper = flippers[0]
        logger.info(f"Using {flipper.serial_number} on {flipper.device}")
        return flipper.device
    elif len(flippers) == 0:
        logger.error("Failed to find connected Flipper")
    elif len(flippers) > 1:
        logger.error("More than one Flipper is attached")
    logger.error("Failed to guess which port to use")
