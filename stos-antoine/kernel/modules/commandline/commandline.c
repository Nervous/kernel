#include <kernel/stos.h>
#include <kernel/kmalloc.h>
#include <kernel/module_loader.h>
#include <kernel/module.h>
#include <kernel/string.h>
#include <kernel/strutils.h>
#include <boot/interface.h>

/*
 * A module parameter
 */
struct cmdline_mod_param {
	char* module;
	char* name;
	char* value;
};

/*
 * Get a module parameter from the command line.
 */
const char* get_mod_parameter(char* param_name);

static struct cmdline_mod_param parse_param(char* param_def)
{
	struct cmdline_mod_param param;

	char* sub_toksave;
	char* subsub_toksave;

	char* full_name = strtok_r(param_def, "=", &sub_toksave);
	char* module_name = strtok_r(full_name, ".", &subsub_toksave);
	char* param_name = strtok_r(NULL, ".", &subsub_toksave);
	param.module = kmalloc((strlen(module_name) + 1) * sizeof (char));
	param.name = kmalloc((strlen(param_name) + 1) * sizeof (char));
	strcpy(param.module, module_name);
	strcpy(param.name, param_name);

	char* value = strtok_r(NULL, "=", &sub_toksave);
	param.value = kmalloc((strlen(value) + 1) * sizeof (char));
	strcpy(param.value, value);

	return param;
}

static void* get_param(const char* module, const char* param)
{
	char* working_cmdline;
	struct boot_interface* boot = get_boot_interface();
	working_cmdline = kmalloc(strlen(boot->commandline) + 1);
	strcpy(working_cmdline, boot->commandline);

	char* toksave;
	char* param_def = strtok_r(working_cmdline, " ", &toksave);

	char* ret = NULL;
	while (param_def) {
		if (strchr(param_def, '=')) {
			struct cmdline_mod_param p = parse_param(param_def);

			if (!strcmp(module, p.module) && !strcmp(param, p.name))
				ret = p.value;

			kfree(p.module, (strlen(p.module) + 1) * sizeof (char));
			kfree(p.name, (strlen(p.name) + 1) * sizeof (char));

			if (ret)
				return ret;

			kfree(p.value, (strlen(p.value) + 1) * sizeof (char));
		}
		param_def = strtok_r(NULL, " ", &toksave);
	}

	kfree(working_cmdline, strlen(boot->commandline) + 1);

	return NULL;
}

static int load_params(struct module* module)
{
	/* Find the parameters declared in the module */
	struct modparam* params_info = NULL;
	size_t section_len = 0;
	for (int i = 0; i < module->nb_section; i++) {
		if (!strcmp(module->sections[i].name, ".modparam")) {
			void* params_vaddr;
			params_vaddr = module->sections[i].reloc->reloc_vaddr;
			params_info = (struct modparam*)params_vaddr;

			section_len = module->sections[i].len;
			section_len /= sizeof (struct modparam);
			break;
		}
	}

	/* Set those arguments acording to the commandline */
	if (params_info) {
		for (size_t i = 0; i < section_len; i++) {
			struct modparam mp = params_info[i];
			char* val = get_param(mp.modinfo->name, mp.param_name);
			*(char**)mp.param_dest = val;
		}
	}
	return 0;
}

extern struct modparam _modparam_start[];
extern struct modparam _modparam_end[];

static void load_static_modules_arguments(void)
{
	struct modparam* mod_start = _modparam_start;
	struct modparam* mod_end = _modparam_end;

	for (struct modparam* mod = mod_start; mod < mod_end; mod++) {
		char* val = get_param(mod->modinfo->name, mod->param_name);
		*(char**)mod->param_dest = val;
	}
}

static void __init_once init(void)
{
	add_section_loader(".modparam", default_load_section);
	register_pre_init_module_handler(load_params);

	load_static_modules_arguments();
}

MODINFO {
	module_name("commandline"),
	module_init_once(init),
	module_deps(M_KMALLOC | M_LIBRARIES),
	module_type(M_COMMANDLINE)
};
