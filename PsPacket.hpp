/*
copyright (z) hzohao
all rights reserved. 
2020/12/22 --  
  
  
  filename    : PsPacket.hpp
  date	      : 2020/12/22
  author	  : oaho(shenhao)
  description : PS��H264���˷�װ,ÿ��IDR NALUǰһ�㶼�����SPS, PPS��NALU����˽�SPS,PPS��IDR��
	  			NALU��װ��һ��PS��������PSͷ��Ȼ�����PS system header, PS system map, PES header
	  			+ H264 raw data. ����һ��IDR NALU��PS�����⵽�ڵ�˳��

	  			PSHeader -- PS System Header -- PS System Map -- PES header -- (H264 raw data ,���ܺ�����������)
	  			���Ҫ����ƵAudio�����PS��װ�������ݼ���PES header�ŵ���ƵPES��˳��
	  			PS = PSͷ -- PES(video) -- PES(audio).Ȼ�������RTP��װ����
	 
	
				�����Ƕ�PS����ʽ�����ݶ��塣
				PSͷ(PS header)
				���¸����ֶε�˵�������� https://www.cnblogs.com/lihaiping/p/4181607.html
 					   					 https://blog.csdn.net/chen495810242/article/details/39207305
			    �������ݶ������ο��ԣ� https://www.oschina.net/code/snippet_99626_23737
*/

/*  PS HEADER ��ͷ��СΪ 14���ֽڡ���������*/
#ifndef _PS_PACKET_HPP
#define _PS_PACKET_HPP
#include<tuple>
#include<iostream>
#include<stdio.h>
using bit = unsigned char;								    /* 8  bit */
using wbit = unsigned short;				                /* 16 bit */
using dwbit = unsigned int;						            /* 32 bit */
#define LITTLE_ENDIAN
#pragma pack (1)
struct PS_HEADER
{
	 unsigned char  pack_start_code[4];			            /* һ��PS������ʼ��(32bit), ֵΪ0x000001BA */
#ifdef LITTLE_ENDIAN
	 /* С����������һ����� */
	 /* ����С����1�ֽ���λ��Ϊ����*/
	 bit			system_clock_reference2_2 : 2;	        /* ��15bit��ȡ�������ֽڹ���*/
	 bit			marker_bit : 1;					        /* always set */
	 bit			system_clock_reference : 3;		        /* SCR */
	 bit			zerone : 2;						        /* ֵһ����01,ռ��2bit */

	 bit			system_clock_reference2 : 8;
	 /* С�����1�ֽ��ڵ���*/
	 bit			system_clock_reference3_1 : 2;	        /* SCR3���ֵ� ȡ���� */
	 bit			marker_bit_2 : 1;				        /* always set */
	 bit			system_clock_reference2_3 : 5;		    /* SCR2���ֵ�5 bit*/

	 bit			system_clock_reference3_2 : 8;
	 /* С�����1�ֽ��ڵ��� */
	 bit			system_clock_reference_extension1 : 2;  /* ȡscrx�е���λ */
	 bit            marker_bit_3 : 1;					    /* always set 1  */
	 bit            system_clock_reference_3_3 : 5;		    /* scr3�ĵ�������5bit */

	 bit			marker_bit_4 : 1;					    /* always set 1 */
	 bit			system_clock_reference_extension1_2 : 7;/* ȡscrx�е�7λ */

	 bit			program_mux_rate_1 : 8;			        /* ��Ŀ���ʵĵ�һ���� */
	 bit			program_mux_rate_1_2 : 8;			    /* ��Ŀ���ʵĵڶ����� */

	 bit			marker_bit_6 : 1;
	 bit			marker_bit_5 : 1;
	 bit			program_mux_rate_1_3 : 6;			    /* ��Ŀ���ʵĵ������� */
	 /* С������1�ֽڵĵ��� */
	 bit			pack_stuffing_length : 3;			    /* 3λ�������涨���ֶκ�����ֽڵĸ��� */
	 bit			reserved : 5;

