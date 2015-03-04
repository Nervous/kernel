/*
 * File: mount.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: mount header.
 */
#ifndef _SYS_MOUNT_H
# define _SYS_MOUNT_H

# include <sys/cdefs.h>

__BEGIN_DECLS

int mount(const char* source, const char* target, const char* fsname);

__END_DECLS

#endif /* _SYS_MOUNT_H */
