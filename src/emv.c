#include <stdlib.h>
#include <stdio.h>
#include "mruby.h"
#include "mruby/compile.h"
#include "mruby/value.h"
#include "mruby/array.h"
#include "mruby/string.h"
#include "mruby/hash.h"

#include "ui.h"
#include "osal.h"
#include "emvlib_Prolin.h"

/*#define E_EMV_ERROR (mrb_class_get_under(mrb,mrb_class_get_under(mrb,mrb_class_get(mrb,"PAX"),"EMV"),"EMVError"))*/

/*#include "CLEntryAPI_Prolin.h"*/
/*#include "ClssApi_Wave_prolin.h"*/
/*#include "ClssApi_MC_prolin.h"*/

/*Callbacks*/

/**
*	@fn	int cEMVPedVerifyPlainPin (uchar IccSlot,uchar *ExpPinLenIn,uchar *IccRespOut,uchar Mode,ulong TimeoutMs)
*	@brief	EMV回调函数，实现脱机明文PIN获取及明文PIN的校验功能
*	@param	[in] IccSlot     卡片所在的卡座号
*	@param	[in] ExpPinLenIn 可输入的合法密码长度字符串
*	@param	[in] Mode		 IC卡命令模式
*	@param	[in] TimeoutMs	 输入PIN的超时时间
*	@param	[out] IccRespOut 卡片响应的状态码
*	@return int
*	@author	Prolin App developer
*	@date	2013-05-20
*/
int cEMVPedVerifyPlainPin (uchar IccSlot,uchar *ExpPinLenIn,uchar *IccRespOut,uchar Mode,ulong TimeoutMs)
{
	int iRet;
	int iPinX = 0, iPinY = 0;

	// OsSleep(50);
	OsScrGetSize(&iPinX, &iPinY);
	iPinX /= 3;
	iPinY -= iPinY / 4;
	OsPedSetAsteriskLayout(iPinX, iPinY, 24, RGB(0x00, 0x00, 0x00), PED_ASTERISK_ALIGN_CENTER);
	display_clear();
	xdisplay("ENTER PIN: ", strlen("ENTER PIN: "), 4, 2);
	sleep(1);
	iRet = OsPedVerifyPlainPin(0, "0,4,5,6,7,8,9,10,11,12", 0x00, 30000, IccRespOut);

	if(RET_OK == iRet)
	{
		return PED_RET_OK;
	}
	else if(ERR_PED_NO_PIN_INPUT == iRet)
	{
		return PED_RET_ERR_NO_PIN_INPUT;
	}
	else if(ERR_PED_PIN_INPUT_CANCEL == iRet)
	{
		return PED_RET_ERR_INPUT_CANCEL;
	}
	else if(ERR_PED_ICC_INIT_ERR == iRet)
	{
		return PED_RET_ERR_ICC_NO_INIT;
	}
	else if(ERR_PED_NO_ICC == iRet)
	{
		return PED_RET_ERR_NO_ICC;
	}
	else if(ERR_PED_WAIT_INTERVAL == iRet)
	{
		return PED_RET_ERR_WAIT_INTERVAL;
	}
	else
	{
		return iRet;
	}
	return EMV_OK;
}

/**
*	@fn int cEMVPedVerifyCipherPin (uchar IccSlot,uchar *ExpPinLenIn,RSA_PINKEY *RsaPinKeyIn, uchar *IccRespOut, uchar Mode, ulong TimeoutMs)
*	@brief	EMV回调函数，实现脱机密文PIN的获取和密文PIN的校验
*	@param	[in] IccSlot 卡片所在的卡座号
*	@param	[in] ExpPinLenIn	可输入的合法密码长度字符串
*	@param	[in] RsaPinKeyIn	加密所需数据结构
*	@param	[in] Mode	IC卡命令模式
*	@param	[in] TimeoutMs	输入PIN的超时时间
*	@param	[out] IccRespOut 卡片响应的状态码
*	@return int
*	@author	Prolin App developer
*	@date	2013-05-20
*/
int cEMVPedVerifyCipherPin (uchar IccSlot,uchar *ExpPinLenIn,RSA_PINKEY *RsaPinKeyIn, uchar *IccRespOut, uchar Mode, ulong TimeoutMs)
{
	// debug
	// display("cEMVPedVerifyCipherPin");
	// sleep(2);
	return 0;
}

