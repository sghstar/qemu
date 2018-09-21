/* Andes V5 instruction extension helpers */
DEF_HELPER_FLAGS_3(andes_v5_bfo_x, TCG_CALL_NO_RWG, tl, tl, tl, tl)
DEF_HELPER_FLAGS_3(andes_v5_fb_x, TCG_CALL_NO_RWG, tl, tl, tl, tl)
DEF_HELPER_2(andes_v5_flhw, void, env, i32)
DEF_HELPER_2(andes_v5_fshw, void, env, i32)
