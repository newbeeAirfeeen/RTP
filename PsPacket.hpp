/*
copyright (z) hzohao
all rights reserved. 
2020/12/22 --  
  
  
  filename    : PsPacket.hpp
  date	      : 2020/12/22
  author	  : oaho(shenhao)
  description : PS对H264做了封装,每个IDR NALU前一般都会包含SPS, PPS等NALU。因此将SPS,PPS的IDR的
	  			NALU封装成一个PS包，包括PS头，然后加上PS system header, PS system map, PES header
	  			+ H264 raw data. 所以一个IDR NALU的PS包由外到内的顺序：

	  			PSHeader -- PS System Header -- PS System Map -- PES header -- (H264 raw data ,可能含有其他类型)
	  			如果要把音频Audio打包进PS封装，将数据加上PES header放到视频PES后。顺序：
	  			PS = PS头 -- PES(video) -- PES(audio).然后最后用RTP封装发送
	 
	
				以下是对PS流格式的数据定义。
				PS头(PS header)
				以下各个字段的说明均来自 https://www.cnblogs.com/lihaiping/p/4181607.html
 					   					 https://blog.csdn.net/chen495810242/article/details/39207305
			    部分数据定义代码参考自： https://www.oschina.net/code/snippet_99626_23737
*/

/*  PS HEADER 包头大小为 14个字节。可以跳过*/
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
	 unsigned char  pack_start_code[4];			            /* 一个PS包的起始码(32bit), 值为0x000001BA */
#ifdef LITTLE_ENDIAN
	 /* 小端序是另外一种情况 */
	 /* 以下小端序1字节内位域为倒序*/
	 bit			system_clock_reference2_2 : 2;	        /* 从15bit中取了两个字节过来*/
	 bit			marker_bit : 1;					        /* always set */
	 bit			system_clock_reference : 3;		        /* SCR */
	 bit			zerone : 2;						        /* 值一定是01,占据2bit */

	 bit			system_clock_reference2 : 8;
	 /* 小端序的1字节内倒序*/
	 bit			system_clock_reference3_1 : 2;	        /* SCR3部分的 取过来 */
	 bit			marker_bit_2 : 1;				        /* always set */
	 bit			system_clock_reference2_3 : 5;		    /* SCR2部分的5 bit*/

	 bit			system_clock_reference3_2 : 8;
	 /* 小端序的1字节内倒序 */
	 bit			system_clock_reference_extension1 : 2;  /* 取scrx中的两位 */
	 bit            marker_bit_3 : 1;					    /* always set 1  */
	 bit            system_clock_reference_3_3 : 5;		    /* scr3的第三部分5bit */

	 bit			marker_bit_4 : 1;					    /* always set 1 */
	 bit			system_clock_reference_extension1_2 : 7;/* 取scrx中的7位 */

	 bit			program_mux_rate_1 : 8;			        /* 节目流率的第一部分 */
	 bit			program_mux_rate_1_2 : 8;			    /* 节目流率的第二部分 */

	 bit			marker_bit_6 : 1;
	 bit			marker_bit_5 : 1;
	 bit			program_mux_rate_1_3 : 6;			    /* 节目流率的第三部分 */
	 /* 小端序内1字节的倒序 */
	 bit			pack_stuffing_length : 3;			    /* 3位整数，规定该字段后填充字节的个数 */
	 bit			reserved : 5;

	 /*bit			stuffing_byte : 8;*/				    /* 扩展字段*/
