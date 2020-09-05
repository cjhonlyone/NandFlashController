
// USBHOSTAPPDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "USBHOSTAPP.h"
#include "USBHOSTAPPDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int

#define s8 signed char
#define s16 signed short
#define s32 signed int

#define f32 float
int g_no_device_flag = 1;

#define edp2in 0x82
#define edp2out 0x03

#define BULK_RECV_EP    0x82
#define BULK_SEND_EP    0x03

#define INT_RECV_EP     0x81
#define INT_SEND_EP     0x02

#define SEND_BUFF_LEN    8704
#define RECV_BUFF_LEN    8704

#define VID 0x03fd
#define PID 0x0103	//jlq

volatile int exit_thread = 0;
volatile int bulk_thread_finished = 0;
volatile int bulk_rev_thread_ready = 0;
CWinThread* bulk_rev_thread_ptr = NULL;

#define ReadPage 0x00
#define ReadParameter 0x01
#define ProgramPage 0x02
#define EraseBlcok 0x03
#define ReadBlock 0x04
#define Set485Ch0LoopBack 0x05
#define Set485Ch1LoopBack 0x06
#define SendSrioRx 0x08
#define EnableSrioRx 0x09

#define TESTWRconsistent 0x0A
#define TESTwritespeed 0x0B
#define TESTreadspeed 0x0C

unsigned char rev_buf[RECV_BUFF_LEN];   //usb 接收缓冲区
unsigned char send_buf[SEND_BUFF_LEN];  //usb发送缓冲区 

libusb_device_handle *handle = NULL;
libusb_context *ctx = NULL;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

#pragma pack(1)
struct nand_onfi_params {
	/* rev info and features block */
	/* 'O' 'N' 'F' 'I'  */
	u8 sig[4];
	u16 revision;
	u16 features;
	u16 opt_cmd;
	u8 reserved0[2];
	u16 ext_param_page_length; /* since ONFI 2.1 */
	u8 num_of_param_pages;        /* since ONFI 2.1 */
	u8 reserved1[17];

	/* manufacturer information block */
	char manufacturer[12];
	char model[20];
	u8 jedec_id;
	u16 date_code;
	u8 reserved2[13];

	/* memory organization block */
	u32 byte_per_page;
	u16 spare_bytes_per_page;
	u32 data_bytes_per_ppage;
	u16 spare_bytes_per_ppage;
	u32 pages_per_block;
	u32 blocks_per_lun;
	u8 lun_count;
	u8 addr_cycles;
	u8 bits_per_cell;
	u16 bb_per_lun;
	u16 block_endurance;
	u8 guaranteed_good_blocks;
	u16 guaranteed_block_endurance;
	u8 programs_per_page;
	u8 ppage_attr;
	u8 ecc_bits;
	u8 interleaved_bits;
	u8 interleaved_ops;
	u8 reserved3[13];

	/* electrical parameter block */
	u8 io_pin_capacitance_max;
	u16 async_timing_mode;
	u16 program_cache_timing_mode;
	u16 t_prog;
	u16 t_bers;
	u16 t_r;
	u16 t_ccs;
	u16 src_sync_timing_mode;
	u8 src_ssync_features;
	u16 clk_pin_capacitance_typ;
	u16 io_pin_capacitance_typ;
	u16 input_pin_capacitance_typ;
	u8 input_pin_capacitance_max;
	u8 driver_strength_support;
	u16 t_int_r;
	u16 t_adl;
	u8 reserved4[8];

	/* vendor */
	u16 vendor_revision;
	u8 vendor[88];

	u16 crc;
};

struct nand_onfi_vendor_micron {
	u8 two_plane_read;
	u8 read_cache;
	u8 read_unique_id;
	u8 dq_imped;
	u8 dq_imped_num_settings;
	u8 dq_imped_feat_addr;
	u8 rb_pulldown_strength;
	u8 rb_pulldown_strength_feat_addr;
	u8 rb_pulldown_strength_num_settings;
	u8 otp_mode;
	u8 otp_page_start;
	u8 otp_data_prot_addr;
	u8 otp_num_pages;
	u8 otp_feat_addr;
	u8 read_retry_options;
	u8 reserved[72];
	u8 param_revision;
};

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CUSBHOSTAPPDlg 对话框



