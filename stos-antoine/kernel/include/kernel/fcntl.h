/*
 * File: fcntl.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: File operations options.
 */
#ifndef FCNTL_H_
# define FCNTL_H_

# define SEEK_SET	0
# define SEEK_CUR	1
# define SEEK_END	2

/*
 * Historically, O_RDONLY was 0, thus O_RDONLY | O_WRONLY was correct,
 * define those flags so that O_RDWR == O_RDONLY | O_WRONLY
 */
# define O_RDONLY		01
# define O_WRONLY		02
# define O_RDWR			(O_RDONLY | O_WRONLY)
# define O_CREAT		0100
# define O_EXCL			0200
# define O_NOCTTY		0400
# define O_TRUNC		01000
# define O_APPEND		02000
# define O_NONBLOCK		04000
# define O_CLOEXEC		010000
# define O_DIRECTORY		020000

/* fcntl(2) arguments */
# define F_DUPFD		1
# define F_DUP2FD		2
# define F_GETFD		3 /* Get FD_CLOEXEC */
# define F_SETFD		4 /* Set FD_CLOEXEC */
# define F_GETFL		5
# define F_SETFL		6

# define FD_CLOEXEC		O_CLOEXEC

#endif /* !FCNTL_H_ */
