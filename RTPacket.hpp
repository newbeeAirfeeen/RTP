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
 * descrition: ���ļ��Ƕ�RTP��ʽ���Ĵ����RTP���Ĵ�����򣬴��ļ�ֻ����������ʽ�ͷ�Ƭ�����ʽ��
 *			   ������ʽ: �ʺ����ݰ�С��MTU��ʱ�̡�
 *			   GB28181 ��RTP ��������ݸ��������й涨���ο�GB28181 ��¼B��������������96-127
 *			   RFC2250 ����96 ��ʾPS ��װ������97 ΪMPEG-4������98 ΪH264��
 *             �����ǽ��յ���RTP ��������Ҫ�жϸ������ͣ�����������Ϊ96�������PS �⸴�ã�������Ƶ�ֿ����롣
 *			   ����������Ϊ98��ֱ�Ӱ���H264 �Ľ������ͽ��롣
 *
 *			   ��Ƭ��ʽ: �����Э����IPv4,�����ݰ�����MTU(ÿһ����·����һ������ЩMTU 1500�ֽڣ�
 *						 ��Щ500�ֽ�)��ʱ��·������ְ���������3000�ֽڵİ�����ֳ�2������
 *						 �ڴ����ʱ�򣬾�����·����������Щ������IPv6�������Ƭ��
 *						 ��FU-A,FU-B.��NAL��Ԫ����28,29��ʾ��
 *			   RTPͷ��(��û��CSRC������£��̶�Ϊ12�ֽ�)
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
 *					V  : RTPЭ��İ汾�š���ǰ�汾Ϊ2
 *					P  : ����־��ռ1λ�����P=1�����ڸñ��ĵ�β�����һ������8λ��(������Ч�غ�)
 *					X  : ��չ��־��ռ1λ�����X=1��RTP��ͷ��һ����չ��ͷ���֡�
 *					CC ��CSRC�ļ�����
 *					M  : ��ǡ��������Ƶ��ʾһ֡�Ľ�������Ƶ����ʾ�Ự�Ŀ�ʼ
 *					PT ����Ч�غ�����
 *					Seq: ���кţ�����������ʶRTP���ĵ����кţ���������ⶪ������0��ʼ���㣬ÿ����һ��+1.
 *					TimeStamp: ʱ�����ӳ��ĳһ����λ��Ĳ���ʱ�̣�����������������ӳٺ��ӳٶ�����
 *					SSRC��ͬ���ź�Դ��ռ32λ������ͬһ��Ƶ���������ͬ���ź�Դ��������ͬ��SSRC��
 *					CCRC����Լ�ź�Դ��ռ32λ��������0-15������ʾ������Լ�ź�Դ
 *			   
 *			   ���ݣ�(��Ϊ����NAL���ͷ�Ƭ��Ԫ)
 *					����NAL����
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
 *					�˽ṹΪ��RTP������NAL��Ԫ����ֻ����һ������ζ�ۺϰ��ͷ�Ƭ��Ԫ�����԰�����ʽ�����
 *                  NAL��Ԫ�ĵ�һ�ֽں�RTP����ͷ��һ���ֽ��غϡ�
 *					
 *					��Ƭ��Ԫ(FU-A):
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
 *					��Ƭֻ���ڵ���NAL��Ԫ�С�NAL��Ԫ��һ����Ƭ��������������NAL��Ԫ�ֽ���ɡ�
 *					��ͬ��NAL��Ԫ�ķ�Ƭ����ʹ�õ�����RTP�������˳����(��һ�������һ��RTP���в��ܰ���
 *					������).���ƣ�NAL��Ԫ���밴��RTP˳��ŵ�˳��װ�䡣
 *					
 *					��һ��NAL��Ԫ����Ƭ�����ڷ�Ƭ��Ԫ(FUs)��ʱ��������Ϊ��ƬNAL��Ԫ��STAPs,MTAPs�����Ա���Ƭ�� FUs������Ƕ�ס� 
 *					��, һ��FU �����԰�����һ��FU������FU��RTPʱ�������óɷ�ƬNAL��Ԫ��NALUʱ�̡�
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
 *					S: 1 bit �����ó�1��ʱ�򣬿�ʼλָʾ��ƬNAL��Ԫ�Ŀ�ʼ���������FU���ز��ǿ�ʼ����Ϊ0.
 *					E: 1 bit �����ó�1��ʱ�򣬽���λָʾ��ƬNAL��Ԫ�Ľ��������ص�����ֽ�Ҳ�Ƿ�ƬNAL�����һ���ֽ�
 *							 ������NAL��Ԫ������Ƭ������λΪ0��
 *					R: 1 bit ����λ��������Ϊ0�������߱�����Ը�λ��
 * 
 *					���ʱ��ԭʼ��NALͷ��ǰ��λΪFU indicator��ǰ��λ��ԭʼ��NALͷ�ĺ�5λΪFU header�ĺ�5λ��
 *					FU indicator��type�ֶ�ΪFU-A��FU-S������ΪFU-A(28).
 *					
 *					���ʱ��FUindicator��F��NRI��NAL Header�е�F��NRI��Type��28��
 *					FU Header��S��E��R�ֱ��շ�Ƭ��ʼλ�����ã�Type��NAL Header�е�Type��
 * 
 *					�ֶ�˵�����ο��ԣ�https://blog.csdn.net/chen495810242/article/details/39207305
 *					�����ο�
 *	 �����ֽ���
 *	 Intel�ܹ��Ĵ����� һ��ΪС����
 *   λ��1�ֽ���Ҳ�д�С�������⡣1�ֽ��ڶ����˳����С������Ҫ���Ŷ���
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
	 bit  V : 2;								          /* RTP Version, ĿǰΪ:10 */
	 bit  P : 1;								          /* Padding(���),P = 1,��ĩβ���һ������8λ�飬������Ч�غ� */
	 bit  X : 1;								          /* extends(��չ��ͷ,X = 1��������չ��ͷ */
	 bit  CC : 4;								          /* CSRC������, ��ʾCSRC��ʶ���ĸ��� */
	 bit  M : 1;								          /* �������Ƶ����1��ʾ����;��Ƶ����ʾ�Ự�Ŀ�ʼ */
	 bit  PT : 7;										  /* ��Ч�غ����ͣ�һ���ʾ��Ƶ��������Ƶ�� */