/**
*	@fn	int  cEMVIccIsoCommand(uchar ucslot, APDU_SEND *tApduSend, APDU_RESP *tApduRecv)
*	@brief	EMV回调函数，实现接触式读卡操作
*	@param	[in] ucslot 卡片逻辑通道号
*	@param	[in] tApduSend	发送给ICC卡命令数据结构
*	@param	[out] tApduRecv 从ICC卡返回的数据结构
*	@return int
*	@author	Prolin App developer
*	@date	2013-05-20
*/
uchar cEMVIccIsoCommand(uchar ucslot, APDU_SEND *tApduSend, APDU_RESP *tApduRecv)
{
	int iRet;
  int iLogCnt;
  char szLogBuf[1024];

	iRet = IccIsoCommand(ucslot, tApduSend, tApduRecv);
	if(iRet != 0)
	{
		return 0x01;
	}

	for(iLogCnt = 0; iLogCnt < 4; iLogCnt++)
	{
		sprintf(szLogBuf+iLogCnt*3, "%02X ", *(tApduSend->Command+iLogCnt));
	}
	if (tApduSend->Lc != 0)
	{
		sprintf(szLogBuf+4*3, "%02X ", (char)tApduSend->Lc);
		for(iLogCnt = 0; iLogCnt < tApduSend->Lc; iLogCnt++)
		{
			sprintf(szLogBuf+(iLogCnt+5)*3, "%02X ", *(tApduSend->DataIn+iLogCnt));
		}
		if (tApduSend->Le != 0)
		{
			sprintf(szLogBuf+(5+tApduSend->Lc)*3, "%02X ", (char)tApduSend->Le);
		}
	}
	else
	{
		sprintf(szLogBuf+4*3, "%02X ", (char)tApduSend->Le);
	}

	for(iLogCnt = 0; iLogCnt < tApduRecv->LenOut; iLogCnt++)
	{
		sprintf(szLogBuf+iLogCnt*3, "%02X ", *(tApduRecv->DataOut+iLogCnt));
	}
	sprintf(szLogBuf+iLogCnt*3, "%02X ", tApduRecv->SWA);
	sprintf(szLogBuf+(iLogCnt+1)*3, "%02X ", tApduRecv->SWB);
	return 0;
}

// Callback function required by EMV core.
// in EMV ver 2.1+, this function is called before GPO
int cEMVSetParam(void)
{
	// debug
	// display("cEMVSetParam");
	return 0;
}

unsigned char cEMVSM3(unsigned char *paucMsgIn, int nMsglenIn,unsigned char *paucResultOut)
{
	// debug
	// display("cEMVSM3");
	// sleep(2);
	return 0;
}

unsigned char cEMVSM2Verify(unsigned char *paucPubkeyIn,unsigned char *paucMsgIn,int nMsglenIn, unsigned char *paucSignIn, int nSignlenIn)
{
	// debug
	// display("cEMVSM2Verify");
	// sleep(2);
	return 0;
}

// it is acallback function for EMV kernel,
// for displaying a amount input box,
// developer customize
int cEMVInputAmount(ulong *AuthAmt, ulong *CashBackAmt)
{
	// debug
	// display("cEMVInputAmount");
	return EMV_OK;
}

// Callback function required by EMV core.
// Wait holder enter PIN.
// developer customized.
// Modified by Kim_LinHB 2014-6-8 v1.01.0000
int cEMVGetHolderPwd(int iTryFlag, int iRemainCnt, uchar *pszPlainPin)
{
	int iResult;
	int iPinX = 0, iPinY = 0;
	unsigned char	ucRet, szBuff[30], szAmount[15], sPinBlock[8];

	// OsSleep(50);
	OsScrGetSize(&iPinX, &iPinY);
	iPinX /= 3;
	iPinY -= iPinY / 4;

	// online PIN
	if (pszPlainPin == NULL)
	{
		OsPedSetAsteriskLayout(iPinX, iPinY, 24, RGB(0x00, 0x00, 0x00), PED_ASTERISK_ALIGN_CENTER);
		display_clear();
		xdisplay("ENTER PIN: ", strlen("ENTER PIN: "), 4, 2);
		sleep(1);
		iResult = PedGetPinBlock(1, "0,4,5,6,7,8", "05704230890341069", sPinBlock, 0, 30000);

		if (iResult == 0)
		{
			return EMV_OK;
		}
		else if (iResult == PED_RET_ERR_INPUT_CANCEL)
		{
			return EMV_USER_CANCEL;
		}
		else
		{
			return EMV_NO_PINPAD;
		}
	}

	// Offline plain/enciphered PIN processing below
	if(iRemainCnt == 1)
	{
		// "LAST ENTER PIN"
		display_clear();
		xdisplay("LAST CHANCE", strlen("LAST CHANCE"), 4, 3);
		sleep(2);
	}

	if (iTryFlag == 0)
	{
	  //  GetDispAmount(szAmount, szAmount);
		// display("Amount x ..");
	}
	else
	{
		// "PIN ERR, RETRY"
		display_clear();
		xdisplay("INCORRECT PIN", strlen("INCORRECT PIN"), 3, 2);
		sleep(2);
	}

	// Offline PIN, done by core itself since EMV core V25_T1. Application only needs to display prompt message.
	// In this mode, cEMVGetHolderPwd() will be called twice. the first time is to display message to user,
	// then back to kernel and wait PIN. afterwards kernel call this again and inform the process result.
	if (pszPlainPin[0] == EMV_PED_TIMEOUT)
	{
		// EMV core has processed PIN entry and it's timeout
		// "PED ERROR"
		// display("EMV_TIME_OUT ..");
		return EMV_TIME_OUT;
	}
	else if (pszPlainPin[0] == EMV_PED_WAIT)
	{
		// API is called too frequently
		sleep(2);
		// OsPedSetAsteriskLayout(iPinX, iPinY, 24, RGB(0x00, 0x00, 0x00), PED_ASTERISK_ALIGN_CENTER);
		return EMV_OK;
	}
	else if (pszPlainPin[0] == EMV_PED_FAIL)
	{
		// EMV core has processed PIN entry and PED failed.
		// "PED ERROR"
		// display("EMV_NO_PINPAD ..");
		return EMV_NO_PINPAD;
	}
	else
	{
		// EMV PIN not processed yet. So just display.
		// OsPedSetAsteriskLayout(iPinX, iPinY, 24, RGB(0x00, 0x00, 0x00), PED_ASTERISK_ALIGN_CENTER);
		return EMV_OK;
	}
}

