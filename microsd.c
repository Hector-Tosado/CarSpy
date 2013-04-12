#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "utils/cmdline.h"
#include "utils/uartstdio.h"
#include "fatfs/src/ff.h"
#include "fatfs/src/diskio.h"


//*****************************************************************************
//
// Defines the size of the buffers that hold the path, or temporary
// data from the SD card.  There are two buffers allocated of this size.
// The buffer size must be large enough to hold the longest expected
// full path name, including the file name, and a trailing null character.
//
//*****************************************************************************
#define PATH_BUF_SIZE   80

//*****************************************************************************
//
// Defines the size of the buffer that holds the command line.
//
//*****************************************************************************
#define CMD_BUF_SIZE    64

//*****************************************************************************
//
// This buffer holds the full path to the current working directory.
// Initially it is root ("/").
//
//*****************************************************************************
static char g_cCwdBuf[PATH_BUF_SIZE] = "/";


//Data to be Stored using the write commmand function
char photo[] ="0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,"
  	"0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,"
		"0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,"
		"0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,"
		"0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,"
		"0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,"
		"0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,"
		"0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,"
		"0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,"
		"0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,"
		"0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,"
		"0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,"
		"0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,"
		"0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,"
		"0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,"
		"0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16";

char GPSCoordinate[]= "$GPRMC,064951.000,A,2307.1256,N,12016.4438,E,0.03,165.48,260406,3.05,W,A*2C";


//*****************************************************************************
//
// A temporary data buffer used when manipulating file paths, or reading data
// from the SD card.
//
//*****************************************************************************
static char g_cTmpBuf[PATH_BUF_SIZE];

//*****************************************************************************
//
// The buffer that holds the command line.
//
//*****************************************************************************
static char g_cCmdBuf[CMD_BUF_SIZE];
char readOrigin[CMD_BUF_SIZE]="read origin.txt";
char readFinal[CMD_BUF_SIZE]="read finalS.txt";
//*****************************************************************************
//
// The following are data structures used by FatFs.
//
//*****************************************************************************
static FATFS g_sFatFs;
static DIR g_sDirObject;
static FILINFO g_sFileInfo;
static FIL g_sFileObject;

//*****************************************************************************
//
// A structure that holds a mapping between an FRESULT numerical code,
// and a string represenation.  FRESULT codes are returned from the FatFs
// FAT file system driver.
//
//*****************************************************************************
typedef struct
{
	FRESULT fresult;
	char *pcResultStr;
}
tFresultString;

//*****************************************************************************
//
// A macro to make it easy to add result codes to the table.
//
//*****************************************************************************
#define FRESULT_ENTRY(f)        { (f), (#f) }

//*****************************************************************************
//
// A table that holds a mapping between the numerical FRESULT code and
// it's name as a string.  This is used for looking up error codes for
// printing to the console.
//
//*****************************************************************************
tFresultString g_sFresultStrings[] =
{
		FRESULT_ENTRY(FR_OK),
		FRESULT_ENTRY(FR_NOT_READY),
		FRESULT_ENTRY(FR_NO_FILE),
		FRESULT_ENTRY(FR_NO_PATH),
		FRESULT_ENTRY(FR_INVALID_NAME),
		FRESULT_ENTRY(FR_INVALID_DRIVE),
		FRESULT_ENTRY(FR_DENIED),
		FRESULT_ENTRY(FR_EXIST),
		FRESULT_ENTRY(FR_RW_ERROR),
		FRESULT_ENTRY(FR_WRITE_PROTECTED),
		FRESULT_ENTRY(FR_NOT_ENABLED),
		FRESULT_ENTRY(FR_NO_FILESYSTEM),
		FRESULT_ENTRY(FR_INVALID_OBJECT),
		FRESULT_ENTRY(FR_MKFS_ABORTED)
};

//*****************************************************************************
//
// A macro that holds the number of result codes.
//
//*****************************************************************************
#define NUM_FRESULT_CODES (sizeof(g_sFresultStrings) / sizeof(tFresultString))

