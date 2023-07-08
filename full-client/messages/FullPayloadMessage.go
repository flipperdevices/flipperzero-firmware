package messages

type FullPayloadMessage struct {
	Header  MessageHeader
	Payload []byte
}

func (m *FullPayloadMessage) Parse(header MessageHeader, payload []byte) {
	m.Header = header
	m.Payload = payload
}

func (m *FullPayloadMessage) Serialize() []byte {
	r := make([]byte, 0, 64)
	r = append(r, m.Header.Serialize()...)
	r = append(r, m.Payload...)
	return r
}

func (m *FullPayloadMessage) MsgType() MessageType {
	return m.Header.MessageType
}

func (m *FullPayloadMessage) MsgHeader() MessageHeader {
	return m.Header
}
