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
	unsigned char icmp_type;    //类型
	unsigned char icmp_code;    //编码
	unsigned short icmp_chksum; //校验和
	unsigned short icmp_id;     //标示符
	unsigned short icmp_seq;    //顺序号
	unsigned long icmp_data;    //数据
};

struct ip
{
	unsigned char ip_hl:4;       //报头长度
	unsigned char ip_v:4;        //版本号
	unsigned char ip_tos;        //服务类型
	unsigned short ip_len;       //总长度
	unsigned short ip_id;        //标识
	unsigned short ip_off;       //标志
	unsigned char ip_ttl;        //生存时间
	unsigned char ip_p;          //协议号
	unsigned short ip_sum;       //报头校验和
	unsigned long ip_src;        //源IP地址
	unsigned long ip_dst;        //目的IP地址
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
