//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "mainform.h"
#include <stdio.h>
#include <time.h>
#include <windows.h>
#include "strbase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

using namespace std;
#include <string>
#include <stdio.h>

/*
基本
加密
1. bulk out CBW   ep1 31
2. bulk out DATA  ep1  len
3. bulk in  CSW   ep2 13
解密
1. bulk out CBW   ep1  31
2. bulk in DATA   ep2  len
3. bulk in CSW    ep2  13

优化：

加密
1. bulk out CBW   ep1
2. bulk out DATA  ep1
3. bulk in  CSW   ep2
解密
1. bulk out CBW   ep1
2. bulk in DATA   ep2
3. bulk in CSW    ep2
*/
#include <stdio.h>
TMForm *MForm;
usb_dev_handle *m_dev_handle=NULL; /* the device handle */
struct usb_device *m_dev = NULL;
struct command_packet ComPkt;
struct command_status_wrapper w_csw;


int write_command(usb_dev_handle* handle, struct command_packet *pComPkt)
{
    struct command_block_wrapper w_cbw;
    unsigned char buf[31];
    int w_bRet;
    memset(w_cbw.CBWCB,0x00,16);
    w_cbw.dCBWSignature[0]=0x55;
    w_cbw.dCBWSignature[1]=0x53;
    w_cbw.dCBWSignature[2]=0x42;
    w_cbw.dCBWSignature[3]=0x43;

    w_cbw.dCBWTag=pComPkt->ret_tag;
    w_cbw.dCBWDataTransferLength=pComPkt->dsr.data_length;

    w_cbw.bmCBWFlags=0x00;  //host
    w_cbw.bCBWLUN=0x00;

    w_cbw.bCBWCBLength=pComPkt->cdb_length;  //0~16

    memcpy(w_cbw.CBWCB,pComPkt->cdb,pComPkt->cdb_length);
    memcpy(buf,&w_cbw,31);
    w_bRet=usb_bulk_write(handle,pComPkt->ep_out,(char*)&w_cbw,31, pComPkt->dsr.time_out);
    if (w_bRet<=0) return false;
//------------------------------------------------------------------------------
    if(pComPkt->dsr.data_length)
    {
      w_bRet=usb_bulk_write(handle,pComPkt->ep_out,pComPkt->dsr.buffer,pComPkt->dsr.data_length,pComPkt->dsr.time_out);
      if (w_bRet<=0) return false;
    }
//------------------------------------------------------------------------------
    w_bRet=usb_bulk_read(handle, pComPkt->ep_in, (char *)&w_csw,13,pComPkt->dsr.time_out);
    if (w_bRet<=0)return false;
    else  return true;
}

int read_command(usb_dev_handle* handle, struct command_packet *pComPkt)
{
    struct command_block_wrapper w_cbw;

    int w_bRet;
    memset(w_cbw.CBWCB,0x00,16);
    w_cbw.dCBWSignature[0]=0x55;
    w_cbw.dCBWSignature[1]=0x53;
    w_cbw.dCBWSignature[2]=0x42;
    w_cbw.dCBWSignature[3]=0x43;

    w_cbw.dCBWTag=pComPkt->ret_tag;
    w_cbw.dCBWDataTransferLength=pComPkt->dsr.data_length;

    w_cbw.bmCBWFlags=0x00;  //host
    w_cbw.bCBWLUN=0x00;

    w_cbw.bCBWCBLength=pComPkt->cdb_length;  //0~16

    memcpy(w_cbw.CBWCB,pComPkt->cdb,pComPkt->cdb_length);
    w_bRet=usb_bulk_write(handle,pComPkt->ep_out,(char*)&w_cbw,31, pComPkt->dsr.time_out);
    if (w_bRet<=0) return false;
//------------------------------------------------------------------------------
    if(pComPkt->dsr.data_length)
    {
      w_bRet=usb_bulk_read(handle, pComPkt->ep_in,pComPkt->dsr.buffer,pComPkt->dsr.data_length,pComPkt->dsr.time_out);
      if (w_bRet<=0) return false;
    }
//------------------------------------------------------------------------------
    w_bRet=usb_bulk_read(handle, pComPkt->ep_in, (char *)&w_csw,13,pComPkt->dsr.time_out);
    if (w_bRet<=0) return false;
    else  return true;
}

