/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <test-debug/debug.hpp>
#include <etk/etk.hpp>
#include <audio/algo/speex/Resampler.hpp>
#include <etk/os/FSNode.hpp>
#include <chrono>

#include <unistd.h>

class Performance {
	private:
		std::chrono::steady_clock::time_point m_timeStart;
		std::chrono::steady_clock::time_point m_timeStop;
		std::chrono::nanoseconds m_totalTimeProcessing;
		std::chrono::nanoseconds m_minProcessing;
		std::chrono::nanoseconds m_maxProcessing;
		int32_t m_totalIteration;
	public:
		Performance() :
		  m_totalTimeProcessing(0),
		  m_minProcessing(99999999999999LL),
		  m_maxProcessing(0),
		  m_totalIteration(0) {
			
		}
		void tic() {
			m_timeStart = std::chrono::steady_clock::now();
		}
		void toc() {
			m_timeStop = std::chrono::steady_clock::now();
			std::chrono::nanoseconds time = m_timeStop - m_timeStart;
			m_minProcessing = std::min(m_minProcessing, time);
			m_maxProcessing = std::max(m_maxProcessing, time);
			m_totalTimeProcessing += time;
			m_totalIteration++;
			
		}
		
		std::chrono::nanoseconds getTotalTimeProcessing() {
			return m_totalTimeProcessing;
		}
		std::chrono::nanoseconds getMinProcessing() {
			return m_minProcessing;
		}
		std::chrono::nanoseconds getMaxProcessing() {
			return m_maxProcessing;
		}
		int32_t getTotalIteration() {
			return m_totalIteration;
		}
		
};

float performanceResamplerStepFloat(float _sampleRateIn, float _sampleRateOut, int8_t _quality) {
	std::vector<float> input;
	input.resize(1024, 0);
	std::vector<float> output;
	output.resize(input.size()*10, 0);
	double sampleRate = _sampleRateIn;
	{
		double phase = 0;
		double baseCycle = 2.0*M_PI/sampleRate * 480.0;
		for (int32_t iii=0; iii<input.size(); iii++) {
			input[iii] = cos(phase) * 5.0;
			phase += baseCycle;
			if (phase >= 2*M_PI) {
				phase -= 2*M_PI;
			}
		}
	}
	TEST_INFO("Start Resampler performance ... " << _sampleRateIn << " -> " << _sampleRateOut << " float");
	Performance perfo;
	audio::algo::speex::Resampler algo;
	algo.init(1, _sampleRateIn, _sampleRateOut, _quality, audio::format_float);
	for (int32_t iii=0; iii<1024; ++iii) {
		perfo.tic();
		size_t sizeOut = output.size();
		algo.process(&output[0], sizeOut, &input[0], input.size());
		perfo.toc();
		usleep(1000);
	}
	TEST_INFO("    blockSize=" << input.size() << " sample");
	TEST_INFO("    min < avg < max =" << perfo.getMinProcessing().count() << "ns < "
	                                  << perfo.getTotalTimeProcessing().count()/perfo.getTotalIteration() << "ns < "
	                                  << perfo.getMaxProcessing().count() << "ns ");
	float avg = (float(((perfo.getTotalTimeProcessing().count()/perfo.getTotalIteration())*sampleRate)/double(input.size()))/1000000000.0)*100.0;
	TEST_INFO("    min < avg < max= " << (float((perfo.getMinProcessing().count()*sampleRate)/double(input.size()))/1000000000.0)*100.0 << "% < "
	                                  << avg << "% < "
	                                  << (float((perfo.getMaxProcessing().count()*sampleRate)/double(input.size()))/1000000000.0)*100.0 << "%");
	TEST_PRINT("float : " << _sampleRateIn << " -> " << _sampleRateOut << " quality=" << int32_t(_quality) << " : " << avg << "%");
	return avg;
}

