#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 6.3.9600.16384
//
//
// Buffer Definitions: 
//
// cbuffer constbutter
// {
//
//   row_major float4x4 viewMatrix;     // Offset:    0 Size:    64
//   row_major float4x4 projMatrix;     // Offset:   64 Size:    64
//   float3 rotAxis;                    // Offset:  128 Size:    12 [unused]
//   float halfQuadWidth;               // Offset:  140 Size:     4 [unused]
//   float4 color;                      // Offset:  144 Size:    16
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim Slot Elements
// ------------------------------ ---------- ------- ----------- ---- --------
// constbutter                       cbuffer      NA          NA    0        1
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// POSITION                 0   xyz         0     NONE   float   xyz 
// WIDTH                    0      w        0     NONE   float      w
// NORMAL                   0   xyz         1     NONE   float   xyz 
// WIDTH                    1      w        1     NONE   float      w
// ROTATIONAXIS             0   xyz         2     NONE   float   xyz 
// LIFETIME                 0      w        2     NONE   float      w
// POSITION                 1   xyz         3     NONE   float   xyz 
// NORMAL                   1   xyz         4     NONE   float   xyz 
// ROTATIONAXIS             1   xyz         5     NONE   float   xyz 
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float   xyzw
// TEXCOORD                 0   xy          1     NONE   float   xy  
// COLOR                    0   xyzw        2     NONE   float   xyzw
//
gs_4_0
dcl_constantbuffer cb0[10], immediateIndexed
dcl_input v[1][0].xyz
dcl_input v[1][0].w
dcl_input v[1][1].xyz
dcl_input v[1][1].w
dcl_input v[1][2].xyz
dcl_input v[1][2].w
dcl_input v[1][3].xyz
dcl_input v[1][4].xyz
dcl_input v[1][5].xyz
dcl_temps 8
dcl_inputprimitive point 
dcl_outputtopology trianglestrip 
dcl_output_siv o0.xyzw, position
dcl_output o1.xy
dcl_output o2.xyzw
dcl_maxout 6
mul r0.xyz, v[0][4].yzxy, v[0][5].zxyz
mad r0.xyz, v[0][5].yzxy, v[0][4].zxyz, -r0.xyzx
dp3 r0.w, r0.xyzx, r0.xyzx
rsq r0.w, r0.w
mul r0.xyz, r0.wwww, r0.xyzx
mad r1.xyz, -r0.xyzx, v[0][1].wwww, v[0][3].xyzx
mad r0.xyz, r0.xyzx, v[0][1].wwww, v[0][3].xyzx
mul r2.xyzw, cb0[1].yyyy, cb0[5].xyzw
mad r2.xyzw, cb0[1].xxxx, cb0[4].xyzw, r2.xyzw
mad r2.xyzw, cb0[1].zzzz, cb0[6].xyzw, r2.xyzw
mad r2.xyzw, cb0[1].wwww, cb0[7].xyzw, r2.xyzw
mul r3.xyzw, r1.yyyy, r2.xyzw
mul r4.xyzw, cb0[0].yyyy, cb0[5].xyzw
mad r4.xyzw, cb0[0].xxxx, cb0[4].xyzw, r4.xyzw
mad r4.xyzw, cb0[0].zzzz, cb0[6].xyzw, r4.xyzw
mad r4.xyzw, cb0[0].wwww, cb0[7].xyzw, r4.xyzw
mad r3.xyzw, r1.xxxx, r4.xyzw, r3.xyzw
mul r5.xyzw, cb0[2].yyyy, cb0[5].xyzw
mad r5.xyzw, cb0[2].xxxx, cb0[4].xyzw, r5.xyzw
mad r5.xyzw, cb0[2].zzzz, cb0[6].xyzw, r5.xyzw
mad r5.xyzw, cb0[2].wwww, cb0[7].xyzw, r5.xyzw
mad r1.xyzw, r1.zzzz, r5.xyzw, r3.xyzw
mul r3.xyzw, cb0[3].yyyy, cb0[5].xyzw
mad r3.xyzw, cb0[3].xxxx, cb0[4].xyzw, r3.xyzw
mad r3.xyzw, cb0[3].zzzz, cb0[6].xyzw, r3.xyzw
mad r3.xyzw, cb0[3].wwww, cb0[7].xyzw, r3.xyzw
add r1.xyzw, r1.xyzw, r3.xyzw
mov o0.xyzw, r1.xyzw
mov o1.xy, l(0,0,0,0)
mov o2.xyz, cb0[9].xyzx
mov o2.w, v[0][2].w
emit 
mul r1.xyzw, r0.yyyy, r2.xyzw
mad r1.xyzw, r0.xxxx, r4.xyzw, r1.xyzw
mad r0.xyzw, r0.zzzz, r5.xyzw, r1.xyzw
add r0.xyzw, r3.xyzw, r0.xyzw
mov o0.xyzw, r0.xyzw
mov o1.xy, l(1.000000,0,0,0)
mov o2.xyz, cb0[9].xyzx
mov o2.w, v[0][2].w
emit 
add r1.x, l(0.500000), -v[0][2].w
add r1.x, r1.x, r1.x
add r1.yzw, -v[0][0].xxyz, v[0][3].xxyz
mad r1.xyz, r1.xxxx, r1.yzwy, v[0][0].xyzx
lt r1.w, v[0][2].w, l(0.500000)
movc r1.xyz, r1.wwww, r1.xyzx, v[0][0].xyzx
mul r6.xyz, v[0][1].yzxy, v[0][2].zxyz
mad r6.xyz, v[0][2].yzxy, v[0][1].zxyz, -r6.xyzx
dp3 r1.w, r6.xyzx, r6.xyzx
rsq r1.w, r1.w
mul r6.xyz, r1.wwww, r6.xyzx
mad r7.xyz, -r6.xyzx, v[0][0].wwww, r1.xyzx
mad r1.xyz, r6.xyzx, v[0][0].wwww, r1.xyzx
mul r6.xyzw, r2.xyzw, r7.yyyy
mul r2.xyzw, r2.xyzw, r1.yyyy
mad r2.xyzw, r1.xxxx, r4.xyzw, r2.xyzw
mad r4.xyzw, r7.xxxx, r4.xyzw, r6.xyzw
mad r4.xyzw, r7.zzzz, r5.xyzw, r4.xyzw
mad r1.xyzw, r1.zzzz, r5.xyzw, r2.xyzw
add r1.xyzw, r3.xyzw, r1.xyzw
add r2.xyzw, r3.xyzw, r4.xyzw
mov o0.xyzw, r2.xyzw
mov o1.xy, l(0,1.000000,0,0)
mov o2.xyz, cb0[9].xyzx
mov o2.w, v[0][2].w
emit 
cut 
mov o0.xyzw, r2.xyzw
mov o1.xy, l(0,1.000000,0,0)
mov o2.xyz, cb0[9].xyzx
mov o2.w, v[0][2].w
emit 
mov o0.xyzw, r0.xyzw
mov o1.xy, l(1.000000,0,0,0)
mov o2.xyz, cb0[9].xyzx
mov o2.w, v[0][2].w
emit 
mov o0.xyzw, r1.xyzw
mov o1.xy, l(1.000000,1.000000,0,0)
mov o2.xyz, cb0[9].xyzx
mov o2.w, v[0][2].w
emit 
cut 
ret 
// Approximately 85 instruction slots used
#endif

