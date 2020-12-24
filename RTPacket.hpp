#pragma once
#ifndef RTP_PACKET_H
#define RTP_PACKET_H
/* copyright (z) hzohao
 * all rights reserved.
 * 2020/12/22 --
 *
 *
 * filename  : RTPPacket.hpp
 * date	     : 2020/12/22
 * author	 : oaho(shenhao)
 *
 * descrition: 此文件是对RTP格式包的打包，RTP包的打包规则，此文件只给出单包形式和分片打包形式。
 *			   单包形式: 适合数据包小于MTU的时刻。
 *			   GB28181 对RTP 传输的数据负载类型有规定（参考GB28181 附录B），负载类型中96-127
 *			   RFC2250 建议96 表示PS 封装，建议97 为MPEG-4，建议98 为H264。
 *             即我们接收到的RTP 包首先需要判断负载类型，若负载类型为96，则采用PS 解复用，将音视频分开解码。
 *			   若负载类型为98，直接按照H264 的解码类型解码。
 *
 *			   分片形式: 网络层协议是IPv4,当数据包大于MTU(每一条链路都不一样，有些MTU 1500字节，
 *						 有些500字节)的时候，路由器会分包处理。比如3000字节的包，会分成2个包。
 *						 在处理的时候，尽量让路由器少做这些工作。IPv6不允许分片。
 *						 分FU-A,FU-B.用NAL单元类型28,29表示。
 *			   RTP头：(在没有CSRC的情况下，固定为12字节)
 *       			 	     			   0               1               2               3
 *							 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
 *							+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *							|V=2|P|X|  CC   |M|     PT      |       sequence number         |
 *							+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *							|                           timestamp                           |
 * 							+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *							|           synchronization source (SSRC) identifier            |
 *							+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 *							|            contributing source (CSRC) identifiers             |
 *							|                             ....                              |
 *							+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *					V  : RTP协议的版本号。当前版本为2
 *					P  : 填充标志，占1位。如果P=1，则在该报文的尾部填充一个或多个8位组(不是有效载荷)
 *					X  : 扩展标志，占1位。如果X=1，RTP报头有一个扩展报头部分。
 *					CC ：CSRC的计数器
 *					M  : 标记。如果是视频表示一帧的结束。音频，表示会话的开始
 *					PT ：有效载荷类型
 *					Seq: 序列号，可以用来标识RTP报文的序列号，可用来检测丢包。从0开始计算，每发送一个+1.
 *					TimeStamp: 时间戳反映了某一个八位组的采样时刻，接收者用这个计算延迟和延迟抖动。
 *					SSRC：同步信号源。占32位，参与同一视频会议的两个同步信号源不能有相同的SSRC。
 *					CCRC：特约信号源。占32位，可以有0-15个。表示所有特约信号源
 *			   
 *			   内容：(分为单个NAL包和分片单元)
 *					单个NAL包：
 *							               0			   1               2               3
 *							 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
 *			                +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *							|F|NRI|  Type   |                                               |
 *						    +-+-+-+-+-+-+-+-+											    |
 *							|                                                               |
 *							|				 Byte 2..n of a Single Nal unit				    |						    
 *							|																|
 *							|							   -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-|					
 *							|							   |	OPTIONAL RTP PADDING	    |
 *					        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *					此结构为打RTP单包，NAL单元必须只包含一个。意味聚合包和分片单元不可以按此形式打包。
 *                  NAL单元的第一字节和RTP荷载头第一个字节重合。
 *					
 *					分片单元(FU-A):
 *						                   0			   1               2               3
 *							 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
 *			                +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *							| FU indicator |   FU header   |                                |
 *						    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-								|			    
 *							|                                                               |
 *							|				 Byte 2..n of a Single Nal unit				    |						    
 *							|																|
 *							|							   -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-|					
 *							|							   |	OPTIONAL RTP PADDING	    |
 *					        +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *					分片只用于单个NAL单元中。NAL单元的一个分片由整数个连续的NAL单元字节组成。
 *					相同的NAL单元的分片必须使用递增的RTP序号连续顺序发送(第一个和最后一个RTP包中不能包含
 *					其他包).类似，NAL单元必须按照RTP顺序号的顺序装配。
 *					
 *					当一个NAL单元被分片运送在分片单元(FUs)中时，被引用为分片NAL单元。STAPs,MTAPs不可以被分片。 FUs不可以嵌套。 
 *					即, 一个FU 不可以包含另一个FU。运送FU的RTP时戳被设置成分片NAL单元的NALU时刻。
 * 
 * 
 *					FU indicator:
 *						    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *                          | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
 *                          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *                          | F |  NRI  |       Type        |
 *                          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 
 *					FU header:
 *						    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *                          | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
 *                          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *                          | S | E | R |       Type        |
 *                          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *					S: 1 bit 当设置成1的时候，开始位指示分片NAL单元的开始。当跟随的FU荷载不是开始，设为0.
 *					E: 1 bit 当设置成1的时候，结束位指示分片NAL单元的结束。荷载的最后字节也是分片NAL的最后一个字节
 *							 当不是NAL单元的最后分片，结束位为0。
 *					R: 1 bit 保留位必须设置为0，接收者必须忽略该位。
 * 
 *					打包时，原始的NAL头的前三位为FU indicator的前三位，原始的NAL头的后5位为FU header的后5位。
 *					FU indicator的type字段为FU-A或FU-S。这里为FU-A(28).
 *					
 *					打包时，FUindicator的F、NRI是NAL Header中的F、NRI，Type是28；
 *					FU Header的S、E、R分别按照分片起始位置设置，Type是NAL Header中的Type。
 * 
 *					字段说明均参考自：https://blog.csdn.net/chen495810242/article/details/39207305
 *					仅供参考
 *	 主机字节序：
 *	 Intel架构的处理器 一般为小端序
 *   位域：1字节内也有大小端序问题。1字节内定义的顺序在小端序里要反着定义
 *
 */
