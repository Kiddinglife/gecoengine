//{future header message}
#ifndef __FvWriteDB_H__
#define __FvWriteDB_H__

enum FvWriteDBFlags
{
	WRITE_LOG_OFF			= 1,
	WRITE_EXPLICIT			= 2,
	WRITE_BASE_DATA			= 4,
	WRITE_CELL_DATA			= 8,
	WRITE_ALL_DATA			= WRITE_BASE_DATA|WRITE_CELL_DATA,
	WRITE_DELETE_FROM_DB	= 16
};

#endif // __FvWriteDB_H__
