/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <etk/types.hpp>
#include <audio/format.hpp>
#include <chrono>
#include <ememory/memory.hpp>
#include <vector>

namespace audio {
	namespace algo {
		namespace speex {
			class VadPrivate;
			class Vad {
				public:
					Vad() = default;
					virtual ~Vad() = default;
				public:
					/**
					 * @brief Initialize the Algorithm
					 * @param[in] _nbChannel Number of channel in the stream.
					 * @param[in] _sampleRate Sample rate.
					 * @param[in] _format Input/output data format.
					 */
					virtual void init(int8_t _nbChannel, float _sampleRate, enum audio::format _format = audio::format_float);
					/**
					 * @brief Get list of samplerate suported.
					 * @return list of supported samplerate
					 */
					virtual etk::Vector<float> getSupportedSampleRate();
					/**
					 * @brief Get list of format suported in input.
					 * @return list of supported format
					 */
					virtual etk::Vector<enum audio::format> getSupportedFormat();
					/**
					 * @brief Get list of algorithm format suported. No format convertion.
					 * @return list of supported format
					 */
					virtual etk::Vector<enum audio::format> getNativeSupportedFormat();
					/**
					 * @brief Main input algo process.
					 * @param[in,out] _output Output data.
					 * @param[in] _input Input data.
					 * @param[in] _inputFeedback Input feedback data (all time MONO).
					 * @param[in] _nbChunk Number of chunk in the input buffer.
					 * @param[in] _nbChannel Number of channel in the stream.
					 */
					virtual void process(const void* _input, size_t _nbChunk);
					
					bool getVoiceDetected();
					
					int32_t getOptimalFrameSize();
				protected:
					ememory::SharedPtr<VadPrivate> m_private; //!< private data.
			};
		}
	}
}