	 /*bit			stuffing_byte : 8;*/				    /* ��չ�ֶ�*/
#else
	 bit			zerone : 2;					            /* ֵһ����01,ռ��2 bit */
	 bit			system_clock_reference : 3;	            /* ϵͳʱ�Ӳο��ֶ� ռ3 bit */
	 bit			marker_bit : 1;				            /* always set */
	 bit			system_clock_reference2_2 : 2;

	 bit			system_clock_reference2 : 8;            /* ϵͳʱ�Ӳο��ֶ� 15 bit  */

	 bit			system_clock_reference2_3 : 5;
	 bit            marker_bit_2 : 1;				        /* always set */
	 bit			system_clock_reference3_1 : 2;

	 bit			system_clock_reference3 : 8;            /* ϵͳʱ�Ӳο��ֶ� 15 bit  */

	 bit			system_clock_reference3_2 : 5;
	 bit			marker_bit_3 : 1;				        /* always set */
	 bit			system_clock_reference_extension1 : 2;  /* ϵͳʱ�Ӳο��ֶ� 15 bit  */

	 bit			system_clock_reference_extension1_2 : 7;
	 bit			marker_bit_4 : 1;				        /* always set */

	 bit			program_mux_rate_1 : 8;			        /* ��Ƶ����һ��22λ�������涨P-STD�ڰ������ֶεİ��ڼ���ս�Ŀ�������ʡ���ֵ��50�ֽ�/��Ϊ��λ��������ȡ0ֵ�����ֶ�����ʾ��ֵ������2.5.2�ж���P-STD����˵��ֽڵ���ʱ�䡣���ֶ�ֵ�ڱ���׼�еĽ�Ŀ��·�������Ĳ�ͬ����ȡֵ���ܲ�ͬ�� */
	 bit			program_mux_rate_2 : 8;

	 bit            program_mux_rate_3 : 6;
	 bit			marker_bit_5 : 1;				        /* always set */
	 bit			marker_bit_6 : 1;				        /* always set */

	 bit			reserved : 5;
	 bit			pack_stuffing_length : 3;		        /* 3λ�������涨���ֶκ�����ֽڵĸ��� */

	 bit			stuffing_byte : 8;				        /* 8λ�ֶΣ�ȡֵ��Ϊ'1111 1111'�����ֶ����ɱ��������룬����Ϊ������ͨ����Ҫ��
															  ���ɽ�������������ÿ�������������ֻ������7������ֽڡ�ǰ5λ����������λ��ʾ���������ĳ���(7�ֽ�)*/
#endif
};
/* ��ȡ 0x000000BB��������ֽ�Ϊ��ǰϵͳͷ�ĳ��ȣ�ֱ����������*/
struct PS_SYSTEM_HEADER
{
	 unsigned  char	system_header_start_code[4];		    /* PSϵͳͷ����ʼ�룺0x000000BB*/
	 unsigned  char	header_length[2];
#ifdef LITTLE_ENDIAN
	 
	 bit			rate_bound_1 : 7;
	 bit			marker_bit   : 1;

	 bit			rate_bound_2 : 8;

	 bit			marker_bit2  : 1;
	 bit			rate_bound_3 : 7;

	 bit			csps_flag   : 1;
	 bit			fixed_flag  : 1;
	 bit			audio_bound : 6;

	 bit			video_bound  : 5;
	 bit			marker_bit_3 : 1;
	 bit			system_video_lock_flag  : 1;
	 bit			system_audio_lock_flag  : 1;

	 bit			reserved_bits : 7;
	 bit			packet_rate_restriction : 1;

	 bit			stream_id : 8;

	 bit			PSTD_buffer_size_bound  : 5;
	 bit			PSTD_buffer_bound_scale : 1;
	 bit			zerone : 2;

	 bit			PSTD_buffer_size_bound2 : 8;

	 //unsigned		char reserved[6];
#else
	 bit			marker_bit : 1;				           /* always set */

	 bit			rate_bound : 8;				           /* 22λ�ֶΣ�ȡֵ��С�ڱ����ڽ�Ŀ�����κΰ��е�program_mux_rate�ֶε����ֵ�����ֶοɱ����������ڹ����Ƿ������������������롣 */
	 bit            rate_bound_1 : 8;

