#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools

def get_desc():
	return "test for speex ALGO"


def create(target):
	myModule = module.Module(__file__, 'audio-algo-speex-test', 'BINARY')
	myModule.add_src_file([
		'test/main.cpp',
		'test/debug.cpp'
		])
	myModule.add_module_depend(['audio-algo-speex'])
	return myModule









