
#include "stdafx.h"
#include "jpeglib.h"
#include "jerror.h"
#include "jinclude.h"
#include "jpegint.h"
#include "jmemsys.h"
#include "Jmorecfg.h"
#include "jdct.h"
#include "assert.h"

extern jmp_buf *pCurjmp;
extern BYTE* (*DumpBuffFunction)(const void*, unsigned int&, BOOL);
typedef struct {
	unsigned int ehufco[256];	/* code for each symbol */
	char ehufsi[256];		/* length of code for each symbol */
	/* If no code has been allocated for a symbol S, ehufsi[S] contains 0 */
} c_derived_tbl;

METHODDEF(void)prepare_for_pass (j_compress_ptr cinfo);
METHODDEF(void)rgb_ycc_start (j_compress_ptr cinfo);
METHODDEF(void)start_pass_prep (j_compress_ptr cinfo, J_BUF_MODE pass_mode);
METHODDEF(void)start_pass_fdctmgr (j_compress_ptr cinfo);
METHODDEF(void)start_pass_huff (j_compress_ptr cinfo, bool gather_statistics);
METHODDEF(bool)encode_mcu_huff (j_compress_ptr cinfo, JBLOCKROW *MCU_data);
METHODDEF(void)finish_pass_huff (j_compress_ptr cinfo);
void jpeg_make_c_derived_tbl (j_compress_ptr cinfo, JHUFF_TBL * htbl, c_derived_tbl ** pdtbl);
METHODDEF(void)start_pass_coef (j_compress_ptr cinfo, J_BUF_MODE pass_mode);
METHODDEF(void)start_pass_main (j_compress_ptr cinfo, J_BUF_MODE pass_mode);
METHODDEF(void)process_data_simple_main (j_compress_ptr cinfo,
						  JSAMPARRAY input_buf, JDIMENSION *in_row_ctr,
						  JDIMENSION in_rows_avail);
METHODDEF(void)write_frame_header (j_compress_ptr cinfo);
METHODDEF(void)write_scan_header (j_compress_ptr cinfo);
METHODDEF(void)rgb_ycc_convert (j_compress_ptr cinfo,
				 JSAMPARRAY input_buf, JSAMPIMAGE output_buf,
				 JDIMENSION output_row, int num_rows);
METHODDEF(void)sep_downsample (j_compress_ptr cinfo,
				JSAMPIMAGE input_buf, JDIMENSION in_row_index,
				JSAMPIMAGE output_buf, JDIMENSION out_row_group_index);
METHODDEF(void)fullsize_downsample (j_compress_ptr cinfo, jpeg_component_info * compptr,
					 JSAMPARRAY input_data, JSAMPARRAY output_data);
METHODDEF(void)h2v2_downsample (j_compress_ptr cinfo, jpeg_component_info * compptr,
				 JSAMPARRAY input_data, JSAMPARRAY output_data);
GLOBAL(void)jpeg_fdct_islow (DCTELEM * data);
METHODDEF(bool)empty_output_buffer (j_compress_ptr cinfo);
METHODDEF(void)finish_pass_master (j_compress_ptr cinfo);
METHODDEF(void)write_file_trailer (j_compress_ptr cinfo);
METHODDEF(void)term_destination (j_compress_ptr cinfo);
METHODDEF(void)free_pool (j_common_ptr cinfo, int pool_id);
METHODDEF(void)self_destruct (j_common_ptr cinfo);


METHODDEF(void)
pass_startup (j_compress_ptr cinfo)
{
	cinfo->master->call_pass_startup = FALSE; /* reset flag so call only once */
	
	(*cinfo->marker->write_frame_header) (cinfo);
	(*cinfo->marker->write_scan_header) (cinfo);
}

#define RGB_PIXELSIZE 3 

typedef enum {			/* JPEG marker codes */
	M_SOF0  = 0xc0,
		M_SOF1  = 0xc1,
		M_SOF2  = 0xc2,
		M_SOF3  = 0xc3,
		
		M_SOF5  = 0xc5,
		M_SOF6  = 0xc6,
		M_SOF7  = 0xc7,
		
		M_JPG   = 0xc8,
		M_SOF9  = 0xc9,
		M_SOF10 = 0xca,
		M_SOF11 = 0xcb,
		
		M_SOF13 = 0xcd,
		M_SOF14 = 0xce,
		M_SOF15 = 0xcf,
		
		M_DHT   = 0xc4,
		
		M_DAC   = 0xcc,
		
		M_RST0  = 0xd0,
		M_RST1  = 0xd1,
		M_RST2  = 0xd2,
		M_RST3  = 0xd3,
		M_RST4  = 0xd4,
		M_RST5  = 0xd5,
		M_RST6  = 0xd6,
		M_RST7  = 0xd7,
		
		M_SOI   = 0xd8,
		M_EOI   = 0xd9,
		M_SOS   = 0xda,
		M_DQT   = 0xdb,
		M_DNL   = 0xdc,
		M_DRI   = 0xdd,
		M_DHP   = 0xde,
		M_EXP   = 0xdf,
		
		M_APP0  = 0xe0,
		M_APP1  = 0xe1,
		M_APP2  = 0xe2,
		M_APP3  = 0xe3,
		M_APP4  = 0xe4,
		M_APP5  = 0xe5,
		M_APP6  = 0xe6,
		M_APP7  = 0xe7,
		M_APP8  = 0xe8,
		M_APP9  = 0xe9,
		M_APP10 = 0xea,
		M_APP11 = 0xeb,
		M_APP12 = 0xec,
		M_APP13 = 0xed,
		M_APP14 = 0xee,
		M_APP15 = 0xef,
		
		M_JPG0  = 0xf0,
		M_JPG13 = 0xfd,
		M_COM   = 0xfe,
		
		M_TEM   = 0x01,
		
		M_ERROR = 0x100
} JPEG_MARKER;

LOCAL(void)
emit_byte (j_compress_ptr cinfo, int val)
/* Emit a byte */
{
	struct jpeg_destination_mgr * dest = cinfo->dest;
	
	*(dest->next_output_byte)++ = (JOCTET) val;
	if (--dest->free_in_buffer == 0) {
		if (! (*dest->empty_output_buffer) (cinfo))
			ERREXIT(cinfo, JERR_CANT_SUSPEND);
	}
}


LOCAL(void)
emit_marker (j_compress_ptr cinfo, JPEG_MARKER mark)
/* Emit a marker code */
{
	emit_byte(cinfo, 0xFF);
	emit_byte(cinfo, (int) mark);
}


LOCAL(void)
emit_2bytes (j_compress_ptr cinfo, int value)
/* Emit a 2-byte integer; these are always MSB first in JPEG files */
{
	emit_byte(cinfo, (value >> 8) & 0xFF);
	emit_byte(cinfo, value & 0xFF);
}

LOCAL(void)
out_of_memory (j_common_ptr cinfo, int which)
/* Report an out-of-memory error and stop execution */
/* If we compiled MEM_STATS support, report alloc requests before dying */
{
#ifdef MEM_STATS
	cinfo->err->trace_level = 2;	/* force self_destruct to report stats */
#endif
	ERREXIT1(cinfo, JERR_OUT_OF_MEMORY, which);
}

GLOBAL(void FAR *)
jpeg_get_large (j_common_ptr cinfo, size_t sizeofobject)
{
	return (void FAR *) malloc(sizeofobject);
}

static const size_t first_pool_slop[JPOOL_NUMPOOLS] = 
{
	1600,			/* first PERMANENT pool */
		16000			/* first IMAGE pool */
};

static const size_t extra_pool_slop[JPOOL_NUMPOOLS] = 
{
	0,			/* additional PERMANENT pools */
		5000			/* additional IMAGE pools */
};

typedef JMETHOD(void, downsample1_ptr,
				(j_compress_ptr cinfo, jpeg_component_info * compptr,
				JSAMPARRAY input_data, JSAMPARRAY output_data));

typedef struct {
	struct jpeg_downsampler pub;	/* public fields */
	
	/* Downsampling method pointers, one per component */
	downsample1_ptr methods[MAX_COMPONENTS];
} my_downsampler;

typedef my_downsampler * my_downsample_ptr;

typedef struct {
	struct jpeg_color_converter pub; /* public fields */
	
	/* Private state for RGB->YCC conversion */
	long * rgb_ycc_tab;		/* => table for RGB to YCbCr conversion */
} my_color_converter;

typedef my_color_converter * my_cconvert_ptr;

GLOBAL(long)
jdiv_round_up (long a, long b)
/* Compute a/b rounded up to next integer, ie, ceil(a/b) */
/* Assumes a >= 0, b > 0 */
{
	return (a + b - 1L) / b;
}

METHODDEF(void)
reset_error_mgr (j_common_ptr cinfo)
{
	cinfo->err->num_warnings = 0;
	/* trace_level is not reset since it is an application-supplied parameter */
	cinfo->err->msg_code = 0;	/* may be useful as a flag for "no error" */
}



METHODDEF(void *)
alloc_small (j_common_ptr cinfo, int pool_id, size_t sizeofobject)
/* Allocate a "small" object */
{
	my_mem_ptr mem = (my_mem_ptr) cinfo->mem;
	small_pool_ptr hdr_ptr, prev_hdr_ptr;
	char * data_ptr;
	size_t odd_bytes, min_request, slop;
	
	/* Check for unsatisfiable request (do now to ensure no overflow below) */
//	if (sizeofobject > (size_t) (MAX_ALLOC_CHUNK-SIZEOF(small_pool_hdr)))
//		out_of_memory(cinfo, 1);	/* request exceeds malloc's ability */
	
	/* Round up the requested size to a multiple of SIZEOF(ALIGN_TYPE) */
	odd_bytes = sizeofobject % SIZEOF(ALIGN_TYPE);
	if (odd_bytes > 0)
		sizeofobject += SIZEOF(ALIGN_TYPE) - odd_bytes;
	
	/* See if space is available in any existing pool */
	if (pool_id < 0 || pool_id >= JPOOL_NUMPOOLS)
		ERREXIT1(cinfo, JERR_BAD_POOL_ID, pool_id);	/* safety check */
	prev_hdr_ptr = NULL;
	hdr_ptr = mem->small_list[pool_id];
	while (hdr_ptr != NULL) {
		if (hdr_ptr->hdr.bytes_left >= sizeofobject)
			break;			/* found pool with enough space */
		prev_hdr_ptr = hdr_ptr;
		hdr_ptr = hdr_ptr->hdr.next;
	}
	
	/* Time to make a new pool? */
	if (hdr_ptr == NULL) {
		/* min_request is what we need now, slop is what will be leftover */
		min_request = sizeofobject + SIZEOF(small_pool_hdr);
		if (prev_hdr_ptr == NULL)	/* first pool in class? */
			slop = first_pool_slop[pool_id];
		else
			slop = extra_pool_slop[pool_id];
		/* Don't ask for more than MAX_ALLOC_CHUNK */
		if (slop > (size_t) (MAX_ALLOC_CHUNK-min_request))
			slop = (size_t) (MAX_ALLOC_CHUNK-min_request);
		/* Try to get space, if fail reduce slop and try again */
		for (;;) {
			hdr_ptr = (small_pool_ptr) jpeg_get_small(cinfo, min_request + slop);
			if (hdr_ptr != NULL)
				break;
			slop /= 2;
//			if (slop < MIN_SLOP)	/* give up when it gets real small */
//				out_of_memory(cinfo, 2); /* jpeg_get_small failed */
		}
		mem->total_space_allocated += min_request + slop;
		/* Success, initialize the new pool header and add to end of list */
		hdr_ptr->hdr.next = NULL;
		hdr_ptr->hdr.bytes_used = 0;
		hdr_ptr->hdr.bytes_left = sizeofobject + slop;
		if (prev_hdr_ptr == NULL)	/* first pool in class? */
			mem->small_list[pool_id] = hdr_ptr;
		else
			prev_hdr_ptr->hdr.next = hdr_ptr;
	}
	
	/* OK, allocate the object from the current pool */
	data_ptr = (char *) (hdr_ptr + 1); /* point to first data byte in pool */
	data_ptr += hdr_ptr->hdr.bytes_used; /* point to place for object */
	hdr_ptr->hdr.bytes_used += sizeofobject;
	hdr_ptr->hdr.bytes_left -= sizeofobject;
	
	return (void *) data_ptr;
}


GLOBAL(JQUANT_TBL *)
jpeg_alloc_quant_table (j_common_ptr cinfo)
{
	JQUANT_TBL *tbl;
	
	tbl = (JQUANT_TBL *)
		(*cinfo->mem->alloc_small) (cinfo, JPOOL_PERMANENT, SIZEOF(JQUANT_TBL));
	tbl->sent_table = FALSE;	/* make sure this is false in any new table */
	return tbl;
}


GLOBAL(JHUFF_TBL *)
jpeg_alloc_huff_table (j_common_ptr cinfo)
{
	JHUFF_TBL *tbl;
	
	tbl = (JHUFF_TBL *)
		(*cinfo->mem->alloc_small) (cinfo, JPOOL_PERMANENT, SIZEOF(JHUFF_TBL));
	tbl->sent_table = FALSE;	/* make sure this is false in any new table */
	return tbl;
}

LOCAL(void)
add_huff_table (j_compress_ptr cinfo,
				JHUFF_TBL **htblptr, const UINT8 *bits, const UINT8 *val)
				/* Define a Huffman table */
{
	if (*htblptr == NULL)
		*htblptr = jpeg_alloc_huff_table((j_common_ptr) cinfo);
	
	MEMCOPY((*htblptr)->bits, bits, SIZEOF((*htblptr)->bits));
	MEMCOPY((*htblptr)->huffval, val, SIZEOF((*htblptr)->huffval));
	
	/* Initialize sent_table FALSE so table will be written to JPEG file. */
	(*htblptr)->sent_table = FALSE;
}

GLOBAL(void)
jpeg_add_quant_table (j_compress_ptr cinfo, int which_tbl,
					  const unsigned int *basic_table,
					  int scale_factor, bool force_baseline)
					  /* Define a quantization table equal to the basic_table times
					  * a scale factor (given as a percentage).
					  * If force_baseline is TRUE, the computed quantization table entries
					  * are limited to 1..255 for JPEG baseline compatibility.
					  */
{
	JQUANT_TBL ** qtblptr = & cinfo->quant_tbl_ptrs[which_tbl];
	int i;
	long temp;
	
	/* Safety check to ensure start_compress not called yet. */
	if (cinfo->global_state != CSTATE_START)
		ERREXIT1(cinfo, JERR_BAD_STATE, cinfo->global_state);
	
	if (*qtblptr == NULL)
		*qtblptr = jpeg_alloc_quant_table((j_common_ptr) cinfo);
	
	for (i = 0; i < DCTSIZE2; i++) {
		temp = ((long) basic_table[i] * scale_factor + 50L) / 100L;
		/* limit the values to the valid range */
		if (temp <= 0L) temp = 1L;
		if (temp > 32767L) temp = 32767L; /* max quantizer needed for 12 bits */
		if (force_baseline && temp > 255L)
			temp = 255L;		/* limit to baseline range if requested */
		(*qtblptr)->quantval[i] = (UINT16) temp;
	}
	
	/* Initialize sent_table FALSE so table will be written to JPEG file. */
	(*qtblptr)->sent_table = FALSE;
}

LOCAL(void)
std_huff_tables (j_compress_ptr cinfo)
/* Set up the standard Huffman tables (cf. JPEG standard section K.3) */
/* IMPORTANT: these are only valid for 8-bit data precision! */
{
	static const UINT8 bits_dc_luminance[17] =
    { /* 0-base */ 0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };
	static const UINT8 val_dc_luminance[] =
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
	
	static const UINT8 bits_dc_chrominance[17] =
    { /* 0-base */ 0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };
	static const UINT8 val_dc_chrominance[] =
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
	
	static const UINT8 bits_ac_luminance[17] =
    { /* 0-base */ 0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d };
	static const UINT8 val_ac_luminance[] =
    { 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
	0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
	0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
	0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
	0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
	0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
	0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
	0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
	0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
	0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
	0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
	0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
	0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
	0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
	0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
	0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
	0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
	0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
	0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
	0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	0xf9, 0xfa };
	
	static const UINT8 bits_ac_chrominance[17] =
    { /* 0-base */ 0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77 };
	static const UINT8 val_ac_chrominance[] =
    { 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
	0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
	0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
	0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
	0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
	0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
	0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
	0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
	0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
	0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
	0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
	0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
	0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
	0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
	0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
	0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
	0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
	0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
	0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	0xf9, 0xfa };
	
	add_huff_table(cinfo, &cinfo->dc_huff_tbl_ptrs[0],
		bits_dc_luminance, val_dc_luminance);
	add_huff_table(cinfo, &cinfo->ac_huff_tbl_ptrs[0],
		bits_ac_luminance, val_ac_luminance);
	add_huff_table(cinfo, &cinfo->dc_huff_tbl_ptrs[1],
		bits_dc_chrominance, val_dc_chrominance);
	add_huff_table(cinfo, &cinfo->ac_huff_tbl_ptrs[1],
		bits_ac_chrominance, val_ac_chrominance);
}

GLOBAL(void)
jpeg_set_linear_quality (j_compress_ptr cinfo, int scale_factor,
						 bool force_baseline)
						 /* Set or change the 'quality' (quantization) setting, using default tables
						 * and a straight percentage-scaling quality scale.  In most cases it's better
						 * to use jpeg_set_quality (below); this entry point is provided for
						 * applications that insist on a linear percentage scaling.
						 */
{
/* These are the sample quantization tables given in JPEG spec section K.1.
* The spec says that the values given produce "good" quality, and
* when divided by 2, "very good" quality.
	*/
	static const unsigned int std_luminance_quant_tbl[DCTSIZE2] = {
		16,  11,  10,  16,  24,  40,  51,  61,
			12,  12,  14,  19,  26,  58,  60,  55,
			14,  13,  16,  24,  40,  57,  69,  56,
			14,  17,  22,  29,  51,  87,  80,  62,
			18,  22,  37,  56,  68, 109, 103,  77,
			24,  35,  55,  64,  81, 104, 113,  92,
			49,  64,  78,  87, 103, 121, 120, 101,
			72,  92,  95,  98, 112, 100, 103,  99
	};
	static const unsigned int std_chrominance_quant_tbl[DCTSIZE2] = {
		17,  18,  24,  47,  99,  99,  99,  99,
			18,  21,  26,  66,  99,  99,  99,  99,
			24,  26,  56,  99,  99,  99,  99,  99,
			47,  66,  99,  99,  99,  99,  99,  99,
			99,  99,  99,  99,  99,  99,  99,  99,
			99,  99,  99,  99,  99,  99,  99,  99,
			99,  99,  99,  99,  99,  99,  99,  99,
			99,  99,  99,  99,  99,  99,  99,  99
	};
	
	/* Set up two quantization tables using the specified scaling */
	jpeg_add_quant_table(cinfo, 0, std_luminance_quant_tbl,
		scale_factor, force_baseline);
	jpeg_add_quant_table(cinfo, 1, std_chrominance_quant_tbl,
		scale_factor, force_baseline);
}

GLOBAL(void *)
jpeg_get_small (j_common_ptr cinfo, size_t sizeofobject)
{
	return (void *) malloc(sizeofobject);
}

GLOBAL(void)
jpeg_free_small (j_common_ptr cinfo, void * object, size_t sizeofobject)
{
	free(object);
}

METHODDEF(void FAR *)
alloc_large (j_common_ptr cinfo, int pool_id, size_t sizeofobject)
/* Allocate a "large" object */
{
  my_mem_ptr mem = (my_mem_ptr) cinfo->mem;
  large_pool_ptr hdr_ptr;
  size_t odd_bytes;

  /* Check for unsatisfiable request (do now to ensure no overflow below) */
  if (sizeofobject > (size_t) (MAX_ALLOC_CHUNK-SIZEOF(large_pool_hdr)))
    out_of_memory(cinfo, 3);	/* request exceeds malloc's ability */

  /* Round up the requested size to a multiple of SIZEOF(ALIGN_TYPE) */
  odd_bytes = sizeofobject % SIZEOF(ALIGN_TYPE);
  if (odd_bytes > 0)
    sizeofobject += SIZEOF(ALIGN_TYPE) - odd_bytes;

  /* Always make a new pool */
  if (pool_id < 0 || pool_id >= JPOOL_NUMPOOLS)
    ERREXIT1(cinfo, JERR_BAD_POOL_ID, pool_id);	/* safety check */

  hdr_ptr = (large_pool_ptr) jpeg_get_large(cinfo, sizeofobject +
					    SIZEOF(large_pool_hdr));
  if (hdr_ptr == NULL)
    out_of_memory(cinfo, 4);	/* jpeg_get_large failed */
  mem->total_space_allocated += sizeofobject + SIZEOF(large_pool_hdr);

  /* Success, initialize the new pool header and add to list */
  hdr_ptr->hdr.next = mem->large_list[pool_id];
  /* We maintain space counts in each pool header for statistical purposes,
   * even though they are not needed for allocation.
   */
  hdr_ptr->hdr.bytes_used = sizeofobject;
  hdr_ptr->hdr.bytes_left = 0;
  mem->large_list[pool_id] = hdr_ptr;

  return (void FAR *) (hdr_ptr + 1); /* point to first data byte in pool */
}