	 bit			rate_bound_2 : 6;
	 bit			marker_bit_2 : 1;
	 bit			audio_bound : 1;				       /* 6λ�ֶΣ�ȡֵ���ڴ�0��32�ı������е��������Ҳ�С�ڽ�Ŀ���н������ͬʱ���GB/T XXXX.3��GB/T AAAA.3��Ƶ���������Ŀ���ڱ�С���У���STD�������ǿջ�չ�ֵ�Ԫ����P-STDģ����չ�֣���GB/T XXXX.3��GB/T AAAA.3��Ƶ���Ľ�������ǻ�ġ�*/

	 bit			audio_bound_1 : 5;
	 bit			fixed_flag : 1;				           /* 1λ��־λ����'1'ʱ��ʾ�����ʺ㶨�Ĳ�������'0'ʱ����ʾ�����ı����ʿɱ䡣�ں㶨�����ʵĲ����ڼ䣬���ϵ�GB/T XXXX.1���е�system_clock_reference�ֶ�ֵӦ�����������Թ�ʽ: SCR_base(i)��((c1��i��c2) DIV 300) % 2��33�η���SCR_ext(i)��((c1��i��c2) DIV 300) % 300*/
	 bit			csps_flag : 1;						   /* 1λ�ֶΡ���'1'ʱ����Ŀ������2.7.9�ж��������*/
	 bit            system_audio_lock_flag : 1;			   /* ��ʾ��ϵͳĿ�����������Ƶ�����ʺ�system_clock_frequency֮����ڹ涨�ı���*/

	 bit			system_video_lock_flag : 1;			   /* ��ʾ��ϵͳĿ�����������Ƶ֡���ʺ�system_clock_frequency֮����ڹ涨�ı��ʡ�*/
	 bit			marker_bit_3 : 1;
	 bit			vedio_bound : 5;					   /* ȡֵ���ڴ�0��16�ı������е������Ҳ�С�ڽ�Ŀ���н������ͬʱ���GB/T XXXX.2��GB/T AAAA.2���������Ŀ*/
	 bit			packet_rate_restriction_flag : 1;	   /* ��CSPS��ʶΪ'1'������ֶα�ʾ2.7.9�й涨���ĸ����������ڷ������ʡ���CSPS��ʶΪ'0'������ֶεĺ���δ���塣*/

	 bit			reserved_bits : 7;
	 bit			stream_id : 1;

	 bit			stream_id_2 : 7;
	 bit			one : 1;			/* always set */

	 bit			one_1 : 1;			/* always set */
	 bit			P_STD_buffer_bound_scale : 1;
	 bit			P_STD_buffer_size_bound : 6;

	 bit			P_STD_buffer_size_bound_2 : 7;
#endif
};
/* ��������ֶεĽ�����������Ҫȡֵ0x000001BC��λ����ָ����Ŀ��ӳ��Ŀ�ʼ����ʱ����Ҫ������ȡHeader Lengthֱ���������ɣ������Ҫ�������������ͣ�������ϸ��������ֶΡ� */
struct PS_SYSTEM_MAP_HEADER
{
	 unsigned char system_map_header_start_code[4];		    /* 32bit, �������һ��Ԫ�ذ����� map_stream_id */
	 unsigned char header_length[2];						/* ӳ�������� 2���ֽ� */
#ifdef LITTLE_ENDIAN
	 bit		   program_stream_map_version : 5;			/* 5λ�ֶΣ���ʾ������Ŀ��ӳ��İ汾�š�һ����Ŀ��ӳ��Ķ��巢���仯�����ֶν�����1������32ȡģ����current_next_indicatorΪ'1'ʱ�����ֶ�Ӧ���ǵ�ǰ���õĽ�Ŀ��ӳ��İ汾�ţ���current_next_indicatorΪ'0'ʱ�����ֶ�Ӧ������һ�����õĽ�Ŀ��ӳ��İ汾��*/
	 bit		   reserved : 2;					        /* ���� */
	 bit		   current_next_indicator : 1;              /* 1λ�ֶΡ���'1'ʱ��ʾ���͵Ľ�Ŀ��ӳ�䵱ǰ�ǿ��õġ���'0'ʱ��ʾ���͵Ľ�Ŀ��ӳ�仹�����ã�����������һ����Ч�ı�*/