CUSBHOSTAPPDlg::CUSBHOSTAPPDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUSBHOSTAPPDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CUSBHOSTAPPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CUSBHOSTAPPDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CUSBHOSTAPPDlg::OnBnClickedButton1)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON2, &CUSBHOSTAPPDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CUSBHOSTAPPDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CUSBHOSTAPPDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CUSBHOSTAPPDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CUSBHOSTAPPDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CUSBHOSTAPPDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CUSBHOSTAPPDlg::OnBnClickedButton8)
END_MESSAGE_MAP()


// CUSBHOSTAPPDlg 消息处理程序

BOOL CUSBHOSTAPPDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标


	// TODO:  在此添加额外的初始化代码
	USES_CONVERSION;
	SetDlgItemText(IDC_EDIT1, A2T("0"));// page
	SetDlgItemText(IDC_EDIT2, A2T("0"));// block
	SetDlgItemText(IDC_EDIT4, A2T("1"));// block
	static CFont font;
	font.DeleteObject();
	font.CreatePointFont(100, _T("Consolas"));
	GetDlgItem(IDC_EDIT1)->SetFont(&font);
	GetDlgItem(IDC_EDIT2)->SetFont(&font);
	GetDlgItem(IDC_EDIT3)->SetFont(&font);



	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CUSBHOSTAPPDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CUSBHOSTAPPDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CUSBHOSTAPPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

UINT intertupt_rev_thread(LPVOID pParam){  //线程要调用的函数

	int i = 0;
	int size;
	int rec;
	int save_bytes;

	TRACE("intertupt_rev_thread started.\n");
	while (exit_thread == 0)
	{
		//if (g_no_device_flag){
		//	Sleep(50);
		//	continue;
		//}

		memset(rev_buf, 0, RECV_BUFF_LEN);
		rec = libusb_interrupt_transfer(handle, INT_RECV_EP, rev_buf, RECV_BUFF_LEN, &size, 20);

		if (rec == 0)
		{
			TRACE("\ninterrupt  ep rev sucess, length: %d bytes. \n", size);


		}
		else if (rec == -7)
		{

		}
		//else
		//{
		//	TRACE("interrupt  ep rev faild, err: %s\n", libusb_error_nalibusb_handle_eventsme(rec));
		//	if (rec == LIBUSB_ERROR_IO)
		//		g_no_device_flag = 1; //防止一直输出err
		//}
	}
	AfxEndThread(0);
	return 0;
}
UINT bulk_rev_thread(LPVOID pParam){  //线程要调用的函数

	//CUSBHOSTAPPDlg*p = (CUSBHOSTAPPDlg*)pParam;
	bulk_rev_thread_ready = 0;
	bulk_thread_finished = 0;
	u32 PacketNum = (u32)pParam;
	u32 ActualPacketNum = 0;
	//int i = 0;
	int size = 0;
	int rec = 0;
	u8* BUFFERPTR = rev_buf;
	//TRACE("bulk_rev_thread started.\n");

	//start = clock();
	memset(BUFFERPTR, 0, RECV_BUFF_LEN*PacketNum);
	//finish = clock();
	//totaltime = (double)(finish - start) / CLOCKS_PER_SEC;
	//TRACE("%.6fs\n", 1 *totaltime);


	while (exit_thread == 0)
	{

		bulk_rev_thread_ready = 1;
		rec = libusb_bulk_transfer(handle, BULK_RECV_EP, BUFFERPTR, RECV_BUFF_LEN, &size, 0);

		if (rec == 0)
		{
			//TRACE("%08x %08x\n", *(u32*)(BUFFERPTR + 32), *(u32*)(BUFFERPTR + 48));
			//TRACE("%d\n", (u32)rev_bufPtr);
			//debug_ptr[ActualPacketNum] = BUFFERPTR;
			ActualPacketNum++;
			BUFFERPTR = BUFFERPTR + RECV_BUFF_LEN;
			if (ActualPacketNum == PacketNum)
			{
				bulk_thread_finished = 1;
				AfxEndThread(0);
				return 0;
			}
			else
			{
				continue;
			}

		}
	}
	bulk_thread_finished = 1;
	AfxEndThread(0);
	return 0;
}

