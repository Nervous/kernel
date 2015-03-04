/*
 * File: syslog.c
 * Author: Gabriel Laskar <gabriel@lse.epita.fr>
 *
 * Description: Small syslog module.
 */

#include <kernel/arch/current.h>
#include <kernel/char.h>
#include <kernel/errno.h>
#include <kernel/klog.h>
#include <kernel/kmalloc.h>
#include <kernel/module.h>
#include <kernel/string.h>
#include <kernel/user.h>
#include <kernel/vfs.h>

static ssize_t syslog_write(struct file* f, const char* buf, size_t len)
{
	char* tmp_buf = kmalloc(len + 1);
	if (!tmp_buf)
		return -ENOMEM;

	int res = copy_user_buf(tmp_buf, buf, len);
	if (res < 0)
		goto err;

	res = len;
	tmp_buf[len] = 0;

	klog("syslog: %u: %s\n", get_current()->pid, tmp_buf);

err:
	kfree(tmp_buf, len + 1);
	return res;
}

static const struct file_ops syslog_fops = {
	.open = default_open,
	.write = syslog_write,
	.release = default_release
};

static void __init_once init(void)
{
	struct char_dev* chr = kmalloc(sizeof(struct char_dev));
	if (!chr)
		return;

	chr->ops = &syslog_fops;
	register_char_dev(chr, "klog");

	register_minor(chr->dev, chr);
}
MODINFO {
	module_name("syslog"),
	module_init_once(init),
	module_deps(M_CHAR | M_KMALLOC | M_SYSCALL)
};