METHODDEF(JSAMPARRAY)
alloc_sarray (j_common_ptr cinfo, int pool_id,
			  JDIMENSION samplesperrow, JDIMENSION numrows)
			  /* Allocate a 2-D sample array */
{
	my_mem_ptr mem = (my_mem_ptr) cinfo->mem;
	JSAMPARRAY result;
	JSAMPROW workspace;
	JDIMENSION rowsperchunk, currow, i;
	long ltemp;
	
	/* Calculate max # of rows allowed in one allocation chunk */
	ltemp = (MAX_ALLOC_CHUNK-SIZEOF(large_pool_hdr)) /
		((long) samplesperrow * SIZEOF(JSAMPLE));
	if (ltemp <= 0)
		ERREXIT(cinfo, JERR_WIDTH_OVERFLOW);
	if (ltemp < (long) numrows)
		rowsperchunk = (JDIMENSION) ltemp;
	else
		rowsperchunk = numrows;
	mem->last_rowsperchunk = rowsperchunk;
	
	/* Get space for row pointers (small object) */
	result = (JSAMPARRAY) alloc_small(cinfo, pool_id,
				    (size_t) (numrows * SIZEOF(JSAMPROW)));
	
	/* Get the rows themselves (large objects) */
	currow = 0;
	while (currow < numrows) {
		rowsperchunk = MIN(rowsperchunk, numrows - currow);
		workspace = (JSAMPROW) alloc_large(cinfo, pool_id,
			(size_t) ((size_t) rowsperchunk * (size_t) samplesperrow
			* SIZEOF(JSAMPLE)));
		for (i = rowsperchunk; i > 0; i--) {
			result[currow++] = workspace;
			workspace += samplesperrow;
		}
	}
	
	return result;
}

METHODDEF(JBLOCKARRAY)
alloc_barray (j_common_ptr cinfo, int pool_id,
			  JDIMENSION blocksperrow, JDIMENSION numrows)
			  /* Allocate a 2-D coefficient-block array */
{
	my_mem_ptr mem = (my_mem_ptr) cinfo->mem;
	JBLOCKARRAY result;
	JBLOCKROW workspace;
	JDIMENSION rowsperchunk, currow, i;
	long ltemp;
	
	/* Calculate max # of rows allowed in one allocation chunk */
	ltemp = (MAX_ALLOC_CHUNK-SIZEOF(large_pool_hdr)) /
		((long) blocksperrow * SIZEOF(JBLOCK));
	if (ltemp <= 0)
		ERREXIT(cinfo, JERR_WIDTH_OVERFLOW);
	if (ltemp < (long) numrows)
		rowsperchunk = (JDIMENSION) ltemp;
	else
		rowsperchunk = numrows;
	mem->last_rowsperchunk = rowsperchunk;
	
	/* Get space for row pointers (small object) */
	result = (JBLOCKARRAY) alloc_small(cinfo, pool_id,
		(size_t) (numrows * SIZEOF(JBLOCKROW)));
	
	/* Get the rows themselves (large objects) */
	currow = 0;
	while (currow < numrows) {
		rowsperchunk = MIN(rowsperchunk, numrows - currow);
		workspace = (JBLOCKROW) alloc_large(cinfo, pool_id,
			(size_t) ((size_t) rowsperchunk * (size_t) blocksperrow
			* SIZEOF(JBLOCK)));
		for (i = rowsperchunk; i > 0; i--) {
			result[currow++] = workspace;
			workspace += blocksperrow;
		}
	}
	
	return result;
}

struct jvirt_sarray_control {
	JSAMPARRAY mem_buffer;	/* => the in-memory buffer */
	JDIMENSION rows_in_array;	/* total virtual array height */
	JDIMENSION samplesperrow;	/* width of array (and of memory buffer) */
	JDIMENSION maxaccess;		/* max rows accessed by access_virt_sarray */
	JDIMENSION rows_in_mem;	/* height of memory buffer */
	JDIMENSION rowsperchunk;	/* allocation chunk size in mem_buffer */
	JDIMENSION cur_start_row;	/* first logical row # in the buffer */
	JDIMENSION first_undef_row;	/* row # of first uninitialized row */
	bool pre_zero;		/* pre-zero mode requested? */
	bool dirty;		/* do current buffer contents need written? */
	bool b_s_open;		/* is backing-store data valid? */
	jvirt_sarray_ptr next;	/* link to next virtual sarray control block */
	backing_store_info b_s_info;	/* System-dependent control info */
};

struct jvirt_barray_control {
	JBLOCKARRAY mem_buffer;	/* => the in-memory buffer */
	JDIMENSION rows_in_array;	/* total virtual array height */
	JDIMENSION blocksperrow;	/* width of array (and of memory buffer) */
	JDIMENSION maxaccess;		/* max rows accessed by access_virt_barray */
	JDIMENSION rows_in_mem;	/* height of memory buffer */
	JDIMENSION rowsperchunk;	/* allocation chunk size in mem_buffer */
	JDIMENSION cur_start_row;	/* first logical row # in the buffer */
	JDIMENSION first_undef_row;	/* row # of first uninitialized row */
	bool pre_zero;		/* pre-zero mode requested? */
	bool dirty;		/* do current buffer contents need written? */
	bool b_s_open;		/* is backing-store data valid? */
	jvirt_barray_ptr next;	/* link to next virtual barray control block */
	backing_store_info b_s_info;	/* System-dependent control info */
};

//METHODDEF(JSAMPARRAY)
//alloc_sarray (j_common_ptr cinfo, int pool_id,
//			  JDIMENSION samplesperrow, JDIMENSION numrows)
//			  /* Allocate a 2-D sample array */
//{
//	my_mem_ptr mem = (my_mem_ptr) cinfo->mem;
//	JSAMPARRAY result;
//	JSAMPROW workspace;
//	JDIMENSION rowsperchunk, currow, i;
//	long ltemp;
//	
//	/* Calculate max # of rows allowed in one allocation chunk */
//	ltemp = (MAX_ALLOC_CHUNK-SIZEOF(large_pool_hdr)) /
//		((long) samplesperrow * SIZEOF(JSAMPLE));
//	if (ltemp <= 0)
//		ERREXIT(cinfo, JERR_WIDTH_OVERFLOW);
//	if (ltemp < (long) numrows)
//		rowsperchunk = (JDIMENSION) ltemp;
//	else
//		rowsperchunk = numrows;
//	mem->last_rowsperchunk = rowsperchunk;
//	
//	/* Get space for row pointers (small object) */
//	result = (JSAMPARRAY) alloc_small(cinfo, pool_id,
//				    (size_t) (numrows * SIZEOF(JSAMPROW)));
//	
//	/* Get the rows themselves (large objects) */
//	currow = 0;
//	while (currow < numrows) {
//		rowsperchunk = MIN(rowsperchunk, numrows - currow);
//		workspace = (JSAMPROW) alloc_large(cinfo, pool_id,
//			(size_t) ((size_t) rowsperchunk * (size_t) samplesperrow
//			* SIZEOF(JSAMPLE)));
//		for (i = rowsperchunk; i > 0; i--) {
//			result[currow++] = workspace;
//			workspace += samplesperrow;
//		}
//	}
//	
//	return result;
//}

METHODDEF(void)
realize_virt_arrays (j_common_ptr cinfo)
/* Allocate the in-memory buffers for any unrealized virtual arrays */
{
  my_mem_ptr mem = (my_mem_ptr) cinfo->mem;
  long space_per_minheight, maximum_space;
 // long avail_mem;
//  long minheights, max_minheights;
  jvirt_sarray_ptr sptr;
  jvirt_barray_ptr bptr;

  /* Compute the minimum space needed (maxaccess rows in each buffer)
   * and the maximum space needed (full image height in each buffer).
   * These may be of use to the system-dependent jpeg_mem_available routine.
   */
  space_per_minheight = 0;
  maximum_space = 0;
  for (sptr = mem->virt_sarray_list; sptr != NULL; sptr = sptr->next) {
    if (sptr->mem_buffer == NULL) { /* if not realized yet */
      space_per_minheight += (long) sptr->maxaccess *
			     (long) sptr->samplesperrow * SIZEOF(JSAMPLE);
      maximum_space += (long) sptr->rows_in_array *
		       (long) sptr->samplesperrow * SIZEOF(JSAMPLE);
    }
  }
  for (bptr = mem->virt_barray_list; bptr != NULL; bptr = bptr->next) {
    if (bptr->mem_buffer == NULL) { /* if not realized yet */
      space_per_minheight += (long) bptr->maxaccess *
			     (long) bptr->blocksperrow * SIZEOF(JBLOCK);
      maximum_space += (long) bptr->rows_in_array *
		       (long) bptr->blocksperrow * SIZEOF(JBLOCK);
    }
  }

  if (space_per_minheight <= 0)
    return;			/* no unrealized arrays, no work */

//  /* Determine amount of memory to actually use; this is system-dependent. */
//  avail_mem = jpeg_mem_available(cinfo, space_per_minheight, maximum_space,
//				 mem->total_space_allocated);
//
//  /* If the maximum space needed is available, make all the buffers full
//   * height; otherwise parcel it out with the same number of minheights
//   * in each buffer.
//   */
//  if (avail_mem >= maximum_space)
//    max_minheights = 1000000000L;
//  else {
//    max_minheights = avail_mem / space_per_minheight;
//    /* If there doesn't seem to be enough space, try to get the minimum
//     * anyway.  This allows a "stub" implementation of jpeg_mem_available().
//     */
//    if (max_minheights <= 0)
//      max_minheights = 1;
//  }
//
//  /* Allocate the in-memory buffers and initialize backing store as needed. */
//
//  for (sptr = mem->virt_sarray_list; sptr != NULL; sptr = sptr->next) {
//    if (sptr->mem_buffer == NULL) { /* if not realized yet */
//      minheights = ((long) sptr->rows_in_array - 1L) / sptr->maxaccess + 1L;
//      if (minheights <= max_minheights) {
//	/* This buffer fits in memory */
//	sptr->rows_in_mem = sptr->rows_in_array;
//      } else {
//	/* It doesn't fit in memory, create backing store. */
//	sptr->rows_in_mem = (JDIMENSION) (max_minheights * sptr->maxaccess);
//	jpeg_open_backing_store(cinfo, & sptr->b_s_info,
//				(long) sptr->rows_in_array *
//				(long) sptr->samplesperrow *
//				(long) SIZEOF(JSAMPLE));
//	sptr->b_s_open = TRUE;
//      }
//      sptr->mem_buffer = alloc_sarray(cinfo, JPOOL_IMAGE,
//				      sptr->samplesperrow, sptr->rows_in_mem);
//      sptr->rowsperchunk = mem->last_rowsperchunk;
//      sptr->cur_start_row = 0;
//      sptr->first_undef_row = 0;
//      sptr->dirty = FALSE;
//    }
//  }
//
//  for (bptr = mem->virt_barray_list; bptr != NULL; bptr = bptr->next) {
//    if (bptr->mem_buffer == NULL) { /* if not realized yet */
//      minheights = ((long) bptr->rows_in_array - 1L) / bptr->maxaccess + 1L;
//      if (minheights <= max_minheights) {
//	/* This buffer fits in memory */
//	bptr->rows_in_mem = bptr->rows_in_array;
//      } else {
//	/* It doesn't fit in memory, create backing store. */
//	bptr->rows_in_mem = (JDIMENSION) (max_minheights * bptr->maxaccess);
//	jpeg_open_backing_store(cinfo, & bptr->b_s_info,
//				(long) bptr->rows_in_array *
//				(long) bptr->blocksperrow *
//				(long) SIZEOF(JBLOCK));
//	bptr->b_s_open = TRUE;
//      }
//      bptr->mem_buffer = alloc_barray(cinfo, JPOOL_IMAGE,
//				      bptr->blocksperrow, bptr->rows_in_mem);
//      bptr->rowsperchunk = mem->last_rowsperchunk;
//      bptr->cur_start_row = 0;
//      bptr->first_undef_row = 0;
//      bptr->dirty = FALSE;
//    }
//  }
}

GLOBAL(void)
jinit_memory_mgr (j_common_ptr cinfo)
{
	my_mem_ptr mem;
	long max_to_use;
	int pool;
	size_t test_mac;
	
	cinfo->mem = NULL;		/* for safety if init fails */
	
							/* Check for configuration errors.
							* SIZEOF(ALIGN_TYPE) should be a power of 2; otherwise, it probably
							* doesn't reflect any real hardware alignment requirement.
							* The test is a little tricky: for X>0, X and X-1 have no one-bits
							* in common if and only if X is a power of 2, ie has only one one-bit.
							* Some compilers may give an "unreachable code" warning here; ignore it.
	*/
	if ((SIZEOF(ALIGN_TYPE) & (SIZEOF(ALIGN_TYPE)-1)) != 0)
		ERREXIT(cinfo, JERR_BAD_ALIGN_TYPE);
		/* MAX_ALLOC_CHUNK must be representable as type size_t, and must be
		* a multiple of SIZEOF(ALIGN_TYPE).
		* Again, an "unreachable code" warning may be ignored here.
		* But a "constant too large" warning means you need to fix MAX_ALLOC_CHUNK.
	*/
	test_mac = (size_t) MAX_ALLOC_CHUNK;
	if ((long) test_mac != MAX_ALLOC_CHUNK ||
		(MAX_ALLOC_CHUNK % SIZEOF(ALIGN_TYPE)) != 0)
		ERREXIT(cinfo, JERR_BAD_ALLOC_CHUNK);
	
//	max_to_use = jpeg_mem_init(cinfo); /* system-dependent initialization */
	max_to_use = 0; // jpeg_mem_init(cinfo) return is 0
	
	/* Attempt to allocate memory manager's control block */
	mem = (my_mem_ptr) jpeg_get_small(cinfo, SIZEOF(my_memory_mgr));
	
	if (mem == NULL) {
//		jpeg_mem_term(cinfo);	/* system-dependent cleanup */
		ERREXIT1(cinfo, JERR_OUT_OF_MEMORY, 0);
	}
	
	/* OK, fill in the method pointers */

	mem->pub.alloc_small = alloc_small;
	mem->pub.alloc_large = alloc_large;
	mem->pub.alloc_sarray = alloc_sarray;
	mem->pub.alloc_barray = NULL;	//alloc_barray;
	mem->pub.request_virt_sarray = NULL;	//request_virt_sarray;
	mem->pub.request_virt_barray = NULL;	//request_virt_barray;
	mem->pub.realize_virt_arrays = realize_virt_arrays;
	mem->pub.access_virt_sarray = NULL;	//access_virt_sarray;
	mem->pub.access_virt_barray = NULL;	//access_virt_barray;
	mem->pub.free_pool = free_pool;
	mem->pub.self_destruct = self_destruct;
	
	/* Initialize working state */
	mem->pub.max_memory_to_use = max_to_use;
	
	for (pool = JPOOL_NUMPOOLS-1; pool >= JPOOL_PERMANENT; pool--) {
		mem->small_list[pool] = NULL;
		mem->large_list[pool] = NULL;
	}
	mem->virt_sarray_list = NULL;
	mem->virt_barray_list = NULL;
	
	mem->total_space_allocated = SIZEOF(my_memory_mgr);
	
	/* Declare ourselves open for business */
	cinfo->mem = & mem->pub;
	
	/* Check for an environment variable JPEGMEM; if found, override the
	* default max_memory setting from jpeg_mem_init.  Note that the
	* surrounding application may again override this value.
	* If your system doesn't support getenv(), define NO_GETENV to disable
	* this feature.
	*/
#ifndef NO_GETENV
	{ char * memenv;
	
    if ((memenv = getenv("JPEGMEM")) != NULL) {
		char ch = 'x';
		
		if (sscanf(memenv, "%ld%c", &max_to_use, &ch) > 0) {
			if (ch == 'm' || ch == 'M')
				max_to_use *= 1000L;
			mem->pub.max_memory_to_use = max_to_use * 1000L;
		}
    }
	}
#endif
	
}

GLOBAL(void)
jpeg_CreateCompress (j_compress_ptr cinfo, int version, size_t structsize)
{
	int i;
	
	/* Guard against version mismatches between library and caller. */
	cinfo->mem = NULL;		/* so jpeg_destroy knows mem mgr not called */
#ifdef _DEBUG
	if (version != JPEG_LIB_VERSION)
	{
		assert(false);
		// JPG压缩版本不对
//		ERREXIT2(cinfo, JERR_BAD_LIB_VERSION, JPEG_LIB_VERSION, version);
	}
	if (structsize != SIZEOF(struct jpeg_compress_struct))
	{
		assert(false);
		// 结构长度不对
		ERREXIT2(cinfo, JERR_BAD_STRUCT_SIZE, 
		(int) SIZEOF(struct jpeg_compress_struct), (int) structsize);
	}
#endif
		/* For debugging purposes, zero the whole master structure.
		* But error manager pointer is already there, so save and restore it.
	*/
	{
		struct jpeg_error_mgr * err = cinfo->err;
		MEMZERO(cinfo, SIZEOF(struct jpeg_compress_struct));
		cinfo->err = err;
	}
	cinfo->is_decompressor = FALSE;
	
	/* Initialize a memory manager instance for this object */
	jinit_memory_mgr((j_common_ptr) cinfo);
	
	/* Zero out pointers to permanent structures. */
	cinfo->progress = NULL;
	cinfo->dest = NULL;
	
	cinfo->comp_info = NULL;
	
	for (i = 0; i < NUM_QUANT_TBLS; i++)
		cinfo->quant_tbl_ptrs[i] = NULL;
	
	for (i = 0; i < NUM_HUFF_TBLS; i++) {
		cinfo->dc_huff_tbl_ptrs[i] = NULL;
		cinfo->ac_huff_tbl_ptrs[i] = NULL;
	}
	
	cinfo->input_gamma = 1.0;	/* in case application forgets */
	
	/* OK, I'm ready */
	cinfo->global_state = CSTATE_START;
}

//METHODDEF(void *)
//alloc_small (j_common_ptr cinfo, int pool_id, size_t sizeofobject)
///* Allocate a "small" object */
//{
//	my_mem_ptr mem = (my_mem_ptr) cinfo->mem;
//	small_pool_ptr hdr_ptr, prev_hdr_ptr;
//	char * data_ptr;
//	size_t odd_bytes, min_request, slop;
//	
//	/* Check for unsatisfiable request (do now to ensure no overflow below) */
////	if (sizeofobject > (size_t) (MAX_ALLOC_CHUNK-SIZEOF(small_pool_hdr)))
////		out_of_memory(cinfo, 1);	/* request exceeds malloc's ability */
//	
//	/* Round up the requested size to a multiple of SIZEOF(ALIGN_TYPE) */
//	odd_bytes = sizeofobject % SIZEOF(ALIGN_TYPE);
//	if (odd_bytes > 0)
//		sizeofobject += SIZEOF(ALIGN_TYPE) - odd_bytes;
//	
//	/* See if space is available in any existing pool */
//	if (pool_id < 0 || pool_id >= JPOOL_NUMPOOLS)
//		ERREXIT1(cinfo, JERR_BAD_POOL_ID, pool_id);	/* safety check */
//	prev_hdr_ptr = NULL;
//	hdr_ptr = mem->small_list[pool_id];
//	while (hdr_ptr != NULL) {
//		if (hdr_ptr->hdr.bytes_left >= sizeofobject)
//			break;			/* found pool with enough space */
//		prev_hdr_ptr = hdr_ptr;
//		hdr_ptr = hdr_ptr->hdr.next;
//	}
//	
//	/* Time to make a new pool? */
//	if (hdr_ptr == NULL) {
//		/* min_request is what we need now, slop is what will be leftover */
//		min_request = sizeofobject + SIZEOF(small_pool_hdr);
//		if (prev_hdr_ptr == NULL)	/* first pool in class? */
//			slop = first_pool_slop[pool_id];
//		else
//			slop = extra_pool_slop[pool_id];
//		/* Don't ask for more than MAX_ALLOC_CHUNK */
//		if (slop > (size_t) (MAX_ALLOC_CHUNK-min_request))
//			slop = (size_t) (MAX_ALLOC_CHUNK-min_request);
//		/* Try to get space, if fail reduce slop and try again */
//		for (;;) {
//			hdr_ptr = (small_pool_ptr) jpeg_get_small(cinfo, min_request + slop);
//			if (hdr_ptr != NULL)
//				break;
//			slop /= 2;
////			if (slop < MIN_SLOP)	/* give up when it gets real small */
////				out_of_memory(cinfo, 2); /* jpeg_get_small failed */
//		}
//		mem->total_space_allocated += min_request + slop;
//		/* Success, initialize the new pool header and add to end of list */
//		hdr_ptr->hdr.next = NULL;
//		hdr_ptr->hdr.bytes_used = 0;
//		hdr_ptr->hdr.bytes_left = sizeofobject + slop;
//		if (prev_hdr_ptr == NULL)	/* first pool in class? */
//			mem->small_list[pool_id] = hdr_ptr;
//		else
//			prev_hdr_ptr->hdr.next = hdr_ptr;
//	}
//	
//	/* OK, allocate the object from the current pool */
//	data_ptr = (char *) (hdr_ptr + 1); /* point to first data byte in pool */
//	data_ptr += hdr_ptr->hdr.bytes_used; /* point to place for object */
//	hdr_ptr->hdr.bytes_used += sizeofobject;
//	hdr_ptr->hdr.bytes_left -= sizeofobject;
//	
//	return (void *) data_ptr;
//}

#define OUTPUT_BUF_SIZE  8000	/* choose an efficiently fwrite'able size */

METHODDEF(void)
init_destination (j_compress_ptr cinfo)
{
	my_dest_ptr dest = (my_dest_ptr) cinfo->dest;
	
	/* Allocate the output buffer --- it will be released when done with image */
//	dest->buffer = (JOCTET *)
//		(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
//		OUTPUT_BUF_SIZE * SIZEOF(JOCTET));
	
//	dest->pub.next_output_byte = dest->buffer;
	dest->buffer = (*DumpBuffFunction)(NULL, dest->pub.free_in_buffer, FALSE);
	dest->pub.next_output_byte = dest->buffer/* + 10*/;
//	dest->pub.free_in_buffer = OUTPUT_BUF_SIZE/* - 10*/;
	if (dest->buffer == NULL)
		longjmp(*pCurjmp, 1);
}