void CUSBHOSTAPPDlg::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	if (handle != NULL)
		goto usb_exit;
	int ret = 0;
	ret = libusb_init(&ctx);
	if (ret < 0) goto usb_error;
	handle = libusb_open_device_with_vid_pid(ctx, VID, PID);
	if (handle == NULL) goto usb_error;

	if (libusb_kernel_driver_active(handle, 0) == 1) {
		if (libusb_detach_kernel_driver(handle, 0) == 0) {
		}
	}

	ret = libusb_reset_device(handle);
	if (ret < 0) goto usb_error;
	libusb_get_device(handle);
	ret = libusb_claim_interface(handle, 0);
	if (ret < 0) goto usb_error;
	ret = libusb_claim_interface(handle, 1);
	if (ret < 0) goto usb_error;

	exit_thread = 0;

	g_no_device_flag = 0;
	MessageBox(_T("USB连接成功"));
	return;

usb_error:
	MessageBox(_T("无法连接USB设备"));
	return;

usb_exit:
	MessageBox(_T("USB设备已经连接"));
	return;


}




void CUSBHOSTAPPDlg::OnBnClickedButton2()
{
	// TODO:  在此添加控件通知处理程序代码
	int ret = 0;
	int actual_len = 0;
	CString str = NULL;
	u32 PaketNum = 1;
	u8* bufferptr = NULL;
	if (handle != NULL)
	{
		bulk_thread_finished = 1;
		bulk_rev_thread_ptr = AfxBeginThread(bulk_rev_thread, (LPVOID)PaketNum);
		//while (bulk_thread_finished == 1);
		//TRACE("read page %dx\n", bulk_thread_finished);
		*(u32 *)(send_buf) = 0x00007f7e | (ReadPage << 16);

		GetDlgItem(IDC_EDIT1)->GetWindowText(str);
		*(u32 *)(send_buf + 4) = _tcstol(str, NULL, 10);
		GetDlgItem(IDC_EDIT2)->GetWindowText(str);
		*(u32 *)(send_buf + 8) = _tcstol(str, NULL, 10);
		GetDlgItem(IDC_EDIT4)->GetWindowText(str);
		*(u32 *)(send_buf + 12) = _tcstol(str, NULL, 10);

		ret = libusb_bulk_transfer(handle, BULK_SEND_EP, send_buf, SEND_BUFF_LEN, &actual_len, 0);

		if (ret != 0)
		{
			//MessageBox(_T("USB 发送失败 %s"));
			TRACE("ReadPage Usb faild, err: %s\n", libusb_error_name(ret));
		}


		//start = clock();

		//int k = 0;

		while (bulk_thread_finished == 0);


		bufferptr = rev_buf + 32;
		//TRACE("read page %08x %08x\n", *(u32*)(rev_buf + 32), *(u32*)(rev_buf + 36));

		CEdit *pBoxOne;
		CString str = _T("");
		CString full_page = _T("");
		CString temp = _T("");
		pBoxOne = (CEdit*)GetDlgItem(IDC_EDIT3);

		for (int i = 0; i < (2048); i++)
		{

			if (((i << 2) & 0x000000ff) == 0)
			{
				temp.Format(_T("Row %d\r\n"), i >> 7);
				full_page = full_page + temp;
			}


			temp.Format(_T("%08X"), *(u32*)(bufferptr + i * 4));
			full_page = full_page + temp;


			if (((i & 0x00000007) == 0x00000007) && (i != 0))
			{
				temp.Format(_T("\r\n"));
			}
			else
			{
				temp.Format(_T(" "));
			}
			full_page = full_page + temp;
		}

		pBoxOne->SetWindowTextW(full_page);

	}
	else
	{
		MessageBox(_T("USB 未连接"));
	}


}
void CUSBHOSTAPPDlg::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (handle != NULL){
		libusb_release_interface(handle, 0);
		libusb_release_interface(handle, 1);
		libusb_close(handle);
		libusb_exit(NULL);
		handle = NULL;
	}

	CDialogEx::OnClose();
}


