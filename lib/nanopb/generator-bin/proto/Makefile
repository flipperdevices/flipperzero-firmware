PROTOC?=protoc

all: nanopb_pb2.py

%_pb2.py: %.proto
	$(PROTOC) --python_out=. $<