usb_dev_handle *open_dev(void)
{
    struct usb_bus *bus;
    unsigned char content[100];
    for (bus = usb_get_busses(); bus; bus = bus->next)
    {
        for (m_dev = bus->devices; m_dev; m_dev = m_dev->next)
        {
            if (m_dev->descriptor.idVendor == MY_VID
                    && m_dev->descriptor.idProduct == MY_PID)
            {
                sprintf(content,"bNumConfigurations = %5u\n",m_dev->descriptor.bNumConfigurations);
                return usb_open(m_dev);
            }
        }
    }
    return NULL;
}

int USB()
{
    unsigned char content[100];
    int M_EP_IN = 0;
    int M_EP_OUT = 0;
    if(m_dev_handle)
    {
      usb_release_interface(m_dev_handle, 0);
      usb_close(m_dev_handle);
    }
    m_dev_handle=0;

    if(m_dev_handle)
    {
        sprintf(content,"已经打开：EP_IN: 0x%02x , EP_OUT: 0x%02x \n", M_EP_IN, M_EP_OUT);
        MForm->StatusBar1->Panels->Items[0]->Text=String((char*)content);
        return 1;
    }

    usb_init(); /* initialize the library */
    usb_find_busses(); /* find all busses */
    usb_find_devices(); /* find all connected devices */
    if (!(m_dev_handle = open_dev()))
    {
        sprintf(content,"error opening device: \n%s\n", usb_strerror());
        MForm->StatusBar1->Panels->Items[0]->Text=String((char*)content);
        return 0;
    }
    else
    {
        sprintf(content,"success: device %04X:%04X opened\n", MY_VID, MY_PID);
        MForm->StatusBar1->Panels->Items[0]->Text=String((char*)content);
   }
    if (usb_set_configuration(m_dev_handle, MY_CONFIG) < 0)
    {
        sprintf(content,"error setting config #%d: %s\n", MY_CONFIG, usb_strerror());
        usb_close(m_dev_handle);
        return 0;
    }
    else
    {
        sprintf(content,"success: set configuration #%d\n", MY_CONFIG);
    }
    if (usb_claim_interface(m_dev_handle, 0) < 0)
    {
        sprintf(content,"error claiming interface #%d:\n%s\n", MY_INTF, usb_strerror());
        usb_close(m_dev_handle);
        return 0;
    }
    else
    {
        sprintf(content,"success: claim_interface #%d\n", MY_INTF);
    }
    int num_ep=m_dev->config->interface->altsetting->bNumEndpoints;
    for(int i=0;i<num_ep;i++)
    {
        int ep = m_dev->config->interface->altsetting->endpoint[i].bEndpointAddress;
        int bmAttributes=m_dev->config->interface->altsetting->endpoint[i].bmAttributes;
    //BIt1-0：00控制，01同步，02批量，03中断．
        if (ep > 0x0f)
        {
    	        M_EP_IN = ep;
        }
        else
        {
    	        M_EP_OUT = ep;
        }
    }
    sprintf(content,"EP_IN: 0x%02x , EP_OUT: 0x%02x \n", M_EP_IN, M_EP_OUT);
    MForm->StatusBar1->Panels->Items[0]->Text=String((char*)content);
    return 1;
}