void CUSBHOSTAPPDlg::OnBnClickedButton3()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (handle != NULL){
		libusb_release_interface(handle, 0);
		libusb_release_interface(handle, 1);
		libusb_close(handle);
		libusb_exit(NULL);
		handle = NULL;
	}
}


void CUSBHOSTAPPDlg::OnBnClickedButton4()
{
	// TODO:  在此添加控件通知处理程序代码
	int ret = 0;
	int actual_len = 0;
	CString str = _T("");

	if (handle != NULL)
	{

		bulk_rev_thread_ptr = AfxBeginThread(bulk_rev_thread, (LPVOID)1);


		*(u32 *)(send_buf) = 0x00007f7e | (EraseBlcok << 16);

		GetDlgItem(IDC_EDIT1)->GetWindowText(str);
		*(u32 *)(send_buf + 4) = _tcstol(str, NULL, 10);

		GetDlgItem(IDC_EDIT2)->GetWindowText(str);
		*(u32 *)(send_buf + 8) = _tcstol(str, NULL, 10);

		GetDlgItem(IDC_EDIT4)->GetWindowText(str);
		*(u32 *)(send_buf + 12) = _tcstol(str, NULL, 10);

		ret = libusb_bulk_transfer(handle, BULK_SEND_EP, send_buf, SEND_BUFF_LEN, &actual_len, 0);

		while (bulk_thread_finished == 0);
		if (ret != 0)
		{
			//MessageBox(_T("USB 发送失败 %s"));
			TRACE("EraseBlcok Usb faild, err: %s\n", libusb_error_name(ret));
		}


		if (*(u32 *)(rev_buf + 12) & 0x00000001)
		{
			MessageBox(_T("Erase Failed!"));
		}
		else
		{
			MessageBox(_T("Erase Successfully!"));
		}

	}
	else
	{
		MessageBox(_T("USB 未连接"));
	}
}


