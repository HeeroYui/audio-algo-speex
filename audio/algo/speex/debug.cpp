/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include "debug.h"


int32_t audio::algo::speex::getLogId() {
	static int32_t g_val = elog::registerInstance("audio-algo-speex");
	return g_val;
}

