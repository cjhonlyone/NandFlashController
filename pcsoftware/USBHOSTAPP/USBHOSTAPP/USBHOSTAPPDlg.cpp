
// USBHOSTAPPDlg.cpp : ʵ���ļ�
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

#define SEND_BUFF_LEN    8640
#define RECV_BUFF_LEN    8640

#define VID 0x03fd
#define PID 0x0103	//jlq

volatile int exit_thread;

CWinThread* intertupt_rev_thread_ptr;
CWinThread* bulk_rev_thread_ptr;

unsigned char rev_buf[RECV_BUFF_LEN];   //usb ���ջ�����
unsigned char send_buf[SEND_BUFF_LEN];  //usb���ͻ����� 

libusb_device_handle *handle = NULL;
libusb_context *ctx = NULL;
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CUSBHOSTAPPDlg �Ի���



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


// CUSBHOSTAPPDlg ��Ϣ�������

BOOL CUSBHOSTAPPDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��


	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	USES_CONVERSION;
	SetDlgItemText(IDC_EDIT1, A2T("0"));// page
	SetDlgItemText(IDC_EDIT2, A2T("0"));// block

	static CFont font;
	font.DeleteObject();
	font.CreatePointFont(100, _T("Consolas"));
	GetDlgItem(IDC_EDIT1)->SetFont(&font);
	GetDlgItem(IDC_EDIT2)->SetFont(&font);
	GetDlgItem(IDC_EDIT3)->SetFont(&font);



	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CUSBHOSTAPPDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CUSBHOSTAPPDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

UINT intertupt_rev_thread(LPVOID pParam){  //�߳�Ҫ���õĺ���

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
		//		g_no_device_flag = 1; //��ֹһֱ���err
		//}
	}
	AfxEndThread(0);
	return 0;
}
UINT bulk_rev_thread(LPVOID pParam){  //�߳�Ҫ���õĺ���

	CUSBHOSTAPPDlg*p = (CUSBHOSTAPPDlg*)pParam;
	int i = 0;
	int size;
	int rec;

	TRACE("bulk_rev_thread started.\n");
	while (exit_thread == 0)
	{
		//if (g_no_device_flag){
		//	Sleep(50);
		//	continue;
		//}

		memset(rev_buf, 0, RECV_BUFF_LEN);
		rec = libusb_bulk_transfer(handle, BULK_RECV_EP, rev_buf, RECV_BUFF_LEN, &size, 20);

		if (rec == 0)
		{
			CEdit *pBoxOne;
			CString str;
			CString full_page, temp;
			pBoxOne = (CEdit*)p->GetDlgItem(IDC_EDIT3);

			u32 *ptr = (u32 *)rev_buf;
			if (*(u32 *)rev_buf == 0x49464e4f)
			{
				for (int i = 0; i < 256; i++)
				{
					if (i % 16 == 0)
					{
						temp.Format(_T("%08X "), i);
						full_page = full_page + temp;
					}

					if ((*(u8*)(rev_buf+i) < 128) && (*(u8*)(rev_buf+i) > 31))
					{
						temp.Format(_T(" %c"), *(u8*)(rev_buf + i));
					}
					else
					{
						temp.Format(_T("%02x"), *(u8*)(rev_buf + i));
					}
					full_page = full_page + temp;

					if (((i & 15) == 15) && (i != 0))
					{
						temp.Format(_T("\r\n")); full_page = full_page + temp;
					}
					else if ((i & 3) == 3)
					{
						temp.Format(_T(" ")); full_page = full_page + temp;
					}
					

				}
				temp.Format(_T("Number of data bytes per Page %d\r\n"), *(u32*)(rev_buf + 80));
				full_page = full_page + temp;
				temp.Format(_T("Number of spare bytes per Page %d\r\n"), *(u32*)(rev_buf + 84));
				full_page = full_page + temp;
				temp.Format(_T("Number of pages per block %d\r\n"), *(u32*)(rev_buf + 92));
				full_page = full_page + temp;
				temp.Format(_T("Number of blocks per LUN %d\r\n"), *(u32*)(rev_buf + 96));
				full_page = full_page + temp;
				temp.Format(_T("Number of LUNs per chip enable %d\r\n"), *(u8*)(rev_buf + 100));
				full_page = full_page + temp;
			}
			else
			{
				for (int i = 0; i < (2048 + 112); i++)
				{
					if (i % 4 == 0)
					{
						temp.Format(_T("%08X "), i << 2);
						full_page = full_page + temp;
					}


					temp.Format(_T("%08X"), *(u32*)(rev_buf + i * 4));
					full_page = full_page + temp;


					if (((i & 0x00000003) == 0x00000003) && (i != 0))
					{
						temp.Format(_T("\r\n"));
					}
					else
					{
						temp.Format(_T(" "));
					}
					full_page = full_page + temp;

				}
			}


			pBoxOne->SetWindowTextW(full_page);
		}
		//else
		//{
		//	TRACE("bulk  ep rev faild, err: %s\n", libusb_error_nalibusb_handle_eventsme(rec));
		//	if (rec == LIBUSB_ERROR_IO)
		//		g_no_device_flag = 1; //��ֹһֱ���err
		//}
	}
	AfxEndThread(0);
	return 0;
}
void CUSBHOSTAPPDlg::OnBnClickedButton1()
{
	
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (handle != NULL)
		goto usb_exit;
	int ret;
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
	//intertupt_rev_thread_ptr = AfxBeginThread(intertupt_rev_thread, (LPVOID)this);
	bulk_rev_thread_ptr = AfxBeginThread(bulk_rev_thread, (LPVOID)this);
	g_no_device_flag = 0;
	MessageBox(_T("USB���ӳɹ�"));
	return;

usb_error:
	MessageBox(_T("�޷�����USB�豸"));
	return;

usb_exit:
	MessageBox(_T("USB�豸�Ѿ�����"));
	return;
	//ret = libusb_bulk_transfer(handle, edp2in, rxbuffer, len, &actual_len, 1);
	//ret = libusb_interrupt_transfer(handle, edp2in, rxbuffer, len, &actual_len, 10);


}




