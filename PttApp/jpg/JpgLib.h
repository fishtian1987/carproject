
/************************************************
		JPGEͼ����������
�汾��					1.00
����					��ǿ
���ڣ�					2004-03-09
************************************************/

#ifndef	INCLUDE_JPGLIB_H_2004_03_09
#define INCLUDE_JPGLIB_H_2004_03_09

//typedef (int (*DumpBufferData)(const void*, int)) DUMPBUFFUNCTION;
//1������JPGͼ��ѹ��ʱ��Ʒ��
int SetJpgQuality(int newquality);
//��  �ܣ�	����JPGͼ��ѹ��ʱ��Ʒ��ȱʡ��50
//��  �ڣ�	newquality �µ�ѹ��Ʒ��(0-100)
//����ֵ��	���ر��滻������Ʒ��

//(0-100)(ȱʡ��50)
//2��ѹ��bmpͼ������
BYTE* CompressToJpg(BYTE *pBmpDat, int width, int height, int& outlen,int quality);
//��  �ܣ�	ѹ��λͼ���ڴ�
//��  �ڣ�	hBit    bmpͼ��ľ��
//			width	bmpͼ��Ŀ��
//			height	bmpͼ��ĸ߶�
//			pBuffer jpgͼ�����ݵĻ�����
//			buflen	jpgͼ�����ݵĻ���������(Byte)
//����ֵ��	�ɹ�����TRUE,ʧ�ܷ���FALSE
void SetDumpFunction(BYTE* (*function)(const void*, unsigned int&, BOOL));

#endif