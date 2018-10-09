#ifndef __ANDES_DSP_OPCODE__H__
#define __ANDES_DSP_OPCODE__H__
enum andes_dsp_simd_imm3u_id {

#define X(V) ((V) << 25) | (0b00 << 23) | (0b0 << 12)
#define Y(V) ((V) << 25) | (0b01 << 23) | (0b0 << 12)
#define Z(V) ((V) << 25) | (0b10 << 23) | (0b0 << 12)

  srai8 = X(0b0111100),
  srai8_u = Y(0b0111100),
  srli8 = X(0b0111101),
  srli8_u = Y(0b0111101),
  slli8 = X(0b0111110),
  kslli8 = Y(0b0111110),
  sclip8 = X(0b1000110),
  uclip8 = Z(0b1000110),
#undef X
#undef Y

};
enum andes_dsp_simd_imm4u_id {

#define X(V) ((V) << 25) | (0b0 << 24) | (0b0 << 12)
#define Y(V) ((V) << 25) | (0b1 << 24) | (0b0 << 12)
  srai16 = X(0b0111000),
  srai16_u = Y(0b0111000),
  srli16 = X(0b0111001),
  srli16_u = Y(0b0111001),
  slli16 = X(0b0111010),
  kslli16 = Y(0b0111010),
  sclip16 = X(0b1000010),
  uclip16 = Y(0b1000010),
#undef X
#undef Y
};
enum andes_dsp_simd_imm5u_000id {
#define X(V) ((V) << 25) | (0b0 << 12)
  sclip32 = X(0x72),
  uclip32 = X(0x7a),
  wexti = X(0x6f),
  kslliw = X(0b0011011),
#undef X
#define X(V) ((V) << 26) | (0b0 << 12)
  bitrevi = X(0b111010),
#undef X
};

enum andes_dsp_simd_imm5u_010id {
#define X(V) ((V) << 25) | (0b10 << 12)
  srai32 = X(0b0111000),
  srli32 = X(0b0111001),
  slli32 = X(0b1000010),
#undef X
};
enum andes_dsp_rr_id {

#define X(V) ((V) << 20)
  clz16 = X(0xae9),
  clo16 = X(0xaeb),
  kabs16 = X(0xad1),
  clrs16 = X(0xae8),
  kabs8 = X(0xad0),
  clrs8 = X(0xae0),
  clz8 = X(0xae1),
  clo8 = X(0xae3),
  sunpkd810 = X(0xac8),
  sunpkd820 = X(0xac9),
  sunpkd830 = X(0xaca),
  sunpkd831 = X(0xacb),
  sunpkd832 = X(0xad3),
  zunpkd810 = X(0xacc),
  zunpkd820 = X(0xacd),
  zunpkd830 = X(0xace),
  zunpkd831 = X(0b101011001111),
  zunpkd832 = X(0xad7),
  clrs32 = X(0xaf8),
  Dclz32 = X(0xaf9), // clz32's alias
  Dclo32 = X(0xafb), // clo32's alias
#undef X
};
enum andes_dsp_rrr000_id {
  bitrev = (0b1110011),
  add8 = (0x24),
  radd8 = (0x4),
  uradd8 = (0x14),
  kadd8 = (0xC),
  ukadd8 = (0x1C),

  sub8 = (0x25),
  rsub8 = (0x5),
  ursub8 = (0x15),
  ksub8 = (0xd),
  uksub8 = (0x1d),

  add16 = (0x20),
  sub16 = (0x21),
  cras16 = (0x22),
  crsa16 = (0x23),

  radd16 = (0x00),
  rsub16 = (0x01),
  rcras16 = (0x2),
  rcrsa16 = (0x3),

  uradd16 = (0x10),
  ursub16 = (0x11),
  urcras16 = (0x12),
  urcrsa16 = (0x13),

  kadd16 = (0x8),
  ksub16 = (0x9),
  kcras16 = (0xa),
  kcrsa16 = (0xb),

  ukadd16 = (0x18),
  uksub16 = (0x19),
  ukcras16 = (0x1a),
  ukcrsa16 = (0x1b),

  sra16 = (0x28),
  sra16_u = (0x30),

  srl16 = (0x29),
  srl16_u = (0x31),

  sll16 = (0x2a),
  ksll16 = (0x32),

  kslra16 = (0x2b),
  kslra16_u = (0x33),

  sra8 = (0x2c),
  sra8_u = (0x34),
  srl8 = (0x2d),
  srl8_u = (0x35),
  sll8 = (0x2e),
  ksll8 = (0x36),
  kslra8 = (0x2f),
  kslra8_u = (0x37),

  cmpeq16 = (0x26),
  scmplt16 = (0x06),
  scmple16 = (0x0e),
  ucmplt16 = (0x16),
  ucmple16 = (0x1e),

  cmpeq8 = (0x27),
  scmplt8 = (0x07),
  scmple8 = (0x0f),
  ucmplt8 = (0x17),
  ucmple8 = (0x1f),

  smin16 = (0x40),
  umin16 = (0x48),
  smax16 = (0x41),
  umax16 = (0x49),
  khm16 = (0x43),
  khmx16 = (0x4b),

  smin8 = (0x44),
  umin8 = (0x4c),
  smax8 = (0x45),
  umax8 = (0x4d),
  khm8 = (0x47),
  khmx8 = (0x4f),

  pbsad = (0x7e),
  pbsada = (0x7f),

  ave = (0b1110000),