GLOBAL(void)
jpeg_stdio_dest (j_compress_ptr cinfo, FILE * outfile)
{
	my_dest_ptr dest;
	
	/* The destination object is made permanent so that multiple JPEG images
	* can be written to the same file without re-executing jpeg_stdio_dest.
	* This makes it dangerous to use this manager and a different destination
	* manager serially with the same JPEG object, because their private object
	* sizes may be different.  Caveat programmer.
	*/
	if (cinfo->dest == NULL) {	/* first time for this JPEG object? */
		cinfo->dest = (struct jpeg_destination_mgr *)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
			SIZEOF(my_destination_mgr));
	}
	
	dest = (my_dest_ptr) cinfo->dest;
	dest->pub.init_destination = init_destination;
	dest->pub.empty_output_buffer = empty_output_buffer;
	dest->pub.term_destination = term_destination;
	dest->outfile = outfile;
}

GLOBAL(void)
jpeg_set_quality (j_compress_ptr cinfo, int quality, bool force_baseline)
/* Set or change the 'quality' (quantization) setting, using default tables.
* This is the standard quality-adjusting entry point for typical user
* interfaces; only those who want detailed control over quantization tables
* would use the preceding three routines directly.
*/
{
	/* Convert user 0-100 rating to percentage scaling */
	quality = jpeg_quality_scaling(quality);
	
	/* Set up standard quality tables */
	jpeg_set_linear_quality(cinfo, quality, force_baseline);
}

GLOBAL(void)
jpeg_set_defaults (j_compress_ptr cinfo)
{
	int i;
	
	/* Safety check to ensure start_compress not called yet. */
	if (cinfo->global_state != CSTATE_START)
		ERREXIT1(cinfo, JERR_BAD_STATE, cinfo->global_state);
	
		/* Allocate comp_info array large enough for maximum component count.
		* Array is made permanent in case application wants to compress
		* multiple images at same param settings.
	*/
	if (cinfo->comp_info == NULL)
		cinfo->comp_info = (jpeg_component_info *)
		(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
		MAX_COMPONENTS * SIZEOF(jpeg_component_info));
	
	/* Initialize everything not dependent on the color space */
	
	cinfo->data_precision = BITS_IN_JSAMPLE;
	/* Set up two quantization tables using default quality of 75 */
	jpeg_set_quality(cinfo, 75, TRUE);
	/* Set up two Huffman tables */
	std_huff_tables(cinfo);
	
	/* Initialize default arithmetic coding conditioning */
	for (i = 0; i < NUM_ARITH_TBLS; i++) {
		cinfo->arith_dc_L[i] = 0;
		cinfo->arith_dc_U[i] = 1;
		cinfo->arith_ac_K[i] = 5;
	}
	
	/* Default is no multiple-scan output */
	cinfo->scan_info = NULL;
	cinfo->num_scans = 0;
	
	/* Expect normal source image, not raw downsampled data */
	cinfo->raw_data_in = FALSE;
	
	/* Use Huffman coding, not arithmetic coding, by default */
	cinfo->arith_code = FALSE;
	
	/* By default, don't do extra passes to optimize entropy coding */
	cinfo->optimize_coding = FALSE;
	/* The standard Huffman tables are only valid for 8-bit data precision.
	* If the precision is higher, force optimization on so that usable
	* tables will be computed.  This test can be removed if default tables
	* are supplied that are valid for the desired precision.
	*/
	if (cinfo->data_precision > 8)
		cinfo->optimize_coding = TRUE;
	
	/* By default, use the simpler non-cosited sampling alignment */
	cinfo->CCIR601_sampling = FALSE;
	
	/* No input smoothing */
	cinfo->smoothing_factor = 0;
	
	/* DCT algorithm preference */
	cinfo->dct_method = JDCT_DEFAULT;
	
	/* No restart markers */
	cinfo->restart_interval = 0;
	cinfo->restart_in_rows = 0;
	
	/* Fill in default JFIF marker parameters.  Note that whether the marker
	* will actually be written is determined by jpeg_set_colorspace.
	*/
	cinfo->density_unit = 0;	/* Pixel size is unknown by default */
	cinfo->X_density = 1;		/* Pixel aspect ratio is square by default */
	cinfo->Y_density = 1;
	
	/* Choose JPEG colorspace based on input space, set defaults accordingly */
	
	jpeg_default_colorspace(cinfo);
}

GLOBAL(int)
jpeg_quality_scaling (int quality)
/* Convert a user-specified quality rating to a percentage scaling factor
* for an underlying quantization table, using our recommended scaling curve.
* The input 'quality' factor should be 0 (terrible) to 100 (very good).
*/
{
	/* Safety limit on quality factor.  Convert 0 to 1 to avoid zero divide. */
	if (quality <= 0) quality = 1;
	if (quality > 100) quality = 100;
	
	/* The basic table is used as-is (scaling 100) for a quality of 50.
	* Qualities 50..100 are converted to scaling percentage 200 - 2*Q;
	* note that at Q=100 the scaling is 0, which will cause jpeg_add_quant_table
	* to make all the table entries 1 (hence, minimum quantization loss).
	* Qualities 1..50 are converted to scaling percentage 5000/Q.
	*/
	if (quality < 50)
		quality = 5000 / quality;
	else
		quality = 200 - quality*2;
	
	return quality;
}

GLOBAL(void)
jpeg_set_colorspace (j_compress_ptr cinfo, J_COLOR_SPACE colorspace)
{
	jpeg_component_info * compptr;
	int ci;
	
#define SET_COMP(index,id,hsamp,vsamp,quant,dctbl,actbl)  \
	(compptr = &cinfo->comp_info[index], \
	compptr->component_id = (id), \
	compptr->h_samp_factor = (hsamp), \
	compptr->v_samp_factor = (vsamp), \
	compptr->quant_tbl_no = (quant), \
	compptr->dc_tbl_no = (dctbl), \
	compptr->ac_tbl_no = (actbl) )
	
	/* Safety check to ensure start_compress not called yet. */
	if (cinfo->global_state != CSTATE_START)
		ERREXIT1(cinfo, JERR_BAD_STATE, cinfo->global_state);
	
		/* For all colorspaces, we use Q and Huff tables 0 for luminance components,
		* tables 1 for chrominance components.
	*/
	
	cinfo->jpeg_color_space = colorspace;
	
	cinfo->write_JFIF_header = FALSE; /* No marker for non-JFIF colorspaces */
	cinfo->write_Adobe_marker = FALSE; /* write no Adobe marker by default */
	
	switch (colorspace) {
	case JCS_GRAYSCALE:
		cinfo->write_JFIF_header = TRUE; /* Write a JFIF marker */
		cinfo->num_components = 1;
		/* JFIF specifies component ID 1 */
		SET_COMP(0, 1, 1,1, 0, 0,0);
		break;
	case JCS_RGB:
		cinfo->write_Adobe_marker = TRUE; /* write Adobe marker to flag RGB */
		cinfo->num_components = 3;
		SET_COMP(0, 0x52 /* 'R' */, 1,1, 0, 0,0);
		SET_COMP(1, 0x47 /* 'G' */, 1,1, 0, 0,0);
		SET_COMP(2, 0x42 /* 'B' */, 1,1, 0, 0,0);
		break;
	case JCS_YCbCr:
		cinfo->write_JFIF_header = TRUE; /* Write a JFIF marker */
		cinfo->num_components = 3;
		/* JFIF specifies component IDs 1,2,3 */
		/* We default to 2x2 subsamples of chrominance */
		SET_COMP(0, 1, 2,2, 0, 0,0);
		SET_COMP(1, 2, 1,1, 1, 1,1);
		SET_COMP(2, 3, 1,1, 1, 1,1);
		break;
	case JCS_CMYK:
		cinfo->write_Adobe_marker = TRUE; /* write Adobe marker to flag CMYK */
		cinfo->num_components = 4;
		SET_COMP(0, 0x43 /* 'C' */, 1,1, 0, 0,0);
		SET_COMP(1, 0x4D /* 'M' */, 1,1, 0, 0,0);
		SET_COMP(2, 0x59 /* 'Y' */, 1,1, 0, 0,0);
		SET_COMP(3, 0x4B /* 'K' */, 1,1, 0, 0,0);
		break;
	case JCS_YCCK:
		cinfo->write_Adobe_marker = TRUE; /* write Adobe marker to flag YCCK */
		cinfo->num_components = 4;
		SET_COMP(0, 1, 2,2, 0, 0,0);
		SET_COMP(1, 2, 1,1, 1, 1,1);
		SET_COMP(2, 3, 1,1, 1, 1,1);
		SET_COMP(3, 4, 2,2, 0, 0,0);
		break;
	case JCS_UNKNOWN:
		cinfo->num_components = cinfo->input_components;
		if (cinfo->num_components < 1 || cinfo->num_components > MAX_COMPONENTS)
			ERREXIT2(cinfo, JERR_COMPONENT_COUNT, cinfo->num_components,
			MAX_COMPONENTS);
		for (ci = 0; ci < cinfo->num_components; ci++) {
			SET_COMP(ci, ci, 1,1, 0, 0,0);
		}
		break;
	default:
		ERREXIT(cinfo, JERR_BAD_J_COLORSPACE);
	}
}


GLOBAL(void)
jpeg_default_colorspace (j_compress_ptr cinfo)
{
	switch (cinfo->in_color_space) {
	case JCS_GRAYSCALE:
		jpeg_set_colorspace(cinfo, JCS_GRAYSCALE);
		break;
	case JCS_RGB:
		jpeg_set_colorspace(cinfo, JCS_YCbCr);
		break;
	case JCS_YCbCr:
		jpeg_set_colorspace(cinfo, JCS_YCbCr);
		break;
	case JCS_CMYK:
		jpeg_set_colorspace(cinfo, JCS_CMYK); /* By default, no translation */
		break;
	case JCS_YCCK:
		jpeg_set_colorspace(cinfo, JCS_YCCK);
		break;
	case JCS_UNKNOWN:
		jpeg_set_colorspace(cinfo, JCS_UNKNOWN);
		break;
	default:
		ERREXIT(cinfo, JERR_BAD_IN_COLORSPACE);
	}
}

GLOBAL(void)
jpeg_suppress_tables (j_compress_ptr cinfo, bool suppress)
{
	int i;
	JQUANT_TBL * qtbl;
	JHUFF_TBL * htbl;
	
	for (i = 0; i < NUM_QUANT_TBLS; i++) {
		if ((qtbl = cinfo->quant_tbl_ptrs[i]) != NULL)
			qtbl->sent_table = suppress;
	}
	
	for (i = 0; i < NUM_HUFF_TBLS; i++) {
		if ((htbl = cinfo->dc_huff_tbl_ptrs[i]) != NULL)
			htbl->sent_table = suppress;
		if ((htbl = cinfo->ac_huff_tbl_ptrs[i]) != NULL)
			htbl->sent_table = suppress;
	}
}

LOCAL(void)
initial_setup (j_compress_ptr cinfo)
/* Do computations that are needed before master selection phase */
{
	int ci;
	jpeg_component_info *compptr;
	long samplesperrow;
	JDIMENSION jd_samplesperrow;
	
	/* Sanity check on image dimensions */
	if (cinfo->image_height <= 0 || cinfo->image_width <= 0
		|| cinfo->num_components <= 0 || cinfo->input_components <= 0)
		ERREXIT(cinfo, JERR_EMPTY_IMAGE);
	
	/* Make sure image isn't bigger than I can handle */
	if ((long) cinfo->image_height > (long) JPEG_MAX_DIMENSION ||
		(long) cinfo->image_width > (long) JPEG_MAX_DIMENSION)
		ERREXIT1(cinfo, JERR_IMAGE_TOO_BIG, (unsigned int) JPEG_MAX_DIMENSION);
	
	/* Width of an input scanline must be representable as JDIMENSION. */
	samplesperrow = (long) cinfo->image_width * (long) cinfo->input_components;
	jd_samplesperrow = (JDIMENSION) samplesperrow;
	if ((long) jd_samplesperrow != samplesperrow)
		ERREXIT(cinfo, JERR_WIDTH_OVERFLOW);
	
	/* For now, precision must match compiled-in value... */
	if (cinfo->data_precision != BITS_IN_JSAMPLE)
		ERREXIT1(cinfo, JERR_BAD_PRECISION, cinfo->data_precision);
	
	/* Check that number of components won't exceed internal array sizes */
	if (cinfo->num_components > MAX_COMPONENTS)
		ERREXIT2(cinfo, JERR_COMPONENT_COUNT, cinfo->num_components,
		MAX_COMPONENTS);
	
	/* Compute maximum sampling factors; check factor validity */
	cinfo->max_h_samp_factor = 1;
	cinfo->max_v_samp_factor = 1;
	for (ci = 0, compptr = cinfo->comp_info; ci < cinfo->num_components;
	ci++, compptr++) {
		if (compptr->h_samp_factor<=0 || compptr->h_samp_factor>MAX_SAMP_FACTOR ||
			compptr->v_samp_factor<=0 || compptr->v_samp_factor>MAX_SAMP_FACTOR)
			ERREXIT(cinfo, JERR_BAD_SAMPLING);
		cinfo->max_h_samp_factor = MAX(cinfo->max_h_samp_factor,
			compptr->h_samp_factor);
		cinfo->max_v_samp_factor = MAX(cinfo->max_v_samp_factor,
			compptr->v_samp_factor);
	}
	
	/* Compute dimensions of components */
	for (ci = 0, compptr = cinfo->comp_info; ci < cinfo->num_components;
	ci++, compptr++) {
		/* Fill in the correct component_index value; don't rely on application */
		compptr->component_index = ci;
		/* For compression, we never do DCT scaling. */
		compptr->DCT_scaled_size = DCTSIZE;
		/* Size in DCT blocks */
		compptr->width_in_blocks = (JDIMENSION)
			jdiv_round_up((long) cinfo->image_width * (long) compptr->h_samp_factor,
			(long) (cinfo->max_h_samp_factor * DCTSIZE));
		compptr->height_in_blocks = (JDIMENSION)
			jdiv_round_up((long) cinfo->image_height * (long) compptr->v_samp_factor,
			(long) (cinfo->max_v_samp_factor * DCTSIZE));
		/* Size in samples */
		compptr->downsampled_width = (JDIMENSION)
			jdiv_round_up((long) cinfo->image_width * (long) compptr->h_samp_factor,
			(long) cinfo->max_h_samp_factor);
		compptr->downsampled_height = (JDIMENSION)
			jdiv_round_up((long) cinfo->image_height * (long) compptr->v_samp_factor,
			(long) cinfo->max_v_samp_factor);
		/* Mark component needed (this flag isn't actually used for compression) */
		compptr->component_needed = TRUE;
	}
	
	/* Compute number of fully interleaved MCU rows (number of times that
	* main controller will call coefficient controller).
	*/
	cinfo->total_iMCU_rows = (JDIMENSION)
		jdiv_round_up((long) cinfo->image_height,
		(long) (cinfo->max_v_samp_factor*DCTSIZE));
}


GLOBAL(void)
jinit_c_master_control (j_compress_ptr cinfo, bool transcode_only)
{
	my_master_ptr master;
	
	master = (my_master_ptr)
		(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
		SIZEOF(my_comp_master));
	cinfo->master = (struct jpeg_comp_master *) master;
	master->pub.prepare_for_pass = prepare_for_pass;
	master->pub.pass_startup = pass_startup;
	master->pub.finish_pass = finish_pass_master;
	master->pub.is_last_pass = FALSE;
	
	/* Validate parameters, determine derived values */
	initial_setup(cinfo);
	
	if (cinfo->scan_info != NULL) {
#ifdef C_MULTISCAN_FILES_SUPPORTED
		validate_script(cinfo);
#else
		ERREXIT(cinfo, JERR_NOT_COMPILED);
#endif
	} else {
		cinfo->progressive_mode = FALSE;
		cinfo->num_scans = 1;
	}
	
	if (cinfo->progressive_mode)	/*  TEMPORARY HACK ??? */
		cinfo->optimize_coding = TRUE; /* assume default tables no good for progressive mode */
	
	/* Initialize my private state */
	if (transcode_only) {
		/* no main pass in transcoding */
		if (cinfo->optimize_coding)
			master->pass_type = huff_opt_pass;
		else
			master->pass_type = output_pass;
	} else {
		/* for normal compression, first pass is always this type: */
		master->pass_type = main_pass;
	}
	master->scan_number = 0;
	master->pass_number = 0;
	if (cinfo->optimize_coding)
		master->total_passes = cinfo->num_scans * 2;
	else
		master->total_passes = cinfo->num_scans;
}

LOCAL(void)
emit_jfif_app0 (j_compress_ptr cinfo)
/* Emit a JFIF-compliant APP0 marker */
{
/*
* Length of APP0 block	(2 bytes)
* Block ID			(4 bytes - ASCII "JFIF")
* Zero byte			(1 byte to terminate the ID string)
* Version Major, Minor	(2 bytes - 0x01, 0x01)
* Units			(1 byte - 0x00 = none, 0x01 = inch, 0x02 = cm)
* Xdpu			(2 bytes - dots per unit horizontal)
* Ydpu			(2 bytes - dots per unit vertical)
* Thumbnail X size		(1 byte)
* Thumbnail Y size		(1 byte)
	*/
	
	emit_marker(cinfo, M_APP0);
	
	emit_2bytes(cinfo, 2 + 4 + 1 + 2 + 1 + 2 + 2 + 1 + 1); /* length */
	
	emit_byte(cinfo, 0x4A);	/* Identifier: ASCII "JFIF" */
	emit_byte(cinfo, 0x46);
	emit_byte(cinfo, 0x49);
	emit_byte(cinfo, 0x46);
	emit_byte(cinfo, 0);
	/* We currently emit version code 1.01 since we use no 1.02 features.
	* This may avoid complaints from some older decoders.
	*/
	emit_byte(cinfo, 1);		/* Major version */
	emit_byte(cinfo, 1);		/* Minor version */
	emit_byte(cinfo, cinfo->density_unit); /* Pixel size information */
	emit_2bytes(cinfo, (int) cinfo->X_density);
	emit_2bytes(cinfo, (int) cinfo->Y_density);
	emit_byte(cinfo, 0);		/* No thumbnail image */
	emit_byte(cinfo, 0);
}

METHODDEF(void)
write_file_header (j_compress_ptr cinfo)
{
	emit_marker(cinfo, M_SOI);	/* first the SOI */
	
	if (cinfo->write_JFIF_header)	/* next an optional JFIF APP0 */
		emit_jfif_app0(cinfo);
//	if (cinfo->write_Adobe_marker) /* next an optional Adobe APP14 */
//		emit_adobe_app14(cinfo);
}

GLOBAL(void)
jinit_compress_master (j_compress_ptr cinfo)
{
	/* Initialize master control (includes parameter checking/processing) */
	jinit_c_master_control(cinfo, FALSE /* full compression */);
	//
	//  /* Preprocessing */
	if (! cinfo->raw_data_in) {
		jinit_color_converter(cinfo);
		jinit_downsampler(cinfo);
		jinit_c_prep_controller(cinfo, FALSE /* never need full buffer here */);
	}
	/* Forward DCT */
	
	jinit_forward_dct(cinfo);
	
	/* Entropy encoding: either Huffman or arithmetic coding. */
	if (cinfo->arith_code) {
		ERREXIT(cinfo, JERR_ARITH_NOTIMPL);
	} else {
		if (cinfo->progressive_mode) {
#ifdef C_PROGRESSIVE_SUPPORTED
			jinit_phuff_encoder(cinfo);
#else
			ERREXIT(cinfo, JERR_NOT_COMPILED);
#endif
		} else
			jinit_huff_encoder(cinfo);
	}
	
	/* Need a full-image coefficient buffer in any multi-pass mode. */
	jinit_c_coef_controller(cinfo,
		(cinfo->num_scans > 1 || cinfo->optimize_coding));
	jinit_c_main_controller(cinfo, FALSE /* never need full buffer here */);
	
	jinit_marker_writer(cinfo);
	
	/* We can now tell the memory manager to allocate virtual arrays. */
	(*cinfo->mem->realize_virt_arrays) ((j_common_ptr) cinfo);
	
	/* Write the datastream header (SOI) immediately.
	* Frame and scan headers are postponed till later.
	* This lets application insert special markers after the SOI.
	*/
	(*cinfo->marker->write_file_header) (cinfo);
}

LOCAL(void)
select_scan_parameters (j_compress_ptr cinfo)
/* Set up the scan parameters for the current scan */
{
  int ci;

#ifdef C_MULTISCAN_FILES_SUPPORTED
  if (cinfo->scan_info != NULL) {
    /* Prepare for current scan --- the script is already validated */
    my_master_ptr master = (my_master_ptr) cinfo->master;
    const jpeg_scan_info * scanptr = cinfo->scan_info + master->scan_number;

    cinfo->comps_in_scan = scanptr->comps_in_scan;
    for (ci = 0; ci < scanptr->comps_in_scan; ci++) {
      cinfo->cur_comp_info[ci] =
	&cinfo->comp_info[scanptr->component_index[ci]];
    }
    cinfo->Ss = scanptr->Ss;
    cinfo->Se = scanptr->Se;
    cinfo->Ah = scanptr->Ah;
    cinfo->Al = scanptr->Al;
  }
  else
#endif
  {
    /* Prepare for single sequential-JPEG scan containing all components */
    if (cinfo->num_components > MAX_COMPS_IN_SCAN)
      ERREXIT2(cinfo, JERR_COMPONENT_COUNT, cinfo->num_components,
	       MAX_COMPS_IN_SCAN);
    cinfo->comps_in_scan = cinfo->num_components;
    for (ci = 0; ci < cinfo->num_components; ci++) {
      cinfo->cur_comp_info[ci] = &cinfo->comp_info[ci];
    }
    cinfo->Ss = 0;
    cinfo->Se = DCTSIZE2-1;
    cinfo->Ah = 0;
    cinfo->Al = 0;
  }
}


