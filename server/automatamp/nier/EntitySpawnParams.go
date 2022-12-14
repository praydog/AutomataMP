// Code generated by the FlatBuffers compiler. DO NOT EDIT.

package nier

import (
	flatbuffers "github.com/google/flatbuffers/go"
)

type EntitySpawnParams struct {
	_tab flatbuffers.Table
}

func GetRootAsEntitySpawnParams(buf []byte, offset flatbuffers.UOffsetT) *EntitySpawnParams {
	n := flatbuffers.GetUOffsetT(buf[offset:])
	x := &EntitySpawnParams{}
	x.Init(buf, n+offset)
	return x
}

func GetSizePrefixedRootAsEntitySpawnParams(buf []byte, offset flatbuffers.UOffsetT) *EntitySpawnParams {
	n := flatbuffers.GetUOffsetT(buf[offset+flatbuffers.SizeUint32:])
	x := &EntitySpawnParams{}
	x.Init(buf, n+offset+flatbuffers.SizeUint32)
	return x
}

func (rcv *EntitySpawnParams) Init(buf []byte, i flatbuffers.UOffsetT) {
	rcv._tab.Bytes = buf
	rcv._tab.Pos = i
}

func (rcv *EntitySpawnParams) Table() flatbuffers.Table {
	return rcv._tab
}

func (rcv *EntitySpawnParams) Name() []byte {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(4))
	if o != 0 {
		return rcv._tab.ByteVector(o + rcv._tab.Pos)
	}
	return nil
}

func (rcv *EntitySpawnParams) Model() uint32 {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(6))
	if o != 0 {
		return rcv._tab.GetUint32(o + rcv._tab.Pos)
	}
	return 0
}

func (rcv *EntitySpawnParams) MutateModel(n uint32) bool {
	return rcv._tab.MutateUint32Slot(6, n)
}

func (rcv *EntitySpawnParams) Model2() uint32 {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(8))
	if o != 0 {
		return rcv._tab.GetUint32(o + rcv._tab.Pos)
	}
	return 0
}

func (rcv *EntitySpawnParams) MutateModel2(n uint32) bool {
	return rcv._tab.MutateUint32Slot(8, n)
}

func (rcv *EntitySpawnParams) Positional(obj *EntitySpawnPositionalData) *EntitySpawnPositionalData {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(10))
	if o != 0 {
		x := o + rcv._tab.Pos
		if obj == nil {
			obj = new(EntitySpawnPositionalData)
		}
		obj.Init(rcv._tab.Bytes, x)
		return obj
	}
	return nil
}

func EntitySpawnParamsStart(builder *flatbuffers.Builder) {
	builder.StartObject(4)
}
func EntitySpawnParamsAddName(builder *flatbuffers.Builder, name flatbuffers.UOffsetT) {
	builder.PrependUOffsetTSlot(0, flatbuffers.UOffsetT(name), 0)
}
func EntitySpawnParamsAddModel(builder *flatbuffers.Builder, model uint32) {
	builder.PrependUint32Slot(1, model, 0)
}
func EntitySpawnParamsAddModel2(builder *flatbuffers.Builder, model2 uint32) {
	builder.PrependUint32Slot(2, model2, 0)
}
func EntitySpawnParamsAddPositional(builder *flatbuffers.Builder, positional flatbuffers.UOffsetT) {
	builder.PrependStructSlot(3, flatbuffers.UOffsetT(positional), 0)
}
func EntitySpawnParamsEnd(builder *flatbuffers.Builder) flatbuffers.UOffsetT {
	return builder.EndObject()
}
