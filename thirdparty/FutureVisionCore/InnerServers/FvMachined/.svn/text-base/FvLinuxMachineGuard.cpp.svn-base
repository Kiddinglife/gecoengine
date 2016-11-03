#include "FvLinuxMachineGuard.h"
#include "FvMachined.h"
#include <glob.h>
#include <libgen.h>

void getProcessTimes( FILE *f, int *utimePtr, int *stimePtr, int *vsizePtr,
	int *cpuPtr );

const char * machinedConfFile = "/etc/machined.conf";
static bool hasExtendedStats = false;

void sigChildHandler( int )
{
	waitpid( -1, NULL, WNOHANG );
}

void initProcessState( bool isDaemon )
{
	if (isDaemon)
		daemon( 1, 0 );
	else
		printf( "Not running as daemon\n" );

	hasExtendedStats = false;

	FILE *fp;
	const char *kernel_version_file = "/proc/sys/kernel/osrelease";
	if ((fp = fopen( kernel_version_file, "r" )) == NULL)
	{
		syslog( LOG_ERR, "Couldn't read %s: %s", kernel_version_file,
				strerror( errno ) );
		hasExtendedStats = false;
	}

	char line[ 512 ];
	uint major, minor;

	fgets( line, sizeof( line ), fp );
	if (sscanf( line, "%d.%d", &major, &minor) != 2)
	{
		syslog( LOG_ERR, "Invalid line in %s: '%s'",
				kernel_version_file, line );
	}
	else
	{
		if ((major > 2) || (minor > 4))
		{
			hasExtendedStats = true;
		}

		if (hasExtendedStats)
		{
			syslog( LOG_INFO, "Kernel version %d.%d detected: Using extended stats "
						"from /proc/stats\n", major, minor );
		}
	}

	fclose( fp );

	signal( SIGCHLD, sigChildHandler );
}

void ProcessInfo::init( const ProcessMessage &pm ) {}

void cleanupProcessState()
{
}

bool updateSystemInfoP( struct SystemInfo &si )
{
	char line[ 512 ];
	FILE *af;

	uint32	cpu;
	uint64	jiffyUser, jiffyNice, jiffySyst, jiffyIdle;
	uint64	jiffyIOwait, jiffyIRQ, jiffySoftIRQ;

	if ((af = fopen( "/proc/stat", "r" )) == NULL)
	{
		syslog( LOG_ERR, "Couldn't read /proc/stat: %s", strerror( errno ) );
		return false;
	}

	fgets( line, sizeof( line ), af );

	uint64 totalWork, totalIdle;
	uint64 systemIOwait = 0;
	uint64 systemTotalWork = 0;
	for (uint i=0; i < si.nCpus; i++)
	{
		fgets( line, sizeof( line ), af );

		if (hasExtendedStats)
		{
			if (sscanf( line, "cpu%u "PRIu64" "PRIu64" "PRIu64" "PRIu64" "PRIu64" "PRIu64" "PRIu64"",
					&cpu, &jiffyUser, &jiffyNice, &jiffySyst, &jiffyIdle,
					&jiffyIOwait, &jiffyIRQ, &jiffySoftIRQ) != 8)
			{
				syslog( LOG_ERR, "Invalid line in /proc/stat: '%s'", line );
				break;
			}
		}
		else
		{
			if (sscanf( line, "cpu%u "PRIu64" "PRIu64" "PRIu64" "PRIu64,
				&cpu, &jiffyUser, &jiffyNice, &jiffySyst, &jiffyIdle) != 5)
			{
				syslog( LOG_ERR, "Invalid line in /proc/stat: '%s'", line );
				break;
			}
		}

		if (cpu != i)
		{
			syslog( LOG_CRIT, "Line %d of /proc/stat was cpu%u, not cpu%d",
				i, cpu, i );
		}

		totalWork = jiffyUser + jiffyNice + jiffySyst;
		totalIdle = jiffyIdle;

		if (hasExtendedStats)
		{
			totalIdle += jiffyIOwait + jiffyIRQ + jiffySoftIRQ;
		}

		si.cpu[i].val.update( totalWork );
		si.cpu[i].max.update( totalWork + totalIdle );

		systemIOwait += jiffyIOwait;
		systemTotalWork += totalWork + totalIdle;
	}
	si.iowait.val.update( systemIOwait );
	si.iowait.max.update( systemTotalWork );

	fclose(af);

	if ((af = fopen( "/proc/meminfo", "r" )) == NULL)
	{
		syslog( LOG_ERR, "Couldn't read /proc/meminfo: %s", strerror( errno ) );
		return false;
	}

	unsigned long totalmem, freemem, bufmem, cachemem, slabmem;

	for (int i=0; i < 5 && fgets( line, sizeof( line ), af) != NULL;)
	{
		if (sscanf( line, "MemTotal: %lu", &totalmem ) == 1)
		{
			i++; continue;
		}
		if (sscanf( line, "MemFree: %lu", &freemem ) == 1)
		{
			i++; continue;
		}
		if (sscanf( line, "Buffers: %lu", &bufmem ) == 1)
		{
			i++; continue;
		}
		if (sscanf( line, "Cached: %lu", &cachemem ) == 1)
		{
			i++; continue;
		}
		if (sscanf( line, "Slab: %lu", &slabmem ) == 1)
		{
			i++; continue;
		}
	}

	fclose( af );

	si.mem.max.update( uint64( totalmem ) << 10 );
	si.mem.val.update(
		uint64( totalmem - freemem - bufmem - cachemem - slabmem ) << 10 );

	if ((af = fopen( "/proc/net/snmp", "r" )) == NULL)
	{
		syslog( LOG_ERR, "Couldn't read /proc/net/snmp: %s", strerror( errno ) );
		return false;
	}

	fgets( line, sizeof( line ), af );
	if (fscanf( af,"%*s %*d %*d %*d %*d %*d %*d %*d "PRIu64" "PRIu64" "PRIu64" "PRIu64,
		&si.packDropIn.next(), &si.packTotIn.next(),
		&si.packTotOut.next(), &si.packDropOut.next() ) != 4)
	{
		syslog( LOG_ERR, "Failed to read packet loss information from "
				"/proc/net/snmp" );
	}
	fclose( af );

	if ((af = fopen( "/proc/net/dev", "r" )) == NULL)
	{
		syslog( LOG_ERR, "Couldn't open /proc/net/dev: %s", strerror( errno ) );
		return false;
	}

	fgets( line, sizeof( line ), af ); fgets( line, sizeof( line ), af );
	for (unsigned int i=0; fgets( line, sizeof( line ), af ) != NULL; )
	{
		if (i >= si.ifInfo.size())
			si.ifInfo.push_back( InterfaceInfo() );
		struct InterfaceInfo &ifInfo = si.ifInfo[i];
		char ifname[32];

		sscanf( line, " %[^:]", ifname );
		if (strstr( ifname, "lo" ) == NULL)
		{
			ifInfo.name = ifname;
			sscanf( line,
				" %*[^:]:"PRIu64" "PRIu64" %*d %*d %*d %*d %*d %*d "
				PRIu64" "PRIu64,
				&ifInfo.bitsTotIn.next(),
				&ifInfo.packTotIn.next(),
				&ifInfo.bitsTotOut.next(),
				&ifInfo.packTotOut.next() );

			ifInfo.bitsTotIn.cur() *= 8;
			ifInfo.bitsTotOut.cur() *= 8;
			i++;
		}
	}

	fclose( af );
	return true;
}