float performanceResamplerStepI16(float _sampleRateIn, float _sampleRateOut, int8_t _quality) {
	std::vector<int16_t> input;
	input.resize(1024, 0);
	std::vector<int16_t> output;
	output.resize(input.size()*10, 0);
	double sampleRate = _sampleRateIn;
	{
		double phase = 0;
		double baseCycle = 2.0*M_PI/sampleRate * 480.0;
		for (int32_t iii=0; iii<input.size(); iii++) {
			input[iii] = cos(phase) * 30000.0;
			phase += baseCycle;
			if (phase >= 2*M_PI) {
				phase -= 2*M_PI;
			}
		}
	}
	TEST_INFO("Start Resampler performance ... " << _sampleRateIn << " -> " << _sampleRateOut << " int16_t");
	Performance perfo;
	audio::algo::speex::Resampler algo;
	algo.init(1, _sampleRateIn, _sampleRateOut, _quality, audio::format_int16);
	for (int32_t iii=0; iii<1024; ++iii) {
		perfo.tic();
		size_t sizeOut = output.size();
		algo.process(&output[0], sizeOut, &input[0], input.size());
		perfo.toc();
		usleep(1000);
	}
	TEST_INFO("    blockSize=" << input.size() << " sample");
	TEST_INFO("    min < avg < max =" << perfo.getMinProcessing().count() << "ns < "
	                                  << perfo.getTotalTimeProcessing().count()/perfo.getTotalIteration() << "ns < "
	                                  << perfo.getMaxProcessing().count() << "ns ");
	float avg = (float(((perfo.getTotalTimeProcessing().count()/perfo.getTotalIteration())*sampleRate)/double(input.size()))/1000000000.0)*100.0;
	TEST_INFO("    min < avg < max= " << (float((perfo.getMinProcessing().count()*sampleRate)/double(input.size()))/1000000000.0)*100.0 << "% < "
	                                  << avg << "% < "
	                                  << (float((perfo.getMaxProcessing().count()*sampleRate)/double(input.size()))/1000000000.0)*100.0 << "%");
	TEST_PRINT("int16_t : " << _sampleRateIn << " -> " << _sampleRateOut << " quality=" << int32_t(_quality) << " : " << avg << "%");
	return avg;
}

void performanceResampler() {
	for (int8_t iii=1; iii<=10; ++iii) {
		float modeFloat = performanceResamplerStepFloat(8000, 48000, iii);
		float modeI16 = performanceResamplerStepI16(8000, 48000, iii);
		modeFloat = performanceResamplerStepFloat(16000, 48000, iii);
		modeI16 = performanceResamplerStepI16(16000, 48000, iii);
		modeFloat = performanceResamplerStepFloat(32000, 48000, iii);
		modeI16 = performanceResamplerStepI16(32000, 48000, iii);
		modeFloat = performanceResamplerStepFloat(44100, 48000, iii);
		modeI16 = performanceResamplerStepI16(44100, 48000, iii);
		modeFloat = performanceResamplerStepFloat(48001, 48000, iii);
		modeI16 = performanceResamplerStepI16(48001, 48000, iii);
		modeFloat = performanceResamplerStepFloat(96000, 48000, iii);
		modeI16 = performanceResamplerStepI16(96000, 48000, iii);
		modeFloat = performanceResamplerStepFloat(48000, 96000, iii);
		modeI16 = performanceResamplerStepI16(48000, 96000, iii);
		modeFloat = performanceResamplerStepFloat(48000, 48001, iii);
		modeI16 = performanceResamplerStepI16(48000, 48001, iii);
		modeFloat = performanceResamplerStepFloat(48000, 44100, iii);
		modeI16 = performanceResamplerStepI16(48000, 44100, iii);
		modeFloat = performanceResamplerStepFloat(48000, 32000, iii);
		modeI16 = performanceResamplerStepI16(48000, 32000, iii);
		modeFloat = performanceResamplerStepFloat(48000, 16000, iii);
		modeI16 = performanceResamplerStepI16(48000, 16000, iii);
		modeFloat = performanceResamplerStepFloat(48000, 8000, iii);
		modeI16 = performanceResamplerStepI16(48000, 8000, iii);
	}
	
	
}

