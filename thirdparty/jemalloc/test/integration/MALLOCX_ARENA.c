#include "test/jemalloc_test.h"

#define	NTHREADS 10

void *
thd_start(void *arg)
{
	unsigned thread_ind = (unsigned)(uintptr_t)arg;
	unsigned arena_ind;
	void *p;
	size_t sz;

	sz = sizeof(arena_ind);
	assert_d_eq(mallctl("arenas.extend", &arena_ind, &sz, NULL, 0), 0,
	    "Error in arenas.extend");

	if (thread_ind % 4 != 3) {
		size_t mib[3];
		size_t miblen = sizeof(mib) / sizeof(size_t);
		const char *dss_precs[] = {"disabled", "primary", "secondary"};
		const char *dss = dss_precs[thread_ind %
		    (sizeof(dss_precs)/sizeof(char*))];
		assert_d_eq(mallctlnametomib("arena.0.dss", mib, &miblen), 0,
		    "Error in mallctlnametomib()");
		mib[1] = arena_ind;
		assert_d_eq(mallctlbymib(mib, miblen, NULL, NULL, (void *)&dss,
		    sizeof(const char *)), 0, "Error in mallctlbymib()");
	}

	p = mallocx(1, MALLOCX_ARENA(arena_ind));
	assert_ptr_not_null(p, "Unexpected mallocx() error");
	dallocx(p, 0);

	return (NULL);
}

TEST_BEGIN(test_ALLOCM_ARENA)
{
	thd_t thds[NTHREADS];
	unsigned i;

	for (i = 0; i < NTHREADS; i++) {
		thd_create(&thds[i], thd_start,
		    (void *)(uintptr_t)i);
	}

	for (i = 0; i < NTHREADS; i++)
		thd_join(thds[i], NULL);
}
TEST_END

int
main(void)
{

	return (test(
	    test_ALLOCM_ARENA));
}
