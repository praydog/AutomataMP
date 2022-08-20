#pragma once

namespace sdk {
enum EAnimation {
    Walk = 1,
    Run = 2,
    Sprint = 3,
    High_Fall = 4,
    Jump = 5,
    Fall = 6,
    High_Fall2 = 7,
    Air_Spin = 9,
    Land = 11,
    Get_Up_Backflip = 12,
    Backflip = 13,
    Vault = 14,
    Lying_Down = 15,
    Light_Attack = 16,
    Dash = 18,
    Dodge = 19,
    Taunt_A2 = 20,
    Parry_thing = 21,
    Electrocuted = 22,
    Taunt_A2_2 = 23,
    Throw_Weapon = 24,
    Slomo_Cutscene = 25,
    Really_Powerful_Kick_2B = 26,
    INVALID_CRASHES_GAME = 27,
    Use_Corpse = 28,
    Use_Corpse_2 = 29,
    Use_Corpse_3 = 30,
    Pick_Up_Item = 31,
    Pervert_2B = 32,
    Pick_Up_Item_2 = 33,
    Pod_Fire_Rocket = 36,
    Pod_Swing = 37,
    Flying_Thing = 38,
    Flying_Mech_Mode_Bugged = 39,
    Push_Back_Weak = 42,
    Push_Back_Weak_2 = 43,
    Push_Back_Medium = 44,
    Push_Back_Hard = 45,
    Push_Back_Fall = 46,
    Push_Box = 48,
    Lift_Up_Fall = 51,
    Die = 53,
    Die_2 = 54,
    Self_Destruct_Start = 55,
    // does a countdown and actually blows up
    Self_Destruct_Complete = 56,
    Self_Destruct_Explode = 57,
    // Slides with anything other than variant 0
    Slide = 58,
    Turn_90_Deg_Right = 63,
    INVALID_CRASHES_GAME2 = 65,
    Hacking_Mode = 70,
    Walk_Forward_Scripted = 85,
    Flying_Mech_Mode = 94,
    Land_Mechs = 95,
    Open_Door = 102,
    Sit_Down = 103,
    INVALID_CRASHES_GAME3 = 104,
    Kick = 105,
    INVALID_CRASHES_GAME4 = 106,
    Lie_In_Bed = 107,
};

enum EModel {
    MODEL_2B = 0x10000,
    MODEL_A2 = 0x10100,
    MODEL_9S = 0x10200,
};

enum ERunSpeedType {
    SPEED_PLAYER,
    SPEED_BUDDY
};
}