//*****************************************************************************
//
// Variabls to be used in the Simulation Command
//
//*****************************************************************************

int sessionFlag=0;
int sessionItem=0;
int newDirectoryFlag=0;


//*****************************************************************************
//
// This function returns a string representation of an error code
// that was returned from a function call to FatFs.  It can be used
// for printing human readable error messages.
//
//*****************************************************************************
const char *
StringFromFresult(FRESULT fresult)
{
	unsigned int uIdx;

	//
	// Enter a loop to search the error code table for a matching
	// error code.
	//
	for(uIdx = 0; uIdx < NUM_FRESULT_CODES; uIdx++)
	{
		//
		// If a match is found, then return the string name of the
		// error code.
		//
		if(g_sFresultStrings[uIdx].fresult == fresult)
		{
			return(g_sFresultStrings[uIdx].pcResultStr);
		}
	}

	//
	// At this point no matching code was found, so return a
	// string indicating unknown error.
	//
	return("UNKNOWN ERROR CODE");
}

//*****************************************************************************
//
// This is the handler for this SysTick interrupt.  FatFs requires a
// timer tick every 10 ms for internal timing purposes.
//
//*****************************************************************************
void
SysTickHandler(void)
{
	//
	// Call the FatFs tick timer.
	//
	disk_timerproc();
}

//*****************************************************************************
//
// This function implements the "ls" command.  It opens the current
// directory and enumerates through the contents, and prints a line for
// each item it finds.  It shows details such as file attributes, time and
// date, and the file size, along with the name.  It shows a summary of
// file sizes at the end along with free space.
//
//*****************************************************************************
int
Cmd_ls(int argc, char *argv[])
{
	unsigned long ulTotalSize;
	unsigned long ulFileCount;
	unsigned long ulDirCount;
	FRESULT fresult;
	FATFS *pFatFs;

	//
	// Open the current directory for access.
	//
	fresult = f_opendir(&g_sDirObject, g_cCwdBuf);

	//
	// Check for error and return if there is a problem.
	//
	if(fresult != FR_OK)
	{
		return(fresult);
	}

	ulTotalSize = 0;
	ulFileCount = 0;
	ulDirCount = 0;

	//
	// Give an extra blank line before the listing.
	//
	UARTprintf("\n");

	//
	// Enter loop to enumerate through all directory entries.
	//
	for(;;)
	{
		//
		// Read an entry from the directory.
		//
		fresult = f_readdir(&g_sDirObject, &g_sFileInfo);

		//
		// Check for error and return if there is a problem.
		//
		if(fresult != FR_OK)
		{
			return(fresult);
		}

		//
		// If the file name is blank, then this is the end of the
		// listing.
		//
		if(!g_sFileInfo.fname[0])
		{
			break;
		}

		//
		// If the attribue is directory, then increment the directory count.
		//
		if(g_sFileInfo.fattrib & AM_DIR)
		{
			ulDirCount++;
		}

		//
		// Otherwise, it is a file.  Increment the file count, and
		// add in the file size to the total.
		//
		else
		{
			ulFileCount++;
			ulTotalSize += g_sFileInfo.fsize;
		}

		//
		// Print the entry information on a single line with formatting
		// to show the attributes, date, time, size, and name.
		//
		UARTprintf("%c%c%c%c%c %u/%02u/%02u %02u:%02u %9u  %s\n",
				(g_sFileInfo.fattrib & AM_DIR) ? 'D' : '-',
						(g_sFileInfo.fattrib & AM_RDO) ? 'R' : '-',
								(g_sFileInfo.fattrib & AM_HID) ? 'H' : '-',
										(g_sFileInfo.fattrib & AM_SYS) ? 'S' : '-',
												(g_sFileInfo.fattrib & AM_ARC) ? 'A' : '-',
														(g_sFileInfo.fdate >> 9) + 1980,
														(g_sFileInfo.fdate >> 5) & 15,
														g_sFileInfo.fdate & 31,
														(g_sFileInfo.ftime >> 11),
														(g_sFileInfo.ftime >> 5) & 63,
														g_sFileInfo.fsize,
														g_sFileInfo.fname);
	}   // endfor

	//
	// Print summary lines showing the file, dir, and size totals.
	//
	UARTprintf("\n%4u File(s),%10u bytes total\n%4u Dir(s)",
			ulFileCount, ulTotalSize, ulDirCount);

	//
	// Get the free space.
	//
	fresult = f_getfree("/", &ulTotalSize, &pFatFs);

	//
	// Check for error and return if there is a problem.
	//
	if(fresult != FR_OK)
	{
		return(fresult);
	}

	//
	// Display the amount of free space that was calculated.
	//
	//UARTprintf(", %10uK bytes free\n", ulTotalSize * pFatFs->sects_clust / 2);

	//
	// Made it to here, return with no errors.
	//
	return(0);
}

