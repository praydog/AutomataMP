// Code generated by the FlatBuffers compiler. DO NOT EDIT.

package nier

import (
	flatbuffers "github.com/google/flatbuffers/go"
)

type Version struct {
	_tab flatbuffers.Table
}

func GetRootAsVersion(buf []byte, offset flatbuffers.UOffsetT) *Version {
	n := flatbuffers.GetUOffsetT(buf[offset:])
	x := &Version{}
	x.Init(buf, n+offset)
	return x
}

func GetSizePrefixedRootAsVersion(buf []byte, offset flatbuffers.UOffsetT) *Version {
	n := flatbuffers.GetUOffsetT(buf[offset+flatbuffers.SizeUint32:])
	x := &Version{}
	x.Init(buf, n+offset+flatbuffers.SizeUint32)
	return x
}

func (rcv *Version) Init(buf []byte, i flatbuffers.UOffsetT) {
	rcv._tab.Bytes = buf
	rcv._tab.Pos = i
}

func (rcv *Version) Table() flatbuffers.Table {
	return rcv._tab
}

func (rcv *Version) Major() uint32 {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(4))
	if o != 0 {
		return rcv._tab.GetUint32(o + rcv._tab.Pos)
	}
	return 1
}

func (rcv *Version) MutateMajor(n uint32) bool {
	return rcv._tab.MutateUint32Slot(4, n)
}

func (rcv *Version) Minor() uint32 {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(6))
	if o != 0 {
		return rcv._tab.GetUint32(o + rcv._tab.Pos)
	}
	return 0
}

func (rcv *Version) MutateMinor(n uint32) bool {
	return rcv._tab.MutateUint32Slot(6, n)
}

func (rcv *Version) Patch() uint32 {
	o := flatbuffers.UOffsetT(rcv._tab.Offset(8))
	if o != 0 {
		return rcv._tab.GetUint32(o + rcv._tab.Pos)
	}
	return 0
}

func (rcv *Version) MutatePatch(n uint32) bool {
	return rcv._tab.MutateUint32Slot(8, n)
}

func VersionStart(builder *flatbuffers.Builder) {
	builder.StartObject(3)
}
func VersionAddMajor(builder *flatbuffers.Builder, major uint32) {
	builder.PrependUint32Slot(0, major, 1)
}
func VersionAddMinor(builder *flatbuffers.Builder, minor uint32) {
	builder.PrependUint32Slot(1, minor, 0)
}
func VersionAddPatch(builder *flatbuffers.Builder, patch uint32) {
	builder.PrependUint32Slot(2, patch, 0)
}
func VersionEnd(builder *flatbuffers.Builder) flatbuffers.UOffsetT {
	return builder.EndObject()
}
