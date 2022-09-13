// dllmain.cpp : Defines the entry point for the DLL application.
// Copyright (C) 2022 Josh Max - See LICENSE for details.
#include "pch.h"

#include <stdio.h>
#include <windows.h>
#include <regex>
#include "../Detours/include/detours.h"

// Debug tracing
//#define DEBUG_TRACE

// Native API function prototypes
typedef LONG NTSTATUS;
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)

// Registry definitions
constexpr PCWSTR REG_SUBKEY_NAME = L"SOFTWARE\\WinHideEx";
constexpr PCWSTR REG_VALUE_NAME_ENABLED = L"HookSettings";
constexpr PCWSTR REG_VALUE_NAME_REGEX = L"RegularExpression";

typedef struct _IO_STATUS_BLOCK
{
	NTSTATUS Status;
	ULONG Information;
} IO_STATUS_BLOCK, * PIO_STATUS_BLOCK;

typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaximumLength;
	PWSTR Buffer;
} UNICODE_STRING, * PUNICODE_STRING;

typedef struct _STRING
{
	USHORT Length;
	USHORT MaximumLength;
	PCHAR  Buffer;
} ANSI_STRING, * PANSI_STRING;

typedef enum class _FILE_INFORMATION_CLASS
{
	FileDirectoryInformation = 1,
	FileFullDirectoryInformation,                   // 2
	FileBothDirectoryInformation,                   // 3
	FileBasicInformation,                           // 4
	FileStandardInformation,                        // 5
	FileInternalInformation,                        // 6
	FileEaInformation,                              // 7
	FileAccessInformation,                          // 8
	FileNameInformation,                            // 9
	FileRenameInformation,                          // 10
	FileLinkInformation,                            // 11
	FileNamesInformation,                           // 12
	FileDispositionInformation,                     // 13
	FilePositionInformation,                        // 14
	FileFullEaInformation,                          // 15
	FileModeInformation,                            // 16
	FileAlignmentInformation,                       // 17
	FileAllInformation,                             // 18
	FileAllocationInformation,                      // 19
	FileEndOfFileInformation,                       // 20
	FileAlternateNameInformation,                   // 21
	FileStreamInformation,                          // 22
	FilePipeInformation,                            // 23
	FilePipeLocalInformation,                       // 24
	FilePipeRemoteInformation,                      // 25
	FileMailslotQueryInformation,                   // 26
	FileMailslotSetInformation,                     // 27
	FileCompressionInformation,                     // 28
	FileObjectIdInformation,                        // 29
	FileCompletionInformation,                      // 30
	FileMoveClusterInformation,                     // 31
	FileQuotaInformation,                           // 32
	FileReparsePointInformation,                    // 33
	FileNetworkOpenInformation,                     // 34
	FileAttributeTagInformation,                    // 35
	FileTrackingInformation,                        // 36
	FileIdBothDirectoryInformation,                 // 37
	FileIdFullDirectoryInformation,                 // 38
	FileValidDataLengthInformation,                 // 39
	FileShortNameInformation,                       // 40
	FileIoCompletionNotificationInformation,        // 41
	FileIoStatusBlockRangeInformation,              // 42
	FileIoPriorityHintInformation,                  // 43
	FileSfioReserveInformation,                     // 44
	FileSfioVolumeInformation,                      // 45
	FileHardLinkInformation,                        // 46
	FileProcessIdsUsingFileInformation,             // 47
	FileNormalizedNameInformation,                  // 48
	FileNetworkPhysicalNameInformation,             // 49
	FileIdGlobalTxDirectoryInformation,             // 50
	FileIsRemoteDeviceInformation,                  // 51
	FileUnusedInformation,                          // 52
	FileNumaNodeInformation,                        // 53
	FileStandardLinkInformation,                    // 54
	FileRemoteProtocolInformation,                  // 55

	FileRenameInformationBypassAccessCheck,         // 56
	FileLinkInformationBypassAccessCheck,           // 57

	FileVolumeNameInformation,                      // 58
	FileIdInformation,                              // 59
	FileIdExtdDirectoryInformation,                 // 60
	FileReplaceCompletionInformation,               // 61
	FileHardLinkFullIdInformation,                  // 62
	FileIdExtdBothDirectoryInformation,             // 63
	FileDispositionInformationEx,                   // 64
	FileRenameInformationEx,                        // 65
	FileRenameInformationExBypassAccessCheck,       // 66
	FileDesiredStorageClassInformation,             // 67
	FileStatInformation,                            // 68
	FileMemoryPartitionInformation,                 // 69
	FileStatLxInformation,                          // 70
	FileCaseSensitiveInformation,                   // 71
	FileLinkInformationEx,                          // 72
	FileLinkInformationExBypassAccessCheck,         // 73
	FileStorageReserveIdInformation,                // 74
	FileCaseSensitiveInformationForceAccessCheck,   // 75
	FileKnownFolderInformation,                     // 76

	FileMaximumInformation
} FILE_INFORMATION_CLASS, * PFILE_INFORMATION_CLASS;