//*****************************************************************************
//
// This function implements the "cd" command.  It takes an argument
// that specifes the directory to make the current working directory.
// Path separators must use a forward slash "/".  The argument to cd
// can be one of the following:
// * root ("/")
// * a fully specified path ("/my/path/to/mydir")
// * a single directory name that is in the current directory ("mydir")
// * parent directory ("..")
//
// It does not understand relative paths, so dont try something like this:
// ("../my/new/path")
//
// Once the new directory is specified, it attempts to open the directory
// to make sure it exists.  If the new path is opened successfully, then
// the current working directory (cwd) is changed to the new path.
//
//*****************************************************************************
int
Cmd_cd(int argc, char *argv[])
{
	unsigned int uIdx;
	FRESULT fresult;

	//
	// Copy the current working path into a temporary buffer so
	// it can be manipulated.
	//
	strcpy(g_cTmpBuf, g_cCwdBuf);

	//
	// If the first character is /, then this is a fully specified
	// path, and it should just be used as-is.
	//
	if(argv[1][0] == '/')
	{
		//
		// Make sure the new path is not bigger than the cwd buffer.
		//
		if(strlen(argv[1]) + 1 > sizeof(g_cCwdBuf))
		{
			UARTprintf("Resulting path name is too long\n");
			return(0);
		}

		//
		// If the new path name (in argv[1])  is not too long, then
		// copy it into the temporary buffer so it can be checked.
		//
		else
		{
			strncpy(g_cTmpBuf, argv[1], sizeof(g_cTmpBuf));
		}
	}

	//
	// If the argument is .. then attempt to remove the lowest level
	// on the CWD.
	//
	else if(!strcmp(argv[1], ".."))
	{
		//
		// Get the index to the last character in the current path.
		//
		uIdx = strlen(g_cTmpBuf) - 1;

		//
		// Back up from the end of the path name until a separator (/)
		// is found, or until we bump up to the start of the path.
		//
		while((g_cTmpBuf[uIdx] != '/') && (uIdx > 1))
		{
			//
			// Back up one character.
			//
			uIdx--;
		}

		//
		// Now we are either at the lowest level separator in the
		// current path, or at the beginning of the string (root).
		// So set the new end of string here, effectively removing
		// that last part of the path.
		//
		g_cTmpBuf[uIdx] = 0;
	}

	//
	// Otherwise this is just a normal path name from the current
	// directory, and it needs to be appended to the current path.
	//
	else
	{
		//
		// Test to make sure that when the new additional path is
		// added on to the current path, there is room in the buffer
		// for the full new path.  It needs to include a new separator,
		// and a trailing null character.
		//
		if(strlen(g_cTmpBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_cCwdBuf))
		{
			UARTprintf("Resulting path name is too long\n");
			return(0);
		}

		//
		// The new path is okay, so add the separator and then append
		// the new directory to the path.
		//
		else
		{
			//
			// If not already at the root level, then append a /
			//
			if(strcmp(g_cTmpBuf, "/"))
			{
				strcat(g_cTmpBuf, "/");
			}

			//
			// Append the new directory to the path.
			//
			strcat(g_cTmpBuf, argv[1]);
		}
	}

	//
	// At this point, a candidate new directory path is in chTmpBuf.
	// Try to open it to make sure it is valid.
	//
	fresult = f_opendir(&g_sDirObject, g_cTmpBuf);

	//
	// If it cant be opened, then it is a bad path.  Inform
	// user and return.
	//
	if(fresult != FR_OK)
	{
		UARTprintf("cd: %s\n", g_cTmpBuf);
		return(fresult);
	}

	//
	// Otherwise, it is a valid new path, so copy it into the CWD.
	//
	else
	{
		strncpy(g_cCwdBuf, g_cTmpBuf, sizeof(g_cCwdBuf));
	}

	//
	// Return success.
	//
	return(0);
}