#else
	 bit			zerone : 2;					            /* 值一定是01,占据2 bit */
	 bit			system_clock_reference : 3;	            /* 系统时钟参考字段 占3 bit */
	 bit			marker_bit : 1;				            /* always set */
	 bit			system_clock_reference2_2 : 2;

	 bit			system_clock_reference2 : 8;            /* 系统时钟参考字段 15 bit  */

	 bit			system_clock_reference2_3 : 5;
	 bit            marker_bit_2 : 1;				        /* always set */
	 bit			system_clock_reference3_1 : 2;

	 bit			system_clock_reference3 : 8;            /* 系统时钟参考字段 15 bit  */

	 bit			system_clock_reference3_2 : 5;
	 bit			marker_bit_3 : 1;				        /* always set */
	 bit			system_clock_reference_extension1 : 2;  /* 系统时钟参考字段 15 bit  */

	 bit			system_clock_reference_extension1_2 : 7;
	 bit			marker_bit_4 : 1;				        /* always set */

	 bit			program_mux_rate_1 : 8;			        /* 视频码率一个22位整数，规定P-STD在包含该字段的包期间接收节目流的速率。其值以50字节/秒为单位。不允许取0值。该字段所表示的值用于在2.5.2中定义P-STD输入端的字节到达时间。该字段值在本标准中的节目多路复合流的不同包中取值可能不同。 */
	 bit			program_mux_rate_2 : 8;

	 bit            program_mux_rate_3 : 6;
	 bit			marker_bit_5 : 1;				        /* always set */
	 bit			marker_bit_6 : 1;				        /* always set */

	 bit			reserved : 5;
	 bit			pack_stuffing_length : 3;		        /* 3位整数，规定该字段后填充字节的个数 */

	 bit			stuffing_byte : 8;				        /* 8位字段，取值恒为'1111 1111'。该字段能由编码器插入，例如为了满足通道的要求。
															  它由解码器丢弃。在每个包标题中最多只允许有7个填充字节。前5位丢弃，后三位表示后面所跟的长度(7字节)*/
#endif
};
/* 读取 0x000000BB后的两个字节为当前系统头的长度，直接跳过即可*/
struct PS_SYSTEM_HEADER
{
	 unsigned  char	system_header_start_code[4];		    /* PS系统头的起始码：0x000000BB*/
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

	 bit			rate_bound : 8;				           /* 22位字段，取值不小于编码在节目流的任何包中的program_mux_rate字段的最大值。该字段可被解码器用于估计是否有能力对整个流解码。 */
	 bit            rate_bound_1 : 8;

	 bit			rate_bound_2 : 6;
	 bit			marker_bit_2 : 1;
	 bit			audio_bound : 1;				       /* 6位字段，取值是在从0到32的闭区间中的整数，且不小于节目流中解码过程同时活动的GB/T XXXX.3和GB/T AAAA.3音频流的最大数目。在本小节中，若STD缓冲区非空或展现单元正在P-STD模型中展现，则GB/T XXXX.3和GB/T AAAA.3音频流的解码过程是活动的。*/

	 bit			audio_bound_1 : 5;
	 bit			fixed_flag : 1;				           /* 1位标志位。置'1'时表示比特率恒定的操作；置'0'时，表示操作的比特率可变。在恒定比特率的操作期间，复合的GB/T XXXX.1流中的system_clock_reference字段值应遵从下面的线性公式: SCR_base(i)＝((c1×i＋c2) DIV 300) % 2的33次方。SCR_ext(i)＝((c1×i＋c2) DIV 300) % 300*/
	 bit			csps_flag : 1;						   /* 1位字段。置'1'时，节目流符合2.7.9中定义的限制*/
	 bit            system_audio_lock_flag : 1;			   /* 表示在系统目标解码器的音频采样率和system_clock_frequency之间存在规定的比率*/

	 bit			system_video_lock_flag : 1;			   /* 表示在系统目标解码器的视频帧速率和system_clock_frequency之间存在规定的比率。*/
	 bit			marker_bit_3 : 1;
	 bit			vedio_bound : 5;					   /* 取值是在从0到16的闭区间中的整数且不小于节目流中解码过程同时活动的GB/T XXXX.2和GB/T AAAA.2流的最大数目*/
	 bit			packet_rate_restriction_flag : 1;	   /* 若CSPS标识为'1'，则该字段表示2.7.9中规定的哪个限制适用于分组速率。若CSPS标识为'0'，则该字段的含义未定义。*/

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
/* 对于这个字段的解析，我们需要取值0x000001BC的位串，指出节目流映射的开始，暂时不需要处理，读取Header Length直接跳过即可，如果需要解析流编码类型，必须详细解析这个字段。 */
struct PS_SYSTEM_MAP_HEADER
{
	 unsigned char system_map_header_start_code[4];		    /* 32bit, 其中最后一个元素包含了 map_stream_id */
	 unsigned char header_length[2];						/* 映射流长度 2个字节 */
#ifdef LITTLE_ENDIAN
	 bit		   program_stream_map_version : 5;			/* 5位字段，表示整个节目流映射的版本号。一旦节目流映射的定义发生变化，该字段将递增1，并对32取模。在current_next_indicator为'1'时，该字段应该是当前适用的节目流映射的版本号；在current_next_indicator为'0'时，该字段应该是下一个适用的节目流映射的版本号*/
	 bit		   reserved : 2;					        /* 保留 */
	 bit		   current_next_indicator : 1;              /* 1位字段。置'1'时表示传送的节目流映射当前是可用的。置'0'时表示传送的节目流映射还不可用，但它将是下一个生效的表。*/

