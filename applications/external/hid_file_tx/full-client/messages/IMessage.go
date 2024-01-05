package messages

import (
	"fmt"
	"log"
	"os"
	"path"
	"sort"
	"strings"
	"time"
)

type IMessage interface {
	MsgType() MessageType
	MsgHeader() MessageHeader
	Parse(MessageHeader, []byte)
	Serialize() []byte
}

func ParseMessage(payload []byte) IMessage {
	messageHeader := new(MessageHeader)
	messageHeader.Parse(payload)

	var msg IMessage

	switch messageHeader.MessageType {
	case MessageTypeMetadata:
		msg = new(MetadataMessage)
	case MessageTypeFullPayload:
		msg = new(FullPayloadMessage)
	case MessageTypePartPayload:
		msg = new(PartPayloadMessage)
	}

	msg.Parse(*messageHeader, payload[3:])
	return msg
}

func DecompressMessages(inPath string, outPath string) {
	fileContent, err := os.ReadFile(inPath)
	if err != nil {
		log.Fatal(err)
	}

	if len(fileContent)%64 != 0 {
		log.Println("File might be corrupted!")
	}

	msgs := make([]IMessage, 0, 2000)
	for len(fileContent) > 0 {
		msgs = append(msgs, ParseMessage(fileContent[:64]))
		fileContent = fileContent[64:]
	}

	if outPath == "" {
		if path.Ext(inPath) != ".raw" {
			outPath = inPath + "_decompress" + path.Ext(inPath)
		} else {
			outPath = strings.ReplaceAll(inPath, ".raw", "")
		}
	} else {
		info, _ := os.Stat(outPath)
		if info != nil && info.IsDir() {
			outPath = path.Join(outPath, path.Base(inPath))
		}
	}

	StoreMessages(msgs, outPath)
}

func StoreMessages(msgs []IMessage, fpath string) {

	sort.Slice(msgs, func(i, j int) bool {
		return msgs[i].MsgHeader().Counter < msgs[j].MsgHeader().Counter
	})

	metaData := msgs[0].(*MetadataMessage)
	fileBuffer := make([]byte, 0)

	counter := uint32(1)
	missingCounters := make([]uint32, 0)

	for _, msg := range msgs[1:] {
		if msg.MsgHeader().Counter != counter {
			missingCounters = append(missingCounters, counter)
		} else {
			counter = msg.MsgHeader().Counter
		}

		counter += 1

		if msg.MsgType() == MessageTypeFullPayload {
			fileBuffer = append(fileBuffer, msg.(*FullPayloadMessage).Payload...)
		} else if msg.MsgType() == MessageTypePartPayload {
			fileBuffer = append(fileBuffer, msg.(*PartPayloadMessage).Payload...)
		} else {
			log.Fatal("Unknown message type...")
		}
	}

	if uint32(len(fileBuffer)) != metaData.FileSize {
		log.Println(fmt.Sprintf("File size does not match! Received: %d, Should be %d", len(fileBuffer), metaData.FileSize))
		log.Println(fmt.Sprintf("%d packets missing (%s)", len(missingCounters), strings.Trim(strings.Join(strings.Fields(fmt.Sprint(missingCounters)), ", "), "[]")))
	}

	if _, err := os.Stat(fpath); err == nil {
		d := path.Dir(fpath)
		base := path.Base(fpath)
		base = time.Now().Format("020106-030405") + "_" + base
		fpath = path.Join(d, base)
		log.Println(fmt.Sprintf("File exists, saved to %s!", fpath))
	}

	err := os.WriteFile(fpath, fileBuffer, 0644)
	if err != nil {
		log.Fatal(fmt.Sprintf("Could not write file %s", err))
	}
}
