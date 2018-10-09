/* Andes V5 instruction extension helpers */
DEF_HELPER_FLAGS_3(andes_v5_bfo_x, TCG_CALL_NO_RWG, tl, tl, tl, tl)
DEF_HELPER_FLAGS_3(andes_v5_fb_x, TCG_CALL_NO_RWG, tl, tl, tl, tl)
DEF_HELPER_2(andes_v5_flhw, void, env, i32)
DEF_HELPER_2(andes_v5_fshw, void, env, i32)

#define CASE(NAME)                                                             \
  DEF_HELPER_FLAGS_2(andes_dsp_##NAME, TCG_CALL_NO_RWG, tl, tl, tl)
#define CASE_ALIAS(NAME, ALIAS)                                                \
  DEF_HELPER_FLAGS_2(andes_dsp_##NAME, TCG_CALL_NO_RWG, tl, tl, tl)
#include "andes_dsp_rr_uimm3.def"
#include "andes_dsp_rr_uimm4.def"
#include "andes_dsp_rr_uimm5.def"
#undef CASE_ALIAS
#include "andes_dsp_rrr.def"
#include "andes_dsp_rrr_psimd.def"
#undef CASE
#define CASE(NAME) DEF_HELPER_FLAGS_1(andes_dsp_##NAME, TCG_CALL_NO_RWG, tl, tl)
#include "andes_dsp_rr.def"
#include "andes_dsp_rr_psimd.def"
#undef CASE
#define CASE(NAME)                                                             \
  DEF_HELPER_FLAGS_3(andes_dsp_##NAME, TCG_CALL_NO_RWG, tl, tl, tl, tl)
#include "andes_dsp_rrrr_psimd.def"
#undef CASE

#define CASE_ALIAS(NAME, ALIAS) CASE(NAME)
#if defined(TARGET_RISCV32)
#define CASE(NAME)                                                             \
  DEF_HELPER_FLAGS_2(andes_dsp_##NAME, TCG_CALL_NO_RWG, i64, i64, i64)
#else
#define CASE(NAME)                                                             \
  DEF_HELPER_FLAGS_2(andes_dsp_##NAME, TCG_CALL_NO_RWG, tl, tl, tl)
#endif
#include "andes_dsp_rrr64.def"
#undef CASE
#undef CASE_ALIAS
DEF_HELPER_FLAGS_2(andes_dsp_wexti, TCG_CALL_NO_RWG, tl, i64, tl)
DEF_HELPER_FLAGS_2(andes_dsp_wext, TCG_CALL_NO_RWG, tl, i64, tl)
DEF_HELPER_FLAGS_3(andes_dsp_bpick, TCG_CALL_NO_RWG, tl, tl, tl, tl)
DEF_HELPER_FLAGS_3(andes_dsp_insb, TCG_CALL_NO_RWG, tl, tl, tl, tl)
DEF_HELPER_FLAGS_3(andes_dsp_smar64, TCG_CALL_NO_RWG, i64, i64, tl, tl)
DEF_HELPER_FLAGS_3(andes_dsp_smsr64, TCG_CALL_NO_RWG, i64, i64, tl, tl)
DEF_HELPER_FLAGS_3(andes_dsp_umar64, TCG_CALL_NO_RWG, i64, i64, tl, tl)
DEF_HELPER_FLAGS_3(andes_dsp_umsr64, TCG_CALL_NO_RWG, i64, i64, tl, tl)
DEF_HELPER_FLAGS_3(andes_dsp_smalda, TCG_CALL_NO_RWG, i64, i64, tl, tl)
DEF_HELPER_FLAGS_3(andes_dsp_smalxda, TCG_CALL_NO_RWG, i64, i64, tl, tl)
DEF_HELPER_FLAGS_3(andes_dsp_smalds, TCG_CALL_NO_RWG, i64, i64, tl, tl)
DEF_HELPER_FLAGS_3(andes_dsp_smaldrs, TCG_CALL_NO_RWG, i64, i64, tl, tl)
DEF_HELPER_FLAGS_3(andes_dsp_smalxds, TCG_CALL_NO_RWG, i64, i64, tl, tl)
DEF_HELPER_FLAGS_3(andes_dsp_smslda, TCG_CALL_NO_RWG, i64, i64, tl, tl)
DEF_HELPER_FLAGS_3(andes_dsp_smslxda, TCG_CALL_NO_RWG, i64, i64, tl, tl)

#define CASE_ALIAS(NAME, ALIAS) CASE(NAME)
#if defined(TARGET_RISCV32)
#define CASE(NAME)                                                             \
  DEF_HELPER_FLAGS_3(andes_dsp_##NAME, TCG_CALL_NO_RWG, i64, i64, i64, i64)
#else
#define CASE(NAME)                                                             \
  DEF_HELPER_FLAGS_3(andes_dsp_##NAME, TCG_CALL_NO_RWG, tl, tl, tl, tl)
#endif
#include "andes_dsp_rrrr64.def"
#undef CASE
#undef CASE_ALIAS
DEF_HELPER_FLAGS_2(andes_dsp_bitrevi, TCG_CALL_NO_RWG, tl, tl, tl)
DEF_HELPER_FLAGS_2(andes_dsp_getdouble, TCG_CALL_NO_RWG, i64, i32, i32)
DEF_HELPER_FLAGS_1(andes_dsp_seteven, TCG_CALL_NO_RWG, i32, i64)
DEF_HELPER_FLAGS_1(andes_dsp_setodd, TCG_CALL_NO_RWG, i32, i64)
