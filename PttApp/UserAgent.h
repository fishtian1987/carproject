// UserAgent.h: interface for the CUserAgent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USERAGENT_H__357010E9_B9E3_4B19_9615_797D8C1D301F__INCLUDED_)
#define AFX_USERAGENT_H__357010E9_B9E3_4B19_9615_797D8C1D301F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*!
  \fn void CreateAgentAndWrite(BOOL bLoadWrite = FALSE);
  \brief ������̬��ʶ
  \param 
  \param 
  \return
  ������ϸ
*/
void CreateAgentAndWrite(BOOL bLoadWrite = FALSE);

void WriteIELog(char* logInfo);

void CheckProxy();

/*!
  \fn int GetMacListStr(char *macLst, int *lstLen) ;
  \brief //���mac
  \param 
  \param 
  \return
  ������ϸ
*/
int GetMacListStr(char *macLst, int *lstLen) ;

BOOL ChangeFireFox(char* fixagent);

#endif // !defined(AFX_USERAGENT_H__357010E9_B9E3_4B19_9615_797D8C1D301F__INCLUDED_)
