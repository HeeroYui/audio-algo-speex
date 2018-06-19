/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#define speex_POSIX 1
#include <audio/algo/speex/Aec.hpp>
#include <audio/algo/speex/debug.hpp>
#include <cmath>

#include <speex/speex_echo.h>
#include <speex/speex_preprocess.h>


namespace audio {
	namespace algo {
		namespace speex {
			class AecPrivate {
				private:
					SpeexEchoState* m_speexEchoState = nullptr;
					SpeexPreprocessState* m_speexPreprocessState = nullptr;
					enum audio::format m_format;
					float m_sampleRate;
					int8_t m_nbChannel;
					etk::Vector<float> m_tmpBufferIn;
					etk::Vector<float> m_tmpBufferOut;
					int32_t m_frameSize;
					uint32_t calculateBlockSize(uint32_t _sampleRate, uint32_t _sizeInMs) {
						uint32_t nframes = (_sampleRate * _sizeInMs) / 1000;
						uint32_t y = 1 << ((8 * sizeof(uint32_t)) - 2);
						if(_sampleRate < 4000) {
							AA_SPEEX_INFO("speex: request Sample rate < 4000: " << _sampleRate);
							return 32;
						}
						if(_sizeInMs < 1) {
							AA_SPEEX_INFO("speex: request size in ms < 1: " << _sizeInMs);
							return 32;
						}
						// calculate power of 2:
						while (y > nframes) {
							y >>= 1;
						}
						return y;
					}
				public:
					AecPrivate(int8_t _nbChannel, float _sampleRate, enum audio::format _format) :
					  m_format(_format),
					  m_sampleRate(_sampleRate),
					  m_nbChannel(_nbChannel) {
						bool paramAGC = false;
						bool paramDenoise = false;
						bool paramEchoSuppress = true;
						int32_t paramEchoSuppressAttenuation = -40;
						int32_t paramEchoSuppressAttenuationActive = -15;
						uint32_t filterSizeMs = 200; // Buffer size in ms [100-500]
						uint32_t frameSizeMs = 20; // Frame size in ms [10..20]
						m_frameSize = calculateBlockSize(m_sampleRate, frameSizeMs);
						AA_SPEEX_INFO("Using framesize: " << m_frameSize);
						/*
						m_speexEchoState = speex_echo_state_init_mc(m_frameSize,
						                                            (m_sampleRate * filterSizeMs) / 1000,
						                                            m_nbChannel, // n microphone channel ...
						                                            1 // 1 speaker
						                                            );
						*/
						m_speexEchoState = speex_echo_state_init(m_frameSize, calculateBlockSize(m_sampleRate,filterSizeMs));//(m_sampleRate*filterSizeMs)/1000);
						if (m_speexEchoState == nullptr) {
							AA_SPEEX_ERROR("Can not intanciate speex handle");
							return;
						}
						int sampleRateToInject = _sampleRate;
						speex_echo_ctl(m_speexEchoState, SPEEX_ECHO_SET_SAMPLING_RATE, &sampleRateToInject);
						
						if (    paramAGC == true
						     || paramDenoise == true
						     || paramEchoSuppress == true) {
							spx_int32_t tmp;
							if (_nbChannel != 1) {
								AA_SPEEX_ERROR("support only one channel ... ==> need to create multiple channel instance ...");
								return;
							}
							m_speexPreprocessState = speex_preprocess_state_init(m_frameSize, m_sampleRate);
							AA_SPEEX_INFO("Set speex preprocessor AGC=" << paramAGC);
							tmp = paramAGC;
							speex_preprocess_ctl(m_speexPreprocessState, SPEEX_PREPROCESS_SET_AGC, &tmp);
							AA_SPEEX_INFO("Set speex preprocessor DENOISE=" << paramDenoise);
							tmp = paramDenoise;
							speex_preprocess_ctl(m_speexPreprocessState, SPEEX_PREPROCESS_SET_DENOISE, &tmp);
							
							if (paramEchoSuppress) {
								if (paramEchoSuppressAttenuation) {
									int32_t tmpEchoSuppressAttenuation;
									speex_preprocess_ctl(m_speexPreprocessState, SPEEX_PREPROCESS_GET_ECHO_SUPPRESS, &tmpEchoSuppressAttenuation);
									AA_SPEEX_INFO("Set speex preprocessor AEC attenuation=" << paramEchoSuppressAttenuation << "  base=" << tmpEchoSuppressAttenuation);
									speex_preprocess_ctl(m_speexPreprocessState, SPEEX_PREPROCESS_SET_ECHO_SUPPRESS, &paramEchoSuppressAttenuation);
								}
								if (paramEchoSuppressAttenuationActive) {
									int32_t tmpEchoSuppressAttenuationActive;
									speex_preprocess_ctl(m_speexPreprocessState, SPEEX_PREPROCESS_GET_ECHO_SUPPRESS_ACTIVE, &tmpEchoSuppressAttenuationActive);
									AA_SPEEX_INFO("Set speex preprocessor AEC attenuation enable=" << paramEchoSuppressAttenuationActive << "  base=" << tmpEchoSuppressAttenuationActive);
									speex_preprocess_ctl(m_speexPreprocessState, SPEEX_PREPROCESS_SET_ECHO_SUPPRESS_ACTIVE, &paramEchoSuppressAttenuationActive);
								}
								speex_preprocess_ctl(m_speexPreprocessState, SPEEX_PREPROCESS_SET_ECHO_STATE, m_speexEchoState);
							}
						} else {
							AA_SPEEX_ERROR("request speex with no algorithm");
						}
					}
				