void CUSBHOSTAPPDlg::OnBnClickedButton5()
{
	// TODO:  在此添加控件通知处理程序代码
	int ret = 0;
	int actual_len = 0;
	CString str = _T("");

	if (handle != NULL)
	{

		bulk_rev_thread_ptr = AfxBeginThread(bulk_rev_thread, (LPVOID)1);


		*(u32 *)(send_buf) = 0x00007f7e | (ReadParameter << 16);

		GetDlgItem(IDC_EDIT1)->GetWindowText(str);
		*(u32 *)(send_buf + 4) = _tcstol(str, NULL, 10);

		GetDlgItem(IDC_EDIT2)->GetWindowText(str);
		*(u32 *)(send_buf + 8) = _tcstol(str, NULL, 10);

		GetDlgItem(IDC_EDIT4)->GetWindowText(str);
		*(u32 *)(send_buf + 12) = _tcstol(str, NULL, 10);

		ret = libusb_bulk_transfer(handle, BULK_SEND_EP, send_buf, SEND_BUFF_LEN, &actual_len, 0);

		while (bulk_thread_finished == 0);
		if (ret != 0)
		{
			//MessageBox(_T("USB 发送失败 %s"));
			TRACE("EraseBlcok Usb faild, err: %s\n", libusb_error_name(ret));
		}


		CEdit *pBoxOne;
		CString str;
		CString full_page, temp;
		pBoxOne = (CEdit*)GetDlgItem(IDC_EDIT3);

		u8 *tmpptr = rev_buf+32;
		struct nand_onfi_params *nand_onfi_params_ptr = (struct nand_onfi_params *)(rev_buf + 32);


		char string[256];
		USES_CONVERSION;
		temp.Format(_T("rev info and features block\r\n")); full_page = full_page + temp;
		memcpy(string, (char *)nand_onfi_params_ptr->sig, 4); string[4] = 0;
		temp.Format(_T("sig[4]                     : %s\r\n"), A2W(string)); full_page = full_page + temp;
		temp.Format(_T("revision                   : 0x%04x\r\n"), nand_onfi_params_ptr->revision); full_page = full_page + temp;
		temp.Format(_T("features                   : 0x%04x\r\n"), nand_onfi_params_ptr->features); full_page = full_page + temp;
		temp.Format(_T("opt_cmd                    : 0x%04x\r\n"), nand_onfi_params_ptr->opt_cmd); full_page = full_page + temp;
		//memcpy(string, (char *)nand_onfi_params_ptr->reserved0, 2); string[2] = 0;
		//temp.Format(_T("reserved0[2]               : %d\r\n"), nand_onfi_params_ptr->reserved0[2]); full_page = full_page + temp;
		temp.Format(_T("ext_param_page_length      : %d\r\n"), nand_onfi_params_ptr->ext_param_page_length); full_page = full_page + temp;
		temp.Format(_T("num_of_param_pages         : %d\r\n"), nand_onfi_params_ptr->num_of_param_pages); full_page = full_page + temp;
		//memcpy(string, (char *)nand_onfi_params_ptr->reserved1, 17); string[17] = 0;
		//temp.Format(_T("reserved1[17]              : %d\r\n"), nand_onfi_params_ptr->reserved1[17]); full_page = full_page + temp;
		
		temp.Format(_T("\r\nmanufacturer information block\r\n")); full_page = full_page + temp;
		memcpy(string, nand_onfi_params_ptr->manufacturer, 12); string[12] = 0;
		temp.Format(_T("manufacturer[12]           : %s\r\n"), A2W(string)); full_page = full_page + temp;
		memcpy(string, nand_onfi_params_ptr->model, 20); string[20] = 0;
		temp.Format(_T("model[20]                  : %s\r\n"), A2W(string)); full_page = full_page + temp;
		temp.Format(_T("jedec_id                   : 0x%02x\r\n"), nand_onfi_params_ptr->jedec_id); full_page = full_page + temp;
		temp.Format(_T("date_code                  : 0x%04x\r\n"), nand_onfi_params_ptr->date_code); full_page = full_page + temp;
		//memcpy(string, (char *)nand_onfi_params_ptr->reserved2, 13); string[13] = 0;
		//temp.Format(_T("reserved2[13]              : %d\r\n"), nand_onfi_params_ptr->reserved2[13]); full_page = full_page + temp;
		
		temp.Format(_T("\r\nmemory organization block\r\n")); full_page = full_page + temp;
		temp.Format(_T("byte_per_page              : %d\r\n"), nand_onfi_params_ptr->byte_per_page); full_page = full_page + temp;
		temp.Format(_T("spare_bytes_per_page       : %d\r\n"), nand_onfi_params_ptr->spare_bytes_per_page); full_page = full_page + temp;
		temp.Format(_T("data_bytes_per_ppage       : %d\r\n"), nand_onfi_params_ptr->data_bytes_per_ppage); full_page = full_page + temp;
		temp.Format(_T("spare_bytes_per_ppage      : %d\r\n"), nand_onfi_params_ptr->spare_bytes_per_ppage); full_page = full_page + temp;
		temp.Format(_T("pages_per_block            : %d\r\n"), nand_onfi_params_ptr->pages_per_block); full_page = full_page + temp;
		temp.Format(_T("blocks_per_lun             : %d\r\n"), nand_onfi_params_ptr->blocks_per_lun); full_page = full_page + temp;
		temp.Format(_T("lun_count                  : %d\r\n"), nand_onfi_params_ptr->lun_count); full_page = full_page + temp;
		temp.Format(_T("addr_cycles                : %d\r\n"), nand_onfi_params_ptr->addr_cycles); full_page = full_page + temp;
		temp.Format(_T("bits_per_cell              : %d\r\n"), nand_onfi_params_ptr->bits_per_cell); full_page = full_page + temp;
		temp.Format(_T("bb_per_lun                 : %d\r\n"), nand_onfi_params_ptr->bb_per_lun); full_page = full_page + temp;
		temp.Format(_T("block_endurance            : %d\r\n"), nand_onfi_params_ptr->block_endurance); full_page = full_page + temp;
		temp.Format(_T("guaranteed_good_blocks     : %d\r\n"), nand_onfi_params_ptr->guaranteed_good_blocks); full_page = full_page + temp;
		temp.Format(_T("guaranteed_block_endurance : %d\r\n"), nand_onfi_params_ptr->guaranteed_block_endurance); full_page = full_page + temp;
		temp.Format(_T("programs_per_page          : %d\r\n"), nand_onfi_params_ptr->programs_per_page); full_page = full_page + temp;
		temp.Format(_T("ppage_attr                 : %d\r\n"), nand_onfi_params_ptr->ppage_attr); full_page = full_page + temp;
		temp.Format(_T("ecc_bits                   : %d\r\n"), nand_onfi_params_ptr->ecc_bits); full_page = full_page + temp;
		temp.Format(_T("interleaved_bits           : %d\r\n"), nand_onfi_params_ptr->interleaved_bits); full_page = full_page + temp;
		temp.Format(_T("interleaved_ops            : %d\r\n"), nand_onfi_params_ptr->interleaved_ops); full_page = full_page + temp;
		//memcpy(string, (char *)nand_onfi_params_ptr->reserved3, 13); string[13] = 0;
		//temp.Format(_T("reserved3[13]              : %d\r\n"), nand_onfi_params_ptr->reserved3[13]); full_page = full_page + temp;
		
		temp.Format(_T("\r\nelectrical parameter block\r\n")); full_page = full_page + temp;
		temp.Format(_T("io_pin_capacitance_max     : %d\r\n"), nand_onfi_params_ptr->io_pin_capacitance_max); full_page = full_page + temp;
		temp.Format(_T("async_timing_mode          : 0x%04x\r\n"), nand_onfi_params_ptr->async_timing_mode); full_page = full_page + temp;
		temp.Format(_T("program_cache_timing_mode  : 0x%04x\r\n"), nand_onfi_params_ptr->program_cache_timing_mode); full_page = full_page + temp;
		temp.Format(_T("t_prog                     : %d\r\n"), nand_onfi_params_ptr->t_prog); full_page = full_page + temp;
		temp.Format(_T("t_bers                     : %d\r\n"), nand_onfi_params_ptr->t_bers); full_page = full_page + temp;
		temp.Format(_T("t_r                        : %d\r\n"), nand_onfi_params_ptr->t_r); full_page = full_page + temp;
		temp.Format(_T("t_ccs                      : %d\r\n"), nand_onfi_params_ptr->t_ccs); full_page = full_page + temp;
		temp.Format(_T("src_sync_timing_mode       : 0x%04x\r\n"), nand_onfi_params_ptr->src_sync_timing_mode); full_page = full_page + temp;
		temp.Format(_T("src_ssync_features         : 0x%04x\r\n"), nand_onfi_params_ptr->src_ssync_features); full_page = full_page + temp;
		temp.Format(_T("clk_pin_capacitance_typ    : %d\r\n"), nand_onfi_params_ptr->clk_pin_capacitance_typ); full_page = full_page + temp;
		temp.Format(_T("io_pin_capacitance_typ     : %d\r\n"), nand_onfi_params_ptr->io_pin_capacitance_typ); full_page = full_page + temp;
		temp.Format(_T("input_pin_capacitance_typ  : %d\r\n"), nand_onfi_params_ptr->input_pin_capacitance_typ); full_page = full_page + temp;
		temp.Format(_T("input_pin_capacitance_max  : %d\r\n"), nand_onfi_params_ptr->input_pin_capacitance_max); full_page = full_page + temp;
		temp.Format(_T("driver_strength_support    : %d\r\n"), nand_onfi_params_ptr->driver_strength_support); full_page = full_page + temp;
		temp.Format(_T("t_int_r                    : %d\r\n"), nand_onfi_params_ptr->t_int_r); full_page = full_page + temp;
		temp.Format(_T("t_adl                      : %d\r\n"), nand_onfi_params_ptr->t_adl); full_page = full_page + temp;
		//memcpy(string, (char *)nand_onfi_params_ptr->reserved4, 8); string[8] = 0;
		//temp.Format(_T("reserved4[8]               : %d\r\n"), nand_onfi_params_ptr->reserved4[8]); full_page = full_page + temp;
		
		temp.Format(_T("\r\nvendor\r\n")); full_page = full_page + temp;
		temp.Format(_T("vendor_revision            : %d\r\n"), nand_onfi_params_ptr->vendor_revision); full_page = full_page + temp;

		struct nand_onfi_vendor_micron *nand_onfi_vendor_micron_ptr = (struct nand_onfi_vendor_micron *)nand_onfi_params_ptr->vendor;
		temp.Format(_T("\r\nnand_onfi_vendor_micron\r\n")); full_page = full_page + temp;
		temp.Format(_T("two_plane_read                    : %d\r\n"), nand_onfi_vendor_micron_ptr->two_plane_read); full_page = full_page + temp;
		temp.Format(_T("read_cache                        : %d\r\n"), nand_onfi_vendor_micron_ptr->read_cache); full_page = full_page + temp;
		temp.Format(_T("read_unique_id                    : %d\r\n"), nand_onfi_vendor_micron_ptr->read_unique_id); full_page = full_page + temp;
		temp.Format(_T("dq_imped                          : %d\r\n"), nand_onfi_vendor_micron_ptr->dq_imped); full_page = full_page + temp;
		temp.Format(_T("dq_imped_num_settings             : %d\r\n"), nand_onfi_vendor_micron_ptr->dq_imped_num_settings); full_page = full_page + temp;
		temp.Format(_T("dq_imped_feat_addr                : %d\r\n"), nand_onfi_vendor_micron_ptr->dq_imped_feat_addr); full_page = full_page + temp;
		temp.Format(_T("rb_pulldown_strength              : %d\r\n"), nand_onfi_vendor_micron_ptr->rb_pulldown_strength); full_page = full_page + temp;
		temp.Format(_T("rb_pulldown_strength_feat_addr    : %d\r\n"), nand_onfi_vendor_micron_ptr->rb_pulldown_strength_feat_addr); full_page = full_page + temp;
		temp.Format(_T("rb_pulldown_strength_num_settings : %d\r\n"), nand_onfi_vendor_micron_ptr->rb_pulldown_strength_num_settings); full_page = full_page + temp;
		temp.Format(_T("otp_mode                          : %d\r\n"), nand_onfi_vendor_micron_ptr->otp_mode); full_page = full_page + temp;
		temp.Format(_T("otp_page_start                    : %d\r\n"), nand_onfi_vendor_micron_ptr->otp_page_start); full_page = full_page + temp;
		temp.Format(_T("otp_data_prot_addr                : %d\r\n"), nand_onfi_vendor_micron_ptr->otp_data_prot_addr); full_page = full_page + temp;
		temp.Format(_T("otp_num_pages                     : %d\r\n"), nand_onfi_vendor_micron_ptr->otp_num_pages); full_page = full_page + temp;
		temp.Format(_T("otp_feat_addr                     : %d\r\n"), nand_onfi_vendor_micron_ptr->otp_feat_addr); full_page = full_page + temp;
		temp.Format(_T("read_retry_options                : %d\r\n"), nand_onfi_vendor_micron_ptr->read_retry_options); full_page = full_page + temp;
		//temp.Format(_T("reserved[72]                      : %d\r\n"), nand_onfi_vendor_micron_ptr->reserved[72]); full_page = full_page + temp;
		temp.Format(_T("param_revision                    : %d\r\n"), nand_onfi_vendor_micron_ptr->param_revision); full_page = full_page + temp;

		//memcpy(string, (char *)nand_onfi_params_ptr->vendor, 88); string[88] = 0;
		//temp.Format(_T("vendor[88]                 : %s\r\n"), A2W(string)); full_page = full_page + temp;
		temp.Format(_T("\r\ncrc                        : 0x%04x\r\n"), nand_onfi_params_ptr->crc); full_page = full_page + temp;



		pBoxOne->SetWindowTextW(full_page);
	}
	else
	{
		MessageBox(_T("USB 未连接"));
	}
}


