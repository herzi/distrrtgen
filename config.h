#ifndef __CONFIG_H_
#define __CONFIG_H_

#define DATA_CHUNK_SIZE 16*100
#define PROTOCOL_VERSION 0x03
#ifndef WIN32
	#define Sleep(num) sleep(num / 1000)
#endif
#endif