typedef struct _FILE_BOTH_DIRECTORY_INFORMATION
{
	ULONG         NextEntryOffset;
	ULONG         FileIndex;
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER EndOfFile;
	LARGE_INTEGER AllocationSize;
	ULONG         FileAttributes;
	ULONG         FileNameLength;
	ULONG         EaInformationLength;
	UCHAR         AlternateNameLength;
	WCHAR         AlternateName[12];
	WCHAR         FileName[1];
} FILE_BOTH_DIR_INFORMATION, * PFILE_BOTH_DIR_INFORMATION;

typedef struct _FILE_DIRECTORY_INFORMATION
{
	ULONG         NextEntryOffset;
	ULONG         FileIndex;
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER EndOfFile;
	LARGE_INTEGER AllocationSize;
	ULONG         FileAttributes;
	ULONG         FileNameLength;
	WCHAR         FileName[1];
} FILE_DIRECTORY_INFORMATION, * PFILE_DIRECTORY_INFORMATION;


typedef struct _FILE_FULL_DIR_INFORMATION
{
	ULONG         NextEntryOffset;
	ULONG         FileIndex;
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER EndOfFile;
	LARGE_INTEGER AllocationSize;
	ULONG         FileAttributes;
	ULONG         FileNameLength;
	ULONG         EaSize;
	WCHAR         FileName[1];
} FILE_FULL_DIR_INFORMATION, * PFILE_FULL_DIR_INFORMATION;

typedef struct _FILE_ID_FULL_DIR_INFORMATION
{
	ULONG         NextEntryOffset;
	ULONG         FileIndex;
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER EndOfFile;
	LARGE_INTEGER AllocationSize;
	ULONG         FileAttributes;
	ULONG         FileNameLength;
	ULONG         EaSize;
	LARGE_INTEGER FileId;
	WCHAR         FileName[1];
} FILE_ID_FULL_DIR_INFORMATION, * PFILE_ID_FULL_DIR_INFORMATION;

typedef struct _FILE_ID_BOTH_DIR_INFORMATION
{
	ULONG         NextEntryOffset;
	ULONG         FileIndex;
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER EndOfFile;
	LARGE_INTEGER AllocationSize;
	ULONG         FileAttributes;
	ULONG         FileNameLength;
	ULONG         EaSize;
	CCHAR         ShortNameLength;
	WCHAR         ShortName[12];
	LARGE_INTEGER FileId;
	WCHAR         FileName[1];
} FILE_ID_BOTH_DIR_INFORMATION, * PFILE_ID_BOTH_DIR_INFORMATION;

typedef struct _FILE_ID_GLOBAL_TX_DIR_INFORMATION {
	ULONG         NextEntryOffset;
	ULONG         FileIndex;
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER EndOfFile;
	LARGE_INTEGER AllocationSize;
	ULONG         FileAttributes;
	ULONG         FileNameLength;
	LARGE_INTEGER FileId;
	GUID          LockingTransactionId;
	ULONG         TxInfoFlags;
	WCHAR         FileName[1];
} FILE_ID_GLOBAL_TX_DIR_INFORMATION, * PFILE_ID_GLOBAL_TX_DIR_INFORMATION;

