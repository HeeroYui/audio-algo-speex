/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#ifndef __AUDIO_ALGO_SPEEX_DEBUG_H__
#define __AUDIO_ALGO_SPEEX_DEBUG_H__

#include <etk/log.h>

namespace audio {
	namespace algo {
		namespace speex {
			int32_t getLogId();
		}
	}
}

#define AA_SPEEX_BASE(info,data) TK_LOG_BASE(audio::algo::speex::getLogId(),info,data)

#define AA_SPEEX_PRINT(data)         AA_SPEEX_BASE(-1, data)
#define AA_SPEEX_CRITICAL(data)      AA_SPEEX_BASE(1, data)
#define AA_SPEEX_ERROR(data)         AA_SPEEX_BASE(2, data)
#define AA_SPEEX_WARNING(data)       AA_SPEEX_BASE(3, data)
#ifdef DEBUG
	#define AA_SPEEX_INFO(data)          AA_SPEEX_BASE(4, data)
	#define AA_SPEEX_DEBUG(data)         AA_SPEEX_BASE(5, data)
	#define AA_SPEEX_VERBOSE(data)       AA_SPEEX_BASE(6, data)
	#define AA_SPEEX_TODO(data)          AA_SPEEX_BASE(4, "TODO : " << data)
#else
	#define AA_SPEEX_INFO(data)          do { } while(false)
	#define AA_SPEEX_DEBUG(data)         do { } while(false)
	#define AA_SPEEX_VERBOSE(data)       do { } while(false)
	#define AA_SPEEX_TODO(data)          do { } while(false)
#endif

#define AA_SPEEX_ASSERT(cond,data) \
	do { \
		if (!(cond)) { \
			AA_SPEEX_CRITICAL(data); \
			assert(!#cond); \
		} \
	} while (0)

#endif
