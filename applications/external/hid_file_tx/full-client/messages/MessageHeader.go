package messages

import "encoding/binary"

type MessageType uint8

const (
	MessageTypeMetadata    MessageType = 0
	MessageTypeFullPayload MessageType = 1
	MessageTypePartPayload MessageType = 2
)

type MessageHeader struct {
	Counter     uint32
	MessageType MessageType
}

func (h *MessageHeader) Parse(payload []byte) {
	header := binary.LittleEndian.Uint32([]byte{payload[0], payload[1], payload[2], 0})

	h.Counter = header >> 2
	h.MessageType = MessageType(header & 3)
}

func (h *MessageHeader) Serialize() []byte {
	r := make([]byte, 0)

	n := uint32(h.Counter<<2) | uint32(h.MessageType)
	r = binary.LittleEndian.AppendUint32(r, n)
	return r[:3]
}
