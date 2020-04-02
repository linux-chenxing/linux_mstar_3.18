/****************************************************************************
* This product contains one or more programs protected under international
* and U.S. copyright laws as unpublished works.  They are confidential and
* proprietary to Dolby Laboratories.  Their reproduction or disclosure, in
* whole or in part, or the production of derivative works therefrom without
* the express permission of Dolby Laboratories is prohibited.
*
*             Copyright 2011 - 2015 by Dolby Laboratories.
*                     All rights reserved.
****************************************************************************/
#ifndef CLI_PRSR_H
#define CLI_PRSR_H
#include <string>

using namespace std;

class CCliValBase
{
public:
  CCliValBase(void) {}
  virtual ~CCliValBase() {}

  // retuurn:
  //  0: OK
  //  1 optional value not in cli
  // -1 mandatory value not in cli
  virtual int GetVal(int &ac, const char ** &av) = 0;
  virtual void *GetValAddr(void) const = 0;
  virtual void SetVal(const void *valAddr) = 0;
};

template <class T> 
class CCliFlagVal : public CCliValBase
{
public:
  CCliFlagVal(T *pVal, T val): mpVal(pVal), mVal(val) {}
  ~CCliFlagVal(void) {}

  int GetVal(int &ac, const char ** &av);
  void *GetValAddr(void) const { return mpVal; }
  void SetVal(const void *valAddr) { *mpVal = *reinterpret_cast<const T *>(valAddr); }

private:
  T *mpVal;
  T  mVal;
};

template <class T> 
class CCliFmtVal : public CCliValBase
{
public:
  CCliFmtVal(T *pVal, bool opt): mpVal(pVal), mOpt(opt) {}
  ~CCliFmtVal(void) {}

  int GetVal(int &ac, const char ** &av);
  void *GetValAddr(void) const { return mpVal; }
  void SetVal(const void *valAddr) { *mpVal = *reinterpret_cast<const T *>(valAddr); }

private:
  T    *mpVal;
  bool mOpt;
};

template <class T, int itemNumMax> 
class CCliMap
{
public:
  CCliMap(void): mItemNumMax(itemNumMax), mItemNum(0) {}
  ~CCliMap(void) {}

  int GetItemNum(void)    { return mItemNum; }
  int GetItemNumMax(void)  { return mItemNumMax; }

  // -1 over max item num
  int AddMapItem(const char *key, T val);

  // -1 no such key
  int MapKey(const char *key, T *pVal);

private:
  int mItemNumMax, mItemNum;

  const char *mKeys[itemNumMax];
  T           mVals[itemNumMax];
};

template<class T, int itemNumMax> class CCliMap;
template <class T, int itemNumMax, template<class U, int itemNumMax_> class CCliMap >
class CCliMapVal : public CCliValBase
{
public:
  CCliMapVal(CCliMap<T, itemNumMax> *pMap, T *pVal, bool opt): mpVal(pVal), mOpt(opt)
  {
    mpMap = pMap;
  }
  ~CCliMapVal(void) {}

  int GetVal(int &ac, const char ** &av);
  void *GetValAddr(void) const { return mpVal; }
  void SetVal(const void *valAddr) { *mpVal = *reinterpret_cast<const T *>(valAddr); }

private:
  T    *mpVal;
  bool mOpt;
  CCliMap<T, itemNumMax> *mpMap;
};

class CCliParamBase
{
public:
  CCliParamBase(const char *paramName, bool opt) : 
      mParamName(paramName), mParamNameAlt(0), mOpt(opt), mInCli(false) {}
  virtual ~CCliParamBase() {}

  const char *GetParamName(void)  { return  mParamName; }

  void SetParamNameAlt(const char *name) { mParamNameAlt = name; }
  const char *GetParamNameAlt(void)  { return  mParamNameAlt; }

  bool GetParamOpt(void) { return mOpt; }
  bool GetParamInCli(void) { return mInCli; }

  // return
  // true: matched
  bool MatchTheParam(int &ac, const char ** &av);

  // return
  // 0: OK
  // -1: value number over parameter valNumMax
  virtual int  AddVal(CCliValBase *pCliVal) = 0;

  // retuurn:
  //  0: OK
  // -1 mandatory value not in cli
  virtual int  GetParamVals(int &ac, const char ** &av) = 0;
protected:
  const char *mParamName;
  const char *mParamNameAlt; // alternative name
  bool mOpt, mInCli; // if optional, if is in CLI
};

template <int valNumMax> 
class CCliParam : public CCliParamBase
{
public:
  CCliParam(const char *paramName, bool opt) : 
      CCliParamBase(paramName, opt), mValNumMax(valNumMax),mValNum(0)  {}
  ~CCliParam();

  int GetValNumMax(void)          { return  mValNumMax; }
  int GetValNum(void)              { return  mValNum; }

  int AddVal(CCliValBase *pCliVal);

  int GetParamVals(int &ac, const char ** &av);

private:
  int mValNumMax;
  int mValNum;

  CCliValBase *mpVals[valNumMax];
};

class CCliDefPrsrBase
{
public:
  CCliDefPrsrBase(void) : 
      mParamNumMax(0), mLineLenMax(0), mParamNum(0), 
      mpParams(0), mCfgArgv(0), mCfgBuf(0), 
      mIgnoreUnknownParam(false), mbSkipToEnd(false) {}
  virtual ~CCliDefPrsrBase();

  int Init(int paramNumMax, int lineLenMax);
  void Reset(void) { mbSkipToEnd = false; mIgnoreUnknownParam = false; }
  void IgnoreUnknownParam(bool ig) { mIgnoreUnknownParam = ig; }

  static int ParseCliForCfgFile(int &argc, const char ** &argv, string &configFile);

