package main

import (
	"flag"
	"fmt"
	"github.com/flynn/hid"
	"log"
	"os"
	"sync"
	"time"
	"u2fhid-host/messages"
)

type receivingInfo struct {
	path    string
	channel <-chan []byte
}

func send(path *string) {
	devs, err := hid.Devices()

	if err != nil {
		log.Fatal("Could not fetch devices")
	}

	inpChannels := make([]hid.Device, 0)

	for _, v := range devs {
		if v.VendorID == 0x0483 && v.ProductID == 0x5741 {
			r, err := v.Open()
			if err != nil {
				log.Fatal(err)
			}
			log.Println("Opened " + v.Path)
			inpChannels = append(inpChannels, r)
		}
	}

	wg := sync.WaitGroup{}
	wg.Add(len(inpChannels))

	totalLength := 1.0 * 1000 * 1000
	sendData := 0.0
	received := false
	var start time.Time
	var lastReceived time.Time

	for _, v := range inpChannels {
		go func(device hid.Device) {
			for sendData < totalLength {
				d := make([]byte, 65)
				err := device.Write(d)
				if err == nil {
					sendData += 64
					if !received {
						start = time.Now()
						lastReceived = start
					}
					received = true
					timeDiff := time.Now().Sub(start)
					if time.Now().Sub(lastReceived) >= 2*time.Second {
						lastReceived = time.Now()
						log.Println(fmt.Sprintf("Sent %.2f%% (%.2f MB) @ %.2f kB/s", sendData/totalLength*100, sendData/1000.0/1000.0, sendData/1000.0/timeDiff.Seconds()))
					}
				} else {
					log.Fatal("Error received ", err)
				}
			}
			wg.Done()
		}(v)
	}

	wg.Wait()
}

func receive(path *string) {
	devs, err := hid.Devices()

	if err != nil {
		log.Fatal("Could not fetch devices")
	}

	inpChannels := make([]receivingInfo, 0)

	for _, v := range devs {
		if v.VendorID == 0x0483 && v.ProductID == 0x5741 {
			r, err := v.Open()
			if err != nil {
				log.Fatal(err)
			}
			log.Println("Opened " + v.Path)
			inpChannels = append(inpChannels, receivingInfo{
				path:    v.Path,
				channel: r.ReadCh(),
			})
		}
	}

	mergedChannel := merge(inpChannels...)
	dataBuffer := make([]byte, 0, 2000)

	var metadata *messages.MetadataMessage
	n := time.Now()
	start := time.Now()

	for msg := range mergedChannel {
		if msg.MsgType() == messages.MessageTypeMetadata {
			m := msg.(*messages.MetadataMessage)
			metadata = m
			start = time.Now()
			log.Printf("Start receiving file %s %.2f kB\n", metadata.FileName, float64(metadata.FileSize)/1000.0)
		} else if msg.MsgType() == messages.MessageTypeFullPayload {
			m := msg.(*messages.FullPayloadMessage)
			dataBuffer = append(dataBuffer, m.Payload...)

			if metadata != nil {
				per := float64(len(dataBuffer)) / float64(metadata.FileSize) * 100.0
				if time.Now().Sub(n) >= time.Second*10 {
					n = time.Now()
					rkb := float64(len(dataBuffer)) / 1000.0
					seconds := n.Sub(start).Seconds()
					log.Println(fmt.Sprintf("Received %.2f%% (%.2f MB) @ %.2f kB/s", per, rkb/1000.0, rkb/seconds))
				}
			}
		}
	}

	if metadata.FileSize != uint32(len(dataBuffer)) {
		println("File size does not match")
	}

	println(string(dataBuffer))
}

func main() {

	sendFlags := flag.NewFlagSet("send", flag.ContinueOnError)
	sendPath := sendFlags.String("path", "", "File that should be send")
	receiveFlags := flag.NewFlagSet("receive", flag.ContinueOnError)
	saveDir := receiveFlags.String("path", "", "Directory where to save the file")

	switch os.Args[1] {
	case "send":
		sendFlags.Parse(os.Args[2:])
		send(sendPath)
	case "receive":
		receiveFlags.Parse(os.Args[2:])
		receive(saveDir)
	}
}

func merge(cs ...receivingInfo) <-chan messages.IMessage {
	out := make(chan messages.IMessage)
	var wg sync.WaitGroup
	wg.Add(len(cs))
	for _, c := range cs {
		go func(c receivingInfo) {
			receiveBuffer := make([]byte, 0, 128)
			received := false
		F:
			for {
				select {
				case a := <-c.channel:
					receiveBuffer = append(receiveBuffer, a...)
					if len(receiveBuffer) >= 64 {
						msg := messages.ParseMessage(receiveBuffer[:64])
						receiveBuffer = receiveBuffer[64:]
						out <- msg

						if !received {
							received = true
							log.Println("Received packet on " + c.path)
						}
					}
				case <-time.After(2 * time.Second):
					if received {
						log.Println("Channel Timeout " + c.path)
						break F
					}
				}
			}
			wg.Done()
		}(c)
	}
	go func() {
		wg.Wait()
		close(out)
	}()
	return out
}