LOCAL(void)
per_scan_setup (j_compress_ptr cinfo)
/* Do computations that are needed before processing a JPEG scan */
/* cinfo->comps_in_scan and cinfo->cur_comp_info[] are already set */
{
  int ci, mcublks, tmp;
  jpeg_component_info *compptr;
  
  if (cinfo->comps_in_scan == 1) {
    
    /* Noninterleaved (single-component) scan */
    compptr = cinfo->cur_comp_info[0];
    
    /* Overall image size in MCUs */
    cinfo->MCUs_per_row = compptr->width_in_blocks;
    cinfo->MCU_rows_in_scan = compptr->height_in_blocks;
    
    /* For noninterleaved scan, always one block per MCU */
    compptr->MCU_width = 1;
    compptr->MCU_height = 1;
    compptr->MCU_blocks = 1;
    compptr->MCU_sample_width = DCTSIZE;
    compptr->last_col_width = 1;
    /* For noninterleaved scans, it is convenient to define last_row_height
     * as the number of block rows present in the last iMCU row.
     */
    tmp = (int) (compptr->height_in_blocks % compptr->v_samp_factor);
    if (tmp == 0) tmp = compptr->v_samp_factor;
    compptr->last_row_height = tmp;
    
    /* Prepare array describing MCU composition */
    cinfo->blocks_in_MCU = 1;
    cinfo->MCU_membership[0] = 0;
    
  } else {
    
    /* Interleaved (multi-component) scan */
    if (cinfo->comps_in_scan <= 0 || cinfo->comps_in_scan > MAX_COMPS_IN_SCAN)
      ERREXIT2(cinfo, JERR_COMPONENT_COUNT, cinfo->comps_in_scan,
	       MAX_COMPS_IN_SCAN);
    
    /* Overall image size in MCUs */
    cinfo->MCUs_per_row = (JDIMENSION)
      jdiv_round_up((long) cinfo->image_width,
		    (long) (cinfo->max_h_samp_factor*DCTSIZE));
    cinfo->MCU_rows_in_scan = (JDIMENSION)
      jdiv_round_up((long) cinfo->image_height,
		    (long) (cinfo->max_v_samp_factor*DCTSIZE));
    
    cinfo->blocks_in_MCU = 0;
    
    for (ci = 0; ci < cinfo->comps_in_scan; ci++) {
      compptr = cinfo->cur_comp_info[ci];
      /* Sampling factors give # of blocks of component in each MCU */
      compptr->MCU_width = compptr->h_samp_factor;
      compptr->MCU_height = compptr->v_samp_factor;
      compptr->MCU_blocks = compptr->MCU_width * compptr->MCU_height;
      compptr->MCU_sample_width = compptr->MCU_width * DCTSIZE;
      /* Figure number of non-dummy blocks in last MCU column & row */
      tmp = (int) (compptr->width_in_blocks % compptr->MCU_width);
      if (tmp == 0) tmp = compptr->MCU_width;
      compptr->last_col_width = tmp;
      tmp = (int) (compptr->height_in_blocks % compptr->MCU_height);
      if (tmp == 0) tmp = compptr->MCU_height;
      compptr->last_row_height = tmp;
      /* Prepare array describing MCU composition */
      mcublks = compptr->MCU_blocks;
      if (cinfo->blocks_in_MCU + mcublks > C_MAX_BLOCKS_IN_MCU)
	ERREXIT(cinfo, JERR_BAD_MCU_SIZE);
      while (mcublks-- > 0) {
	cinfo->MCU_membership[cinfo->blocks_in_MCU++] = ci;
      }
    }
    
  }

  /* Convert restart specified in rows to actual MCU count. */
  /* Note that count must fit in 16 bits, so we provide limiting. */
  if (cinfo->restart_in_rows > 0) {
    long nominal = (long) cinfo->restart_in_rows * (long) cinfo->MCUs_per_row;
    cinfo->restart_interval = (unsigned int) MIN(nominal, 65535L);
  }
}


METHODDEF(void)
prepare_for_pass (j_compress_ptr cinfo)
{
	my_master_ptr master = (my_master_ptr) cinfo->master;
	
	switch (master->pass_type) {
	case main_pass:
    /* Initial pass: will collect input data, and do either Huffman
	* optimization or data output for the first scan.
		*/
		select_scan_parameters(cinfo);
		per_scan_setup(cinfo);
		if (! cinfo->raw_data_in) {
			(*cinfo->cconvert->start_pass) (cinfo);
//			(*cinfo->downsample->start_pass) (cinfo);
			(*cinfo->prep->start_pass) (cinfo, JBUF_PASS_THRU);
		}
		(*cinfo->fdct->start_pass) (cinfo);
		(*cinfo->entropy->start_pass) (cinfo, cinfo->optimize_coding);
		(*cinfo->coef->start_pass) (cinfo,
			(master->total_passes > 1 ?
JBUF_SAVE_AND_PASS : JBUF_PASS_THRU));
		(*cinfo->main->start_pass) (cinfo, JBUF_PASS_THRU);
		if (cinfo->optimize_coding) {
			/* No immediate data output; postpone writing frame/scan headers */
			master->pub.call_pass_startup = FALSE;
		} else {
			/* Will write frame/scan headers at first jpeg_write_scanlines call */
			master->pub.call_pass_startup = TRUE;
		}
		break;
#ifdef ENTROPY_OPT_SUPPORTED
	case huff_opt_pass:
		/* Do Huffman optimization for a scan after the first one. */
		select_scan_parameters(cinfo);
		per_scan_setup(cinfo);
		if (cinfo->Ss != 0 || cinfo->Ah == 0 || cinfo->arith_code) {
			(*cinfo->entropy->start_pass) (cinfo, TRUE);
			(*cinfo->coef->start_pass) (cinfo, JBUF_CRANK_DEST);
			master->pub.call_pass_startup = FALSE;
			break;
		}
		/* Special case: Huffman DC refinement scans need no Huffman table
		* and therefore we can skip the optimization pass for them.
		*/
		master->pass_type = output_pass;
		master->pass_number++;
		/*FALLTHROUGH*/
#endif
	case output_pass:
		/* Do a data-output pass. */
		/* We need not repeat per-scan setup if prior optimization pass did it. */
		if (! cinfo->optimize_coding) {
			select_scan_parameters(cinfo);
			per_scan_setup(cinfo);
		}
		(*cinfo->entropy->start_pass) (cinfo, FALSE);
		(*cinfo->coef->start_pass) (cinfo, JBUF_CRANK_DEST);
		/* We emit frame/scan headers now */
		if (master->scan_number == 0)
			(*cinfo->marker->write_frame_header) (cinfo);
		(*cinfo->marker->write_scan_header) (cinfo);
		master->pub.call_pass_startup = FALSE;
		break;
	default:
		ERREXIT(cinfo, JERR_NOT_COMPILED);
	}
	
	master->pub.is_last_pass = (master->pass_number == master->total_passes-1);
	
	/* Set up progress monitor's pass info if present */
	if (cinfo->progress != NULL) {
		cinfo->progress->completed_passes = master->pass_number;
		cinfo->progress->total_passes = master->total_passes;
	}
}

GLOBAL(void)
jpeg_start_compress (j_compress_ptr cinfo, bool write_all_tables)
{
	if (cinfo->global_state != CSTATE_START)
		ERREXIT1(cinfo, JERR_BAD_STATE, cinfo->global_state);
	
	if (write_all_tables)
		jpeg_suppress_tables(cinfo, FALSE);	/* mark all tables to be written */
	
	/* (Re)initialize error mgr and destination modules */
	(*cinfo->err->reset_error_mgr) ((j_common_ptr) cinfo);
	(*cinfo->dest->init_destination) (cinfo);
	/* Perform master selection of active modules */
	jinit_compress_master(cinfo);
	/* Set up for the first pass */
	(*cinfo->master->prepare_for_pass) (cinfo);
	/* Ready for application to drive first pass through jpeg_write_scanlines
	* or jpeg_write_raw_data.
	*/
	cinfo->next_scanline = 0;
	cinfo->global_state = (cinfo->raw_data_in ? CSTATE_RAW_OK : CSTATE_SCANNING);
}

GLOBAL(void)
jinit_color_converter (j_compress_ptr cinfo)
{
  my_cconvert_ptr cconvert;

  cconvert = (my_cconvert_ptr)
    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				SIZEOF(my_color_converter));
  cinfo->cconvert = (struct jpeg_color_converter *) cconvert;
  /* set start_pass to null method until we find out differently */
  cconvert->pub.start_pass = NULL;	//null_method;

  /* Make sure input_components agrees with in_color_space */
  switch (cinfo->in_color_space) {
  case JCS_GRAYSCALE:
    if (cinfo->input_components != 1)
      ERREXIT(cinfo, JERR_BAD_IN_COLORSPACE);
    break;

  case JCS_RGB:
#if RGB_PIXELSIZE != 3
    if (cinfo->input_components != RGB_PIXELSIZE)
      ERREXIT(cinfo, JERR_BAD_IN_COLORSPACE);
    break;
#endif /* else share code with YCbCr */

  case JCS_YCbCr:
    if (cinfo->input_components != 3)
      ERREXIT(cinfo, JERR_BAD_IN_COLORSPACE);
    break;

  case JCS_CMYK:
  case JCS_YCCK:
    if (cinfo->input_components != 4)
      ERREXIT(cinfo, JERR_BAD_IN_COLORSPACE);
    break;

  default:			/* JCS_UNKNOWN can be anything */
    if (cinfo->input_components < 1)
      ERREXIT(cinfo, JERR_BAD_IN_COLORSPACE);
    break;
  }

  /* Check num_components, set conversion method based on requested space */
  switch (cinfo->jpeg_color_space) {
  case JCS_GRAYSCALE:
    if (cinfo->num_components != 1)
      ERREXIT(cinfo, JERR_BAD_J_COLORSPACE);
    if (cinfo->in_color_space == JCS_GRAYSCALE)
      cconvert->pub.color_convert = NULL;	//grayscale_convert;
    else if (cinfo->in_color_space == JCS_RGB) {
      cconvert->pub.start_pass = rgb_ycc_start;
      cconvert->pub.color_convert = NULL;	//rgb_gray_convert;
    } else if (cinfo->in_color_space == JCS_YCbCr)
      cconvert->pub.color_convert = NULL;	//grayscale_convert;
    else
      ERREXIT(cinfo, JERR_CONVERSION_NOTIMPL);
    break;

  case JCS_RGB:
    if (cinfo->num_components != 3)
      ERREXIT(cinfo, JERR_BAD_J_COLORSPACE);
    if (cinfo->in_color_space == JCS_RGB && RGB_PIXELSIZE == 3)
      cconvert->pub.color_convert = NULL;	//null_convert;
    else
      ERREXIT(cinfo, JERR_CONVERSION_NOTIMPL);
    break;

  case JCS_YCbCr:
    if (cinfo->num_components != 3)
      ERREXIT(cinfo, JERR_BAD_J_COLORSPACE);
    if (cinfo->in_color_space == JCS_RGB) {
      cconvert->pub.start_pass = rgb_ycc_start;
      cconvert->pub.color_convert = rgb_ycc_convert;
    } else if (cinfo->in_color_space == JCS_YCbCr)
      cconvert->pub.color_convert = NULL;	//null_convert;
    else
      ERREXIT(cinfo, JERR_CONVERSION_NOTIMPL);
    break;

  case JCS_CMYK:
    if (cinfo->num_components != 4)
      ERREXIT(cinfo, JERR_BAD_J_COLORSPACE);
    if (cinfo->in_color_space == JCS_CMYK)
      cconvert->pub.color_convert = NULL;	//null_convert;
    else
      ERREXIT(cinfo, JERR_CONVERSION_NOTIMPL);
    break;

  case JCS_YCCK:
    if (cinfo->num_components != 4)
      ERREXIT(cinfo, JERR_BAD_J_COLORSPACE);
    if (cinfo->in_color_space == JCS_CMYK) {
      cconvert->pub.start_pass = rgb_ycc_start;
      cconvert->pub.color_convert = NULL;	//cmyk_ycck_convert;
    } else if (cinfo->in_color_space == JCS_YCCK)
      cconvert->pub.color_convert = NULL;	//null_convert;
    else
      ERREXIT(cinfo, JERR_CONVERSION_NOTIMPL);
    break;

  default:			/* allow null conversion of JCS_UNKNOWN */
    if (cinfo->jpeg_color_space != cinfo->in_color_space ||
	cinfo->num_components != cinfo->input_components)
      ERREXIT(cinfo, JERR_CONVERSION_NOTIMPL);
    cconvert->pub.color_convert = NULL;	//null_convert;
    break;
  }
}

GLOBAL(void)
jinit_downsampler (j_compress_ptr cinfo)
{
	my_downsample_ptr downsample;
	int ci;
	jpeg_component_info * compptr;
	bool smoothok = TRUE;
	
	downsample = (my_downsample_ptr)
		(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
		SIZEOF(my_downsampler));
	cinfo->downsample = (struct jpeg_downsampler *) downsample;
	downsample->pub.start_pass = NULL;	//start_pass_downsample;
	downsample->pub.downsample = sep_downsample;
	downsample->pub.need_context_rows = FALSE;
	
	if (cinfo->CCIR601_sampling)
		ERREXIT(cinfo, JERR_CCIR601_NOTIMPL);
	
	/* Verify we can handle the sampling factors, and set up method pointers */
	for (ci = 0, compptr = cinfo->comp_info; ci < cinfo->num_components;
	ci++, compptr++) {
		if (compptr->h_samp_factor == cinfo->max_h_samp_factor &&
			compptr->v_samp_factor == cinfo->max_v_samp_factor) {
#ifdef INPUT_SMOOTHING_SUPPORTED
			if (cinfo->smoothing_factor) {
				downsample->methods[ci] = fullsize_smooth_downsample;
				downsample->pub.need_context_rows = TRUE;
			} else
#endif
				downsample->methods[ci] = fullsize_downsample;
		} else if (compptr->h_samp_factor * 2 == cinfo->max_h_samp_factor &&
			compptr->v_samp_factor == cinfo->max_v_samp_factor) {
			smoothok = FALSE;
			downsample->methods[ci] = NULL;		//h2v1_downsample;
		} else if (compptr->h_samp_factor * 2 == cinfo->max_h_samp_factor &&
			compptr->v_samp_factor * 2 == cinfo->max_v_samp_factor) {
#ifdef INPUT_SMOOTHING_SUPPORTED
			if (cinfo->smoothing_factor) {
				downsample->methods[ci] = NULL;		//h2v2_smooth_downsample;
				downsample->pub.need_context_rows = TRUE;
			} else
#endif
				downsample->methods[ci] = h2v2_downsample;
		} else if ((cinfo->max_h_samp_factor % compptr->h_samp_factor) == 0 &&
			(cinfo->max_v_samp_factor % compptr->v_samp_factor) == 0) {
			smoothok = FALSE;
			downsample->methods[ci] = NULL;		//int_downsample;
		} else
			ERREXIT(cinfo, JERR_FRACT_SAMPLE_NOTIMPL);
	}
	
#ifdef INPUT_SMOOTHING_SUPPORTED
	if (cinfo->smoothing_factor && !smoothok)
		TRACEMS(cinfo, 0, JTRC_SMOOTH_NOTIMPL);
#endif
}

typedef struct {
	struct jpeg_c_prep_controller pub; /* public fields */
	
									   /* Downsampling input buffer.  This buffer holds color-converted data
									   * until we have enough to do a downsample step.
	*/
	JSAMPARRAY color_buf[MAX_COMPONENTS];
	
	JDIMENSION rows_to_go;	/* counts rows remaining in source image */
	int next_buf_row;		/* index of next row to store in color_buf */
	
#ifdef CONTEXT_ROWS_SUPPORTED	/* only needed for context case */
	int this_row_group;		/* starting row index of group to process */
	int next_buf_stop;		/* downsample when we reach this index */
#endif
} my_prep_controller;

typedef my_prep_controller * my_prep_ptr;

METHODDEF(void)
start_pass_prep (j_compress_ptr cinfo, J_BUF_MODE pass_mode)
{
	my_prep_ptr prep = (my_prep_ptr) cinfo->prep;
	
	if (pass_mode != JBUF_PASS_THRU)
		ERREXIT(cinfo, JERR_BAD_BUFFER_MODE);
	
	/* Initialize total-height counter for detecting bottom of image */
	prep->rows_to_go = cinfo->image_height;
	/* Mark the conversion buffer empty */
	prep->next_buf_row = 0;
#ifdef CONTEXT_ROWS_SUPPORTED
	/* Preset additional state variables for context mode.
	* These aren't used in non-context mode, so we needn't test which mode.
	*/
	prep->this_row_group = 0;
	/* Set next_buf_stop to stop after two row groups have been read in. */
	prep->next_buf_stop = 2 * cinfo->max_v_samp_factor;
#endif
}

GLOBAL(void)
jcopy_sample_rows (JSAMPARRAY input_array, int source_row,
				   JSAMPARRAY output_array, int dest_row,
				   int num_rows, JDIMENSION num_cols)
				   /* Copy some rows of samples from one place to another.
				   * num_rows rows are copied from input_array[source_row++]
				   * to output_array[dest_row++]; these areas may overlap for duplication.
				   * The source and destination arrays must be at least as wide as num_cols.
				   */
{
	register JSAMPROW inptr, outptr;
#ifdef FMEMCOPY
	register size_t count = (size_t) (num_cols * SIZEOF(JSAMPLE));
#else
	register JDIMENSION count;
#endif
	register int row;
	
	input_array += source_row;
	output_array += dest_row;
	
	for (row = num_rows; row > 0; row--) {
		inptr = *input_array++;
		outptr = *output_array++;
#ifdef FMEMCOPY
		FMEMCOPY(outptr, inptr, count);
#else
		for (count = num_cols; count > 0; count--)
			*outptr++ = *inptr++;	/* needn't bother with GETJSAMPLE() here */
#endif
	}
}

LOCAL(void)
expand_bottom_edge (JSAMPARRAY image_data, JDIMENSION num_cols,
					int input_rows, int output_rows)
{
	register int row;
	
	for (row = input_rows; row < output_rows; row++) {
		jcopy_sample_rows(image_data, input_rows-1, image_data, row,
			1, num_cols);
	}
}

METHODDEF(void)
pre_process_data (j_compress_ptr cinfo,
				  JSAMPARRAY input_buf, JDIMENSION *in_row_ctr,
				  JDIMENSION in_rows_avail,
				  JSAMPIMAGE output_buf, JDIMENSION *out_row_group_ctr,
				  JDIMENSION out_row_groups_avail)
{
	my_prep_ptr prep = (my_prep_ptr) cinfo->prep;
	int numrows, ci;
	JDIMENSION inrows;
	jpeg_component_info * compptr;
	
	while (*in_row_ctr < in_rows_avail &&
		*out_row_group_ctr < out_row_groups_avail) {
		/* Do color conversion to fill the conversion buffer. */
		inrows = in_rows_avail - *in_row_ctr;
		numrows = cinfo->max_v_samp_factor - prep->next_buf_row;
		numrows = (int) MIN((JDIMENSION) numrows, inrows);
		(*cinfo->cconvert->color_convert) (cinfo, input_buf + *in_row_ctr,
			prep->color_buf,
			(JDIMENSION) prep->next_buf_row,
			numrows);
		*in_row_ctr += numrows;
		prep->next_buf_row += numrows;
		prep->rows_to_go -= numrows;
		/* If at bottom of image, pad to fill the conversion buffer. */
		if (prep->rows_to_go == 0 &&
			prep->next_buf_row < cinfo->max_v_samp_factor) {
			for (ci = 0; ci < cinfo->num_components; ci++) {
				expand_bottom_edge(prep->color_buf[ci], cinfo->image_width,
					prep->next_buf_row, cinfo->max_v_samp_factor);
			}
			prep->next_buf_row = cinfo->max_v_samp_factor;
		}
		/* If we've filled the conversion buffer, empty it. */
		if (prep->next_buf_row == cinfo->max_v_samp_factor) {
			(*cinfo->downsample->downsample) (cinfo,
				prep->color_buf, (JDIMENSION) 0,
				output_buf, *out_row_group_ctr);
			prep->next_buf_row = 0;
			(*out_row_group_ctr)++;
		}
		/* If at bottom of image, pad the output to a full iMCU height.
		* Note we assume the caller is providing a one-iMCU-height output buffer!
		*/
		if (prep->rows_to_go == 0 &&
			*out_row_group_ctr < out_row_groups_avail) {
			for (ci = 0, compptr = cinfo->comp_info; ci < cinfo->num_components;
			ci++, compptr++) {
				expand_bottom_edge(output_buf[ci],
					compptr->width_in_blocks * DCTSIZE,
					(int) (*out_row_group_ctr * compptr->v_samp_factor),
					(int) (out_row_groups_avail * compptr->v_samp_factor));
			}
			*out_row_group_ctr = out_row_groups_avail;
			break;			/* can exit outer loop without test */
		}
	}
}


GLOBAL(void)
jinit_c_prep_controller (j_compress_ptr cinfo, bool need_full_buffer)
{
	my_prep_ptr prep;
	int ci;
	jpeg_component_info * compptr;
	
	if (need_full_buffer)		/* safety check */
		ERREXIT(cinfo, JERR_BAD_BUFFER_MODE);
	
	prep = (my_prep_ptr)
		(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
		SIZEOF(my_prep_controller));
	cinfo->prep = (struct jpeg_c_prep_controller *) prep;
	prep->pub.start_pass = start_pass_prep;
	
	/* Allocate the color conversion buffer.
	* We make the buffer wide enough to allow the downsampler to edge-expand
	* horizontally within the buffer, if it so chooses.
	*/
	if (cinfo->downsample->need_context_rows) {
		/* Set up to provide context rows */
#ifdef CONTEXT_ROWS_SUPPORTED
		prep->pub.pre_process_data = pre_process_context;
		create_context_buffer(cinfo);
#else
		ERREXIT(cinfo, JERR_NOT_COMPILED);
#endif
	} else {
		/* No context, just make it tall enough for one row group */
		prep->pub.pre_process_data = pre_process_data;
		for (ci = 0, compptr = cinfo->comp_info; ci < cinfo->num_components;
		ci++, compptr++) {
			prep->color_buf[ci] = (*cinfo->mem->alloc_sarray)
				((j_common_ptr) cinfo, JPOOL_IMAGE,
				(JDIMENSION) (((long) compptr->width_in_blocks * DCTSIZE *
				cinfo->max_h_samp_factor) / compptr->h_samp_factor),
				(JDIMENSION) cinfo->max_v_samp_factor);
		}
	}
}

