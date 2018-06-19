/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */


#include <audio/algo/speex/Resampler.hpp>
#include <audio/algo/speex/debug.hpp>
#include <cmath>
#ifdef HAVE_SPEEX_DSP
	#include <speex/speex_resampler.h>
#endif

namespace audio {
	namespace algo {
		namespace speex {
			class ResamplerPrivate {
				private:
					#ifdef HAVE_SPEEX_DSP
						SpeexResamplerState* m_speexResampler;
					#endif
					enum audio::format m_format;
				public:
					ResamplerPrivate(int8_t _nbChannel, float _inputSampleRate, float _outputSampleRate, int8_t _quality, enum audio::format _format) :
					  #ifdef HAVE_SPEEX_DSP
					    m_speexResampler(null),
					  #endif
					  m_format(_format) {
						#ifdef HAVE_SPEEX_DSP
							if (m_speexResampler != null) {
								speex_resampler_destroy(m_speexResampler);
								m_speexResampler = null;
							}
							AA_SPEEX_DEBUG("Create resampler for : " << _inputSampleRate << " to " << _outputSampleRate);
							int err = 0;
							m_speexResampler = speex_resampler_init(_nbChannel,
							                                        _inputSampleRate,
							                                        _outputSampleRate,
							                                        _quality, &err);
						#else
							AA_SPEEX_WARNING("SPEEX DSP lib not accessible ==> can not resample");
						#endif
					}
				
				~ResamplerPrivate() {
					#ifdef HAVE_SPEEX_DSP
						if (m_speexResampler != null) {
							speex_resampler_destroy(m_speexResampler);
							m_speexResampler = null;
						}
					#endif
				}
				
				void process(void* _output, size_t& _nbChunkOut, const void* _input, size_t _nbChunk) {
					#ifdef HAVE_SPEEX_DSP
						switch (m_format) {
							case audio::format_int16:
								{
									uint32_t nbChunkInput = _nbChunk;
									uint32_t nbChunkOutput = _nbChunkOut;
									int ret = speex_resampler_process_interleaved_int(m_speexResampler,
									                                                  reinterpret_cast<const int16_t*>(_input),
									                                                  &nbChunkInput,
									                                                  reinterpret_cast<int16_t*>(_output),
									                                                  &nbChunkOutput);
									// Check all input and output ...
									if (nbChunkInput != _nbChunk) {
										AA_SPEEX_ERROR("inputSize (not all read ...) proceed=" << nbChunkInput << " requested=" << _nbChunk);
										// TODO : manage this case ...
									}
									if (nbChunkOutput == _nbChunkOut) {
										AA_SPEEX_ERROR("Might have not enought data in output... output size=" << _nbChunkOut);
										// TODO : manage this case ...
									}
									_nbChunkOut = nbChunkOutput;
								}
								break;
							case audio::format_float:
								{
									uint32_t nbChunkInput = _nbChunk;
									uint32_t nbChunkOutput = _nbChunkOut;
									int ret = speex_resampler_process_interleaved_float(m_speexResampler,
									                                                    reinterpret_cast<const float*>(_input),
									                                                    &nbChunkInput,
									                                                    reinterpret_cast<float*>(_output),
									                                                    &nbChunkOutput);
									// Check all input and output ...
									if (nbChunkInput != _nbChunk) {
										AA_SPEEX_ERROR("inputSize (not all read ...) proceed=" << nbChunkInput << " requested=" << _nbChunk);
										// TODO : manage this case ...
									}
									if (nbChunkOutput == _nbChunkOut) {
										AA_SPEEX_ERROR("Might have not enought data in output... output size=" << _nbChunkOut);
										// TODO : manage this case ...
									}
									_nbChunkOut = nbChunkOutput;
								}
								break;
							default:
								AA_SPEEX_ERROR("Can not Limit with unsupported format : " << m_format);
								break;
						}
					#else
						AA_SPEEX_ERROR("Not build with speex DSP ... ");
						_nbChunkOut = _nbChunk/10;
					#endif
				}
			};
		}
	}
}


audio::algo::speex::Resampler::Resampler() {
	
}

audio::algo::speex::Resampler::~Resampler() {
	
}

void audio::algo::speex::Resampler::init(int8_t _nbChannel, float _inputSampleRate, float _outputSampleRate, int8_t _quality, enum audio::format _format) {
	m_private.reset();
	m_private = ememory::makeShared<audio::algo::speex::ResamplerPrivate>(_nbChannel, _inputSampleRate, _outputSampleRate, _quality, _format);
}

etk::Vector<enum audio::format> audio::algo::speex::Resampler::getSupportedFormat() {
	etk::Vector<enum audio::format> out = getNativeSupportedFormat();
	return out;
}

etk::Vector<enum audio::format> audio::algo::speex::Resampler::getNativeSupportedFormat() {
	etk::Vector<enum audio::format> out;
	out.pushBack(audio::format_float);
	out.pushBack(audio::format_int16);
	return out;
}

void audio::algo::speex::Resampler::process(void* _output, size_t& _nbChunkOut, const void* _input, size_t _nbChunk) {
	if (m_private == null) {
		AA_SPEEX_ERROR("Algo is not initialized...");
	}
	m_private->process(_output, _nbChunkOut, _input, _nbChunk);
}

