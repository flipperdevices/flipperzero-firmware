package main

import (
	"flag"
	"log"
	"os"
	"os/user"
	"runtime"
	"u2fhid-host/beashHid"
	"u2fhid-host/messages"
)

func main() {

	log.SetFlags(log.Ldate | log.Ltime | log.Lshortfile)

	sendFlags := flag.NewFlagSet("send", flag.ExitOnError)
	sendPath := sendFlags.String("path", "", "Path to the file that should be send.")

	receiveFlags := flag.NewFlagSet("receive", flag.ExitOnError)
	saveDir := receiveFlags.String("outPath", "./", "Directory or filepath where to save the incoming file.")

	decompressFlags := flag.NewFlagSet("decompress", flag.ExitOnError)
	inPath := decompressFlags.String("in", "", "File received by the flipper that should be decompressed.")
	outPath := decompressFlags.String("out", "", "Directory or filepath where the decompressed file "+
		"should be stored. (defaults to the same folder as -in)")

	if len(os.Args) < 2 {
		log.Fatal("Subcommand is missing. Valid commands: send, receive, decompress, debugSend, debugReceive")
	}

	if runtime.GOOS == "linux" {
		u, _ := user.Current()
		if u.Uid != "0" && os.Args[1] != "decompress" {
			log.Fatal("Needs root privileges on linux")
		}
	}

	manager := beashHid.NewHIDManger()

	switch os.Args[1] {
	case "send":
		sendFlags.Parse(os.Args[2:])
		if *sendPath == "" {
			sendFlags.Usage()
			log.Fatal("-path is required")
		}
		manager.SendFile(*sendPath)
	case "receive":
		receiveFlags.Parse(os.Args[2:])
		manager.ReceiveToFile(*saveDir)
	case "decompress":
		decompressFlags.Parse(os.Args[2:])
		if *inPath == "" {
			decompressFlags.Usage()
			log.Fatal("-in is required")
		}
		messages.DecompressMessages(*inPath, *outPath)
	case "debugSend":
		a := make([]byte, 64)
		a[0] = 1
		a[63] = 64
		manager.Devices[0].Write(a)
	case "debugReceive":
		a := make([]byte, 66)
		r, _ := manager.Devices[0].Read(a)
		log.Printf("Received %d bytes, 00: %d last: %d", r, a[0], a[r-1])
	default:
		log.Fatal("Invalid subcommand. Valid commands: send, receive, decompress, debugSend, debugReceive")
	}

}
