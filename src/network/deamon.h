#pragma once

#define MAX_BIT_RATE (1<<27)
#define BIT_INCREMENT (MAX_BIT_RATE / 0xFF)
#define MAX_PACKET_RATE 256000
#define PACK_INCREMENT (MAX_PACKET_RATE / 0xFF)

#if !defined( PLAYSTATION3 )
#include <signal.h>
#endif

#if !defined( _WIN32 ) && !defined( PLAYSTATION3 )
# include <sys/types.h>
# include <pwd.h>
#else
#define SIGINT 2
#define SIGQUIT 3
#define SIGUSR1 10
#endif

extern const char *g_pcMachinedConfFile;

class deamon
{
public:
	deamon();
	virtual ~deamon();
};

