#include <stdlib.h>
#include <stdio.h>
#include "mruby.h"
#include "mruby/compile.h"
#include "mruby/value.h"
#include "mruby/array.h"
#include "mruby/string.h"
#include "mruby/hash.h"

#include "osal.h"
#include "emvlib_Prolin.h"

/*Callbacks*/

int  cEMVSetParam(void) { return EMV_OK; };
unsigned char cEMVSM3(unsigned char *paucMsgIn, int nMsglenIn,unsigned char *paucResultOut) { return EMV_OK; };
unsigned char cEMVSM2Verify(unsigned char *paucPubkeyIn,unsigned char *paucMsgIn,int nMsglenIn, unsigned char *paucSignIn, int nSignlenIn) { return EMV_OK; };
int  cEMVInputAmount(unsigned long *AuthAmt, unsigned long *CashBackAmt) { return EMV_OK; };
int cEMVPedVerifyPlainPin(uchar ucIccSlot, uchar *pucExpPinLenIn, uchar *ucIccRespOut, uchar ucMode,ulong ulTimeoutMs) { return EMV_OK; };
int cEMVPedVerifyCipherPin(uchar ucIccSlot, uchar *pExpPinLenIn, RSA_PINKEY *tRsaPinKeyIn, uchar *pucIccRespOut, uchar ucMode,ulong ulTimeoutMs) { return EMV_OK; };
int  cEMVGetHolderPwd(int TryFlag, int RemainCnt, unsigned char *pin) { return EMV_OK; };
int  cCertVerify(void) { return EMV_OK; };
void cEMVVerifyPINOK(void) { return; };
unsigned char cEMVPiccIsoCommand(unsigned char cid,APDU_SEND *ApduSend,APDU_RESP *ApduRecv) { return EMV_OK; };
int  cEMVUnknowTLVData(unsigned short Tag, unsigned char *dat, int len) { return EMV_OK; };
int cEMVWaitAppSel(int TryCnt, EMV_APPLIST List[], int AppNum) { return EMV_OK; };
unsigned char cEMVIccIsoCommand(uchar ucslot, APDU_SEND *tApduSend, APDU_RESP *tApduRecv) { return EMV_OK; };

/*Callbacks*/

static mrb_value
get_emv_parameter(mrb_state *mrb, mrb_value klass)
{
  EMV_PARAM parameter;
  mrb_value hash;

  EMVGetParameter(&parameter);

  memset(&parameter, 0, sizeof(parameter));

  hash = mrb_funcall(mrb, klass, "parameter_default", 0);

  mrb_hash_set(mrb, hash, mrb_str_new_cstr(mrb, "MerchName"), mrb_str_new_cstr(mrb, parameter.MerchName));
  mrb_hash_set(mrb, hash, mrb_str_new_cstr(mrb, "MerchCateCode"), mrb_str_new_cstr(mrb, parameter.MerchCateCode));
  mrb_hash_set(mrb, hash, mrb_str_new_cstr(mrb, "MerchId"), mrb_str_new_cstr(mrb, parameter.MerchId));
  mrb_hash_set(mrb, hash, mrb_str_new_cstr(mrb, "TermId"), mrb_str_new_cstr(mrb, parameter.TermId));
  mrb_hash_set(mrb, hash, mrb_str_new_cstr(mrb, "TerminalType"), mrb_str_new_cstr(mrb, parameter.TerminalType));
  mrb_hash_set(mrb, hash, mrb_str_new_cstr(mrb, "Capability"), mrb_str_new_cstr(mrb, parameter.Capability));
  mrb_hash_set(mrb, hash, mrb_str_new_cstr(mrb, "ExCapability"), mrb_str_new_cstr(mrb, parameter.ExCapability));
  mrb_hash_set(mrb, hash, mrb_str_new_cstr(mrb, "TransCurrExp"), mrb_str_new_cstr(mrb, parameter.TransCurrExp));
  mrb_hash_set(mrb, hash, mrb_str_new_cstr(mrb, "ReferCurrExp"), mrb_str_new_cstr(mrb, parameter.ReferCurrExp));
  mrb_hash_set(mrb, hash, mrb_str_new_cstr(mrb, "ReferCurrCode"), mrb_str_new_cstr(mrb, parameter.ReferCurrCode));
  mrb_hash_set(mrb, hash, mrb_str_new_cstr(mrb, "CountryCode"), mrb_str_new_cstr(mrb, parameter.CountryCode));
  mrb_hash_set(mrb, hash, mrb_str_new_cstr(mrb, "TransCurrCode"), mrb_str_new_cstr(mrb, parameter.TransCurrCode));
  mrb_hash_set(mrb, hash, mrb_str_new_cstr(mrb, "ReferCurrCon"), mrb_str_new_cstr(mrb, parameter.ReferCurrCon));
  mrb_hash_set(mrb, hash, mrb_str_new_cstr(mrb, "TransType"), mrb_str_new_cstr(mrb, parameter.TransType));
  mrb_hash_set(mrb, hash, mrb_str_new_cstr(mrb, "ForceOnline"), mrb_str_new_cstr(mrb, parameter.ForceOnline));
  mrb_hash_set(mrb, hash, mrb_str_new_cstr(mrb, "GetDataPIN"), mrb_str_new_cstr(mrb, parameter.GetDataPIN));
  mrb_hash_set(mrb, hash, mrb_str_new_cstr(mrb, "SurportPSESel"), mrb_str_new_cstr(mrb, parameter.SurportPSESel));

  return hash;
}