	 bit		   marker_bit : 1;
	 bit		   reserved_2 : 7;

	 unsigned char program_stream_info_length[2];			/* 16λ�ֶΣ�ָ�������ڸ��ֶκ�����������ܳ��ȡ�*/
	 unsigned char elementary_stream_map_length[2];			/* 16λ�ֶΣ�ָ���ڸý�Ŀ��ӳ���е����л�������Ϣ���ֽڳ���*/
															/*
																 8λ�ֶΣ����ݱ�2-29�涨���������͡�
																 ���ֶ�ֻ�ܱ�־������PES�����еĻ�������ȡֵ����Ϊ0x05��
																 1��MPEG-4 ��Ƶ���� 0x10��
																 2��H.264 ��Ƶ���� 0x1B��
																 3��SVAC ��Ƶ���� 0x80��
																 4��G.711 ��Ƶ���� 0x90��
																 5��G.722.1 ��Ƶ���� 0x92��
																 6��G.723.1 ��Ƶ���� 0x93��
																 7��G.729 ��Ƶ���� 0x99��
																 8��SVAC��Ƶ���� 0x9B��
															*/
	 bit		   stream_type : 8;
	 bit		   elementary_stream_id : 8;				/* 8λ�ֶΣ�ָ���û���������PES�����PES���������stream_id�ֶε�ֵ�� */
	 unsigned char elementary_stream_info_length[2];		/* 16λ�ֶΣ�ָ�������ڸ��ֶκ�����������ֽڳ���*/
	 unsigned char CRC_32[4];								/* 32λ�ֶΣ�������CRCֵ���ڴ�����������Ŀ��ӳ����ڸ�¼A�ж���Ľ������Ĵ�������0���ֵ��*/
	 unsigned char reserved_3[16];
#endif // LITTLE_ENDIAN

};
/* PES ͷ���� */
/* PES ͷΪ6���ֽ� */
struct PES_HEADER
{
	 unsigned char pes_start_code_prefix[3];				/* ռ24λ ��ʼ��  (0x000001)��λ��*/
	 unsigned char stream_id;								/* 8bit, �ڽ�Ŀ���У����涨�˻������ĺ�������͡�*/
	 unsigned char PES_packet_length[2];					/* 16bit, PES���鳤���ֶΣ�Ϊ���ڵ�ǰֵ֮��ĳ���) */
															/* ֵΪ0 ��ʾPES ���鳤��Ҫôû�й涨Ҫôû�����ơ��������ֻ�����������Ч���ذ�����Դ�ڴ�����������ĳ����Ƶ���������ֽڵ�PES �����С�*/
};

struct OPTIONAL_PES_HEADER {
#ifdef LITTLE_ENDIAN
	 bit original_or_copy : 1;
	 bit copyright : 1;
	 bit data_alignment_indicator : 1;
	 bit PES_priority : 1;
	 bit PES_scrambling_control : 2;
	 bit fix_bit : 2;

	 bit PES_extension_flag : 1;
	 bit PES_CRC_flag : 1;
	 bit additional_copy_info_flag : 1;
	 bit DSM_trick_mode_flag : 1;
	 bit ES_rate_flag : 1;
	 bit ESCR_flag : 1;
	 bit PTS_DTS_flags : 2;
#else
	 bit fix_bit : 2;
	 bit PES_scrambling_control : 2;						/* 2λ�ֶΣ���ʾPES������Ч���صļ��ŷ�ʽ�������ŷ�����PES�㣬PES������⣬����п�ѡ�ֶεĻ�Ҳ�������ڣ���Ӧ������*/
	 bit PES_priority : 1;									/* 1λ�ֶΣ�ָʾPES��������Ч���ص����ȼ���'1'��ʾPES��������Ч���ص����ȼ����ڸ��ֶ�Ϊ'0'��PES������Ч���ء���·��������ʹ�ø��ֶ������ְ��Ż����������ݵ����ȼ���������Ʋ�Ӧ�Ķ����ֶΡ�*/
	 bit data_alignment_indicator : 1;						/* 1λ��־����'1'ʱ��ʾPES���������������2.6.10�е�data_alignment_indicator��ָ������Ƶ��ʼ�����Ƶͬ���֣������data_alignment_indicator�������Ļ�������ֵΪ'1'���޸�������������Ҫ�ڱ�2-47��2-48��alignment_type '01'����ʾ�Ķ��롣��ֵΪ'0'ʱ��û�ж����Ƿ����κδ��ֵĶ���*/
	 bit copyright : 1;										/* 1λ�ֶΡ���'1'ʱ��ʾ���PES������Ч���صĲ����ܵ���Ȩ��������ֵΪ'0'ʱ��û�ж���ò����Ƿ��ܵ���Ȩ������2.6.24�������İ�Ȩ�����������PES����Ļ�������ء��������������ڰ���PES����Ĳ��ϣ����Ȩ��־����Ϊ'1'*/
	 bit original_or_copy : 1;								/* 1λ�ֶΡ���'1'ʱ��ʾ���PES������Ч���ص�������ԭʼ�ģ�ֵΪ'0'��ʾ���PES������Ч���ص�������һ�ݿ�����*/

