/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#define speex_POSIX 1
#include <audio/algo/speex/Vad.hpp>
#include <audio/algo/speex/debug.hpp>
#include <cmath>

#include <webrtc/common_audio/vad/include/webrtc_vad.h>

namespace audio {
	namespace algo {
		namespace speex {
			class VadPrivate {
				private:
					VadInst *m_vad = null;
					bool m_voiceDetected = false;
					uint32_t calculateBlockSize(uint32_t _sampleRate, uint32_t _sizeInMs) {
						return 0;
					}
				public:
					VadPrivate() {
						WebRtcVad_Create(&m_vad);
						WebRtcVad_Init(m_vad);
					}
				
				~VadPrivate() {
					/* TODO : Check this leak ...
					if (m_speexPreprocessState) {
						speex_preprocess_state_destroy(m_speexPreprocessState);
						m_speexPreprocessState = null;
					}
					if (m_speexEchoState) {
						speex_echo_state_destroy(m_speexEchoState);
						m_speexEchoState = null;
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
				void process(const void* _input, size_t _nbChunk) {
					if (m_vad == null) {
						AA_SPEEX_ERROR("speex handle is not initialize ==> can not process");
						return;
					}
					m_voiceDetected = WebRtcVad_Process(m_vad, 16000, (const int16_t*)_input, _nbChunk);
				}
				
				int32_t getOptimalFrameSize() {
					return 160;
				}
				
				bool getVoiceDetected() {
					return m_voiceDetected;
				}
			};
		}
	}
}


void audio::algo::speex::Vad::init(int8_t _nbChannel, float _sampleRate, enum audio::format _format) {
	m_private.reset();
	m_private = ememory::makeShared<audio::algo::speex::VadPrivate>();
}

etk::Vector<float> audio::algo::speex::Vad::getSupportedSampleRate() {
	etk::Vector<float> out;
	out.pushBack(16000);
	return out;
}
etk::Vector<enum audio::format> audio::algo::speex::Vad::getSupportedFormat() {
	return getNativeSupportedFormat();
}

etk::Vector<enum audio::format> audio::algo::speex::Vad::getNativeSupportedFormat() {
	etk::Vector<enum audio::format> out;
	out.pushBack(audio::format_int16);
	return out;
}
 /// set 10 ms ==> 160 sample
void audio::algo::speex::Vad::process(const void* _input, size_t _nbChunk) {
	if (m_private == null) {
		AA_SPEEX_ERROR("Algo is not initialized...");
	}
	m_private->process(_input, _nbChunk);
}

int32_t audio::algo::speex::Vad::getOptimalFrameSize() {
	if (m_private == null) {
		AA_SPEEX_ERROR("Algo is not initialized...");
		return 32;
	}
	return m_private->getOptimalFrameSize();
}


bool audio::algo::speex::Vad::getVoiceDetected() {
	if (m_private == null) {
		AA_SPEEX_ERROR("Algo is not initialized...");
		return 32;
	}
	return m_private->getVoiceDetected();
}