//*****************************************************************************
//
// This function implements the "pwd" command.  It simply prints the
// current working directory.
//
//*****************************************************************************
int
Cmd_pwd(int argc, char *argv[])
{
	//
	// Print the CWD to the console.
	//
	UARTprintf("%s\n", g_cCwdBuf);

	//
	// Return success.
	//
	return(0);
}

//*****************************************************************************
//
// This function implements the "cat" command.  It reads the contents of
// a file and prints it to the console.  This should only be used on
// text files.  If it is used on a binary file, then a bunch of garbage
// is likely to printed on the console.
//
//*****************************************************************************
int
Cmd_read(int argc, char *argv[])
{
	FRESULT fresult;
	unsigned short usBytesRead;

	//
	// First, check to make sure that the current path (CWD), plus
	// the file name, plus a separator and trailing null, will all
	// fit in the temporary buffer that will be used to hold the
	// file name.  The file name must be fully specified, with path,
	// to FatFs.
	//
	if(strlen(g_cCwdBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_cTmpBuf))
	{
		UARTprintf("Resulting path name is too long\n");
		return(0);
	}

	//
	// Copy the current path to the temporary buffer so it can be manipulated.
	//
	strcpy(g_cTmpBuf, g_cCwdBuf);

	//
	// If not already at the root level, then append a separator.
	//
	if(strcmp("/", g_cCwdBuf))
	{
		strcat(g_cTmpBuf, "/");
	}

	//
	// Now finally, append the file name to result in a fully specified file.
	//
	strcat(g_cTmpBuf, argv[1]);


	//
	// Open the file for reading.
	//
	fresult = f_open(&g_sFileObject, g_cTmpBuf, FA_READ);

	//
	// If there was some problem opening the file, then return
	// an error.
	//
	if(fresult != FR_OK)
	{
		return(fresult);
	}

	//
	// Enter a loop to repeatedly read data from the file and display it,
	// until the end of the file is reached.
	//
	do
	{
		//
		// Read a block of data from the file.  Read as much as can fit
		// in the temporary buffer, including a space for the trailing null.
		//
		fresult = f_read(&g_sFileObject, g_cTmpBuf, sizeof(g_cTmpBuf) - 1,
				&usBytesRead);

		//
		// If there was an error reading, then print a newline and
		// return the error to the user.
		//
		if(fresult != FR_OK)
		{
			UARTprintf("\n");
			return(fresult);
		}

		//
		// Null terminate the last block that was read to make it a
		// null terminated string that can be used with printf.
		//
		g_cTmpBuf[usBytesRead] = 0;

		//
		// Print the last chunk of the file that was received.
		//
		UARTprintf("%s", g_cTmpBuf);

		//
		// Continue reading until less than the full number of bytes are
		// read.  That means the end of the buffer was reached.
		//
	}
	while(usBytesRead == sizeof(g_cTmpBuf) - 1);

	//
	// Return success.
	//
	return(0);
}
//WRITE COMMAND
int WriteFile(char *file, char *data)
{

	unsigned short usBytesWrite;

	//
	// Open the file for create new text file.
	//

	if (f_open(&g_sFileObject, file, FA_CREATE_NEW | FA_WRITE)==FR_OK)
	{
		//
		// write file
		//
		if(f_write(&g_sFileObject,data,strlen(data),&usBytesWrite)==FR_OK)
		{
			UARTprintf("Write file success\n");
			f_sync(&g_sFileObject);
		}
		else
		{
			UARTprintf("Write into file Error\n");
		}
	}
	else
	{
		// if the file is exist
		if (f_open(&g_sFileObject, file, FA_CREATE_NEW | FA_WRITE)==FR_EXIST)

		{

			if (f_open(&g_sFileObject, file, FA_OPEN_EXISTING | FA_WRITE)==FR_OK)
			{
				//
				// write file
				//
				if(f_lseek(&g_sFileObject, g_sFileObject.fsize)==FR_OK)
				{
					if(f_write(&g_sFileObject,data,strlen(data),&usBytesWrite)==FR_OK)
					{
						UARTprintf("File append successn");
						f_close(&g_sFileObject);
					}
					else
					{
						UARTprintf("Write into file Errorrn");
					}
				}
			}
			else
			{
				UARTprintf("Append File ceation Error, file not foundn");
			}

		}
		else
			UARTprintf("New File ceation Errorn");
	}

	return(0);
}
//*****************************************************************************
//
// This function implements the "help" command.  It prints a simple list
// of the available commands with a brief description.
//
//*****************************************************************************
int
Cmd_help(int argc, char *argv[])
{
	tCmdLineEntry *pEntry;

	//
	// Print some header text.
	//
	UARTprintf("\nCarSpy MicroSD Testing Commands\n");
	UARTprintf("---------------------------------\n");

	//
	// Point at the beginning of the command table.
	//
	pEntry = &g_sCmdTable[0];

	//
	// Enter a loop to read each entry from the command table.  The
	// end of the table has been reached when the command name is NULL.
	//
	while(pEntry->pcCmd)
	{
		//
		// Print the command name and the brief description.
		//
		UARTprintf("%s%s\n", pEntry->pcCmd, pEntry->pcHelp);

		//
		// Advance to the next entry in the table.
		//
		pEntry++;
	}

	//
	// Return success.
	//
	return(0);
}

