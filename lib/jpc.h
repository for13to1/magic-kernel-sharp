/* jpC: a thin syntactic sugar wrapper around ANSI C (C89).
 *
 * These are the main themes of jpC:
 *
 * CODE CONVENTIONS: These follow most modern usage, and VIM's default syntax
 * formatting for C is generally applied. The braceless block is avoided.
 * Pointers are written in the original form 'char *s' rather than 'char* s'.
 * Fundamental types are given names used by C# (rather than C99). For clarity
 * and definiteness, all variable declarations are made at the start of a
 * function (not at the start of any internal block, as ANSI C allows), with
 * only one variable declared per line, with those lines appearing in
 * increasing "type complexity," with variables sorted alphabetically within
 * the same type. Except as noted below, variables and functions use underscore
 * case. Static global variable names end with an underscore; static local
 * variables end with two underscores. Global variables externally referenced
 * are always considered special "tricks" of the library in question, not
 * intended to be directly used in any other code, and the variable names are
 * of the form g_<LIB>_<name>_ to avoid any potential clash with normal
 * variables. Blocks are allowed to "breathe": if they do not start or end with
 * a change in indentation, blank lines are added. The same convention is often
 * applied around pairs of construction statements (see below) and before
 * return statements.
 *
 * C++ COMPATIBILITY: For a period of time, these libraries were tested using
 * Googletest, and so all header files are wrapped in C++ guards, and the code
 * was confirmed to be compatible with C++. As described below, all unit
 * testing is now written in jpC itself, but all code should still be
 * C++-compatible; please contact me if you find any bugs.
 *
 * SOURCE FILE STRUCTURE: To allow automatic dependency inference and build,
 * without ever needing to write a makefile or equivalent, every file <name>.c
 * is always accompanied by a header file <name>.h. The only local #include
 * (i.e. with "") that may appear in <name>.c is '#include "<name>.h"'; any
 * other local includes must be done in <name>.h itself, or else the automated
 * make will fail. System includes, i.e. with <>, may appear in <name>.c, since
 * they do not need to be rebuilt; they only need to be in <name>.h if they
 * define something required in <name>.h itself.
 *
 * MEMORY HANDLING: jpC seeks to retain C's native, direct memory allocation
 * and freeing paradigm, but provides a lightweight wrapper around it. The
 * paradigm is that you can only allocate memory to a pointer that is currently
 * empty (null). When you free that memory, the pointer is again set to empty.
 * This doesn't prevent all mistakes, but it tends to bring in just enough
 * discipline to make them rare; and now that we have ASAN and LSAN, bugs can,
 * with the right unit testing, be essentially eliminated. Allocated memory is
 * always guaranteed to be zeroed, which provides a useful deterministic
 * default for all members, and means that member pointers are initially empty
 * (null) without the need for lots of boilerplate initializers. Allocating
 * zero bytes or zero elements is always safe, on any compiler. All pointers
 * (except those that are program-global, i.e. have external linkage) that
 * "own" allocated memory have names that start with an underscore; such
 * "ownderscores" create visual "pockets" for storing the allocations. All
 * other pointers are only references, referred to as "reference pointers" or
 * just "references" for short, and should never be deleted, renewed, or
 * donated. (Note that a pointer starting with an ownderscore that is an array
 * of pointers only indicates that the pointer itself owns the array; it
 * doesn't tell you whether the objects in the array are also owned by the
 * array or are just references. The former is usually the case, but the latter
 * is sometimes true, and is commented in the code.) Attempting to delete an
 * already-empty pointer is always safe: it is a no-op. Arrays of pointers to
 * objects are deleted with an explicitly specified destructor called on each
 * element. Arrays of value-type structs can also be deleted with a "cleanup"
 * function called on each element (if needed).
 *
 * OBJECT CLASSES: As with other wrappings of C, these are emulated using
 * 'typedef struct'. Class names use upper camel case; members use lower camel
 * case. Methods and static functions for a class are, of course, regular C
 * functions, and always use underscore case, with the first words being the
 * name of the class; for example, MyClass has methods and static functions
 * my_class_...(). The filenames for a class are always the same as the class
 * name, i.e. in upper camel case, and are located in the lib/ directory; for
 * example, MyClass is defined in lib/MyClass.h and lib/MyClass.c. All object
 * classes must include at least one "constructor," with a name of the general
 * form my_class_new[_qualifiers](), the first argument of which must provide
 * the pointer to the (null, i.e. empty) owning pointer, as well as exactly one
 * "destructor," with a name always of the form my_class_delete(), whose only
 * argument is this same pointer to the owning pointer, which will be annulled
 * (made empty). Each constructor must be of the form
 *
 *   MyClass *my_class_new[_<description>](MyClass **p_this[, ...]);
 *
 * returning, for convenience, the pointer to the created object itself. As
 * noted above, the pointer 'p_this' points to the empty owning pointer, which
 * will be both checked to be empty, as well as set to the memory location
 * allocated. Within the constructor, a simple pointer reference
 *
 *   MyClass *this;
 *   ...
 *   this = *p_this;
 *
 * is often defined and used, for convenience, so that instance members can be
 * referred to as this->..., as they are in other high-level languages.
 *
 * Some classes have "try" constructors, that try to construct an object, and
 * return a boolean signifying whether such an attempt was successful. If not,
 * no memory is allocated, and the owning pointer remains null.
 *
 *   bool my_class_try_new[_<description>](MyClass **p_this[, ...]);
 *
 * Some classes may also have "generalized constructors" that construct more
 * than one object at a time, either all of that class, or sometimes including
 * additional classes; the positioning of the pointers to the owning pointers
 * in the argument list of such cases is at the discretion of the designer of
 * the generalized constructor, and need not be (but may be) at the start of
 * the list of arguments.
 *
 * The destructor for an object class is likewise of the form
 *
 *   void my_class_delete(MyClass **p_this);
 *
 * which destroys all of the instance's members and then frees the instance
 * itself; this implies that a class struct must contain all information needed
 * for the destructor, such as the lengths of pointer arrays, etc.
 *
 * Thus a constructor for a class tracking memory will in general have the form
 *
 *   MyClass *this;
 *   ...
 *   this = newo(p_this);
 *   ...
 *   return this;
 *
 * Destructors will generally be of the form
 *
 *   MyClass *this;
 *   ...
 *   if ((this = *p_this) == null) {
 *      return;
 *   }
 *   ...
 *   deleteo(p_this);
 *
 * DEBUG PRINT: Many classes also include a "print" method
 *
 *   void my_class_print(MyClass *this, Printer *p);
 *
 * which allows a convenient printing into a string or console for debugging
 * purposes. See Printer.h for more details.
 *
 * STRUCTURE OBJECT CLASSES: A small number of object classes return structs
 * directly, i.e. on the stack, rather than allocating them on the heap. Such
 * classes have factory methods of the form
 *
 *   MyStruct my_struct_make(...);
 *
 * In an even smaller number of cases, when an array of such structure objects
 * is destroyed, "cleanup" code must be applied to the structure object to
 * clean up instance objects owned by the structure object.
 *
 * STATIC CLASSES: These are simulated by files located in the lib/ directory
 * with underscore case, with the filenames generally being the first words of
 * each static function; for example, bit_array.c and bit_array.h define static
 * functions bit_array_...(). For such static libraries, the naming convention
 * is not as strict as for classes; in some cases, it is convenient to include
 * other static functions in the same file; for example, bit_array.c includes
 * some bit_matrix_...() static functions as well, and the string_...()
 * functions are in files named strng.c and strng.h, to avoid clashing with
 * the system header file string.h and the common Unix header file strings.h.
 * Static classes may also contain "factory" constructors and/or generalized
 * constructors for object classes.
 *
 * PROGRAMS: All programs have lower camel case names, and must be in the prog/
 * directory. As with libraries, <prog-name>.c must include exactly one local
 * header file with #include "<prog-name>.h" which contains all of its local
 * includes. The Makefile automatically figures out how to build each program,
 * in both debug and optimized modes.
 *
 * COMMAND-LINE ARGUMENTS: jpC includes a native 'args' library which provides
 * comprehensive yet simple command-line argument handling, similar to that
 * provided in many frameworks. Look at one of the programs in prog/ for an
 * example of how to use it.
 *
 * STRINGS: C doesn't have string classes, just C arrays of chars terminated by
 * the '\0' sentinel. Since efficiency is paramount, jpC doesn't create any
 * bloated class to represent a string, but simply provides many string_<...>()
 * functions in the static library strng.c and strng.h. These static functions
 * are so fundamental to jpC that strng.h is explicitly #include-d below, so
 * that it never needs to be explicitly included in any library or program.
 * (As noted above, the library is called "strng" to avoid clashes with
 * the string.h and strings.h.)
 *
 * EXCEPTION HANDLING: jpC includes a simple yet effective simulation of
 * exceptions. The function throw() constructs, from its arguments, a
 * printf-formatted error string, with numbers five digits or longer
 * automatically reformatted to include thousands separators (where
 * appropriate). By default, throw() will print this string to stderr,
 * including the file and line of the call, and then exits the program. In
 * debug mode (specified by JPC_DEBUG being passed through from the compiler),
 * the exit actually occurs by trying to reference memory location zero, which
 * triggers a trap by ASAN, if present, and hence shows a strack trace. Without
 * the JPC_DEBUG flag being set, the program simply exits with an error code
 * via exit(1). However, if the throw() is wrapped in a try {} catch {} yrt
 * block, the exception will effectively be "caught," and the exception string
 * is made available in the global string 'exception'. If an exception is
 * subsequently rethrown in the catch {} block, then both the original
 * exception and the newly-thrown exception are printed as the program exits as
 * described above. Note that this is only a simulation of exception handling,
 * and so try {} catch {} yrt blocks cannot be nested. Also note that such a
 * try block will in general leak memory, so LSAN should be turned off when
 * executing any programs using it (e.g. death tests).
 *
 * UNIT TESTS: As noted above, unit testing in jpC was previously performed
 * using Googletest, but is now natively coded in jpC in the library 'test,'
 * hewing as closely to Googletest syntax as possible. The tests within a given
 * file are all actually executed as a single program, since wrapping the
 * executable isn't feasible with a code-level C implementation; but the
 * results are remarkably faithful to how they used to run under Googletest.
 * All tests are located in the test/ directory, and named after the class or
 * static library being tested; for example, ExternalFileTest.c (for a class)
 * or strng_test.c (for a static library), each with its .h file as stipulated
 * above. Death tests make use of the simulated try / catch framework described
 * above to catch all throw()s. Such death tests are always kept in separate
 * tests ending with 'Death' or '_death' respectively; for example,
 * ExternalFileTestDeath.c and strng_test_death.c. Keeping them separate allows
 * the Makefile to run them without LSAN, while keeping LSAN on for regular
 * (non-death) tests. To make the test framework as similar to Googletest as
 * possible, the macros START and FINISH must be called around all the TEST(){}
 * blocks; these actually provide the boilerplate of the program that is
 * executed. (I may enhance this in future to run the tests more similarly to
 * how Googletest does, as parallel processes, but the current implementation
 * is good enough for current practical purposes.) Calling the macro DISABLE
 * between TESTs disables all tests until the matching ENABLE. Calling the
 * macro ASSERT_ALL turns every subsequent EXPECT[...] call into its equivalent
 * ASSERT[...] call, which is useful for providing stack traces to debug test
 * failures (only available if the program throws and is trapped by ASAN); this
 * is canceled by a NO_ASSERT_ALL. Because ANSI C does not have operator
 * overloading like C++, the _EQ tests are split into separate, type-dependent
 * calls. (If there is no test/ directory in the distribution you are looking
 * at, you can contact me for some of the unit tests, to use as examples.)
 *
 * PARALLEL PROCESSING: The 'parallel' static library and the Lock class
 * provide a useful lightweight wrapper around pthreads. Most classes that tend
 * to be resource-intensive are parallelized where possible, where a minimum
 * size threshold is set to prevent inefficiencies in thread creation and
 * tear-down.
 */