//---------------------------------------------------------------------------
__fastcall TMForm::TMForm(TComponent* Owner)
        : TForm(Owner)
{
  begin_test=0;
}
//---------------------------------------------------------------------------
void __fastcall TMForm::Button1Click(TObject *Sender)
{
  USB();
}
//---------------------------------------------------------------------------
void __fastcall TMForm::Button2Click(TObject *Sender)
{
  int len;
  unsigned char *pt,*pt1;
  int stop=0;
  char *DisplayBuf;
  int ll=Edit4->Text.ToInt();
  if(m_dev==NULL)
  {
    StatusBar1->Panels->Items[0]->Text="USB 设备没有打开";
    return;
  }
  int p_len=Edit4->Text.ToInt();
  ComPkt.ep_out=0x1;
  ComPkt.ep_in=0x82;
  ComPkt.ret_tag=0x4F3b0010;
  ComPkt.lun=0x00;
  ComPkt.dsr.time_out=100;
  ComPkt.cdb_length=0xa;
  memset(ComPkt.cdb,0x00,16);

  string tmp=Edit1->Text.c_str();
  StringToBin(ComPkt.cdb,tmp);
  if((ComPkt.cdb[0]==0xfd)||(ComPkt.cdb[0]==0xfe));
  else
  {
    StatusBar1->Panels->Items[0]->Text="错误命令";
    return;
  }
  tmp=Edit2->Text.c_str();

  pt=new unsigned char [tmp.length()];
  DisplayBuf=new char [tmp.length()];
  len=StringToBin(pt,tmp);
  int writetime=len/(p_len*1024);
  pt1=pt;
  int i=0;
  while(1)
  {
    if(stop)break;
    if(len>p_len*1024)
    {
      ComPkt.dsr.data_length=p_len*1024;
      len-=p_len*1024;
      i++;
    }
    else
    {
      ComPkt.dsr.data_length=len%16? (len/16+1)*16:len;
      stop=1;
    }
    ComPkt.dsr.buffer=pt1;
    pt1+=ll*1024;
    ComPkt.cdb[0]=0xfd;
    int wret =write_command(m_dev_handle, &ComPkt);
    if(wret==true)
    {
      StatusBar1->Panels->Items[0]->Text="写成功！";
    }
    else
    {
      StatusBar1->Panels->Items[0]->Text="写失败！";
      break;
    }
    if(CheckBox1->Checked)
    {
      ComPkt.cdb[0]=0xfe;
      int wret =read_command(m_dev_handle, &ComPkt);
      if(wret==true)
      {
        StatusBar1->Panels->Items[0]->Text.sprintf("读成功！len:%d",ComPkt.dsr.data_length+i*p_len*1024);
      }
      else
      {
        Edit3->Text="";
        StatusBar1->Panels->Items[0]->Text="读失败！";
        break;
      }
    }
  }
  HexToString(DisplayBuf,pt,len);
  Edit3->Text=String(DisplayBuf);
  delete DisplayBuf;
  delete pt;
}
//---------------------------------------------------------------------------


void __fastcall TMForm::Button4Click(TObject *Sender)
{

    if(m_dev_handle)
    {
      usb_release_interface(m_dev_handle, 0);
      usb_close(m_dev_handle);
    }
    Close();

}
//---------------------------------------------------------------------------

