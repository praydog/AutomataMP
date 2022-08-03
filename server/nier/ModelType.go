// Code generated by the FlatBuffers compiler. DO NOT EDIT.

package nier

import "strconv"

type ModelType uint32

const (
	ModelTypeMODEL_2B ModelType = 65536
	ModelTypeMODEL_A2 ModelType = 65792
	ModelTypeMODEL_9S ModelType = 66048
)

var EnumNamesModelType = map[ModelType]string{
	ModelTypeMODEL_2B: "MODEL_2B",
	ModelTypeMODEL_A2: "MODEL_A2",
	ModelTypeMODEL_9S: "MODEL_9S",
}

var EnumValuesModelType = map[string]ModelType{
	"MODEL_2B": ModelTypeMODEL_2B,
	"MODEL_A2": ModelTypeMODEL_A2,
	"MODEL_9S": ModelTypeMODEL_9S,
}

func (v ModelType) String() string {
	if s, ok := EnumNamesModelType[v]; ok {
		return s
	}
	return "ModelType(" + strconv.FormatInt(int64(v), 10) + ")"
}