#ifndef JPC_H
#define JPC_H

/* In case this gets included in a C++ program.
 */
#ifdef __cplusplus
extern "C" {
#endif

/* We will always assume these.
 */
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>

/* C99 introduced other names for these. These are more convenient, and follow
 * the C# convention. The unit test for this static library confirms that sizes
 * are what you would expect:
 *
 *   Unsigned  Signed  Bytes
 *   --------  ------  -----
 *     byte    sbyte     1
 *    ushort   short     2
 *     uint     int      4
 *     ulong    long     8
 *
 * jpC further assumes single-byte characters, IEEE 754 for floating-point
 * numbers, and a 64-bit memory model:
 *
 *         Type        Bytes
 *         ----        -----
 *         char          1
 *        float          4
 *        double         8
 *        size_t         8
 *    memory pointers    8
 *   function pointers   8
 */
typedef unsigned char byte;
typedef signed char sbyte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

/* We don't yell about null these days.
 */
#define null NULL

/* C++ added these Boolean definitions. Add compatible versions here. Making it
 * a single byte makes it reasonably efficient even in a struct; if there is
 * more than one and efficiency is paramount, use bitfields with 'boolbit'
 * below.
 */
#ifndef __cplusplus
typedef byte bool;
#define false ((bool)0)
#define true ((bool)!false)
#endif /* !__cplusplus */

/* C89 only allows int types to be bitfields. This typedef allows declarations
 * in classes (structs) of the form
 *
 *   boolbit isLocked: 1;
 *
 * where the single bit of width still needs to be specified.
 */
typedef uint boolbit;

/* The token 'this' is a keyword in C++, which will get upset if it's used as
 * an argument. This definition allows us to use 'this' but switch it to
 * 'this_' under the hood, so that C++ doesn't get upset if one of our header
 * files is included in a C++ program.
 */
#ifdef __cplusplus
#define this this_
#endif

/* Memory management functions, per the description above.
 *
 * void *newo(p_this):
 *   Allocates memory for a single object, almost always within a constructor
 *   of an object class. *p_this is set to the allocated memory location, and
 *   is also returned for convenience. The canonical form of the code calling
 *   such a constructor is thus
 *
 *     MyClass *_my_class = null;
 *     ...
 *     my_class_new[...](&_my_class[, args...]);
 *
 *   Within the constructor, a 'this' pointer is often defined, always listed
 *   at the end of the variable definition block.
 *
 *     [other variables...]
 *     MyClass *this;
 *     ...
 *     this = newo(p_this);
 *     ...
 *     this->... = ...;
 *
 * deleteo(p_this);
 *   Frees the memory for a single object and marks the pointer as empty:
 *
 *     deleteo(p_this);
 *
 *   Note that, when used inside a class destructor, this call only frees the
 *   memory for the class struct itself; it doesn't destroy any members of it,
 *   which must be done explicitly in the destructor prior to this call.
 *
 * new(p_this, n):
 *   Allocates an array of n objects. These are standard C arrays, so there is
 *   no length stored or bounds checking. This function automatically infers
 *   the byte-width of each element based on the type of the pointer.
 *
 * delete(p_this, n, destroy):
 *   Frees an array of object pointers, and calls 'destroy' on each non-null
 *   element, which must have signature void f(T **). If the elements are of
 *   value type, then null can be passed in for 'destroy', but also see the
 *   following function.
 *
 * deletev(p_this):
 *   Frees an array of value type, i.e. calls delete() with null 'destroy.'
 *
 * renew(p_this, old_n, new_n, destroy):
 *   Resizes the array. Note that, as with realloc() (which this wraps), the
 *   pointer pointed to by 'p_this' may or may not change, so any references to
 *   it must be refreshed after this call. If the length of the array is
 *   reduced, 'destroy' is called on the deleted elements; again, use
 *   null if the array is of value type. If the length of the array is
 *   increased, the new elements are zeroed. In either case, any surviving
 *   elements are never changed.
 *
 * renewv(p_this, old_n, new_n):
 *   Same as renew(), but for a value type, so that no destructor is needed
 *   even if new_n < old_n.
 *
 * renewup(p_this, old_n, new_n):
 *   Same as renew(), but when you're definite that new_n >= old_n, so that no
 *   destructor will ever be needed. Throws an exception if this is not true.
 *
 * renewup1(p_this, old_n):
 *   Same as renewup() but with new_n = old_n + 1.
 *
 * append_copy(pthis, n, value):
 *   renewup1() the array, copy 'value' to this new element, and n++.
 *
 * append_donate(pthis, n, psrc):
 *   renewup1() the array, donate psrc to this new element, and n++.
 *
 * donate(psrc, pdst):
 *   Gives *psrc's object to *pdst. *psrc must be non-empty and *pdst must be
 *   empty. *psrc is marked empty after the transfer.
 *
 * newm(p_this, m, n):
 *   Allocates a matrix (array of arrays) of m x n objects.
 *
 * newrm(p_this, m, ns):
 *   Allocates a ragged matrix (array of arrays) of m x ns objects.
 *
 * deletem(p_this, m, n, destroy):
 *   Frees a matrix of objects, as per delete().
 *
 * deleterm(p_this, m, ns, destroy):
 *   Frees a ragged matrix of objects.
 *
 * deletemv(p_this, m):
 *   Frees a matrix of values, as per deletev().
 *
 * deletermv(p_this, m):
 *   Frees a ragged matrix of values.
 *
 * newt3(p_this, n0, n1, n2):
 *   Allocates a 3-dimensional tensor of n0 x n1 x n2 objects.
 *
 * deletet3(p_this, n0, n1, n2, destroy):
 *   Frees a 3-dimensional tensor of objects, as per delete().
 *
 * deletet3v(p_this, n0, n1):
 *   Frees a 3-dimensional tensor of values, as per deletev().
 *
 * newt4(p_this, n0, n1, n2, n3):
 *   Allocates a 4-dimensional tensor of n0 x n1 x n2 x n3 objects.
 *
 * deletet4(p_this, n0, n1, n2, n3, destroy):
 *   Frees a 4-dimensional tensor of objects, as per delete().
 *
 * deletet4v(p_this, n0, n1, n2):
 *   Frees a 4-dimensional tensor of values, as per deletev().
 */

#define newo(p_this) \
  newoI((void **)(p_this), sizeof(**(p_this)), __FILE__, __LINE__)

#define deleteo(p_this) \
  deleteoI((void **)(p_this), sizeof(**(p_this)), __FILE__, __LINE__)

#define new(p_this, n) \
  newI((void **)(p_this), (n), sizeof(**(p_this)), __FILE__, __LINE__)

#define delete(p_this, n, d) \
  deleteI( \
      (void **)(p_this), \
      (n), \
      (ulong (*)(void **))(d), \
      sizeof(**(p_this)), \
      __FILE__, \
      __LINE__)

#define deletev deleteo

#define renew(p_this, old_n, new_n, d) \
  renewI( \
      (void **)(p_this), \
      (old_n), \
      (new_n), \
      sizeof(**(p_this)), \
      (ulong (*)(void **))(d), \
      __FILE__, \
      __LINE__)

#define renewv(p_this, old_n, new_n) renew((p_this), (old_n), (new_n), null)

#define renewup(p_this, old_n, new_n) \
  renewupI( \
      (void **)(p_this), \
      (old_n), \
      (new_n), \
      sizeof(**(p_this)), \
      __FILE__, \
      __LINE__)

#define renewup1(p_this, old_n) renewup((p_this), (old_n), (old_n) + 1)

#define append_copy(pthis, n, value) \
  renewup1((pthis), (n)); \
  (*(pthis))[(n)] = (value); \
  (n)++;

#define append_donate(pthis, n, psrc) \
  renewup1((pthis), (n)); \
  donate((psrc), &(*(pthis))[(n)]); \
  (n)++;

#define newm(p_this, n0, n1) \
  newmI((void ***)(p_this), \
      (n0), \
      (n1) * sizeof(***(p_this)), \
      __FILE__, \
      __LINE__)

#define newrm(p_this, n0, n1s) \
  newrmI((void ***)(p_this), \
      (n0), \
      (n1s), \
      sizeof(***(p_this)), \
      __FILE__, \
      __LINE__)

#define deletem(p_this, n0, n1, d) \
  deletemI( \
      (void ***)(p_this), \
      (n0), \
      (n1), \
      (ulong (*)(void **))(d), \
      sizeof(***(p_this)), \
      __FILE__, \
      __LINE__)

#define deleterm(p_this, n0, n1s, d) \
  deletermI( \
      (void ***)(p_this), \
      (n0), \
      (n1s), \
      (ulong (*)(void **))(d), \
      sizeof(***(p_this)), \
      __FILE__, \
      __LINE__)

#define deletemv(p_this, n0) \
  deletemvI( \
      (void ***)(p_this), \
      (n0), \
      __FILE__, \
      __LINE__)

#define deletermv deletemv

#define newt3(p_this, n0, n1, n2) \
  newt3I( \
      (void ****)(p_this), \
      (n0), \
      (n1), \
      (n2) * sizeof(****(p_this)), \
      __FILE__, \
      __LINE__)

#define deletet3(p_this, n0, n1, n2, d) \
  deletet3I( \
      (void ****)(p_this), \
      (n0), \
      (n1), \
      (n2), \
      (ulong (*)(void **))(d), \
      sizeof(****(p_this)), \
      __FILE__, \
      __LINE__)

#define deletet3v(p_this, n0, n1) \
  deletet3vI( \
      (void ****)(p_this), \
      (n0), \
      (n1), \
      __FILE__, \
      __LINE__)

#define newt4(p_this, n0, n1, n2, n3) \
  newt4I( \
      (void *****)(p_this), \
      (n0), \
      (n1), \
      (n2), \
      (n3) * sizeof(*****(p_this)), \
      __FILE__, \
      __LINE__)

#define deletet4(p_this, n0, n1, n2, n3, d) \
  deletet4I( \
      (void *****)(p_this), \
      (n0), \
      (n1), \
      (n2), \
      (n3), \
      (ulong (*)(void **))(d), \
      sizeof(*****(p_this)), \
      __FILE__, \
      __LINE__)

#define deletet4v(p_this, n0, n1, n2) \
  deletet4vI( \
      (void *****)(p_this), \
      (n0), \
      (n1), \
      (n2), \
      __FILE__, \
      __LINE__)

#define donate(psrc, pdst) \
  donateI((void **)(psrc), (void **)(pdst), __FILE__, __LINE__)

void *newoI(void **p_this, ulong size, char *file, int line);

void deleteoI(void **p_this, ulong bytes, char *file, int line);

void *newI(void **p_this, ulong n, ulong size, char *file, int line);

void deleteI(void **p_this,
    ulong n,
    ulong (*destroy)(void **p_this),
    ulong size,
    char *file,
    int line);

void renewI(void **p_this,
    ulong old_n,
    ulong new_n,
    ulong size,
    ulong (*destroy)(void **p_this),
    char *file,
    int line);

void renewupI(void **p_this,
    ulong old_n,
    ulong new_n,
    ulong size,
    char *file,
    int line);

void newmI(void ***p_this, ulong n0, ulong bytes, char *file, int line);

void newrmI(void ***p_this,
    ulong n0,
    ulong *n1s,
    ulong size,
    char *file,
    int line);

void deletemI(void ***p_this,
    ulong n0,
    ulong n1,
    ulong (*destroy)(void **p_this),
    ulong size,
    char *file,
    int line);

void deletermI(void ***p_this,
    ulong n0,
    ulong *n1s,
    ulong (*destroy)(void **p_this),
    ulong size,
    char *file,
    int line);

void deletemvI(void ***p_this, ulong n0, char *file, int line);

void newt3I(void ****p_this,
    ulong n0,
    ulong n1,
    ulong bytes,
    char *file,
    int line);

void deletet3I(void ****p_this,
    ulong n0,
    ulong n1,
    ulong n2,
    ulong (*destroy)(void **p_this),
    ulong bytes,
    char *file,
    int line);

void deletet3vI(void ****p_this, ulong n0, ulong n1, char *file, int line);

void newt4I(void *****p_this,
    ulong n0,
    ulong n1,
    ulong n2,
    ulong bytes,
    char *file,
    int line);

void deletet4I(void *****p_this,
    ulong n0,
    ulong n1,
    ulong n2,
    ulong n3,
    ulong (*destroy)(void **p_this),
    ulong bytes,
    char *file,
    int line);

void deletet4vI(void *****p_this,
    ulong n0,
    ulong n1,
    ulong n2,
    char *file,
    int line);

void donateI(void **psrc, void **pdst, char *file, int line);

/* Simulate exceptions, as described above.
 */
#define throw \
  throw_lockI(); \
  g_JPC_internal_throw_ = false, \
  g_JPC_file_2_ = g_JPC_file_1_, \
  g_JPC_line_2_ = g_JPC_line_1_, \
  g_JPC_file_1_ = __FILE__, \
  g_JPC_line_1_ = __LINE__, \
  throwI

#define try \
  if (g_JPC_try_ == 1) {g_JPC_try_ = 0; throw("Already in try");} \
  if (g_JPC_try_ == 2) {g_JPC_try_ = 0; throw("Already in catch");} \
  g_JPC_try_ = 1; switch (setjmp(g_JPC_jmp_buf_)) {case 0:

#define catch \
  if (g_JPC_try_ == 0) {g_JPC_try_ = 0; throw("Not in try");} \
  if (g_JPC_try_ == 2) {g_JPC_try_ = 0; throw("Already in catch");} \
  break; case 1: g_JPC_try_ = 2;

#define yrt \
  if (g_JPC_try_ == 0) {g_JPC_try_ = 0; throw("Not in try");} \
  if (g_JPC_try_ == 1) {g_JPC_try_ = 0; throw("No catch");} \
  break; default: throw("Uncaught throw");} g_JPC_try_ = 0;

void throw_lockI(void);

void throw_unlockI(void);

void throwI(char *fmt, ...);

/* Exit normally (with return code 0). If allow_exit_normally(false) has been
 * called, this will throw rather than exit normally. This allows tests to
 * prevent a library from exiting the test directly back to the operating
 * system, rather than returning control to the test. (Tests do not truly wrap
 * the process, but simulate it through the macros here and in test.h.)
 */
void exit_normally(void);

void allow_exit_normally(bool allow);

/* Should have been in math.h from day one.
 */
#ifndef __cplusplus
double round(double x);
#endif

/* Useful macros. Note that 'max()' and 'min()' are not used as they are
 * elsewhere, both so that 'max' and 'min' can be used as variable names
 * without confusion or syntax highlighting, and also to avoid clashes with
 * those other definitions of max() and min(). The macros modulo(a, b) and
 * intdiv(a, b) provide consistent (translation-invariant) results of positive
 * values [0, b - 1] and rounding down respectively, even for negative values
 * of a, which is not true for the raw C operators % and / because an
 * architecture is free to round the latter towards zero rather than down,
 * which results in non-positive values for % for negative a. Casting to long
 * is also done on all values, as mixing longs and ulongs with % and / can
 * result in incorrect results (i.e. casts to ulong).
 */
#define bool_string(b) ((b) ? "true" : "false")

#define plural(n, s, p) ((n) == 1 ? (s) : (p))

#define plural_s(n) (plural((n), "", "s"))

#define plural_s_c(n) (plural((n), "", "S"))

#define maximum(a, b) ((a) > (b) ? (a) : (b))

#define minimum(a, b) ((a) < (b) ? (a) : (b))

#define lower_bound(a, min) ((a) < (min) ? (min) : (a))

#define upper_bound(a, max) ((a) > (max) ? (max) : (a))

#define bound(a, min, max) ((a) < (min) ? (min) : (a) > (max) ? (max) : (a))

#define modulo(a, b) ((((long)(a)) % ((long)(b)) + ((long)(b))) % ((long)(b)))

#define intdiv(a, b) \
  (((long)(a)) / ((long)(b)) - (((long)(a) % (long)(b)) < 0 ? 1 : 0))

/* "Defrags" an array of pointers by removing any nulls and shortening the
 * array to suit. The address of the array and of its length must be passed in,
 * and are updated. As with delete(), the reduction in immutable-size memory is
 * returned.
 */
#define defrag_pointer_array(p_this, pn) \
  defrag_pointer_arrayI((void ***)(p_this), (pn))

void defrag_pointer_arrayI(void ***p_this, ulong *pn);

/* Copy a block of memory, like memcpy(), but in parallel where appropriate.
 */
void copy_memory(void *src, void *dst, ulong num_bytes);

/* Copy an array of values, auto-detecting the size of each value.
 */
#define copy_values(src, dst, n) \
  copy_memory((src), (dst), (n) * sizeof(*(src)))

/* Copy a matrix of memory.
 */
void copy_memorym(void *src, void *dst, ulong n0, ulong num_bytes);

/* Copy a matrix of values, auto-detecting the size of each value.
 */
#define copy_valuesm(src, dst, n0, n1) \
  copy_memorym((src), (dst), (n0), (n1) * sizeof(**(src)))

/* Create an array of values and then copy them, auto-detecting the size of
 * each value.
 */
#define new_copy(pdst, n, src) \
  new((pdst), (n)), \
  copy_values((src), *(pdst), (n)), \
  (n) * sizeof(*(pdst))

/* Copy a matrix of memory to an array, i.e., flattening the matrix.
 */
void copy_memorym_flatten(void *src, void *dst, ulong n0, ulong num_bytes);

/* Copy a matrix of values to an array, auto-detecting the size of each value.
 */
#define copy_valuesm_flatten(src, dst, n0, n1) \
  copy_memorym_flatten((src), (dst), (n0), (n1) * sizeof(**(src)))

/* Copy an array of memory to a matrix, i.e., unflattening the array.
 */
void copy_memorym_unflatten(void *src, void *dst, ulong n0, ulong num_bytes);

/* Copy an array of values to a matrix, auto-detecting the size of each value.
 */
#define copy_valuesm_unflatten(src, dst, n0, n1) \
  copy_memorym_flatten((src), (dst), (n0), (n1) * sizeof(**(src)))

/* Clear a block of memory, i.e. set all bytes back to zero.
 */
void clear_memory(void *memory, ulong num_bytes);

/* Clear an array of values, auto-detecting the size of each value.
 */
#define clear_values(values, n) \
  clear_memory((values), (n) * sizeof(*(values)))

/* Move a block of memory defensively, i.e., copy it and then clear the source.
 */
void move_memory(void *src, void *dst, ulong n);

/* Move an array of values defensively, auto-detecting the size of each value.
 */
#define move_values(src, dst, n) \
  move_memory((src), (dst), (n) * sizeof(*(src)))

/* Whether two blocks of memory are equal.
 */
bool memory_equals(void *a, void *b, ulong num_bytes);

/* Whether two arrays of values are equal.
 */
#define values_equal(a, b, n) \
  memory_equals((a), (b), (n) * sizeof(*(a)))

/* Whether two matrixes of values are equal.
 */
#define values_equalm(a, b, n0, n1) \
  values_equalmI((byte **)(a), (byte **)(b), (n0), (n1) * sizeof(**(a)))

bool values_equalmI(byte **a, byte **b, ulong n0, ulong num_bytes);

/* Whether the machine is little-endian. Note that as of March 2023, this must
 * be true for these jpC libraries, since there are no serious architectures
 * left that are not.
 */
bool little_endian(void);

/* This function can be used to "use" any pointer, during code construction, to
 * prevent any compilation warnings about unused variables. For non-pointer
 * variables, use() the address of the variable.
 */
#define use(x) useI((void *)(x))

void useI(void *p);

/* A destructor that does nothing. A.k.a. the "Legal Hold" destructor.
 */
void delete_nothing(void **p_this);

/* A comparator that just compares the actual addresses of the pointers.
 */
sbyte pointer_compare(void *a, void *b, void *x);

/* Same, but for when the pointers are stored as references in an array.
 */
sbyte pointer_pointer_compare(void *p_a, void *p_b, void *x);

/* Useful for debugging.
 */
#define HERE dlog(0, "--> HERE <--");
#define TODO

/* External decalarations of variables defined in jpc.c that are used to
 * deliver all of the above trickery.
 */
#ifndef JPC_C
extern bool g_JPC_internal_throw_;
extern bool g_JPC_exception_separators_;
extern byte g_JPC_try_;
extern char *exception;
extern char *g_JPC_file_1_;
extern char *g_JPC_file_2_;
extern char *g_JPC_orig_exception__;
extern int g_JPC_line_1_;
extern int g_JPC_line_2_;
extern jmp_buf g_JPC_jmp_buf_;
#endif /* JPC_C */

/* These libraries are considered core to jpC, so include them here so they
 * don't have to be explicitly included anywhere else.
 */
#include "File.h"
#include "IeeeDouble.h"
#include "Lock.h"
#include "Logger.h"
#include "Printer.h"
#include "Progress.h"
#include "log.h"
#include "parallel.h"
#include "strng.h"

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
