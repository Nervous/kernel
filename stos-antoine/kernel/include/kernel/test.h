/*
 * File: test.h
 * Author: Louis Feuvrier <manny@lse.epita.fr>
 *
 * Description: stos global and unit tests header.
 */

#ifndef TEST_H_
# define TEST_H_

# include <kernel/klog.h>

/* tprint - printing function to be used by the tests.
 *
 * Temporarily a simple macro the forwards the printing to the klog function.
 * Could be a normal function in the future.
 */
# define tprint(...) do { klog(__VA_ARGS__); } while (0);

/* struct gtest - describe a global test
 *
 * name: name of the test
 * next: node in the list of tests to be executed
 * res: result of the test function when executed
 * failure: function to be executed if the test fails
 * test: test to be executed
 * data: data to pass to the test when executing the test suite
 */
struct gtest {
	const char *name;
	struct list_node next;

	int res;
	void (*failure)(struct gtest*);

	void (*test)(struct gtest*);
	void *data;
};

/* register_gtest - register a global test to be executed by the test suite
 * later on in the boot process.
 *
 * @name: name of the test (will be copied)
 * @test: test function
 * @data: data to be passed to test function at execution
 * @failure: failure function to be executed when the test fails.
 */
int register_gtest(const char *name, void (*test)(struct gtest*), void *data,
	void (*failure)(struct gtest*));

/* execute_gtests - function to execute the test suite right before (or instead
 * of) starting the userland with the start_uland module.
 */
void execute_gtests(void);

#endif /* !TEST_H_ */
