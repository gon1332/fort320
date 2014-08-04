#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../../include/DebugInfo/errcheck.h"


/* void __safe_call (void *err,        Return value from function call
 *		     const char *file, The file in which the call occured
 *		     const int line)   The line in the file in which the call occured
 *
 * Checks if everything is ok in the code. If not, it stops the execution.
 */
void __safe_call (void *err, const char *file, const int line)
{
#ifdef _ERRCHECK
	if (NULL == err) {
		char err_msg[60];
		memset(err_msg, '\0', 60);
		sprintf(err_msg, "SafeCall() failed at %s:%i : ", file, line);
		perror(err_msg);
		exit(EXIT_FAILURE);
	}
#endif

	return;
}