typedef struct {
	struct jpeg_forward_dct pub;	/* public fields */
	
	/* Pointer to the DCT routine actually in use */
	forward_DCT_method_ptr do_dct;
	
	/* The actual post-DCT divisors --- not identical to the quant table
	* entries, because of scaling (especially for an unnormalized DCT).
	* Each table is given in normal array order.
	*/
	DCTELEM * divisors[NUM_QUANT_TBLS];
	
#ifdef DCT_FLOAT_SUPPORTED
	/* Same as above for the floating-point case. */
	float_DCT_method_ptr do_float_dct;
	FAST_FLOAT * float_divisors[NUM_QUANT_TBLS];
#endif
} my_fdct_controller;

typedef my_fdct_controller * my_fdct_ptr;


#define DCT_ISLOW_SUPPORTED	/* slow but accurate integer algorithm */
#define DCT_IFAST_SUPPORTED	/* faster, less accurate integer method */
//#define DCT_FLOAT_SUPPORTED	/* floating-point: accurate, fast on fast HW */

METHODDEF(void)
forward_DCT (j_compress_ptr cinfo, jpeg_component_info * compptr,
			 JSAMPARRAY sample_data, JBLOCKROW coef_blocks,
			 JDIMENSION start_row, JDIMENSION start_col,
			 JDIMENSION num_blocks)
			 /* This version is used for integer DCT implementations. */
{
	/* This routine is heavily used, so it's worth coding it tightly. */
	my_fdct_ptr fdct = (my_fdct_ptr) cinfo->fdct;
	forward_DCT_method_ptr do_dct = fdct->do_dct;
	DCTELEM * divisors = fdct->divisors[compptr->quant_tbl_no];
	DCTELEM workspace[DCTSIZE2];	/* work area for FDCT subroutine */
	JDIMENSION bi;
	
	sample_data += start_row;	/* fold in the vertical offset once */
	
	for (bi = 0; bi < num_blocks; bi++, start_col += DCTSIZE) {
		/* Load data into workspace, applying unsigned->signed conversion */
		{ register DCTELEM *workspaceptr;
		register JSAMPROW elemptr;
		register int elemr;
		
		workspaceptr = workspace;
		for (elemr = 0; elemr < DCTSIZE; elemr++) {
			elemptr = sample_data[elemr] + start_col;
#if DCTSIZE == 8		/* unroll the inner loop */
			*workspaceptr++ = GETJSAMPLE(*elemptr++) - CENTERJSAMPLE;
			*workspaceptr++ = GETJSAMPLE(*elemptr++) - CENTERJSAMPLE;
			*workspaceptr++ = GETJSAMPLE(*elemptr++) - CENTERJSAMPLE;
			*workspaceptr++ = GETJSAMPLE(*elemptr++) - CENTERJSAMPLE;
			*workspaceptr++ = GETJSAMPLE(*elemptr++) - CENTERJSAMPLE;
			*workspaceptr++ = GETJSAMPLE(*elemptr++) - CENTERJSAMPLE;
			*workspaceptr++ = GETJSAMPLE(*elemptr++) - CENTERJSAMPLE;
			*workspaceptr++ = GETJSAMPLE(*elemptr++) - CENTERJSAMPLE;
#else
			{ register int elemc;
			for (elemc = DCTSIZE; elemc > 0; elemc--) {
				*workspaceptr++ = GETJSAMPLE(*elemptr++) - CENTERJSAMPLE;
			}
			}
#endif
		}
		}
		
		/* Perform the DCT */
		(*do_dct) (workspace);
		
		/* Quantize/descale the coefficients, and store into coef_blocks[] */
		{ register DCTELEM temp, qval;
		register int i;
		register JCOEFPTR output_ptr = coef_blocks[bi];
		
		for (i = 0; i < DCTSIZE2; i++) {
			qval = divisors[i];
			temp = workspace[i];
			/* Divide the coefficient value by qval, ensuring proper rounding.
			* Since C does not specify the direction of rounding for negative
			* quotients, we have to force the dividend positive for portability.
			*
			* In most files, at least half of the output values will be zero
			* (at default quantization settings, more like three-quarters...)
			* so we should ensure that this case is fast.  On many machines,
			* a comparison is enough cheaper than a divide to make a special test
			* a win.  Since both inputs will be nonnegative, we need only test
			* for a < b to discover whether a/b is 0.
			* If your machine's division is fast enough, define FAST_DIVIDE.
			*/
#ifdef FAST_DIVIDE
#define DIVIDE_BY(a,b)	a /= b
#else
#define DIVIDE_BY(a,b)	if (a >= b) a /= b; else a = 0
#endif
			if (temp < 0) {
				temp = -temp;
				temp += qval>>1;	/* for rounding */
				DIVIDE_BY(temp, qval);
				temp = -temp;
			} else {
				temp += qval>>1;	/* for rounding */
				DIVIDE_BY(temp, qval);
			}
			output_ptr[i] = (JCOEF) temp;
		}
		}
	}
}

GLOBAL(void)
jinit_forward_dct (j_compress_ptr cinfo)
{
	my_fdct_ptr fdct;
	int i;
	
	fdct = (my_fdct_ptr)
		(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
		SIZEOF(my_fdct_controller));
	cinfo->fdct = (struct jpeg_forward_dct *) fdct;
	fdct->pub.start_pass = start_pass_fdctmgr;
	
	switch (cinfo->dct_method) {
#ifdef DCT_ISLOW_SUPPORTED
	case JDCT_ISLOW:
		fdct->pub.forward_DCT = forward_DCT;
		fdct->do_dct = jpeg_fdct_islow;
		break;
#endif
#ifdef DCT_IFAST_SUPPORTED
	case JDCT_IFAST:
		fdct->pub.forward_DCT = forward_DCT;
		fdct->do_dct = NULL;	//jpeg_fdct_ifast;
		break;
#endif
#ifdef DCT_FLOAT_SUPPORTED
	case JDCT_FLOAT:
		fdct->pub.forward_DCT = NULL;	//forward_DCT_float;
//		fdct->do_float_dct = jpeg_fdct_float;
		break;
#endif
	default:
		ERREXIT(cinfo, JERR_NOT_COMPILED);
		break;
	}
	
	/* Mark divisor tables unallocated */
	for (i = 0; i < NUM_QUANT_TBLS; i++) {
		fdct->divisors[i] = NULL;
#ifdef DCT_FLOAT_SUPPORTED
		fdct->float_divisors[i] = NULL;
#endif
	}
}

typedef struct {
	long put_buffer;		/* current bit-accumulation buffer */
	int put_bits;			/* # of bits now in it */
	int last_dc_val[MAX_COMPS_IN_SCAN]; /* last DC coef for each component */
} savable_state;

typedef struct {
	struct jpeg_entropy_encoder pub; /* public fields */
	
	savable_state saved;		/* Bit buffer & DC state at start of MCU */
	
	/* These fields are NOT loaded into local working state. */
	unsigned int restarts_to_go;	/* MCUs left in this restart interval */
	int next_restart_num;		/* next restart number to write (0-7) */
	
	/* Pointers to derived tables (these workspaces have image lifespan) */
	c_derived_tbl * dc_derived_tbls[NUM_HUFF_TBLS];
	c_derived_tbl * ac_derived_tbls[NUM_HUFF_TBLS];
	
#ifdef ENTROPY_OPT_SUPPORTED	/* Statistics tables for optimization */
	long * dc_count_ptrs[NUM_HUFF_TBLS];
	long * ac_count_ptrs[NUM_HUFF_TBLS];
#endif
} huff_entropy_encoder;

typedef huff_entropy_encoder * huff_entropy_ptr;


GLOBAL(void)
jinit_huff_encoder (j_compress_ptr cinfo)
{
	huff_entropy_ptr entropy;
	int i;
	
	entropy = (huff_entropy_ptr)
		(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
		SIZEOF(huff_entropy_encoder));
	cinfo->entropy = (struct jpeg_entropy_encoder *) entropy;
	entropy->pub.start_pass = start_pass_huff;
	
	/* Mark tables unallocated */
	for (i = 0; i < NUM_HUFF_TBLS; i++) {
		entropy->dc_derived_tbls[i] = entropy->ac_derived_tbls[i] = NULL;
#ifdef ENTROPY_OPT_SUPPORTED
		entropy->dc_count_ptrs[i] = entropy->ac_count_ptrs[i] = NULL;
#endif
	}
}

typedef struct {
	struct jpeg_c_coef_controller pub; /* public fields */
	
	JDIMENSION iMCU_row_num;	/* iMCU row # within image */
	JDIMENSION mcu_ctr;		/* counts MCUs processed in current row */
	int MCU_vert_offset;		/* counts MCU rows within iMCU row */
	int MCU_rows_per_iMCU_row;	/* number of such rows needed */
	
								/* For single-pass compression, it's sufficient to buffer just one MCU
								* (although this may prove a bit slow in practice).  We allocate a
								* workspace of C_MAX_BLOCKS_IN_MCU coefficient blocks, and reuse it for each
								* MCU constructed and sent.  (On 80x86, the workspace is FAR even though
								* it's not really very big; this is to keep the module interfaces unchanged
								* when a large coefficient buffer is necessary.)
								* In multi-pass modes, this array points to the current MCU's blocks
								* within the virtual arrays.
	*/
	JBLOCKROW MCU_buffer[C_MAX_BLOCKS_IN_MCU];
	
	/* In multi-pass modes, we need a virtual block array for each component. */
	jvirt_barray_ptr whole_image[MAX_COMPONENTS];
} my_coef_controller;

typedef my_coef_controller * my_coef_ptr;

GLOBAL(void)
jinit_c_coef_controller (j_compress_ptr cinfo, bool need_full_buffer)
{
	my_coef_ptr coef;
	
	coef = (my_coef_ptr)
		(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
		SIZEOF(my_coef_controller));
	cinfo->coef = (struct jpeg_c_coef_controller *) coef;
	coef->pub.start_pass = start_pass_coef;
	
	/* Create the coefficient buffer. */
	if (need_full_buffer) {
#ifdef FULL_COEF_BUFFER_SUPPORTED
		/* Allocate a full-image virtual array for each component, */
		/* padded to a multiple of samp_factor DCT blocks in each direction. */
		int ci;
		jpeg_component_info *compptr;
		
		for (ci = 0, compptr = cinfo->comp_info; ci < cinfo->num_components;
		ci++, compptr++) {
			coef->whole_image[ci] = (*cinfo->mem->request_virt_barray)
				((j_common_ptr) cinfo, JPOOL_IMAGE, FALSE,
				(JDIMENSION) jround_up((long) compptr->width_in_blocks,
				(long) compptr->h_samp_factor),
				(JDIMENSION) jround_up((long) compptr->height_in_blocks,
				(long) compptr->v_samp_factor),
				(JDIMENSION) compptr->v_samp_factor);
		}
#else
		ERREXIT(cinfo, JERR_BAD_BUFFER_MODE);
#endif
	} else {
		/* We only need a single-MCU buffer. */
		JBLOCKROW buffer;
		int i;
		
		buffer = (JBLOCKROW)
			(*cinfo->mem->alloc_large) ((j_common_ptr) cinfo, JPOOL_IMAGE,
			C_MAX_BLOCKS_IN_MCU * SIZEOF(JBLOCK));
		for (i = 0; i < C_MAX_BLOCKS_IN_MCU; i++) {
			coef->MCU_buffer[i] = buffer + i;
		}
		coef->whole_image[0] = NULL; /* flag for no virtual arrays */
	}
}

typedef struct {
	struct jpeg_c_main_controller pub; /* public fields */
	
	JDIMENSION cur_iMCU_row;	/* number of current iMCU row */
	JDIMENSION rowgroup_ctr;	/* counts row groups received in iMCU row */
	bool suspended;		/* remember if we suspended output */
	J_BUF_MODE pass_mode;		/* current operating mode */
	
								/* If using just a strip buffer, this points to the entire set of buffers
								* (we allocate one for each component).  In the full-image case, this
								* points to the currently accessible strips of the virtual arrays.
	*/
	JSAMPARRAY buffer[MAX_COMPONENTS];
	
#ifdef FULL_MAIN_BUFFER_SUPPORTED
	/* If using full-image storage, this array holds pointers to virtual-array
	* control blocks for each component.  Unused if not full-image storage.
	*/
	jvirt_sarray_ptr whole_image[MAX_COMPONENTS];
#endif
} my_main_controller;

typedef my_main_controller * my_main_ptr;


GLOBAL(void)
jinit_c_main_controller (j_compress_ptr cinfo, bool need_full_buffer)
{
	my_main_ptr main;
	int ci;
	jpeg_component_info *compptr;
	
	main = (my_main_ptr)
		(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
		SIZEOF(my_main_controller));
	cinfo->main = (struct jpeg_c_main_controller *) main;
	main->pub.start_pass = start_pass_main;
	
	/* We don't need to create a buffer in raw-data mode. */
	if (cinfo->raw_data_in)
		return;
	
		/* Create the buffer.  It holds downsampled data, so each component
		* may be of a different size.
	*/
	if (need_full_buffer) {
#ifdef FULL_MAIN_BUFFER_SUPPORTED
		/* Allocate a full-image virtual array for each component */
		/* Note we pad the bottom to a multiple of the iMCU height */
		for (ci = 0, compptr = cinfo->comp_info; ci < cinfo->num_components;
		ci++, compptr++) {
			main->whole_image[ci] = (*cinfo->mem->request_virt_sarray)
				((j_common_ptr) cinfo, JPOOL_IMAGE, FALSE,
				compptr->width_in_blocks * DCTSIZE,
				(JDIMENSION) jround_up((long) compptr->height_in_blocks,
				(long) compptr->v_samp_factor) * DCTSIZE,
				(JDIMENSION) (compptr->v_samp_factor * DCTSIZE));
		}
#else
		ERREXIT(cinfo, JERR_BAD_BUFFER_MODE);
#endif
	} else {
#ifdef FULL_MAIN_BUFFER_SUPPORTED
		main->whole_image[0] = NULL; /* flag for no virtual arrays */
#endif
		/* Allocate a strip buffer for each component */
		for (ci = 0, compptr = cinfo->comp_info; ci < cinfo->num_components;
		ci++, compptr++) {
			main->buffer[ci] = (*cinfo->mem->alloc_sarray)
				((j_common_ptr) cinfo, JPOOL_IMAGE,
				compptr->width_in_blocks * DCTSIZE,
				(JDIMENSION) (compptr->v_samp_factor * DCTSIZE));
		}
	}
}

GLOBAL(void)
jinit_marker_writer (j_compress_ptr cinfo)
{
	/* Create the subobject */
	cinfo->marker = (struct jpeg_marker_writer *)
		(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
		SIZEOF(struct jpeg_marker_writer));
	/* Initialize method pointers */
	cinfo->marker->write_any_marker = NULL;	//write_any_marker;
	cinfo->marker->write_file_header = write_file_header;
	cinfo->marker->write_frame_header = write_frame_header;
	cinfo->marker->write_scan_header = write_scan_header;
	cinfo->marker->write_file_trailer = write_file_trailer;
	cinfo->marker->write_tables_only = NULL;	//write_tables_only;
}

GLOBAL(JDIMENSION)
jpeg_write_scanlines (j_compress_ptr cinfo, JSAMPARRAY scanlines,
					  JDIMENSION num_lines)
{
	JDIMENSION row_ctr, rows_left;

	if (cinfo->global_state != CSTATE_SCANNING)
		ERREXIT1(cinfo, JERR_BAD_STATE, cinfo->global_state);
	if (cinfo->next_scanline >= cinfo->image_height)
		WARNMS(cinfo, JWRN_TOO_MUCH_DATA);
	/* Call progress monitor hook if present */
	if (cinfo->progress != NULL) {
		cinfo->progress->pass_counter = (long) cinfo->next_scanline;
		cinfo->progress->pass_limit = (long) cinfo->image_height;
		(*cinfo->progress->progress_monitor) ((j_common_ptr) cinfo);
	}
	/* Give master control module another chance if this is first call to
	* jpeg_write_scanlines.  This lets output of the frame/scan headers be
	* delayed so that application can write COM, etc, markers between
	* jpeg_start_compress and jpeg_write_scanlines.
	*/
	if (cinfo->master->call_pass_startup)
		(*cinfo->master->pass_startup) (cinfo);
	/* Ignore any extra scanlines at bottom of image. */
	rows_left = cinfo->image_height - cinfo->next_scanline;
	if (num_lines > rows_left)
		num_lines = rows_left;
	row_ctr = 0;
	(*cinfo->main->process_data) (cinfo, scanlines, &row_ctr, num_lines);
	cinfo->next_scanline += row_ctr;
	return row_ctr;
}

GLOBAL(void)
jpeg_finish_compress (j_compress_ptr cinfo)
{
	JDIMENSION iMCU_row;
	
	if (cinfo->global_state == CSTATE_SCANNING ||
		cinfo->global_state == CSTATE_RAW_OK) {
		/* Terminate first pass */
		if (cinfo->next_scanline < cinfo->image_height)
			ERREXIT(cinfo, JERR_TOO_LITTLE_DATA);
		(*cinfo->master->finish_pass) (cinfo);
	} else if (cinfo->global_state != CSTATE_WRCOEFS)
		ERREXIT1(cinfo, JERR_BAD_STATE, cinfo->global_state);
	/* Perform any remaining passes */
	while (! cinfo->master->is_last_pass) {
		(*cinfo->master->prepare_for_pass) (cinfo);
		for (iMCU_row = 0; iMCU_row < cinfo->total_iMCU_rows; iMCU_row++) {
			if (cinfo->progress != NULL) {
				cinfo->progress->pass_counter = (long) iMCU_row;
				cinfo->progress->pass_limit = (long) cinfo->total_iMCU_rows;
				(*cinfo->progress->progress_monitor) ((j_common_ptr) cinfo);
			}
			/* We bypass the main controller and invoke coef controller directly;
			* all work is being done from the coefficient buffer.
			*/
			if (! (*cinfo->coef->compress_data) (cinfo, (JSAMPIMAGE) NULL))
				ERREXIT(cinfo, JERR_CANT_SUSPEND);
		}
		(*cinfo->master->finish_pass) (cinfo);
	}
	/* Write EOI, do final cleanup */
	(*cinfo->marker->write_file_trailer) (cinfo);
	(*cinfo->dest->term_destination) (cinfo);
	/* We can use jpeg_abort to release memory and reset global_state */
	jpeg_abort((j_common_ptr) cinfo);
}

GLOBAL(void)
jpeg_abort (j_common_ptr cinfo)
{
	int pool;
	
	/* Releasing pools in reverse order might help avoid fragmentation
	* with some (brain-damaged) malloc libraries.
	*/
	for (pool = JPOOL_NUMPOOLS-1; pool > JPOOL_PERMANENT; pool--) {
		(*cinfo->mem->free_pool) (cinfo, pool);
	}
	
	/* Reset overall state for possible reuse of object */
	cinfo->global_state = (cinfo->is_decompressor ? DSTATE_START : CSTATE_START);
}

GLOBAL(void)
jpeg_destroy_compress (j_compress_ptr cinfo)
{
	jpeg_destroy((j_common_ptr) cinfo); /* use common routine */
}

GLOBAL(void)
jpeg_destroy (j_common_ptr cinfo)
{
	/* We need only tell the memory manager to release everything. */
	/* NB: mem pointer is NULL if memory mgr failed to initialize. */
	if (cinfo->mem != NULL)
		(*cinfo->mem->self_destruct) (cinfo);
	cinfo->mem = NULL;		/* be safe if jpeg_destroy is called twice */
	cinfo->global_state = 0;	/* mark it destroyed */
}

GLOBAL(struct jpeg_error_mgr *)
jpeg_std_error (struct jpeg_error_mgr * err)
{
	err->error_exit = NULL;	//error_exit;
	err->emit_message = NULL;	//emit_message;
	err->output_message = NULL;	//output_message;
	err->format_message = NULL;	//format_message;
	err->reset_error_mgr = reset_error_mgr;
	
	err->trace_level = 0;		/* default = no tracing */
	err->num_warnings = 0;	/* no warnings emitted yet */
	err->msg_code = 0;		/* may be useful as a flag for "no error" */
	
	/* Initialize message table pointers */
	err->jpeg_message_table = NULL;	//jpeg_std_message_table;
	err->last_jpeg_message = (int) JMSG_LASTMSGCODE - 1;
	
	err->addon_message_table = NULL;
	err->first_addon_message = 0;	/* for safety */
	err->last_addon_message = 0;
	
	return err;
}

#define SCALEBITS	16	/* speediest right-shift on some machines */
#define CBCR_OFFSET	((long) CENTERJSAMPLE << SCALEBITS)
#define ONE_HALF	((long) 1 << (SCALEBITS-1))
#define FIX(x)		((long) ((x) * (1L<<SCALEBITS) + 0.5))

#define R_Y_OFF		0			/* offset to R => Y section */
#define G_Y_OFF		(1*(MAXJSAMPLE+1))	/* offset to G => Y section */
#define B_Y_OFF		(2*(MAXJSAMPLE+1))	/* etc. */
#define R_CB_OFF	(3*(MAXJSAMPLE+1))
#define G_CB_OFF	(4*(MAXJSAMPLE+1))
#define B_CB_OFF	(5*(MAXJSAMPLE+1))
#define R_CR_OFF	B_CB_OFF		/* B=>Cb, R=>Cr are the same */
#define G_CR_OFF	(6*(MAXJSAMPLE+1))
#define B_CR_OFF	(7*(MAXJSAMPLE+1))
#define TABLE_SIZE	(8*(MAXJSAMPLE+1))