// 持卡人认证例程
// Callback function required by EMV core.
// Don't need to care about this function
int cCertVerify(void)
{
	// debug
	// display("cCertVerify");
	// sleep(2);
	return -1;
}

unsigned char  cEMVPiccIsoCommand(unsigned char cid,APDU_SEND *ApduSend,APDU_RESP *ApduRecv)
{
	// debug
	// display("cEMVPiccIsoCommand");
	// sleep(2);
	return 0;
}

// 处理DOL的过程中，EMV库遇到不识别的TAG时会调用该回调函数，要求应用程序处理
// 如果应用程序无法处理，则直接返回-1，提供该函数只为解决一些不符合EMV的特殊
// 应用程序的要求，一般情况下返回-1即可
// Callback function required by EMV core.
// When processing DOL, if there is a tag that EMV core doesn't know about, core will call this function.
// developer should offer processing for proprietary tag.
// if really unable to, just return -1
int cEMVUnknowTLVData(ushort iTag, uchar *psDat, int iDataLen)
{
	// debug
	// display("cEMVUnknowTLVData");
	return 0;
}

// Modified by Kim_LinHB 2014-5-31
// for displaying a application list to card holder to select
// if there is only one application in the chip, then EMV kernel will not call this callback function
int cEMVWaitAppSel(int TryCnt, EMV_APPLIST List[], int AppNum)
{
	// debug
	// display("cEMVWaitAppSel");
	// sleep(2);
	return 0;
}

// 如果不需要提示密码验证成功，则直接返回就可以了
// Callback function required by EMV core.
// Display "EMV PIN OK" info. (plaintext/enciphered PIN)
// Modified by Kim_LinHB 2014-6-8 v1.01.0000
void cEMVVerifyPINOK(void)
{
	// debug
	// display("cEMVVerifyPINOK");
	// sleep(2);
  return;
}

/*Callbacks*/

/*[>CTLS Callbacks<]*/
/*[>Generical<]*/
/*unsigned char cPiccIsoCommand_Entry(uchar cid, APDU_SEND *ApduSend, APDU_RESP *ApduRecv) { return EMV_OK; };*/
/*void DelayMs(ushort Ms) { sleep(Ms/1000); };*/

/*[>MasterCard<]*/
/*unsigned char cPiccIsoCommand_MC(uchar cid, APDU_SEND *ApduSend, APDU_RESP *ApduRecv) { return EMV_OK; };*/
/*int cClssGetUnknowTLVData_MC(uchar *pucTag, uchar ucTagLen, uint unLen, uchar *pucdata) { return EMV_OK; };*/
/*[>Wave<]*/
/*int cClssGetUnknowTLVData_Wave(unsigned short usTag, unsigned char *pucData, int nLen) { return EMV_OK; };*/
/*uchar cPiccIsoCommand_Wave(uchar cid,APDU_SEND *ApduSend,APDU_RESP *ApduRecv) { return EMV_OK; };*/
/*[>CTLS Callbacks<]*/

