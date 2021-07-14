thread_dep = dependency('threads')

main_test = executable('benchmark_test','main.c', cbuff_srcs,
			include_directories:cbuff_includes,
			dependencies : thread_dep)
test('Main test', main_test)
