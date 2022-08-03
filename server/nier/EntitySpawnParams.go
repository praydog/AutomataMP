// Code generated by the FlatBuffers compiler. DO NOT EDIT.

package nier

import (
	flatbuffers "github.com/google/flatbuffers/go"
)

type EntitySpawnParams struct {
	_tab flatbuffers.Struct
}

func (rcv *EntitySpawnParams) Init(buf []byte, i flatbuffers.UOffsetT) {
	rcv._tab.Bytes = buf
	rcv._tab.Pos = i
}

func (rcv *EntitySpawnParams) Table() flatbuffers.Table {
	return rcv._tab.Table
}

func (rcv *EntitySpawnParams) Forward(obj *Vector4f) *Vector4f {
	if obj == nil {
		obj = new(Vector4f)
	}
	obj.Init(rcv._tab.Bytes, rcv._tab.Pos+0)
	return obj
}
func (rcv *EntitySpawnParams) Up(obj *Vector4f) *Vector4f {
	if obj == nil {
		obj = new(Vector4f)
	}
	obj.Init(rcv._tab.Bytes, rcv._tab.Pos+16)
	return obj
}
func (rcv *EntitySpawnParams) Right(obj *Vector4f) *Vector4f {
	if obj == nil {
		obj = new(Vector4f)
	}
	obj.Init(rcv._tab.Bytes, rcv._tab.Pos+32)
	return obj
}
func (rcv *EntitySpawnParams) W(obj *Vector4f) *Vector4f {
	if obj == nil {
		obj = new(Vector4f)
	}
	obj.Init(rcv._tab.Bytes, rcv._tab.Pos+48)
	return obj
}
func (rcv *EntitySpawnParams) Position(obj *Vector4f) *Vector4f {
	if obj == nil {
		obj = new(Vector4f)
	}
	obj.Init(rcv._tab.Bytes, rcv._tab.Pos+64)
	return obj
}
func (rcv *EntitySpawnParams) Unknown(obj *Vector4f) *Vector4f {
	if obj == nil {
		obj = new(Vector4f)
	}
	obj.Init(rcv._tab.Bytes, rcv._tab.Pos+80)
	return obj
}
func (rcv *EntitySpawnParams) Unknown2(obj *Vector4f) *Vector4f {
	if obj == nil {
		obj = new(Vector4f)
	}
	obj.Init(rcv._tab.Bytes, rcv._tab.Pos+96)
	return obj
}
func (rcv *EntitySpawnParams) Unk() uint32 {
	return rcv._tab.GetUint32(rcv._tab.Pos + flatbuffers.UOffsetT(112))
}
func (rcv *EntitySpawnParams) MutateUnk(n uint32) bool {
	return rcv._tab.MutateUint32(rcv._tab.Pos+flatbuffers.UOffsetT(112), n)
}

func (rcv *EntitySpawnParams) Unk2() uint32 {
	return rcv._tab.GetUint32(rcv._tab.Pos + flatbuffers.UOffsetT(116))
}
func (rcv *EntitySpawnParams) MutateUnk2(n uint32) bool {
	return rcv._tab.MutateUint32(rcv._tab.Pos+flatbuffers.UOffsetT(116), n)
}

func (rcv *EntitySpawnParams) Unk3() uint32 {
	return rcv._tab.GetUint32(rcv._tab.Pos + flatbuffers.UOffsetT(120))
}
func (rcv *EntitySpawnParams) MutateUnk3(n uint32) bool {
	return rcv._tab.MutateUint32(rcv._tab.Pos+flatbuffers.UOffsetT(120), n)
}

func (rcv *EntitySpawnParams) Unk4() uint32 {
	return rcv._tab.GetUint32(rcv._tab.Pos + flatbuffers.UOffsetT(124))
}
func (rcv *EntitySpawnParams) MutateUnk4(n uint32) bool {
	return rcv._tab.MutateUint32(rcv._tab.Pos+flatbuffers.UOffsetT(124), n)
}

