/* Object class providing convenient nested printing of objects for debugging
 * purposes. An object class wishing to provide such functionality must define
 * a method
 *
 *   void my_class_print(MyClass *this, Printer *p);
 *
 * Within that method, an indented block is started by
 *
 *   sprint(p, fmt, ...);
 *
 * which prints the supplied string in square brackets, and finished with
 *
 *   fprint(p);
 *
 * which prints the same original string in square brackets but with a leading
 * slash, like for end tags in XML. There cannot be more than 99 nested levels
 * of indentation.
 *
 * A random line of printing (not the start or end of an indented block) is
 * specified by
 *
 *   rprint(p, fmt, ...);
 *
 * To print an object with an indent level 'i' in debugging mode (to console):
 *
 *   dprint(i, my_class, object);
 *
 * To olog() it out (logged to stdout):
 *
 *   oprint(i, my_class, object);
 *
 * To print it into an arbitary string (creating or appending the string) with
 * an indent level 'i':
 *
 *   aprint(char **ps, i, myclass, object);
 *
 * (For an internal class, use dprint_() and aprint_() respectively.)
 */
#ifndef PRINTER_H
#define PRINTER_H

#ifdef __cplusplus
extern "C" {
#endif

struct Printer;

#include "Lock.h"
#include "log.h"

typedef struct Printer {
  char **ps;
  char **_headings;
  byte indent;
  ulong numHeadings;
  ulong maxHeadings;
} Printer;

/* Constructor. You shouldn't need to call this explicitly; it is called from
 * dprint() and sprint().
 */
void printer_new(Printer **p_this, char **ps, byte indent);

/* Destructor. Again, you shouldn't need to call this explicitly.
 */
void printer_delete(Printer **p_this);

/* Start an indented block.
 */
void sprint(Printer *this, char *fmt, ...);

/* Finish an indented block.
 */
void fprint(Printer *this);

/* Random (single-line) print.
 */
void rprint(Printer *this, char *fmt, ...);

/* Debug print of an object.
 */
#define dprint(i, c, o) mprint(null, i, c##_print, o)

/* Stdout log print of an object.
 */
#define oprint(i, c, o) \
  { \
    char *_s = null; \
    \
    aprint(&_s, (i), c, (o)); \
    olog(0, "%s", _s); \
    string_delete(&_s); \
  } \

/* Stderr log print of an object.
 */
#define eprint(i, c, o) \
  { \
    char *_s = null; \
    \
    aprint(&_s, (i), c, (o)); \
    elog(0, "%s", _s); \
    string_delete(&_s); \
  } \

/* Stderr log print of an object just before a throw, so suppress the
 * timestamps.
 */
#define tprint(n, c, o) \
  { \
    char *_s = null; \
    \
    aprint(&_s, 0, c, (o)); \
    string_renew_prepend_f(&_s, "%s:\n\n", (n)); \
    tlog("%s", _s); \
    string_delete(&_s); \
  } \

/* Print object into a string.
 */
#define aprint(ps, i, c, o) mprint(ps, i, c##_print, o)

/* Debug print of an internal-class object.
 */
#define dprint_(i, c, o) mprint(null, i, c##_print_, o)

/* Debug print of an internal-class object that has an extra argument x.
 */
#define dprintx_(i, c, o, x) mprintx(null, i, c##_print_, o, x)

/* Print internal-class object into a string.
 */
#define aprint_(ps, i, c, o) mprint(ps, i, c##_print_, o)

/* Print internal-class object with extra argument x into a string.
 */
#define aprintx_(ps, i, c, o, x) mprintx(ps, i, c##_print_, o, x)

/* Print into an arbitrary string (or null for debug print) using the specified
 * standard-format print method. Pieces together the following internal macros.
 */
#define mprint(ps, i, m, o) \
  mprintI1((ps), (i), (o)); \
  mprintI2((m), (o)); \
  mprintI3;

/* Same, but for a print method m that takes an extra argument x.
 */
#define mprintx(ps, i, m, o, x) \
  mprintI1((ps), (i), (o)); \
  mprintI2x((m), (o), (x)); \
  mprintI3;

/* First part of the above.
 */
#define mprintI1(ps, i, o) \
  g_LOG_file_ = __FILE__; \
  g_LOG_line_ = __LINE__; \
  \
  { \
    Printer *_printer = null; \
    \
    printer_new(&_printer, (ps), (i));

/* Second part of the above.
 */
#define mprintI2(m, o) \
    m((o), _printer);

/* Alternate second part with extra argument x.
 */
#define mprintI2x(m, o, x) \
    m((o), _printer, x);

/* Last part of the above.
 */
#define mprintI3 \
    printer_delete(&_printer); \
  }

/* A print function that just prints the pointer's address.
 */
void pointer_print(void *this, Printer *p);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _H */