	 bit		   marker_bit : 1;
	 bit		   reserved_2 : 7;

	 unsigned char program_stream_info_length[2];			/* 16位字段，指出紧跟在该字段后的描述符的总长度。*/
	 unsigned char elementary_stream_map_length[2];			/* 16位字段，指出在该节目流映射中的所有基本流信息的字节长度*/
															/*
																 8位字段，根据表2-29规定了流的类型。
																 该字段只能标志包含在PES分组中的基本流且取值不能为0x05。
																 1、MPEG-4 视频流： 0x10；
																 2、H.264 视频流： 0x1B；
																 3、SVAC 视频流： 0x80；
																 4、G.711 音频流： 0x90；
																 5、G.722.1 音频流： 0x92；
																 6、G.723.1 音频流： 0x93；
																 7、G.729 音频流： 0x99；
																 8、SVAC音频流： 0x9B。
															*/
	 bit		   stream_type : 8;
	 bit		   elementary_stream_id : 8;				/* 8位字段，指出该基本流所在PES分组的PES分组标题中stream_id字段的值。 */
	 unsigned char elementary_stream_info_length[2];		/* 16位字段，指出紧跟在该字段后的描述符的字节长度*/
	 unsigned char CRC_32[4];								/* 32位字段，它包含CRC值以在处理完整个节目流映射后在附录A中定义的解码器寄存器产生0输出值。*/
	 unsigned char reserved_3[16];
#endif // LITTLE_ENDIAN

};
/* PES 头部分 */
/* PES 头为6个字节 */
struct PES_HEADER
{
	 unsigned char pes_start_code_prefix[3];				/* 占24位 起始码  (0x000001)的位串*/
	 unsigned char stream_id;								/* 8bit, 在节目流中，它规定了基本流的号码和类型。*/
	 unsigned char PES_packet_length[2];					/* 16bit, PES分组长度字段（为跟在当前值之后的长度) */
															/* 值为0 表示PES 分组长度要么没有规定要么没有限制。这种情况只允许出现在有效负载包含来源于传输流分组中某个视频基本流的字节的PES 分组中。*/
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
	 bit PES_scrambling_control : 2;						/* 2位字段，表示PES分组有效负载的加扰方式。当加扰发生在PES层，PES分组标题，如果有可选字段的话也包括在内，不应被加扰*/
	 bit PES_priority : 1;									/* 1位字段，指示PES分组中有效负载的优先级。'1'表示PES分组中有效负载的优先级高于该字段为'0'的PES分组有效负载。多路复合器能使用该字段来区分安排基本流中数据的优先级。传输机制不应改动该字段。*/
	 bit data_alignment_indicator : 1;						/* 1位标志。置'1'时表示PES分组标题后紧跟着在2.6.10中的data_alignment_indicator所指出的视频起始码或音频同步字，如果有data_alignment_indicator描述符的话。若其值为'1'且无该描述符，则需要在表2-47和2-48中alignment_type '01'所表示的对齐。当值为'0'时，没有定义是否有任何此种的对齐*/
	 bit copyright : 1;										/* 1位字段。置'1'时表示相关PES分组有效负载的材料受到版权保护。当值为'0'时，没有定义该材料是否受到版权保护。2.6.24中描述的版权描述符与包含PES分组的基本流相关。若描述符作用于包含PES分组的材料，则版权标志被置为'1'*/
	 bit original_or_copy : 1;								/* 1位字段。置'1'时表示相关PES分组有效负载的内容是原始的；值为'0'表示相关PES分组有效负载的内容是一份拷贝。*/

