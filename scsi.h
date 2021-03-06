#ifndef SCSI_COMMAND_H
#define SCSI_COMMAND_H

#include "lusb0_usb.h"
// Device vendor and product id.
#define MY_VID 0x2309
#define MY_PID 0x0606
// Device endpoint(s)
#define EP_IN 0x81
#define EP_OUT 0x01
// Device of bytes to transfer.
#define BUF_SIZE 64

// Device configuration and interface id.
#define MY_CONFIG 1
#define MY_INTF 0

typedef unsigned char   uint8_t;     //无符号8位数
typedef signed   char   int8_t;      //有符号8位数
typedef unsigned int    uint16_t;    //无符号16位数
typedef signed   int    int16_t;     //有符号16位数
typedef unsigned long   uint32_t;    //无符号32位数
typedef signed   long   int32_t;     //有符号32位数
typedef float           float32;     //单精度浮点数
typedef double          float64;     //双精度浮点数

struct command_block_wrapper
{
    uint8_t dCBWSignature[4];
    uint32_t dCBWTag;
    uint32_t dCBWDataTransferLength;
    uint8_t bmCBWFlags;
    uint8_t bCBWLUN;
    uint8_t bCBWCBLength;
    uint8_t CBWCB[16];
};
/*
dCBWSignature: CBW的标识，固定值：43425355h (little endian)。
dCBWTag: 主机发送的一个命令块标识，设备需要原样作为dCSWTag（CSW中的一部分）再发送给Host;主要用于关联CSW到对应的CBW。
dCBWDataTransferLength: 本次CBW命令要求在命令与回应之间传输的字节数。如果为0，则不传输数据。
bmCBWFlags: 反映数据传输的方向，0x00 表示来自Host，0x80 表示发至Host；
bCBWLUN: 对于有多个LUN逻辑单元的设备，用来选择具体目标。如果没有多个LUN，/则写0。
bCBWCBLength: 命令的长度，范围在0~16.
CBWCB: 传输的具体命令，符合bInterfaceSubClass.中定义的命令规范，此处是SCSI指令集
*/
struct command_status_wrapper
{
    uint8_t dCSWSignature[4];
    uint32_t dCSWTag;
    uint32_t dCSWDataResidue;
    uint8_t bCSWStatus;
};
/*
dCSWSignature: CSW的标识，固定值：53425355h (little endian)
dCSWTag: 设置这个标识和CBW中的dCBWTag一致，参照上面关于dCBWTag的解释
dCSWDataResidue: 还需要传送的数据，此数据根据dCBWDataTransferLength－本次已经传送的数据得到
bCSWStatus: 指示命令的执行状态。如果命令正确执行，bCSWStatus 返回0 ，不正确返回1，phase错返回2（当HOST收到此错误时需要对Device复位）
*/
struct data_send_receive
{
    uint8_t *buffer;//1K 数据缓冲区
    uint32_t data_length;// 发送数据长度
    uint32_t time_out;// 超时时间
};
struct command_packet
{
    struct data_send_receive dsr;// 数据
    uint8_t cdb[16];  //命令描述块
    uint8_t cdb_length;// 实际长度
    uint32_t ret_tag;//CBW 和CSW 的对应关系标签
    uint8_t ep_out;// 传输端点
    uint8_t ep_in;//  输入端点
    uint8_t lun;// 为0
    uint32_t actual;// 实际长度
};
int write_command(usb_dev_handle *handle, struct command_packet *pComPkt);
#endif