const BYTE Ribbon_GS[] =
{
     68,  88,  66,  67,   8, 170, 
    219, 211, 214,  62, 148, 235, 
    121, 178,  64, 235, 160, 123, 
    169,  40,   1,   0,   0,   0, 
    152,  14,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
    192,   1,   0,   0, 212,   2, 
      0,   0,  72,   3,   0,   0, 
     28,  14,   0,   0,  82,  68, 
     69,  70, 132,   1,   0,   0, 
      1,   0,   0,   0,  72,   0, 
      0,   0,   1,   0,   0,   0, 
     28,   0,   0,   0,   0,   4, 
     83,  71,   0,   1,   0,   0, 
     80,   1,   0,   0,  60,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     99, 111, 110, 115, 116,  98, 
    117, 116, 116, 101, 114,   0, 
     60,   0,   0,   0,   5,   0, 
      0,   0,  96,   0,   0,   0, 
    160,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    216,   0,   0,   0,   0,   0, 
      0,   0,  64,   0,   0,   0, 
      2,   0,   0,   0, 228,   0, 
      0,   0,   0,   0,   0,   0, 
    244,   0,   0,   0,  64,   0, 
      0,   0,  64,   0,   0,   0, 
      2,   0,   0,   0, 228,   0, 
      0,   0,   0,   0,   0,   0, 
    255,   0,   0,   0, 128,   0, 
      0,   0,  12,   0,   0,   0, 
      0,   0,   0,   0,   8,   1, 
      0,   0,   0,   0,   0,   0, 
     24,   1,   0,   0, 140,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,  40,   1, 
      0,   0,   0,   0,   0,   0, 
     56,   1,   0,   0, 144,   0, 
      0,   0,  16,   0,   0,   0, 
      2,   0,   0,   0,  64,   1, 
      0,   0,   0,   0,   0,   0, 
    118, 105, 101, 119,  77,  97, 
    116, 114, 105, 120,   0, 171, 
      2,   0,   3,   0,   4,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 112, 114, 
    111, 106,  77,  97, 116, 114, 
    105, 120,   0, 114, 111, 116, 
     65, 120, 105, 115,   0, 171, 
      1,   0,   3,   0,   1,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 104,  97, 
    108, 102,  81, 117,  97, 100, 
     87, 105, 100, 116, 104,   0, 
    171, 171,   0,   0,   3,   0, 
      1,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     99, 111, 108, 111, 114,   0, 
    171, 171,   1,   0,   3,   0, 
      1,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     77, 105,  99, 114, 111, 115, 
    111, 102, 116,  32,  40,  82, 
     41,  32,  72,  76,  83,  76, 
     32,  83, 104,  97, 100, 101, 
    114,  32,  67, 111, 109, 112, 
    105, 108, 101, 114,  32,  54, 
     46,  51,  46,  57,  54,  48, 
     48,  46,  49,  54,  51,  56, 
     52,   0, 171, 171,  73,  83, 
     71,  78,  12,   1,   0,   0, 
      9,   0,   0,   0,   8,   0, 
      0,   0, 224,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   7,   7, 
      0,   0, 233,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   8,   8, 
      0,   0, 239,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,   7,   7, 
      0,   0, 233,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,   8,   8, 
      0,   0, 246,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      2,   0,   0,   0,   7,   7, 
      0,   0,   3,   1,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      2,   0,   0,   0,   8,   8, 
      0,   0, 224,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      3,   0,   0,   0,   7,   7, 
      0,   0, 239,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      4,   0,   0,   0,   7,   7, 
      0,   0, 246,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      5,   0,   0,   0,   7,   7, 
      0,   0,  80,  79,  83,  73, 
     84,  73,  79,  78,   0,  87, 
     73,  68,  84,  72,   0,  78, 
     79,  82,  77,  65,  76,   0, 
     82,  79,  84,  65,  84,  73, 
     79,  78,  65,  88,  73,  83, 
      0,  76,  73,  70,  69,  84, 
     73,  77,  69,   0,  79,  83, 
     71,  78, 108,   0,   0,   0, 
      3,   0,   0,   0,   8,   0, 
      0,   0,  80,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,  15,   0, 
      0,   0,  92,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,   3,  12, 
      0,   0, 101,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      2,   0,   0,   0,  15,   0, 
      0,   0,  83,  86,  95,  80, 
     79,  83,  73,  84,  73,  79, 
     78,   0,  84,  69,  88,  67, 
     79,  79,  82,  68,   0,  67, 
     79,  76,  79,  82,   0, 171, 
     83,  72,  68,  82, 204,  10, 
      0,   0,  64,   0,   2,   0, 
    179,   2,   0,   0,  89,   0, 
      0,   4,  70, 142,  32,   0, 
      0,   0,   0,   0,  10,   0, 
      0,   0,  95,   0,   0,   4, 
    114,  16,  32,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
     95,   0,   0,   4, 130,  16, 
     32,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,  95,   0, 
      0,   4, 114,  16,  32,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  95,   0,   0,   4, 
    130,  16,  32,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     95,   0,   0,   4, 114,  16, 
     32,   0,   1,   0,   0,   0, 
      2,   0,   0,   0,  95,   0, 
      0,   4, 130,  16,  32,   0, 
      1,   0,   0,   0,   2,   0, 
      0,   0,  95,   0,   0,   4, 
    114,  16,  32,   0,   1,   0, 
      0,   0,   3,   0,   0,   0, 
     95,   0,   0,   4, 114,  16, 
     32,   0,   1,   0,   0,   0, 
      4,   0,   0,   0,  95,   0, 
      0,   4, 114,  16,  32,   0, 
      1,   0,   0,   0,   5,   0, 
      0,   0, 104,   0,   0,   2, 
      8,   0,   0,   0,  93,   8, 
      0,   1,  92,  40,   0,   1, 
    103,   0,   0,   4, 242,  32, 
     16,   0,   0,   0,   0,   0, 
      1,   0,   0,   0, 101,   0, 
      0,   3,  50,  32,  16,   0, 
      1,   0,   0,   0, 101,   0, 
      0,   3, 242,  32,  16,   0, 
      2,   0,   0,   0,  94,   0, 
      0,   2,   6,   0,   0,   0, 
     56,   0,   0,   9, 114,   0, 
     16,   0,   0,   0,   0,   0, 
    150,  20,  32,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
     38,  25,  32,   0,   0,   0, 
      0,   0,   5,   0,   0,   0, 
     50,   0,   0,  12, 114,   0, 
     16,   0,   0,   0,   0,   0, 
    150,  20,  32,   0,   0,   0, 
      0,   0,   5,   0,   0,   0, 
     38,  25,  32,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
     70,   2,  16, 128,  65,   0, 
      0,   0,   0,   0,   0,   0, 
     16,   0,   0,   7, 130,   0, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,  70,   2,  16,   0, 
      0,   0,   0,   0,  68,   0, 
      0,   5, 130,   0,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
     56,   0,   0,   7, 114,   0, 
     16,   0,   0,   0,   0,   0, 
    246,  15,  16,   0,   0,   0, 
      0,   0,  70,   2,  16,   0, 
      0,   0,   0,   0,  50,   0, 
      0,  12, 114,   0,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16, 128,  65,   0,   0,   0, 
      0,   0,   0,   0, 246,  31, 
     32,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,  70,  18, 
     32,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,  50,   0, 
      0,  11, 114,   0,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   0,   0,   0,   0, 
    246,  31,  32,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     70,  18,  32,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
     56,   0,   0,   9, 242,   0, 
     16,   0,   2,   0,   0,   0, 
     86, 133,  32,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   5,   0,   0,   0, 
     50,   0,   0,  11, 242,   0, 
     16,   0,   2,   0,   0,   0, 
      6, 128,  32,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
     70,  14,  16,   0,   2,   0, 
      0,   0,  50,   0,   0,  11, 
    242,   0,  16,   0,   2,   0, 
      0,   0, 166, 138,  32,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   6,   0, 
      0,   0,  70,  14,  16,   0, 
      2,   0,   0,   0,  50,   0, 
      0,  11, 242,   0,  16,   0, 
      2,   0,   0,   0, 246, 143, 
     32,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      7,   0,   0,   0,  70,  14, 
     16,   0,   2,   0,   0,   0, 
     56,   0,   0,   7, 242,   0, 
     16,   0,   3,   0,   0,   0, 
     86,   5,  16,   0,   1,   0, 
      0,   0,  70,  14,  16,   0, 
      2,   0,   0,   0,  56,   0, 
      0,   9, 242,   0,  16,   0, 
      4,   0,   0,   0,  86, 133, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      5,   0,   0,   0,  50,   0, 
      0,  11, 242,   0,  16,   0, 
      4,   0,   0,   0,   6, 128, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      4,   0,   0,   0,  70,  14, 
     16,   0,   4,   0,   0,   0, 
     50,   0,   0,  11, 242,   0, 
     16,   0,   4,   0,   0,   0, 
    166, 138,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   6,   0,   0,   0, 
     70,  14,  16,   0,   4,   0, 
      0,   0,  50,   0,   0,  11, 
    242,   0,  16,   0,   4,   0, 
      0,   0, 246, 143,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   7,   0, 
      0,   0,  70,  14,  16,   0, 
      4,   0,   0,   0,  50,   0, 
      0,   9, 242,   0,  16,   0, 
      3,   0,   0,   0,   6,   0, 
     16,   0,   1,   0,   0,   0, 
     70,  14,  16,   0,   4,   0, 
      0,   0,  70,  14,  16,   0, 
      3,   0,   0,   0,  56,   0, 
      0,   9, 242,   0,  16,   0, 
      5,   0,   0,   0,  86, 133, 
     32,   0,   0,   0,   0,   0, 
      2,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      5,   0,   0,   0,  50,   0, 
      0,  11, 242,   0,  16,   0, 
      5,   0,   0,   0,   6, 128, 
     32,   0,   0,   0,   0,   0, 
      2,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      4,   0,   0,   0,  70,  14, 
     16,   0,   5,   0,   0,   0, 
     50,   0,   0,  11, 242,   0, 
     16,   0,   5,   0,   0,   0, 
    166, 138,  32,   0,   0,   0, 
      0,   0,   2,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   6,   0,   0,   0, 
     70,  14,  16,   0,   5,   0, 
      0,   0,  50,   0,   0,  11, 
    242,   0,  16,   0,   5,   0, 
      0,   0, 246, 143,  32,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   7,   0, 
      0,   0,  70,  14,  16,   0, 
      5,   0,   0,   0,  50,   0, 
      0,   9, 242,   0,  16,   0, 
      1,   0,   0,   0, 166,  10, 
     16,   0,   1,   0,   0,   0, 
     70,  14,  16,   0,   5,   0, 
      0,   0,  70,  14,  16,   0, 
      3,   0,   0,   0,  56,   0, 
      0,   9, 242,   0,  16,   0, 
      3,   0,   0,   0,  86, 133, 
     32,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      5,   0,   0,   0,  50,   0, 
      0,  11, 242,   0,  16,   0, 
      3,   0,   0,   0,   6, 128, 
     32,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      4,   0,   0,   0,  70,  14, 
     16,   0,   3,   0,   0,   0, 
     50,   0,   0,  11, 242,   0, 
     16,   0,   3,   0,   0,   0, 
    166, 138,  32,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   6,   0,   0,   0, 
     70,  14,  16,   0,   3,   0, 
      0,   0,  50,   0,   0,  11, 
    242,   0,  16,   0,   3,   0, 
      0,   0, 246, 143,  32,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   7,   0, 
      0,   0,  70,  14,  16,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   7, 242,   0,  16,   0, 
      1,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
     70,  14,  16,   0,   3,   0, 
      0,   0,  54,   0,   0,   5, 
    242,  32,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  54,   0, 
      0,   8,  50,  32,  16,   0, 
      1,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     54,   0,   0,   6, 114,  32, 
     16,   0,   2,   0,   0,   0, 
     70, 130,  32,   0,   0,   0, 
      0,   0,   9,   0,   0,   0, 
     54,   0,   0,   6, 130,  32, 
     16,   0,   2,   0,   0,   0, 
     58,  16,  32,   0,   0,   0, 
      0,   0,   2,   0,   0,   0, 
     19,   0,   0,   1,  56,   0, 
      0,   7, 242,   0,  16,   0, 
      1,   0,   0,   0,  86,   5, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   2,   0, 
      0,   0,  50,   0,   0,   9, 
    242,   0,  16,   0,   1,   0, 
      0,   0,   6,   0,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   4,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,  50,   0,   0,   9, 
    242,   0,  16,   0,   0,   0, 
      0,   0, 166,  10,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   5,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,   0,   0,   0,   7, 
    242,   0,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      3,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     54,   0,   0,   5, 242,  32, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  54,   0,   0,   8, 
     50,  32,  16,   0,   1,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0, 128,  63,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   6, 114,  32,  16,   0, 
      2,   0,   0,   0,  70, 130, 
     32,   0,   0,   0,   0,   0, 
      9,   0,   0,   0,  54,   0, 
      0,   6, 130,  32,  16,   0, 
      2,   0,   0,   0,  58,  16, 
     32,   0,   0,   0,   0,   0, 
      2,   0,   0,   0,  19,   0, 
      0,   1,   0,   0,   0,   9, 
     18,   0,  16,   0,   1,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,  63,  58,  16, 
     32, 128,  65,   0,   0,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   7, 
     18,   0,  16,   0,   1,   0, 
      0,   0,  10,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   1,   0,   0,   0, 
      0,   0,   0,  10, 226,   0, 
     16,   0,   1,   0,   0,   0, 
      6,  25,  32, 128,  65,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   6,  25, 
     32,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,  50,   0, 
      0,  10, 114,   0,  16,   0, 
      1,   0,   0,   0,   6,   0, 
     16,   0,   1,   0,   0,   0, 
    150,   7,  16,   0,   1,   0, 
      0,   0,  70,  18,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  49,   0,   0,   8, 
    130,   0,  16,   0,   1,   0, 
      0,   0,  58,  16,  32,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,  63,  55,   0, 
      0,  10, 114,   0,  16,   0, 
      1,   0,   0,   0, 246,  15, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  70,  18,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  56,   0,   0,   9, 
    114,   0,  16,   0,   6,   0, 
      0,   0, 150,  20,  32,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  38,  25,  32,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,  50,   0,   0,  12, 
    114,   0,  16,   0,   6,   0, 
      0,   0, 150,  20,  32,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,  38,  25,  32,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  70,   2,  16, 128, 
     65,   0,   0,   0,   6,   0, 
      0,   0,  16,   0,   0,   7, 
    130,   0,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      6,   0,   0,   0,  70,   2, 
     16,   0,   6,   0,   0,   0, 
     68,   0,   0,   5, 130,   0, 
     16,   0,   1,   0,   0,   0, 
     58,   0,  16,   0,   1,   0, 
      0,   0,  56,   0,   0,   7, 
    114,   0,  16,   0,   6,   0, 
      0,   0, 246,  15,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   6,   0,   0,   0, 
     50,   0,   0,  11, 114,   0, 
     16,   0,   7,   0,   0,   0, 
     70,   2,  16, 128,  65,   0, 
      0,   0,   6,   0,   0,   0, 
    246,  31,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  50,   0,   0,  10, 
    114,   0,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      6,   0,   0,   0, 246,  31, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     56,   0,   0,   7, 242,   0, 
     16,   0,   6,   0,   0,   0, 
     70,  14,  16,   0,   2,   0, 
      0,   0,  86,   5,  16,   0, 
      7,   0,   0,   0,  56,   0, 
      0,   7, 242,   0,  16,   0, 
      2,   0,   0,   0,  70,  14, 
     16,   0,   2,   0,   0,   0, 
     86,   5,  16,   0,   1,   0, 
      0,   0,  50,   0,   0,   9, 
    242,   0,  16,   0,   2,   0, 
      0,   0,   6,   0,  16,   0, 
      1,   0,   0,   0,  70,  14, 
     16,   0,   4,   0,   0,   0, 
     70,  14,  16,   0,   2,   0, 
      0,   0,  50,   0,   0,   9, 
    242,   0,  16,   0,   4,   0, 
      0,   0,   6,   0,  16,   0, 
      7,   0,   0,   0,  70,  14, 
     16,   0,   4,   0,   0,   0, 
     70,  14,  16,   0,   6,   0, 
      0,   0,  50,   0,   0,   9, 
    242,   0,  16,   0,   4,   0, 
      0,   0, 166,  10,  16,   0, 
      7,   0,   0,   0,  70,  14, 
     16,   0,   5,   0,   0,   0, 
     70,  14,  16,   0,   4,   0, 
      0,   0,  50,   0,   0,   9, 
    242,   0,  16,   0,   1,   0, 
      0,   0, 166,  10,  16,   0, 
      1,   0,   0,   0,  70,  14, 
     16,   0,   5,   0,   0,   0, 
     70,  14,  16,   0,   2,   0, 
      0,   0,   0,   0,   0,   7, 
    242,   0,  16,   0,   1,   0, 
      0,   0,  70,  14,  16,   0, 
      3,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
      0,   0,   0,   7, 242,   0, 
     16,   0,   2,   0,   0,   0, 
     70,  14,  16,   0,   3,   0, 
      0,   0,  70,  14,  16,   0, 
      4,   0,   0,   0,  54,   0, 
      0,   5, 242,  32,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   2,   0,   0,   0, 
     54,   0,   0,   8,  50,  32, 
     16,   0,   1,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0, 128,  63, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  54,   0,   0,   6, 
    114,  32,  16,   0,   2,   0, 
      0,   0,  70, 130,  32,   0, 
      0,   0,   0,   0,   9,   0, 
      0,   0,  54,   0,   0,   6, 
    130,  32,  16,   0,   2,   0, 
      0,   0,  58,  16,  32,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,  19,   0,   0,   1, 
      9,   0,   0,   1,  54,   0, 
      0,   5, 242,  32,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   2,   0,   0,   0, 
     54,   0,   0,   8,  50,  32, 
     16,   0,   1,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0, 128,  63, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  54,   0,   0,   6, 
    114,  32,  16,   0,   2,   0, 
      0,   0,  70, 130,  32,   0, 
      0,   0,   0,   0,   9,   0, 
      0,   0,  54,   0,   0,   6, 
    130,  32,  16,   0,   2,   0, 
      0,   0,  58,  16,  32,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,  19,   0,   0,   1, 
     54,   0,   0,   5, 242,  32, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  54,   0,   0,   8, 
     50,  32,  16,   0,   1,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0, 128,  63,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   6, 114,  32,  16,   0, 
      2,   0,   0,   0,  70, 130, 
     32,   0,   0,   0,   0,   0, 
      9,   0,   0,   0,  54,   0, 
      0,   6, 130,  32,  16,   0, 
      2,   0,   0,   0,  58,  16, 
     32,   0,   0,   0,   0,   0, 
      2,   0,   0,   0,  19,   0, 
      0,   1,  54,   0,   0,   5, 
    242,  32,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  54,   0, 
      0,   8,  50,  32,  16,   0, 
      1,   0,   0,   0,   2,  64, 
      0,   0,   0,   0, 128,  63, 
      0,   0, 128,  63,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     54,   0,   0,   6, 114,  32, 
     16,   0,   2,   0,   0,   0, 
     70, 130,  32,   0,   0,   0, 
      0,   0,   9,   0,   0,   0, 
     54,   0,   0,   6, 130,  32, 
     16,   0,   2,   0,   0,   0, 
     58,  16,  32,   0,   0,   0, 
      0,   0,   2,   0,   0,   0, 
     19,   0,   0,   1,   9,   0, 
      0,   1,  62,   0,   0,   1, 
     83,  84,  65,  84, 116,   0, 
      0,   0,  85,   0,   0,   0, 
      8,   0,   0,   0,   0,   0, 
      0,   0,  12,   0,   0,   0, 
     51,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   2,   0,   0,   0, 
      6,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   5,   0,   0,   0, 
      6,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0
};