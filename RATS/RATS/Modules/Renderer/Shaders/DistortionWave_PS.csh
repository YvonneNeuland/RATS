#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 6.3.9600.16384
//
//
// Buffer Definitions: 
//
// cbuffer THIS_IS_PRAM
// {
//
//   float fadeAmount;                  // Offset:    0 Size:     4 [unused]
//   float playerHealth;                // Offset:    4 Size:     4 [unused]
//      = 0x3f800000 
//   float currTime;                    // Offset:    8 Size:     4 [unused]
//      = 0x00000000 
//   float warpAmount;                  // Offset:   12 Size:     4 [unused]
//   float4 color;                      // Offset:   16 Size:    16 [unused]
//   float screenWarp;                  // Offset:   32 Size:     4 [unused]
//   float cutAlpha;                    // Offset:   36 Size:     4 [unused]
//   float gamma;                       // Offset:   40 Size:     4
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim Slot Elements
// ------------------------------ ---------- ------- ----------- ---- --------
// filters                           sampler      NA          NA    0        1
// baseTexture                       texture  float4          2d    0        1
// THIS_IS_PRAM                      cbuffer      NA          NA    0        1
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// TEXCOORD                 0   xy          0     NONE   float   xy  
// NORMAL                   0   xyz         1     NONE   float   xyz 
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_TARGET                0   xyzw        0   TARGET   float   xyzw
//
ps_4_0
dcl_constantbuffer cb0[3], immediateIndexed
dcl_sampler s0, mode_default
dcl_resource_texture2d (float,float,float,float) t0
dcl_input_ps linear v0.xy
dcl_input_ps linear v1.xyz
dcl_output o0.xyzw
dcl_temps 4
mul r0.xy, v0.xyxx, l(32.000000, 16.000000, 0.000000, 0.000000)
lt r0.zw, l(0.000000, 0.000000, 0.031250, 0.062500), v0.xxxy
ge r1.xy, r0.xyxx, -r0.xyxx
movc r1.xy, r1.xyxx, l(1.000000,1.000000,0,0), l(-1.000000,-1.000000,0,0)
mul r1.zw, r0.xxxy, r1.xxxy
frc r1.zw, r1.zzzw
mul r1.xy, r1.zwzz, r1.xyxx
movc r0.xy, r0.zwzz, r1.xyxx, r0.xyxx
sample r0.xyzw, r0.xyxx, t0.xyzw, s0
lt r1.xyz, l(1.000000, 1.000000, 1.000000, 0.000000), |v1.xyzx|
add r2.xy, r0.yzyy, |v1.yzyy|
add r2.xy, r2.xyxx, l(-1.000000, -1.000000, 0.000000, 0.000000)
mov r2.z, l(-1)
mov r1.w, r0.y
movc r3.xy, r1.yyyy, r2.xzxx, r1.wxww
mov r3.z, r0.z
movc r0.yz, r1.zzzz, r2.yyzy, r3.zzyz
not r1.y, r0.z
discard_nz r1.y
add r1.y, r0.x, |v1.x|
add r1.y, r1.y, l(-1.000000)
movc r0.x, r1.x, r1.y, r0.x
dp3 r1.x, v1.xyzx, v1.xyzx
sqrt r1.x, r1.x
mul r1.x, r1.x, l(0.500000)
movc o0.w, r0.z, r1.x, r0.w
log r1.x, r0.x
log r1.y, r3.x
log r1.z, r0.y
mul r0.xyz, r1.xyzx, cb0[2].zzzz
exp o0.xyz, r0.xyzx
ret 
// Approximately 32 instruction slots used
#endif

