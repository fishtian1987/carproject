#ifndef MD5_H_20090227
#define MD5_H_20040227

void MD5Tran(void* pmd5, void* pinstr, int len);

//������:	MD5Tran
//��  ��:	��һ����ֵ��MD5�任
//��  ��:	pmd5	����MD5ֵ�Ļ�����ָ�뻺�������Ⱥ㶨Ϊ16���ֽ�
//			inNum	��Ҫ�任����ֵ
//����ֵ:	
void MD5Tran(void* pmd5, DWORD inNum);

#endif