; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CAdministrator
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Si216Client.h"

ClassCount=13
Class1=CSi216ClientApp
Class2=CSi216ClientDlg
Class3=CAboutDlg

ResourceCount=9
Resource1=IDD_WaveSheet
Resource2=IDR_MAINFRAME
Class5=Udp
Class4=UdpRecv
Resource3=IDD_ALARMINFORM
Class6=CWaveSheet
Class7=CWaveSheet1
Resource4=IDD_Strain
Class8=CStrain
Class9=UDP1
Resource5=IDD_ParaSet
Class10=ParaSet
Resource6=IDD_ADMINISTRATOR
Class11=CAdministrator
Resource7=IDD_ABOUTBOX
Class12=CAlarmInform
Resource8=IDD_SI216CLIENT_DIALOG
Class13=CCheckHistoryAlarm
Resource9=IDD_CHECKHISTORYALARM

[CLS:CSi216ClientApp]
Type=0
HeaderFile=Si216Client.h
ImplementationFile=Si216Client.cpp
Filter=N

[CLS:CSi216ClientDlg]
Type=0
HeaderFile=Si216ClientDlg.h
ImplementationFile=Si216ClientDlg.cpp
Filter=D
LastObject=IDC_BUTTON1
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=Si216ClientDlg.h
ImplementationFile=Si216ClientDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_SI216CLIENT_DIALOG]
Type=1
Class=CSi216ClientDlg
ControlCount=16
Control1=IDOK,button,1342242817
Control2=IDC_TAB1,SysTabControl32,1342177280
Control3=IDC_blue,static,1342177283
Control4=IDC_background,static,1342177283
Control5=IDC_yellow,static,1342177283
Control6=IDC_dataErr,static,1342308352
Control7=IDC_normal,static,1342177283
Control8=IDC_fault,static,1342177283
Control9=IDC_alarm,static,1342177283
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_grey1,static,1342177283
Control14=IDC_grey2,static,1342177283
Control15=IDC_grey3,static,1342177283
Control16=IDC_start,button,1342242816

[CLS:UdpRecv]
Type=0
HeaderFile=UdpRecv.h
ImplementationFile=UdpRecv.cpp
BaseClass=CSocket
Filter=N
VirtualFilter=uq

[CLS:Udp]
Type=0
HeaderFile=Udp.h
ImplementationFile=Udp.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CSi216ClientDlg

[DLG:IDD_WaveSheet]
Type=1
Class=CWaveSheet1
ControlCount=6
Control1=IDC_LIST1,SysListView32,1350633473
Control2=IDC_Count1,edit,1350631552
Control3=IDC_STATIC,static,1342308352
Control4=IDC_PROGRESS1,msctls_progress32,1350565888
Control5=IDC_maxCycle,edit,1350631552
Control6=IDC_STATIC,static,1342308352

[CLS:CWaveSheet]
Type=0
HeaderFile=WaveSheet.h
ImplementationFile=WaveSheet.cpp
BaseClass=CDialog
Filter=D
LastObject=CWaveSheet
VirtualFilter=dWC

[CLS:CWaveSheet1]
Type=0
HeaderFile=WaveSheet1.h
ImplementationFile=WaveSheet1.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_maxCycle

[DLG:IDD_Strain]
Type=1
Class=CStrain
ControlCount=3
Control1=IDC_LIST1,SysListView32,1350633474
Control2=IDC_STATIC,static,1342308352
Control3=IDC_calMidd,edit,1350631552

[CLS:CStrain]
Type=0
HeaderFile=Strain.h
ImplementationFile=Strain.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_calMidd

[CLS:UDP1]
Type=0
HeaderFile=UDP1.h
ImplementationFile=UDP1.cpp
BaseClass=CSocket
Filter=N
VirtualFilter=uq

