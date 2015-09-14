#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools

def get_desc():
	return "speex algos wrapper"


def create(target):
	myModule = module.Module(__file__, 'audio-algo-speex', 'LIBRARY')
	myModule.add_src_file([
		'audio/algo/speex/debug.cpp',
		'audio/algo/speex/Resampler.cpp'
		])
	
	myModule.add_header_file([
		'audio/algo/speex/Resampler.h'
		])
	
	myModule.add_module_depend(['etk', 'audio'])
	
	myModule.add_optionnal_module_depend('speex-dsp', ["c++", "-DHAVE_SPEEX_DSP"])
	
	myModule.add_path(tools.get_current_path(__file__))
	# return module
	return myModule