	 bit PTS_DTS_flags : 2;									/* 2λ�ֶΡ���ֵΪ'10'ʱ��PTS�ֶ�Ӧ������PES��������У���ֵΪ'11'ʱ��PTS�ֶκ�DTS�ֶζ�Ӧ������PES��������У���ֵΪ'00'ʱ��PTS�ֶκ�DTS�ֶζ���������PES��������С�ֵ'01'�ǲ�����ġ�*/
	 bit ESCR_flag : 1;										/* 1λ��־����'1'ʱ��ʾESCR��������չ�ֶγ�����PES��������У�ֵΪ'0'��ʾû��ESCR�ֶΡ�;*/
	 bit ES_rate_flag : 1;									/* 1λ��־����'1'ʱ��ʾES_rate�ֶγ�����PES��������У�ֵΪ'0'��ʾû��ES_rate�ֶΡ�*/
	 bit DSM_trick_mode_flag : 1;							/* 1λ��־����'1'ʱ��ʾ��8λ�ؼ���ʽ�ֶΣ�ֵΪ'0'��ʾû�и��ֶΡ�*/
	 bit additional_copy_info_flag : 1;						/* 1λ��־����'1'ʱ��ʾ�и��ӿ�����Ϣ�ֶΣ�ֵΪ'0'��ʾû�и��ֶΡ�*/
	 bit PES_CRC_flag : 1;									/* 1λ��־����'1'ʱ��ʾCRC�ֶγ�����PES��������У�ֵΪ'0'��ʾû�и��ֶΡ�*/
	 bit PES_extension_flag : 1;							/* 1λ��־����'1'ʱ��ʾPES�������������չ�ֶΣ�ֵΪ'0'��ʾû�и��ֶΡ�*/
#endif
	 bit PES_header_data_length : 8;						/* 8λ�ֶΡ�ָ��������PES��������еĿ�ѡ�ֶκ��κ�����ֽ���ռ�õ����ֽ��������ֶ�֮ǰ���ֽ�ָ�������޿�ѡ�ֶΡ�*/
};

#pragma pack ()
enum PSStatus
{
	 ps_padding, //δ֪״̬
	 ps_ps,      //ps״̬
	 ps_ssh,
	 ps_psm,
	 ps_pes,
	 ps_pes_video,
	 ps_pes_audio
};
#ifndef AV_RB16
#   define AV_RB16(x)								  \
    ((((const unsigned char*)(x))[0] << 8) |          \
    ((const unsigned char*)(x))[1])
