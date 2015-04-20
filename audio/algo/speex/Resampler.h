/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */


#ifndef __AUDIO_ALGO_SPEEX_RESAMPLER_H__
#define __AUDIO_ALGO_SPEEX_RESAMPLER_H__

#include <etk/types.h>
#include <audio/format.h>
#include <etk/chrono.h>
#include <vector>
#ifdef HAVE_SPEEX_DSP
	#include <speex/speex_resampler.h>
#endif

namespace audio {
	namespace algo {
		namespace speex {
			class Resampler {
				protected:
					#ifdef HAVE_SPEEX_DSP
						SpeexResamplerState* m_speexResampler;
					#endif
					bool m_isConfigured;
				public:
					Resampler();
					virtual ~Resampler();
				public:
					/**
					 * @brief Initialize the Algorithm
					 * @param[in] _nbChannel Number of channel in the stream.
					 * @param[in] _inputSampleRate Input sample rate.
					 * @param[in] _outputSampleRate Output sample rate.
					 * @param[in] _quality Resampler quality [1..10].
					 */
					virtual void init(int8_t _nbChannel, float _inputSampleRate, float _outputSampleRate, int8_t _quality);
					/**
					 * @brief Get list of format suported in input.
					 * @return list of supported format
					 */
					virtual std::vector<enum audio::format> getSupportedFormat();
					/**
					 * @brief Get list of algorithm format suported. No format convertion.
					 * @return list of supported format
					 */
					virtual std::vector<enum audio::format> getNativeSupportedFormat();
					/**
					 * @brief Main input algo process.
					 * @param[in,out] _output Output data.
					 * @param[in,out] _nbChunkOut Number of chunk allocated in the output buffer and return the number of chunk in the buffer.
					 * @param[in] _input Input data.
					 * @param[in] _nbChunk Number of chunk in the input buffer.
					 * @param[in] _nbChannel Number of channel in the stream.
					 * @param[in] _format Input data format.
					 */
					virtual void process(void* _output, size_t& _nbChunkOut, const void* _input, size_t _nbChunk, enum audio::format _format = audio::format_float);
			};
		}
	}
}

#endif
