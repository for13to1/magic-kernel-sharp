#include "args_test_death.h"

static void cycle_(void);

static void command_(char *first, ...);

static void test_(char *expected, ...);


START_DEATH

TEST(Args, Cleanup) {
  EXPECT_DEATH(args_cleanup(), "args_description() was never called");

  args_description("woof");
  args_cleanup();
  EXPECT_DEATH(args_cleanup(), "args_cleanup() has already been called");

  EXPECT_DEATH(
      args_help(null),
      "Must call args_parse() or args_program() before args_help()");

  args_description("woof");
  command_("", null);
  EXPECT_DEATH(command_("", null), "args_parse() already called");
  args_cleanup();
}

TEST(ArgsDeath, Init) {
  EXPECT_DEATH(
      args_string("name", "desc"),
      "args_description() not called");
  EXPECT_DEATH(
      args_integer("name", "desc"),
      "args_description() not called");
  EXPECT_DEATH(
      args_real("name", "desc"),
      "args_description() not called");
  EXPECT_DEATH(
      args_flag("name", 'a', "desc"),
      "args_description() not called");
  EXPECT_DEATH(
      args_optional_string("name", ' ', "a", "desc"),
      "args_description() not called");
  EXPECT_DEATH(
      args_optional_integer("name", 'a', 12, "desc"),
      "args_description() not called");
  EXPECT_DEATH(
      args_optional_real("name", 'a', 3.14, "desc"),
      "args_description() not called");
  EXPECT_DEATH(
      args_parse(0, null),
      "args_description() not called");
}

TEST(ArgsDeath, Spec) {
  EXPECT_DEATH(
      args_string("1name", "desc"),
      "args_description() not called");
  cycle_();

  EXPECT_DEATH(
      args_string("", "desc"),
      "Name cannot be empty");
  EXPECT_DEATH(
      args_string("name", ""),
      "Description cannot be empty");
  EXPECT_DEATH(
      args_string("1name", "desc"),
      "Invalid name '1name': must start with alpha");
  EXPECT_DEATH(
      args_string("na!me", "desc"),
      "Invalid name 'na!me': can only include alphanumeric or '-'");
  args_optional_integer("n1-Me", 'a', -1, "desc");
  EXPECT_DEATH(
      args_optional_integer("n1-Me", 'b', -2, "desc"),
      "Repeated name 'n1-Me'");
  EXPECT_DEATH(
      args_optional_integer("name", 'a', -2, "desc"),
      "Repeated short option '-a'");
  args_strings("files", "desc");
  EXPECT_DEATH(
      args_string("dog", "desc"),
      "Cannot call after args_strings() called");
  EXPECT_DEATH(
      args_integer("dog", "desc"),
      "Cannot call after args_strings() called");
  EXPECT_DEATH(
      args_real("dog", "desc"),
      "Cannot call after args_strings() called");
  EXPECT_DEATH(
      args_strings("dog", "desc"),
      "Cannot call after args_strings() called");
  EXPECT_DEATH(
      args_optional_string("dog", '!', "", "desc"),
      "Invalid short option '!': must be alphanumeric, or space for none");

  args_cleanup();
}

TEST(ArgsDeath, Description) {
  char *_s = null;

  cycle_();
  args_string("dog", "\twoof");
  command_("woof", null);
  EXPECT_DEATH(args_help(&_s), "Can't have tabs on the first line");
  string_delete(&_s);

  cycle_();
  args_string("dog", "A \twoof");
  command_("woof", null);
  EXPECT_DEATH(args_help(&_s), "Can't have tabs in the middle of a line");
  string_delete(&_s);

}

