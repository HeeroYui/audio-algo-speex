#!/usr/bin/python
import realog.debug as debug
import lutin.tools as tools


def get_type():
	return "LIBRARY"

def get_desc():
	return "speex algos wrapper"

def get_licence():
	return "MPL-2"

def get_compagny_type():
	return "com"

def get_compagny_name():
	return "atria-soft"

def get_maintainer():
	return "authors.txt"

def get_version():
	return "version.txt"

def configure(target, my_module):
	my_module.add_src_file([
	    'audio/algo/speex/debug.cpp',
	    'audio/algo/speex/Resampler.cpp',
	    'audio/algo/speex/Aec.cpp',
	    'audio/algo/speex/Vad.cpp',
	    ])
	my_module.add_header_file([
	    'audio/algo/speex/Resampler.hpp',
	    'audio/algo/speex/Aec.hpp',
	    'audio/algo/speex/Vad.hpp',
	    ])
	my_module.add_depend([
	    'etk',
	    'audio',
	    'webrtc',
	    ])
	my_module.add_optionnal_depend('speex-dsp', ["c++", "-DHAVE_SPEEX_DSP"])
	my_module.add_path(".")
	return True









