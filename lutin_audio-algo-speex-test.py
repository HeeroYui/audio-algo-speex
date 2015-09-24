#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools

def get_desc():
	return "test for speex ALGO"


def create(target):
	my_module = module.Module(__file__, 'audio-algo-speex-test', 'BINARY')
	my_module.add_src_file([
		'test/main.cpp',
		'test/debug.cpp'
		])
	my_module.add_module_depend(['audio-algo-speex'])
	return my_module









