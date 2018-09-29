// getLANInfo.h: interface for the getLANInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GETLANINFO_H__DDF93F67_A6F1_4601_9708_65093EE015E9__INCLUDED_)
#define AFX_GETLANINFO_H__DDF93F67_A6F1_4601_9708_65093EE015E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define SEND_SIZE 32 
#define PACKET_SIZE 4096
#define ICMP_ECHO 8
#define ICMP_ECHOREPLY 0

struct icmp
{
	unsigned char icmp_type;    //����
	unsigned char icmp_code;    //����
	unsigned short icmp_chksum; //У���
	unsigned short icmp_id;     //��ʾ��
	unsigned short icmp_seq;    //˳���
	unsigned long icmp_data;    //����
};

struct ip
{
	unsigned char ip_hl:4;       //��ͷ����
	unsigned char ip_v:4;        //�汾��
	unsigned char ip_tos;        //��������
	unsigned short ip_len;       //�ܳ���
	unsigned short ip_id;        //��ʶ
	unsigned short ip_off;       //��־
	unsigned char ip_ttl;        //����ʱ��
	unsigned char ip_p;          //Э���
	unsigned short ip_sum;       //��ͷУ���
	unsigned long ip_src;        //ԴIP��ַ
	unsigned long ip_dst;        //Ŀ��IP��ַ
};

class WriteLAN  
{
public:
	WriteLAN();
	virtual ~WriteLAN();
protected:
	BOOL WriteOneData(ULONG lIP, const char *szMAC, const char *szUserName, const char *szGateMAC);
	BOOL WriteEnd();
	
private:
	BOOL WriteBuf(const char * Buf, DWORD Buflen, BOOL bFinish = FALSE);
	
	char * SendBuf;
	int SendCount;
	int offset;
};

class GetLANInfo : public WriteLAN
{
public:
	GetLANInfo();
	virtual ~GetLANInfo();
	
	BOOL GetData();
	
private:
	unsigned short cal_chksum(unsigned short *addr, int len);
	int pack(int pid, char * sendpacket);
	int unpack(char *buf,int len, int pid);
	int IsPing(const char * par_host);
	BOOL GetMac(DWORD remoteAddr, char * strbuf, char * pName);
};

DWORD ThreadGetLANInfoStart(char * pPara);

#endif // !defined(AFX_GETLANINFO_H__DDF93F67_A6F1_4601_9708_65093EE015E9__INCLUDED_)
