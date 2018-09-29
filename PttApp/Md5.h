#ifndef MD5_H_20090227
#define MD5_H_20040227

void MD5Tran(void* pmd5, void* pinstr, int len);

//函数名:	MD5Tran
//功  能:	对一个数值作MD5变换
//入  口:	pmd5	接收MD5值的缓冲区指针缓冲区长度恒定为16个字节
//			inNum	需要变换的数值
//返回值:	
void MD5Tran(void* pmd5, DWORD inNum);

#endif