static mrb_value
mrb_s_get_emv_parameter(mrb_state *mrb, mrb_value klass)
{
  EMV_PARAM parameter;
  mrb_value hash;

  memset(&parameter, 0, sizeof(parameter));

  EMVGetParameter(&parameter);
  hash = mrb_funcall(mrb, klass, "parameter_default", 0);

  /*TODO Scalone: loss data is posible in conversation from unsigned char to const char*/
  mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "MerchName")     , mrb_str_new(mrb , parameter.MerchName      , 256));
  mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "MerchCateCode") , mrb_str_new(mrb , parameter.MerchCateCode  , 2));
  mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "MerchId")       , mrb_str_new(mrb , &parameter.MerchId       , 15));
  mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "TermId")        , mrb_str_new(mrb , &parameter.TermId        , 8));
  mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "TerminalType")  , mrb_str_new(mrb , &parameter.TerminalType  , 1));
  mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "Capability")    , mrb_str_new(mrb , &parameter.Capability    , 3));
  mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "ExCapability")  , mrb_str_new(mrb , &parameter.ExCapability  , 5));
  mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "TransCurrExp")  , mrb_str_new(mrb , &parameter.TransCurrExp  , 1));
  mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "ReferCurrExp")  , mrb_str_new(mrb , &parameter.ReferCurrExp  , 1));
  mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "ReferCurrCode") , mrb_str_new(mrb , &parameter.ReferCurrCode , 2));
  mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "CountryCode")   , mrb_str_new(mrb , &parameter.CountryCode   , 2));
  mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "TransCurrCode") , mrb_str_new(mrb , &parameter.TransCurrCode , 2));
  mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "ReferCurrCon")  , mrb_str_new(mrb , &parameter.ReferCurrCon  , 1));
  mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "TransType")     , mrb_str_new(mrb , &parameter.TransType     , 1));
  mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "ForceOnline")   , mrb_str_new(mrb , &parameter.ForceOnline   , 1));
  mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "GetDataPIN")    , mrb_str_new(mrb , &parameter.GetDataPIN    , 1));
  mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "SurportPSESel") , mrb_str_new(mrb , &parameter.SurportPSESel , 1));

  return hash;
}

static void
set_emv_parameter(mrb_state *mrb, mrb_value klass, mrb_value hash)
{
  EMV_PARAM parameter;
  mrb_value value;
  mrb_int iValue;

  memset(&parameter, 0, sizeof(parameter));
  EMVSetParameter(&parameter);

  memset(&parameter, 0, sizeof(parameter));

  EMVGetParameter(&parameter);

  /*memset(&value, 0, sizeof(value));*/
  /*value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "MerchName"));*/
  /*memcpy(&parameter.MerchName, RSTRING_PTR(value), 256);*/

  /*memset(&value, 0, sizeof(value));*/
  /*value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "MerchCateCode"));*/
  /*memcpy(&parameter.MerchCateCode, RSTRING_PTR(value), 2);*/

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "MerchId"));
  memcpy(&parameter.MerchId, RSTRING_PTR(value), 15);
  /*display("MerchId");*/
  /*display("[%s]", &parameter.MerchId);*/

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "TermId"));
  memcpy(&parameter.TermId, RSTRING_PTR(value), 8);
  /*display("TermId[%s]", &parameter.TermId);*/

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "TerminalType"));
  /*memcpy(&parameter.TerminalType, RSTRING_PTR(value), 1);*/
  parameter.TerminalType = 0x22;
  /*display("TermType[%02X]", parameter.TerminalType);*/

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "Capability"));
  memcpy(&parameter.Capability, RSTRING_PTR(value), 3);
  /*display("Cap[%02X-%02X-%02X]", parameter.Capability[0], parameter.Capability[1], parameter.Capability[2]);*/

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "ExCapability"));
  memcpy(&parameter.ExCapability, RSTRING_PTR(value), 5);
  /*display("ExC[%02X-%02X-%02X-%02X-%02X]", parameter.ExCapability[0], parameter.ExCapability[1], parameter.ExCapability[2], parameter.ExCapability[3], parameter.ExCapability[4]);*/

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "TransCurrExp"));
  /*memcpy(&parameter.TransCurrExp, RSTRING_PTR(value), 1);*/
  parameter.TransCurrExp = 0x02;
  /*display("TransCurrExp[%02X]", parameter.TransCurrExp);*/

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "ReferCurrExp"));
  /*memcpy(&parameter.ReferCurrExp, RSTRING_PTR(value), 1);*/
  parameter.ReferCurrExp = 0x02;
  /*display("ReferCurrExp[%02X]", parameter.ReferCurrExp);*/

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "ReferCurrCode"));
  memcpy(&parameter.ReferCurrCode, RSTRING_PTR(value), 2);
  /*display("ReferCurrCode[%02X-%02X]", parameter.ReferCurrCode[0], parameter.ReferCurrCode[1]);*/

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "CountryCode"));
  memcpy(&parameter.CountryCode, RSTRING_PTR(value), 2);
  /*display("CountryCode[%02X-%02X]", parameter.CountryCode[0], parameter.CountryCode[1]);*/

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "TransCurrCode"));
  memcpy(&parameter.TransCurrCode, RSTRING_PTR(value), 2);
  /*display("TransCurrCode[%02X-%02X]", parameter.TransCurrCode[0], parameter.TransCurrCode[1]);*/

  memset(&value, 0, sizeof(value));
  iValue = mrb_fixnum(mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "ReferCurrCon")));
  parameter.ReferCurrCon = iValue;
  /*display("ReferCurrCon[%d]", parameter.ReferCurrCon);*/

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "TransType"));
  memcpy(&parameter.TransType, RSTRING_PTR(value), 1);
  parameter.TransType = 0x01;
  /*display("TransType[%02X]", parameter.TransType);*/

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "ForceOnline"));
  /*memcpy(&parameter.ForceOnline, RSTRING_PTR(value), 1);*/
  parameter.ForceOnline = 0;
  /*display("ForceOnline[%02X]", parameter.ForceOnline);*/

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "GetDataPIN"));
  parameter.GetDataPIN = 1;
  /*display("GetDataPIN[%02X]", parameter.GetDataPIN);*/

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "SurportPSESel"));
  /*memcpy(&parameter.SurportPSESel, RSTRING_PTR(value), 1);*/
  parameter.SurportPSESel = 1;
  /*display("SurportPSESel[%02X]", parameter.SurportPSESel);*/

  EMVSetParameter(&parameter);
}