				~AecPrivate() {
					/* TODO : Check this leak ...
					if (m_speexPreprocessState) {
						speex_preprocess_state_destroy(m_speexPreprocessState);
						m_speexPreprocessState = nullptr;
					}
					if (m_speexEchoState) {
						speex_echo_state_destroy(m_speexEchoState);
						m_speexEchoState = nullptr;
					}
					*/
				}
				
				/**
				 * @brief Main input algo process.
				 * @param[in,out] _output Output data.
				 * @param[in] _input Input data.
				 * @param[in] _inputFeedback Input feedback data (all time MONO).
				 * @param[in] _nbChunk Number of chunk in the input buffer.
				 * @param[in] _nbChannel Number of channel in the stream.
				 */
				void process(void* _output, const void* _input, const void* _inputFeedback, size_t _nbChunk) {
					if (m_speexEchoState == nullptr) {
						AA_SPEEX_ERROR("speex handle is not initialize ==> can not process");
						return;
					}
					switch (m_format) {
						case audio::format_int16:
							// process APC
							speex_echo_cancellation(m_speexEchoState, (const spx_int16_t*)_input, (const spx_int16_t*)_inputFeedback, (spx_int16_t*)_output);
							// apply NR after AEC
							if (m_speexPreprocessState != nullptr) {
								speex_preprocess_run(m_speexPreprocessState, (spx_int16_t*) _output);
							}
							return;
						default:
							AA_SPEEX_ERROR("Can not Limit with unsupported format : " << m_format);
							break;
					}
				}
				
				int32_t getOptimalFrameSize() {
					return m_frameSize;
				}
			};
		}
	}
}


void audio::algo::speex::Aec::init(int8_t _nbChannel, float _sampleRate, enum audio::format _format) {
	m_private.reset();
	m_private = ememory::makeShared<audio::algo::speex::AecPrivate>(_nbChannel, _sampleRate, _format);
}

etk::Vector<float> audio::algo::speex::Aec::getSupportedSampleRate() {
	etk::Vector<float> out;
	out.pushBack(4000);
	out.pushBack(8000);
	out.pushBack(16000);
	out.pushBack(32000);
	out.pushBack(48000);
	return out;
}
etk::Vector<enum audio::format> audio::algo::speex::Aec::getSupportedFormat() {
	return getNativeSupportedFormat();
}

etk::Vector<enum audio::format> audio::algo::speex::Aec::getNativeSupportedFormat() {
	etk::Vector<enum audio::format> out;
	//out.pushBack(audio::format_float);
	out.pushBack(audio::format_int16);
	return out;
}

void audio::algo::speex::Aec::process(void* _output, const void* _input, const void* _inputFeedback, size_t _nbChunk) {
	if (m_private == nullptr) {
		AA_SPEEX_ERROR("Algo is not initialized...");
	}
	m_private->process(_output, _input, _inputFeedback, _nbChunk);
}

int32_t audio::algo::speex::Aec::getOptimalFrameSize() {
	if (m_private == nullptr) {
		AA_SPEEX_ERROR("Algo is not initialized...");
		return 32;
	}
	return m_private->getOptimalFrameSize();
}

