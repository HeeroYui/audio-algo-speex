#!/usr/bin/python
import lutinModule as module
import lutinTools as tools
import lutinDebug as debug

def get_desc():
	return "audio_algo_speex : speex algos wrapper"


def create(target):
	myModule = module.Module(__file__, 'audio_algo_speex', 'LIBRARY')
	myModule.add_src_file([
		'audio/algo/speex/debug.cpp',
		'audio/algo/speex/Resampler.cpp'
		])
	myModule.add_module_depend(['etk', 'audio'])
	
	myModule.add_optionnal_module_depend('speexdsp', "HAVE_SPEEX_DSP", True)
	
	myModule.add_export_path(tools.get_current_path(__file__))
	# return module
	return myModule