//*****************************************************************************
//
// This function implements the "write" command.  It uses the Writefile function to
// write the photo and the coordinates file into the MicroSD Card.
//
//*****************************************************************************
int Cmd_writePhoto(int argc, char *argv[])
{
	//Example of writing a photo into the MicroSD Card
	//This function will be used when the photo is received
	//For each photo the function will be called with the appropriated
	//file name to be created and the array containing the photo.
	//Example:	WriteFile( photo1.txt, photoArray1)
	//			WriteFile( photo2.txt, photoArray2)

	strcpy(g_cTmpBuf, g_cCwdBuf);
	if(strcmp(g_cTmpBuf, "/"))
		{
			strcat(g_cTmpBuf, "/");
		}

		strcat(g_cTmpBuf, "photo.txt");
	WriteFile(g_cTmpBuf,photo);
	//WriteFile("photo5.txt",GPSCoordinate2);


	return(0);


}
int Cmd_writeGPS(int argc, char *argv[])
{
	//WriteFile("photo5.txt",GPSCoordinate2);
	//Example of writing a GPS coordinate in the MicroSD Card
	//When the GPS Module finishes capturing the coordinate this function will be used.
	//First parameter is the coordinate file name and the second is the array with the values.
	//ITS WORKING!!!
	strcpy(g_cTmpBuf, g_cCwdBuf);
	if(strcmp(g_cTmpBuf, "/"))
		{
			strcat(g_cTmpBuf, "/");
		}

		strcat(g_cTmpBuf, "coord.txt");
	WriteFile(g_cTmpBuf,GPSCoordinate);

	return(0);

}
//*****************************************************************************
//
// This function implements the "delete" command.  It uses the fat32 function to
// to delete using the path wanted to be deleted. This function works for directories and files.
//
//*****************************************************************************

