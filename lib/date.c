#include "date.h"
#include <ctype.h>
#include <time.h>

#define LEN_ 10

static ushort parse_serial_(char *date);

static bool try_parse_serial_(ushort *serial, char *date);

static void render_(char **p_this, ushort serial);

static void validate_month_(byte month);

static void validate_day_of_week_(byte day_of_week);


void date_validate(char *date) {
  parse_serial_(date);
}

bool date_try_validate(char *date) {
  char *_test = null;
  ushort serial = 0;

  if (!try_parse_serial_(&serial, date)) {
    return false;
  }

  render_(&_test, serial);

  if (!string_equals(_test, date)) {
    throw("Impossible: Expected '%s' to equal '%s'", _test, date);
  }

  string_delete(&_test);

  return true;
}

void date_parse(char *date, byte *day, byte *month, ushort *year) {
  date_try_parse(date, day, month, year);
}

bool date_try_parse(char *date, byte *pday, byte *pmonth, ushort *pyear) {
  char d[LEN_ + 1];
  byte day;
  byte month;
  ushort year;

  if (string_length(date) != LEN_) {
    return false;
  }

  if (date[4] != '-' || date[7] != '-') {
    return false;
  }

  string_copy(date, d, LEN_);
  d[4] = '\0';
  d[7] = '\0';

  if (!isdigit(d[0]) ||
      !isdigit(d[1]) ||
      !isdigit(d[2]) ||
      !isdigit(d[3]) ||
      !try_parse_ushort(&year, d)) {
    return false;
  }

  if (year < 1901 || year > 2079) {
    return false;
  }

  if (!isdigit(d[5]) ||
      !isdigit(d[6]) ||
      !try_parse_byte_decimal(&month, d + 5)) {
    return false;
  }

  if (month < 1 || month > 12) {
    return false;
  }

  if (!isdigit(d[8]) ||
      !isdigit(d[9]) ||
      !try_parse_byte_decimal(&day, d + 8)) {
    return false;
  }

  if (day < 1 ||
      day > 31 ||
      (day == 31 && (month == 4 || month == 6 || month == 9 || month == 11)) ||
      (month == 2 && (day > 29 || (day == 29 && year % 4 != 0)))) {
    return false;
  }

  *pday = day;
  *pmonth = month;
  *pyear = year;

  return true;
}

byte date_day(char *date) {
  byte day;
  byte month;
  ushort year;

  date_parse(date, &day, &month, &year);
  return day;
}

byte date_month(char *date) {
  byte day;
  byte month;
  ushort year;

  date_parse(date, &day, &month, &year);
  return month;
}

char *date_month_shortname(char *date) {
  byte day;
  byte month;
  ushort year;

  date_parse(date, &day, &month, &year);
  return date_month_shortname_from_value(month);
}

char *date_month_shortname_from_value(byte month) {
  static char *names__[] = {
    "!!!", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep",
    "Oct", "Nov", "Dec"};

  validate_month_(month);
  return names__[month];
}

char *date_month_longname(char *date) {
  byte day;
  byte month;
  ushort year;

  date_parse(date, &day, &month, &year);
  return date_month_longname_from_value(month);
}

char *date_month_longname_from_value(byte month) {
  static char *names__[] = {
    "!!!", "January", "February", "March", "April", "May", "June", "July",
    "August", "September", "October", "November", "December"};

  validate_month_(month);
  return names__[month];
}

ushort date_year(char *date) {
  byte day;
  byte month;
  ushort year;

  date_parse(date, &day, &month, &year);
  return year;
}

byte date_day_of_week(char *date) {
  uint serial;

  serial = parse_serial_(date);
  return serial % 7;
}

char *date_day_of_week_shortname(char *date) {
  return date_day_of_week_shortname_from_index(date_day_of_week(date));
}

