#include <functional>
#include <nds_intrinsic.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> /* time */
#include <vector>

#define TEST_RRR64 1
#define TEST_RRRR 1 // 1 fails
#define TEST_RRR 1
#define TEST_RR 1       // pass
#define TEST_RR_UIMM3 1 // pass
#define TEST_RR_UIMM4 1 // pass
#define TEST_RR_UIMM5 1

void Assert(unsigned x, unsigned y, const char *insn) {

    printf("%x:%s", x, insn);
    if (x == y) {
      fprintf(stderr, "\n");
      return;
    }
    else {
      printf(". Error, expect %x\n", y);
      exit(1);
    }
}
void run_rr(unsigned x, const char *insn) {
    printf("%s:%x\n", insn, x);
}
#if 1
void test_rr_uimm3() {
  std::vector<uint64_t> Val{0xffffffff, 0x00000000, 0x7fffffff, 0x12345678};
  std::vector<uint64_t> Val2{0x7, 0x0, 0x4};
#define CASE_ALIAS(NAME, ALIAS)                                                \
  for (auto in1 : Val) {                                                       \
    printf("%s: %llx op %llx", #NAME, in1, 7);                                 \
    run_rr(__nds__##ALIAS(in1, 0x7), "");                                      \
    printf("%s: %llx op %llx", #NAME, in1, 0);                                 \
    run_rr(__nds__##ALIAS(in1, 0x0), "");                                      \
    printf("%s: %llx op %llx", #NAME, in1, 4);                                 \
    run_rr(__nds__##ALIAS(in1, 0x4), "");                                      \
  }
#define CASE(NAME)                                                             \
  for (auto in1 : Val) {                                                       \
    printf("%s: %llx op %llx", #NAME, in1, 7);                                 \
    run_rr(__nds__##ALIAS(in1, 0x7), "");                                      \
    printf("%s: %llx op %llx", #NAME, in1, 0);                                 \
    run_rr(__nds__##ALIAS(in1, 0x0), "");                                      \
    printf("%s: %llx op %llx", #NAME, in1, 4);                                 \
    run_rr(__nds__##ALIAS(in1, 0x4), "");                                      \
  }
#define NOT_TESTABLE
#include "../../../target/riscv/andes_dsp_rr_uimm3.def"
#undef NOT_TESTABLE
#undef CASE
#undef CASE_ALIAS

}
void test_rr_uimm4() {

  std::vector<uint64_t> Val{0xffffffff, 0x00000000, 0x7fffffff, 0x12345678};
#define CASE_ALIAS(NAME, ALIAS)                                                \
  for (auto in1 : Val) {                                                       \
    printf("%s: %lx op %llx", #NAME, in1, 15);                                 \
    run_rr(__nds__##ALIAS(in1, 0x7), "");                                      \
    printf("%s: %lx op %llx", #NAME, in1, 0);                                  \
    run_rr(__nds__##ALIAS(in1, 0x0), "");                                      \
    printf("%s: %lx op %llx", #NAME, in1, 7);                                  \
    run_rr(__nds__##ALIAS(in1, 0x4), "");                                      \
  }
#define CASE(NAME)                                                             \
  for (auto in1 : Val) {                                                       \
    printf("%s: %lx op %llx", #NAME, in1, 15);                                 \
    run_rr(__nds__##ALIAS(in1, 0x7), "");                                      \
    printf("%s: %lx op %llx", #NAME, in1, 0);                                  \
    run_rr(__nds__##ALIAS(in1, 0x0), "");                                      \
    printf("%s: %lx op %llx", #NAME, in1, 7);                                  \
    run_rr(__nds__##ALIAS(in1, 0x4), "");                                      \
  }
#define NOT_TESTABLE
#include "../../../target/riscv/andes_dsp_rr_uimm4.def"
#undef NOT_TESTABLE
#undef CASE
#undef CASE_ALIAS

}
void test_rr_uimm5() {
  std::vector<uint64_t> Val{0xffffffff, 0x00000000, 0x7fffffff, 0x12345678};
#define CASE_ALIAS(NAME, ALIAS)                                                \
  for (auto in1 : Val) {                                                       \
    printf("%s: %lx op %llx", #NAME, in1, 31);                                 \
    run_rr(__nds__##ALIAS(in1, 0x7), "");                                      \
    printf("%s: %lx op %llx", #NAME, in1, 0);                                  \
    run_rr(__nds__##ALIAS(in1, 0x0), "");                                      \
    printf("%s: %lx op %llx", #NAME, in1, 15);                                 \
    run_rr(__nds__##ALIAS(in1, 0x4), "");                                      \
  }
#define CASE(NAME)                                                             \
  for (auto in1 : Val) {                                                       \
    printf("%s: %lx op %llx", #NAME, in1, 31);                                 \
    run_rr(__nds__##ALIAS(in1, 0x7), "");                                      \
    printf("%s: %lx op %llx", #NAME, in1, 0);                                  \
    run_rr(__nds__##ALIAS(in1, 0x0), "");                                      \
    printf("%s: %lx op %llx", #NAME, in1, 15);                                 \
    run_rr(__nds__##ALIAS(in1, 0x4), "");                                      \
  }
#define NOT_TESTABLE
#include "../../../target/riscv/andes_dsp_rr_uimm5.def"
#undef NOT_TESTABLE
#undef CASE
#undef CASE_ALIAS

}
#endif
void test_rr() {

  std::vector<uint64_t> Val{0xffffffff, 0x00000000, 0x7fff7fff, 0x12345678};
#define CASE_ALIAS(NAME, ALIAS)                                                \
  for (auto in1 : Val) {                                                       \
    printf("%s: op %llx", #NAME, in1);                                         \
    run_rr(__nds__##ALIAS(in1), "");                                           \
  }

#define CASE(NAME)                                                             \
  for (auto in1 : Val) {                                                       \
    printf("%s: op %llx", #NAME, in1);                                         \
    run_rr(__nds__##NAME(in1), "");                                            \
  }

#include "../../../target/riscv/andes_dsp_rr.def"
#define NOT_TESTABLE
#include "../../../target/riscv/andes_dsp_rr_psimd.def"
#undef NOT_TESTABLE
#undef CASE
#undef CASE_ALIAS
}

void test_rrr64() {
  std::vector<uint64_t> Val{0xffffffffffffffff, 0x0000000000000000,
                            0x7fffffffffffffff, 0x1234567887654321};
#define CASE_ALIAS(NAME, ALIAS)                                                \
  for (auto in1 : Val) {                                                       \
    for (auto in2 : Val) {                                                     \
      printf("%s: %lx op %lx", #NAME, in1, in2);                               \
      run_rr(__nds__##ALIAS(in1, in2), "");                                    \
    }                                                                          \
  }
#define CASE(NAME)                                                             \
  for (auto in1 : Val) {                                                       \
    for (auto in2 : Val) {                                                     \
      printf("%s: %lx op %lx", #NAME, in1, in2);                               \
      run_rr(__nds__##NAME(in1, in2), "");                                     \
    }                                                                          \
  }
#define NOT_TESTABLE
#include "../../../target/riscv/andes_dsp_rrr64.def"
#undef NOT_TESTABLE
#undef CASE
#undef CASE_ALIAS
}
#if 1
void test_rrr() {
  std::vector<uint64_t> Val{0xffffffff, 0x00000001, 0x7fff7fff, 0x12345678};
#define CASE_ALIAS(NAME, ALIAS)                                                \
  for (auto in1 : Val) {                                                       \
    printf("%s: %llx op %llx", #NAME, in1, 0xffffffff);                        \
    run_rr(__nds__##ALIAS(in1, 0xffffffff), "");                               \
    printf("%s: %llx op %llx", #NAME, in1, 0x00000001);                        \
    run_rr(__nds__##ALIAS(in1, 0x00000001), "");                               \
  }

#define CASE(NAME)                                                             \
  for (auto in1 : Val) {                                                       \
    printf("%s: %llx op %llx", #NAME, in1, 0xffffffff);                        \
    run_rr(__nds__##NAME(in1, 0xffffffff), "");                                \
    printf("%s: %llx op %llx", #NAME, in1, 0x00000001);                        \
    run_rr(__nds__##NAME(in1, 0x00000001), "");                                \
  }
#include "../../../target/riscv/andes_dsp_rrr.def"
#include "../../../target/riscv/andes_dsp_rrr_psimd.def"
#undef CASE
#undef CASE_ALIAS
  }

#endif
#define UM 0xffffffff
#define Um 0x0
#define SM 0x7fffffff
#define Sm 0x80000000
void test_rrrr() {
  std::vector<uint64_t> Val{UM, Um, SM, Sm, 0x12345678};
#define CASE_ALIAS(NAME, ALIAS)                                                \
  for (auto in1 : Val) {                                                       \
    for (auto in2 : Val) {                                                     \
      for (auto in3 : Val) {                                                   \
        printf("%s: %llx op %llx op %llx", #NAME, in1, in2, in3);              \
        run_rr(__nds__##ALIAS(in1, in2, in3), "");                             \
      }                                                                        \
    }                                                                          \
  }

#define CASE(NAME)                                                             \
  for (auto in1 : Val) {                                                       \
    for (auto in2 : Val) {                                                     \
      for (auto in3 : Val) {                                                   \
        printf("%s: %llx op %llx op %llx", #NAME, in1, in2, in3);              \
        run_rr(__nds__##NAME(in1, in2, in3), "");                              \
      }                                                                        \
    }                                                                          \
  }

#define NOT_TESTABLE
#include "../../../target/riscv/andes_dsp_rrrr_psimd.def"
#undef NOT_TESTABLE
#undef CASE

}
int main ()
{
#if 1
#if TEST_RR
  test_rr();
#endif
#if TEST_RRR
  test_rrr();
#endif
#if TEST_RRRR
  test_rrrr();
#endif
#if TEST_RR_UIMM3
  test_rr_uimm3();
#endif
#if TEST_RR_UIMM4
  test_rr_uimm4();
#endif
#if TEST_RR_UIMM5
  test_rr_uimm5();
#endif
#if TEST_RRR64
  test_rrr64();
#endif
  printf("wexti:  0xffff op 0xf");
  run_rr(__nds__wext(0xffff, 0xf), "");
  printf("wext:  0xfffff op 0x7777");
  run_rr(__nds__wext(0xffff, 0x7777), "");
  printf("bitrevi:  0xffff op 0xf");
  run_rr(__nds__bitrev(0xffff, 0xf), "");
#endif
  printf("bpick:  0xffff op 0x1233445 op 0x12321");
  run_rr(__nds__bpick(0xffff, 0x1233445, 0x12321), "");

  // Why intirnsic receives 3 arguments (check spec.)
  // printf("insb:  0xffff op 3");
  // run_rr(__nds__insb(0xffff, 3), "");
  return 0;
}