#define LITTLE_ENDIAN
 //#define USE_RTP_CCRC
using bit = unsigned char;					              /* 8  bit */
using wbit = unsigned short;				              /* 16 bit */
using dwbit = unsigned int;						          /* 32 bit */

#include"PsParser.h"
#include<random>
#include<string.h>
#ifdef _WIN32
	 #include<WinSock2.h>
#else
	 #include<arpa/inet.h>
#endif
#define MAX_RTP_PACKAGE 1400
#pragma pack (1)
struct RTP_HEADER
{
#ifndef LITTLE_ENDIAN
	 bit  V : 2;								          /* RTP Version, 目前为:10 */
	 bit  P : 1;								          /* Padding(填充),P = 1,在末尾填充一个或多个8位组，不是有效载荷 */
	 bit  X : 1;								          /* extends(扩展报头,X = 1，则含有扩展报头 */
	 bit  CC : 4;								          /* CSRC计数器, 表示CSRC标识符的个数 */
	 bit  M : 1;								          /* 如果是视频流，1表示结束;音频流表示会话的开始 */
	 bit  PT : 7;										  /* 有效载荷类型，一般表示视频流或者音频流 */
#else
	 bit  CC : 4;
	 bit  X : 1;
	 bit  P : 1;
	 bit  V : 2;

	 bit  PT : 7;
	 bit  M : 1;
#endif
	 /* 这里注意要字节序号转换 */
	 unsigned short sequeceNumber;				           /* 占16位，发送每个包后，改序号加1，视频和音频分开计数*/
	 unsigned int	timestamp;					           /* 时间戳，必须使用90KHZ时钟频率*/
	 unsigned int   ssrc;						           /* 同步信源,该标识符是随机选择的，参加同一视频会议的两个同步信源不能有相同的SSRC*/
#ifdef USE_RTP_CCRC
	 unsigned int	csrc;						           /* 特约信源，一般RTP不定义扩展头(X = 1),如果需要则特殊处理*/
#endif
};

/* FU-A indicator */
struct RTP_INDICATOR
{
#ifdef LITTLE_ENDIAN
	 bit	   type : 5;
	 bit	   NRI  : 2;
	 bit	   F    : 1;
#else
	 bit	   F	: 1;
	 bit	   NRI  : 2;
	 bit	   type : 5;
#endif
};
/* FU-header */
struct RTP_FU_HEADER
{
#ifdef LITTLE_ENDIAN
	 bit	   type : 5;
	 bit	   R : 1;
	 bit	   E : 1;
	 bit	   S : 1;
#else
	 bit       S : 1;
	 bit       E : 1;
	 bit       R : 1;
	 bit       type : 5;
#endif
};
#pragma pack ()