#endif
static inline unsigned __int64 ff_parse_pes_pts(const unsigned char* buf) {
	 return (unsigned __int64)(*buf & 0x0e) << 29 |
		  (AV_RB16(buf + 1) >> 1) << 15 |
		  AV_RB16(buf + 3) >> 1;
}
static unsigned __int64 get_pts( const OPTIONAL_PES_HEADER& option)
{
	 if (option.PTS_DTS_flags != 2 && option.PTS_DTS_flags != 3 && option.PTS_DTS_flags != 0)
	 {
		  return 0;
	 }
	 if ((option.PTS_DTS_flags & 2) == 2)
	 {
		  unsigned char* pts = (unsigned char*)&option + sizeof(OPTIONAL_PES_HEADER);
		  return ff_parse_pes_pts(pts);
	 }
	 return 0;
}
static unsigned __int64 get_dts(const OPTIONAL_PES_HEADER& option)
{
	 if (option.PTS_DTS_flags != 2 && option.PTS_DTS_flags != 3 && option.PTS_DTS_flags != 0)
	 {
		  return 0;
	 }
	 if ((option.PTS_DTS_flags & 3) == 3)
	 {
		  unsigned char* dts = (unsigned char*)&option + sizeof(OPTIONAL_PES_HEADER) + 5;
		  return ff_parse_pes_pts(dts);
	 }
	 return 0;
}
/* �ж��Ƿ���PSͷ -- 0x000001BA */
bool inline is_ps_header(const PS_HEADER& ps)
{
	 //unsigned int* header = (unsigned int*)&ps.pack_start_code[0];
	 //return *header == 0x000001BA;
	 if (ps.pack_start_code[0] == 0 && ps.pack_start_code[1] == 0 && ps.pack_start_code[2] == 1 && ps.pack_start_code[3] == 0xBA)
		  return true;
	 return false;
}
/* �ж��Ƿ���ϵͳͷ -- 0x000001BB */
bool inline is_pss_header(const PS_SYSTEM_HEADER& ssh)
{
	 if (ssh.system_header_start_code[0] == 0 && ssh.system_header_start_code[1] == 0 && ssh.system_header_start_code[2] == 1 && ssh.system_header_start_code[3] == 0xBB)
		  return true;
	 return false;
}
/*   �ж��Ƿ��ǽ�Ŀӳ��ͷ -- 0x000001BC */
bool inline is_psm_header(const PS_SYSTEM_MAP_HEADER& psm)
{
	 if (psm.system_map_header_start_code[0] == 0 && psm.system_map_header_start_code[1] == 0 && psm.system_map_header_start_code[2] == 1 && psm.system_map_header_start_code[3] == 0xBC)
		  return true;
	 return false;
}
/*   �ж��Ƿ�����Ƶ�� -- 0x000001E0 */
bool inline is_pes_video_header(const PES_HEADER& pes)
{
	 if (pes.pes_start_code_prefix[0] == 0 && pes.pes_start_code_prefix[1] == 0 && pes.pes_start_code_prefix[2] == 1 && pes.stream_id == 0xE0)
		  return true;
	 return false;
}
/*   �ж��Ƿ�����Ƶ�� -- 0x000001C0 */
bool inline is_pes_audio_header(const PES_HEADER& pes)
{
	 if (pes.pes_start_code_prefix[0] == 0 && pes.pes_start_code_prefix[1] == 0 && pes.pes_start_code_prefix[2] == 1 && pes.stream_id == 0xC0)
		  return true;
	 return false;
}
/*   �ж��Ƿ���PESͷ 0x000001E0 | 0x000001C0 */
bool inline is_pes_header(const PES_HEADER& pes)
{
	 if (pes.pes_start_code_prefix[0] == 0 && pes.pes_start_code_prefix[1] == 0 && pes.pes_start_code_prefix[2] == 1)
	 {
		  if (pes.stream_id == 0xC0 || pes.stream_id == 0xE0)
		  {
			   return true;
		  }
	 }
	 return false;
}
/*   �ж�PES���ص����� */
PSStatus inline pes_type(const PES_HEADER& pes)
{
	 if (pes.pes_start_code_prefix[0] == 0 && pes.pes_start_code_prefix[1] == 0 && pes.pes_start_code_prefix[2] == 1)
	 {
		  if (pes.stream_id == 0xC0)
		  {
			   return ps_pes_audio;
		  }
		  else if (pes.stream_id == 0xE0)
		  {
			   return ps_pes_video;
		  }
	 }
	 return ps_padding;
}