bool updateProcessStats( ProcessInfo &pi )
{
	char pinfoFilename[64];
	FvSNPrintf( pinfoFilename, sizeof( pinfoFilename ),
		"/proc/%d/stat", (int)pi.m.pid_ );

	FILE *pinfo;
	if ((pinfo = fopen( pinfoFilename, "r" )) == NULL)
	{
		if (errno != ENOENT)
		{
			syslog( LOG_ERR, "Couldn't open %s: %s",
				pinfoFilename, strerror( errno ) );
		}

		return false;
	}

	int utime, stime, vsize, cpu;
	getProcessTimes( pinfo, &utime, &stime, &vsize, &cpu );
	pi.cpu.update( utime + stime );
	pi.mem.update( vsize );
	pi.affinity = cpu;

	fclose( pinfo );
	return true;
}

void getProcessorSpeeds( std::vector<float>& speeds )
{
	FILE	*cpuf;
	float	mhz;
	char	aline[BUFSIZ],	*look;

	speeds.clear();

	if ((cpuf = fopen( "/proc/cpuinfo", "r" )) == NULL)
	{
		syslog( LOG_ERR, "Couldn't read /proc/cpuinfo: %s", strerror( errno ) );
		return;
	}

	while (fgets(aline,BUFSIZ,cpuf))
	{
		if ((look=strchr(aline,'\n'))) *look=0;
		if ((look=strchr(aline,'\r'))) *look=0;

		if (!aline[0])
		{
			//ncpus++;
		}
		else
		{
			look=strchr(aline,':');	*look++=0;

			if (!strcmp(aline,"cpu MHz\t\t"))
			{
				sscanf(look,"%f",&mhz);
				speeds.push_back( mhz );
			}
		}
	}
	fclose(cpuf);
}

