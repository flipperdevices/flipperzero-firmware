#include "message.pb.hpp"

#include <pb_encode.h>
#include <pb_decode.h>

#include <algorithm>
#include <cstdio>

// See tests/alltypes_callback, tests/oneoff_callback and examples/network_server for more...
bool TestMessage_submessages_callback(pb_istream_t *istream, pb_ostream_t *ostream, const pb_field_t *field)
{
	if (ostream != NULL) {
		const std::vector<int> &v = *(const std::vector<int> *)field->pData;
		for (std::vector<int>::const_iterator i = v.begin(); i != v.end(); ++i) {
			if (!pb_encode_tag_for_field(ostream, field)) {
				return false;
			}
			SubMessage tmp;
			tmp.actual_value = *i;
			if (!pb_encode_submessage(ostream, SubMessage_fields, &tmp)) {
				return false;
			}
		}
	} else if (istream != NULL) {
		std::vector<int> &v = *(std::vector<int> *)field->pData;
		SubMessage tmp;
		if (!pb_decode(istream, SubMessage_fields, &tmp)) {
			return false;
		}
		v.push_back(tmp.actual_value);
	}
	return true;
}

extern "C"
bool TestMessage_callback(pb_istream_t *istream, pb_ostream_t *ostream, const pb_field_t *field)
{
	if (field->tag == TestMessage_submessages_tag) {
		return TestMessage_submessages_callback(istream, ostream, field);
	}
	return true;
}

extern "C"
int main() {
	std::vector<int> source;
	source.push_back(5);
	source.push_back(4);
	source.push_back(3);
	source.push_back(2);
	source.push_back(1);


	std::vector<uint8_t> serialized;
	pb_ostream_t sizestream = {0};
	pb_encode(&sizestream, TestMessage_fields, &source);
	serialized.resize(sizestream.bytes_written);
	pb_ostream_t outstream = pb_ostream_from_buffer(&serialized.front(), serialized.size());
	if (!pb_encode(&outstream, TestMessage_fields, &source)) {
		fprintf(stderr, "Failed to encode: %s\n", PB_GET_ERROR(&outstream));
		return 1;
	}


	std::vector<int> destination;
	pb_istream_t instream = pb_istream_from_buffer(&serialized.front(), outstream.bytes_written);
	if (!pb_decode(&instream, TestMessage_fields, &destination)) {
		fprintf(stderr, "Failed to decode: %s\n", PB_GET_ERROR(&instream));
		return 2;
	}
	if (source != destination) {
		fprintf(stderr, "Result does not match\n");
		fprintf(stderr, "source(%d): ", (int)source.size());
		for (std::vector<int>::iterator i = source.begin(); i != source.end(); ++i)
		{
			fprintf(stderr, "%d, ", *i);
		}
		fprintf(stderr, "\nencoded(%d): ", (int)serialized.size());
		for (unsigned i = 0; i != std::min(serialized.size(), outstream.bytes_written); ++i) {
			fprintf(stderr, "0x%02x ", serialized[i]);
		}
		fprintf(stderr, "\ndestination(%d): ", (int)destination.size());
		for (std::vector<int>::iterator i = destination.begin(); i != destination.end(); ++i)
		{
			fprintf(stderr, "%d, ", *i);
		}
		fprintf(stderr, "\n");
		return 3;
	}

	return 0;
}