#else
	 bit  CC : 4;
	 bit  X : 1;
	 bit  P : 1;
	 bit  V : 2;

	 bit  PT : 7;
	 bit  M : 1;
#endif
	 /* ����ע��Ҫ�ֽ����ת�� */
	 unsigned short sequeceNumber;				           /* ռ16λ������ÿ�����󣬸���ż�1����Ƶ����Ƶ�ֿ�����*/
	 unsigned int	timestamp;					           /* ʱ���������ʹ��90KHZʱ��Ƶ��*/
	 unsigned int   ssrc;						           /* ͬ����Դ,�ñ�ʶ�������ѡ��ģ��μ�ͬһ��Ƶ���������ͬ����Դ��������ͬ��SSRC*/
#ifdef USE_RTP_CCRC
	 unsigned int	csrc;						           /* ��Լ��Դ��һ��RTP��������չͷ(X = 1),�����Ҫ�����⴦��*/
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
					@Param dst_buff: Ŀ�껺��������Ž�� 
					@Param PSPacket: ps��������
					@param module_ : ����ģʽ. RTP_USE_UDP or RTP_USE_TCP
			   */
			   void PrintRTPacketFromPSFile(const char* pathname, int module_);
			   RTP(const RTP&) = delete;
			   RTP& operator =(const RTP& rtp) = delete;

		  private:
			   void			    get_current_pes_packet_length(const oaho::Media::PSPacket& pspkg)const;
		  private:
			   RTP_HEADER       rtp_header;
			   mutable  unsigned short	current_pes_length;	   /*��ǰpes���ĳ���*/
		  public:
			   static const int RTP_USE_UDP;
			   static const int RTP_USE_TCP;
			   static const int RTP_MAX_PACKAGE;
		  private:
			   static const int TIME_STAMP; /* Ϊÿһ֡�ļ������(һ��Ϊ40ms) x 90khz(ʱ��Ƶ��) */
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
	 /* ת���������ֽ��� */
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
	 /* ���ͻ����� */
	 char arr[RTP_MAX_PACKAGE + 60] = {0};
	 char buff[RTP_MAX_PACKAGE + 60] = {0};
	 unsigned int sequence_number = 0;
	 unsigned int timestamp = 0;
	 unsigned short pkglen = 0;

ReRoad:
	 /* ��ʼ�������� */
	 unsigned short readlen = RTP_MAX_PACKAGE;
	 /* readlenΪʵ�ʶ��������ݣ�ֵ�������*/
	 pspkg.read_ps_data_to_buff(buff, &readlen);
	 /*��ʾ�Ѿ�����*/
	 if (readlen == -1)
		  return;
	 /* ����ͷ */
	 pspkg.parse_ps_header(buff, 0, pspkg);
	 /* �õ���ǰPES���ĳ��� */
	 get_current_pes_packet_length(pspkg);
	 /* ��ʼ����RTP�� */

	 /* ����Ϊһ��pes�����ݰ�*/
	 /* �򵥰� */
	 if (current_pes_length <= RTP_MAX_PACKAGE)
	 {
		  rtp_header.M = 0b01;
		  /* RTP���ĳ��� */
		  pkglen = current_pes_length + sizeof(RTP_HEADER);
		  /* ����RTP���ĳ����ֶ�*/
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

	 }/* �ְ���ʽ */
	 else
	 {
		  /* ��ʾ�Ѿ����Ͱ��� */
		  int sendpkgcount = 0;
		  /* �ѷ��͵��ֽ��� */
		  int sendbytes = 0;
		  int pkg_count = current_pes_length / RTP_MAX_PACKAGE;
		  if ((current_pes_length % RTP_MAX_PACKAGE) > 0)
			   pkg_count++;
		  while (pkg_count < current_pes_length)
		  {
			   /* ���һ���� */
			   if (sendpkgcount == pkg_count - 1)
			   {

					rtp_header.M = 1;
					/*���һ�������ֽ��� + RTP����ͷ������*/
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
			   /* ����Ҳ����漰�����벻��1400�ֽڵ���� */
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
			   /* ���¶��뻺���� */
			   readlen = RTP_MAX_PACKAGE;
			   pspkg.read_ps_data_to_buff(buff, &readlen);
		  }
		  goto ReRoad;
	 }
}

void oaho::Media::RTP::get_current_pes_packet_length(const oaho::Media::PSPacket& pspkg) const 
{
	 unsigned char high_low[2];
	 /* �õ���ǰPES���ݰ��ĳ��� */
#ifdef  LITTLE_ENDIAN
	 high_low[0] = pspkg.pes_header.PES_packet_length[1];
	 high_low[1] = pspkg.pes_header.PES_packet_length[0];
#else
	 high_low[0] = pspkg.pes_header.PES_packet_length[0];
	 high_low[1] = pspkg.pes_header.PES_packet_length[1];
#endif //  LITTLE_ENDIAN
	 /*���ó���*/
	 current_pes_length = *(unsigned short*)(&high_low[0]);
}



#endif