int main(int _argc, const char** _argv) {
	// the only one init for etk:
	etk::init(_argc, _argv);
	std::string inputName = "";
	std::string outputName = "output.raw";
	bool performance = false;
	bool perf = false;
	int64_t sampleRateIn = 48000;
	int64_t sampleRateOut = 48000;
	int32_t nbChan = 1;
	int32_t quality = 4;
	std::string test = "";
	for (int32_t iii=0; iii<_argc ; ++iii) {
		std::string data = _argv[iii];
		if (etk::start_with(data,"--in=")) {
			inputName = &data[5];
		} else if (etk::start_with(data,"--out=")) {
			outputName = &data[6];
		} else if (data == "--performance") {
			performance = true;
		} else if (data == "--perf") {
			perf = true;
		} else if (etk::start_with(data,"--test=")) {
			data = &data[7];
			sampleRateIn = etk::string_to_int32_t(data);
		} else if (etk::start_with(data,"--sample-rate-in=")) {
			data = &data[17];
			sampleRateIn = etk::string_to_int32_t(data);
		} else if (etk::start_with(data,"--sample-rate-out=")) {
			data = &data[18];
			sampleRateOut = etk::string_to_int32_t(data);
		} else if (etk::start_with(data,"--nb=")) {
			data = &data[5];
			nbChan = etk::string_to_int32_t(data);
		} else if (etk::start_with(data,"--quality=")) {
			data = &data[10];
			quality = etk::string_to_int32_t(data);
		} else if (    data == "-h"
		            || data == "--help") {
			TEST_PRINT("Help : ");
			TEST_PRINT("    ./xxx --fb=file.raw --mic=file.raw");
			TEST_PRINT("        --in=YYY.raw            input file");
			TEST_PRINT("        --out=zzz.raw           output file");
			TEST_PRINT("        --performance           Generate signal to force algo to maximum process time");
			TEST_PRINT("        --perf                  Enable performence test (little slower but real performence test)");
			TEST_PRINT("        --test=XXXX             some test availlable ...");
			TEST_PRINT("            RESAMPLING          Test resampling data 16 bit mode");
			TEST_PRINT("                --sample-rate-in=XXXX   Input signal sample rate (default 48000)");
			TEST_PRINT("                --sample-rate-out=XXXX  Output signal sample rate (default 48000)");
			TEST_PRINT("                --quality=XX            Resampling quality [0..10] (default 4)");
			TEST_PRINT("                --nb=XX                 Number of channel in the file (default 1)");
			
			exit(0);
		}
	}
	// PERFORMANCE test only ....
	if (performance == true) {
		performanceResampler();
		return 0;
	}
	if (test == "RESAMPLING") {
		TEST_INFO("Start resampling test ... ");
		if (inputName == "") {
			TEST_ERROR("Can not Process missing parameters...");
			exit(-1);
		}
		TEST_INFO("Read input:");
		std::vector<int16_t> inputData = etk::FSNodeReadAllDataType<int16_t>(inputName);
		TEST_INFO("    " << inputData.size() << " samples");
		// resize output :
		std::vector<int16_t> output;
		output.resize(inputData.size()*sampleRateOut/sampleRateIn+5000, 0);
		// process in chunk of 256 samples
		int32_t blockSize = 256*nbChan;
		
		Performance perfo;
		audio::algo::speex::Resampler algo;
		algo.init(nbChan, sampleRateIn, sampleRateOut, quality, audio::format_int16);
		int32_t lastPourcent = -1;
		size_t outputPosition = 0;
		for (int32_t iii=0; iii<inputData.size()/blockSize; ++iii) {
			if (lastPourcent != 100*iii / (inputData.size()/blockSize)) {
				lastPourcent = 100*iii / (inputData.size()/blockSize);
				TEST_INFO("Process : " << iii*blockSize << "/" << int32_t(inputData.size()/blockSize)*blockSize << " " << lastPourcent << "/100");
			} else {
				TEST_VERBOSE("Process : " << iii*blockSize << "/" << int32_t(inputData.size()/blockSize)*blockSize);
			}
			size_t availlableSize = (output.size() - outputPosition) / nbChan;
			perfo.tic();
			algo.process(&output[outputPosition], availlableSize, &inputData[iii*blockSize], blockSize);
			if (perf == true) {
				perfo.toc();
				usleep(1000);
			}
			outputPosition += availlableSize*nbChan;
		}
		if (perf == true) {
			TEST_INFO("Performance Result: ");
			TEST_INFO("    blockSize=" << blockSize << " sample");
			TEST_INFO("    min=" << perfo.getMinProcessing().count() << " ns");
			TEST_INFO("    max=" << perfo.getMaxProcessing().count() << " ns");
			TEST_INFO("    avg=" << perfo.getTotalTimeProcessing().count()/perfo.getTotalIteration() << " ns");
			
			TEST_INFO("    min=" << (float((perfo.getMinProcessing().count()*sampleRateIn)/blockSize)/1000000000.0)*100.0 << " %");
			TEST_INFO("    max=" << (float((perfo.getMaxProcessing().count()*sampleRateIn)/blockSize)/1000000000.0)*100.0 << " %");
			TEST_INFO("    avg=" << (float(((perfo.getTotalTimeProcessing().count()/perfo.getTotalIteration())*sampleRateIn)/blockSize)/1000000000.0)*100.0 << " %");
		}
		etk::FSNodeWriteAllDataType<int16_t>(outputName, output);
	}
	
}