void CUSBHOSTAPPDlg::OnBnClickedButton6()
{
	// TODO:  在此添加控件通知处理程序代码
	int ret;
	int actual_len = 0;
	CString str;

	if (handle != NULL)
	{
		bulk_thread_finished = 1;
		bulk_rev_thread_ptr = AfxBeginThread(bulk_rev_thread, (LPVOID)1);
		//while (bulk_thread_finished == 1);
		//TRACE("read page %dx\n", bulk_thread_finished);
		*(u32 *)(send_buf) = 0x00007f7e | (TESTWRconsistent << 16);
		GetDlgItem(IDC_EDIT4)->GetWindowText(str);
		*(u32 *)(send_buf + 12) = _tcstol(str, NULL, 10);

		ret = libusb_bulk_transfer(handle, BULK_SEND_EP, send_buf, SEND_BUFF_LEN, &actual_len, 0);

		if (ret != 0)
		{
			//MessageBox(_T("USB 发送失败 %s"));
			TRACE("ReadPage Usb faild, err: %s\n", libusb_error_name(ret));
		}


		//start = clock();

		//int k = 0;

		while (bulk_thread_finished == 0);

		u8 * tmpptr = rev_buf;
		
		if (*(u32*)(tmpptr + 12) == 1)
		{
			MessageBox(_T("读写一致检查成功"));
		}
		else
		{
			MessageBox(_T("读写一致检查失败"));
		}

	}
	else
	{
		MessageBox(_T("USB 未连接"));
	}
}


