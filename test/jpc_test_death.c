#include "jpc_test_death.h"

START_DEATH

TEST(Memory, Object) {
  double *_d = null;
  double *d2;

  EXPECT_DEATH(newo((char **)null), "Pointer to pointer null");
  EXPECT_DEATH(deleteo((char **)null), "Pointer to pointer null");

  newo(&_d);
  EXPECT_DEATH(newo(&_d), "Pointer not empty");
  d2 = _d;
  deleteo(&_d);
  EXPECT_DEATH(newo(&d2), "Pointer not empty");
}

TEST(Memory, Array) {
  int *_a = null;

  new(&_a, 10);
  EXPECT_DEATH(new(&_a, 3), "Pointer not empty");
  EXPECT_DEATH(delete(&_a, 3, null), "Destructor null");
  EXPECT_DEATH(deletev((char **)null), "Pointer to pointer null");
  EXPECT_DEATH(renewv((char **)null, 3, 4), "Pointer to pointer null");
  EXPECT_DEATH(renewup(&_a, 3, 2), "Decrease from 3 to 2!");
  deletev(&_a);
  EXPECT_DEATH(renewv(&_a, 3, 4), "Pointer empty");
}

TEST(Memory, Tensors) {
  EXPECT_DEATH(deletem((char ***)null, 3, 2, null), "Pointer to pointer null");
  EXPECT_DEATH(deletemv((char ***)null, 3), "Pointer to pointer null");
  EXPECT_DEATH(
      deletet3((char ****)null, 3, 2, 5, null), "Pointer to pointer null");
  EXPECT_DEATH(
      deletet4((char *****)null, 3, 2, 5, 3, null), "Pointer to pointer null");
}

TEST(Memory, Donate) {
  double *_src = null;
  double *_dst = null;
  double *local;

  EXPECT_DEATH(donate((char **)null, &_src), "Pointer to source pointer null");
  EXPECT_DEATH(donate(&_src, &_dst), "Source pointer empty");

  newo(&_src);
  EXPECT_DEATH(
      donate(&_src, (char **)null),
      "Pointer to destination pointer null");
  local = _src;
  donate(&_src, &_dst);
  EXPECT_DEATH(donate(&_dst, &local), "Destination pointer not empty");
  deleteo(&_dst);
}

TEST(Memory, Copy) {
  byte a[8];
  char *_e = null;

  string_new_f(
      &_e,
      "Attempting to copy memory between overlapping ranges: "
      "src [%p to %p] dst [%p to %p]",
      a,
      a,
      a,
      a);
  string_renew_add_separators(&_e);
  EXPECT_DEATH(copy_memory(a, a, 1), _e);
  string_delete(&_e);

  string_new_f(
      &_e,
      "Attempting to copy memory between overlapping ranges: "
      "src [%p to %p] dst [%p to %p]",
      a,
      a + 1,
      a,
      a + 1);
  string_renew_add_separators(&_e);
  EXPECT_DEATH(copy_memory(a, a, 2), _e);
  string_delete(&_e);

  string_new_f(
      &_e,
      "Attempting to copy memory between overlapping ranges: "
      "src [%p to %p] dst [%p to %p]",
      a,
      a + 2,
      a,
      a + 2);
  string_renew_add_separators(&_e);
  EXPECT_DEATH(copy_memory(a, a, 3), _e);
  string_delete(&_e);

  string_new_f(
      &_e,
      "Attempting to copy memory between overlapping ranges: "
      "src [%p to %p] dst [%p to %p]",
      a,
      a + 1,
      a + 1,
      a + 2);
  string_renew_add_separators(&_e);
  EXPECT_DEATH(copy_memory(a, a + 1, 2), _e);
  string_delete(&_e);

  string_new_f(
      &_e,
      "Attempting to copy memory between overlapping ranges: "
      "src [%p to %p] dst [%p to %p]",
      a,
      a + 2,
      a + 1,
      a + 3);
  string_renew_add_separators(&_e);
  EXPECT_DEATH(copy_memory(a, a + 1, 3), _e);
  string_delete(&_e);

  string_new_f(
      &_e,
      "Attempting to copy memory between overlapping ranges: "
      "src [%p to %p] dst [%p to %p]",
      a,
      a + 2,
      a + 2,
      a + 4);
  string_renew_add_separators(&_e);
  EXPECT_DEATH(copy_memory(a, a + 2, 3), _e);
  string_delete(&_e);

  string_new_f(
      &_e,
      "Attempting to copy memory between overlapping ranges: "
      "src [%p to %p] dst [%p to %p]",
      a + 1,
      a + 2,
      a,
      a + 1);
  string_renew_add_separators(&_e);
  EXPECT_DEATH(copy_memory(a + 1, a, 2), _e);
  string_delete(&_e);

  string_new_f(
      &_e,
      "Attempting to copy memory between overlapping ranges: "
      "src [%p to %p] dst [%p to %p]",
      a + 1,
      a + 3,
      a,
      a + 2);
  string_renew_add_separators(&_e);
  EXPECT_DEATH(copy_memory(a + 1, a, 3), _e);
  string_delete(&_e);

  string_new_f(
      &_e,
      "Attempting to copy memory between overlapping ranges: "
      "src [%p to %p] dst [%p to %p]",
      a + 2,
      a + 4,
      a,
      a + 2);
  string_renew_add_separators(&_e);
  EXPECT_DEATH(copy_memory(a + 2, a, 3), _e);
  string_delete(&_e);
}

