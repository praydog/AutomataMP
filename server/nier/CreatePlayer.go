// Code generated by the FlatBuffers compiler. DO NOT EDIT.

package nier

import (
	flatbuffers "github.com/google/flatbuffers/go"
)

type CreatePlayer struct {
	_tab flatbuffers.Table
}

func GetRootAsCreatePlayer(buf []byte, offset flatbuffers.UOffsetT) *CreatePlayer {
	n := flatbuffers.GetUOffsetT(buf[offset:])
	x := &CreatePlayer{}
	x.Init(buf, n+offset)
	return x
}

func GetSizePrefixedRootAsCreatePlayer(buf []byte, offset flatbuffers.UOffsetT) *CreatePlayer {
	n := flatbuffers.GetUOffsetT(buf[offset+flatbuffers.SizeUint32:])
	x := &CreatePlayer{}
	x.Init(buf, n+offset+flatbuffers.SizeUint32)
	return x
}

func (rcv *CreatePlayer) Init(buf []byte, i flatbuffers.UOffsetT) {
	rcv._tab.Bytes = buf
	rcv._tab.Pos = i
}

func (rcv *CreatePlayer) Table() flatbuffers.Table {
	return rcv._tab
}

func (rcv *CreatePlayer) Guid() uint64 {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(4))
	if o != 0 {
		return rcv._tab.GetUint64(o + rcv._tab.Pos)
	}
	return 0
}

func (rcv *CreatePlayer) MutateGuid(n uint64) bool {
	return rcv._tab.MutateUint64Slot(4, n)
}

func (rcv *CreatePlayer) Name() []byte {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(6))
	if o != 0 {
		return rcv._tab.ByteVector(o + rcv._tab.Pos)
	}
	return nil
}

func (rcv *CreatePlayer) Model() uint32 {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(8))
	if o != 0 {
		return rcv._tab.GetUint32(o + rcv._tab.Pos)
	}
	return 0
}

func (rcv *CreatePlayer) MutateModel(n uint32) bool {
	return rcv._tab.MutateUint32Slot(8, n)
}

func CreatePlayerStart(builder *flatbuffers.Builder) {
	builder.StartObject(3)
}
func CreatePlayerAddGuid(builder *flatbuffers.Builder, guid uint64) {
	builder.PrependUint64Slot(0, guid, 0)
}
func CreatePlayerAddName(builder *flatbuffers.Builder, name flatbuffers.UOffsetT) {
	builder.PrependUOffsetTSlot(1, flatbuffers.UOffsetT(name), 0)
}
func CreatePlayerAddModel(builder *flatbuffers.Builder, model uint32) {
	builder.PrependUint32Slot(2, model, 0)
}
func CreatePlayerEnd(builder *flatbuffers.Builder) flatbuffers.UOffsetT {
	return builder.EndObject()
}