	 bit PTS_DTS_flags : 2;									/* 2位字段。当值为'10'时，PTS字段应出现在PES分组标题中；当值为'11'时，PTS字段和DTS字段都应出现在PES分组标题中；当值为'00'时，PTS字段和DTS字段都不出现在PES分组标题中。值'01'是不允许的。*/
	 bit ESCR_flag : 1;										/* 1位标志。置'1'时表示ESCR基础和扩展字段出现在PES分组标题中；值为'0'表示没有ESCR字段。;*/
	 bit ES_rate_flag : 1;									/* 1位标志。置'1'时表示ES_rate字段出现在PES分组标题中；值为'0'表示没有ES_rate字段。*/
	 bit DSM_trick_mode_flag : 1;							/* 1位标志。置'1'时表示有8位特技方式字段；值为'0'表示没有该字段。*/
	 bit additional_copy_info_flag : 1;						/* 1位标志。置'1'时表示有附加拷贝信息字段；值为'0'表示没有该字段。*/
	 bit PES_CRC_flag : 1;									/* 1位标志。置'1'时表示CRC字段出现在PES分组标题中；值为'0'表示没有该字段。*/
	 bit PES_extension_flag : 1;							/* 1位标志。置'1'时表示PES分组标题中有扩展字段；值为'0'表示没有该字段。*/
#endif
	 bit PES_header_data_length : 8;						/* 8位字段。指出包含在PES分组标题中的可选字段和任何填充字节所占用的总字节数。该字段之前的字节指出了有无可选字段。*/
};

#pragma pack ()
enum PSStatus
{
	 ps_padding, //未知状态
	 ps_ps,      //ps状态
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
/* 判断是否是PS头 -- 0x000001BA */
bool inline is_ps_header(const PS_HEADER& ps)
{
	 //unsigned int* header = (unsigned int*)&ps.pack_start_code[0];
	 //return *header == 0x000001BA;
	 if (ps.pack_start_code[0] == 0 && ps.pack_start_code[1] == 0 && ps.pack_start_code[2] == 1 && ps.pack_start_code[3] == 0xBA)
		  return true;
	 return false;
}
/* 判断是否是系统头 -- 0x000001BB */
bool inline is_pss_header(const PS_SYSTEM_HEADER& ssh)
{
	 if (ssh.system_header_start_code[0] == 0 && ssh.system_header_start_code[1] == 0 && ssh.system_header_start_code[2] == 1 && ssh.system_header_start_code[3] == 0xBB)
		  return true;
	 return false;
}
/*   判断是否是节目映射头 -- 0x000001BC */
bool inline is_psm_header(const PS_SYSTEM_MAP_HEADER& psm)
{
	 if (psm.system_map_header_start_code[0] == 0 && psm.system_map_header_start_code[1] == 0 && psm.system_map_header_start_code[2] == 1 && psm.system_map_header_start_code[3] == 0xBC)
		  return true;
	 return false;
}
/*   判断是否是视频流 -- 0x000001E0 */
bool inline is_pes_video_header(const PES_HEADER& pes)
{
	 if (pes.pes_start_code_prefix[0] == 0 && pes.pes_start_code_prefix[1] == 0 && pes.pes_start_code_prefix[2] == 1 && pes.stream_id == 0xE0)
		  return true;
	 return false;
}
/*   判断是否是音频流 -- 0x000001C0 */
bool inline is_pes_audio_header(const PES_HEADER& pes)
{
	 if (pes.pes_start_code_prefix[0] == 0 && pes.pes_start_code_prefix[1] == 0 && pes.pes_start_code_prefix[2] == 1 && pes.stream_id == 0xC0)
		  return true;
	 return false;
}
/*   判断是否是PES头 0x000001E0 | 0x000001C0 */
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
/*   判断PES荷载的类型 */
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
* 1. 是否包含数据
* 2. 下一个ps状态
* 3. 数据指针
* 4. 数据长度
*/
using PES_Tuple  = std::tuple<bool, PSStatus, PES_HEADER* >;
/*
_1 是否包含数据
_2 数据类型
_3 PTS时间戳
_4 DTS时间戳
_5 数据指针
_6 数据长度
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
						 m_status = ps_psm;//冲掉s_sh状态
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
			   //寻找下一个pes 或者 ps
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
	 PSStatus      m_status;                     //当前状态
	 char          m_pPSBuffer[MAX_PS_LENGTH];   //PS缓冲区
	 unsigned int  m_nPSWrtiePos;                //PS写入位置
	 unsigned int  m_nPESIndicator;              //PES指针
private:
	 char          m_pPESBuffer[MAX_PES_LENGTH]; //PES缓冲区
	 unsigned int  m_nPESLength;                 //PES数据长度
private:
	 PS_HEADER* m_ps;							 //PS头
	 PS_SYSTEM_HEADER* m_sh;                     //系统头
	 PS_SYSTEM_MAP_HEADER* m_psm;                //节目流头
	 PES_HEADER* m_pes;                          //PES头
private:
	 char         m_pESBuffer[MAX_ES_LENGTH];    //裸码流
	 unsigned int m_nESLength;                   //裸码流长度
};

#endif