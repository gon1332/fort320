#ifndef __ERRCHECK_H__
#define __ERRCHECK_H__

#define _ERRCHECK
#define SafeCall(err)	__safe_call(err, __FILE__, __LINE__)


void __safe_call (void *err, const char *file, const int line);

#endif /* __RROR_CHECKS_H__ */