typedef struct _FILE_ID_EXTD_DIR_INFORMATION {
	ULONG         NextEntryOffset;
	ULONG         FileIndex;
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER EndOfFile;
	LARGE_INTEGER AllocationSize;
	ULONG         FileAttributes;
	ULONG         FileNameLength;
	ULONG         EaSize;
	ULONG         ReparsePointTag;
	FILE_ID_128   FileId;
	WCHAR         FileName[1];
} FILE_ID_EXTD_DIR_INFORMATION, * PFILE_ID_EXTD_DIR_INFORMATION;

typedef struct _FILE_ID_EXTD_BOTH_DIR_INFORMATION {
	ULONG         NextEntryOffset;
	ULONG         FileIndex;
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	LARGE_INTEGER EndOfFile;
	LARGE_INTEGER AllocationSize;
	ULONG         FileAttributes;
	ULONG         FileNameLength;
	ULONG         EaSize;
	ULONG         ReparsePointTag;
	FILE_ID_128   FileId;
	CCHAR         ShortNameLength;
	WCHAR         ShortName[12];
	WCHAR         FileName[1];
} FILE_ID_EXTD_BOTH_DIR_INFORMATION, * PFILE_ID_EXTD_BOTH_DIR_INFORMATION;

typedef struct _FILE_NAMES_INFORMATION
{
	ULONG NextEntryOffset;
	ULONG FileIndex;
	ULONG FileNameLength;
	WCHAR FileName[1];
} FILE_NAMES_INFORMATION, * PFILE_NAMES_INFORMATION;

typedef VOID(NTAPI* PIO_APC_ROUTINE)(
	IN PVOID ApcContext,
	IN PIO_STATUS_BLOCK IoStatusBlock,
	IN ULONG Reserved);

typedef NTSTATUS(NTAPI* NtQueryDirectoryFile)(
	IN HANDLE FileHandle,
	IN HANDLE Event,
	IN PIO_APC_ROUTINE ApcRoutine,
	IN PVOID ApcContext,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass,
	IN BOOLEAN ReturnSingleEntry,
	IN PUNICODE_STRING FileName,
	IN BOOLEAN RestartScan);

typedef NTSTATUS(NTAPI* NtQueryDirectoryFileEx)(
	IN HANDLE FileHandle,
	IN HANDLE Event,
	IN PIO_APC_ROUTINE ApcRoutine,
	IN PVOID ApcContext,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID FileInformation,
	IN ULONG Length,
	IN FILE_INFORMATION_CLASS FileInformationClass,
	IN ULONG QueryFlags,
	IN PUNICODE_STRING FileName);

// Handles to our original (unhooked) functions
NtQueryDirectoryFile TrueNtQueryDirectoryFile = NULL;
NtQueryDirectoryFileEx TrueNtQueryDirectoryFileEx = NULL;

// Based on code from https://github.com/GiovanniDicanio/ReadStringsFromRegistry/
std::wstring RegistryGetString(HKEY key, PCWSTR subKey, PCWSTR valueName)
{
	DWORD keyType = 0;
	DWORD dataSize = 0;
	const DWORD flags = RRF_RT_REG_SZ; // Only read strings (REG_SZ)
	LONG result = RegGetValue(
		key,
		subKey,
		valueName,
		flags,
		&keyType,
		nullptr,
		&dataSize);

	if (result != ERROR_SUCCESS || keyType != REG_SZ)
		return L"";

	// Allocate a buffer to hold our registry data
	PWCHAR buf = (PWCHAR)malloc(dataSize);

	if (buf == NULL)
		return L"";

	result = RegGetValue(
		key,
		subKey,
		valueName,
		flags,
		nullptr,
		buf, // Write string in this destination buffer
		&dataSize);

	if (result != ERROR_SUCCESS)
		return L"";

	std::wstring res(buf);
	free(buf);

	return res;
}

BOOL RegexCompare(PWCHAR FileName, int NumCharacters)
{
	std::wstring ws(FileName, FileName + NumCharacters);
	std::wstring regVal = RegistryGetString(HKEY_CURRENT_USER, REG_SUBKEY_NAME, REG_VALUE_NAME_REGEX);

	if (regVal.empty())
		return FALSE;

	return std::regex_match(ws, std::wregex(regVal));
}