mrb_value
mrb_s_core_init(mrb_state *mrb, mrb_value klass)
{
  return mrb_fixnum_value(EMVCoreInit());
}

static mrb_value
mrb_s_set_emv_parameter(mrb_state *mrb, mrb_value klass)
{
  mrb_value hash;

  mrb_get_args(mrb, "o", &hash);

  if (mrb_hash_p(hash))
    set_emv_parameter(mrb, klass, hash);
  else
    mrb_raise(mrb, E_ARGUMENT_ERROR, "object isn't a hash");

  return mrb_nil_value();
}

static mrb_value
mrb_s_get_emv_app(mrb_state *mrb, mrb_value klass)
{
  EMV_APPLIST parameter;
  mrb_value hash;
  mrb_int index;
  int ret=EMV_NOT_FOUND;

  memset(&parameter, 0, sizeof(parameter));

  mrb_get_args(mrb, "i", &index);

  ret = EMVGetApp(index, &parameter);

  if (ret == EMV_OK)
  {
    hash = mrb_funcall(mrb, klass, "app_default", 0);
    /*TODO Scalone: loss data is posible in conversation from unsigned char to const char*/
    /*TODO Scalone AppName 16 instead 32 for now*/
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "AppName")         , mrb_str_new(mrb      , &parameter.AppName         , 16));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "AID")             , mrb_str_new(mrb      , &parameter.AID             , 16));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "AidLen")          , mrb_str_new(mrb      , &parameter.AidLen          , 1));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "SelFlag")         , mrb_str_new(mrb      , &parameter.SelFlag         , 1));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "Priority")        , mrb_str_new(mrb      , &parameter.Priority        , 1));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "TargetPer")       , mrb_str_new(mrb      , &parameter.TargetPer       , 1));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "MaxTargetPer")    , mrb_str_new(mrb      , &parameter.MaxTargetPer    , 1));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "FloorLimitCheck") , mrb_str_new(mrb      , &parameter.FloorLimitCheck , 1));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "RandTransSel")    , mrb_str_new(mrb      , &parameter.RandTransSel    , 1));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "VelocityCheck")   , mrb_str_new(mrb      , &parameter.VelocityCheck   , 1));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "FloorLimit")      , mrb_fixnum_value(parameter.FloorLimit));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "Threshold")       , mrb_str_new(mrb      , &parameter.Threshold       , 1));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "TACDenial")       , mrb_str_new(mrb      , &parameter.TACDenial       , 5));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "TACOnline")       , mrb_str_new(mrb      , &parameter.TACOnline       , 5));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "TACDefault")      , mrb_str_new(mrb      , &parameter.TACDefault      , 5));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "AcquierId")       , mrb_str_new(mrb      , &parameter.AcquierId       , 5));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "dDOL")            , mrb_str_new(mrb      , &parameter.dDOL            , 256));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "tDOL")            , mrb_str_new(mrb      , &parameter.tDOL            , 256));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "Version")         , mrb_str_new(mrb      , &parameter.Version         , 3));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "RiskManData")     , mrb_str_new(mrb      , &parameter.RiskManData     , 10));

    return hash;
  } else {
    return mrb_fixnum_value(ret);
    /*mrb_raisef(mrb, E_EMV_ERROR, "object isn't a hash", );*/
  }
}

