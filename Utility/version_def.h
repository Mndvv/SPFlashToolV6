#ifndef VERION_DEF_H
#define VERION_DEF_H

//************** PLEASE DO NOT MODIFY THE NAMES MANUALLY IN THIS FILE *******************
#define  CUSTOMER_VER      true
#define  TEMP_VER          false

#define  MAJOR_VER         6
#define  MINOR_VER         2136
#define  BUILD_NUM         00
#define  BUILD_SN          000
//Note: REVISION_NUM is commit hash of the tool, should be HEX(0x...)
#define  REVISION_NUM      0x573082
#define  BUILD_DATE        "2021/09/08 11:27"
#define  COMMENTS          "N/A"

#define  VER_FILEVERSION      		MAJOR_VER,MINOR_VER,BUILD_NUM,BUILD_SN
#define  VER_FILEVERSION_STR        "6.2136.00.000"
//************** THE NAMES ABOVE WILL BE MODIFIED BY DAILY BUILD SCRIPTS *****************

#define  VER_PRODUCTVERSION          VER_FILEVERSION
#define  VER_PRODUCTVERSION_STR 	 VER_FILEVERSION_STR

#define  VER_COMPANYNAME_STR         "Mediatek Inc"

#define  VER_FILEDESCRIPTION_STR     "SPFlashToolV6.exe"
#define  VER_ORIGINALFILENAME_STR    "SPFlashToolV6.exe"
#define  VER_PRODUCTNAME_STR         "Smart Phone Flash Tool V6"
#define  APP_NAME                    QStringLiteral("MediaTek Smart Phone Flash Tool V6")

#define  VER_LEGALCOPYRIGHT_STR      "Copyright(C) 2020-2021 Mediatek Inc. All Rights Reserved"

#define  VER_COMPANYDOMAIN_STR       "www.mediatek.com"
//*****************************************************************

#endif // VERION_DEF_H
