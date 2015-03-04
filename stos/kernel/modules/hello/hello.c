#include <kernel/klog.h>
#include <kernel/module.h>
#include <kernel/stos.h>
#include <kernel/test.h>

static void init(void);

MODINFO {
    module_name("hello"),
    //module_type(),
    // module_init(), on commente car mono cpu
    module_init_once(init),
    module_deps(M_TEST)
};

void testHello(struct gtest* test) {
    tprint("%s", "Hello !")
    test->res = 1; // success
}

void failFunction(struct gest* test) {
    tprint("%s", "Failed ...")
   while (1);
}

static void __init_once init(void) {
    register_gtest("test_helloWorld", testHello, NULL, failFunction);
    klog("Hello world !\n");
}