namespace oaho
{
	 namespace Media
	 {
		  static unsigned char ps_pt = 96;
		  class RTP final
		  {
		  public: 
			   RTP();
			   /*
					@Param dst_buff: 目标缓冲区。存放结果 
					@Param PSPacket: ps操作对象
					@param module_ : 发送模式. RTP_USE_UDP or RTP_USE_TCP
			   */
			   void PrintRTPacketFromPSFile(const char* pathname, int module_);
			   RTP(const RTP&) = delete;
			   RTP& operator =(const RTP& rtp) = delete;

		  private:
			   void			    get_current_pes_packet_length(const oaho::Media::PSPacket& pspkg)const;
		  private:
			   RTP_HEADER       rtp_header;
			   mutable  unsigned short	current_pes_length;	   /*当前pes包的长度*/
		  public:
			   static const int RTP_USE_UDP;
			   static const int RTP_USE_TCP;
			   static const int RTP_MAX_PACKAGE;
		  private:
			   static const int TIME_STAMP; /* 为每一帧的间隔毫秒(一般为40ms) x 90khz(时钟频率) */
		  };
	 }
}
const int oaho::Media::RTP::RTP_USE_UDP = 0;
const int oaho::Media::RTP::RTP_USE_TCP = 1;
const int oaho::Media::RTP::RTP_MAX_PACKAGE = MAX_RTP_PACKAGE;
const int oaho::Media::RTP::TIME_STAMP = 3600;
oaho::Media::RTP::RTP() 
{
	 std::default_random_engine e;
	 std::uniform_int_distribution <unsigned int> u(20201223, 937176211);
	 rtp_header.V    = 0b10;
	 rtp_header.P    = 0b0;
	 rtp_header.X    = 0b0;
	 rtp_header.CC   = 0b0000;
	 rtp_header.M    = 0b0;
	 rtp_header.PT   = 0b0000000;
	 rtp_header.sequeceNumber = 0;
	 rtp_header.timestamp = 0;
	 /* 转换成网络字节序 */
	 rtp_header.ssrc = htonl(u(e));
	 current_pes_length = 0;
}
void oaho::Media::RTP::PrintRTPacketFromPSFile(const char* pathname, int module_)
{
	 if ((module_ != RTP::RTP_USE_UDP) || (module_ != RTP::RTP_USE_TCP))
	 {
		  cerr << "must use UDP or TCP\n";
		  return;
	 }
	 oaho::Media::PSPacket pspkg{ pathname };
	 /* 发送缓冲区 */
	 char arr[RTP_MAX_PACKAGE + 60] = {0};
	 char buff[RTP_MAX_PACKAGE + 60] = {0};
	 unsigned int sequence_number = 0;
	 unsigned int timestamp = 0;
	 unsigned short pkglen = 0;

ReRoad:
	 /* 开始读入数据 */
	 unsigned short readlen = RTP_MAX_PACKAGE;
	 /* readlen为实际读到的数据，值结果参数*/
	 pspkg.read_ps_data_to_buff(buff, &readlen);
	 /*表示已经读完*/
	 if (readlen == -1)
		  return;
	 /* 解析头 */
	 pspkg.parse_ps_header(buff, 0, pspkg);
	 /* 得到当前PES包的长度 */
	 get_current_pes_packet_length(pspkg);
	 /* 开始构造RTP包 */

	 /* 以下为一个pes的数据包*/
	 /* 打单包 */
	 if (current_pes_length <= RTP_MAX_PACKAGE)
	 {
		  rtp_header.M = 0b01;
		  /* RTP报文长度 */
		  pkglen = current_pes_length + sizeof(RTP_HEADER);
		  /* 打入RTP报文长度字段*/
#ifdef LITTLE_ENDIAN
		  arr[0] = ((unsigned char*)&pkglen)[1];
		  arr[1] = ((unsigned char*)&pkglen)[0];
		  rtp_header.sequeceNumber = htonl(sequence_number);
		  rtp_header.sequeceNumber = htonl(timestamp);
#else
		  arr[1] = ((unsigned char*)&pkglen)[1];
		  arr[0] = ((unsigned char*)&pkglen)[0];
		  rtp_header.sequeceNumber = sequence_number;
		  rtp_header.sequeceNumber = timestamp;
#endif
		  memcpy(&arr[2], (char*)&rtp_header, sizeof(RTP_HEADER));
		  memcpy(&arr[sizeof(RTP_HEADER) + 2], buff, readlen);
		  sequence_number++;
		  timestamp += RTP::TIME_STAMP;
		  /* send */
		  goto ReRoad;

	 }/* 分包形式 */
	 else
	 {
		  /* 表示已经发送包数 */
		  int sendpkgcount = 0;
		  /* 已发送的字节数 */
		  int sendbytes = 0;
		  int pkg_count = current_pes_length / RTP_MAX_PACKAGE;
		  if ((current_pes_length % RTP_MAX_PACKAGE) > 0)
			   pkg_count++;
		  while (pkg_count < current_pes_length)
		  {
			   /* 最后一个包 */
			   if (sendpkgcount == pkg_count - 1)
			   {

					rtp_header.M = 1;
					/*最后一个包的字节数 + RTP包的头部长度*/
					unsigned short len = current_pes_length - sendbytes + sizeof(RTP_HEADER);
					pspkg.read_ps_data_to_buff(buff, &len);
			   #ifdef LITTLE_ENDIAN
					arr[0] = ((unsigned char*)&pkglen)[1];
					arr[1] = ((unsigned char*)&pkglen)[0];
					rtp_header.sequeceNumber = htonl(sequence_number);
					rtp_header.sequeceNumber = htonl(timestamp);
			   #else
					arr[1] = ((unsigned char*)&pkglen)[1];
					arr[0] = ((unsigned char*)&pkglen)[0];
					rtp_header.sequeceNumber = sequence_number;
					rtp_header.sequeceNumber = timestamp;
			   #endif
					memcpy(&arr[2], (char*)&rtp_header, sizeof(RTP_HEADER));
					memcpy(&arr[sizeof(RTP_HEADER) + 2], buff, len);

					sequence_number++;
					timestamp += RTP::TIME_STAMP;
					/* send */

					goto ReRoad;
			   }
			   /* 这里也许会涉及到读入不足1400字节的情况 */
			   unsigned short len = RTP_MAX_PACKAGE;
#ifdef LITTLE_ENDIAN
			   arr[0] = ((unsigned char*)&pkglen)[1];
			   arr[1] = ((unsigned char*)&pkglen)[0];
			   rtp_header.sequeceNumber = htonl(sequence_number);
			   rtp_header.sequeceNumber = htonl(timestamp);
#else
			   arr[1] = ((unsigned char*)&pkglen)[1];
			   arr[0] = ((unsigned char*)&pkglen)[0];
			   rtp_header.sequeceNumber = sequence_number;
			   rtp_header.sequeceNumber = timestamp;
#endif
			   memcpy(&arr[2], (char*)&rtp_header, sizeof(RTP_HEADER));
			   memcpy(&arr[sizeof(RTP_HEADER) + 2], buff, readlen);
			   sendpkgcount++;
			   sendbytes += readlen;
			   sequence_number++;
			   timestamp += RTP::TIME_STAMP;
			   /* 重新读入缓冲区 */
			   readlen = RTP_MAX_PACKAGE;
			   pspkg.read_ps_data_to_buff(buff, &readlen);
		  }
		  goto ReRoad;
	 }
}

void oaho::Media::RTP::get_current_pes_packet_length(const oaho::Media::PSPacket& pspkg) const 
{
	 unsigned char high_low[2];
	 /* 得到当前PES数据包的长度 */
#ifdef  LITTLE_ENDIAN
	 high_low[0] = pspkg.pes_header.PES_packet_length[1];
	 high_low[1] = pspkg.pes_header.PES_packet_length[0];
#else
	 high_low[0] = pspkg.pes_header.PES_packet_length[0];
	 high_low[1] = pspkg.pes_header.PES_packet_length[1];
#endif //  LITTLE_ENDIAN
	 /*设置长度*/
	 current_pes_length = *(unsigned short*)(&high_low[0]);
}



#endif