METHODDEF(void)
rgb_ycc_start (j_compress_ptr cinfo)
{
	my_cconvert_ptr cconvert = (my_cconvert_ptr) cinfo->cconvert;
	long * rgb_ycc_tab;
	long i;
	
	/* Allocate and fill in the conversion tables. */
	cconvert->rgb_ycc_tab = rgb_ycc_tab = (long *)
		(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
		(TABLE_SIZE * SIZEOF(long)));
	
	for (i = 0; i <= MAXJSAMPLE; i++) {
		rgb_ycc_tab[i+R_Y_OFF] = FIX(0.29900) * i;
		rgb_ycc_tab[i+G_Y_OFF] = FIX(0.58700) * i;
		rgb_ycc_tab[i+B_Y_OFF] = FIX(0.11400) * i     + ONE_HALF;
		rgb_ycc_tab[i+R_CB_OFF] = (-FIX(0.16874)) * i;
		rgb_ycc_tab[i+G_CB_OFF] = (-FIX(0.33126)) * i;
		/* We use a rounding fudge-factor of 0.5-epsilon for Cb and Cr.
		* This ensures that the maximum output will round to MAXJSAMPLE
		* not MAXJSAMPLE+1, and thus that we don't have to range-limit.
		*/
		rgb_ycc_tab[i+B_CB_OFF] = FIX(0.50000) * i    + CBCR_OFFSET + ONE_HALF-1;
		/*  B=>Cb and R=>Cr tables are the same
		rgb_ycc_tab[i+R_CR_OFF] = FIX(0.50000) * i    + CBCR_OFFSET + ONE_HALF-1;
		*/
		rgb_ycc_tab[i+G_CR_OFF] = (-FIX(0.41869)) * i;
		rgb_ycc_tab[i+B_CR_OFF] = (-FIX(0.08131)) * i;
	}
}

//METHODDEF(void)
//start_pass_prep (j_compress_ptr cinfo, J_BUF_MODE pass_mode)
//{
//	my_prep_ptr prep = (my_prep_ptr) cinfo->prep;
//	
//	if (pass_mode != JBUF_PASS_THRU)
//		ERREXIT(cinfo, JERR_BAD_BUFFER_MODE);
//	
//	/* Initialize total-height counter for detecting bottom of image */
//	prep->rows_to_go = cinfo->image_height;
//	/* Mark the conversion buffer empty */
//	prep->next_buf_row = 0;
//#ifdef CONTEXT_ROWS_SUPPORTED
//	/* Preset additional state variables for context mode.
//	* These aren't used in non-context mode, so we needn't test which mode.
//	*/
//	prep->this_row_group = 0;
//	/* Set next_buf_stop to stop after two row groups have been read in. */
//	prep->next_buf_stop = 2 * cinfo->max_v_samp_factor;
//#endif
//}

METHODDEF(void)
start_pass_fdctmgr (j_compress_ptr cinfo)
{
  my_fdct_ptr fdct = (my_fdct_ptr) cinfo->fdct;
  int ci, qtblno, i;
  jpeg_component_info *compptr;
  JQUANT_TBL * qtbl;
  DCTELEM * dtbl;

  for (ci = 0, compptr = cinfo->comp_info; ci < cinfo->num_components;
       ci++, compptr++) {
    qtblno = compptr->quant_tbl_no;
    /* Make sure specified quantization table is present */
    if (qtblno < 0 || qtblno >= NUM_QUANT_TBLS ||
	cinfo->quant_tbl_ptrs[qtblno] == NULL)
      ERREXIT1(cinfo, JERR_NO_QUANT_TABLE, qtblno);
    qtbl = cinfo->quant_tbl_ptrs[qtblno];
    /* Compute divisors for this quant table */
    /* We may do this more than once for same table, but it's not a big deal */
    switch (cinfo->dct_method) {
#ifdef DCT_ISLOW_SUPPORTED
    case JDCT_ISLOW:
      /* For LL&M IDCT method, divisors are equal to raw quantization
       * coefficients multiplied by 8 (to counteract scaling).
       */
      if (fdct->divisors[qtblno] == NULL) {
	fdct->divisors[qtblno] = (DCTELEM *)
	  (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				      DCTSIZE2 * SIZEOF(DCTELEM));
      }
      dtbl = fdct->divisors[qtblno];
      for (i = 0; i < DCTSIZE2; i++) {
	dtbl[i] = ((DCTELEM) qtbl->quantval[i]) << 3;
      }
      break;
#endif
#ifdef DCT_IFAST_SUPPORTED
    case JDCT_IFAST:
      {
	/* For AA&N IDCT method, divisors are equal to quantization
	 * coefficients scaled by scalefactor[row]*scalefactor[col], where
	 *   scalefactor[0] = 1
	 *   scalefactor[k] = cos(k*PI/16) * sqrt(2)    for k=1..7
	 * We apply a further scale factor of 8.
	 */
#define CONST_BITS 14
	static const INT16 aanscales[DCTSIZE2] = {
	  /* precomputed values scaled up by 14 bits */
	  16384, 22725, 21407, 19266, 16384, 12873,  8867,  4520,
	  22725, 31521, 29692, 26722, 22725, 17855, 12299,  6270,
	  21407, 29692, 27969, 25172, 21407, 16819, 11585,  5906,
	  19266, 26722, 25172, 22654, 19266, 15137, 10426,  5315,
	  16384, 22725, 21407, 19266, 16384, 12873,  8867,  4520,
	  12873, 17855, 16819, 15137, 12873, 10114,  6967,  3552,
	   8867, 12299, 11585, 10426,  8867,  6967,  4799,  2446,
	   4520,  6270,  5906,  5315,  4520,  3552,  2446,  1247
	};
	SHIFT_TEMPS

	if (fdct->divisors[qtblno] == NULL) {
	  fdct->divisors[qtblno] = (DCTELEM *)
	    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
					DCTSIZE2 * SIZEOF(DCTELEM));
	}
	dtbl = fdct->divisors[qtblno];
	for (i = 0; i < DCTSIZE2; i++) {
	  dtbl[i] = (DCTELEM)
	    DESCALE(MULTIPLY16V16((long) qtbl->quantval[i],
				  (long) aanscales[i]),
		    CONST_BITS-3);
	}
      }
      break;
#endif
#ifdef DCT_FLOAT_SUPPORTED
    case JDCT_FLOAT:
      {
	/* For float AA&N IDCT method, divisors are equal to quantization
	 * coefficients scaled by scalefactor[row]*scalefactor[col], where
	 *   scalefactor[0] = 1
	 *   scalefactor[k] = cos(k*PI/16) * sqrt(2)    for k=1..7
	 * We apply a further scale factor of 8.
	 * What's actually stored is 1/divisor so that the inner loop can
	 * use a multiplication rather than a division.
	 */
	FAST_FLOAT * fdtbl;
	int row, col;
	static const double aanscalefactor[DCTSIZE] = {
	  1.0, 1.387039845, 1.306562965, 1.175875602,
	  1.0, 0.785694958, 0.541196100, 0.275899379
	};

	if (fdct->float_divisors[qtblno] == NULL) {
	  fdct->float_divisors[qtblno] = (FAST_FLOAT *)
	    (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
					DCTSIZE2 * SIZEOF(FAST_FLOAT));
	}
	fdtbl = fdct->float_divisors[qtblno];
	i = 0;
	for (row = 0; row < DCTSIZE; row++) {
	  for (col = 0; col < DCTSIZE; col++) {
	    fdtbl[i] = (FAST_FLOAT)
	      (1.0 / (((double) qtbl->quantval[i] *
		       aanscalefactor[row] * aanscalefactor[col] * 8.0)));
	    i++;
	  }
	}
      }
      break;
#endif
    default:
      ERREXIT(cinfo, JERR_NOT_COMPILED);
      break;
    }
  }
}

METHODDEF(void)
start_pass_huff (j_compress_ptr cinfo, bool gather_statistics)
{
	huff_entropy_ptr entropy = (huff_entropy_ptr) cinfo->entropy;
	int ci, dctbl, actbl;
	jpeg_component_info * compptr;
	
	if (gather_statistics) {
#ifdef ENTROPY_OPT_SUPPORTED
		entropy->pub.encode_mcu = encode_mcu_gather;
		entropy->pub.finish_pass = finish_pass_gather;
#else
		ERREXIT(cinfo, JERR_NOT_COMPILED);
#endif
	} else {
		entropy->pub.encode_mcu = encode_mcu_huff;
		entropy->pub.finish_pass = finish_pass_huff;
	}
	
	for (ci = 0; ci < cinfo->comps_in_scan; ci++) {
		compptr = cinfo->cur_comp_info[ci];
		dctbl = compptr->dc_tbl_no;
		actbl = compptr->ac_tbl_no;
		/* Make sure requested tables are present */
		/* (In gather mode, tables need not be allocated yet) */
		if (dctbl < 0 || dctbl >= NUM_HUFF_TBLS ||
			(cinfo->dc_huff_tbl_ptrs[dctbl] == NULL && !gather_statistics))
			ERREXIT1(cinfo, JERR_NO_HUFF_TABLE, dctbl);
		if (actbl < 0 || actbl >= NUM_HUFF_TBLS ||
			(cinfo->ac_huff_tbl_ptrs[actbl] == NULL && !gather_statistics))
			ERREXIT1(cinfo, JERR_NO_HUFF_TABLE, actbl);
		if (gather_statistics) {
#ifdef ENTROPY_OPT_SUPPORTED
			/* Allocate and zero the statistics tables */
			/* Note that jpeg_gen_optimal_table expects 257 entries in each table! */
			if (entropy->dc_count_ptrs[dctbl] == NULL)
				entropy->dc_count_ptrs[dctbl] = (long *)
				(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				257 * SIZEOF(long));
			MEMZERO(entropy->dc_count_ptrs[dctbl], 257 * SIZEOF(long));
			if (entropy->ac_count_ptrs[actbl] == NULL)
				entropy->ac_count_ptrs[actbl] = (long *)
				(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				257 * SIZEOF(long));
			MEMZERO(entropy->ac_count_ptrs[actbl], 257 * SIZEOF(long));
#endif
		} else {
			/* Compute derived values for Huffman tables */
			/* We may do this more than once for a table, but it's not expensive */
			jpeg_make_c_derived_tbl(cinfo, cinfo->dc_huff_tbl_ptrs[dctbl],
				& entropy->dc_derived_tbls[dctbl]);
			jpeg_make_c_derived_tbl(cinfo, cinfo->ac_huff_tbl_ptrs[actbl],
				& entropy->ac_derived_tbls[actbl]);
		}
		/* Initialize DC predictions to 0 */
		entropy->saved.last_dc_val[ci] = 0;
	}
	
	/* Initialize bit buffer to empty */
	entropy->saved.put_buffer = 0;
	entropy->saved.put_bits = 0;
	
	/* Initialize restart stuff */
	entropy->restarts_to_go = cinfo->restart_interval;
	entropy->next_restart_num = 0;
}

typedef struct {
	JOCTET * next_output_byte;	/* => next byte to write in buffer */
	size_t free_in_buffer;	/* # of byte spaces remaining in buffer */
	savable_state cur;		/* Current bit buffer & DC state */
	j_compress_ptr cinfo;		/* dump_buffer needs access to this */
} working_state;

LOCAL(bool)emit_restart (working_state * state, int restart_num);

#ifndef NO_STRUCT_ASSIGN
#define ASSIGN_STATE(dest,src)  ((dest) = (src))
#else
#if MAX_COMPS_IN_SCAN == 4
#define ASSIGN_STATE(dest,src)  \
	((dest).put_buffer = (src).put_buffer, \
	(dest).put_bits = (src).put_bits, \
	(dest).last_dc_val[0] = (src).last_dc_val[0], \
	(dest).last_dc_val[1] = (src).last_dc_val[1], \
	(dest).last_dc_val[2] = (src).last_dc_val[2], \
(dest).last_dc_val[3] = (src).last_dc_val[3])
#endif
#endif

LOCAL(bool)
encode_one_block (working_state * state, JCOEFPTR block, int last_dc_val,
				  c_derived_tbl *dctbl, c_derived_tbl *actbl);

METHODDEF(bool)
encode_mcu_huff (j_compress_ptr cinfo, JBLOCKROW *MCU_data)
{
	huff_entropy_ptr entropy = (huff_entropy_ptr) cinfo->entropy;
	working_state state;
	int blkn, ci;
	jpeg_component_info * compptr;
	
	/* Load up working state */
	state.next_output_byte = cinfo->dest->next_output_byte;
	state.free_in_buffer = cinfo->dest->free_in_buffer;
	ASSIGN_STATE(state.cur, entropy->saved);
	state.cinfo = cinfo;
	
	/* Emit restart marker if needed */
	if (cinfo->restart_interval)
	{
//		if (entropy->restarts_to_go == 0)
//			if (! emit_restart(&state, entropy->next_restart_num))
//				return FALSE;
	}
	
	/* Encode the MCU data blocks */
	for (blkn = 0; blkn < cinfo->blocks_in_MCU; blkn++) {
		ci = cinfo->MCU_membership[blkn];
		compptr = cinfo->cur_comp_info[ci];
		if (! encode_one_block(&state,
			MCU_data[blkn][0], state.cur.last_dc_val[ci],
			entropy->dc_derived_tbls[compptr->dc_tbl_no],
			entropy->ac_derived_tbls[compptr->ac_tbl_no]))
			return FALSE;
		/* Update last_dc_val */
		state.cur.last_dc_val[ci] = MCU_data[blkn][0][0];
	}
	
	/* Completed MCU, so update state */
	cinfo->dest->next_output_byte = state.next_output_byte;
	cinfo->dest->free_in_buffer = state.free_in_buffer;
	ASSIGN_STATE(entropy->saved, state.cur);
	
	/* Update restart-interval state too */
	if (cinfo->restart_interval) {
		if (entropy->restarts_to_go == 0) {
			entropy->restarts_to_go = cinfo->restart_interval;
			entropy->next_restart_num++;
			entropy->next_restart_num &= 7;
		}
		entropy->restarts_to_go--;
	}

	return TRUE;
}

#ifndef NO_STRUCT_ASSIGN
#define ASSIGN_STATE(dest,src)  ((dest) = (src))
#else
#if MAX_COMPS_IN_SCAN == 4
#define ASSIGN_STATE(dest,src)  \
	((dest).put_buffer = (src).put_buffer, \
	(dest).put_bits = (src).put_bits, \
	(dest).last_dc_val[0] = (src).last_dc_val[0], \
	(dest).last_dc_val[1] = (src).last_dc_val[1], \
	(dest).last_dc_val[2] = (src).last_dc_val[2], \
(dest).last_dc_val[3] = (src).last_dc_val[3])
#endif
#endif

LOCAL(bool)
dump_buffer (working_state * state)
/* Empty the output buffer; return TRUE if successful, FALSE if must suspend */
{
	struct jpeg_destination_mgr * dest = state->cinfo->dest;
	
	if (! (*dest->empty_output_buffer) (state->cinfo))
		return FALSE;
	/* After a successful buffer dump, must reset buffer pointers */
	state->next_output_byte = dest->next_output_byte;
	state->free_in_buffer = dest->free_in_buffer;
	return TRUE;
}

#define emit_byte_define(state,val,action)  \
{ *(state)->next_output_byte++ = (JOCTET) (val);  \
	if (--(state)->free_in_buffer == 0)  \
	if (! dump_buffer(state))  \
{ action; } }

LOCAL(bool)
emit_bits (working_state * state, unsigned int code, int size)
/* Emit some bits; return TRUE if successful, FALSE if must suspend */
{
	/* This routine is heavily used, so it's worth coding tightly. */
	register long put_buffer = (long) code;
	register int put_bits = state->cur.put_bits;
	
	/* if size is 0, caller used an invalid Huffman table entry */
	if (size == 0)
		ERREXIT(state->cinfo, JERR_HUFF_MISSING_CODE);
	
	put_buffer &= (((long) 1)<<size) - 1; /* mask off any extra bits in code */
	
	put_bits += size;		/* new number of bits in buffer */
	
	put_buffer <<= 24 - put_bits; /* align incoming bits */
	
	put_buffer |= state->cur.put_buffer; /* and merge with old buffer contents */
	
	while (put_bits >= 8)
	{
//		assert(false);
		int c = (int) ((put_buffer >> 16) & 0xFF);
		
		emit_byte_define(state, c, return FALSE);
		if (c == 0xFF) {		/* need to stuff a zero byte? */
			emit_byte_define(state, 0, return FALSE);
		}
		put_buffer <<= 8;
		put_bits -= 8;
	}
	
	state->cur.put_buffer = put_buffer; /* update state variables */
	state->cur.put_bits = put_bits;
	
	return TRUE;
}

LOCAL(bool)
flush_bits (working_state * state)
{
	if (! emit_bits(state, 0x7F, 7)) /* fill any partial byte with ones */
		return FALSE;
	state->cur.put_buffer = 0;	/* and reset bit-buffer to empty */
	state->cur.put_bits = 0;
	return TRUE;
}

METHODDEF(void)
finish_pass_huff (j_compress_ptr cinfo)
{
	huff_entropy_ptr entropy = (huff_entropy_ptr) cinfo->entropy;
	working_state state;
	
	/* Load up working state ... flush_bits needs it */
	state.next_output_byte = cinfo->dest->next_output_byte;
	state.free_in_buffer = cinfo->dest->free_in_buffer;
	ASSIGN_STATE(state.cur, entropy->saved);
	state.cinfo = cinfo;
	
	/* Flush out the last data */
	if (! flush_bits(&state))
		ERREXIT(cinfo, JERR_CANT_SUSPEND);
	
	/* Update state */
	cinfo->dest->next_output_byte = state.next_output_byte;
	cinfo->dest->free_in_buffer = state.free_in_buffer;
	ASSIGN_STATE(entropy->saved, state.cur);
}

GLOBAL(void)
jpeg_make_c_derived_tbl (j_compress_ptr cinfo, JHUFF_TBL * htbl,
						 c_derived_tbl ** pdtbl)
{
	c_derived_tbl *dtbl;
	int p, i, l, lastp, si;
	char huffsize[257];
	unsigned int huffcode[257];
	unsigned int code;
	
	/* Allocate a workspace if we haven't already done so. */
	if (*pdtbl == NULL)
		*pdtbl = (c_derived_tbl *)
		(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
		SIZEOF(c_derived_tbl));
	dtbl = *pdtbl;
	
	/* Figure C.1: make table of Huffman code length for each symbol */
	/* Note that this is in code-length order. */
	
	p = 0;
	for (l = 1; l <= 16; l++) {
		for (i = 1; i <= (int) htbl->bits[l]; i++)
			huffsize[p++] = (char) l;
	}
	huffsize[p] = 0;
	lastp = p;
	
	/* Figure C.2: generate the codes themselves */
	/* Note that this is in code-length order. */
	
	code = 0;
	si = huffsize[0];
	p = 0;
	while (huffsize[p]) {
		while (((int) huffsize[p]) == si) {
			huffcode[p++] = code;
			code++;
		}
		code <<= 1;
		si++;
	}
	
	/* Figure C.3: generate encoding tables */
	/* These are code and size indexed by symbol value */
	
	/* Set any codeless symbols to have code length 0;
	* this allows emit_bits to detect any attempt to emit such symbols.
	*/
	MEMZERO(dtbl->ehufsi, SIZEOF(dtbl->ehufsi));
	
	for (p = 0; p < lastp; p++) {
		dtbl->ehufco[htbl->huffval[p]] = huffcode[p];
		dtbl->ehufsi[htbl->huffval[p]] = huffsize[p];
	}
}

//LOCAL(bool)
//emit_restart (working_state * state, int restart_num)
//{
//	int ci;
//	
//	if (! flush_bits(state))
//		return FALSE;
//	
//	emit_byte(state, 0xFF, return FALSE);
//	emit_byte(state, JPEG_RST0 + restart_num, return FALSE);
//	
//	/* Re-initialize DC predictions to 0 */
//	for (ci = 0; ci < state->cinfo->comps_in_scan; ci++)
//		state->cur.last_dc_val[ci] = 0;
//	
//	/* The restart counter is not updated until we successfully write the MCU. */
//	
//	return TRUE;
//}

const int jpeg_natural_order[DCTSIZE2+16] = {
	0,  1,  8, 16,  9,  2,  3, 10,
		17, 24, 32, 25, 18, 11,  4,  5,
		12, 19, 26, 33, 40, 48, 41, 34,
		27, 20, 13,  6,  7, 14, 21, 28,
		35, 42, 49, 56, 57, 50, 43, 36,
		29, 22, 15, 23, 30, 37, 44, 51,
		58, 59, 52, 45, 38, 31, 39, 46,
		53, 60, 61, 54, 47, 55, 62, 63,
		63, 63, 63, 63, 63, 63, 63, 63, /* extra entries for safety in decoder */
		63, 63, 63, 63, 63, 63, 63, 63
};

