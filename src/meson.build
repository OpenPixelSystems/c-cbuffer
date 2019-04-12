cbuffersrc = ['cbuffer.c']
thread_dep = dependency('threads')

cbufferlib = shared_library(
    	'cbufferlib', # library name
	cbuffersrc, # source files to be compiled
    	include_directories : inc,
        dependencies : thread_dep,
        install : false)