  // parsing line of cfg file into a argc, argv style CLI buffer 
  // return argc
  int CliLizeCfgFile(FILE *cfgFp);

  int GetParamNumMax(void) { return  mParamNumMax; }
  int GetLineLenMax(void)  { return  mLineLenMax; }
  int GetParamNum(void)    { return  mParamNum; }

protected:
  int mParamNumMax;
  int mLineLenMax;
  int mParamNum;

  CCliParamBase **mpParams;
  char **mCfgArgv, *mCfgBuf;

  bool mIgnoreUnknownParam;

private:
  bool mbSkipToEnd;
};

// it need two external functions: DmUsage(prgName) and ShowVersion()
class CCliDefPrsr : public CCliDefPrsrBase
{
public:
  CCliDefPrsr(void): mbUsage(false), mbVersion(false) {}
  ~CCliDefPrsr() {}

  int Init(int paramNumMax, int lineLenMax);
  bool MandParamsSet(void);

  // return:
  // 0: OK
  // 1: help or versin only
  // -1: invalid value
  // -2: undefined parameter
  int Parse(int &ac, const char ** &av, const char *prgName);

  /*! @brief to parse DM configuration file

    This function reads and parse DM configuration file

      @param[in] cfgFile DM config file name
      @return
          @li 0 Succeed.
          @li <0 Error condition
  */
  int ParseCfgFile(const char *cfgFile);
  // for all the cfg file give in CLI, save last one into configFile
  int ParseCliCfgFile(int argc, const char **argv, string &configFile);

  ////// help functions to define CLI
  //// add a param with flag type value(the paramter itself is a flag): 
  // param name, if param optional, pointer to var to receive value, the value to set if flag is given
  template <class T>
  CCliParamBase* AddFlagParam(const char *paramName, bool paramOpt, T *pFlagVal, T val);

  //// add a param with flag type value(the paramter itself is a flag) and followed by a fmt value: 
  // param name, if param optional, pointer to var to receive value, the value to set if flag is given
  // then the definitin of the fmt value
  template <class T1, class T2>
  CCliParamBase* AddFlagFmtParam(const char *paramName, bool paramOpt, T1 *pFlagVal, T1 flagVal,
                      T2 *pFmtVal, bool fmtValOpt = false);
  
  template <class T1, class T2, class T3>
  CCliParamBase* AddFlagFmtParam2(const char *paramName, bool paramOpt, T1 *pFlagVal, T1 flagVal,
                      T2 *pFmtVal1, T3 *pFmtVal2, bool fmtValOpt = false);

  template <class T1, class T2, class T3, class T4, class T5>
  CCliParamBase* AddFlagFmtParam4(const char *paramName, bool paramOpt, T1 *pFlagVal, T1 flagVal,
                      T2 *pFmtVal1, T3 *pFmtVal2, T4 *pFmtVal3, T5 *pFmtVal4, bool fmtValOpt = false);

  //// add a param with flag type value(the paramter itself is a flag) and followed by N T1 value: 
  // param name, if param optional, pointer to var to receive value, the value to set if flag is given
  // then pointer to the var
  template <int valNum, class T1, class T2>
  CCliParamBase* AddFlagFmtParamN(const char *paramName, bool paramOpt, T1 *pFlagVal, T1 flagVal,
                      T2 *pFmtVals);

  //// add a param with one format type value: 
  // param name, if param optional, pointer to var to receive value, if value is optional
  template <class T>
  CCliParamBase* AddFmtParam1(const char *paramName, bool paramOpt, T *pFmtVal, bool valOpt = false);

  //// add a param with two format type value: 
  // param name, if param optional, pointer to var to receive value, if value is optional
  // normally, valOpt2 shall be false except T1 and T2 are compatible
  template <class T1, class T2>
  CCliParamBase* AddFmtParam2(const char *paramName, bool paramOpt, T1 *pFmtVal1, T2 *pFmtVal2, bool valOpt2 = false);

  //// add a param with three format type value: 
  // param name, if param optional, pointer to var to receive value, if value is optional
  // normally, valOpt3 shall be false except T2 and T3 are compatible
  template <class T1, class T2, class T3>
  CCliParamBase* AddFmtParam3(const char *paramName, bool paramOpt, T1 *pFmtVal1, T2 *pFmtVal2, 
                              T3 *pFmtVal3, bool valOpt3 = false);

  // add valNum of value of same format fmt and value type, value is save in array pFmtVals
  template <int valNum, class T>
  CCliParamBase* AddFmtParamN(const char *paramName, bool paramOpt, T *pFmtVals);

  //// add parameter with one map type value: 
  // param name, if param optional, the map, pointer to var to receive value, if the value is optional
  template <class T, int itemNumMax> 
  CCliParamBase* AddMapParam1(const char *paramName, bool paramOpt, CCliMap<T, itemNumMax> *pMap, T *pMapVal, bool valOpt = false);

  template <class T, int itemNumMax> 
  CCliParamBase* AddMapParam2(const char *paramName, bool paramOpt, CCliMap<T, itemNumMax> *pMap, T *pMapVal1, 
                    T *pMapVal2, bool valOpt2 = false);

  //// add parameter with one a map type value followed by a format type value: 
  // param name, if param optional, the map, pointer to var to receive value, if the value is optional
  template <class T1, int itemNumMax, class T2> 
  CCliParamBase* AddMapFmtParam(const char *paramName, bool paramOpt, CCliMap<T1, itemNumMax> *pMap, T1 *pMapVal,
                     T2 *pFmtVal, bool valOpt2 = false);
  //// EOF help functions to define CLI

protected:
  bool mbUsage, mbVersion;
};

#endif // CLI_PRSR_H
