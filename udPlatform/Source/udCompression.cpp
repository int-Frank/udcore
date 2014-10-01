// To prevent collisions with other apps using miniz
#define mz_adler32 udComp_adler32
#define mz_crc32 udComp_crc32
#define mz_free udComp_free                                    
#define mz_version  udComp_version 
#define mz_deflateEnd udComp_deflateEnd                              
#define mz_deflateBound udComp_deflateBound                            
#define mz_compressBound udComp_compressBound                           
#define mz_inflateInit2 udComp_inflateInit2                            
#define mz_inflateInit udComp_inflateInit                             
#define mz_inflateEnd udComp_inflateEnd                              
#define mz_error udComp_error                                   
#define tinfl_decompress udCompTInf_decompress                           
#define tinfl_decompress_mem_to_heap udCompTInf_decompress_mem_to_heap               
#define tinfl_decompress_mem_to_mem udCompTInf_decompress_mem_to_mem                
#define tinfl_decompress_mem_to_callback udCompTInf_decompress_mem_to_callback           
#define tdefl_compress udCompTDefl_compress                             
#define tdefl_compress_buffer udCompTDefl_compress_buffer                      
#define tdefl_init udCompTDefl_init                                 
#define tdefl_get_prev_return_status udCompTDefl_get_prev_return_status               
#define tdefl_get_adler32 udCompTDefl_get_adler32                          
#define tdefl_compress_mem_to_output udCompTDefl_compress_mem_to_output               
#define tdefl_compress_mem_to_heap udCompTDefl_compress_mem_to_heap                 
#define tdefl_compress_mem_to_mem udCompTDefl_compress_mem_to_mem                  
#define tdefl_create_comp_flags_from_zip_params udCompTDefl_create_comp_flags_from_zip_params    
#define tdefl_write_image_to_png_file_in_memory_ex udCompTDefl_write_image_to_png_file_in_memory_ex
#define tdefl_write_image_to_png_file_in_memory udCompTDefl_write_image_to_png_file_in_memory   
#define mz_deflateInit2 udComp_deflateInit2                           
#define mz_deflateReset udComp_deflateReset                           
#define mz_deflate udComp_deflate                                
#define mz_inflate udComp_inflate                                
#define mz_uncompress udComp_uncompress                             
#define mz_deflateInit udComp_deflateInit                            
#define mz_compress2 udComp_compress2                              
#define mz_compress udComp_compress                               


#define MINIZ_NO_ARCHIVE_APIS
#define MINIZ_NO_TIME
#define MINIZ_NO_MALLOC 
#include "miniz.c"

#define WINDOW_BITS (-MZ_DEFAULT_WINDOW_BITS)
#include "udPlatform.h"

static void *MiniZCompressor_Alloc(void *pOpaque, size_t items, size_t size);
static void MiniZCompressor_Free(void *pOpaque, void *address);

// ----------------------------------------------------------------------------
// Author: David Ely, September 2014
struct udMiniZCompressor
{
  tdefl_compressor compressor;
  mz_stream *pStream;
};

// ****************************************************************************
// Author: David Ely, September 2014
udResult udMiniZCompressor_Create(udMiniZCompressor **ppCompressor)
{
  if (ppCompressor == nullptr)
  {
    return udR_InvalidParameter_;
  }
  udMiniZCompressor *pCompressor = udAllocType(udMiniZCompressor, 1, udAF_None);

  if (!pCompressor)
  {
    return udR_MemoryAllocationFailure;
  }
  
  pCompressor->pStream = nullptr;

  *ppCompressor = pCompressor;
  return udR_Success;
}

// ----------------------------------------------------------------------------
// Author: David Ely, September 2014
udResult udMiniZCompressor_InitStream(udMiniZCompressor *pCompressor, void *pDestBuffer, size_t size)
{
  if (!size || size > 0xFFFFFFFFU || pDestBuffer == nullptr) 
  {
    return udR_InvalidParameter_;
  }

  int status;
  pCompressor->pStream = udAllocType(mz_stream, 1, udAF_Zero);

  pCompressor->pStream->next_out = (uint8_t*)pDestBuffer;
  pCompressor->pStream->avail_out = (mz_uint32)size;
  pCompressor->pStream->zalloc = MiniZCompressor_Alloc;
  pCompressor->pStream->zfree = MiniZCompressor_Free;
  pCompressor->pStream->opaque = (void*)pCompressor;

  status = udComp_deflateInit2(pCompressor->pStream, MZ_BEST_COMPRESSION, MZ_DEFLATED, WINDOW_BITS, 9, MZ_DEFAULT_STRATEGY);
  if (status != MZ_OK) 
  {
    return udR_Failure_;
  }

  return udR_Success;
}

// ----------------------------------------------------------------------------
// Author: David Ely, September 2014
udResult udMiniZCompressor_DeflateStream(udMiniZCompressor *pCompressor, void *pStream, size_t size, size_t *pCompressedSize)
{
  if (!size || size > 0xFFFFFFFFU || pStream == nullptr) 
  {
    return udR_InvalidParameter_;
  }

  pCompressor->pStream->next_in = (const uint8_t*)pStream;
  pCompressor->pStream->avail_in = (mz_uint32)size;

  int statusCheck;
  int flush;
  if (pCompressedSize)
  {
    statusCheck = MZ_STREAM_END;
    flush = MZ_FINISH;
  }
  else
  {
    statusCheck = MZ_NO_FLUSH;
    flush = MZ_OK;
  }

  int status = udComp_deflate(pCompressor->pStream, flush);
  if (status != statusCheck)
  {
    udComp_deflateEnd(pCompressor->pStream);
    return udR_Failure_;
  }

  if (pCompressedSize)
  {
    *pCompressedSize = pCompressor->pStream->total_out;
    int status = udComp_deflateEnd(pCompressor->pStream);
    if (status != MZ_OK)
    {
      return udR_Failure_;
    }
  }
  return udR_Success;
}

