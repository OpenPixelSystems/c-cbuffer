thread_dep = dependency('threads')

main_test = executable('logger_test','main.c',
			include_directories:inc,
			dependencies : thread_dep,
			link_with : cbufferlib)
test('Main test', main_test)