const BYTE DistortionWave_PS[] =
{
     68,  88,  66,  67, 156, 190, 
    163,  18,  81,  31, 118,   4, 
    153,   8, 194,  45, 140,  63, 
     78,  68,   1,   0,   0,   0, 
    104,   7,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
    100,   2,   0,   0, 180,   2, 
      0,   0, 232,   2,   0,   0, 
    236,   6,   0,   0,  82,  68, 
     69,  70,  40,   2,   0,   0, 
      1,   0,   0,   0, 160,   0, 
      0,   0,   3,   0,   0,   0, 
     28,   0,   0,   0,   0,   4, 
    255, 255,   0,   1,   0,   0, 
    246,   1,   0,   0, 124,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
    132,   0,   0,   0,   2,   0, 
      0,   0,   5,   0,   0,   0, 
      4,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
      1,   0,   0,   0,  13,   0, 
      0,   0, 144,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0, 102, 105, 
    108, 116, 101, 114, 115,   0, 
     98,  97, 115, 101,  84, 101, 
    120, 116, 117, 114, 101,   0, 
     84,  72,  73,  83,  95,  73, 
     83,  95,  80,  82,  65,  77, 
      0, 171, 171, 171, 144,   0, 
      0,   0,   8,   0,   0,   0, 
    184,   0,   0,   0,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 120,   1, 
      0,   0,   0,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0, 132,   1,   0,   0, 
      0,   0,   0,   0, 148,   1, 
      0,   0,   4,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0, 132,   1,   0,   0, 
    164,   1,   0,   0, 168,   1, 
      0,   0,   8,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0, 132,   1,   0,   0, 
    180,   1,   0,   0, 184,   1, 
      0,   0,  12,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0, 132,   1,   0,   0, 
      0,   0,   0,   0, 195,   1, 
      0,   0,  16,   0,   0,   0, 
     16,   0,   0,   0,   0,   0, 
      0,   0, 204,   1,   0,   0, 
      0,   0,   0,   0, 220,   1, 
      0,   0,  32,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0, 132,   1,   0,   0, 
      0,   0,   0,   0, 231,   1, 
      0,   0,  36,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0, 132,   1,   0,   0, 
      0,   0,   0,   0, 240,   1, 
      0,   0,  40,   0,   0,   0, 
      4,   0,   0,   0,   2,   0, 
      0,   0, 132,   1,   0,   0, 
      0,   0,   0,   0, 102,  97, 
    100, 101,  65, 109, 111, 117, 
    110, 116,   0, 171,   0,   0, 
      3,   0,   1,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 112, 108,  97, 121, 
    101, 114,  72, 101,  97, 108, 
    116, 104,   0, 171, 171, 171, 
      0,   0, 128,  63,  99, 117, 
    114, 114,  84, 105, 109, 101, 
      0, 171, 171, 171,   0,   0, 
      0,   0, 119,  97, 114, 112, 
     65, 109, 111, 117, 110, 116, 
      0,  99, 111, 108, 111, 114, 
      0, 171, 171, 171,   1,   0, 
      3,   0,   1,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 115,  99, 114, 101, 
    101, 110,  87,  97, 114, 112, 
      0,  99, 117, 116,  65, 108, 
    112, 104,  97,   0, 103,  97, 
    109, 109,  97,   0,  77, 105, 
     99, 114, 111, 115, 111, 102, 
    116,  32,  40,  82,  41,  32, 
     72,  76,  83,  76,  32,  83, 
    104,  97, 100, 101, 114,  32, 
     67, 111, 109, 112, 105, 108, 
    101, 114,  32,  54,  46,  51, 
     46,  57,  54,  48,  48,  46, 
     49,  54,  51,  56,  52,   0, 
     73,  83,  71,  78,  72,   0, 
      0,   0,   2,   0,   0,   0, 
      8,   0,   0,   0,  56,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   3,   0,   0,  65,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      7,   7,   0,   0,  84,  69, 
     88,  67,  79,  79,  82,  68, 
      0,  78,  79,  82,  77,  65, 
     76,   0,  79,  83,  71,  78, 
     44,   0,   0,   0,   1,   0, 
      0,   0,   8,   0,   0,   0, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
     83,  86,  95,  84,  65,  82, 
     71,  69,  84,   0, 171, 171, 
     83,  72,  68,  82, 252,   3, 
      0,   0,  64,   0,   0,   0, 
    255,   0,   0,   0,  89,   0, 
      0,   4,  70, 142,  32,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,  90,   0,   0,   3, 
      0,  96,  16,   0,   0,   0, 
      0,   0,  88,  24,   0,   4, 
      0, 112,  16,   0,   0,   0, 
      0,   0,  85,  85,   0,   0, 
     98,  16,   0,   3,  50,  16, 
     16,   0,   0,   0,   0,   0, 
     98,  16,   0,   3, 114,  16, 
     16,   0,   1,   0,   0,   0, 
    101,   0,   0,   3, 242,  32, 
     16,   0,   0,   0,   0,   0, 
    104,   0,   0,   2,   4,   0, 
      0,   0,  56,   0,   0,  10, 
     50,   0,  16,   0,   0,   0, 
      0,   0,  70,  16,  16,   0, 
      0,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,  66, 
      0,   0, 128,  65,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     49,   0,   0,  10, 194,   0, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,  61,   0,   0, 
    128,  61,   6,  20,  16,   0, 
      0,   0,   0,   0,  29,   0, 
      0,   8,  50,   0,  16,   0, 
      1,   0,   0,   0,  70,   0, 
     16,   0,   0,   0,   0,   0, 
     70,   0,  16, 128,  65,   0, 
      0,   0,   0,   0,   0,   0, 
     55,   0,   0,  15,  50,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   0,  16,   0,   1,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0, 128,  63,   0,   0, 
    128,  63,   0,   0,   0,   0, 
      0,   0,   0,   0,   2,  64, 
      0,   0,   0,   0, 128, 191, 
      0,   0, 128, 191,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     56,   0,   0,   7, 194,   0, 
     16,   0,   1,   0,   0,   0, 
      6,   4,  16,   0,   0,   0, 
      0,   0,   6,   4,  16,   0, 
      1,   0,   0,   0,  26,   0, 
      0,   5, 194,   0,  16,   0, 
      1,   0,   0,   0, 166,  14, 
     16,   0,   1,   0,   0,   0, 
     56,   0,   0,   7,  50,   0, 
     16,   0,   1,   0,   0,   0, 
    230,  10,  16,   0,   1,   0, 
      0,   0,  70,   0,  16,   0, 
      1,   0,   0,   0,  55,   0, 
      0,   9,  50,   0,  16,   0, 
      0,   0,   0,   0, 230,  10, 
     16,   0,   0,   0,   0,   0, 
     70,   0,  16,   0,   1,   0, 
      0,   0,  70,   0,  16,   0, 
      0,   0,   0,   0,  69,   0, 
      0,   9, 242,   0,  16,   0, 
      0,   0,   0,   0,  70,   0, 
     16,   0,   0,   0,   0,   0, 
     70, 126,  16,   0,   0,   0, 
      0,   0,   0,  96,  16,   0, 
      0,   0,   0,   0,  49,   0, 
      0,  11, 114,   0,  16,   0, 
      1,   0,   0,   0,   2,  64, 
      0,   0,   0,   0, 128,  63, 
      0,   0, 128,  63,   0,   0, 
    128,  63,   0,   0,   0,   0, 
     70,  18,  16, 128, 129,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   8,  50,   0, 
     16,   0,   2,   0,   0,   0, 
    150,   5,  16,   0,   0,   0, 
      0,   0, 150,  21,  16, 128, 
    129,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,  10, 
     50,   0,  16,   0,   2,   0, 
      0,   0,  70,   0,  16,   0, 
      2,   0,   0,   0,   2,  64, 
      0,   0,   0,   0, 128, 191, 
      0,   0, 128, 191,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     54,   0,   0,   5,  66,   0, 
     16,   0,   2,   0,   0,   0, 
      1,  64,   0,   0, 255, 255, 
    255, 255,  54,   0,   0,   5, 
    130,   0,  16,   0,   1,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,  55,   0, 
      0,   9,  50,   0,  16,   0, 
      3,   0,   0,   0,  86,   5, 
     16,   0,   1,   0,   0,   0, 
    134,   0,  16,   0,   2,   0, 
      0,   0,  54,  15,  16,   0, 
      1,   0,   0,   0,  54,   0, 
      0,   5,  66,   0,  16,   0, 
      3,   0,   0,   0,  42,   0, 
     16,   0,   0,   0,   0,   0, 
     55,   0,   0,   9,  98,   0, 
     16,   0,   0,   0,   0,   0, 
    166,  10,  16,   0,   1,   0, 
      0,   0,  86,   6,  16,   0, 
      2,   0,   0,   0, 166,   9, 
     16,   0,   3,   0,   0,   0, 
     59,   0,   0,   5,  34,   0, 
     16,   0,   1,   0,   0,   0, 
     42,   0,  16,   0,   0,   0, 
      0,   0,  13,   0,   4,   3, 
     26,   0,  16,   0,   1,   0, 
      0,   0,   0,   0,   0,   8, 
     34,   0,  16,   0,   1,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  10,  16, 
     16, 128, 129,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   7,  34,   0,  16,   0, 
      1,   0,   0,   0,  26,   0, 
     16,   0,   1,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128, 191,  55,   0,   0,   9, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      1,   0,   0,   0,  26,   0, 
     16,   0,   1,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  16,   0,   0,   7, 
     18,   0,  16,   0,   1,   0, 
      0,   0,  70,  18,  16,   0, 
      1,   0,   0,   0,  70,  18, 
     16,   0,   1,   0,   0,   0, 
     75,   0,   0,   5,  18,   0, 
     16,   0,   1,   0,   0,   0, 
     10,   0,  16,   0,   1,   0, 
      0,   0,  56,   0,   0,   7, 
     18,   0,  16,   0,   1,   0, 
      0,   0,  10,   0,  16,   0, 
      1,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,  63, 
     55,   0,   0,   9, 130,  32, 
     16,   0,   0,   0,   0,   0, 
     42,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      1,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
     47,   0,   0,   5,  18,   0, 
     16,   0,   1,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  47,   0,   0,   5, 
     34,   0,  16,   0,   1,   0, 
      0,   0,  10,   0,  16,   0, 
      3,   0,   0,   0,  47,   0, 
      0,   5,  66,   0,  16,   0, 
      1,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     56,   0,   0,   8, 114,   0, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0, 166, 138,  32,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,  25,   0,   0,   5, 
    114,  32,  16,   0,   0,   0, 
      0,   0,  70,   2,  16,   0, 
      0,   0,   0,   0,  62,   0, 
      0,   1,  83,  84,  65,  84, 
    116,   0,   0,   0,  32,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,  19,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      6,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0
};