TEST(Memory, Move) {
  byte a[8];
  char *_e = null;

  string_new_f(
      &_e,
      "Attempting to copy memory between overlapping ranges: "
      "src [%p to %p] dst [%p to %p]",
      a,
      a,
      a,
      a);
  string_renew_add_separators(&_e);
  EXPECT_DEATH(move_memory(a, a, 1), _e);
  string_delete(&_e);

  string_new_f(
      &_e,
      "Attempting to copy memory between overlapping ranges: "
      "src [%p to %p] dst [%p to %p]",
      a,
      a + 1,
      a,
      a + 1);
  string_renew_add_separators(&_e);
  EXPECT_DEATH(move_memory(a, a, 2), _e);
  string_delete(&_e);

  string_new_f(
      &_e,
      "Attempting to copy memory between overlapping ranges: "
      "src [%p to %p] dst [%p to %p]",
      a,
      a + 2,
      a,
      a + 2);
  string_renew_add_separators(&_e);
  EXPECT_DEATH(move_memory(a, a, 3), _e);
  string_delete(&_e);

  string_new_f(
      &_e,
      "Attempting to copy memory between overlapping ranges: "
      "src [%p to %p] dst [%p to %p]",
      a,
      a + 1,
      a + 1,
      a + 2);
  string_renew_add_separators(&_e);
  EXPECT_DEATH(move_memory(a, a + 1, 2), _e);
  string_delete(&_e);

  string_new_f(
      &_e,
      "Attempting to copy memory between overlapping ranges: "
      "src [%p to %p] dst [%p to %p]",
      a,
      a + 2,
      a + 1,
      a + 3);
  string_renew_add_separators(&_e);
  EXPECT_DEATH(move_memory(a, a + 1, 3), _e);
  string_delete(&_e);

  string_new_f(
      &_e,
      "Attempting to copy memory between overlapping ranges: "
      "src [%p to %p] dst [%p to %p]",
      a,
      a + 2,
      a + 2,
      a + 4);
  string_renew_add_separators(&_e);
  EXPECT_DEATH(move_memory(a, a + 2, 3), _e);
  string_delete(&_e);

  string_new_f(
      &_e,
      "Attempting to copy memory between overlapping ranges: "
      "src [%p to %p] dst [%p to %p]",
      a + 1,
      a + 2,
      a,
      a + 1);
  string_renew_add_separators(&_e);
  EXPECT_DEATH(move_memory(a + 1, a, 2), _e);
  string_delete(&_e);

  string_new_f(
      &_e,
      "Attempting to copy memory between overlapping ranges: "
      "src [%p to %p] dst [%p to %p]",
      a + 1,
      a + 3,
      a,
      a + 2);
  string_renew_add_separators(&_e);
  EXPECT_DEATH(move_memory(a + 1, a, 3), _e);
  string_delete(&_e);

  string_new_f(
      &_e,
      "Attempting to copy memory between overlapping ranges: "
      "src [%p to %p] dst [%p to %p]",
      a + 2,
      a + 4,
      a,
      a + 2);
  string_renew_add_separators(&_e);
  EXPECT_DEATH(move_memory(a + 2, a, 3), _e);
  string_delete(&_e);
}

TEST(ExitNormally, Disallowed) {
  allow_exit_normally(false);
  EXPECT_DEATH(
      exit_normally(),
      "exit_normally() was called, but this has been disallowed");
}

FINISH_DEATH
