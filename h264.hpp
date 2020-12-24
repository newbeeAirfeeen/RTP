#pragma once
/*
*    copyright (z) hzsh
*    all rights reserved.
* 
*    filename   : h264.hpp
*    date       : 2020/12/23
*    author     : oaho(shenhao)
* 
*    descrition : 此文件定义了h264文件格式的数据项定义。
*                 h264 = [start_code][NALU_Header][RBSP]. 为一个前缀开始，加上一个NALU_HEADER,最后才是RBSP
*                 负载数据
*                 start_code : 0x000001 or 0x00000001.
*                 NALU_HEADER: 
*                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*                               | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
*                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*                               | F |  NRI  |       Type        |
*                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*                 列出h264的头是因为RTP在打包的时候需要此头。详情在RTP_Packet.h文件中
* 
*                 h264文件格式说明：
*                 00 00 00 01 67    (SPS)：SPS即Sequence Paramater Set，又称作序列参数集。SPS中保存了一组编码视频序列(Coded video sequence)的全局参数。
                                     所谓的编码视频序列即原始视频的一帧一帧的像素数据经过编码之后的结构组成的序列。而每一帧的编码后数据所依赖的参数保存
                                     于图像参数集中。一般情况SPS和PPS的NAL Unit通常位于整个码流的起始位置。但在某些特殊情况下，在码流中间也可能出现这两
                                     种结构，主要原因可能为：
                                        1：解码器需要在码流中间开始解码；
                                        2：编码器在编码的过程中改变了码流的参数（如图像分辨率等）；
                  00 00 00 01 68    (PPS)：除了序列参数集SPS之外，H.264中另一重要的参数集合为图像参数集Picture Paramater Set(PPS)。通常情况下，PPS类似于
                                     SPS，在H.264的裸码流中单独保存在一个NAL Unit中，只是PPS NAL Unit的nal_unit_type值为8；而在封装格式中，PPS通常与SPS一
                                     起，保存在视频文件的文件头中。
                  00 00 00 01 65    (IDR帧) ： I帧表示关键帧，你可以理解为这一帧画面的完整保留；解码时只需要本帧数据就可以完成（因为包含完整画面）
                  00 00 00 01 61    (P帧) ：P帧表示的是这一帧跟之前的一个关键帧（或P帧）的差别，解码时需要用之前缓存的画面叠加上本帧定义的差别，
                                     生成最终画面。（也就是差别帧，P帧没有完整画面数据，只有与前一帧的画面差别的数据
                                      
                  字段参考来自： https://www.jianshu.com/p/e061bc1c20c8
                                 https://www.pianshen.com/article/322556341/
      
*/
#ifndef H264_HPP_
#define H264_HPP_
#pragma pack(1)
struct h264_prefix_3
{
     unsigned char start_code_prefix[3];
#ifdef LITTLE_ENDIAN
     unsigned char type : 5;
     unsigned char NRI  : 2;
     unsigned char F    : 1;
#else
     unsigned char F    : 1;
     unsigned char NRI  : 2;
     unsigned type      : 5;
#endif

};
struct h264_prefix_4
{
     unsigned char start_code_prefix[4];
#ifdef LITTLE_ENDIAN
     unsigned char type : 5;
     unsigned char NRI : 2;
     unsigned char F : 1;
#else
     unsigned char F : 1;
     unsigned char NRI : 2;
     unsigned type : 5;
#endif
};
#pragma pop()
/*
     判断数据流是否是h264格式
     @param : 字节数组
     @return: 返回3表示以0x000001开始的h264数据流
              返回4表示以0x00000001开始的h264数据流。
              返回-1表示均不是
*/
inline unsigned char is_h264_prefix(const char* buf)
{
     if (buf[0] != 0x00 || buf[1] != 0x00) return -1;
     /* h264 prefix 3*/
     if (buf[2] == 0x01)     return 3;
     if (buf[2] == 0x00 && buf[3] == 0x01)return 4;
     return -1;
}
#endif