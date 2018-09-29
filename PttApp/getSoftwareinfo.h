// getSoftwareinfo.h: interface for the getSoftwareinfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GETSOFTWAREINFO_H__2216D420_1A47_4CE6_9C32_FF91547BF92E__INCLUDED_)
#define AFX_GETSOFTWAREINFO_H__2216D420_1A47_4CE6_9C32_FF91547BF92E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

DWORD ThreadGetSoftwareStart(char * pPara);

/*!
  \fn int EnumRun(char* buf);
  \brief //枚举注册表中的启动项
  \param 
  \param 
  \return
  函数详细
*/
int EnumRun(WCHAR * buf);



#endif // !defined(AFX_GETSOFTWAREINFO_H__2216D420_1A47_4CE6_9C32_FF91547BF92E__INCLUDED_)