TEST(ArgsDeath, CommandLine) {
  cycle_();

  /* Test stdin '-' when no positional arguments, as well as spaces around
   * argument.
   */
  CALL(test_(
      "Specified stdin value '-' but no positional arguments left",
      "-",
      null));
  cycle_();

  CALL(test_(
      "Specified stdin value '-' but no positional arguments left",
      "- ",
      null));
  cycle_();

  CALL(test_(
      "Specified stdin value '-' but no positional arguments left",
      " -",
      null));
  cycle_();

  CALL(test_(
      "Specified stdin value '-' but no positional arguments left",
      " - ",
      null));
  cycle_();

  CALL(test_(
      "Specified stdin value '-' but no positional arguments left",
      " - d",
      null));
  cycle_();

  /* Test stdin '-' when argument is not a string.
   */
  args_integer("n1-Me", "desc");
  CALL(test_(
      "Supplied stdin value '-' for non-string argument <N1_ME>",
      "-",
      null));
  cycle_();

  /* Test too many positional arguments.
   */
  CALL(test_(
      "Positional value 'woof' supplied but no positional arguments left",
      "woof",
      null));
  cycle_();

  /* Test an extra '-' after a flag run already started.
   */
  args_flag("a", 'c', "woof");
  CALL(test_(
      "Invalid '-' after flag option",
      "-c-",
      null));
  cycle_();

  /* Test end of command line before specifying an argument.
   */
  args_optional_real("num", 'c', 0, "woof");
  CALL(test_(
      "Command line ended before specifying a value for --num",
      "-c",
      null));
  cycle_();

  /* Test supplying wrong type of value for positional argument.
   */
  args_integer("n1-Me", "desc");
  CALL(test_(
      "Could not parse 'woof' as an integer for argument <N1_ME>",
      "woof",
      null));
  cycle_();

  args_real("n1-Me", "desc");
  CALL(test_(
      "Could not parse 'woof' as a real for argument <N1_ME>",
      "woof",
      null));
  cycle_();

  /* Test not enough positional arguments.
   */
  args_string("c", "desc");
  args_integer("n1-Me", "desc");
  CALL(test_(
      "No value supplied for the positional argument <N1_ME>",
      "dog",
      null));
  cycle_();

  args_string("c", "desc");
  args_integer("n1-Me", "desc");
  args_integer("name", "desc");
  CALL(test_(
      "No values supplied for the positional arguments <N1_ME> and <NAME>",
      "dog",
      null));
  cycle_();

  args_string("c", "desc");
  args_integer("n1-Me", "desc");
  args_integer("name", "desc");
  args_real("n", "descr");
  CALL(test_(
      "No values supplied for the positional arguments <N1_ME>, <NAME>, "
      "and <N>",
      "dog",
      null));
  cycle_();

  args_integer("n1-Me", "desc");
  args_integer("name", "desc");
  CALL(test_(
      "No value supplied for the positional argument <NAME>",
      "--",
      "-12",
      null));
  cycle_();

  args_integer("n1-Me", "desc");
  args_integer("name", "desc");
  args_real("size", "desc");
  CALL(test_(
      "No values supplied for the positional arguments <NAME> and <SIZE>",
      "--",
      "-12",
      null));
  cycle_();

  args_integer("n1-Me", "desc");
  args_integer("name", "desc");
  args_real("size", "desc");
  args_string("a", "desc");
  CALL(test_(
      "No values supplied for the positional arguments <NAME>, <SIZE>, and <A>",
      "--",
      "-12",
      null));
  cycle_();

  args_integer("n1-Me", "desc");
  args_integer("name", "desc");
  args_real("size", "desc");
  args_string("a", "desc");
  CALL(test_(
      "No values supplied for the positional arguments <SIZE> and <A>",
      "--",
      "-12",
      "42",
      null));
  cycle_();

  args_integer("n1-Me", "desc");
  args_integer("name", "desc");
  args_real("size", "desc");
  args_string("a", "desc");
  CALL(test_(
      "No value supplied for the positional argument <A>",
      "12",
      "42",
      "3.14",
      null));
  cycle_();

  /* Test non-existent optional arguments.
   */
  CALL(test_(
      "Invalid option '-a'",
      "-a",
      null));
  cycle_();

  CALL(test_(
      "Invalid option '--name'",
      "--name",
      null));
  cycle_();

  args_integer("a-b", "desc");
  CALL(test_(
      "Invalid option '-a'",
      "-a",
      null));
  cycle_();

  args_real("a-b", "desc");
  CALL(test_(
      "Invalid option '--name'",
      "--name",
      null));
  cycle_();

  /* Test invalid short option names.
   */
  CALL(test_(
      "Invalid short option name '-!'",
      "-!",
      null));
  cycle_();

  CALL(test_(
      "Invalid short option name '-$'",
      "-$",
      null));
  cycle_();

  /* Test invalid long option names.
   */
  CALL(test_(
      "Invalid long option name in argument '--1'",
      "--1",
      null));
  cycle_();

  CALL(test_(
      "Invalid long option name in argument '--2dogs=cat'",
      "--2dogs=cat",
      null));
  cycle_();

  CALL(test_(
      "Invalid long option name in argument '--one_dog'",
      "--one_dog",
      null));
  cycle_();

  CALL(test_(
      "Invalid long option name in argument '--one!'",
      "--one!",
      null));
  cycle_();

  /* Test invalid flag runs.
   */
  args_flag("dog", 'd', "desc");
  args_flag("cat", 'c', "desc");
  CALL(test_(
      "Invalid short option name '-!'",
      "-cd!",
      null));
  cycle_();

  args_flag("dog", 'd', "desc");
  args_flag("cat", 'c', "desc");
  CALL(test_(
      "Invalid short option name '-='",
      "-cd=",
      null));
  cycle_();

  /* Test repeated option.
   */
  args_flag("dog", 'd', "desc");
  args_flag("cat", 'c', "desc");
  CALL(test_(
      "Repeated option '-c', '--cat'",
      "-cdc",
      null));
  cycle_();

  args_flag("dog", 'd', "desc");
  args_flag("cat", 'c', "desc");
  CALL(test_(
      "Repeated option '-c', '--cat'",
      "-cd",
      "--cat",
      null));
  cycle_();

  args_flag("dog", 'd', "desc");
  args_flag("cat", 'c', "desc");
  CALL(test_(
      "Repeated option '-c', '--cat'",
      "--cat",
      "-cd",
      null));
  cycle_();

  /* Test wrong type supplied for optional arguments.
   */
  args_optional_integer("dog", 'd', -1, "desc");
  CALL(test_(
      "Could not parse 'cat' as an integer for argument <DOG>",
      "-dcat",
      null));
  cycle_();

  args_optional_integer("dog", 'd', -1, "desc");
  CALL(test_(
      "Could not parse '1.7' as an integer for argument <DOG>",
      "--dog=1.7",
      null));
  cycle_();

  args_optional_integer("dog", 'd', -1, "desc");
  CALL(test_(
      "Could not parse '-1e-3' as an integer for argument <DOG>",
      "--dog",
      "-1e-3",
      null));
  cycle_();

  args_optional_integer("dog", 'd', -1, "desc");
  CALL(test_(
      "Could not parse 'cat' as an integer for argument <DOG>",
      "-dcat",
      null));
  cycle_();

  args_optional_integer("dog", 'd', -1, "desc");
  CALL(test_(
      "Could not parse 'cat' as an integer for argument <DOG>",
      "--dog=cat",
      null));
  cycle_();

  args_optional_integer("dog", 'd', -1, "desc");
  CALL(test_(
      "Could not parse 'cat' as an integer for argument <DOG>",
      "--dog",
      "cat",
      null));
  cycle_();

  args_cleanup();
}