func (rcv *EntitySpawnParams) Unk5() uint32 {
	return rcv._tab.GetUint32(rcv._tab.Pos + flatbuffers.UOffsetT(128))
}
func (rcv *EntitySpawnParams) MutateUnk5(n uint32) bool {
	return rcv._tab.MutateUint32(rcv._tab.Pos+flatbuffers.UOffsetT(128), n)
}

func (rcv *EntitySpawnParams) Unk6() uint32 {
	return rcv._tab.GetUint32(rcv._tab.Pos + flatbuffers.UOffsetT(132))
}
func (rcv *EntitySpawnParams) MutateUnk6(n uint32) bool {
	return rcv._tab.MutateUint32(rcv._tab.Pos+flatbuffers.UOffsetT(132), n)
}

func (rcv *EntitySpawnParams) Unk7() uint32 {
	return rcv._tab.GetUint32(rcv._tab.Pos + flatbuffers.UOffsetT(136))
}
func (rcv *EntitySpawnParams) MutateUnk7(n uint32) bool {
	return rcv._tab.MutateUint32(rcv._tab.Pos+flatbuffers.UOffsetT(136), n)
}

func (rcv *EntitySpawnParams) Unk8() uint32 {
	return rcv._tab.GetUint32(rcv._tab.Pos + flatbuffers.UOffsetT(140))
}
func (rcv *EntitySpawnParams) MutateUnk8(n uint32) bool {
	return rcv._tab.MutateUint32(rcv._tab.Pos+flatbuffers.UOffsetT(140), n)
}

func CreateEntitySpawnParams(builder *flatbuffers.Builder, forward_x float32, forward_y float32, forward_z float32, forward_w float32, up_x float32, up_y float32, up_z float32, up_w float32, right_x float32, right_y float32, right_z float32, right_w float32, w_x float32, w_y float32, w_z float32, w_w float32, position_x float32, position_y float32, position_z float32, position_w float32, unknown_x float32, unknown_y float32, unknown_z float32, unknown_w float32, unknown2_x float32, unknown2_y float32, unknown2_z float32, unknown2_w float32, unk uint32, unk2 uint32, unk3 uint32, unk4 uint32, unk5 uint32, unk6 uint32, unk7 uint32, unk8 uint32) flatbuffers.UOffsetT {
	builder.Prep(4, 144)
	builder.PrependUint32(unk8)
	builder.PrependUint32(unk7)
	builder.PrependUint32(unk6)
	builder.PrependUint32(unk5)
	builder.PrependUint32(unk4)
	builder.PrependUint32(unk3)
	builder.PrependUint32(unk2)
	builder.PrependUint32(unk)
	builder.Prep(4, 16)
	builder.PrependFloat32(unknown2_w)
	builder.PrependFloat32(unknown2_z)
	builder.PrependFloat32(unknown2_y)
	builder.PrependFloat32(unknown2_x)
	builder.Prep(4, 16)
	builder.PrependFloat32(unknown_w)
	builder.PrependFloat32(unknown_z)
	builder.PrependFloat32(unknown_y)
	builder.PrependFloat32(unknown_x)
	builder.Prep(4, 16)
	builder.PrependFloat32(position_w)
	builder.PrependFloat32(position_z)
	builder.PrependFloat32(position_y)
	builder.PrependFloat32(position_x)
	builder.Prep(4, 16)
	builder.PrependFloat32(w_w)
	builder.PrependFloat32(w_z)
	builder.PrependFloat32(w_y)
	builder.PrependFloat32(w_x)
	builder.Prep(4, 16)
	builder.PrependFloat32(right_w)
	builder.PrependFloat32(right_z)
	builder.PrependFloat32(right_y)
	builder.PrependFloat32(right_x)
	builder.Prep(4, 16)
	builder.PrependFloat32(up_w)
	builder.PrependFloat32(up_z)
	builder.PrependFloat32(up_y)
	builder.PrependFloat32(up_x)
	builder.Prep(4, 16)
	builder.PrependFloat32(forward_w)
	builder.PrependFloat32(forward_z)
	builder.PrependFloat32(forward_y)
	builder.PrependFloat32(forward_x)
	return builder.Offset()
}