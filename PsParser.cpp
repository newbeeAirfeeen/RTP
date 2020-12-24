#include"./PsParser.h"

using namespace oaho::Media;
PSPacket::PSPacket(const char* pathname) :fp{nullptr},
                                          eof{false},
                                          ps_header{ 0 },
                                          ps_ssh{ 0 },
                                          ps_map_header{ 0 },
                                          pes_header{ 0 }
{
      fp   = fopen(pathname, "rb");
      assert(fp != nullptr);
      fseek(fp, 0, SEEK_SET);
}    
PSPacket::~PSPacket()
{
     fclose(fp);
}

void PSPacket::read_ps_data_to_buff(char* buff, unsigned short* buff_size)
{
     if (feof(fp))
     {
          eof = true;
          *buff_size = -1;
          return;
     }
     memset(buff, 0, *buff_size);
     /* 如果文件本身的大小小于1400*/
     int block = fread(buff, *buff_size, 1, fp);
     if (ferror(fp))
          exit(-1);
     if (block != 1)
          *buff_size = strlen(buff);

}

bool PSPacket::parse_ps_header(const char* buff, int position, PSPacket& pspkg)
{
     //用于更换高低字节
     unsigned char high_low[2];
     int PS_HEADER_SIZE = sizeof(PS_HEADER);
     /* 文件头的四字节 0x000001BA + 后面的14字节*/
     pspkg.ps_header = *(PS_HEADER*)(&buff[position]);
     if (!is_ps_header(ps_header))
     {
          cout << "文件格式有误!\n";
          return false;
     }
     /* 系统头0x000001BB*/
     pspkg.ps_ssh = *(PS_SYSTEM_HEADER*)(&buff[position + PS_HEADER_SIZE]);
     /* 需要更换高低位 */
#ifdef LITTLE_ENDIAN
     high_low[0] = pspkg.ps_ssh.header_length[1];
     high_low[1] = pspkg.ps_ssh.header_length[0];
#else
     high_low[0] = pspkg.ps_ssh.header_length[0];
     high_low[1] = pspkg.ps_ssh.header_length[1];
#endif
     unsigned short jmplen = *(unsigned short*)(&high_low[0]);

     /* ps header + ps system header length  + */
     int ps_header_and_ps_ssh_length = PS_HEADER_SIZE + 4 + jmplen + 2;

     /* 系统头0x000001BC*/
     pspkg.ps_map_header = *(PS_SYSTEM_MAP_HEADER*)(&buff[ps_header_and_ps_ssh_length]);
#ifdef LITTLE_ENDIAN
     high_low[0] = pspkg.ps_map_header.header_length[1];
     high_low[1] = pspkg.ps_map_header.header_length[0];
#else
     high_low[0] = pspkg.ps_map_header.header_length[0];
     high_low[1] = pspkg.ps_map_header.header_length[1];
#endif
     jmplen = *(unsigned short*)(&high_low[0]);

     /* 把光标移动到PES开始的地方 PES[0]*/
     int pes_start_position = ps_header_and_ps_ssh_length + jmplen + 4 + 2;

     pspkg.pes_header = *(PES_HEADER*)(&buff[pes_start_position]);
     if (pspkg.pes_header.stream_id != 0xE0 || pspkg.pes_header.stream_id != 0xC0 )
     {
          cout << "解析错误!\n";
          return false;
     }
     /*
     *    开始读取可选字段的长度
     */
     int len = sizeof(PES_HEADER);
     pes_start_position += sizeof(PES_HEADER);
     pspkg.pes_optional_header = *(OPTIONAL_PES_HEADER*)(&buff[pes_start_position]);
     return true;
}