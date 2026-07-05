#include "printf.h"
#include "console.h"
#include <stdarg.h>
#include "types.h"

static u8 digits[] = "0123456789ABCDEF";

/* static function declarations */
static void _print_int(u8 buf[static 32], i32 *size, i64 x, u8 base, bool sign);
static void print_int(i64 x, u8 base, bool sign);
static void print_double(f64 f, i32 precision);
static void print_string(const u8 *s);

void printf_init()
{
}

void printf_deinit(void)
{
}

// NOLINTBEGIN(clang-analyzer-valist.Uninitialized,
// clang-analyzer-valist.Uninitialized)
void printf(const i8 *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}

void vprintf(const i8 *fmt, va_list ap)
{
	bool state_format_specifier = false;
	for (usize i = 0; fmt[i] != 0; i++) {
		u8 ch = fmt[i] & 0xFF;
		if (!state_format_specifier) {
			/* not in format specifier mode */
			if ('%' == ch) {
				state_format_specifier = true;
			} else {
				console_write_char(ch);
				if (ch == '\n') {
					console_write_char('\r');
				}
			}
		} else {
			/* in format specifier mode */
			if ('d' == ch) {
				print_int(va_arg(ap, i64), 10, true);
			} else if ('x' == ch) {
				print_int(va_arg(ap, i64), 16, false);
			} else if ('f' == ch) {
				print_double(va_arg(ap, f64), 12);
			} else if ('p' == ch) {
				console_write_char('0');
				console_write_char('x');
				print_int(va_arg(ap, i64), 16, false);
			} else if ('s' == ch) {
				u8 *s = va_arg(ap, u8 *);
				print_string(s);
			} else if ('c' == ch) {
				console_write_char((u8)va_arg(ap, i32));
			} else if ('b' == ch) {
				bool v = (bool)va_arg(ap, i32);
				print_string(
					(const u8 *)(v ? "true" : "false"));
			} else if ('%' == ch) {
				console_write_char('%');
			} else {
				console_write_char('%');
				console_write_char(ch);
			}
			state_format_specifier = false;
		}
	}
}

// NOLINTEND(clang-analyzer-valist.Uninitialized,
// clang-analyzer-valist.Uninitialized)

static void _print_int(u8 buf[static 32], i32 *size, i64 x, u8 base, bool sign)
{
	u64 xx = 0;
	bool is_negative = false;
	if (sign && x < 0) {
		is_negative = true;
		xx = (u64)-x;
	} else {
		xx = (u64)x;
	}

	i32 i = 0;
	do {
		buf[i++] = digits[xx % base];
	} while ((xx /= base) != 0);

	if (is_negative) {
		buf[i++] = '-';
	}
	*size = i;
}

static void print_int(i64 x, u8 base, bool sign)
{
	u8 buf[32] = { 0 };
	i32 i;
	_print_int(buf, &i, x, base, sign);
	while (--i >= 0)
		console_write_char(buf[i]);
}

static void print_double(f64 f, i32 precision)
{
	bool is_negative = false;
	if (f < 0) {
		is_negative = true;
		f = -f;
	}

	u64 integer = (u64)f;
	double fraction = f - (f64)integer;
	u8 buf_integer[32] = { 0 };
	i32 integer_buf_size;
	_print_int(buf_integer, &integer_buf_size, (i32)integer, 10, false);

	u8 buf_float[16] = { 0 };
	i32 float_buf_size = 0;
	for (; float_buf_size < precision; float_buf_size++) {
		fraction *= 10.0;
		i32 digit = (i32)fraction;
		buf_float[float_buf_size++] = '0' + (u8)digit;
		fraction -= digit;
	}

	if (is_negative)
		console_write_char('-');

	while (--integer_buf_size >= 0)
		console_write_char(buf_integer[integer_buf_size]);

	console_write_char('.');

	i32 i = 0;
	while (i < float_buf_size)
		console_write_char(buf_float[i++]);
}

static void print_string(const u8 *s)
{
	while (*s != 0) {
		console_write_char(*s);
		if (*s == '\n') {
			console_write_char('\r');
		}
		s++;
	}
}
