
#include "stdafx.h"
#include "jpglib.h"
#include "jpeglib.h"
#include "../WinApi.h"

// 全局变量定义
static int m_quality = 20;	// JPG图像压缩时的品质
jmp_buf *pCurjmp = NULL;
BYTE* (*DumpBuffFunction)(const void*, unsigned int&, BOOL);
// 函数定义
//1、设置JPG图像压缩时的品质
int SetJpgQuality(int newquality)
//功  能：	设置JPG图像压缩时的品质缺省是50
//入  口：	newquality 新的压缩品质(0-100)
//返回值：	返回被替换下来的品质
{
	int quality = m_quality;
	m_quality = newquality;
	return quality;
}

//(0-100)(缺省是50)
//2、压缩bmp图像数据
BYTE* CompressToJpg(BYTE *pBmpDat, int width, int height, int& outlen,int quality)
//功  能：	压缩bmp图像数据
//入  口：	pBmpDat bmp图像的图像数据缓冲区必须是32位色
//			width	bmp图像的宽度
//			height	bmp图像的高度
//			outlen	jpg图像数据的长度(Byte)
//返回值：	jpg图像的数据缓冲区
{
	// 程序中的细节请参照TestJpg工程
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
		// 如果出现错误，则恢复现场程序从这开始执行
		//		assert(false);

		IF_LOUD(OutputDebugStringA("CompressToJpg From err begin\r\n");)

		jpeg_destroy_compress(&cinfo);
		
		//		if (outfile!=NULL)
		//			fclose(outfile);

		IF_LOUD(OutputDebugStringA("CompressToJpg From err end\r\n");)
		
		return NULL;
	}
	
	// 创建并初始化一个压缩对象
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
			//		  wsprintf(buf, "处理第%d行!\n", cinfo.next_scanline);
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


















