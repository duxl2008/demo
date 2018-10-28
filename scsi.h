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

typedef unsigned char   uint8_t;     //�޷���8λ��
typedef signed   char   int8_t;      //�з���8λ��
typedef unsigned int    uint16_t;    //�޷���16λ��
typedef signed   int    int16_t;     //�з���16λ��
typedef unsigned long   uint32_t;    //�޷���32λ��
typedef signed   long   int32_t;     //�з���32λ��
typedef float           float32;     //�����ȸ�����
typedef double          float64;     //˫���ȸ�����

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
dCBWSignature: CBW�ı�ʶ���̶�ֵ��43425355h (little endian)��
dCBWTag: �������͵�һ��������ʶ���豸��Ҫԭ����ΪdCSWTag��CSW�е�һ���֣��ٷ��͸�Host;��Ҫ���ڹ���CSW����Ӧ��CBW��
dCBWDataTransferLength: ����CBW����Ҫ�����������Ӧ֮�䴫����ֽ��������Ϊ0���򲻴������ݡ�
bmCBWFlags: ��ӳ���ݴ���ķ���0x00 ��ʾ����Host��0x80 ��ʾ����Host��
bCBWLUN: �����ж��LUN�߼���Ԫ���豸������ѡ�����Ŀ�ꡣ���û�ж��LUN��/��д0��
bCBWCBLength: ����ĳ��ȣ���Χ��0~16.
CBWCB: ����ľ����������bInterfaceSubClass.�ж��������淶���˴���SCSIָ�
*/
struct command_status_wrapper
{
    uint8_t dCSWSignature[4];
    uint32_t dCSWTag;
    uint32_t dCSWDataResidue;
    uint8_t bCSWStatus;
};
/*
dCSWSignature: CSW�ı�ʶ���̶�ֵ��53425355h (little endian)
dCSWTag: ���������ʶ��CBW�е�dCBWTagһ�£������������dCBWTag�Ľ���
dCSWDataResidue: ����Ҫ���͵����ݣ������ݸ���dCBWDataTransferLength�������Ѿ����͵����ݵõ�
bCSWStatus: ָʾ�����ִ��״̬�����������ȷִ�У�bCSWStatus ����0 ������ȷ����1��phase������2����HOST�յ��˴���ʱ��Ҫ��Device��λ��
*/
struct data_send_receive
{
    uint8_t *buffer;//1K ���ݻ�����
    uint32_t data_length;// �������ݳ���
    uint32_t time_out;// ��ʱʱ��
};
struct command_packet
{
    struct data_send_receive dsr;// ����
    uint8_t cdb[16];  //����������
    uint8_t cdb_length;// ʵ�ʳ���
    uint32_t ret_tag;//CBW ��CSW �Ķ�Ӧ��ϵ��ǩ
    uint8_t ep_out;// ����˵�
    uint8_t ep_in;//  ����˵�
    uint8_t lun;// Ϊ0
    uint32_t actual;// ʵ�ʳ���
};
int write_command(usb_dev_handle *handle, struct command_packet *pComPkt);
#endif