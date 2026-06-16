#include "error.h"
#include "types.h"

#define X(i) #i,
static const i8 *error_strings[] = { ERR_LIST };
#undef X

const i8 *str_err_raw(errno_t err)
{
	return error_strings[err];
}

const i8 *str_err(isize err)
{
	/* we could get negative error code */
	if (err < 0)
		err *= -1;

	/* make sure we are in the range */
	if (err >= EOK && err <= EMAXERR)
		return str_err_raw((errno_t)err);

	return "nil";
}