char *date_day_of_week_shortname_from_index(byte day_of_week) {
  static char *names__[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

  validate_day_of_week_(day_of_week);
  return names__[day_of_week];
}

char *date_day_of_week_longname(char *date) {
  return date_day_of_week_longname_from_index(date_day_of_week(date));
}

char *date_day_of_week_longname_from_index(byte day_of_week) {
  static char *names__[] = {"Monday", "Tuesday", "Wednesday", "Thursday",
    "Friday", "Saturday", "Sunday"};

  validate_day_of_week_(day_of_week);
  return names__[day_of_week];
}

char *date_min(void) {
  return "1901-01-01";
}

char *date_max(void) {
  return "2079-12-31";
}

void string_new_date_copy(char **p_this, char *other) {
  date_validate(other);
  string_new_copy(p_this, other);
}

void string_new_date_min(char **p_this) {
  string_new_copy(p_this, date_min());
}

void string_new_date_max(char **p_this) {
  string_new_copy(p_this, date_max());
}

void string_new_date_today(char **p_this, bool utc) {
  time_t now;
  struct tm t;

  now = time(null);

  t = time_t_to_tm(&now, utc);

  string_new_f(
      p_this,
      "%04d-%02d-%02d",
      t.tm_year + 1900,
      t.tm_mon + 1,
      t.tm_mday);
}

void string_new_date_yesterday(char **p_this, bool utc) {
  char *_today = null;

  string_new_date_today(&_today, utc);
  string_new_date_previous(p_this, _today);
  string_delete(&_today);
}

void string_new_date_next(char **p_this, char *date) {
  string_new_date_add(p_this, date, 1);
}

void string_new_date_previous(char **p_this, char *date) {
  string_new_date_subtract(p_this, date, 1);
}

void string_renew_date_increment(char **p_this) {
  char *_n = null;

  string_new_date_next(&_n, *p_this);
  string_delete(p_this);
  donate(&_n, p_this);
}

void string_renew_date_decrement(char **p_this) {
  char *_n = null;

  string_new_date_previous(&_n, *p_this);
  string_delete(p_this);
  donate(&_n, p_this);
}

void string_new_date_add(char **p_this, char *date, int add) {
  render_(p_this, parse_serial_(date) + add);
}

void string_new_date_subtract(char **p_this, char *date, int subtract) {
  string_new_date_add(p_this, date, -subtract);
}

void string_renew_date_add(char **p_this, int add) {
  char *_n = null;

  string_new_date_add(&_n, *p_this, add);
  string_delete(p_this);
  donate(&_n, p_this);
}

void string_renew_date_subtract(char **p_this, int subtract) {
  char *_n = null;

  string_new_date_subtract(&_n, *p_this, subtract);
  string_delete(p_this);
  donate(&_n, p_this);
}

int date_subtract(char * date1, char * date2) {
  return parse_serial_(date1) - parse_serial_(date2);
}

bool date_equals(char *date1, char *date2) {
  return parse_serial_(date1) == parse_serial_(date2);
}

bool date_earlier(char *date1, char *date2) {
  return parse_serial_(date1) < parse_serial_(date2);
}

bool date_earlier_or_equal(char *date1, char *date2) {
  return parse_serial_(date1) <= parse_serial_(date2);
}

bool date_later(char * date1, char * date2) {
  return parse_serial_(date1) > parse_serial_(date2);
}

bool date_later_or_equal(char * date1, char * date2) {
  return parse_serial_(date1) >= parse_serial_(date2);
}

void date_increment(char *this) {
  char *_next = null;

  string_new_date_next(&_next, this);
  string_copy(_next, this, LEN_);
  string_delete(&_next);
}

void date_decrement(char * this) {
  char *_previous = null;

  string_new_date_previous(&_previous, this);
  string_copy(_previous, this, LEN_);
  string_delete(&_previous);
}


static ushort parse_serial_(char *date) {
  ushort serial = 0;

  if (!try_parse_serial_(&serial, date)) {
    throw("Could not parse '%s' as a 'YYYY-MM-DD' date string", date);
  }

  return serial;
}

static bool try_parse_serial_(ushort *serial, char *date) {
  byte day;
  byte month;
  ushort year;

  if (!date_try_parse(date, &day, &month, &year)) {
    return false;
  }

  if (month < 3) {
    month += 12;
    year--;
  }

  *serial = day + (153 * month - 457) / 5 + 365 * year + year / 4 - 694281;

  return true;
}

static void render_(char **p_this, ushort serial) {
  byte day;
  byte month;
  ushort year;
  uint z;
  uint h;
  uint a;
  uint b;
  uint c;

  z = serial + 694266;
  h = 100 * z - 25;
  a = h / 3652425;
  b = a - a / 4;
  year = (ushort)((100 * b + h) / 36525);
  c = b + z - 365 * year - year / 4;
  month = (byte)((5 * c + 456) / 153);
  day = (byte)(c - (153 * month - 457) / 5);

  if (month > 12) {
    year++;
    month -= 12;
  }

  string_new_f(p_this, "%04u-%02u-%02u", year, month, day);
}

static void validate_month_(byte month) {
  if (month == 0 || month > 12) {
    throw("Invalid month value %u: must be in [1, 12]", month);
  }
}

static void validate_day_of_week_(byte day_of_week) {
  if (day_of_week > 6) {
    throw("Invalid day of week index %u: must be in [0, 6]", day_of_week);
  }
}
