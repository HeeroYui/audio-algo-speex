/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <etk/types.h>
#include <audio/format.h>
#include <chrono>
#include <memory>
#include <vector>

namespace audio {
	namespace algo {
		namespace speex {
			class ResamplerPrivate;
			class Resampler {
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
					 * @param[in] _format Input/output data format.
					 */
					virtual void init(int8_t _nbChannel, float _inputSampleRate, float _outputSampleRate, int8_t _quality=4, enum audio::format _format = audio::format_float);
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
					 */
					virtual void process(void* _output, size_t& _nbChunkOut, const void* _input, size_t _nbChunk);
				protected:
					std11::shared_ptr<ResamplerPrivate> m_private; // private data.
			};
		}
	}
}