template <class fiType>
void CheckAndModifyMatchingDetails(fiType FileInformation)
{
	// Check if the client is enabled
	if (RegistryGetString(HKEY_CURRENT_USER, REG_SUBKEY_NAME, REG_VALUE_NAME_ENABLED) != L"enabled")
	{
#ifdef DEBUG_TRACE
		printf("CheckAndModifyMatchingDetails: disabled due to registry config\n");
#endif // DEBUG_TRACE
		
		return;
	}

	// NextEntryOffset is the offset in bytes to the next FileInformation entry returned by the syscall
	// so we first convert the current address to a char* and perform pointer arithmetic. Next, we save
	// the old FileInformation pointer address and compare it to the new address to see if there are more entries
	for (fiType OldFileInformation = NULL; OldFileInformation != FileInformation;
		FileInformation = (fiType)((char*)FileInformation + FileInformation->NextEntryOffset))
	{
		// Save current pointer address
		OldFileInformation = FileInformation;

		// Ignore "." and ".."
		if (FileInformation->FileNameLength == 2 && !memcmp(FileInformation->FileName, L".", 2) ||
			FileInformation->FileNameLength == 4 && !memcmp(FileInformation->FileName, L"..", 4))
			continue;

		// Hide files if they match the loaded regular expression list
		if (RegexCompare(FileInformation->FileName, FileInformation->FileNameLength / sizeof(WCHAR)))
			FileInformation->FileAttributes |= FILE_ATTRIBUTE_HIDDEN;
	}
}

BOOL ModifyFileInformation(FILE_INFORMATION_CLASS FileInformationClass, PVOID FileInformation)
{
#ifdef DEBUG_TRACE
	printf("ModifyFileInformation fiType = %d\n", FileInformationClass);
#endif // DEBUG_TRACE

	switch (FileInformationClass)
	{
	case FILE_INFORMATION_CLASS::FileDirectoryInformation:
		CheckAndModifyMatchingDetails<PFILE_DIRECTORY_INFORMATION>((PFILE_DIRECTORY_INFORMATION)FileInformation);
		break;
	case FILE_INFORMATION_CLASS::FileIdBothDirectoryInformation:
		CheckAndModifyMatchingDetails<PFILE_ID_BOTH_DIR_INFORMATION>((PFILE_ID_BOTH_DIR_INFORMATION)FileInformation);
		break;
	case FILE_INFORMATION_CLASS::FileBothDirectoryInformation:
		CheckAndModifyMatchingDetails<PFILE_BOTH_DIR_INFORMATION>((PFILE_BOTH_DIR_INFORMATION)FileInformation);
		break;
	case FILE_INFORMATION_CLASS::FileIdFullDirectoryInformation:
		CheckAndModifyMatchingDetails<PFILE_ID_FULL_DIR_INFORMATION>((PFILE_ID_FULL_DIR_INFORMATION)FileInformation);
		break;
	case FILE_INFORMATION_CLASS::FileFullDirectoryInformation:
		CheckAndModifyMatchingDetails<PFILE_FULL_DIR_INFORMATION>((PFILE_FULL_DIR_INFORMATION)FileInformation);
		break;
	case FILE_INFORMATION_CLASS::FileIdGlobalTxDirectoryInformation:
		CheckAndModifyMatchingDetails<PFILE_ID_GLOBAL_TX_DIR_INFORMATION>((PFILE_ID_GLOBAL_TX_DIR_INFORMATION)FileInformation);
		break;
	case FILE_INFORMATION_CLASS::FileIdExtdDirectoryInformation:
		CheckAndModifyMatchingDetails<PFILE_ID_EXTD_DIR_INFORMATION>((PFILE_ID_EXTD_DIR_INFORMATION)FileInformation);
		break;
	case FILE_INFORMATION_CLASS::FileIdExtdBothDirectoryInformation:
		CheckAndModifyMatchingDetails<PFILE_ID_EXTD_BOTH_DIR_INFORMATION>((PFILE_ID_EXTD_BOTH_DIR_INFORMATION)FileInformation);
		break;
	default:
#ifdef DEBUG_TRACE
		printf("Irgnored directory fiType = %d\n", FileInformationClass);
#endif // DEBUG_TRACE

		return FALSE;
	}

	return TRUE;
}

