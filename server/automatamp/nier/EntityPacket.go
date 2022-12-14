// Code generated by the FlatBuffers compiler. DO NOT EDIT.

package nier

import (
	flatbuffers "github.com/google/flatbuffers/go"
)

type EntityPacket struct {
	_tab flatbuffers.Table
}

func GetRootAsEntityPacket(buf []byte, offset flatbuffers.UOffsetT) *EntityPacket {
	n := flatbuffers.GetUOffsetT(buf[offset:])
	x := &EntityPacket{}
	x.Init(buf, n+offset)
	return x
}

func GetSizePrefixedRootAsEntityPacket(buf []byte, offset flatbuffers.UOffsetT) *EntityPacket {
	n := flatbuffers.GetUOffsetT(buf[offset+flatbuffers.SizeUint32:])
	x := &EntityPacket{}
	x.Init(buf, n+offset+flatbuffers.SizeUint32)
	return x
}

func (rcv *EntityPacket) Init(buf []byte, i flatbuffers.UOffsetT) {
	rcv._tab.Bytes = buf
	rcv._tab.Pos = i
}

func (rcv *EntityPacket) Table() flatbuffers.Table {
	return rcv._tab
}

func (rcv *EntityPacket) Guid() uint32 {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(4))
	if o != 0 {
		return rcv._tab.GetUint32(o + rcv._tab.Pos)
	}
	return 0
}

func (rcv *EntityPacket) MutateGuid(n uint32) bool {
	return rcv._tab.MutateUint32Slot(4, n)
}

func (rcv *EntityPacket) Data(j int) byte {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(6))
	if o != 0 {
		a := rcv._tab.Vector(o)
		return rcv._tab.GetByte(a + flatbuffers.UOffsetT(j*1))
	}
	return 0
}

func (rcv *EntityPacket) DataLength() int {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(6))
	if o != 0 {
		return rcv._tab.VectorLen(o)
	}
	return 0
}

func (rcv *EntityPacket) DataBytes() []byte {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(6))
	if o != 0 {
		return rcv._tab.ByteVector(o + rcv._tab.Pos)
	}
	return nil
}

func (rcv *EntityPacket) MutateData(j int, n byte) bool {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(6))
	if o != 0 {
		a := rcv._tab.Vector(o)
		return rcv._tab.MutateByte(a+flatbuffers.UOffsetT(j*1), n)
	}
	return false
}

func EntityPacketStart(builder *flatbuffers.Builder) {
	builder.StartObject(2)
}
func EntityPacketAddGuid(builder *flatbuffers.Builder, guid uint32) {
	builder.PrependUint32Slot(0, guid, 0)
}
func EntityPacketAddData(builder *flatbuffers.Builder, data flatbuffers.UOffsetT) {
	builder.PrependUOffsetTSlot(1, flatbuffers.UOffsetT(data), 0)
}
func EntityPacketStartDataVector(builder *flatbuffers.Builder, numElems int) flatbuffers.UOffsetT {
	return builder.StartVector(1, numElems, 1)
}
func EntityPacketEnd(builder *flatbuffers.Builder) flatbuffers.UOffsetT {
	return builder.EndObject()
}