TEST(ArgsDeath, Get) {
  char **_strings = null;

  cycle_();

  /* Try to get argument when none specified.
   */
  command_(null);
  EXPECT_DEATH(args_get_flag("a"), "No argument named 'a'");
  cycle_();

  /* Try to get non-existent argument when one other one specified.
   */
  args_string("ab", "desc");
  command_("dog", null);
  EXPECT_DEATH(args_get_flag("a"), "No argument named 'a'");
  cycle_();

  args_integer("ab", "desc");
  command_("45", null);
  EXPECT_DEATH(args_get_flag("a"), "No argument named 'a'");
  cycle_();

  args_real("ab", "desc");
  command_("1.23e-45", null);
  EXPECT_DEATH(args_get_flag("a"), "No argument named 'a'");
  cycle_();

  args_string("c", "desc");
  args_flag("ab", ' ', "desc");
  command_("dog", null);
  EXPECT_DEATH(args_get_flag("a"), "No argument named 'a'");
  cycle_();

  args_string("c", "desc");
  args_optional_integer("ab", 'x', -12, "desc");
  command_("dog", null);
  EXPECT_DEATH(args_get_flag("a"), "No argument named 'a'");
  cycle_();

  args_string("c", "desc");
  args_optional_string("ab", 'x', "woof", "desc");
  command_("dog", null);
  EXPECT_DEATH(args_get_flag("a"), "No argument named 'a'");
  cycle_();

  args_string("c", "desc");
  args_optional_real("ab", 'a', -3.14, "desc");
  command_("dog", null);
  EXPECT_DEATH(args_get_flag("a"), "No argument named 'a'");
  cycle_();

  args_string("c", "desc");
  args_optional_integer("ab", 'd', 42, "desc");
  command_("dog", null);
  EXPECT_DEATH(args_get_flag("a"), "No argument named 'a'");
  cycle_();

  /* Try to call args_set() on a positional argument.
   */
  args_string("cat", "desc");
  command_("dog", null);
  EXPECT_DEATH(
      args_set("cat"),
      "Can't call args_set() on the positional argument --cat");
  cycle_();

  /* Try to get strings when none specified.
   */
  command_(null);
  EXPECT_DEATH(args_get_strings(&_strings), "No strings argument specified");
  cycle_();

  /* Try to get value of the wrong type.
   */
  args_string("ab", "desc");
  command_("dog", null);
  EXPECT_DEATH(args_get_flag("ab"), "Argument 'AB' is not a flag");
  EXPECT_DEATH(args_get_integer("ab"), "Argument 'AB' is not an integer");
  EXPECT_DEATH(args_get_real("ab"), "Argument 'AB' is not a real");
  cycle_();

  args_integer("ab", "desc");
  command_("12", null);
  EXPECT_DEATH(args_get_flag("ab"), "Argument 'AB' is not a flag");
  EXPECT_DEATH(args_get_string("ab"), "Argument 'AB' is not a string");
  EXPECT_DEATH(args_get_real("ab"), "Argument 'AB' is not a real");
  cycle_();

  args_real("ab", "desc");
  command_("3.14", null);
  EXPECT_DEATH(args_get_flag("ab"), "Argument 'AB' is not a flag");
  EXPECT_DEATH(args_get_string("ab"), "Argument 'AB' is not a string");
  EXPECT_DEATH(args_get_integer("ab"), "Argument 'AB' is not an integer");
  cycle_();

  args_string("c", "desc");
  args_flag("ab", ' ', "desc");
  command_("dog", null);
  EXPECT_DEATH(args_get_string("ab"), "Argument 'ab' is not a string");
  EXPECT_DEATH(args_get_integer("ab"), "Argument 'ab' is not an integer");
  EXPECT_DEATH(args_get_real("ab"), "Argument 'ab' is not a real");
  cycle_();

  args_string("c", "desc");
  args_optional_string("ab", 'a', "dog", "desc");
  command_("dog", null);
  EXPECT_DEATH(args_get_flag("ab"), "Argument 'ab' is not a flag");
  EXPECT_DEATH(args_get_integer("ab"), "Argument 'ab' is not an integer");
  EXPECT_DEATH(args_get_real("ab"), "Argument 'ab' is not a real");
  cycle_();

  args_string("c", "desc");
  args_optional_integer("ab", '1', -1, "desc");
  command_("dog", null);
  EXPECT_DEATH(args_get_flag("ab"), "Argument 'ab' is not a flag");
  EXPECT_DEATH(args_get_string("ab"), "Argument 'ab' is not a string");
  EXPECT_DEATH(args_get_real("ab"), "Argument 'ab' is not a real");
  cycle_();

  args_string("c", "desc");
  args_optional_real("ab", 'A', -1.2e-3, "desc");
  command_("dog", null);
  EXPECT_DEATH(args_get_flag("ab"), "Argument 'ab' is not a flag");
  EXPECT_DEATH(args_get_string("ab"), "Argument 'ab' is not a string");
  EXPECT_DEATH(args_get_integer("ab"), "Argument 'ab' is not an integer");
  cycle_();

  /* Test dynamic defaults.
   */
  args_optional_string("cat", 'c', "dog", "desc");
  command_("", null);
  EXPECT_DEATH(
      args_set_string_default("cat", "miaow"),
      "Argument 'cat' already has a default value ('dog')");
  EXPECT_DEATH(
      args_set_integer_default("cat", -3),
      "Argument 'cat' is not an integer");
  EXPECT_DEATH(
      args_set_real_default("cat", -3.14),
      "Argument 'cat' is not a real");
  cycle_();

  args_optional_string("cat", 'c', ARGS_STRING_NO_DEFAULT, "desc");
  command_("", null);
  EXPECT_DEATH(
      args_set_integer_default("cat", -3),
      "Argument 'cat' is not an integer");
  EXPECT_DEATH(
      args_set_real_default("cat", -3.14),
      "Argument 'cat' is not a real");
  args_set_string_default("cat", "dog"),
  EXPECT_DEATH(
      args_set_string_default("cat", "miaow"),
      "Argument 'cat' already has a default value ('dog')");
  cycle_();

  /* Test constraints.
   */
  args_integer("ab", "desc");
  command_("2", null);
  EXPECT_DEATH(
      args_get_bounded_integer("ab", 2, 1),
      "Maximum 1 less than minimum 2");
  EXPECT_DEATH(
      args_get_bounded_integer("ab", 1, 1),
      "Argument AB must be in the range [1, 1], but the value 2 was specified");
  EXPECT_DEATH(
      args_get_bounded_integer("ab", 0, 1),
      "Argument AB must be in the range [0, 1], but the value 2 was specified");
  cycle_();

  args_real("ab", "desc");
  command_("1.5", null);
  EXPECT_DEATH(
      args_get_bounded_real("ab", 2.3, 2.1),
      "Maximum 2.1 less than minimum 2.3");
  EXPECT_DEATH(
      args_get_bounded_real("ab", 2.3, 2.3),
      "Argument AB must be in the range [2.3, 2.3], but the value 1.5 "
      "was specified");
  EXPECT_DEATH(
      args_get_bounded_real("ab", 1.50001, 2.3),
      "Argument AB must be in the range [1.500 01, 2.3], but the value 1.5 "
      "was specified");
  cycle_();

  args_string("ab", "desc");
  command_("-", null);
  EXPECT_DEATH(args_get_string_no_std("ab"), "Argument AB cannot be '-'");
  cycle_();

  args_strings("ab", "desc");
  command_("-", null);
  EXPECT_DEATH(
      args_get_strings_no_std(&_strings),
      "Argument AB cannot contain the string '-'");
  cycle_();

  args_strings("ab", "desc");
  command_("abc", "-", null);
  EXPECT_DEATH(
      args_get_strings_no_std(&_strings),
      "Argument AB cannot contain the string '-'");
  cycle_();

  args_strings("ab", "desc");
  command_("-", "abc", null);
  EXPECT_DEATH(
      args_get_strings_no_std(&_strings),
      "Argument AB cannot contain the string '-'");
  cycle_();

  args_strings("ab", "desc");
  command_("-", "-", null);
  EXPECT_DEATH(
      args_get_strings_no_std(&_strings),
      "Argument AB cannot contain the string '-'");
  cycle_();

  args_optional_string("ab", 'c', "", "desc");
  command_("-c", "", null);
  EXPECT_DEATH(
      args_get_nonempty_string("ab"),
      "Argument AB must not be empty, but an empty string was specified");
  cycle_();

  args_optional_integer("ab", 'c', 1, "desc");
  command_("-c-1", null);
  EXPECT_DEATH(
      args_get_positive_integer("ab"),
      "Argument AB must be positive, but the value -1 was specified");
  cycle_();

  args_optional_integer("ab", 'c', 1, "desc");
  command_("-c 0", null);
  EXPECT_DEATH(
      args_get_positive_integer("ab"),
      "Argument AB must be positive, but the value 0 was specified");
  cycle_();

  args_optional_integer("ab", 'c', 1, "desc");
  command_("-c -1", null);
  EXPECT_DEATH(
      args_get_nonnegative_integer("ab"),
      "Argument AB must be nonnegative, but the value -1 was specified");
  cycle_();

  /* Test vetoes.
   */
  args_flag("ab", 'a', "desc");
  args_flag("cd", 'b', "desc");
  command_("-ab", null);
  EXPECT_DEATH(
      args_set_veto("ab", "cd"),
      "Cannot specify both --ab and --cd");
  cycle_();

  args_flag("ab", 'a', "desc");
  args_flag("cd", 'b', "desc");
  args_flag("ef", 'c', "desc");
  command_("-ac", null);
  EXPECT_DEATH(
      args_set_veto_until_null("ab", "cd", "ef", null),
      "Cannot specify both --ab and --ef");
  cycle_();

  args_cleanup();
}

FINISH_DEATH


static void cycle_(void) {
  args_try_cleanup();
  args_description("");
}

static void command_(char *first, ...) {
  char *s;
  char **_argv = null;
  int argc;
  va_list ap;

  if (first == null) {
    new(&_argv, argc = 1);
    _argv[0] = "prog";
  } else {
    new(&_argv, argc = 2);
    _argv[0] = "prog";
    _argv[1] = first;

    va_start(ap, first);

    while ((s = va_arg(ap, char *)) != null) {
      append_copy(&_argv, argc, s);
    }

    va_end(ap);
  }

  args_parse(argc, _argv);

  deletev(&_argv);
}

static void test_(char *expected, ...) {
  char *s;
  char **_argv = null;
  int argc;
  va_list ap;

  new(&_argv, argc = 1);
  _argv[0] = "prog";
  va_start(ap, expected);

  while ((s = va_arg(ap, char *)) != null) {
    append_copy(&_argv, argc, s);
  }

  va_end(ap);

  if (argc == 0) {
    throw("Must at least supply program name");
  }

  EXPECT_DEATH(args_parse(argc, _argv), expected);

  deletev(&_argv);
}