NTSTATUS NTAPI NewNtQueryDirectoryFile(HANDLE FileHandle, HANDLE Event, PIO_APC_ROUTINE ApcRoutine,
	PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock, PVOID FileInformation, ULONG Length,
	FILE_INFORMATION_CLASS FileInformationClass, BOOLEAN ReturnSingleEntry, PUNICODE_STRING FileName,
	BOOLEAN RestartScan)
{
#ifdef DEBUG_TRACE
	printf("Hooked NewNtQueryDirectoryFile\n");
#endif // DEBUG_TRACE

	NTSTATUS ret = TrueNtQueryDirectoryFile(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, FileInformation, Length, FileInformationClass, ReturnSingleEntry, FileName, RestartScan);

	if (ret != STATUS_SUCCESS)
		return ret;

	// Modify NewNtQueryDirectoryFile file attributes
	ModifyFileInformation(FileInformationClass, FileInformation);

	return ret;
}

NTSTATUS NTAPI NewNtQueryDirectoryFileEx(HANDLE FileHandle, HANDLE Event, PIO_APC_ROUTINE ApcRoutine,
	PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock, PVOID FileInformation, ULONG Length,
	FILE_INFORMATION_CLASS FileInformationClass, ULONG QueryFlags, PUNICODE_STRING FileName)
{
#ifdef DEBUG_TRACE
	printf("Hooked NewNtQueryDirectoryFileEx\n");
#endif // DEBUG_TRACE

	NTSTATUS ret = TrueNtQueryDirectoryFileEx(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, FileInformation, Length, FileInformationClass, QueryFlags, FileName);

	if (ret != STATUS_SUCCESS)
		return ret;

	// Modify NewNtQueryDirectoryFileEx file attributes
	ModifyFileInformation(FileInformationClass, FileInformation);

	return ret;
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
	LONG error;
	(void)hinst;
	(void)reserved;

	if (DetourIsHelperProcess())
		return TRUE;

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DetourRestoreAfterWith();

#ifdef DEBUG_TRACE
		printf("WinHideEx" DETOURS_STRINGIFY(DETOURS_BITS) ".dll: starting.\n");
		fflush(stdout);
#endif // DEBUG_TRACE

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		// Get our handle to the WinNT API DLL
		HMODULE hNtDLL = GetModuleHandle(L"ntdll.dll");

		if (hNtDLL == NULL)
		{
#ifdef DEBUG_TRACE
			printf("Error detouring NtQueryDirectoryFile, could not get handle to ntdll!");
#endif // DEBUG_TRACE

			return TRUE;
		}

		TrueNtQueryDirectoryFile = (NtQueryDirectoryFile)GetProcAddress(hNtDLL, "NtQueryDirectoryFile");
		TrueNtQueryDirectoryFileEx = (NtQueryDirectoryFileEx)GetProcAddress(hNtDLL, "NtQueryDirectoryFileEx");

		DetourAttach(&(PVOID&)TrueNtQueryDirectoryFile, NewNtQueryDirectoryFile);
		DetourAttach(&(PVOID&)TrueNtQueryDirectoryFileEx, NewNtQueryDirectoryFileEx);
		error = DetourTransactionCommit();

#ifdef DEBUG_TRACE
		if (error == NO_ERROR)
			printf("WinHideEx" DETOURS_STRINGIFY(DETOURS_BITS) ".dll: detoured!\n");
		else
			printf("WinHideEx" DETOURS_STRINGIFY(DETOURS_BITS) ".dll: error detouring: %ld\n", error);
#endif // DEBUG_TRACE
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)TrueNtQueryDirectoryFile, NewNtQueryDirectoryFile);
		DetourDetach(&(PVOID&)TrueNtQueryDirectoryFileEx, NewNtQueryDirectoryFileEx);
		error = DetourTransactionCommit();

#ifdef DEBUG_TRACE
		printf("WinHideEx" DETOURS_STRINGIFY(DETOURS_BITS) ".dll: removed (result=%ld).\n", error);
		fflush(stdout);
#endif // DEBUG_TRACE
	}
	
	return TRUE;
}