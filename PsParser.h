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
			   FILE						*fp;					 /* �ļ�ָ�� */
			   bool						eof;					 /* �Ƿ��Ѿ��ﵽβ��*/
		  private:
			   void						read_ps_data_to_buff(char* buff, unsigned short* buff_size);
			   /* ����PS��ʽ�ļ�ͷ */
			   /* �˺�������Ҫ�������еĵ�һ���ֽ�Ϊ0x000001BA*/
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