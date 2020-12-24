#pragma once
#ifndef PS_PARSER_H_
#define PS_PARSER_H_
#include"PsPacket.hpp"
#include<stdio.h>
#include<assert.h>
#include<stdlib.h>
#include<string.h>
#include<iostream>
using namespace std;

#define BUFFER_MAX_MACRO 4200
namespace oaho
{
	 namespace Media
	 {
		  class RTP;
		  class PSPacket final
		  {
			   friend class RTP;
		  public:
			   PSPacket(const char* pathname);
			   ~PSPacket();
			   
		  private:
			   FILE						*fp;					 /* 文件指针 */
			   bool						eof;					 /* 是否已经达到尾部*/
		  private:
			   void						read_ps_data_to_buff(char* buff, unsigned short* buff_size);
			   /* 解析PS格式文件头 */
			   /* 此函数必须要缓冲区中的第一个字节为0x000001BA*/
			   bool						parse_ps_header(const char* buff, int position, PSPacket& pspkg);
		  private:
			   PS_HEADER				ps_header;
			   PS_SYSTEM_HEADER			ps_ssh;
			   PS_SYSTEM_MAP_HEADER		ps_map_header;
			   PES_HEADER				pes_header;
			   OPTIONAL_PES_HEADER		pes_optional_header;
		  };
	 }
}
#endif