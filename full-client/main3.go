package main

import (
	"fmt"
	"github.com/google/gousb"
	"log"
	"time"
	"u2fhid-host/messages"
)

func maicfn() {

	ctx := gousb.NewContext()

	dev, err := ctx.OpenDeviceWithVIDPID(0x0483, 0x5741)
	if err != nil {
		log.Fatal("no device found ", err)
	}

	inf, f, err := dev.DefaultInterface()
	defer f()

	inEndpoint, err := inf.InEndpoint(1)
	if err != nil {
		log.Fatal("Failed to open inEndpoint ", err)
	}

	receiveBuffer := make([]byte, 0, 64)
	dataBuffer := make([]byte, 0, 2000)
	received := false

	var metadata *messages.MetadataMessage
	n := time.Now()
	start := time.Now()

	//L:
	for {
		a := make([]byte, 64, 64)
		readC, err := inEndpoint.Read(a)
		if err != nil {
			//log.Println(err)
			continue
		}

		if len(a) == 0 || readC == 0 {
			continue
		}
		a = a[:readC]
		receiveBuffer = append(receiveBuffer, a...)

		if len(receiveBuffer) < 64 {
			continue
		}

		msg := messages.ParseMessage(receiveBuffer[:64])
		if !received {
			m, ok := msg.(*messages.MetadataMessage)
			if !ok {
				log.Fatal("no metadata message received")
			}
			metadata = m
			received = true
			start = time.Now()
			log.Printf("Start receiving file %s %.2f kB\n", metadata.FileName, float64(metadata.FileSize)/1000.0)
		} else {
			m, ok := msg.(*messages.FullPayloadMessage)
			if !ok {
				log.Fatal("Received non valid payload message")
			}
			dataBuffer = append(dataBuffer, m.Payload...)

			per := float64(len(dataBuffer)) / float64(metadata.FileSize) * 100.0
			if time.Now().Sub(n) >= time.Second*10 {
				n = time.Now()
				rkb := float64(len(dataBuffer)) / 1000.0
				seconds := n.Sub(start).Seconds()
				log.Println(fmt.Sprintf("Received %.2f%% (%.2f MB) @ %.2f kB/s", per, rkb/1000.0, rkb/seconds))
			}

		}
		receiveBuffer = receiveBuffer[64:]
	}

	if metadata.FileSize != uint32(len(dataBuffer)) {
		println("Filesize does not match")
	}

	println(string(dataBuffer))
}