LOCAL(bool)
encode_one_block (working_state * state, JCOEFPTR block, int last_dc_val,
				  c_derived_tbl *dctbl, c_derived_tbl *actbl)
{
	register int temp, temp2;
	register int nbits;
	register int k, r, i;
	
	/* Encode the DC coefficient difference per section F.1.2.1 */
	
	temp = temp2 = block[0] - last_dc_val;
	
	if (temp < 0) {
		temp = -temp;		/* temp is abs value of input */
		/* For a negative input, want temp2 = bitwise complement of abs(input) */
		/* This code assumes we are on a two's complement machine */
		temp2--;
	}
	
	/* Find the number of bits needed for the magnitude of the coefficient */
	nbits = 0;
	while (temp) {
		nbits++;
		temp >>= 1;
	}
	
	/* Emit the Huffman-coded symbol for the number of bits */
	if (! emit_bits(state, dctbl->ehufco[nbits], dctbl->ehufsi[nbits]))
		return FALSE;
	
	/* Emit that number of bits of the value, if positive, */
	/* or the complement of its magnitude, if negative. */
	if (nbits)			/* emit_bits rejects calls with size 0 */
		if (! emit_bits(state, (unsigned int) temp2, nbits))
			return FALSE;
		
		/* Encode the AC coefficients per section F.1.2.2 */
		
		r = 0;			/* r = run length of zeros */
		
		for (k = 1; k < DCTSIZE2; k++) {
			if ((temp = block[jpeg_natural_order[k]]) == 0) {
				r++;
			} else {
				/* if run length > 15, must emit special run-length-16 codes (0xF0) */
				while (r > 15) {
					if (! emit_bits(state, actbl->ehufco[0xF0], actbl->ehufsi[0xF0]))
						return FALSE;
					r -= 16;
				}
				
				temp2 = temp;
				if (temp < 0) {
					temp = -temp;		/* temp is abs value of input */
					/* This code assumes we are on a two's complement machine */
					temp2--;
				}
				
				/* Find the number of bits needed for the magnitude of the coefficient */
				nbits = 1;		/* there must be at least one 1 bit */
				while ((temp >>= 1))
					nbits++;
				
				/* Emit Huffman symbol for run length / number of bits */
				i = (r << 4) + nbits;
				if (! emit_bits(state, actbl->ehufco[i], actbl->ehufsi[i]))
					return FALSE;
				
				/* Emit that number of bits of the value, if positive, */
				/* or the complement of its magnitude, if negative. */
				if (! emit_bits(state, (unsigned int) temp2, nbits))
					return FALSE;
				
				r = 0;
			}
		}
		
		/* If the last coef(s) were zero, emit an end-of-block code */
		if (r > 0)
			if (! emit_bits(state, actbl->ehufco[0], actbl->ehufsi[0]))
				return FALSE;
			
			return TRUE;
}

LOCAL(void)
start_iMCU_row (j_compress_ptr cinfo)
/* Reset within-iMCU-row counters for a new row */
{
	my_coef_ptr coef = (my_coef_ptr) cinfo->coef;
	
	/* In an interleaved scan, an MCU row is the same as an iMCU row.
	* In a noninterleaved scan, an iMCU row has v_samp_factor MCU rows.
	* But at the bottom of the image, process only what's left.
	*/
	if (cinfo->comps_in_scan > 1) {
		coef->MCU_rows_per_iMCU_row = 1;
	} else {
		if (coef->iMCU_row_num < (cinfo->total_iMCU_rows-1))
			coef->MCU_rows_per_iMCU_row = cinfo->cur_comp_info[0]->v_samp_factor;
		else
			coef->MCU_rows_per_iMCU_row = cinfo->cur_comp_info[0]->last_row_height;
	}
	
	coef->mcu_ctr = 0;
	coef->MCU_vert_offset = 0;
}

METHODDEF(bool)
compress_data (j_compress_ptr cinfo, JSAMPIMAGE input_buf)
{
	my_coef_ptr coef = (my_coef_ptr) cinfo->coef;
	JDIMENSION MCU_col_num;	/* index of current MCU within row */
	JDIMENSION last_MCU_col = cinfo->MCUs_per_row - 1;
	JDIMENSION last_iMCU_row = cinfo->total_iMCU_rows - 1;
	int blkn, bi, ci, yindex, yoffset, blockcnt;
	JDIMENSION ypos, xpos;
	jpeg_component_info *compptr;
	/* Loop to write as much as one whole iMCU row */
	for (yoffset = coef->MCU_vert_offset; yoffset < coef->MCU_rows_per_iMCU_row;
	yoffset++) {
		for (MCU_col_num = coef->mcu_ctr; MCU_col_num <= last_MCU_col;
		MCU_col_num++) {
		/* Determine where data comes from in input_buf and do the DCT thing.
		* Each call on forward_DCT processes a horizontal row of DCT blocks
		* as wide as an MCU; we rely on having allocated the MCU_buffer[] blocks
		* sequentially.  Dummy blocks at the right or bottom edge are filled in
		* specially.  The data in them does not matter for image reconstruction,
		* so we fill them with values that will encode to the smallest amount of
		* data, viz: all zeroes in the AC entries, DC entries equal to previous
		* block's DC value.  (Thanks to Thomas Kinsman for this idea.)
			*/
			blkn = 0;
			for (ci = 0; ci < cinfo->comps_in_scan; ci++) {
				compptr = cinfo->cur_comp_info[ci];
				blockcnt = (MCU_col_num < last_MCU_col) ? compptr->MCU_width
					: compptr->last_col_width;
				xpos = MCU_col_num * compptr->MCU_sample_width;
				ypos = yoffset * DCTSIZE; /* ypos == (yoffset+yindex) * DCTSIZE */
				for (yindex = 0; yindex < compptr->MCU_height; yindex++) {
					if (coef->iMCU_row_num < last_iMCU_row ||
						yoffset+yindex < compptr->last_row_height) {
						(*cinfo->fdct->forward_DCT) (cinfo, compptr,
							input_buf[ci], coef->MCU_buffer[blkn],
							ypos, xpos, (JDIMENSION) blockcnt);
						if (blockcnt < compptr->MCU_width) {
							/* Create some dummy blocks at the right edge of the image. */
//							jzero_far((void FAR *) coef->MCU_buffer[blkn + blockcnt],
//								(compptr->MCU_width - blockcnt) * SIZEOF(JBLOCK));
							for (bi = blockcnt; bi < compptr->MCU_width; bi++) {
								coef->MCU_buffer[blkn+bi][0][0] = coef->MCU_buffer[blkn+bi-1][0][0];
							}
						}
					} else {
						/* Create a row of dummy blocks at the bottom of the image. */
//						jzero_far((void FAR *) coef->MCU_buffer[blkn],
//							compptr->MCU_width * SIZEOF(JBLOCK));
						for (bi = 0; bi < compptr->MCU_width; bi++) {
							coef->MCU_buffer[blkn+bi][0][0] = coef->MCU_buffer[blkn-1][0][0];
						}
					}
					blkn += compptr->MCU_width;
					ypos += DCTSIZE;
				}
			}
			/* Try to write the MCU.  In event of a suspension failure, we will
			* re-DCT the MCU on restart (a bit inefficient, could be fixed...)
			*/
			if (! (*cinfo->entropy->encode_mcu) (cinfo, coef->MCU_buffer)) {
				/* Suspension forced; update state counters and exit */
				coef->MCU_vert_offset = yoffset;
				coef->mcu_ctr = MCU_col_num;
				return FALSE;
			}
		}
		/* Completed an MCU row, but perhaps not an iMCU row */
		coef->mcu_ctr = 0;
	}
	/* Completed the iMCU row, advance counters for next one */
	coef->iMCU_row_num++;
	start_iMCU_row(cinfo);
	return TRUE;
}

METHODDEF(void)
start_pass_coef (j_compress_ptr cinfo, J_BUF_MODE pass_mode)
{
	my_coef_ptr coef = (my_coef_ptr) cinfo->coef;
	
	coef->iMCU_row_num = 0;
	start_iMCU_row(cinfo);
	
	switch (pass_mode) {
	case JBUF_PASS_THRU:
		if (coef->whole_image[0] != NULL)
			ERREXIT(cinfo, JERR_BAD_BUFFER_MODE);
		coef->pub.compress_data = compress_data;
		break;
#ifdef FULL_COEF_BUFFER_SUPPORTED
	case JBUF_SAVE_AND_PASS:
		if (coef->whole_image[0] == NULL)
			ERREXIT(cinfo, JERR_BAD_BUFFER_MODE);
		coef->pub.compress_data = compress_first_pass;
		break;
	case JBUF_CRANK_DEST:
		if (coef->whole_image[0] == NULL)
			ERREXIT(cinfo, JERR_BAD_BUFFER_MODE);
		coef->pub.compress_data = compress_output;
		break;
#endif
	default:
		ERREXIT(cinfo, JERR_BAD_BUFFER_MODE);
		break;
	}
}

METHODDEF(void)
start_pass_main (j_compress_ptr cinfo, J_BUF_MODE pass_mode)
{
	my_main_ptr main = (my_main_ptr) cinfo->main;
	
	/* Do nothing in raw-data mode. */
	if (cinfo->raw_data_in)
		return;
	
	main->cur_iMCU_row = 0;	/* initialize counters */
	main->rowgroup_ctr = 0;
	main->suspended = FALSE;
	main->pass_mode = pass_mode;	/* save mode for use by process_data */
	
	switch (pass_mode) {
	case JBUF_PASS_THRU:
#ifdef FULL_MAIN_BUFFER_SUPPORTED
		if (main->whole_image[0] != NULL)
			ERREXIT(cinfo, JERR_BAD_BUFFER_MODE);
#endif
		main->pub.process_data = process_data_simple_main;
		break;
#ifdef FULL_MAIN_BUFFER_SUPPORTED
	case JBUF_SAVE_SOURCE:
	case JBUF_CRANK_DEST:
	case JBUF_SAVE_AND_PASS:
		if (main->whole_image[0] == NULL)
			ERREXIT(cinfo, JERR_BAD_BUFFER_MODE);
		main->pub.process_data = process_data_buffer_main;
		break;
#endif
	default:
		ERREXIT(cinfo, JERR_BAD_BUFFER_MODE);
		break;
	}
}

METHODDEF(void)
process_data_simple_main (j_compress_ptr cinfo,
						  JSAMPARRAY input_buf, JDIMENSION *in_row_ctr,
						  JDIMENSION in_rows_avail)
{
	my_main_ptr main = (my_main_ptr) cinfo->main;

	while (main->cur_iMCU_row < cinfo->total_iMCU_rows) {
		/* Read input data if we haven't filled the main buffer yet */

		if (main->rowgroup_ctr < DCTSIZE)
			(*cinfo->prep->pre_process_data) (cinfo,
			input_buf, in_row_ctr, in_rows_avail,
			main->buffer, &main->rowgroup_ctr,
			(JDIMENSION) DCTSIZE);

			/* If we don't have a full iMCU row buffered, return to application for
			* more data.  Note that preprocessor will always pad to fill the iMCU row
			* at the bottom of the image.
		*/
		if (main->rowgroup_ctr != DCTSIZE)
			return;

		/* Send the completed row to the compressor */
		if (! (*cinfo->coef->compress_data) (cinfo, main->buffer)) {
		/* If compressor did not consume the whole row, then we must need to
		* suspend processing and return to the application.  In this situation
		* we pretend we didn't yet consume the last input row; otherwise, if
		* it happened to be the last row of the image, the application would
		* think we were done.
			*/
			if (! main->suspended) {
				(*in_row_ctr)--;
				main->suspended = TRUE;
			}
			return;
		}
		/* We did finish the row.  Undo our little suspension hack if a previous
		* call suspended; then mark the main buffer empty.
		*/
		if (main->suspended) {
			(*in_row_ctr)++;
			main->suspended = FALSE;
		}
		main->rowgroup_ctr = 0;
		main->cur_iMCU_row++;
	}
}

LOCAL(int)
emit_dqt (j_compress_ptr cinfo, int index)
/* Emit a DQT marker */
/* Returns the precision used (0 = 8bits, 1 = 16bits) for baseline checking */
{
	JQUANT_TBL * qtbl = cinfo->quant_tbl_ptrs[index];
	int prec;
	int i;
	
	if (qtbl == NULL)
		ERREXIT1(cinfo, JERR_NO_QUANT_TABLE, index);
	
	prec = 0;
	for (i = 0; i < DCTSIZE2; i++) {
		if (qtbl->quantval[i] > 255)
			prec = 1;
	}
	
	if (! qtbl->sent_table) {
		emit_marker(cinfo, M_DQT);
		
		emit_2bytes(cinfo, prec ? DCTSIZE2*2 + 1 + 2 : DCTSIZE2 + 1 + 2);
		
		emit_byte(cinfo, index + (prec<<4));
		
		for (i = 0; i < DCTSIZE2; i++) {
			/* The table entries must be emitted in zigzag order. */
			unsigned int qval = qtbl->quantval[jpeg_natural_order[i]];
			if (prec)
				emit_byte(cinfo, qval >> 8);
			emit_byte(cinfo, qval & 0xFF);
		}
		
		qtbl->sent_table = TRUE;
	}
	
	return prec;
}

LOCAL(void)
emit_sof (j_compress_ptr cinfo, JPEG_MARKER code)
/* Emit a SOF marker */
{
	int ci;
	jpeg_component_info *compptr;
	
	emit_marker(cinfo, code);
	
	emit_2bytes(cinfo, 3 * cinfo->num_components + 2 + 5 + 1); /* length */
	
	/* Make sure image isn't bigger than SOF field can handle */
	if ((long) cinfo->image_height > 65535L ||
		(long) cinfo->image_width > 65535L)
		ERREXIT1(cinfo, JERR_IMAGE_TOO_BIG, (unsigned int) 65535);
	
	emit_byte(cinfo, cinfo->data_precision);
	emit_2bytes(cinfo, (int) cinfo->image_height);
	emit_2bytes(cinfo, (int) cinfo->image_width);
	
	emit_byte(cinfo, cinfo->num_components);
	
	for (ci = 0, compptr = cinfo->comp_info; ci < cinfo->num_components;
	ci++, compptr++) {
		emit_byte(cinfo, compptr->component_id);
		emit_byte(cinfo, (compptr->h_samp_factor << 4) + compptr->v_samp_factor);
		emit_byte(cinfo, compptr->quant_tbl_no);
	}
}

METHODDEF(void)
write_frame_header (j_compress_ptr cinfo)
{
	int ci, prec;
	bool is_baseline;
	jpeg_component_info *compptr;
	
	/* Emit DQT for each quantization table.
	* Note that emit_dqt() suppresses any duplicate tables.
	*/
	prec = 0;
	for (ci = 0, compptr = cinfo->comp_info; ci < cinfo->num_components;
	ci++, compptr++) {
		prec += emit_dqt(cinfo, compptr->quant_tbl_no);
	}
	/* now prec is nonzero iff there are any 16-bit quant tables. */
	
	/* Check for a non-baseline specification.
	* Note we assume that Huffman table numbers won't be changed later.
	*/
	if (cinfo->arith_code || cinfo->progressive_mode ||
		cinfo->data_precision != 8) {
		is_baseline = FALSE;
	} else {
		is_baseline = TRUE;
		for (ci = 0, compptr = cinfo->comp_info; ci < cinfo->num_components;
		ci++, compptr++) {
			if (compptr->dc_tbl_no > 1 || compptr->ac_tbl_no > 1)
				is_baseline = FALSE;
		}
		if (prec && is_baseline) {
			is_baseline = FALSE;
			/* If it's baseline except for quantizer size, warn the user */
			TRACEMS(cinfo, 0, JTRC_16BIT_TABLES);
		}
	}
	
	/* Emit the proper SOF marker */
	if (cinfo->arith_code) {
		emit_sof(cinfo, M_SOF9);	/* SOF code for arithmetic coding */
	} else {
		if (cinfo->progressive_mode)
			emit_sof(cinfo, M_SOF2);	/* SOF code for progressive Huffman */
		else if (is_baseline)
			emit_sof(cinfo, M_SOF0);	/* SOF code for baseline implementation */
		else
			emit_sof(cinfo, M_SOF1);	/* SOF code for non-baseline Huffman file */
	}
}

LOCAL(void)
emit_dht (j_compress_ptr cinfo, int index, bool is_ac)
/* Emit a DHT marker */
{
	JHUFF_TBL * htbl;
	int length, i;
	
	if (is_ac) {
		htbl = cinfo->ac_huff_tbl_ptrs[index];
		index += 0x10;		/* output index has AC bit set */
	} else {
		htbl = cinfo->dc_huff_tbl_ptrs[index];
	}
	
	if (htbl == NULL)
		ERREXIT1(cinfo, JERR_NO_HUFF_TABLE, index);
	
	if (! htbl->sent_table) {
		emit_marker(cinfo, M_DHT);
		
		length = 0;
		for (i = 1; i <= 16; i++)
			length += htbl->bits[i];
		
		emit_2bytes(cinfo, length + 2 + 1 + 16);
		emit_byte(cinfo, index);
		
		for (i = 1; i <= 16; i++)
			emit_byte(cinfo, htbl->bits[i]);
		
		for (i = 0; i < length; i++)
			emit_byte(cinfo, htbl->huffval[i]);
		
		htbl->sent_table = TRUE;
	}
}

LOCAL(void)
emit_dri (j_compress_ptr cinfo)
/* Emit a DRI marker */
{
	emit_marker(cinfo, M_DRI);
	
	emit_2bytes(cinfo, 4);	/* fixed length */
	
	emit_2bytes(cinfo, (int) cinfo->restart_interval);
}


//LOCAL(void)
//emit_sof (j_compress_ptr cinfo, JPEG_MARKER code)
///* Emit a SOF marker */
//{
//	int ci;
//	jpeg_component_info *compptr;
//	
//	emit_marker(cinfo, code);
//	
//	emit_2bytes(cinfo, 3 * cinfo->num_components + 2 + 5 + 1); /* length */
//	
//	/* Make sure image isn't bigger than SOF field can handle */
//	if ((long) cinfo->image_height > 65535L ||
//		(long) cinfo->image_width > 65535L)
//		ERREXIT1(cinfo, JERR_IMAGE_TOO_BIG, (unsigned int) 65535);
//	
//	emit_byte(cinfo, cinfo->data_precision);
//	emit_2bytes(cinfo, (int) cinfo->image_height);
//	emit_2bytes(cinfo, (int) cinfo->image_width);
//	
//	emit_byte(cinfo, cinfo->num_components);
//	
//	for (ci = 0, compptr = cinfo->comp_info; ci < cinfo->num_components;
//	ci++, compptr++) {
//		emit_byte(cinfo, compptr->component_id);
//		emit_byte(cinfo, (compptr->h_samp_factor << 4) + compptr->v_samp_factor);
//		emit_byte(cinfo, compptr->quant_tbl_no);
//	}
//}

LOCAL(void)
emit_sos (j_compress_ptr cinfo)
/* Emit a SOS marker */
{
	int i, td, ta;
	jpeg_component_info *compptr;
	
	emit_marker(cinfo, M_SOS);
	
	emit_2bytes(cinfo, 2 * cinfo->comps_in_scan + 2 + 1 + 3); /* length */
	
	emit_byte(cinfo, cinfo->comps_in_scan);
	
	for (i = 0; i < cinfo->comps_in_scan; i++) {
		compptr = cinfo->cur_comp_info[i];
		emit_byte(cinfo, compptr->component_id);
		td = compptr->dc_tbl_no;
		ta = compptr->ac_tbl_no;
		if (cinfo->progressive_mode) {
		/* Progressive mode: only DC or only AC tables are used in one scan;
		* furthermore, Huffman coding of DC refinement uses no table at all.
		* We emit 0 for unused field(s); this is recommended by the P&M text
		* but does not seem to be specified in the standard.
			*/
			if (cinfo->Ss == 0) {
				ta = 0;			/* DC scan */
				if (cinfo->Ah != 0 && !cinfo->arith_code)
					td = 0;		/* no DC table either */
			} else {
				td = 0;			/* AC scan */
			}
		}
		emit_byte(cinfo, (td << 4) + ta);
	}
	
	emit_byte(cinfo, cinfo->Ss);
	emit_byte(cinfo, cinfo->Se);
	emit_byte(cinfo, (cinfo->Ah << 4) + cinfo->Al);
}

METHODDEF(void)
write_scan_header (j_compress_ptr cinfo)
{
	int i;
	jpeg_component_info *compptr;
	
	if (cinfo->arith_code) {
    /* Emit arith conditioning info.  We may have some duplication
	* if the file has multiple scans, but it's so small it's hardly
	* worth worrying about.
		*/
//		emit_dac(cinfo);
	} else {
    /* Emit Huffman tables.
	* Note that emit_dht() suppresses any duplicate tables.
		*/
		for (i = 0; i < cinfo->comps_in_scan; i++) {
			compptr = cinfo->cur_comp_info[i];
			if (cinfo->progressive_mode) {
				/* Progressive mode: only DC or only AC tables are used in one scan */
				if (cinfo->Ss == 0) {
					if (cinfo->Ah == 0)	/* DC needs no table for refinement scan */
						emit_dht(cinfo, compptr->dc_tbl_no, FALSE);
				} else {
					emit_dht(cinfo, compptr->ac_tbl_no, TRUE);
				}
			} else {
				/* Sequential mode: need both DC and AC tables */
				emit_dht(cinfo, compptr->dc_tbl_no, FALSE);
				emit_dht(cinfo, compptr->ac_tbl_no, TRUE);
			}
		}
	}
	
	/* Emit DRI if required --- note that DRI value could change for each scan.
	* If it doesn't, a tiny amount of space is wasted in multiple-scan files.
	* We assume DRI will never be nonzero for one scan and zero for a later one.
	*/
	if (cinfo->restart_interval)
		emit_dri(cinfo);
	
	emit_sos(cinfo);
}

#define RGB_RED		0	/* Offset of Red in an RGB scanline element */
#define RGB_GREEN	1	/* Offset of Green */
#define RGB_BLUE	2	/* Offset of Blue */
#define RGB_PIXELSIZE	3	/* JSAMPLEs per RGB scanline element */

