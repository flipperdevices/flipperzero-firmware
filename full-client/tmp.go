package main

import (
	"encoding/binary"
	"log"
)

func maicn() {
	//a := ((1<<24) - 1) - 3
	log.Println(binary.LittleEndian.Uint64([]byte{1, 0, 0, 0, 0, 0, 0, 0}))
}