[DLG:IDD_ParaSet]
Type=1
Class=ParaSet
ControlCount=56
Control1=IDC_COMBO1,combobox,1344339970
Control2=IDC_LIST2,SysListView32,1350633482
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1073872896
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1073872896
Control15=IDC_STATIC,static,1342308352
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC,static,1342308352
Control18=IDC_id,edit,1350631553
Control19=IDC_type,edit,1350631553
Control20=IDC_orignWave,edit,1350631552
Control21=IDC_leftWave,edit,1350631552
Control22=IDC_rightWave,edit,1350631552
Control23=IDC_orignTemp,edit,1082196096
Control24=IDC_tempCoeff,edit,1082196096
Control25=IDC_alarmMin,edit,1350631552
Control26=IDC_alarmMax,edit,1350631552
Control27=IDC_tempCompID,edit,1350631552
Control28=IDC_tempCompCoeff,edit,1082196096
Control29=IDC_position,edit,1350631552
Control30=IDC_note,edit,1350631552
Control31=IDC_unit,edit,1350631553
Control32=IDC_k,edit,1082196096
Control33=IDC_STATIC,static,1073873547
Control34=IDC_shield,button,1342242819
Control35=IDC_accept,button,1342242816
Control36=IDC_addPara,button,1342242816
Control37=IDC_STATIC,static,1073872896
Control38=IDC_ReadParameter,button,1073807360
Control39=IDC_delete,button,1342242816
Control40=IDC_BUTTON1,button,1073807360
Control41=IDC_STATIC,static,1073872896
Control42=IDC_STATIC,static,1342308352
Control43=IDC_STATIC,static,1342308352
Control44=IDC_STATIC,static,1342308352
Control45=IDC_STATIC,static,1342308352
Control46=IDC_STATIC,static,1073872896
Control47=IDC_STATIC,static,1073872896
Control48=IDC_STATIC,static,1073872896
Control49=IDC_a,edit,1350631552
Control50=IDC_b,edit,1350631552
Control51=IDC_c,edit,1350631552
Control52=IDC_bhk,edit,1350631552
Control53=IDC_d,edit,1082196096
Control54=IDC_e,edit,1082196096
Control55=IDC_f,edit,1082196096
Control56=IDC_STATIC,static,1342308352

[CLS:ParaSet]
Type=0
HeaderFile=ParaSet.h
ImplementationFile=ParaSet.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_id
VirtualFilter=dWC

[DLG:IDD_ADMINISTRATOR]
Type=1
Class=CAdministrator
ControlCount=20
Control1=IDC_IPADDRESS1,SysIPAddress32,1342242816
Control2=IDC_modifyIP,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_autoSend,button,1342242819
Control5=IDC_reset,button,1342242816
Control6=IDC_STATIC,button,1342177287
Control7=IDC_STATIC,button,1342177287
Control8=IDC_saveResult,button,1342242819
Control9=IDC_sampleCheck,button,1342242819
Control10=IDC_saveSample,edit,1350631552
Control11=IDC_STATIC,static,1342308352
Control12=IDC_set,button,1342242816
Control13=IDC_UdpSendChoice,button,1342242819
Control14=IDC_udpSendFrame,edit,1350631552
Control15=IDC_STATIC,static,1342308352
Control16=IDC_pcID,edit,1350631552
Control17=IDC_sendFFTBuffer,button,1342242819
Control18=IDC_STATIC,button,1342177287
Control19=IDC_STATIC,static,1342308352
Control20=IDC_addressPort,edit,1350631552

[CLS:CAdministrator]
Type=0
HeaderFile=Administrator.h
ImplementationFile=Administrator.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_addressPort
VirtualFilter=dWC

[DLG:IDD_ALARMINFORM]
Type=1
Class=CAlarmInform
ControlCount=2
Control1=IDC_CHECKALARM,button,1342242816
Control2=IDC_LIST3,SysListView32,1350633474

[CLS:CAlarmInform]
Type=0
HeaderFile=AlarmInform.h
ImplementationFile=AlarmInform.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CAlarmInform

[DLG:IDD_CHECKHISTORYALARM]
Type=1
Class=CCheckHistoryAlarm
ControlCount=4
Control1=IDC_LIST1,SysListView32,1350631426
Control2=IDC_before,button,1342242816
Control3=IDC_Exit,button,1342242816
Control4=IDC_next,button,1342242816

[CLS:CCheckHistoryAlarm]
Type=0
HeaderFile=CheckHistoryAlarm.h
ImplementationFile=CheckHistoryAlarm.cpp
BaseClass=CDialog
Filter=D
LastObject=CCheckHistoryAlarm