static mrb_value
set_emv_parameter(mrb_state *mrb, mrb_value klass, mrb_value hash)
{
  EMV_PARAM parameter;
  mrb_value value;

  memset(&parameter, 0, sizeof(parameter));

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "MerchName"));
  parameter.MerchName = RSTRING_PTR(value);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "MerchCateCode"));
  parameter.MerchCateCode = RSTRING_PTR(value);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "MerchId"));
  parameter.MerchId = RSTRING_PTR(value);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "TermId"));
  parameter.TermId = RSTRING_PTR(value);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "TerminalType"));
  parameter.TerminalType = RSTRING_PTR(value);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "Capability"));
  parameter.Capability = RSTRING_PTR(value);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "ExCapability"));
  parameter.ExCapability = RSTRING_PTR(value);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "TransCurrExp"));
  parameter.TransCurrExp = RSTRING_PTR(value);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "ReferCurrExp"));
  parameter.ReferCurrExp = RSTRING_PTR(value);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "ReferCurrCode"));
  parameter.ReferCurrCode = RSTRING_PTR(value);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "CountryCode"));
  parameter.CountryCode = RSTRING_PTR(value);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "TransCurrCode"));
  parameter.TransCurrCode = RSTRING_PTR(value);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "ReferCurrCon"));
  parameter.ReferCurrCon = RSTRING_PTR(value);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "TransType"));
  parameter.TransType = RSTRING_PTR(value);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "ForceOnline"));
  parameter.ForceOnline = RSTRING_PTR(value);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "GetDataPIN"));
  parameter.GetDataPIN = RSTRING_PTR(value);

  memset(&value, 0, sizeof(value));
  value = mrb_hash_get(mrb, hash, mrb_str_new_cstr(mrb, "SurportPSESel"));
  parameter.SurportPSESel = RSTRING_PTR(value);

  EMVSetParameter(&parameter);

  return mrb_nil_value();
}

mrb_value
mrb_s_core_init(mrb_state *mrb, mrb_value klass)
{
  return mrb_fixnum_value(EMVCoreInit());
}

void
mrb_emv_init(mrb_state* mrb)
{
  struct RClass *pax;
  struct RClass *emv;

  pax = mrb_class_get(mrb, "PAX");
  emv = mrb_define_class(mrb, "EMV", pax);

  mrb_define_class_method(mrb, emv, "core_init", mrb_s_core_init , MRB_ARGS_NONE());
}