void __fastcall TMForm::Button3Click(TObject *Sender)
{
    unsigned char buf[50*1024];
    OpenDialog1->Filter ="bin|*.bin|cy|*.cy|*|*";
    OpenDialog1->DefaultExt ="bin";
    String FileName;

    int ll=Edit4->Text.ToInt();

    double start,end,cost;


    if(m_dev==NULL)
    {
        StatusBar1->Panels->Items[0]->Text="USB 设备没有打开";
        return;
    }
    if(OpenDialog1->Execute())
    {
       FileName=OpenDialog1->FileName;
    }
    else return;
    StatusBar1->Panels->Items[0]->Text="开始...";
    StatusBar1->Update();
    FILE *fp,*fp1;//判断命令行是否正确
    String wf= FileName+".mkv";


    if((fp1=fopen(wf.c_str(),"wb"))==NULL)
    {
        StatusBar1->Panels->Items[0]->Text="打开文件失败！";
        return;//结束程序的执行
    }

    if((fp=fopen(FileName.c_str(),"rb"))==NULL)
    {
        StatusBar1->Panels->Items[0]->Text="打开文件失败！";
        return;//结束程序的执行
    }
    fseek(fp,0L,SEEK_END); /* 定位到文件末尾 */
    unsigned long flen=ftell(fp);
    fseek(fp,0L,SEEK_SET);
    int cylen;
    if((flen%(ll*1024))==0)
      cylen=flen;
    else
      cylen=(flen/(ll*1024)+1)*ll*1024;
   int i,j;
   j=cylen/(ll*1024);

   ComPkt.ep_out=0x1;
   ComPkt.ep_in=0x82;
   ComPkt.ret_tag=0x4F3b0010;
   ComPkt.lun=0x00;
   ComPkt.dsr.time_out=100;
   ComPkt.cdb_length=0xa;
   memset(ComPkt.cdb,0x00,16);

   string tmp=Edit1->Text.c_str();
   StringToBin(ComPkt.cdb,tmp);
   if((ComPkt.cdb[0]==0xfd)||(ComPkt.cdb[0]==0xfe));
   else
   {
     StatusBar1->Panels->Items[0]->Text="错误命令";
     return;
   }

   int wret;
   start=clock();
   for(i=0;i<j;i++)
   {
      int datalen=fread(buf,1,ll*1024,fp);
      if(datalen!=ll*1024)
         memset(buf+datalen,0x00,ll*1024-datalen);
      {
         ComPkt.dsr.buffer=buf;
         ComPkt.dsr.data_length=ll*1024;//(len/8)*8;
         ComPkt.cdb[0]=0xfd;
         wret =write_command(m_dev_handle, &ComPkt);
         ComPkt.cdb[0]=0xfe;
         wret =read_command(m_dev_handle, &ComPkt);
      }
      datalen=fwrite(buf,1,ll*1024,fp1);
    if(!(i%100))
    {
      String dis;
      dis.sprintf("i:%d,len:%d,%.2f%%",i,i*ll*1024,i*ll*1024.0*100/flen);
      StatusBar1->Panels->Items[0]->Text=dis;
      Application->ProcessMessages();
     }
   }
   fclose(fp);
   fclose(fp1);

   end=clock();
   cost=end-start;
   String dis;
   if(cost!=0)
     dis.sprintf("%s,用时：%.3fs,码率：%.2fmps","文件加密完成！",cost/1000.0,flen*8.0/(cost/1000.0)/1000/1000);
   else
     dis.sprintf("%s,用时：%.3fs","文件加密完成！",cost/1000.0);
   StatusBar1->Panels->Items[0]->Text=dis;
}
//---------------------------------------------------------------------------

void __fastcall TMForm::Button5Click(TObject *Sender)
{
      int wret;
      char buf[100],buf1[100];
      wret=usb_control_msg(m_dev_handle,0xc0, 0x02,0x0000, 0x0000,buf,64,10); //wret >0 返回字节数
//      wret=usb_control_msg(m_dev_handle,0xc0, 0x01,0x0000, 0x0000,buf,64,10); //wret >0 返回字节数
      sprintf(buf1,"State %d:%02x,%02x,%02x,%02x,%02x,%02x,%02x\n",wret,buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6]);
      Sleep(1);
      USB();


/*    if(m_dev_handle)
    {
      usb_resetep(m_dev_handle,0x82);
      usb_resetep(m_dev_handle,0x1);
      usb_reset(m_dev_handle);
      usb_close(m_dev_handle);
      m_dev_handle=NULL;
    }
*/
}
//---------------------------------------------------------------------------
void __fastcall TMForm::SpeedButton1Click(TObject *Sender)
{
    Edit2->Text=Edit3->Text;
}
//---------------------------------------------------------------------------