/*
* 1. �Ƿ��������
* 2. ��һ��ps״̬
* 3. ����ָ��
* 4. ���ݳ���
*/
using PES_Tuple  = std::tuple<bool, PSStatus, PES_HEADER* >;
/*
_1 �Ƿ��������
_2 ��������
_3 PTSʱ���
_4 DTSʱ���
_5 ����ָ��
_6 ���ݳ���
*/
using naked_tuple = std::tuple<bool, unsigned char, unsigned __int64, unsigned __int64, char*, unsigned int>;


const unsigned int MAX_PS_LENGTH  = 0xFFFF;
const unsigned int MAX_PES_LENGTH = 0xFFFF;
const unsigned int MAX_ES_LENGTH  = 0x100000;
class PSPacket_
{
public:
	 PSPacket_()
	 {
		  m_status = ps_padding;
		  m_nESLength = m_nPESIndicator = m_nPSWrtiePos = m_nPESLength = 0;
	 }
	 void PSWrite(char* pBuffer, unsigned int sz)
	 {
		  if (m_nPSWrtiePos + sz < MAX_PS_LENGTH)
		  {
			   memcpy((m_pPSBuffer + m_nPSWrtiePos), pBuffer, sz);
			   m_nPSWrtiePos += sz;
		  }
		  else
		  {
			   m_status = ps_padding;
			   m_nESLength = m_nPESIndicator = m_nPSWrtiePos = m_nPESLength = 0;
		  }

	 }
	 /*
	 void RTPWrite(char* pBuffer, unsigned int sz)
	 {
		  char* data = (pBuffer + sizeof(RTP_header_t));
		  unsigned int length = sz - sizeof(RTP_header_t);
		  if (m_nPSWrtiePos + length < MAX_PS_LENGTH)
		  {
			   memcpy((m_pPSBuffer + m_nPSWrtiePos), data, length);
			   m_nPSWrtiePos += length;
		  }
		  else
		  {
			   m_status = ps_padding;
			   m_nESLength = m_nPESIndicator = m_nPSWrtiePos = m_nPESLength = 0;
		  }
	 }*/
	 PES_Tuple pes_payload()
	 {
		  if (m_status == ps_padding)
		  {
			   for (; m_nPESIndicator < m_nPSWrtiePos; m_nPESIndicator++)
			   {
					m_ps = (PS_HEADER*)(m_pPSBuffer + m_nPESIndicator);
					if (is_ps_header(*m_ps))
					{
						 m_status = ps_ps;
						 break;
					}
			   }
		  }
		  if (m_status == ps_ps)
		  {
			   for (; m_nPESIndicator < m_nPSWrtiePos; m_nPESIndicator++)
			   {
					m_sh = (PS_SYSTEM_HEADER*)(m_pPSBuffer + m_nPESIndicator);
					m_pes = (PES_HEADER*)(m_pPSBuffer + m_nPESIndicator);
					if (is_pss_header(*m_sh))
					{
						 m_status = ps_ssh;
						 break;
					}
					else if (is_pes_header(*m_pes))
					{
						 m_status = ps_pes;
						 break;
					}
			   }
		  }
		  if (m_status == ps_ssh)
		  {
			   for (; m_nPESIndicator < m_nPSWrtiePos; m_nPESIndicator++)
			   {
					m_psm = (PS_SYSTEM_MAP_HEADER*)(m_pPSBuffer + m_nPESIndicator);
					m_pes = (PES_HEADER*)(m_pPSBuffer + m_nPESIndicator);
					if (is_psm_header(*m_psm))
					{
						 m_status = ps_psm;//���s_sh״̬
						 break;
					}
					if (is_pes_header(*m_pes))
					{
						 m_status = ps_pes;
						 break;
					}
			   }
		  }
		  if (m_status == ps_psm)
		  {
			   for (; m_nPESIndicator < m_nPSWrtiePos; m_nPESIndicator++)
			   {
					m_pes = (PES_HEADER*)(m_pPSBuffer + m_nPESIndicator);
					if (is_pes_header(*m_pes))
					{
						 m_status = ps_pes;
						 break;
					}
			   }
		  }
		  if (m_status == ps_pes)
		  {
			   //Ѱ����һ��pes ���� ps
			   //unsigned short PES_packet_length = ntohs(m_pes->PES_packet_length);
			  /*
			   if ((m_nPESIndicator + PES_packet_length + sizeof(PES_HEADER)) < m_nPSWrtiePos)
			   {
					char* next = (m_pPSBuffer + m_nPESIndicator + sizeof(PES_HEADER) + PES_packet_length);
					PES_HEADER* pes = (PES_HEADER*)next;
					PS_HEADER* ps = (PS_HEADER*)next;
					m_nPESLength = PES_packet_length + sizeof(PES_HEADER);
					memcpy(m_pPESBuffer, m_pes, m_nPESLength);
					if (is_pes_header(*pes) || is_ps_header(*ps))
					{
						 PSStatus status = ps_padding;
						 if (is_ps_header(*ps))
						 {
							  status = m_status = ps_ps;
						 }
						 else
						 {
							  status = pes_type(*pes);
						 }
						 int remain = m_nPSWrtiePos - (next - m_pPSBuffer);
						 memcpy(m_pPSBuffer, next, remain);
						 m_nPSWrtiePos = remain; m_nPESIndicator = 0;
						 m_ps = (PS_HEADER*)m_pPSBuffer;
						 m_pes = (PES_HEADER*)m_pPSBuffer;
						 return PES_Tuple(true, status, ((PES_HEADER*)m_pPESBuffer));
					}
					else
					{
						 m_status = ps_padding;
						 m_nPESIndicator = m_nPSWrtiePos = 0;
					}
			   }*/
		  }
		  return PES_Tuple(false, ps_padding, NULL);
	 }
	 naked_tuple naked_payload()
	 {
		 
		  naked_tuple tuple = naked_tuple(false, 0, 0, 0, NULL, 0);/*
		  do
		  {
			   PES_Tuple t = pes_payload();
			   if (!std::get<0>(t))
			   {
					break;
			   }
			   PSStatus status = std::get<1>(t);
			   PES_HEADER* pes = std::get<2>(t);
			   OPTIONAL_PES_HEADER* option = (OPTIONAL_PES_HEADER*)((char*)pes + sizeof(PES_HEADER));
			   if (option->PTS_DTS_flags != 2 && option->PTS_DTS_flags != 3 && option->PTS_DTS_flags != 0)
			   {
					break;
			   }
			   unsigned __int64 pts = get_pts(*option);
			   unsigned __int64 dts = get_dts(*option);
			   unsigned char stream_id = pes->stream_id;
			   unsigned short PES_packet_length = ntohs(pes->PES_packet_length);
			   char* pESBuffer = ((char*)option + sizeof(OPTIONAL_PES_HEADER) + option->PES_header_data_length);
			   int nESLength = PES_packet_length - (sizeof(OPTIONAL_PES_HEADER) + option->PES_header_data_length);
			   memcpy(m_pESBuffer + m_nESLength, pESBuffer, nESLength);
			   m_nESLength += nESLength;
			   if (stream_id == 0xE0 && (status == ps_ps || status == ps_pes_audio))
			   {
					tuple = naked_tuple(true, 0xE0, pts, dts, m_pESBuffer, m_nESLength);
					m_nESLength = 0;
			   }
			   else if (stream_id == 0xC0)
			   {
					tuple = naked_tuple(true, 0xC0, pts, dts, m_pESBuffer, m_nESLength);
					m_nESLength = 0;
			   }
		  } while (false);*/
		  return tuple;
	 }
private:
	 PSStatus      m_status;                     //��ǰ״̬
	 char          m_pPSBuffer[MAX_PS_LENGTH];   //PS������
	 unsigned int  m_nPSWrtiePos;                //PSд��λ��
	 unsigned int  m_nPESIndicator;              //PESָ��
private:
	 char          m_pPESBuffer[MAX_PES_LENGTH]; //PES������
	 unsigned int  m_nPESLength;                 //PES���ݳ���
private:
	 PS_HEADER* m_ps;							 //PSͷ
	 PS_SYSTEM_HEADER* m_sh;                     //ϵͳͷ
	 PS_SYSTEM_MAP_HEADER* m_psm;                //��Ŀ��ͷ
	 PES_HEADER* m_pes;                          //PESͷ
private:
	 char         m_pESBuffer[MAX_ES_LENGTH];    //������
	 unsigned int m_nESLength;                   //����������
};

#endif