METHODDEF(void)
rgb_ycc_convert (j_compress_ptr cinfo,
				 JSAMPARRAY input_buf, JSAMPIMAGE output_buf,
				 JDIMENSION output_row, int num_rows)
{
	my_cconvert_ptr cconvert = (my_cconvert_ptr) cinfo->cconvert;
	register int r, g, b;
	register long * ctab = cconvert->rgb_ycc_tab;
	register JSAMPROW inptr;
	register JSAMPROW outptr0, outptr1, outptr2;
	register JDIMENSION col;
	JDIMENSION num_cols = cinfo->image_width;
	
	while (--num_rows >= 0) {
		inptr = *input_buf++;
		outptr0 = output_buf[0][output_row];
		outptr1 = output_buf[1][output_row];
		outptr2 = output_buf[2][output_row];
		output_row++;
		for (col = 0; col < num_cols; col++) {
			r = GETJSAMPLE(inptr[RGB_RED]);
			g = GETJSAMPLE(inptr[RGB_GREEN]);
			b = GETJSAMPLE(inptr[RGB_BLUE]);
			inptr += RGB_PIXELSIZE;
			/* If the inputs are 0..MAXJSAMPLE, the outputs of these equations
			* must be too; we do not need an explicit range-limiting operation.
			* Hence the value being shifted is never negative, and we don't
			* need the general RIGHT_SHIFT macro.
			*/
			/* Y */
			outptr0[col] = (JSAMPLE)
				((ctab[r+R_Y_OFF] + ctab[g+G_Y_OFF] + ctab[b+B_Y_OFF])
				>> SCALEBITS);
			/* Cb */
			outptr1[col] = (JSAMPLE)
				((ctab[r+R_CB_OFF] + ctab[g+G_CB_OFF] + ctab[b+B_CB_OFF])
				>> SCALEBITS);
			/* Cr */
			outptr2[col] = (JSAMPLE)
				((ctab[r+R_CR_OFF] + ctab[g+G_CR_OFF] + ctab[b+B_CR_OFF])
				>> SCALEBITS);
		}
	}
}

METHODDEF(void)
sep_downsample (j_compress_ptr cinfo,
				JSAMPIMAGE input_buf, JDIMENSION in_row_index,
				JSAMPIMAGE output_buf, JDIMENSION out_row_group_index)
{
	my_downsample_ptr downsample = (my_downsample_ptr) cinfo->downsample;
	int ci;
	jpeg_component_info * compptr;
	JSAMPARRAY in_ptr, out_ptr;
	
	for (ci = 0, compptr = cinfo->comp_info; ci < cinfo->num_components;
	ci++, compptr++) {
		in_ptr = input_buf[ci] + in_row_index;
		out_ptr = output_buf[ci] + (out_row_group_index * compptr->v_samp_factor);
		(*downsample->methods[ci]) (cinfo, compptr, in_ptr, out_ptr);
	}
}

LOCAL(void)
expand_right_edge (JSAMPARRAY image_data, int num_rows,
				   JDIMENSION input_cols, JDIMENSION output_cols)
{
	register JSAMPROW ptr;
	register JSAMPLE pixval;
	register int count;
	int row;
	int numcols = (int) (output_cols - input_cols);
	
	if (numcols > 0) {
		for (row = 0; row < num_rows; row++) {
			ptr = image_data[row] + input_cols;
			pixval = ptr[-1];		/* don't need GETJSAMPLE() here */
			for (count = numcols; count > 0; count--)
				*ptr++ = pixval;
		}
	}
}

METHODDEF(void)
fullsize_downsample (j_compress_ptr cinfo, jpeg_component_info * compptr,
					 JSAMPARRAY input_data, JSAMPARRAY output_data)
{
	/* Copy the data */
	jcopy_sample_rows(input_data, 0, output_data, 0,
		cinfo->max_v_samp_factor, cinfo->image_width);
	/* Edge-expand */
	expand_right_edge(output_data, cinfo->max_v_samp_factor,
		cinfo->image_width, compptr->width_in_blocks * DCTSIZE);
}

METHODDEF(void)
h2v2_downsample (j_compress_ptr cinfo, jpeg_component_info * compptr,
				 JSAMPARRAY input_data, JSAMPARRAY output_data)
{
	int inrow, outrow;
	JDIMENSION outcol;
	JDIMENSION output_cols = compptr->width_in_blocks * DCTSIZE;
	register JSAMPROW inptr0, inptr1, outptr;
	register int bias;
	
	/* Expand input data enough to let all the output samples be generated
	* by the standard loop.  Special-casing padded output would be more
	* efficient.
	*/
	expand_right_edge(input_data, cinfo->max_v_samp_factor,
		cinfo->image_width, output_cols * 2);
	
	inrow = 0;
	for (outrow = 0; outrow < compptr->v_samp_factor; outrow++) {
		outptr = output_data[outrow];
		inptr0 = input_data[inrow];
		inptr1 = input_data[inrow+1];
		bias = 1;			/* bias = 1,2,1,2,... for successive samples */
		for (outcol = 0; outcol < output_cols; outcol++) {
			*outptr++ = (JSAMPLE) ((GETJSAMPLE(*inptr0) + GETJSAMPLE(inptr0[1]) +
				GETJSAMPLE(*inptr1) + GETJSAMPLE(inptr1[1])
				+ bias) >> 2);
			bias ^= 3;		/* 1=>2, 2=>1 */
			inptr0 += 2; inptr1 += 2;
		}
		inrow += 2;
	}
}

#if BITS_IN_JSAMPLE == 8
#define CONST_BITS  13
#define PASS1_BITS  2
#else
#define CONST_BITS  13
#define PASS1_BITS  1		/* lose a little precision to avoid overflow */
#endif

/* Some C compilers fail to reduce "FIX(constant)" at compile time, thus
* causing a lot of useless floating-point operations at run time.
* To get around this we use the following pre-calculated constants.
* If you change CONST_BITS you may want to add appropriate values.
* (With a reasonable C compiler, you can just rely on the FIX() macro...)
*/

#if CONST_BITS == 13
#define FIX_0_298631336  ((long)  2446)	/* FIX(0.298631336) */
#define FIX_0_390180644  ((long)  3196)	/* FIX(0.390180644) */
#define FIX_0_541196100  ((long)  4433)	/* FIX(0.541196100) */
#define FIX_0_765366865  ((long)  6270)	/* FIX(0.765366865) */
#define FIX_0_899976223  ((long)  7373)	/* FIX(0.899976223) */
#define FIX_1_175875602  ((long)  9633)	/* FIX(1.175875602) */
#define FIX_1_501321110  ((long)  12299)	/* FIX(1.501321110) */
#define FIX_1_847759065  ((long)  15137)	/* FIX(1.847759065) */
#define FIX_1_961570560  ((long)  16069)	/* FIX(1.961570560) */
#define FIX_2_053119869  ((long)  16819)	/* FIX(2.053119869) */
#define FIX_2_562915447  ((long)  20995)	/* FIX(2.562915447) */
#define FIX_3_072711026  ((long)  25172)	/* FIX(3.072711026) */
#else
#define FIX_0_298631336  FIX(0.298631336)
#define FIX_0_390180644  FIX(0.390180644)
#define FIX_0_541196100  FIX(0.541196100)
#define FIX_0_765366865  FIX(0.765366865)
#define FIX_0_899976223  FIX(0.899976223)
#define FIX_1_175875602  FIX(1.175875602)
#define FIX_1_501321110  FIX(1.501321110)
#define FIX_1_847759065  FIX(1.847759065)
#define FIX_1_961570560  FIX(1.961570560)
#define FIX_2_053119869  FIX(2.053119869)
#define FIX_2_562915447  FIX(2.562915447)
#define FIX_3_072711026  FIX(3.072711026)
#endif

#if BITS_IN_JSAMPLE == 8
#define MULTIPLY(var,const)  MULTIPLY16C16(var,const)
#else
#define MULTIPLY(var,const)  ((var) * (const))
#endif

GLOBAL(void)
jpeg_fdct_islow (DCTELEM * data)
{
  long tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
  long tmp10, tmp11, tmp12, tmp13;
  long z1, z2, z3, z4, z5;
  DCTELEM *dataptr;
  int ctr;
  SHIFT_TEMPS

  /* Pass 1: process rows. */
  /* Note results are scaled up by sqrt(8) compared to a true DCT; */
  /* furthermore, we scale the results by 2**PASS1_BITS. */

  dataptr = data;
  for (ctr = DCTSIZE-1; ctr >= 0; ctr--) {
    tmp0 = dataptr[0] + dataptr[7];
    tmp7 = dataptr[0] - dataptr[7];
    tmp1 = dataptr[1] + dataptr[6];
    tmp6 = dataptr[1] - dataptr[6];
    tmp2 = dataptr[2] + dataptr[5];
    tmp5 = dataptr[2] - dataptr[5];
    tmp3 = dataptr[3] + dataptr[4];
    tmp4 = dataptr[3] - dataptr[4];
    
    /* Even part per LL&M figure 1 --- note that published figure is faulty;
     * rotator "sqrt(2)*c1" should be "sqrt(2)*c6".
     */
    
    tmp10 = tmp0 + tmp3;
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;
    
    dataptr[0] = (DCTELEM) ((tmp10 + tmp11) << PASS1_BITS);
    dataptr[4] = (DCTELEM) ((tmp10 - tmp11) << PASS1_BITS);
    
    z1 = MULTIPLY(tmp12 + tmp13, FIX_0_541196100);
    dataptr[2] = (DCTELEM) DESCALE(z1 + MULTIPLY(tmp13, FIX_0_765366865),
				   CONST_BITS-PASS1_BITS);
    dataptr[6] = (DCTELEM) DESCALE(z1 + MULTIPLY(tmp12, - FIX_1_847759065),
				   CONST_BITS-PASS1_BITS);
    
    /* Odd part per figure 8 --- note paper omits factor of sqrt(2).
     * cK represents cos(K*pi/16).
     * i0..i3 in the paper are tmp4..tmp7 here.
     */
    
    z1 = tmp4 + tmp7;
    z2 = tmp5 + tmp6;
    z3 = tmp4 + tmp6;
    z4 = tmp5 + tmp7;
    z5 = MULTIPLY(z3 + z4, FIX_1_175875602); /* sqrt(2) * c3 */
    
    tmp4 = MULTIPLY(tmp4, FIX_0_298631336); /* sqrt(2) * (-c1+c3+c5-c7) */
    tmp5 = MULTIPLY(tmp5, FIX_2_053119869); /* sqrt(2) * ( c1+c3-c5+c7) */
    tmp6 = MULTIPLY(tmp6, FIX_3_072711026); /* sqrt(2) * ( c1+c3+c5-c7) */
    tmp7 = MULTIPLY(tmp7, FIX_1_501321110); /* sqrt(2) * ( c1+c3-c5-c7) */
    z1 = MULTIPLY(z1, - FIX_0_899976223); /* sqrt(2) * (c7-c3) */
    z2 = MULTIPLY(z2, - FIX_2_562915447); /* sqrt(2) * (-c1-c3) */
    z3 = MULTIPLY(z3, - FIX_1_961570560); /* sqrt(2) * (-c3-c5) */
    z4 = MULTIPLY(z4, - FIX_0_390180644); /* sqrt(2) * (c5-c3) */
    
    z3 += z5;
    z4 += z5;
    
    dataptr[7] = (DCTELEM) DESCALE(tmp4 + z1 + z3, CONST_BITS-PASS1_BITS);
    dataptr[5] = (DCTELEM) DESCALE(tmp5 + z2 + z4, CONST_BITS-PASS1_BITS);
    dataptr[3] = (DCTELEM) DESCALE(tmp6 + z2 + z3, CONST_BITS-PASS1_BITS);
    dataptr[1] = (DCTELEM) DESCALE(tmp7 + z1 + z4, CONST_BITS-PASS1_BITS);
    
    dataptr += DCTSIZE;		/* advance pointer to next row */
  }

  /* Pass 2: process columns.
   * We remove the PASS1_BITS scaling, but leave the results scaled up
   * by an overall factor of 8.
   */

  dataptr = data;
  for (ctr = DCTSIZE-1; ctr >= 0; ctr--) {
    tmp0 = dataptr[DCTSIZE*0] + dataptr[DCTSIZE*7];
    tmp7 = dataptr[DCTSIZE*0] - dataptr[DCTSIZE*7];
    tmp1 = dataptr[DCTSIZE*1] + dataptr[DCTSIZE*6];
    tmp6 = dataptr[DCTSIZE*1] - dataptr[DCTSIZE*6];
    tmp2 = dataptr[DCTSIZE*2] + dataptr[DCTSIZE*5];
    tmp5 = dataptr[DCTSIZE*2] - dataptr[DCTSIZE*5];
    tmp3 = dataptr[DCTSIZE*3] + dataptr[DCTSIZE*4];
    tmp4 = dataptr[DCTSIZE*3] - dataptr[DCTSIZE*4];
    
    /* Even part per LL&M figure 1 --- note that published figure is faulty;
     * rotator "sqrt(2)*c1" should be "sqrt(2)*c6".
     */
    
    tmp10 = tmp0 + tmp3;
    tmp13 = tmp0 - tmp3;
    tmp11 = tmp1 + tmp2;
    tmp12 = tmp1 - tmp2;
    
    dataptr[DCTSIZE*0] = (DCTELEM) DESCALE(tmp10 + tmp11, PASS1_BITS);
    dataptr[DCTSIZE*4] = (DCTELEM) DESCALE(tmp10 - tmp11, PASS1_BITS);
    
    z1 = MULTIPLY(tmp12 + tmp13, FIX_0_541196100);
    dataptr[DCTSIZE*2] = (DCTELEM) DESCALE(z1 + MULTIPLY(tmp13, FIX_0_765366865),
					   CONST_BITS+PASS1_BITS);
    dataptr[DCTSIZE*6] = (DCTELEM) DESCALE(z1 + MULTIPLY(tmp12, - FIX_1_847759065),
					   CONST_BITS+PASS1_BITS);
    
    /* Odd part per figure 8 --- note paper omits factor of sqrt(2).
     * cK represents cos(K*pi/16).
     * i0..i3 in the paper are tmp4..tmp7 here.
     */
    
    z1 = tmp4 + tmp7;
    z2 = tmp5 + tmp6;
    z3 = tmp4 + tmp6;
    z4 = tmp5 + tmp7;
    z5 = MULTIPLY(z3 + z4, FIX_1_175875602); /* sqrt(2) * c3 */
    
    tmp4 = MULTIPLY(tmp4, FIX_0_298631336); /* sqrt(2) * (-c1+c3+c5-c7) */
    tmp5 = MULTIPLY(tmp5, FIX_2_053119869); /* sqrt(2) * ( c1+c3-c5+c7) */
    tmp6 = MULTIPLY(tmp6, FIX_3_072711026); /* sqrt(2) * ( c1+c3+c5-c7) */
    tmp7 = MULTIPLY(tmp7, FIX_1_501321110); /* sqrt(2) * ( c1+c3-c5-c7) */
    z1 = MULTIPLY(z1, - FIX_0_899976223); /* sqrt(2) * (c7-c3) */
    z2 = MULTIPLY(z2, - FIX_2_562915447); /* sqrt(2) * (-c1-c3) */
    z3 = MULTIPLY(z3, - FIX_1_961570560); /* sqrt(2) * (-c3-c5) */
    z4 = MULTIPLY(z4, - FIX_0_390180644); /* sqrt(2) * (c5-c3) */
    
    z3 += z5;
    z4 += z5;
    
    dataptr[DCTSIZE*7] = (DCTELEM) DESCALE(tmp4 + z1 + z3,
					   CONST_BITS+PASS1_BITS);
    dataptr[DCTSIZE*5] = (DCTELEM) DESCALE(tmp5 + z2 + z4,
					   CONST_BITS+PASS1_BITS);
    dataptr[DCTSIZE*3] = (DCTELEM) DESCALE(tmp6 + z2 + z3,
					   CONST_BITS+PASS1_BITS);
    dataptr[DCTSIZE*1] = (DCTELEM) DESCALE(tmp7 + z1 + z4,
					   CONST_BITS+PASS1_BITS);
    
    dataptr++;			/* advance pointer to next column */
  }
}

METHODDEF(bool)
empty_output_buffer (j_compress_ptr cinfo)
{
	my_dest_ptr dest = (my_dest_ptr) cinfo->dest;
	
//	if (JFWRITE(dest->outfile, dest->buffer, OUTPUT_BUF_SIZE) !=
//		(size_t) OUTPUT_BUF_SIZE)
//		ERREXIT(cinfo, JERR_FILE_WRITE);
	dest->buffer = (*DumpBuffFunction)(dest->buffer, dest->pub.free_in_buffer, FALSE);
	
	dest->pub.next_output_byte = dest->buffer;
//	dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
	
	if (dest->buffer == NULL)
	{
		longjmp(*pCurjmp, 1);
	}
	return TRUE;
}

METHODDEF(void)
finish_pass_master (j_compress_ptr cinfo)
{
	my_master_ptr master = (my_master_ptr) cinfo->master;
	
	/* The entropy coder always needs an end-of-pass call,
	* either to analyze statistics or to flush its output buffer.
	*/
	(*cinfo->entropy->finish_pass) (cinfo);
	
	/* Update state for next pass */
	switch (master->pass_type) {
	case main_pass:
    /* next pass is either output of scan 0 (after optimization)
	* or output of scan 1 (if no optimization).
		*/
		master->pass_type = output_pass;
		if (! cinfo->optimize_coding)
			master->scan_number++;
		break;
	case huff_opt_pass:
		/* next pass is always output of current scan */
		master->pass_type = output_pass;
		break;
	case output_pass:
		/* next pass is either optimization or output of next scan */
		if (cinfo->optimize_coding)
			master->pass_type = huff_opt_pass;
		master->scan_number++;
		break;
	}
	
	master->pass_number++;
}

METHODDEF(void)
write_file_trailer (j_compress_ptr cinfo)
{
	emit_marker(cinfo, M_EOI);
}

METHODDEF(void)
term_destination (j_compress_ptr cinfo)
{
	my_dest_ptr dest = (my_dest_ptr) cinfo->dest;
//	size_t datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;
	
	/* Write any data remaining in the buffer */
//	if (datacount > 0)
//	{
		(*DumpBuffFunction)(dest->buffer, dest->pub.free_in_buffer, TRUE);
//		if (JFWRITE(dest->outfile, dest->buffer, datacount) != datacount)
//			ERREXIT(cinfo, JERR_FILE_WRITE);
//	}
//	fflush(dest->outfile);
	/* Make sure we wrote the output file OK */
//	if (ferror(dest->outfile))
//		ERREXIT(cinfo, JERR_FILE_WRITE);
}

METHODDEF(void)
free_pool (j_common_ptr cinfo, int pool_id)
{
	my_mem_ptr mem = (my_mem_ptr) cinfo->mem;
	small_pool_ptr shdr_ptr;
	large_pool_ptr lhdr_ptr;
	size_t space_freed;
	
	if (pool_id < 0 || pool_id >= JPOOL_NUMPOOLS)
		ERREXIT1(cinfo, JERR_BAD_POOL_ID, pool_id);	/* safety check */
	
#ifdef MEM_STATS
	if (cinfo->err->trace_level > 1)
		print_mem_stats(cinfo, pool_id); /* print pool's memory usage statistics */
#endif
	
	/* If freeing IMAGE pool, close any virtual arrays first */
	if (pool_id == JPOOL_IMAGE) {
		jvirt_sarray_ptr sptr;
		jvirt_barray_ptr bptr;
		
		for (sptr = mem->virt_sarray_list; sptr != NULL; sptr = sptr->next) {
			if (sptr->b_s_open) {	/* there may be no backing store */
				sptr->b_s_open = FALSE;	/* prevent recursive close if error */
				(*sptr->b_s_info.close_backing_store) (cinfo, & sptr->b_s_info);
			}
		}
		mem->virt_sarray_list = NULL;
		for (bptr = mem->virt_barray_list; bptr != NULL; bptr = bptr->next) {
			if (bptr->b_s_open) {	/* there may be no backing store */
				bptr->b_s_open = FALSE;	/* prevent recursive close if error */
				(*bptr->b_s_info.close_backing_store) (cinfo, & bptr->b_s_info);
			}
		}
		mem->virt_barray_list = NULL;
	}
	
	/* Release large objects */
	lhdr_ptr = mem->large_list[pool_id];
	mem->large_list[pool_id] = NULL;
	
	while (lhdr_ptr != NULL) {
		large_pool_ptr next_lhdr_ptr = lhdr_ptr->hdr.next;
		space_freed = lhdr_ptr->hdr.bytes_used +
			lhdr_ptr->hdr.bytes_left +
			SIZEOF(large_pool_hdr);
		jpeg_free_large(cinfo, (void FAR *) lhdr_ptr, space_freed);
		mem->total_space_allocated -= space_freed;
		lhdr_ptr = next_lhdr_ptr;
	}
	
	/* Release small objects */
	shdr_ptr = mem->small_list[pool_id];
	mem->small_list[pool_id] = NULL;
	
	while (shdr_ptr != NULL) {
		small_pool_ptr next_shdr_ptr = shdr_ptr->hdr.next;
		space_freed = shdr_ptr->hdr.bytes_used +
			shdr_ptr->hdr.bytes_left +
			SIZEOF(small_pool_hdr);
		jpeg_free_small(cinfo, (void *) shdr_ptr, space_freed);
		mem->total_space_allocated -= space_freed;
		shdr_ptr = next_shdr_ptr;
	}
}

GLOBAL(void)
jpeg_free_large (j_common_ptr cinfo, void FAR * object, size_t sizeofobject)
{
	free(object);
}

METHODDEF(void)
self_destruct (j_common_ptr cinfo)
{
	int pool;
	
	/* Close all backing store, release all memory.
	* Releasing pools in reverse order might help avoid fragmentation
	* with some (brain-damaged) malloc libraries.
	*/
	for (pool = JPOOL_NUMPOOLS-1; pool >= JPOOL_PERMANENT; pool--) {
		free_pool(cinfo, pool);
	}
	
	/* Release the memory manager control block too. */
	jpeg_free_small(cinfo, (void *) cinfo->mem, SIZEOF(my_memory_mgr));
	cinfo->mem = NULL;		/* ensures I will be called only once */
	
//	jpeg_mem_term(cinfo);		/* system-dependent cleanup */
}




