void CUSBHOSTAPPDlg::OnBnClickedButton2()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int ret;
	int actual_len = 0;
	CString str;

	if (handle != NULL)
	{
		*(u32 *)(send_buf) = 0x00007f7e;

		GetDlgItem(IDC_EDIT1)->GetWindowText(str);
		*(u32 *)(send_buf + 4) = _tcstol(str, NULL, 10);

		GetDlgItem(IDC_EDIT2)->GetWindowText(str);
		*(u32 *)(send_buf + 8) = _tcstol(str, NULL, 10);

		ret = libusb_bulk_transfer(handle, BULK_SEND_EP, send_buf, 16, &actual_len, 0);
		//ret = libusb_bulk_transfer(handle, INT_SEND_EP, send_buf, 16, &actual_len, 0);
		if (ret != 0)
		{
			MessageBox(_T("USB ����ʧ�� %s"));
			TRACE("interrupt  ep rev faild, err: %s\n", libusb_error_name(ret));
		}
		*(u32 *)(send_buf) = 0x00017f7e;

		ret = libusb_bulk_transfer(handle, BULK_SEND_EP, send_buf, 16, &actual_len, 0);
		if (ret != 0)
		{
			MessageBox(_T("USB ����ʧ��"));
		}
	}
	else
	{
		MessageBox(_T("USB δ����"));
	}


}
void CUSBHOSTAPPDlg::OnClose()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (handle != NULL){
		exit_thread = 1;
		WaitForSingleObject(bulk_rev_thread_ptr->m_hThread, INFINITE);



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
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (handle != NULL){
	exit_thread = 1;
	//WaitForSingleObject(intertupt_rev_thread_ptr->m_hThread, INFINITE);
	WaitForSingleObject(bulk_rev_thread_ptr->m_hThread, INFINITE);
	//Sleep(100);



		libusb_release_interface(handle, 0);
		libusb_release_interface(handle, 1);
		libusb_close(handle);
		libusb_exit(NULL);
		handle = NULL;
	}
}