static int
add_emv_app(mrb_state *mrb, mrb_value klass, mrb_value hash)
{
  EMV_APPLIST parameter;
  mrb_value value;
  mrb_int iValue;

  memset(&parameter, 0, sizeof(parameter));

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "AppName"));
  memcpy(&parameter.AppName, RSTRING_PTR(value), RSTRING_LEN(value));

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "AID"));
  memcpy(&parameter.AID, RSTRING_PTR(value), 14);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "AidLen"));
  memcpy(&parameter.AidLen, RSTRING_PTR(value), 1);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "SelFlag"));
  memcpy(&parameter.SelFlag, RSTRING_PTR(value), 1);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "Priority"));
  memcpy(&parameter.Priority, RSTRING_PTR(value), 1);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "TargetPer"));
  memcpy(&parameter.TargetPer, RSTRING_PTR(value), 1);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "MaxTargetPer"));
  memcpy(&parameter.MaxTargetPer, RSTRING_PTR(value), 1);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "FloorLimitCheck"));
  memcpy(&parameter.FloorLimitCheck, RSTRING_PTR(value), 1);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "RandTransSel"));
  memcpy(&parameter.RandTransSel, RSTRING_PTR(value), 1);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "VelocityCheck"));
  memcpy(&parameter.VelocityCheck, RSTRING_PTR(value), 1);
  parameter.VelocityCheck = 1;

  iValue = mrb_fixnum(mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "FloorLimit")));
  parameter.FloorLimit = iValue;

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "Threshold"));
  memcpy(&parameter.Threshold, RSTRING_PTR(value), 1);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "TACDenial"));
  memcpy(&parameter.TACDenial, RSTRING_PTR(value), 5);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "TACOnline"));
  memcpy(&parameter.TACOnline, RSTRING_PTR(value), 5);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "TACDefault"));
  memcpy(&parameter.TACDefault, RSTRING_PTR(value), 5);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "AcquierId"));
  memcpy(&parameter.AcquierId, RSTRING_PTR(value), 6);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "dDOL"));
  memcpy(&parameter.dDOL, RSTRING_PTR(value), RSTRING_LEN(value));

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "tDOL"));
  memcpy(&parameter.tDOL, RSTRING_PTR(value), RSTRING_LEN(value));

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "Version"));
  memcpy(&parameter.Version, RSTRING_PTR(value), RSTRING_LEN(value));

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "RiskManData"));
  memcpy(&parameter.RiskManData, RSTRING_PTR(value), 10);

  return EMVAddApp(&parameter);
}

static mrb_value
mrb_s_add_emv_app(mrb_state *mrb, mrb_value klass)
{
  mrb_value hash;
  mrb_int ret=EMV_OK;

  mrb_get_args(mrb, "o", &hash);

  if (mrb_hash_p(hash))
    ret = add_emv_app(mrb, klass, hash);
  else
    mrb_raise(mrb, E_ARGUMENT_ERROR, "object isn't a hash");

  return mrb_fixnum_value(ret);
}

static mrb_value
mrb_s_del_emv_app(mrb_state *mrb, mrb_value klass)
{
  mrb_value aid;
  mrb_int ret=EMV_OK;

  mrb_get_args(mrb, "s", &aid);

  if (mrb_string_p(aid))
    ret = EMVDelApp(RSTRING_PTR(aid), RSTRING_LEN(aid));
  else
    mrb_raise(mrb, E_ARGUMENT_ERROR, "object isn't a string");

  return mrb_fixnum_value(ret);
}

static mrb_value
mrb_s_del_emv_apps(mrb_state *mrb, mrb_value klass)
{
  return mrb_fixnum_value(EMVDelAllApp());
}

static mrb_value
mrb_s_get_emv_pki(mrb_state *mrb, mrb_value klass)
{
  EMV_CAPK parameter;
  mrb_value hash;
  mrb_int index;
  int ret=EMV_NOT_FOUND;

  memset(&parameter, 0, sizeof(parameter));

  mrb_get_args(mrb, "i", &index);

  ret = EMVGetCAPK(index, &parameter);

  if (ret == EMV_OK)
  {
    hash = mrb_funcall(mrb, klass, "pki_default", 0);
    /*TODO Scalone: loss data is posible in conversation from unsigned char to const char*/
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "RID")         , mrb_str_new(mrb , &parameter.RID         , 5));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "KeyID")       , mrb_str_new(mrb , &parameter.KeyID       , 1));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "HashInd")     , mrb_str_new(mrb , &parameter.HashInd     , 1));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "ArithInd")    , mrb_str_new(mrb , &parameter.ArithInd    , 1));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "ModulLen")    , mrb_str_new(mrb , &parameter.ModulLen    , 1));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "Modul")       , mrb_str_new(mrb , &parameter.Modul       , 248));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "ExponentLen") , mrb_str_new(mrb , &parameter.ExponentLen , 1));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "Exponent")    , mrb_str_new(mrb , &parameter.Exponent    , 3));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "ExpDate")     , mrb_str_new(mrb , &parameter.ExpDate     , 3));
    mrb_hash_set(mrb , hash , mrb_str_new_cstr(mrb , "CheckSum")    , mrb_str_new(mrb , &parameter.CheckSum    , 20));

    return hash;
  } else {
    return mrb_fixnum_value(ret);
    /*mrb_raisef(mrb, E_EMV_ERROR, "object isn't a hash", );*/
  }
}