void __fastcall TMForm::Button6Click(TObject *Sender)
{
    unsigned char buf[64000];
    unsigned char buf1[64000];
    unsigned char buf_r[100];
    OpenDialog1->DefaultExt ="bin";
    OpenDialog1->Filter ="bin|*.bin";
    String FileName;
    if(OpenDialog1->Execute())
    {
       FileName=OpenDialog1->FileName;
    }
    else return;
    FILE *fp,*fp1;//判断命令行是否正确
    if((fp=fopen(FileName.c_str(),"rb"))==NULL)
    {
        StatusBar1->Panels->Items[0]->Text="打开文件失败！";
        return;//结束程序的执行
    }
    fseek(fp,0L,SEEK_END); /* 定位到文件末尾 */
    int flen=ftell(fp);
    fseek(fp,0L,SEEK_SET);
    if(flen>64000)
    {
        StatusBar1->Panels->Items[0]->Text="程序太大！";
        return;//结束程序的执行
    }
    fread(buf,1,flen,fp);
    fclose(fp);

    if((fp1=fopen("firmware.bin","wb"))==NULL)
    {
        StatusBar1->Panels->Items[0]->Text="产生文件失败！";
        return;//结束程序的执行
    }
    for(int i=0;i<512;i++)
      buf1[i]=buf[i];
    for(int i=512;i<flen;i++)
      buf1[i]=buf[i]^buf[i%512];
    int datalen=fwrite(buf1,1,flen,fp1);
    fclose(fp1);
    if(m_dev_handle)
    {
      ComPkt.ep_out=0x1;
      ComPkt.ep_in=0x82;
      ComPkt.ret_tag=0x4F3b0010;
      ComPkt.lun=0x00;
      ComPkt.dsr.time_out=500;
      ComPkt.cdb_length=0x6;
      memset(ComPkt.cdb,0x00,16);

      int wret;
      ComPkt.dsr.buffer=buf_r;
      ComPkt.cdb[0]=0xfe; ComPkt.cdb[1]=0xe0;   //set begin address
      ComPkt.cdb[2]=0x00; ComPkt.cdb[3]=0x00;
      ComPkt.cdb[4]=0x20; ComPkt.cdb[5]=0x00;
      ComPkt.dsr.data_length=2;
      for(int i=0;i<2;i++)
      {
        wret=read_command(m_dev_handle, &ComPkt);
      }
      if((wret<0)||(buf_r[0]!=0x90))
      {
        StatusBar1->Panels->Items[0]->Text="编程失败！";
        return;
      }
      ComPkt.dsr.time_out=200;
      ComPkt.dsr.buffer=buf_r;
      ComPkt.cdb[0]=0xfe; ComPkt.cdb[1]=0xe0;     //erase flash
      ComPkt.cdb[2]=0x01; ComPkt.cdb[3]=0x00;
      ComPkt.cdb[4]=flen/1024+1; ComPkt.cdb[5]=0x00;
      ComPkt.dsr.data_length=0x2;
      wret=read_command(m_dev_handle, &ComPkt);
      if((wret<0)||(buf_r[0]!=0x90))
      {
        StatusBar1->Panels->Items[0]->Text="编程失败！";
        return;
      }
      ComPkt.dsr.buffer=buf;
      ComPkt.dsr.time_out=500;
      ComPkt.cdb_length=0xa;
      ComPkt.dsr.data_length=flen;
      ComPkt.cdb[0]=0xfd; ComPkt.cdb[1]=0xe0;          //program
      ComPkt.cdb[2]=0x02; ComPkt.cdb[3]=0x80;
      ComPkt.cdb[4]=flen/256; ComPkt.cdb[5]=flen%256;
      wret=write_command(m_dev_handle, &ComPkt);
      if((wret<0))
      {
        StatusBar1->Panels->Items[0]->Text="编程失败！";
        return;
      }
      ComPkt.dsr.buffer=buf_r;
      ComPkt.dsr.time_out=500;
      ComPkt.cdb_length=0xa;
      ComPkt.dsr.data_length=2;
      ComPkt.cdb[0]=0xfe; ComPkt.cdb[1]=0xe0;          //program
      ComPkt.cdb[2]=0x02; ComPkt.cdb[3]=0x80;
      ComPkt.cdb[4]=flen/256; ComPkt.cdb[5]=flen%256;
      wret=read_command(m_dev_handle, &ComPkt);
      if((wret<0)||(buf_r[0]!=0x90))
      {
        StatusBar1->Panels->Items[0]->Text="编程失败！";
        return;
      }

      ComPkt.dsr.buffer=buf_r;
      ComPkt.cdb[0]=0xfe; ComPkt.cdb[1]=0xc0;           //reset
      ComPkt.cdb[2]=0x44; ComPkt.cdb[3]=0x00;
      ComPkt.cdb[4]=0x00; ComPkt.cdb[5]=0x00;
      ComPkt.dsr.data_length=2;
      wret=read_command(m_dev_handle, &ComPkt);

      if((wret<0)||(buf_r[0]!=0x90))
      {
        StatusBar1->Panels->Items[0]->Text="复位失败！";
        return;
      }
      StatusBar1->Panels->Items[0]->Text="复位成功！";
   }


}
//---------------------------------------------------------------------------

