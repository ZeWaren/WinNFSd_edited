#include "MountProg.h"
#include "FileTable.h"
#include <string.h>

enum
{
	MOUNTPROC_NULL = 0,
	MOUNTPROC_MNT = 1,
	MOUNTPROC_DUMP = 2,
	MOUNTPROC_UMNT = 3,
	MOUNTPROC_UMNTALL = 4,
	MOUNTPROC_EXPORT = 5
};

enum
{
	MNT_OK = 0,
    MNTERR_PERM = 1,
    MNTERR_NOENT = 2,
	MNTERR_IO = 5,
    MNTERR_ACCESS = 13,
	MNTERR_NOTDIR = 20,
    MNTERR_INVAL = 22
};

typedef void (CMountProg::*PPROC)(void);

CMountProg::CMountProg() : CRPCProg()
{
	m_pExportPath[0] = '\0';
	m_nMountNum = 0;
	memset(m_pClientAddr, 0, sizeof(m_pClientAddr));
}

CMountProg::~CMountProg()
{
	int i;

	for (i = 0; i < MOUNT_NUM_MAX; i++)
		delete[] m_pClientAddr[i];
}

void CMountProg::Export(char *path)
{
	strncpy(m_pExportPath, path, sizeof(m_pExportPath) - 1);
	m_pExportPath[sizeof(m_pExportPath) - 1] = '\0';
}

int CMountProg::GetMountNumber(void)
{
	return m_nMountNum;  //the number of clients mounted
}

char *CMountProg::GetClientAddr(int nIndex)
{
	int i;

	if (nIndex < 0 || nIndex >= m_nMountNum)
		return NULL;
	for (i = 0; i < MOUNT_NUM_MAX; i++)
	{
		if (m_pClientAddr[i] != NULL)
			if (nIndex == 0)
				return m_pClientAddr[i];  //client address
			else
				--nIndex;
	}
	return NULL;
}

int CMountProg::Process(IInputStream *pInStream, IOutputStream *pOutStream, ProcessParam *pParam)
{
	static PPROC pf[] = {&CMountProg::ProcedureNULL, &CMountProg::ProcedureMNT, &CMountProg::ProcedureNOIMP, &CMountProg::ProcedureUMNT};

	PrintLog("MOUNT ");
	if (pParam->nProc >= sizeof(pf) / sizeof(PPROC))
	{
		ProcedureNOIMP();
		PrintLog("\n");
		return PRC_NOTIMP;
	}

	m_pInStream = pInStream;
	m_pOutStream = pOutStream;
	m_pParam = pParam;
	m_nResult = PRC_OK;
	(this->*pf[pParam->nProc])();
	PrintLog("\n");
	return m_nResult;
}

void CMountProg::ProcedureNULL(void)
{
	PrintLog("NULL");
}

void CMountProg::ProcedureMNT(void)
{
	char *path;
	int i;

	PrintLog("MNT");
	path = GetPath();
	PrintLog(" from %s", m_pParam->pRemoteAddr);

	if (m_nMountNum < MOUNT_NUM_MAX && stricmp(path, m_pExportPath) == 0)  //path match
	{
		m_pOutStream->Write(MNT_OK);  //OK
		if (m_pParam->nVersion == 1)
			m_pOutStream->Write(GetFileHandle(path), FHSIZE);  //fhandle
		else
		{
			m_pOutStream->Write(NFS3_FHSIZE);  //length
			m_pOutStream->Write(GetFileHandle(path), NFS3_FHSIZE);  //fhandle
			m_pOutStream->Write(0);  //flavor
		}
		++m_nMountNum;

		for (i = 0; i < MOUNT_NUM_MAX; i++)
			if (m_pClientAddr[i] == NULL)  //search an empty space
			{
				m_pClientAddr[i] = new char[strlen(m_pParam->pRemoteAddr) + 1];
				strcpy(m_pClientAddr[i], m_pParam->pRemoteAddr);  //remember the client address
				break;
			}
	}
	else
		m_pOutStream->Write(MNTERR_ACCESS);  //permission denied
}

void CMountProg::ProcedureUMNT(void)
{
	char *path;
	int i;

	PrintLog("UMNT");
	path = GetPath();
	PrintLog(" from %s", m_pParam->pRemoteAddr);

	for (i = 0; i < MOUNT_NUM_MAX; i++)
		if (m_pClientAddr[i] != NULL)
			if (strcmp(m_pParam->pRemoteAddr, m_pClientAddr[i]) == 0)  //address match
			{
				delete[] m_pClientAddr[i];  //remove this address
				m_pClientAddr[i] = NULL;
				--m_nMountNum;
				break;
			}
}

void CMountProg::ProcedureNOIMP(void)
{
	PrintLog("NOIMP");
	m_nResult = PRC_NOTIMP;
}

char *CMountProg::GetPath(void)
{
	unsigned long i, nSize;
	static char path[MAXPATHLEN + 1];

	m_pInStream->Read(&nSize);
	if (nSize > MAXPATHLEN)
		nSize = MAXPATHLEN;
	m_pInStream->Read(path, nSize);
	path[0] = path[1];  //transform mount path to Windows format
	path[1] = ':';
	for (i = 2; i < nSize; i++)
		if (path[i] == '/')
			path[i] = '\\';
	path[nSize] = '\0';
	PrintLog(" %s", path);

	if ((nSize & 3) != 0)
		m_pInStream->Read(&i, 4 - (nSize & 3));  //skip opaque bytes

	return path;
}