int Cmd_delete(int argc, char *argv[])
{	FRESULT fresult;
	strcpy(g_cTmpBuf, g_cCwdBuf);
	if(strcmp(g_cTmpBuf, "/"))
		{
			strcat(g_cTmpBuf, "/");
		}

	strcat(g_cTmpBuf, argv[1]);
	fresult= f_unlink(g_cTmpBuf);
//fresult= f_unlink("coord.txt");

if(fresult != FR_OK)
{
	UARTprintf("Error deleting file");

}
else
{
	UARTprintf("File Successfully Deleted");
}
return(0);


}



int Cmd_size(int argc, char *argv[])
{
	checkSize();
	return (0);
}


int checkSize()
{
	unsigned long ulTotalSize;

	FRESULT fresult;
	FATFS *pFatFs;
	//
	// Get the free space.
	//

	fresult = f_getfree("/", &ulTotalSize, &pFatFs);
	UARTprintf("Space Available: %10uK bytes free\n", ulTotalSize * pFatFs->sects_clust / 2);
	ulTotalSize=ulTotalSize * pFatFs->sects_clust / 2;
	//
	// Check for error and return if there is a problem.
	//
	return (0);

}
int Cmd_createDir(int argc, char *argv[])
{
	FRESULT fresult;
	strcpy(g_cTmpBuf, g_cCwdBuf);
	if(strcmp(g_cTmpBuf, "/"))
	{
		strcat(g_cTmpBuf, "/");
	}

	strcat(g_cTmpBuf, "/");
	strcat(g_cTmpBuf, argv[1]);


	fresult=f_mkdir(g_cTmpBuf);
	if(fresult != FR_OK)
	{
		UARTprintf("Error creating directory");

	}
	else
	{
		UARTprintf("Directory Successfully Created");
	}
	return(0);
}

/*
int Cmd_simulation(int argc, char *argv[])
{
	FRESULT fresult;
	char g_cTmpBufOrigin[PATH_BUF_SIZE];
	char g_cTmpBufFinal[PATH_BUF_SIZE];
	if (sessionFlag==0)
	{
		sessionFlag=1;
		newDirectoryFlag=1;
		sessionItem=0;
	}


	long availability2=checkSize();


	//Read "Origin Session file" to get origin session directory name
	CmdLineProcess(readOrigin);
	//Save Origin Session directory name

	memcpy(g_cTmpBufOrigin, g_cTmpBuf, PATH_BUF_SIZE);
	UARTprintf("%s", g_cTmpBufOrigin);
	//Read "Final Session file" to get final session directory name
	CmdLineProcess(readFinal);
	//Save Final Session file directory
	memcpy(g_cTmpBufFinal, g_cTmpBuf, PATH_BUF_SIZE);
	UARTprintf("%s", g_cTmpBufFinal);
	//size available must be more than 40KB
	//if not then the oldest file (origin) must be deleted
	while (availability2<=40)

	{   //Delete Origin Session Directory to create space
		Cmd_delete(g_cTmpBufOrigin);
		//TODO in integration modify origin file (parse get the integer and add 1)
		//memcpy(readOrigin[CMD_BUF_SIZE], readOrigin[CMD_BUF_SIZE]+1, PATH_BUF_SIZE); //
		//delete origin txt and write a new origin.txt with the updated value (this example is session2)
		Cmd_delete("origin.txt");
		WriteFile("origin.txt","session2");
		//Check again if more space is needed.
		availability2=checkSize();

	}
	//I need to create now the new file or session if needed
	if(newDirectoryFlag==1)
	{
		newDirectoryFlag==0;
		//TODO modify final file (parse get the integer and add 1)
		//memcpy(readFinal[CMD_BUF_SIZE], readFinal[CMD_BUF_SIZE]+1, PATH_BUF_SIZE); //
		//This parameter will be changed with the new origin file
		//EXample the session final before was 9 now is 10
		Cmd_delete("finalS.txt");
		WriteFile("finalS.txt","session10");
		//Now the directory will be created using that new value
		fresult=f_mkdir ("session10");
	}
	//Go to directory of the g_cTmpBufFinal
	WriteFile("gps.txt","GPSCoordinate");
	sessionItem++;


	if (sessionItem==4)
	{
		sessionFlag=0;
		//	ReadGPS&Photofiles // keep them for later transmission
		//	Activate Transmission flag
	}

	//		if (transmissionFlag==1)
	//	{Send files to GSM Module}


	return(0);


}
*/
//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
tCmdLineEntry g_sCmdTable[] =
{
		{ "help",   Cmd_help,      " : Display list of commands" },
		{ "size",     Cmd_size,      "   : Display size Available" },
		{ "list",     Cmd_ls,      "   : Display list of files" },
		{ "changeDir",     Cmd_cd,      "   : Change directory" },
		{ "showDir",    Cmd_pwd,      "  : Show current working directory" },
		{ "read",    Cmd_read,      "  : Read contents of a text file" },
		{ "writeGPS",     Cmd_writeGPS,   "  : Write a Photo & GPS file"},
		{ "delete",     Cmd_delete,   "  : Delete a file"},
		{ "writePhoto",     Cmd_writePhoto,   "  : Write a Photo & GPS file"},
		{ "createDir",     Cmd_createDir,   "  : Create a Directory"},

		//{ "simulation",     Cmd_simulation,   "  : Simulate CarSpy function"},

		{ 0, 0, 0 }
};
//*****************************************************************************
//
// This function implements the "help" command.  It prints a simple list
// of the available commands with a brief description.
//
//*****************************************************************************


