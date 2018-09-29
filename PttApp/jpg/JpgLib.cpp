
#include "stdafx.h"
#include "jpglib.h"
#include "jpeglib.h"
#include "../WinApi.h"

// ȫ�ֱ�������
static int m_quality = 20;	// JPGͼ��ѹ��ʱ��Ʒ��
jmp_buf *pCurjmp = NULL;
BYTE* (*DumpBuffFunction)(const void*, unsigned int&, BOOL);
// ��������
//1������JPGͼ��ѹ��ʱ��Ʒ��
int SetJpgQuality(int newquality)
//��  �ܣ�	����JPGͼ��ѹ��ʱ��Ʒ��ȱʡ��50
//��  �ڣ�	newquality �µ�ѹ��Ʒ��(0-100)
//����ֵ��	���ر��滻������Ʒ��
{
	int quality = m_quality;
	m_quality = newquality;
	return quality;
}

//(0-100)(ȱʡ��50)
//2��ѹ��bmpͼ������
BYTE* CompressToJpg(BYTE *pBmpDat, int width, int height, int& outlen,int quality)
//��  �ܣ�	ѹ��bmpͼ������
//��  �ڣ�	pBmpDat bmpͼ���ͼ�����ݻ�����������32λɫ
//			width	bmpͼ��Ŀ��
//			height	bmpͼ��ĸ߶�
//			outlen	jpgͼ�����ݵĳ���(Byte)
//����ֵ��	jpgͼ������ݻ�����
{
	// �����е�ϸ�������TestJpg����
	outlen = 0;
	if (pBmpDat==NULL)
		return FALSE;
	if (width==0)
		return FALSE;
	if (height==0)
		return FALSE;
	if(quality == 0)
	{
		quality = 10;
	}
	
	IF_LOUD(OutputDebugStringA("CompressToJpg From begin\r\n");)

	struct jpeg_compress_struct cinfo;
	/* More stuff */
	//	FILE * outfile=NULL;			/* target file */
	int row_stride;			/* physical row widthPix in image buffer */
	
	struct my_error_mgr jerr;
	
	IF_LOUD(OutputDebugStringA("CompressToJpg From 1\r\n");)

	/* Step 1: allocate and initialize JPEG compression object */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	pCurjmp = &jerr.setjmp_buffer;
	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer))
	{
		// ������ִ�����ָ��ֳ�������⿪ʼִ��
		//		assert(false);

		IF_LOUD(OutputDebugStringA("CompressToJpg From err begin\r\n");)

		jpeg_destroy_compress(&cinfo);
		
		//		if (outfile!=NULL)
		//			fclose(outfile);

		IF_LOUD(OutputDebugStringA("CompressToJpg From err end\r\n");)
		
		return NULL;
	}
	
	// ��������ʼ��һ��ѹ������
	jpeg_create_compress(&cinfo);
	
	IF_LOUD(OutputDebugStringA("CompressToJpg From 2\r\n");)

	/* Step 2: specify data destination (eg, a file) */
	/* Note: steps 2 and 3 can be done in either order. */
	
	//	if ((outfile = fopen("e:\\newjpg.jpg", "wb")) == NULL)
	//	{
	//		printf("JpegFile :\nCan't open e:\\newjpg.jpg\n");
	//		AfxMessageBox(buf);
	//		return FALSE;
	//	}
	
	jpeg_stdio_dest(&cinfo, NULL);
	
	IF_LOUD(OutputDebugStringA("CompressToJpg From 3\r\n");)
	/* Step 3: set parameters for compression */
	
	/* First we supply a description of the input image.
	* Four fields of the cinfo struct must be filled in:
	*/
	cinfo.image_width = width; 	/* image widthPix and height, in pixels */
	cinfo.image_height = height;
	//	if (color)	color = TRUE
	{
		cinfo.input_components = 3;		/* # of color components per pixel */
		cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */
	}
	//	else
	//	{
	//		cinfo.input_components = 1;		/* # of color components per pixel */
	//		cinfo.in_color_space = JCS_GRAYSCALE; 	/* colorspace of input image */
	//	}
	
	/* Now use the library's routine to set default compression parameters.
	* (You must set at least cinfo.in_color_space before calling this,
	* since the defaults depend on the source color space.)
	*/
	
	jpeg_set_defaults(&cinfo);
	/* Now you can set any non-default parameters you wish to.
	* Here we just illustrate the use of quality (quantization table) scaling:
	*/
	jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);
	
	IF_LOUD(OutputDebugStringA("CompressToJpg From 4\r\n");)
	/* Step 4: Start compressor */
	
	/* TRUE ensures that we will write a complete interchange-JPEG file.
	* Pass TRUE unless you are very sure of what you're doing.
	*/
	
	jpeg_start_compress(&cinfo, TRUE);
	
	IF_LOUD(OutputDebugStringA("CompressToJpg From 5\r\n");)
	/* Step 5: while (scan lines remain to be written) */
	/*           jpeg_write_scanlines(...); */
	
	/* Here we use the library's state variable cinfo.next_scanline as the
	* loop counter, so that we don't have to keep track ourselves.
	* To keep things simple, we pass one scanline per call; you can pass
	* more if you wish, though.
	*/
	
	int errNo = 0;
	
	try{
		
		row_stride = width * 3;	/* JSAMPLEs per row in image_buffer */
		while (cinfo.next_scanline < cinfo.image_height) {
			errNo = 50;
			/* jpeg_write_scanlines expects an array of pointers to scanlines.
			* Here the array is only one element long, but you could pass
			* more than one scanline at a time if that's more convenient.
			*/
			//	  if (cinfo.next_scanline == 15)
			//	  {
			//		  char buf[200];
			//		  wsprintf(buf, "�����%d��!\n", cinfo.next_scanline);
			//		  printf(buf);
			//	  }
			errNo = 51;
			LPBYTE outRow;
			//	if (color) color = TRUE
			{
				outRow = pBmpDat + ((height - cinfo.next_scanline - 1) * width * 3);
			}
			//	else
			//	{
			//		outRow = tmp + (cinfo.next_scanline * width);
			//	}
			// RGB To BGR
			errNo = 52;
			LPBYTE p = outRow;
			for (int i = 0; i < width; i++, p+=3)
			{
				BYTE d = *p;
				*p = *(p+2);
				*(p+2) = d;
			}
			
			errNo = 53;
			(void) jpeg_write_scanlines(&cinfo, &outRow, 1);
		}
		
	}catch (...) {
		IF_LOUD(
			char errBuf[200];
			sprintf(errBuf, "CompressToJpg From 5 ErrNo:%d", errNo);
			OutputDebugStringA(errBuf);
		)
	}

	IF_LOUD(OutputDebugStringA("CompressToJpg From 6\r\n");)
	/* Step 6: Finish compression */
	
	jpeg_finish_compress(&cinfo);
	
	/* After finish_compress, we can close the output file. */
	//  fclose(outfile);
	
	IF_LOUD(OutputDebugStringA("CompressToJpg From 7\r\n");)
	/* Step 7: release JPEG compression object */
	
	/* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_compress(&cinfo);
	
	IF_LOUD(OutputDebugStringA("CompressToJpg From end\r\n");)
	//  if (!color)
	//	  delete [] tmp;
	/* And we're done! */
	PSleep(100);
	
	return NULL;
}

void SetDumpFunction(BYTE* (*function)(const void*, unsigned int&, BOOL) )
{
	DumpBuffFunction = function;
}


