void __fastcall TMForm::Button7Click(TObject *Sender)
{

    unsigned char *buf;
    OpenDialog1->DefaultExt ="bin";
    OpenDialog1->Filter ="bin|*.bin";
    String FileName;
    if(OpenDialog1->Execute())
    {
       FileName=OpenDialog1->FileName;
    }
    else return;
    FILE *fp;//判断命令行是否正确
    if((fp=fopen(FileName.c_str(),"rb"))==NULL)
    {
        StatusBar1->Panels->Items[0]->Text="打开文件失败！";
        return;//结束程序的执行
    }
    fseek(fp,0L,SEEK_END); /* 定位到文件末尾 */
    unsigned long flen=ftell(fp);
    fseek(fp,0L,SEEK_SET);
    buf=new unsigned char [flen];
    if(flen>64000)
    {
//        StatusBar1->Panels->Items[0]->Text="程序太大！";
//        return;//结束程序的执行
    }
    fread(buf,1,flen,fp);
    fclose(fp);
    String tmp="",st;
    for(unsigned long i=0;i<flen;i++)
    {
      st.sprintf("%02x ",buf[i]);
      tmp+=st;
    }
    Edit2->Text=tmp;
    st.sprintf("文件长度：%d,len%%48K=%d",flen,flen%(48*1024));
    StatusBar1->Panels->Items[0]->Text=st;
    delete buf;

}
//---------------------------------------------------------------------------

void __fastcall TMForm::Button8Click(TObject *Sender)
{
  int len;
  unsigned char *pt,*pt1;
  char *DisplayBuf;
  double start,end,cost;
  int i=0;

  if(m_dev==NULL)
  {
    StatusBar1->Panels->Items[0]->Text="USB 设备没有打开";
    return;
  }
  if(begin_test==0){
     Button8->Caption="停止";
     begin_test=1;
  }
  else{
     Button8->Caption="测试";
     begin_test=0;
     return;
  }

  int p_len=Edit4->Text.ToInt();
  ComPkt.ep_out=0x1;
  ComPkt.ep_in=0x82;
  ComPkt.ret_tag=0x4F3b0010;
  ComPkt.lun=0x00;
  ComPkt.dsr.time_out=100;
  ComPkt.cdb_length=0xa;
  memset(ComPkt.cdb,0x00,16);

  string tmp=Edit1->Text.c_str();
  StringToBin(ComPkt.cdb,tmp);
  if((ComPkt.cdb[0]==0xfd)||(ComPkt.cdb[0]==0xfe));
  else
  {
    StatusBar1->Panels->Items[0]->Text="错误命令";
    return;
  }
  tmp=Edit2->Text.c_str();

  pt=new unsigned char [48*1024];
  DisplayBuf=new char [48*1024];
  //len=StringToBin(pt,tmp);
  int writetime=1000000;//len/(48*1024);
  pt1=pt;
  int k=0;
  start=clock();
  for(k=0;;k++)
  {
    if(begin_test==0)break;
    ComPkt.dsr.buffer=pt1;
    ComPkt.dsr.data_length=p_len*1024;;
    ComPkt.cdb[0]=0xfd;
    int wret =write_command(m_dev_handle, &ComPkt);
    if(wret==true)
    {
      if(!(k%10))
        StatusBar1->Panels->Items[0]->Text="写成功！";
    }
    else
    {
      StatusBar1->Panels->Items[0]->Text="写失败！";
      break;
    }
    if(CheckBox1->Checked)
    {
      ComPkt.cdb[0]=0xfe;
      int wret =read_command(m_dev_handle, &ComPkt);
      if(wret==true)
      {
          if(!(k%10))
            StatusBar1->Panels->Items[0]->Text.sprintf("%s,%d","操作成功！",k);
      }
      else
      {
        Edit3->Text="";
        StatusBar1->Panels->Items[0]->Text="操作失败！";
        break;
      }
    }
    Application->ProcessMessages();
  }
   end=clock();
   cost=end-start;
   String dis;
   if(cost!=0)
     dis.sprintf("%s,用时：%.3fs,码率：%.2fmps","测试完成！",cost/1000.0,k*p_len*1024*8.0/(cost/1000.0)/1000/1000);
   else
     dis.sprintf("%s,用时：%.3fs","测试完成！",cost/1000.0);
   StatusBar1->Panels->Items[0]->Text=dis;

  delete DisplayBuf;
  delete pt;
}
//---------------------------------------------------------------------------



