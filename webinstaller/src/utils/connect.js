
export const connect = async () => {
  let port;
  try {
    port = await navigator.serial.requestPort();
  } catch (err) {
    if (err.name === "NotFoundError") {
      console.log("no-port-picked");
      return;
    }
    alert(`Error: ${err.message}`);
    return;
  }

  if (!port) {
    return;
  }

  try {
    await port.open({ baudRate: 115200 });
  } catch (err) {
    alert(err.message);
    return;
  }

  //   const el = document.createElement("ewt-install-dialog");
  //   el.port = port;
  //   el.manifestPath = button.manifest || button.getAttribute("manifest")!;
  //   el.overrides = button.overrides;
  //   el.addEventListener(
  //     "closed",
  //     () => {
  //       port!.close();
  //     },
  //     { once: true },
  //   );
  //   document.body.appendChild(el);
  return port;
};