  maxw = (0x79),
  minw = (0x78),
};
enum andes_dsp_rrr001_id {

#define X(V) ((V) << 25) | (0b1 << 12)
  pkbb16 = X(0x07),
  pkbt16 = X(0x0f),
  pktb16 = X(0x1f),
  pktt16 = X(0x17),
  smmul = X(0x20),
  smmul_u = X(0x28),
  kmmac = X(0x30),
  kmmac_u = X(0x38),
  kmmsb = X(0x21),
  kmmsb_u = X(0x29),
  kwmmul = X(0x31),
  kwmmul_u = X(0x39),

  smmwb = X(0x22),
  smmwb_u = X(0x2a),
  smmwt = X(0x32),
  smmwt_u = X(0x3a),
  kmmawb = X(0x23),
  kmmawb_u = X(0x2b),
  kmmawt = X(0x33),
  kmmawt_u = X(0x3b),
  kmmwb2 = X(0x47),
  kmmwb2_u = X(0x4f),
  kmmwt2 = X(0x57),
  kmmwt2_u = X(0x5f),
  kmmawb2 = X(0x67),
  kmmawb2_u = X(0x6f),
  kmmawt2 = X(0x77),
  kmmawt2_u = X(0x7f),

  smbb16 = X(0x04),
  smbt16 = X(0x0c),
  smtt16 = X(0x14),
  kmda = X(0x1c),
  kmxda = X(0x1d),
  smds = X(0x2c),
  smdrs = X(0x34),
  smxds = X(0x3c),
  kmabb = X(0x2d),
  kmabt = X(0x35),
  kmatt = X(0x3d),
  kmada = X(0x24),
  kmaxda = X(0x25),
  kmads = X(0x2e),
  kmadrs = X(0x36),
  kmaxds = X(0x3e),
  kmsda = X(0x26),
  kmsxda = X(0x27),

  smal = X(0x2f),
  add64 = X(0x60),
  radd64 = X(0x40),
  uradd64 = X(0x50),
  kadd64 = X(0x48),
  ukadd64 = X(0x58),

  sub64 = X(0x61),
  rsub64 = X(0x41),
  ursub64 = X(0x51),
  ksub64 = X(0x49),
  uksub64 = X(0x59),

  smar64 = X(0b1000010),
  smsr64 = X(0b1000011),
  umar64 = X(0b1010010),
  umsr64 = X(0b1010011),
  kmar64 = X(0b1001010),
  kmsr64 = X(0b1001011),
  ukmar64 = X(0b1011010),
  ukmsr64 = X(0b1011011),

  smalbb = X(0b1000100),
  smalbt = X(0b1001100),
  smaltt = X(0b1010100),
  smalda = X(0b1000110),
  smalxda = X(0b1001110),
  smalds = X(0b1000101),
  smaldrs = X(0b1001101),
  smalxds = X(0b1010101),
  smslda = X(0b1010110),
  smslxda = X(0b1011110),

  raddw = X(0b0010000),
  uraddw = X(0b0011000),
  rsubw = X(0b0010001),
  ursubw = X(0b0011001),

  kaddh = X(0x2),
  ksubh = X(0x3),
  khmbb = X(0x6),
  khmbt = X(0xe),
  khmtt = X(0x16),

  kaddw = X(0b0000000),
  ksubw = X(0b0000001),
  kdmbb = X(0b0000101),
  kdmbt = X(0b0001101),
  kdmtt = X(0b0010101),
  kslraw = X(0b0110111),
  kslraw_u = X(0b0111111),
  ksllw = X(0b0010011),
  kdmabb = X(0b1101001),
  kdmabt = X(0b1110001),
  kdmatt = X(0b1111001),

  maddr32 = X(0x62),

  sra_u = (0b0010010),
#undef X
};

enum andes_dsp_rrr010_id {

#define X(V) ((V) << 25) | (0b10 << 12)
  add32 = X(0b0100000),
  radd32 = X(0b0000000),
  uradd32 = X(0b0010000),

  sub32 = X(0b0100001),
  rsub32 = X(0b0000001),
  ursub32 = X(0b0010001),

  cras32 = X(0b0100010),
  rcras32 = X(0b0000010),
  urcras32 = X(0b0010010),
  crsa32 = X(0b0100011),
  rcrsa32 = X(0b0000011),
  urcrsa32 = X(0b0010011),

  sra32 = X(0b0101000),
#undef X
};
enum andes_dsp_rrr011_id {

#define X(V) ((V) << 25) | (0b11 << 12)
  stas16 = X(0b0100010),
  rstas16 = X(0b0000010),
  urstas16 = X(0b0010010),
  stsa16 = X(0b0100011),
  rstsa16 = X(0b0000011),
  urstsa16 = X(0b0010011),

  stas32 = X(0b0100000),
  rstas32 = X(0b0000000),
  urstas32 = X(0b0010000),

  stsa32 = X(0b0100001),
  rstsa32 = X(0b0000001),
  urstsa32 = X(0b0010001),

  srl32 = X(0b0101001),
  sll32 = X(0b0110010),

  smin32 = X(0b1001000),
  umin32 = X(0b1010000),
  smax32 = X(0b1001001),
  umax32 = X(0b1010001),

  smbb32 = X(0b0000100),
  smbt32 = X(0b0001100),
  smtt32 = X(0b0010100),

  smds32 = X(0b0101100),
  smdrs32 = X(0b0110100),
  smxds32 = X(0b0111100),
#undef X
};

#endif
