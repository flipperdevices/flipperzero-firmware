package messages

type PartPayloadMessage struct {
	Header        MessageHeader
	PayloadLength uint8
	Payload       []byte
}

func (m *PartPayloadMessage) Parse(header MessageHeader, payload []byte) {
	m.Header = header
	m.PayloadLength = payload[0]
	m.Payload = payload[1 : m.PayloadLength+1]
}

func (m *PartPayloadMessage) Serialize() []byte {
	r := make([]byte, 0, 64)
	r = append(r, m.Header.Serialize()...)
	r = append(r, byte(len(m.Payload)))
	r = append(r, m.Payload...)
	if len(r) < 64 {
		r = append(r, make([]byte, 64-len(r))...)
	}
	return r
}

func (m *PartPayloadMessage) MsgType() MessageType {
	return m.Header.MessageType
}

func (m *PartPayloadMessage) MsgHeader() MessageHeader {
	return m.Header
}