//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

//*****************************************************************************
//
// The program main function.  It performs initialization, then runs
// a command processing loop to read commands from the console.
//
//*****************************************************************************
int
main(void)
{
	int nStatus;
	FRESULT fresult;

	//Data to be written

	//
	// Set the system clock to run at ?? from the main oscillator.
	//
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	//
	// Enable the peripherals used by this example.
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	//
	// Configure SysTick for a 100Hz interrupt.  The FatFs driver
	// wants a 10 ms tick.
	//
	SysTickPeriodSet(SysCtlClockGet() / 100);
	SysTickEnable();
	SysTickIntEnable();

	//
	// Enable Interrupts
	//
	IntMasterEnable();

	//
	// Set GPIO A0 and A1 as UART.
	//
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	//
	// Initialize the UART as a console for text I/O.
	//
	UARTStdioInitExpClk(0,9600);
	//UARTStdioInit(0);

	//
	// Print hello message to user.
	//
	UARTprintf("\n\nCarSpy MicroSD Card Test\n");
	UARTprintf("Type \'help\' for help.\n");

	//
	// Mount the file system, using logical disk 0.
	//
	fresult = f_mount(0, &g_sFatFs);
	if(fresult != FR_OK)
	{
		UARTprintf("f_mount error: %s\n", StringFromFresult(fresult));
		return(1);
	}

	//
	// Enter an infinite loop for reading and processing commands from the
	// user.
	//
	while(1)
	{
		//
		// Print a prompt to the console.  Show the CWD.
		//
		//UARTprintf("\n%s> ", g_cCwdBuf);

		//
		// Get a line of text from the user.
		//
		UARTgets(g_cCmdBuf, sizeof(g_cCmdBuf));

		//
		// Pass the line from the user to the command processor.
		// It will be parsed and valid commands executed.
		//
		nStatus = CmdLineProcess(g_cCmdBuf);

		//
		// Handle the case of bad command.
		//
		if(nStatus == CMDLINE_BAD_CMD)
		{
			UARTprintf("Bad command!\n");
		}

		//
		// Handle the case of too many arguments.
		//
		else if(nStatus == CMDLINE_TOO_MANY_ARGS)
		{
			UARTprintf("Too many arguments for command processor!\n");
		}

		//
		// Otherwise the command was executed.  Print the error
		// code if one was returned.
		//
		else if(nStatus != 0)
		{
			UARTprintf("Command returned error code %s\n",
					StringFromFresult((FRESULT)nStatus));
		}
	}
}
