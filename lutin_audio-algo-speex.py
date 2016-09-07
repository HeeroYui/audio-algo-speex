#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools


def get_type():
	return "LIBRARY"

def get_desc():
	return "speex algos wrapper"

def get_licence():
	return "APACHE-2"

def get_compagny_type():
	return "com"

def get_compagny_name():
	return "atria-soft"

def get_maintainer():
	return ["Mr DUPIN Edouard <yui.heero@gmail.com>"]

def get_version():
	return [0,1,"dev"]

def create(target, module_name):
	my_module = module.Module(__file__, module_name, get_type())
	my_module.add_src_file([
		'audio/algo/speex/debug.cpp',
		'audio/algo/speex/Resampler.cpp'
		])
	my_module.add_header_file([
		'audio/algo/speex/Resampler.h'
		])
	my_module.add_depend(['etk', 'audio'])
	my_module.add_optionnal_depend('speex-dsp', ["c++", "-DHAVE_SPEEX_DSP"])
	my_module.add_path(tools.get_current_path(__file__))
	return my_module