void CUSBHOSTAPPDlg::OnBnClickedButton7()
{
	// TODO:  在此添加控件通知处理程序代码
	int ret;
	int actual_len = 0;
	CString str;

	if (handle != NULL)
	{
		bulk_thread_finished = 1;
		bulk_rev_thread_ptr = AfxBeginThread(bulk_rev_thread, (LPVOID)1);
		//while (bulk_thread_finished == 1);
		//TRACE("read page %dx\n", bulk_thread_finished);
		*(u32 *)(send_buf) = 0x00007f7e | (TESTwritespeed << 16);
		GetDlgItem(IDC_EDIT4)->GetWindowText(str);
		*(u32 *)(send_buf + 12) = _tcstol(str, NULL, 10);
		ret = libusb_bulk_transfer(handle, BULK_SEND_EP, send_buf, SEND_BUFF_LEN, &actual_len, 0);

		if (ret != 0)
		{
			//MessageBox(_T("USB 发送失败 %s"));
			TRACE("ReadPage Usb faild, err: %s\n", libusb_error_name(ret));
		}


		//start = clock();

		//int k = 0;

		while (bulk_thread_finished == 0);

		u8 * tmpptr = rev_buf + 12;
		CString tmpstring;
		tmpstring.Format(_T("写入速度 %f MB/s\r\n"), *(float*)(tmpptr));

		MessageBox(tmpstring);
	}
	else
	{
		MessageBox(_T("USB 未连接"));
	}
}


