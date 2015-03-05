/*
 * File: wait.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: header file for wait()
 */
#ifndef _SYS_WAIT_H_
#define _SYS_WAIT_H_

#include <sys/cdefs.h>
#include <sys/types.h>

__BEGIN_DECLS

pid_t wait(int* status);
pid_t waitpid(pid_t pid, int* status, int options);

__END_DECLS

#endif /* _SYS_WAIT_H_ */
