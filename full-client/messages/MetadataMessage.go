package messages

import "encoding/binary"

type MetadataMessage struct {
	Header   MessageHeader
	FileSize uint32
	FileName string
}

func NewMetadataMessage(fileName string, fileSize uint32) *MetadataMessage {
	r := new(MetadataMessage)
	r.Header = MessageHeader{
		Counter:     0,
		MessageType: MessageTypeMetadata,
	}
	r.FileName = fileName
	r.FileSize = fileSize
	return r
}

func (m *MetadataMessage) Parse(header MessageHeader, payload []byte) {
	m.Header = header
	m.FileSize = binary.LittleEndian.Uint32(payload[:4])

	n := payload[4:]
	for idx, i := range n {
		if i == 0 {
			m.FileName = string(n[:idx])
			break
		}
	}
}

func (m *MetadataMessage) Serialize() []byte {
	r := make([]byte, 0, 64)
	r = append(r, m.Header.Serialize()...)
	r = binary.LittleEndian.AppendUint32(r, m.FileSize)
	r = append(r, m.FileName...)
	if len(r) < 64 {
		r = append(r, make([]byte, 64-len(r))...)
	}
	return r
}

func (m *MetadataMessage) MsgType() MessageType {
	return m.Header.MessageType
}

func (m *MetadataMessage) MsgHeader() MessageHeader {
	return m.Header
}