void CUSBHOSTAPPDlg::OnBnClickedButton4()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int ret;
	int actual_len = 0;
	CString str;

	if (handle != NULL)
	{
		*(u32 *)(send_buf) = 0x00007f7e;

		GetDlgItem(IDC_EDIT1)->GetWindowText(str);
		*(u32 *)(send_buf + 4) = _tcstol(str, NULL, 10);

		GetDlgItem(IDC_EDIT2)->GetWindowText(str);
		*(u32 *)(send_buf + 8) = _tcstol(str, NULL, 10);

		ret = libusb_bulk_transfer(handle, BULK_SEND_EP, send_buf, 16, &actual_len, 0);
		//ret = libusb_bulk_transfer(handle, INT_SEND_EP, send_buf, 16, &actual_len, 0);
		if (ret != 0)
		{
			MessageBox(_T("USB ����ʧ�� %s"));
			TRACE("interrupt  ep rev faild, err: %s\n", libusb_error_name(ret));
		}
		*(u32 *)(send_buf) = 0x00027f7e;

		ret = libusb_bulk_transfer(handle, BULK_SEND_EP, send_buf, 16, &actual_len, 0);
		if (ret != 0)
		{
			MessageBox(_T("USB ����ʧ��"));
		}
	}
	else
	{
		MessageBox(_T("USB δ����"));
	}
}


void CUSBHOSTAPPDlg::OnBnClickedButton5()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int ret;
	int actual_len = 0;
	CString str;

	if (handle != NULL)
	{
		*(u32 *)(send_buf) = 0x00067f7e;


		ret = libusb_bulk_transfer(handle, BULK_SEND_EP, send_buf, 16, &actual_len, 0);
		//ret = libusb_bulk_transfer(handle, INT_SEND_EP, send_buf, 16, &actual_len, 0);
		if (ret != 0)
		{
			MessageBox(_T("USB ����ʧ�� %s"));
			TRACE("interrupt  ep rev faild, err: %s\n", libusb_error_name(ret));
		}
	}
	else
	{
		MessageBox(_T("USB δ����"));
	}
}


void CUSBHOSTAPPDlg::OnBnClickedButton6()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int ret;
	int actual_len = 0;
	CString str;

	if (handle != NULL)
	{
		*(u32 *)(send_buf) = 0x00037f7e;


		ret = libusb_bulk_transfer(handle, BULK_SEND_EP, send_buf, 16, &actual_len, 0);
		//ret = libusb_bulk_transfer(handle, INT_SEND_EP, send_buf, 16, &actual_len, 0);
		if (ret != 0)
		{
			MessageBox(_T("USB ����ʧ�� %s"));
			TRACE("interrupt  ep rev faild, err: %s\n", libusb_error_name(ret));
		}
	}
	else
	{
		MessageBox(_T("USB δ����"));
	}
}


void CUSBHOSTAPPDlg::OnBnClickedButton7()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int ret;
	int actual_len = 0;
	CString str;

	if (handle != NULL)
	{
		*(u32 *)(send_buf) = 0x00047f7e;


		ret = libusb_bulk_transfer(handle, BULK_SEND_EP, send_buf, 16, &actual_len, 0);
		//ret = libusb_bulk_transfer(handle, INT_SEND_EP, send_buf, 16, &actual_len, 0);
		if (ret != 0)
		{
			MessageBox(_T("USB ����ʧ�� %s"));
			TRACE("interrupt  ep rev faild, err: %s\n", libusb_error_name(ret));
		}
	}
	else
	{
		MessageBox(_T("USB δ����"));
	}
}


void CUSBHOSTAPPDlg::OnBnClickedButton8()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int ret;
	int actual_len = 0;
	CString str;

	if (handle != NULL)
	{
		*(u32 *)(send_buf) = 0x00057f7e;


		ret = libusb_bulk_transfer(handle, BULK_SEND_EP, send_buf, 16, &actual_len, 0);
		//ret = libusb_bulk_transfer(handle, INT_SEND_EP, send_buf, 16, &actual_len, 0);
		if (ret != 0)
		{
			MessageBox(_T("USB ����ʧ�� %s"));
			TRACE("interrupt  ep rev faild, err: %s\n", libusb_error_name(ret));
		}
	}
	else
	{
		MessageBox(_T("USB δ����"));
	}
}