void CUSBHOSTAPPDlg::OnBnClickedButton8()
{
	// TODO:  在此添加控件通知处理程序代码
	int ret;
	int actual_len = 0;
	CString str;

	if (handle != NULL)
	{
		bulk_thread_finished = 1;
		bulk_rev_thread_ptr = AfxBeginThread(bulk_rev_thread, (LPVOID)1);
		//while (bulk_thread_finished == 1);
		//TRACE("read page %dx\n", bulk_thread_finished);
		*(u32 *)(send_buf) = 0x00007f7e | (TESTreadspeed << 16);
		GetDlgItem(IDC_EDIT4)->GetWindowText(str);
		*(u32 *)(send_buf + 12) = _tcstol(str, NULL, 10);
		ret = libusb_bulk_transfer(handle, BULK_SEND_EP, send_buf, SEND_BUFF_LEN, &actual_len, 0);

		if (ret != 0)
		{
			//MessageBox(_T("USB 发送失败 %s"));
			TRACE("ReadPage Usb faild, err: %s\n", libusb_error_name(ret));
		}

		while (bulk_thread_finished == 0);


		u8 * tmpptr = rev_buf + 12;
		CString tmpstring;
		tmpstring.Format(_T("读取速度 %f MB/s\r\n"), *(float*)(tmpptr));
		MessageBox(tmpstring);
	}
	else
	{
		MessageBox(_T("USB 未连接"));
	}
}