// ****************************************************************************
// Author: David Ely, September 2014
udResult udMiniZCompressor_Destroy(udMiniZCompressor **ppCompressor)
{
  if (ppCompressor == nullptr)
  {
    return udR_InvalidParameter_;
  }

  udFree((*ppCompressor)->pStream);

  udFree(*ppCompressor);
  return udR_Success;
}

// ****************************************************************************
// Author: David Ely, September 2014
udResult udMiniZCompressor_Deflate(udMiniZCompressor *pCompressor, void *pDest, size_t destLength, const void *pSource, size_t sourceLength, size_t *pCompressedSize)
{
  if (pCompressor == nullptr || pDest == nullptr || pSource == nullptr)
  {
    return udR_InvalidParameter_;
  }

  int status;
  mz_stream stream;
  memset(&stream, 0, sizeof(stream));

  if ((sourceLength | destLength) > 0xFFFFFFFFU) 
  {
    return udR_InvalidParameter_;
  }

  stream.next_in = (const uint8_t*)pSource;
  stream.avail_in = (mz_uint32)sourceLength;
  stream.next_out = (uint8_t*)pDest;
  stream.avail_out = (mz_uint32)destLength;
  stream.zalloc = MiniZCompressor_Alloc;
  stream.zfree = MiniZCompressor_Free;
  stream.opaque = (void*)pCompressor;

  status = udComp_deflateInit2(&stream, MZ_BEST_COMPRESSION, MZ_DEFLATED, WINDOW_BITS, 9, MZ_DEFAULT_STRATEGY);
  if (status != MZ_OK) 
  {
    return udR_Failure_;
  }

  status = udComp_deflate(&stream, MZ_FINISH);
  if (status != MZ_STREAM_END)
  {
    udComp_deflateEnd(&stream);
    return udR_Failure_;
  }

  *pCompressedSize = stream.total_out;
  status = udComp_deflateEnd(&stream);
  if (status != MZ_OK)
  {
    return udR_Failure_;
  }
  return udR_Success;
}

// ----------------------------------------------------------------------------
// Author: David Ely, September 2014
static void *MiniZCompressor_Alloc(void *pOpaque, size_t IF_UDASSERT(items), size_t IF_UDASSERT(size))
{ 
  UDASSERT(items * size == (sizeof(udMiniZCompressor) - sizeof(mz_stream*)), "Error allocation for the incorrect size");
  return pOpaque; 
}

// ----------------------------------------------------------------------------
// Author: David Ely, September 2014
static void MiniZCompressor_Free(void *, void *) {}

// ----------------------------------------------------------------------------
// Author: David Ely, September 2014
struct udMiniZDecompressor
{
  tinfl_decompressor decompressor;
};

// ****************************************************************************
// Author: David Ely, September 2014
udResult udMiniZDecompressor_Create(udMiniZDecompressor **ppDecompressor)
{
  if (ppDecompressor == nullptr)
  {
    return udR_InvalidParameter_;
  }

  udMiniZDecompressor *pDecompressor = udAllocType(udMiniZDecompressor, 1, udAF_None);

  if (!pDecompressor)
  {
    return udR_MemoryAllocationFailure;
  }

  *ppDecompressor = pDecompressor;
  return udR_Success;
}

// ****************************************************************************
// Author: David Ely, September 2014
udResult udMiniZDecompressor_Destroy(udMiniZDecompressor **ppDecompressor)
{
  if (ppDecompressor == nullptr)
  {
    return udR_InvalidParameter_;
  }

  udFree(*ppDecompressor);
  return udR_Success;
}

// ****************************************************************************
// Author: David Ely, September 2014
udResult udMiniZDecompressor_Inflate(udMiniZDecompressor *pDecompressor, void *pDest, size_t destLength, const void *pSource, size_t sourceLength, size_t *pInflatedSize)
{
  if (pDecompressor == nullptr || pDest == nullptr || pSource == nullptr)
  {
    return udR_InvalidParameter_;
  }

  tinfl_init(&pDecompressor->decompressor);
  
  mz_uint8* pCurrentDest = (mz_uint8*)pDest; 
  tinfl_status status = udCompTInf_decompress(&pDecompressor->decompressor, (const mz_uint8*)pSource, &sourceLength, 
                                              (mz_uint8*)pDest, pCurrentDest, &destLength, 
                                              TINFL_FLAG_COMPUTE_ADLER32 | TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF);
  
  if (status != TINFL_STATUS_DONE)
  {
    return udR_Failure_;
  }

  if (pInflatedSize)
  {
    *pInflatedSize = destLength;
  }

  return udR_Success;
}

// ****************************************************************************
// Author: David Ely, September 2014
size_t udMiniZDecompressor_GetStructureSize()
{
  return sizeof(udMiniZDecompressor);
}