void __fastcall TMForm::Button9Click(TObject *Sender)
{
    Application->MessageBoxA("Key:40bb12dd6a8273867f3529d354b4a026\nIV:  4dd5660e5b9b4b6473c3c52d5f2661f6","提示",MB_ICONWARNING);
}
//---------------------------------------------------------------------------

void __fastcall TMForm::Button10Click(TObject *Sender)
{
      int wret;
      char buf[100],buf1[100];
      wret=usb_control_msg(m_dev_handle,0xc0, 0x01,0x0000, 0x0000,buf,64,10); //wret >0 返回字节数
      char DisplayBuf[500];
      if(wret>0)
        HexToString(DisplayBuf,buf,wret);
      else
        sprintf(DisplayBuf,"%s","error!");
      Edit3->Text=String(DisplayBuf);
return;
/*

    struct command_block_wrapper w_cbw;
    struct command_status_wrapper w_csw;
    unsigned char buf[31];
    int w_bRet;
    if(m_dev==NULL)
    {
      StatusBar1->Panels->Items[0]->Text="USB 设备没有打开";
      return;
    }
    ComPkt.ep_out=0x1;
    ComPkt.ep_in=0x82;
    ComPkt.ret_tag=0x4F3b0010;
    ComPkt.lun=0x00;
    ComPkt.dsr.time_out=100;
    ComPkt.cdb_length=0xa;
    memset(ComPkt.cdb,0x00,16);

    string tmp=Edit1->Text.c_str();
    StringToBin(ComPkt.cdb,tmp);
    ComPkt.cdb[0]=0xf1;
    memcpy(w_cbw.CBWCB,ComPkt.cdb,ComPkt.cdb_length);
    memcpy(buf,&w_cbw,31);
    w_cbw.dCBWSignature[0]=0x55;
    w_cbw.dCBWSignature[1]=0x53;
    w_cbw.dCBWSignature[2]=0x42;
    w_cbw.dCBWSignature[3]=0x43;

    w_bRet=usb_bulk_write(m_dev_handle,ComPkt.ep_out,(char*)&w_cbw,31, ComPkt.dsr.time_out);

    w_bRet=usb_bulk_read(m_dev_handle, ComPkt.ep_in, (char *)&w_csw,13,ComPkt.dsr.time_out);
*/
}
//---------------------------------------------------------------------------

void __fastcall TMForm::Button11Click(TObject *Sender)
{
      int wret;
      char buf[100],buf1[100];
      string tmp=Edit2->Text.c_str();
      StringToBin(buf+8,tmp);
      buf[0]=0xa,buf[1]=0xb,buf[2]=0xc,buf[3]=0xd;
      buf[4]=0x00,buf[5]=0x0,buf[6]=0x0,buf[7]=0x0;

//      for(int i=0;i<40;i++)
//         buf[4+i]=8+i;
      wret=usb_control_msg(m_dev_handle,0x41, 0x02,0x0001, 0x0000,buf,24,100); //wret >0 返回字节数
         String dis;
   if(wret>0)
     dis.sprintf("完成");
   else
     dis.sprintf("失败！");
   StatusBar1->Panels->Items[0]->Text=dis;
}
//---------------------------------------------------------------------------