static int
add_emv_pki(mrb_state *mrb, mrb_value klass, mrb_value hash)
{
  EMV_CAPK parameter;
  mrb_value value;
  mrb_int iValue;

  memset(&parameter, 0, sizeof(parameter));

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "RID"));
  memcpy(&parameter.RID, RSTRING_PTR(value), 5);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "KeyID"));
  parameter.KeyID = (unsigned char)RSTRING_PTR(value);
  memcpy(&parameter.KeyID, RSTRING_PTR(value), 1);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "HashInd"));
  parameter.HashInd = (unsigned char)RSTRING_PTR(value);
  memcpy(&parameter.HashInd, RSTRING_PTR(value), 1);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "ArithInd"));
  parameter.ArithInd = (unsigned char)RSTRING_PTR(value);
  memcpy(&parameter.ArithInd, RSTRING_PTR(value), 1);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "ModulLen"));
  memcpy(&parameter.ModulLen, RSTRING_PTR(value), 1);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "Modul"));
  memcpy(&parameter.Modul, RSTRING_PTR(value), parameter.ModulLen);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "ExponentLen"));
  parameter.ExponentLen = (unsigned char)RSTRING_PTR(value);
  memcpy(&parameter.ExponentLen, RSTRING_PTR(value), 1);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "Exponent"));
  memcpy(&parameter.Exponent, RSTRING_PTR(value), 3);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "ExpDate"));
  memcpy(&parameter.ExpDate, RSTRING_PTR(value), 3);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "CheckSum"));
  memcpy(&parameter.CheckSum, RSTRING_PTR(value), 20);

  return EMVAddCAPK(&parameter);
}

static mrb_value
mrb_s_add_emv_pki(mrb_state *mrb, mrb_value klass)
{
  mrb_value hash;
  mrb_int ret=EMV_OK;

  mrb_get_args(mrb, "o", &hash);

  if (mrb_hash_p(hash))
    ret = add_emv_pki(mrb, klass, hash);
  else
    mrb_raise(mrb, E_ARGUMENT_ERROR, "object isn't a hash");

  return mrb_fixnum_value(ret);
}

static mrb_value
mrb_s_del_emv_pki(mrb_state *mrb, mrb_value klass)
{
  mrb_value keyID, rid;
  mrb_int ret=EMV_OK;

  mrb_get_args(mrb, "ss", &keyID, &rid);

  if (mrb_string_p(keyID) || mrb_string_p(rid))
    ret = EMVDelCAPK(RSTRING_PTR(keyID), RSTRING_PTR(rid));
  else
    mrb_raise(mrb, E_ARGUMENT_ERROR, "object isn't a string");

  return mrb_fixnum_value(ret);
}

static mrb_value
mrb_s_check_emv_pki(mrb_state *mrb, mrb_value klass)
{
  mrb_value keyID, rid;
  mrb_int ret=EMV_OK;

  mrb_get_args(mrb, "ss", &keyID, &rid);

  if (mrb_string_p(keyID) || mrb_string_p(rid))
    ret = EMVCheckCAPK(RSTRING_PTR(keyID), RSTRING_PTR(rid));
  else
    mrb_raise(mrb, E_ARGUMENT_ERROR, "object isn't a string");

  return mrb_fixnum_value(ret);
}

/*Check core_init before initialize, system error*/
static mrb_value
mrb_s_emv__init(mrb_state *mrb, mrb_value klass)
{
  EMVInitTLVData();
  return mrb_true_value();
}

static mrb_value
mrb_s_emv_app_select(mrb_state *mrb, mrb_value klass)
{
	int ret = -2;
  mrb_int slot, number;

  mrb_get_args(mrb, "ii", &slot, &number);

	do
	{
		ret = EMVAppSelect(slot, (unsigned long)number);
	}while(ret == -2);
  return mrb_fixnum_value(ret);
}

static mrb_value
mrb_s_emv_read_data(mrb_state *mrb, mrb_value klass)
{
  return mrb_fixnum_value(EMVReadAppData());
}

static mrb_value
mrb_s_emv_get_tlv(mrb_state *mrb, mrb_value klass)
{
	mrb_int tag, dataLength;
	char dataOut[1024];

	memset(dataOut, 0, sizeof(dataOut));

  mrb_get_args(mrb, "i", &tag);

  EMVGetTLVData(tag, dataOut, &dataLength);

  return mrb_str_new(mrb, dataOut, dataLength);
}

