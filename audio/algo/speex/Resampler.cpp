/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */


#include <audio/algo/speex/Resampler.h>
#include <audio/algo/speex/debug.h>
#include <cmath>

#undef __class__
#define __class__ "algo::speex::Resampler"

audio::algo::speex::Resampler::Resampler() :
  #ifdef HAVE_SPEEX_DSP
    m_speexResampler(nullptr),
  #endif
  m_isConfigured(false) {
	
}

audio::algo::speex::Resampler::~Resampler() {
	#ifdef HAVE_SPEEX_DSP
		if (m_speexResampler != nullptr) {
			speex_resampler_destroy(m_speexResampler);
			m_speexResampler = nullptr;
		}
	#endif
}

void audio::algo::speex::Resampler::init(int8_t _nbChannel, float _inputSampleRate, float _outputSampleRate, int8_t _quality) {
	#ifdef HAVE_SPEEX_DSP
		if (m_speexResampler != nullptr) {
			speex_resampler_destroy(m_speexResampler);
			m_speexResampler = nullptr;
		}
		AA_SPEEX_DEBUG("Create resampler for : " << _inputSampleRate << " to " << _outputSampleRate);
		int err = 0;
		m_speexResampler = speex_resampler_init(_nbChannel,
		                                        _inputSampleRate,
		                                        _outputSampleRate,
		                                        _quality, &err);
		m_isConfigured = true;
	#else
		AA_SPEEX_WARNING("SPEEX DSP lib not accessible ==> can not resample");
		m_isConfigured = false;
	#endif
}

std::vector<enum audio::format> audio::algo::speex::Resampler::getSupportedFormat() {
	std::vector<enum audio::format> out = getNativeSupportedFormat();
	return out;
}

std::vector<enum audio::format> audio::algo::speex::Resampler::getNativeSupportedFormat() {
	std::vector<enum audio::format> out;
	out.push_back(audio::format_float);
	out.push_back(audio::format_int16);
	return out;
}

void audio::algo::speex::Resampler::process(void* _output, size_t& _nbChunkOut, const void* _input, size_t _nbChunk, enum audio::format _format) {
	if (m_isConfigured == false) {
		AA_SPEEX_ERROR("Algo is not initialized...");
	}
	#ifdef HAVE_SPEEX_DSP
		switch (_format) {
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
				AA_SPEEX_ERROR("Can not Limit with unsupported format : " << _format);
				break;
		}
	#else
		AA_SPEEX_ERROR("Not build with speex DSP ... ");
		_nbChunkOut = _nbChunk/10;
	#endif
}

