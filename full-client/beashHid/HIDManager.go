package beashHid

import (
	"fmt"
	"github.com/bearsh/hid"
	"log"
	"os"
	"path"
	fp "path/filepath"
	"runtime"
	"sync"
	"time"
	"u2fhid-host/messages"
)

type HIDManger struct {
	Devices  []*hid.Device
	Receiver <-chan byte
	Sender   chan byte
}

func NewHIDManger() *HIDManger {
	hidManager := new(HIDManger)
	hidManager.Devices = make([]*hid.Device, 0)
	hidManager.Receiver = make(<-chan byte, 0)
	hidManager.Sender = make(chan byte, 0)

	devs := hid.Enumerate(0x0485, 0xffff)
	for _, a := range devs {
		d, err := a.Open()
		_ = d.SetNonblocking(false)
		if err != nil {
			log.Println("Failed to open device ", err)
			continue
		}

		hidManager.Devices = append(hidManager.Devices, d)
	}

	return hidManager
}

func (m *HIDManger) SendFile(filePath string) {
	if len(m.Devices) == 0 {
		log.Fatal("No HID devices found.")
	}

	absPath, _ := fp.Abs(filePath)
	fileName := fmt.Sprintf("%s_%s", time.Now().Format("020106_150405"), fp.Base(absPath))
	fileContent, err := os.ReadFile(filePath)
	if err != nil {
		log.Fatal(err)
	}

	f, _ := os.Open(filePath)
	fi, _ := f.Stat()

	metaDataMessage := messages.NewMetadataMessage(fileName, uint32(fi.Size()))
	sendBuf := metaDataMessage.Serialize()
	if runtime.GOOS == "linux" {
		sendBuf = append([]byte{0}, metaDataMessage.Serialize()...)
	}
	w, err := m.Devices[0].Write(sendBuf)
	log.Printf("Sent Metadata message %d\n", w)

	<-time.After(2 * time.Second)

	var lock sync.Mutex
	var wg sync.WaitGroup
	wg.Add(len(m.Devices))

	readIndex := 0
	msgCounter := uint32(1)

	lastLog := time.Now()
	start := time.Now()

	for _, dev := range m.Devices {
		go func(d *hid.Device) {
			for {
				var msg messages.IMessage
				lock.Lock()
				toRead := len(fileContent) - readIndex
				if toRead <= 0 {
					lock.Unlock()
					break
				} else if toRead < 61 {
					msg = &messages.PartPayloadMessage{
						Header: messages.MessageHeader{
							Counter:     msgCounter + 1,
							MessageType: messages.MessageTypePartPayload,
						},
						PayloadLength: uint8(toRead),
						Payload:       fileContent[readIndex:],
					}
				} else {
					toRead = 61
					msg = &messages.FullPayloadMessage{
						Header: messages.MessageHeader{
							Counter:     msgCounter + 1,
							MessageType: messages.MessageTypeFullPayload,
						},
						Payload: fileContent[readIndex : readIndex+61],
					}
				}
				readIndex += 61
				msgCounter += 1

				timeDiff := time.Now().Sub(lastLog).Seconds()
				if timeDiff >= 2.0 {
					lastLog = time.Now()
					log.Println(fmt.Sprintf("Sent %.2f%% (%.2f MB) @ %.2f kB/s",
						float64(readIndex)/float64(metaDataMessage.FileSize)*100,
						float64(readIndex)/1000.0/1000.0,
						float64(readIndex)/1000.0/time.Now().Sub(start).Seconds()))
				}
				lock.Unlock()
				sendBuf := msg.Serialize()
				if runtime.GOOS == "linux" {
					sendBuf = append([]byte{0}, msg.Serialize()...)
				}
				d.Write(sendBuf)
			}
			wg.Done()
		}(dev)
	}
	wg.Wait()
}

func (m *HIDManger) ReceiveToFile(savePath string) {

	if len(m.Devices) == 0 {
		log.Fatal("No HID devices found.")
	}

	log.Println("Waiting for file...")

	msgReceiver := make(chan messages.IMessage)

	for _, dev := range m.Devices {
		go func(d *hid.Device) {
			received := false
			receivedBuffer := make([]byte, 0, 64)
			for {
				timeout := 5000
				if !received {
					timeout = -1
				}

				readBuffer := make([]byte, 67)
				read, err := d.ReadTimeout(readBuffer, timeout)
				if err != nil && err == hid.ErrDeviceClosed {
					log.Fatal("Could not read device, device closed")
				} else if err != nil {
					log.Println("Probably timeout error ", err)
					return
				}

				if read > 64 {
					log.Printf("received more than 64 bytes (%d)", read)
				} else if read < 64 && read > 0 {
					log.Printf("received less than 64 bytes (%d)", read)
				}

				received = true
				receivedBuffer = append(receivedBuffer, readBuffer[:read]...)
				for len(receivedBuffer) >= 64 {
					msg := messages.ParseMessage(receivedBuffer[:64])
					msgReceiver <- msg
					receivedBuffer = receivedBuffer[64:]
				}
			}
		}(dev)
	}

	var metadata *messages.MetadataMessage
	received := false
	msgs := make([]messages.IMessage, 0, 10000)
	start := time.Now()
	lastLog := time.Now()
	receivedData := 0

L:
	for {
		select {
		case msg := <-msgReceiver:
			if !received {
				start = time.Now()
				lastLog = start
			}
			received = true
			msgs = append(msgs, msg)
			if msg.MsgType() == messages.MessageTypeMetadata {
				metadata = msg.(*messages.MetadataMessage)
				log.Println(fmt.Sprintf("Receiving file: %s, Size: %.2fMB",
					metadata.FileName, float64(metadata.FileSize)/1000.0/1000.0))
			} else {
				receivedData += 61
				timeDiff := time.Now().Sub(lastLog).Seconds()
				if timeDiff >= 2.0 {
					lastLog = time.Now()
					log.Println(fmt.Sprintf("Received %.2f%% (%.2f MB) @ %.2f kB/s",
						float64(receivedData)/float64(metadata.FileSize)*100,
						float64(receivedData)/1000.0/1000.0,
						float64(receivedData)/1000.0/time.Now().Sub(start).Seconds()))
				}
			}
		case <-time.After(5 * time.Second):
			if received {
				log.Println("Timeout, receiving done.")
				break L
			}
		}
	}

	if savePath == "" {
		savePath = "./" + metadata.FileName
	} else {
		statInfo, _ := os.Stat(savePath)
		if statInfo != nil && statInfo.IsDir() {
			savePath = path.Join(savePath, metadata.FileName)
		}
	}

	messages.StoreMessages(msgs, savePath)
}