static mrb_value
mrb_s_emv_set_tlv(mrb_state *mrb, mrb_value klass)
{
	mrb_int tag, dataLength;
	unsigned char *dataOut;

  mrb_get_args(mrb, "is", &tag, &dataOut, &dataLength);

  return mrb_fixnum_value(EMVSetTLVData(tag, dataOut, dataLength));
}

static mrb_value
mrb_s_emv_version(mrb_state *mrb, mrb_value klass)
{
  char paucVer[20];

  memset(&paucVer, 0, sizeof(paucVer));

  EMVReadVerInfo((char *)&paucVer);

  return mrb_str_new_cstr(mrb, paucVer);
}

static mrb_value
mrb_s_card_auth(mrb_state *mrb, mrb_value klass)
{
  return mrb_fixnum_value(EMVCardAuth());
}

static mrb_value
mrb_s_start_transaction(mrb_state *mrb, mrb_value klass)
{
	mrb_int amount, ret;
	unsigned char ACType;

	mrb_get_args(mrb, "i", &amount);

	EMVStartTrans(amount, 0, &ACType);

	if (ACType == AC_AAC)						ret = 0;
	else if (ACType == AC_TC)				ret = 1;
	else if (ACType == AC_ARQC)			ret = 2;
	else if (ACType == AC_AAC_HOST)	ret = 3;

  return mrb_fixnum_value(ret);
}

static mrb_value
mrb_s_complete_transaction(mrb_state *mrb, mrb_value klass)
{
	mrb_int ret;
	unsigned char ACType;
	unsigned char *scripts;
	int resp_code, sc_len, tag, arc;

	mrb_get_args(mrb, "is", &resp_code, &scripts, &sc_len);

	if (resp_code == 00)
		arc = ONLINE_APPROVE;
	else
		arc = ONLINE_DENIAL;

	EMVCompleteTrans(arc, scripts, &sc_len, &ACType);

	if (ACType == AC_AAC)						ret = 0;
	else if (ACType == AC_TC)				ret = 1;
	else if (ACType == AC_ARQC)			ret = 2;
	else if (ACType == AC_AAC_HOST)	ret = 3;

	return mrb_fixnum_value(ret);
}

void
mrb_emv_init(mrb_state* mrb)
{
  struct RClass *pax;
  struct RClass *emv;
  struct RClass *error;

  pax = mrb_class_get(mrb, "PAX");
  emv = mrb_define_class_under(mrb, pax, "EMV",  mrb->object_class);
  /*error = mrb_define_class_under(mrb, pax, "EMVError", mrb->eStandardError_class);*/

  mrb_define_class_method(mrb, emv, "core_init", mrb_s_core_init , MRB_ARGS_NONE());
  mrb_define_class_method(mrb, emv, "get_parameter", mrb_s_get_emv_parameter , MRB_ARGS_NONE());
  mrb_define_class_method(mrb, emv, "set_parameter", mrb_s_set_emv_parameter , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, emv, "get_app", mrb_s_get_emv_app , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, emv, "add_app", mrb_s_add_emv_app , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, emv, "del_app", mrb_s_del_emv_app , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, emv, "del_apps", mrb_s_del_emv_apps , MRB_ARGS_NONE());
  mrb_define_class_method(mrb, emv, "get_pki", mrb_s_get_emv_pki , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, emv, "add_pki", mrb_s_add_emv_pki , MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, emv, "del_pki", mrb_s_del_emv_pki , MRB_ARGS_REQ(2));
  mrb_define_class_method(mrb, emv, "check_pki", mrb_s_check_emv_pki , MRB_ARGS_REQ(2));
  mrb_define_class_method(mrb, emv, "_init", mrb_s_emv__init , MRB_ARGS_NONE());
  mrb_define_class_method(mrb, emv, "app_select", mrb_s_emv_app_select , MRB_ARGS_REQ(2));
	mrb_define_class_method(mrb, emv, "read_data", mrb_s_emv_read_data , MRB_ARGS_NONE());
	mrb_define_class_method(mrb, emv, "get_tlv", mrb_s_emv_get_tlv , MRB_ARGS_REQ(1));
	mrb_define_class_method(mrb, emv, "set_tlv", mrb_s_emv_set_tlv , MRB_ARGS_REQ(2));
	mrb_define_class_method(mrb, emv, "card_auth", mrb_s_card_auth , MRB_ARGS_NONE());
	mrb_define_class_method(mrb, emv, "start_transaction", mrb_s_start_transaction , MRB_ARGS_REQ(1));
	mrb_define_class_method(mrb, emv, "complete_transaction", mrb_s_complete_transaction , MRB_ARGS_REQ(2));
  mrb_define_class_method(mrb, emv, "version", mrb_s_emv_version , MRB_ARGS_NONE());
}