void getProcessTimes( FILE *f, int *utimePtr, int *stimePtr, int *vsizePtr,
	int *cpuPtr )
{
	int		pid;
	char	name[24];
	char	state;
	int		ppid,	pgrp,	session,	tty,	tpgid;
	unsigned int	flags,	minflt,	cminflt,	majflt,	cmajflt;
	int		utime,	stime,	cutime,	cstime;
	int		counter,	priority;
	unsigned int	timeout,	itrealvalue;
	int		starttime;
	unsigned int	vsize,		rssize;

	unsigned int rlim, startcode, endcode, startstack, kstkesp, kstkeip,
		signal, blocked, sigignore, sigcatch, wchan, nswap, cnswap;

	int exit_signal, processor;

	fseek(f,SEEK_SET,0);
	fscanf(f,"%d %s %c %d %d %d %d %d %u %u %u %u %d %d %d %d"
		" %d %d %u %u %d %u %u %u"
		" %u %u %u %u %u %u %u %u %u %u %u %u %u"
		" %d %d",

		&pid,
		name,
		&state,
		&ppid,&pgrp,&session,&tty,&tpgid,
		&flags,&minflt,&cminflt,&majflt,&cmajflt,
		&utime,&stime,&cutime,&cstime,
		&counter,&priority,
		&timeout,&itrealvalue,
		&starttime,
		&vsize,&rssize,
		&rlim, &startcode, &endcode, &startstack, &kstkesp, &kstkeip,
		&signal, &blocked, &sigignore, &sigcatch, &wchan, &nswap, &cnswap,
		&exit_signal, &processor
		);

	if( utimePtr ) *utimePtr = utime;
	if( stimePtr ) *stimePtr = stime;
	if( vsizePtr ) *vsizePtr = vsize;
	if( cpuPtr ) *cpuPtr = processor;
}

char* putEnvAlloc(const char* name, const char* value)
{
	int len = strlen(name) + strlen(value) + 2;
	char* str = new char[len];
	strcpy(str, name);
	strcat(str, "=");
	strcat(str, value);
	putenv(str);
	return str;
}


uint16 startProcess( const char * mfRoot,
	const char * pcResPath,
	const char * config,
	const char * type,
	uint16 uid,
	uint16 gid,
	const char * home,
	int argc,
	const char ** argv,
	FvMachined &machined )
{
	uint16 childpid;

	if ((childpid = fork()) == 0)
	{
		if (setgid( gid ) == -1)
		{
			syslog( LOG_ERR,
				"Failed to setgid() to %d for user %d, group will be root\n",
				gid, uid );
		}

		if (setuid( uid ) == -1)
		{
			syslog( LOG_ERR, "Failed to setuid to %d, aborting exec for %s\n",
				uid, type );
			exit( 1 );
		}

		char path[ 512 ];
		strcpy( path, mfRoot );
		strcat( path, "/bigworld/bin/" );
		strcat( path, config );
		strcat( path, "/" );

		chdir( path );

		strncat( path, type, 32 );
		argv[0] = path;

		argv[ argc++ ] = "--res";
		argv[ argc++ ] = strdup( pcResPath );

		machined.closeEndpoints();

		putEnvAlloc( "FV_TIMING_METHOD", machined.timingMethod() );

		putEnvAlloc( "HOME", home );

		syslog( LOG_INFO, "UID %d execing %s", uid, path );

		argv[ argc ] = NULL;

		int result = execv( path, const_cast< char * const * >( argv ) );

		if (result == -1)
		{
			syslog(LOG_ERR, "Failed to exec %s\n", path);
		}

		exit(1);
		return 0;
	}
	else
		return childpid;
}

bool checkProcess( uint16 pid )
{
	char buf[32];
	struct stat statinfo;
	int ret;

	snprintf( buf, sizeof( buf ) - 1, "/proc/%d", pid );

	if ((ret = stat( buf, &statinfo )) == -1 && errno != ENOENT)
	{
		syslog( LOG_ERR, "checkProcess(): stat(%s) failed (%s)",
			buf, strerror( errno ) );
	}

	return ret == 0;
}

void checkCoreDumps( const char *mfroot, UserMessage &um )
{
	char path[ 512 ], cwd[ 512 ], assert[ 512 ], subdir[ 512 ];
	struct stat statinfo;
	glob_t matches;
	unsigned i;
	FILE *fp;
	char c;

	FvSNPrintf( path, sizeof( path ), "%s/futurevision/bin", mfroot );
	getcwd( cwd, sizeof( cwd ) );
	chdir( path );
	um.coredumps_.clear();

	if (glob( "*/core.*", 0, NULL, &matches ) == 0)
	{
		size_t MAX_CORES = 10;

		for (i=0; i < std::min( MAX_CORES, matches.gl_pathc ); i++)
		{
			const char *core = matches.gl_pathv[i];
			UserMessage::CoreDump cd;
			cd.filename_ = core;

			strcpy( subdir, core );

			FvSNPrintf( assert, sizeof( assert ), "%s/assert.%s.log",
				dirname( subdir ), strstr( core, "/" ) + 6 );

			if ((fp = fopen( assert, "r" )) != NULL)
			{
				while ((c = fgetc( fp )) != EOF)
				{
					cd.assert_.push_back( c );
				}

				fclose( fp );
			}

			if (stat( core, &statinfo ) == 0)
			{
				cd.time_ = statinfo.st_ctime;
			}
			else
			{
				syslog( LOG_ERR, "Couldn't stat() %s: %s",
					core, strerror( errno ) );
			}

			um.coredumps_.push_back( cd );
		}

		syslog( LOG_ERR, "Found %u coredumps in %s, %zu reported.",
				i, path, matches.gl_pathc );
	}

	chdir( cwd );
}
