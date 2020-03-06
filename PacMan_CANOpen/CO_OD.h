// clang-format off
/*******************************************************************************

   File - CO_OD.c/CO_OD.h
   CANopen Object Dictionary.

   This file was automatically generated with libedssharp Object
   Dictionary Editor v0.8-0-gb60f4eb   DON'T EDIT THIS FILE MANUALLY !!!!
*******************************************************************************/


#ifndef CO_OD_H_
#define CO_OD_H_

/*******************************************************************************
   CANopen DATA TYPES
*******************************************************************************/
   typedef bool_t       BOOLEAN;
   typedef uint8_t      UNSIGNED8;
   typedef uint16_t     UNSIGNED16;
   typedef uint32_t     UNSIGNED32;
   typedef uint64_t     UNSIGNED64;
   typedef int8_t       INTEGER8;
   typedef int16_t      INTEGER16;
   typedef int32_t      INTEGER32;
   typedef int64_t      INTEGER64;
   typedef float32_t    REAL32;
   typedef float64_t    REAL64;
   typedef char_t       VISIBLE_STRING;
   typedef oChar_t      OCTET_STRING;
   typedef domain_t     CO_DOMAIN;

   #ifdef DOMAIN
   #undef DOMAIN
   #endif

   typedef domain_t     DOMAIN;

#ifndef timeOfDay_t
    typedef union {
        unsigned long long ullValue;
        struct {
            unsigned long ms:28;
            unsigned reserved:4;
            unsigned days:16;
            unsigned reserved2:16;
        };
    }timeOfDay_t;
#endif

    typedef timeOfDay_t TIME_OF_DAY;
    typedef timeOfDay_t TIME_DIFFERENCE;


/*******************************************************************************
   FILE INFO:
      FileName:     
      FileVersion:  1
      CreationTime: 7:35PM
      CreationDate: 02-09-2020
      CreatedBy:    TSV Team
******************************************************************************/


/*******************************************************************************
   DEVICE INFO:
      VendorName:     Espressif
      VendorNumber    0
      ProductName:    ESP32
      ProductNumber:  1
******************************************************************************/


/*******************************************************************************
   FEATURES
*******************************************************************************/
   #define CO_NO_SYNC                     1   //Associated objects: 1005, 1006, 1007, 2103, 2104
   #define CO_NO_TIME                     1   //Associated objects: 1012-1013
   #define CO_NO_EMERGENCY                1   //Associated objects: 1014, 1015
   #define CO_NO_SDO_SERVER               1   //Associated objects: 1200
   #define CO_NO_SDO_CLIENT               0
   #define CO_NO_RPDO                     4   //Associated objects: 1400, 1401, 1402, 1403, 1600, 1601, 1602, 1603
   #define CO_NO_TPDO                     4   //Associated objects: 1800, 1801, 1802, 1803, 1A00, 1A01, 1A02, 1A03
   #define CO_NO_NMT_MASTER               0
   #define CO_NO_TRACE                    0
   #define CO_NO_LSS_SERVER               0
   #define CO_NO_LSS_CLIENT               0

/*******************************************************************************
   OBJECT DICTIONARY
*******************************************************************************/
   #define CO_OD_NoOfElements             102


/*******************************************************************************
   TYPE DEFINITIONS FOR RECORDS
*******************************************************************************/
/*1018      */ typedef struct{
               UNSIGNED8      maxSubIndex;
               UNSIGNED32     vendorID;
               UNSIGNED32     productCode;
               UNSIGNED32     revisionNumber;
               UNSIGNED32     serialNumber;
               }              OD_identity_t;

/*1200[1]   */ typedef struct{
               UNSIGNED8      maxSubIndex;
               UNSIGNED32     COB_IDClientToServer;
               UNSIGNED32     COB_IDServerToClient;
               }              OD_SDOServerParameter_t;
/*1280    */ typedef struct {
               UNSIGNED8      maxSubIndex;
               UNSIGNED32     COB_IDClientToServer;
               UNSIGNED32     COB_IDServerToClient;
               UNSIGNED8      nodeIDOfTheSDOServer;
               }              OD_SDOClientParameter_t;
/*1400    */ typedef struct {
               UNSIGNED8      maxSubIndex;
               UNSIGNED32     COB_IDUsedByRPDO;
               UNSIGNED8      transmissionType;
               }              OD_RPDOCommunicationParameter_t;

/*1600[4]   */ typedef struct{
               UNSIGNED8      numberOfMappedObjects;
               UNSIGNED32     mappedObject1;
               UNSIGNED32     mappedObject2;
               UNSIGNED32     mappedObject3;
               UNSIGNED32     mappedObject4;
               UNSIGNED32     mappedObject5;
               UNSIGNED32     mappedObject6;
               UNSIGNED32     mappedObject7;
               UNSIGNED32     mappedObject8;
               }              OD_RPDOMappingParameter_t;

/*1800[4]   */ typedef struct{
               UNSIGNED8      maxSubIndex;
               UNSIGNED32     COB_IDUsedByTPDO;
               UNSIGNED8      transmissionType;
               UNSIGNED16     inhibitTime;
               UNSIGNED8      compatibilityEntry;
               UNSIGNED16     eventTimer;
               UNSIGNED8      SYNCStartValue;
               }              OD_TPDOCommunicationParameter_t;

/*1A00[4]   */ typedef struct{
               UNSIGNED8      numberOfMappedObjects;
               UNSIGNED32     mappedObject1;
               UNSIGNED32     mappedObject2;
               UNSIGNED32     mappedObject3;
               UNSIGNED32     mappedObject4;
               UNSIGNED32     mappedObject5;
               UNSIGNED32     mappedObject6;
               UNSIGNED32     mappedObject7;
               UNSIGNED32     mappedObject8;
               }              OD_TPDOMappingParameter_t;

/*2120      */ typedef struct{
               UNSIGNED8      maxSubIndex;
               INTEGER64      I64;
               UNSIGNED64     U64;
               REAL32         R32;
               REAL64         R64;
               CO_DOMAIN         domain;
               }              OD_testVar_t;

/*2130      */ typedef struct{
               UNSIGNED8      maxSubIndex;
               VISIBLE_STRING string[30];
               UNSIGNED64     epochTimeBaseMs;
               UNSIGNED32     epochTimeOffsetMs;
               }              OD_time_t;
/*******************************************************************************
   TYPE DEFINITIONS FOR OBJECT DICTIONARY INDEXES

   some of those are redundant with CO_SDO.h CO_ObjDicId_t <Common CiA301 object 
   dictionary entries>
*******************************************************************************/
/*1000 */
        #define OD_1000_deviceType                                  0x1000

/*1001 */
        #define OD_1001_errorRegister                               0x1001

/*1002 */
        #define OD_1002_manufacturerStatusRegister                  0x1002

/*1003 */
        #define OD_1003_preDefinedErrorField                        0x1003

        #define OD_1003_0_preDefinedErrorField_maxSubIndex          0
        #define OD_1003_1_preDefinedErrorField_standardErrorField   1
        #define OD_1003_2_preDefinedErrorField_standardErrorField   2
        #define OD_1003_3_preDefinedErrorField_standardErrorField   3
        #define OD_1003_4_preDefinedErrorField_standardErrorField   4
        #define OD_1003_5_preDefinedErrorField_standardErrorField   5
        #define OD_1003_6_preDefinedErrorField_standardErrorField   6
        #define OD_1003_7_preDefinedErrorField_standardErrorField   7
        #define OD_1003_8_preDefinedErrorField_standardErrorField   8

/*1005 */
        #define OD_1005_COB_ID_SYNCMessage                          0x1005

/*1006 */
        #define OD_1006_communicationCyclePeriod                    0x1006

/*1007 */
        #define OD_1007_synchronousWindowLength                     0x1007

/*1008 */
        #define OD_1008_manufacturerDeviceName                      0x1008

/*1009 */
        #define OD_1009_manufacturerHardwareVersion                 0x1009

/*100a */
        #define OD_100a_manufacturerSoftwareVersion                 0x100a

/*100c */
        #define OD_100c_guardTime                                   0x100c

/*100d */
        #define OD_100d_lifeTimeFactor                              0x100d

/*1010 */
        #define OD_1010_storeParameters                             0x1010

        #define OD_1010_0_storeParameters_maxSubIndex               0
        #define OD_1010_1_storeParameters_saveAllParameters         1

/*1011 */
        #define OD_1011_restoreDefaultParameters                    0x1011

        #define OD_1011_0_restoreDefaultParameters_maxSubIndex      0
        #define OD_1011_1_restoreDefaultParameters_restoreAllDefaultParameters 1

/*1012 */
        #define OD_1012_COB_ID_TIME                                 0x1012

/*1013 */
        #define OD_1013_highResolutionTimeStamp                     0x1013

/*1014 */
        #define OD_1014_COB_ID_EMCY                                 0x1014

/*1015 */
        #define OD_1015_inhibitTimeEMCY                             0x1015

/*1016 */
        #define OD_1016_consumerHeartbeatTime                       0x1016

        #define OD_1016_0_consumerHeartbeatTime_maxSubIndex         0
        #define OD_1016_1_consumerHeartbeatTime_consumerHeartbeatTime 1
        #define OD_1016_2_consumerHeartbeatTime_consumerHeartbeatTime 2
        #define OD_1016_3_consumerHeartbeatTime_consumerHeartbeatTime 3
        #define OD_1016_4_consumerHeartbeatTime_consumerHeartbeatTime 4

/*1017 */
        #define OD_1017_producerHeartbeatTime                       0x1017

/*1018 */
        #define OD_1018_identity                                    0x1018

        #define OD_1018_0_identity_maxSubIndex                      0
        #define OD_1018_1_identity_vendorID                         1
        #define OD_1018_2_identity_productCode                      2
        #define OD_1018_3_identity_revisionNumber                   3
        #define OD_1018_4_identity_serialNumber                     4

/*1019 */
        #define OD_1019_synchronousCounterOverflowValue             0x1019

/*1029 */
        #define OD_1029_errorBehavior                               0x1029

        #define OD_1029_0_errorBehavior_maxSubIndex                 0
        #define OD_1029_1_errorBehavior_communication               1
        #define OD_1029_2_errorBehavior_communicationOther          2
        #define OD_1029_3_errorBehavior_communicationPassive        3
        #define OD_1029_4_errorBehavior_generic                     4
        #define OD_1029_5_errorBehavior_deviceProfile               5
        #define OD_1029_6_errorBehavior_manufacturerSpecific        6

/*1200 */
        #define OD_1200_SDOServerParameter                          0x1200

        #define OD_1200_0_SDOServerParameter_maxSubIndex            0
        #define OD_1200_1_SDOServerParameter_COB_IDClientToServer   1
        #define OD_1200_2_SDOServerParameter_COB_IDServerToClient   2

/*1280 */
        #define OD_1280_SDOClientParameter                          0x1280

        #define OD_1280_0_SDOClientParameter_maxSubIndex            0
        #define OD_1280_1_SDOClientParameter_COB_IDClientToServer   1
        #define OD_1280_2_SDOClientParameter_COB_IDServerToClient   2
        #define OD_1280_3_SDOClientParameter_nodeIDOfTheSDOServer   3

/*1400 */
        #define OD_1400_RPDOCommunicationParameter                  0x1400

        #define OD_1400_0_RPDOCommunicationParameter_maxSubIndex    0
        #define OD_1400_1_RPDOCommunicationParameter_COB_IDUsedByRPDO 1
        #define OD_1400_2_RPDOCommunicationParameter_transmissionType 2

/*1600 */
        #define OD_1600_RPDOMappingParameter                        0x1600

        #define OD_1600_0_RPDOMappingParameter_maxSubIndex          0
        #define OD_1600_1_RPDOMappingParameter_mappedObject1        1
        #define OD_1600_2_RPDOMappingParameter_mappedObject2        2
        #define OD_1600_3_RPDOMappingParameter_mappedObject3        3
        #define OD_1600_4_RPDOMappingParameter_mappedObject4        4
        #define OD_1600_5_RPDOMappingParameter_mappedObject5        5
        #define OD_1600_6_RPDOMappingParameter_mappedObject6        6
        #define OD_1600_7_RPDOMappingParameter_mappedObject7        7
        #define OD_1600_8_RPDOMappingParameter_mappedObject8        8

/*1800 */
        #define OD_1800_TPDOCommunicationParameter                  0x1800

        #define OD_1800_0_TPDOCommunicationParameter_maxSubIndex    0
        #define OD_1800_1_TPDOCommunicationParameter_COB_IDUsedByTPDO 1
        #define OD_1800_2_TPDOCommunicationParameter_transmissionType 2
        #define OD_1800_3_TPDOCommunicationParameter_inhibitTime    3
        #define OD_1800_4_TPDOCommunicationParameter_compatibilityEntry 4
        #define OD_1800_5_TPDOCommunicationParameter_eventTimer     5
        #define OD_1800_6_TPDOCommunicationParameter_SYNCStartValue 6

/*1a00 */
        #define OD_1a00_TPDOMappingParameter                        0x1a00

        #define OD_1a00_0_TPDOMappingParameter_maxSubIndex          0
        #define OD_1a00_1_TPDOMappingParameter_mappedObject1        1
        #define OD_1a00_2_TPDOMappingParameter_mappedObject2        2
        #define OD_1a00_3_TPDOMappingParameter_mappedObject3        3
        #define OD_1a00_4_TPDOMappingParameter_mappedObject4        4
        #define OD_1a00_5_TPDOMappingParameter_mappedObject5        5
        #define OD_1a00_6_TPDOMappingParameter_mappedObject6        6
        #define OD_1a00_7_TPDOMappingParameter_mappedObject7        7
        #define OD_1a00_8_TPDOMappingParameter_mappedObject8        8

/*2000 */
        #define OD_2000_packNodeID                                  0x2000

/*2001 */
        #define OD_2001_SLOOP_Relay                                 0x2001

/*2002 */
        #define OD_2002_SLOOP1                                      0x2002

/*2003 */
        #define OD_2003_SLOOP2                                      0x2003

/*2004 */
        #define OD_2004_AIRS                                        0x2004

/*2005 */
        #define OD_2005_packVoltage                                 0x2005

/*2006 */
        #define OD_2006_packDischargeCurrent                        0x2006

/*2007 */
        #define OD_2007_packSOC                                     0x2007

/*2008 */
        #define OD_2008_chargeCableDetected                         0x2008

/*2009 */
        #define OD_2009_chargingEnabled                             0x2009

/*200a */
        #define OD_200a_packChargeCurrent                           0x200a

/*200b */
        #define OD_200b_chargeTime                                  0x200b

/*200c */
        #define OD_200c_timeSinceLastCharge                         0x200c

/*200d */
        #define OD_200d_pacManTSV_Current                           0x200d

/*200e */
        #define OD_200e_packAmbientTemperature                      0x200e

/*200f */
        #define OD_200f_smallestCellVoltage                         0x200f

/*2010 */
        #define OD_2010_greatestCellVoltage                         0x2010

/*2011 */
        #define OD_2011_averageCellVoltage                          0x2011

/*2012 */
        #define OD_2012_smallestCellTemp                            0x2012

/*2013 */
        #define OD_2013_greatestCellTemp                            0x2013

/*2014 */
        #define OD_2014_averageCellTemperature                      0x2014

/*2015 */
        #define OD_2015_numberOfExpectedCells                       0x2015

/*2016 */
        #define OD_2016_numberOfDetectedCells                       0x2016

/*2017 */
        #define OD_2017_cellmanRefreshRate                          0x2017

/*2018 */
        #define OD_2018_ambientTempRefreshRate                      0x2018

/*3000 */
        #define OD_3000_I2C_Address                                 0x3000

        #define OD_3000_0_I2C_Address_maxSubIndex                   0
        #define OD_3000_1_I2C_Address_cell1                         1
        #define OD_3000_2_I2C_Address_cell2                         2
        #define OD_3000_3_I2C_Address_cell3                         3
        #define OD_3000_4_I2C_Address_cell4                         4
        #define OD_3000_5_I2C_Address_cell5                         5
        #define OD_3000_6_I2C_Address_cell6                         6
        #define OD_3000_7_I2C_Address_cell7                         7
        #define OD_3000_8_I2C_Address_cell8                         8
        #define OD_3000_9_I2C_Address_cell9                         9
        #define OD_3000_10_I2C_Address_cell10                       10
        #define OD_3000_11_I2C_Address_cell11                       11
        #define OD_3000_12_I2C_Address_cell12                       12
        #define OD_3000_13_I2C_Address_cell13                       13
        #define OD_3000_14_I2C_Address_cell14                       14
        #define OD_3000_15_I2C_Address_cell15                       15
        #define OD_3000_16_I2C_Address_cell16                       16

/*3001 */
        #define OD_3001_cellPosition                                0x3001

        #define OD_3001_0_cellPosition_maxSubIndex                  0
        #define OD_3001_1_cellPosition_cell1                        1
        #define OD_3001_2_cellPosition_cell2                        2
        #define OD_3001_3_cellPosition_cell3                        3
        #define OD_3001_4_cellPosition_cell4                        4
        #define OD_3001_5_cellPosition_cell5                        5
        #define OD_3001_6_cellPosition_cell6                        6
        #define OD_3001_7_cellPosition_cell7                        7
        #define OD_3001_8_cellPosition_cell8                        8
        #define OD_3001_9_cellPosition_cell9                        9
        #define OD_3001_10_cellPosition_cell10                      10
        #define OD_3001_11_cellPosition_cell11                      11
        #define OD_3001_12_cellPosition_cell12                      12
        #define OD_3001_13_cellPosition_cell13                      13
        #define OD_3001_14_cellPosition_cell14                      14
        #define OD_3001_15_cellPosition_cell15                      15
        #define OD_3001_16_cellPosition_cell16                      16

/*3002 */
        #define OD_3002_cellVoltage                                 0x3002

        #define OD_3002_0_cellVoltage_maxSubIndex                   0
        #define OD_3002_1_cellVoltage_cell1                         1
        #define OD_3002_2_cellVoltage_cell2                         2
        #define OD_3002_3_cellVoltage_cell3                         3
        #define OD_3002_4_cellVoltage_cell4                         4
        #define OD_3002_5_cellVoltage_cell5                         5
        #define OD_3002_6_cellVoltage_cell6                         6
        #define OD_3002_7_cellVoltage_cell7                         7
        #define OD_3002_8_cellVoltage_cell8                         8
        #define OD_3002_9_cellVoltage_cell9                         9
        #define OD_3002_10_cellVoltage_cell10                       10
        #define OD_3002_11_cellVoltage_cell11                       11
        #define OD_3002_12_cellVoltage_cell12                       12
        #define OD_3002_13_cellVoltage_cell13                       13
        #define OD_3002_14_cellVoltage_cell14                       14
        #define OD_3002_15_cellVoltage_cell15                       15
        #define OD_3002_16_cellVoltage_cell16                       16

/*3003 */
        #define OD_3003_cellTemperature                             0x3003

        #define OD_3003_0_cellTemperature_maxSubIndex               0
        #define OD_3003_1_cellTemperature_cell1                     1
        #define OD_3003_2_cellTemperature_cell2                     2
        #define OD_3003_3_cellTemperature_cell3                     3
        #define OD_3003_4_cellTemperature_cell4                     4
        #define OD_3003_5_cellTemperature_cell5                     5
        #define OD_3003_6_cellTemperature_cell6                     6
        #define OD_3003_7_cellTemperature_cell7                     7
        #define OD_3003_8_cellTemperature_cell8                     8
        #define OD_3003_9_cellTemperature_cell9                     9
        #define OD_3003_10_cellTemperature_cell10                   10
        #define OD_3003_11_cellTemperature_cell11                   11
        #define OD_3003_12_cellTemperature_cell12                   12
        #define OD_3003_13_cellTemperature_cell13                   13
        #define OD_3003_14_cellTemperature_cell14                   14
        #define OD_3003_15_cellTemperature_cell15                   15
        #define OD_3003_16_cellTemperature_cell16                   16

/*3004 */
        #define OD_3004_minusTerminalVoltage                        0x3004

        #define OD_3004_0_minusTerminalVoltage_maxSubIndex          0
        #define OD_3004_1_minusTerminalVoltage_cell1                1
        #define OD_3004_2_minusTerminalVoltage_cell2                2
        #define OD_3004_3_minusTerminalVoltage_cell3                3
        #define OD_3004_4_minusTerminalVoltage_cell4                4
        #define OD_3004_5_minusTerminalVoltage_cell5                5
        #define OD_3004_6_minusTerminalVoltage_cell6                6
        #define OD_3004_7_minusTerminalVoltage_cell7                7
        #define OD_3004_8_minusTerminalVoltage_cell8                8
        #define OD_3004_9_minusTerminalVoltage_cell9                9
        #define OD_3004_10_minusTerminalVoltage_cell10              10
        #define OD_3004_11_minusTerminalVoltage_cell11              11
        #define OD_3004_12_minusTerminalVoltage_cell12              12
        #define OD_3004_13_minusTerminalVoltage_cell13              13
        #define OD_3004_14_minusTerminalVoltage_cell14              14
        #define OD_3004_15_minusTerminalVoltage_cell15              15
        #define OD_3004_16_minusTerminalVoltage_cell16              16

/*3005 */
        #define OD_3005_cellBalancingEnabled                        0x3005

        #define OD_3005_0_cellBalancingEnabled_maxSubIndex          0
        #define OD_3005_1_cellBalancingEnabled_cell1                1
        #define OD_3005_2_cellBalancingEnabled_cell2                2
        #define OD_3005_3_cellBalancingEnabled_cell3                3
        #define OD_3005_4_cellBalancingEnabled_cell4                4
        #define OD_3005_5_cellBalancingEnabled_cell5                5
        #define OD_3005_6_cellBalancingEnabled_cell6                6
        #define OD_3005_7_cellBalancingEnabled_cell7                7
        #define OD_3005_8_cellBalancingEnabled_cell8                8
        #define OD_3005_9_cellBalancingEnabled_cell9                9
        #define OD_3005_10_cellBalancingEnabled_cell10              10
        #define OD_3005_11_cellBalancingEnabled_cell11              11
        #define OD_3005_12_cellBalancingEnabled_cell12              12
        #define OD_3005_13_cellBalancingEnabled_cell13              13
        #define OD_3005_14_cellBalancingEnabled_cell14              14
        #define OD_3005_15_cellBalancingEnabled_cell15              15
        #define OD_3005_16_cellBalancingEnabled_cell16              16

/*3006 */
        #define OD_3006_cellFlybackVoltage                          0x3006

        #define OD_3006_0_cellFlybackVoltage_maxSubIndex            0
        #define OD_3006_1_cellFlybackVoltage_cell1                  1
        #define OD_3006_2_cellFlybackVoltage_cell2                  2
        #define OD_3006_3_cellFlybackVoltage_cell3                  3
        #define OD_3006_4_cellFlybackVoltage_cell4                  4
        #define OD_3006_5_cellFlybackVoltage_cell5                  5
        #define OD_3006_6_cellFlybackVoltage_cell6                  6
        #define OD_3006_7_cellFlybackVoltage_cell7                  7
        #define OD_3006_8_cellFlybackVoltage_cell8                  8
        #define OD_3006_9_cellFlybackVoltage_cell9                  9
        #define OD_3006_10_cellFlybackVoltage_cell10                10
        #define OD_3006_11_cellFlybackVoltage_cell11                11
        #define OD_3006_12_cellFlybackVoltage_cell12                12
        #define OD_3006_13_cellFlybackVoltage_cell13                13
        #define OD_3006_14_cellFlybackVoltage_cell14                14
        #define OD_3006_15_cellFlybackVoltage_cell15                15
        #define OD_3006_16_cellFlybackVoltage_cell16                16

/*3007 */
        #define OD_3007_cellBalancingCurrent                        0x3007

        #define OD_3007_0_cellBalancingCurrent_maxSubIndex          0
        #define OD_3007_1_cellBalancingCurrent_cell1                1
        #define OD_3007_2_cellBalancingCurrent_cell2                2
        #define OD_3007_3_cellBalancingCurrent_cell3                3
        #define OD_3007_4_cellBalancingCurrent_cell4                4
        #define OD_3007_5_cellBalancingCurrent_cell5                5
        #define OD_3007_6_cellBalancingCurrent_cell6                6
        #define OD_3007_7_cellBalancingCurrent_cell7                7
        #define OD_3007_8_cellBalancingCurrent_cell8                8
        #define OD_3007_9_cellBalancingCurrent_cell9                9
        #define OD_3007_10_cellBalancingCurrent_cell10              10
        #define OD_3007_11_cellBalancingCurrent_cell11              11
        #define OD_3007_12_cellBalancingCurrent_cell12              12
        #define OD_3007_13_cellBalancingCurrent_cell13              13
        #define OD_3007_14_cellBalancingCurrent_cell14              14
        #define OD_3007_15_cellBalancingCurrent_cell15              15
        #define OD_3007_16_cellBalancingCurrent_cell16              16

/*3008 */
        #define OD_3008_cellSOC                                     0x3008

        #define OD_3008_0_cellSOC_maxSubIndex                       0
        #define OD_3008_1_cellSOC_cell1                             1
        #define OD_3008_2_cellSOC_cell2                             2
        #define OD_3008_3_cellSOC_cell3                             3
        #define OD_3008_4_cellSOC_cell4                             4
        #define OD_3008_5_cellSOC_cell5                             5
        #define OD_3008_6_cellSOC_cell6                             6
        #define OD_3008_7_cellSOC_cell7                             7
        #define OD_3008_8_cellSOC_cell8                             8
        #define OD_3008_9_cellSOC_cell9                             9
        #define OD_3008_10_cellSOC_cell10                           10
        #define OD_3008_11_cellSOC_cell11                           11
        #define OD_3008_12_cellSOC_cell12                           12
        #define OD_3008_13_cellSOC_cell13                           13
        #define OD_3008_14_cellSOC_cell14                           14
        #define OD_3008_15_cellSOC_cell15                           15
        #define OD_3008_16_cellSOC_cell16                           16

/*3009 */
        #define OD_3009_minCellVoltage                              0x3009

        #define OD_3009_0_minCellVoltage_maxSubIndex                0
        #define OD_3009_1_minCellVoltage_cell1                      1
        #define OD_3009_2_minCellVoltage_cell2                      2
        #define OD_3009_3_minCellVoltage_cell3                      3
        #define OD_3009_4_minCellVoltage_cell4                      4
        #define OD_3009_5_minCellVoltage_cell5                      5
        #define OD_3009_6_minCellVoltage_cell6                      6
        #define OD_3009_7_minCellVoltage_cell7                      7
        #define OD_3009_8_minCellVoltage_cell8                      8
        #define OD_3009_9_minCellVoltage_cell9                      9
        #define OD_3009_10_minCellVoltage_cell10                    10
        #define OD_3009_11_minCellVoltage_cell11                    11
        #define OD_3009_12_minCellVoltage_cell12                    12
        #define OD_3009_13_minCellVoltage_cell13                    13
        #define OD_3009_14_minCellVoltage_cell14                    14
        #define OD_3009_15_minCellVoltage_cell15                    15
        #define OD_3009_16_minCellVoltage_cell16                    16

/*300a */
        #define OD_300a_maxCellVoltage                              0x300a

        #define OD_300a_0_maxCellVoltage_maxSubIndex                0
        #define OD_300a_1_maxCellVoltage_cell1                      1
        #define OD_300a_2_maxCellVoltage_cell2                      2
        #define OD_300a_3_maxCellVoltage_cell3                      3
        #define OD_300a_4_maxCellVoltage_cell4                      4
        #define OD_300a_5_maxCellVoltage_cell5                      5
        #define OD_300a_6_maxCellVoltage_cell6                      6
        #define OD_300a_7_maxCellVoltage_cell7                      7
        #define OD_300a_8_maxCellVoltage_cell8                      8
        #define OD_300a_9_maxCellVoltage_cell9                      9
        #define OD_300a_10_maxCellVoltage_cell10                    10
        #define OD_300a_11_maxCellVoltage_cell11                    11
        #define OD_300a_12_maxCellVoltage_cell12                    12
        #define OD_300a_13_maxCellVoltage_cell13                    13
        #define OD_300a_14_maxCellVoltage_cell14                    14
        #define OD_300a_15_maxCellVoltage_cell15                    15
        #define OD_300a_16_maxCellVoltage_cell16                    16

/*300b */
        #define OD_300b_minCellTemp                                 0x300b

        #define OD_300b_0_minCellTemp_maxSubIndex                   0
        #define OD_300b_1_minCellTemp_cell1                         1
        #define OD_300b_2_minCellTemp_cell2                         2
        #define OD_300b_3_minCellTemp_cell3                         3
        #define OD_300b_4_minCellTemp_cell4                         4
        #define OD_300b_5_minCellTemp_cell5                         5
        #define OD_300b_6_minCellTemp_cell6                         6
        #define OD_300b_7_minCellTemp_cell7                         7
        #define OD_300b_8_minCellTemp_cell8                         8
        #define OD_300b_9_minCellTemp_cell9                         9
        #define OD_300b_10_minCellTemp_cell10                       10
        #define OD_300b_11_minCellTemp_cell11                       11
        #define OD_300b_12_minCellTemp_cell12                       12
        #define OD_300b_13_minCellTemp_cell13                       13
        #define OD_300b_14_minCellTemp_cell14                       14
        #define OD_300b_15_minCellTemp_cell15                       15
        #define OD_300b_16_minCellTemp_cell16                       16

/*300c */
        #define OD_300c_maxCellTemp                                 0x300c

        #define OD_300c_0_maxCellTemp_maxSubIndex                   0
        #define OD_300c_1_maxCellTemp_cell1                         1
        #define OD_300c_2_maxCellTemp_cell2                         2
        #define OD_300c_3_maxCellTemp_cell3                         3
        #define OD_300c_4_maxCellTemp_cell4                         4
        #define OD_300c_5_maxCellTemp_cell5                         5
        #define OD_300c_6_maxCellTemp_cell6                         6
        #define OD_300c_7_maxCellTemp_cell7                         7
        #define OD_300c_8_maxCellTemp_cell8                         8
        #define OD_300c_9_maxCellTemp_cell9                         9
        #define OD_300c_10_maxCellTemp_cell10                       10
        #define OD_300c_11_maxCellTemp_cell11                       11
        #define OD_300c_12_maxCellTemp_cell12                       12
        #define OD_300c_13_maxCellTemp_cell13                       13
        #define OD_300c_14_maxCellTemp_cell14                       14
        #define OD_300c_15_maxCellTemp_cell15                       15
        #define OD_300c_16_maxCellTemp_cell16                       16

/*300d */
        #define OD_300d_minCellCurrent                              0x300d

        #define OD_300d_0_minCellCurrent_maxSubIndex                0
        #define OD_300d_1_minCellCurrent_cell1                      1
        #define OD_300d_2_minCellCurrent_cell2                      2
        #define OD_300d_3_minCellCurrent_cell3                      3
        #define OD_300d_4_minCellCurrent_cell4                      4
        #define OD_300d_5_minCellCurrent_cell5                      5
        #define OD_300d_6_minCellCurrent_cell6                      6
        #define OD_300d_7_minCellCurrent_cell7                      7
        #define OD_300d_8_minCellCurrent_cell8                      8
        #define OD_300d_9_minCellCurrent_cell9                      9
        #define OD_300d_10_minCellCurrent_cell10                    10
        #define OD_300d_11_minCellCurrent_cell11                    11
        #define OD_300d_12_minCellCurrent_cell12                    12
        #define OD_300d_13_minCellCurrent_cell13                    13
        #define OD_300d_14_minCellCurrent_cell14                    14
        #define OD_300d_15_minCellCurrent_cell15                    15
        #define OD_300d_16_minCellCurrent_cell16                    16

/*300e */
        #define OD_300e_maxCellCurrent                              0x300e

        #define OD_300e_0_maxCellCurrent_maxSubIndex                0
        #define OD_300e_1_maxCellCurrent_cell1                      1
        #define OD_300e_2_maxCellCurrent_cell2                      2
        #define OD_300e_3_maxCellCurrent_cell3                      3
        #define OD_300e_4_maxCellCurrent_cell4                      4
        #define OD_300e_5_maxCellCurrent_cell5                      5
        #define OD_300e_6_maxCellCurrent_cell6                      6
        #define OD_300e_7_maxCellCurrent_cell7                      7
        #define OD_300e_8_maxCellCurrent_cell8                      8
        #define OD_300e_9_maxCellCurrent_cell9                      9
        #define OD_300e_10_maxCellCurrent_cell10                    10
        #define OD_300e_11_maxCellCurrent_cell11                    11
        #define OD_300e_12_maxCellCurrent_cell12                    12
        #define OD_300e_13_maxCellCurrent_cell13                    13
        #define OD_300e_14_maxCellCurrent_cell14                    14
        #define OD_300e_15_maxCellCurrent_cell15                    15
        #define OD_300e_16_maxCellCurrent_cell16                    16

/*300f */
        #define OD_300f_maxCellChargeVoltage                        0x300f

        #define OD_300f_0_maxCellChargeVoltage_maxSubIndex          0
        #define OD_300f_1_maxCellChargeVoltage_cell1                1
        #define OD_300f_2_maxCellChargeVoltage_cell2                2
        #define OD_300f_3_maxCellChargeVoltage_cell3                3
        #define OD_300f_4_maxCellChargeVoltage_cell4                4
        #define OD_300f_5_maxCellChargeVoltage_cell5                5
        #define OD_300f_6_maxCellChargeVoltage_cell6                6
        #define OD_300f_7_maxCellChargeVoltage_cell7                7
        #define OD_300f_8_maxCellChargeVoltage_cell8                8
        #define OD_300f_9_maxCellChargeVoltage_cell9                9
        #define OD_300f_10_maxCellChargeVoltage_cell10              10
        #define OD_300f_11_maxCellChargeVoltage_cell11              11
        #define OD_300f_12_maxCellChargeVoltage_cell12              12
        #define OD_300f_13_maxCellChargeVoltage_cell13              13
        #define OD_300f_14_maxCellChargeVoltage_cell14              14
        #define OD_300f_15_maxCellChargeVoltage_cell15              15
        #define OD_300f_16_maxCellChargeVoltage_cell16              16

/*3010 */
        #define OD_3010_cellSOC_Min                                 0x3010

        #define OD_3010_0_cellSOC_Min_maxSubIndex                   0
        #define OD_3010_1_cellSOC_Min_cell1                         1
        #define OD_3010_2_cellSOC_Min_cell2                         2
        #define OD_3010_3_cellSOC_Min_cell3                         3
        #define OD_3010_4_cellSOC_Min_cell4                         4
        #define OD_3010_5_cellSOC_Min_cell5                         5
        #define OD_3010_6_cellSOC_Min_cell6                         6
        #define OD_3010_7_cellSOC_Min_cell7                         7
        #define OD_3010_8_cellSOC_Min_cell8                         8
        #define OD_3010_9_cellSOC_Min_cell9                         9
        #define OD_3010_10_cellSOC_Min_cell10                       10
        #define OD_3010_11_cellSOC_Min_cell11                       11
        #define OD_3010_12_cellSOC_Min_cell12                       12
        #define OD_3010_13_cellSOC_Min_cell13                       13
        #define OD_3010_14_cellSOC_Min_cell14                       14
        #define OD_3010_15_cellSOC_Min_cell15                       15
        #define OD_3010_16_cellSOC_Min_cell16                       16

/*3011 */
        #define OD_3011_cellSOH                                     0x3011

        #define OD_3011_0_cellSOH_maxSubIndex                       0
        #define OD_3011_1_cellSOH_cell1                             1
        #define OD_3011_2_cellSOH_cell2                             2
        #define OD_3011_3_cellSOH_cell3                             3
        #define OD_3011_4_cellSOH_cell4                             4
        #define OD_3011_5_cellSOH_cell5                             5
        #define OD_3011_6_cellSOH_cell6                             6
        #define OD_3011_7_cellSOH_cell7                             7
        #define OD_3011_8_cellSOH_cell8                             8
        #define OD_3011_9_cellSOH_cell9                             9
        #define OD_3011_10_cellSOH_cell10                           10
        #define OD_3011_11_cellSOH_cell11                           11
        #define OD_3011_12_cellSOH_cell12                           12
        #define OD_3011_13_cellSOH_cell13                           13
        #define OD_3011_14_cellSOH_cell14                           14
        #define OD_3011_15_cellSOH_cell15                           15
        #define OD_3011_16_cellSOH_cell16                           16

/*******************************************************************************
   STRUCTURES FOR VARIABLES IN DIFFERENT MEMORY LOCATIONS
*******************************************************************************/
#define  CO_OD_FIRST_LAST_WORD     0x55 //Any value from 0x01 to 0xFE. If changed, EEPROM will be reinitialized.

/***** Structure for RAM variables ********************************************/
struct sCO_OD_RAM{
               UNSIGNED32     FirstWord;

/*1001      */ UNSIGNED8       errorRegister;
/*1002      */ UNSIGNED32      manufacturerStatusRegister;
/*1003      */ UNSIGNED32      preDefinedErrorField[8];
/*100d      */ UNSIGNED8       lifeTimeFactor;
/*1010      */ UNSIGNED32      storeParameters[1];
/*1011      */ UNSIGNED32      restoreDefaultParameters[1];
/*1013      */ UNSIGNED32      highResolutionTimeStamp;
/*1280      */ OD_SDOClientParameter_t SDOClientParameter[1];
/*2001      */ BOOLEAN         SLOOP_Relay;
/*2002      */ BOOLEAN         SLOOP1;
/*2003      */ BOOLEAN         SLOOP2;
/*2004      */ BOOLEAN         AIRS;
/*2005      */ REAL32          packVoltage;
/*2006      */ REAL32          packDischargeCurrent;
/*2007      */ UNSIGNED8       packSOC;
/*2019      */ UNSIGNED8       fault;
/*201A      */ UNSIGNED8       warning;
/*2008      */ BOOLEAN         chargeCableDetected;
/*2009      */ BOOLEAN         chargingEnabled;
/*200a      */ REAL32          packChargeCurrent;
/*200b      */ UNSIGNED32      chargeTime;
/*200c      */ UNSIGNED32      timeSinceLastCharge;
/*200d      */ UNSIGNED8       pacManTSV_Current;
/*200e      */ UNSIGNED8       packAmbientTemperature;
/*200f      */ UNSIGNED8       smallestCellVoltage;
/*2010      */ UNSIGNED8       greatestCellVoltage;
/*2011      */ UNSIGNED8       averageCellVoltage;
/*2012      */ UNSIGNED8       smallestCellTemp;
/*2013      */ UNSIGNED8       greatestCellTemp;
/*2014      */ REAL32          averageCellTemperature;
/*2016      */ UNSIGNED8       numberOfDetectedCells;
/*2100      */ OCTET_STRING   errorStatusBits[10];
/*2103      */ UNSIGNED16     SYNCCounter;
/*2104      */ UNSIGNED16     SYNCTime;
/*2107      */ UNSIGNED16     performance[5];
/*2108      */ INTEGER16      temperature[1];
/*2109      */ INTEGER16      voltage[1];
/*2110      */ INTEGER32      variableInt32[16];
/*2120      */ OD_testVar_t   testVar;
/*2130      */ OD_time_t      time;
/*3000      */ UNSIGNED8       I2C_Address[16];
/*3001      */ UNSIGNED8       cellPosition[16];
/*3002      */ UNSIGNED16      cellVoltage[16];
/*3003      */ UNSIGNED16      cellTemperature[16];
/*3004      */ UNSIGNED16      minusTerminalVoltage[16];
/*3005      */ BOOLEAN         cellBalancingEnabled[16];
/*3006      */ UNSIGNED8       cellFlybackVoltage[16];
/*3007      */ UNSIGNED16      cellBalancingCurrent[16];
/*3008      */ UNSIGNED8       cellSOC[16];
/*3011      */ BOOLEAN         cellSOH[16];
/*6000      */ UNSIGNED8      readInput8Bit[8];
/*6200      */ UNSIGNED8      writeOutput8Bit[8];
/*6401      */ INTEGER16      readAnalogueInput16Bit[12];
/*6411      */ INTEGER16      writeAnalogueOutput16Bit[8];

               UNSIGNED32     LastWord;
};

/***** Structure for EEPROM variables *****************************************/
struct sCO_OD_EEPROM{
               UNSIGNED32     FirstWord;

/*2000      */ UNSIGNED8       packNodeID;
/*2015      */ UNSIGNED8       numberOfExpectedCells;
/*2017      */ UNSIGNED8       cellmanRefreshRate;
/*2018      */ UNSIGNED8       ambientTempRefreshRate;
/*2106      */ UNSIGNED32      powerOnCounter;
/*2112      */ INTEGER32       variableNVInt32[16];
/*3009      */ UNSIGNED16      minCellVoltage[16];
/*300a      */ UNSIGNED16      maxCellVoltage[16];
/*300b      */ UNSIGNED8       minCellTemp[16];
/*300c      */ UNSIGNED8       maxCellTemp[16];
/*300d      */ UNSIGNED16      minCellCurrent[16];
/*300e      */ UNSIGNED16      maxCellCurrent[16];
/*300f      */ UNSIGNED16      maxCellChargeVoltage[16];
/*3010      */ UNSIGNED8       cellSOC_Min[16];

               UNSIGNED32     LastWord;
};


/***** Structure for ROM variables ********************************************/
struct sCO_OD_ROM{
               UNSIGNED32     FirstWord;

/*1000      */ UNSIGNED32     deviceType;
/*1005      */ UNSIGNED32     COB_ID_SYNCMessage;
/*1006      */ UNSIGNED32     communicationCyclePeriod;
/*1007      */ UNSIGNED32     synchronousWindowLength;
/*1008      */ VISIBLE_STRING manufacturerDeviceName[11];
/*1009      */ VISIBLE_STRING manufacturerHardwareVersion[4];
/*100A      */ VISIBLE_STRING manufacturerSoftwareVersion[4];
/*100c      */ UNSIGNED16      guardTime;
/*1012      */ UNSIGNED32     COB_ID_TIME;
/*1014      */ UNSIGNED32     COB_ID_EMCY;
/*1015      */ UNSIGNED16     inhibitTimeEMCY;
/*1016      */ UNSIGNED32     consumerHeartbeatTime[4];
/*1017      */ UNSIGNED16     producerHeartbeatTime;
/*1018      */ OD_identity_t  identity;
/*1019      */ UNSIGNED8      synchronousCounterOverflowValue;
/*1029      */ UNSIGNED8      errorBehavior[6];
/*1200[1]   */ OD_SDOServerParameter_t SDOServerParameter[1];
/*1400[4]   */ OD_RPDOCommunicationParameter_t RPDOCommunicationParameter[4];
/*1600[4]   */ OD_RPDOMappingParameter_t RPDOMappingParameter[4];
/*1800[4]   */ OD_TPDOCommunicationParameter_t TPDOCommunicationParameter[4];
/*1A00[4]   */ OD_TPDOMappingParameter_t TPDOMappingParameter[4];
/*1F80      */ UNSIGNED32     NMTStartup;
/*2101      */ UNSIGNED8      CANNodeID;
/*2102      */ UNSIGNED16     CANBitRate;
/*2111      */ INTEGER32      variableROMInt32[16];

               UNSIGNED32     LastWord;
};


/***** Declaration of Object Dictionary variables *****************************/
extern struct sCO_OD_RAM CO_OD_RAM;

extern struct sCO_OD_EEPROM CO_OD_EEPROM;

extern struct sCO_OD_ROM CO_OD_ROM;


/*******************************************************************************
   ALIASES FOR OBJECT DICTIONARY VARIABLES
*******************************************************************************/
/*1000, Data Type: UNSIGNED32 */
        #define OD_deviceType                                       CO_OD_ROM.deviceType

/*1001, Data Type: UNSIGNED8 */
        #define OD_errorRegister                                    CO_OD_RAM.errorRegister

/*1002, Data Type: UNSIGNED32 */
        #define OD_manufacturerStatusRegister                       CO_OD_RAM.manufacturerStatusRegister

/*1003, Data Type: UNSIGNED32, Array[8] */
        #define OD_preDefinedErrorField                             CO_OD_RAM.preDefinedErrorField
        #define ODL_preDefinedErrorField_arrayLength                8
        #define ODA_preDefinedErrorField_standardErrorField         0

/*1005, Data Type: UNSIGNED32 */
        #define OD_COB_ID_SYNCMessage                               CO_OD_ROM.COB_ID_SYNCMessage

/*1006, Data Type: UNSIGNED32 */
        #define OD_communicationCyclePeriod                         CO_OD_ROM.communicationCyclePeriod

/*1007, Data Type: UNSIGNED32 */
        #define OD_synchronousWindowLength                          CO_OD_ROM.synchronousWindowLength

/*1008, Data Type: VISIBLE_STRING */
        #define OD_manufacturerDeviceName                           CO_OD_ROM.manufacturerDeviceName
        #define ODL_manufacturerDeviceName_stringLength             11

/*1009, Data Type: VISIBLE_STRING */
        #define OD_manufacturerHardwareVersion                      CO_OD_ROM.manufacturerHardwareVersion
        #define ODL_manufacturerHardwareVersion_stringLength        4

/*100a, Data Type: VISIBLE_STRING */
        #define OD_manufacturerSoftwareVersion                      CO_OD_ROM.manufacturerSoftwareVersion
        #define ODL_manufacturerSoftwareVersion_stringLength        4

/*100c, Data Type: UNSIGNED16 */
        #define OD_guardTime                                        CO_OD_ROM.guardTime

/*100d, Data Type: UNSIGNED8 */
        #define OD_lifeTimeFactor                                   CO_OD_RAM.lifeTimeFactor

/*1010, Data Type: UNSIGNED32, Array[1] */
        #define OD_storeParameters                                  CO_OD_RAM.storeParameters
        #define ODL_storeParameters_arrayLength                     1
        #define ODA_storeParameters_saveAllParameters               0

/*1011, Data Type: UNSIGNED32, Array[1] */
        #define OD_restoreDefaultParameters                         CO_OD_RAM.restoreDefaultParameters
        #define ODL_restoreDefaultParameters_arrayLength            1
        #define ODA_restoreDefaultParameters_restoreAllDefaultParameters 0

/*1012, Data Type: UNSIGNED32 */
      #define OD_COB_ID_TIME                             CO_OD_ROM.COB_ID_TIME

/*1014, Data Type: UNSIGNED32 */
      #define OD_COB_ID_EMCY                             CO_OD_ROM.COB_ID_EMCY

/*1015, Data Type: UNSIGNED16 */
      #define OD_inhibitTimeEMCY                         CO_OD_ROM.inhibitTimeEMCY

/*1016, Data Type: UNSIGNED32, Array[4] */
        #define OD_consumerHeartbeatTime                            CO_OD_ROM.consumerHeartbeatTime
        #define ODL_consumerHeartbeatTime_arrayLength               4
        #define ODA_consumerHeartbeatTime_consumerHeartbeatTime     0

/*1017, Data Type: UNSIGNED16 */
      #define OD_producerHeartbeatTime                   CO_OD_ROM.producerHeartbeatTime

/*1018, Data Type: OD_identity_t */
      #define OD_identity                                CO_OD_ROM.identity

/*1019, Data Type: UNSIGNED8 */
      #define OD_synchronousCounterOverflowValue         CO_OD_ROM.synchronousCounterOverflowValue

/*1029, Data Type: UNSIGNED8, Array[6] */
      #define OD_errorBehavior                           CO_OD_ROM.errorBehavior
      #define ODL_errorBehavior_arrayLength              6
      #define ODA_errorBehavior_communication            0
      #define ODA_errorBehavior_communicationOther       1
      #define ODA_errorBehavior_communicationPassive     2
      #define ODA_errorBehavior_generic                  3
      #define ODA_errorBehavior_deviceProfile            4
      #define ODA_errorBehavior_manufacturerSpecific     5

/*1200[1], Data Type: OD_SDOServerParameter_t, Array[1] */
      #define OD_SDOServerParameter                      CO_OD_ROM.SDOServerParameter

/*1280, Data Type: SDOClientParameter_t */
        #define OD_SDOClientParameter                               CO_OD_RAM.SDOClientParameter
/*1400[4], Data Type: OD_RPDOCommunicationParameter_t, Array[4] */
      #define OD_RPDOCommunicationParameter              CO_OD_ROM.RPDOCommunicationParameter

/*1600[4], Data Type: OD_RPDOMappingParameter_t, Array[4] */
      #define OD_RPDOMappingParameter                    CO_OD_ROM.RPDOMappingParameter

/*1800[4], Data Type: OD_TPDOCommunicationParameter_t, Array[4] */
      #define OD_TPDOCommunicationParameter              CO_OD_ROM.TPDOCommunicationParameter

/*1A00[4], Data Type: OD_TPDOMappingParameter_t, Array[4] */
      #define OD_TPDOMappingParameter                    CO_OD_ROM.TPDOMappingParameter

/*1F80, Data Type: UNSIGNED32 */
      #define OD_NMTStartup                              CO_OD_ROM.NMTStartup

/*2000, Data Type: UNSIGNED8 */
        #define OD_packNodeID                                       CO_OD_EEPROM.packNodeID

/*2001, Data Type: BOOLEAN */
        #define OD_SLOOP_Relay                                      CO_OD_RAM.SLOOP_Relay

/*2002, Data Type: BOOLEAN */
        #define OD_SLOOP1                                           CO_OD_RAM.SLOOP1

/*2003, Data Type: BOOLEAN */
        #define OD_SLOOP2                                           CO_OD_RAM.SLOOP2

/*2004, Data Type: BOOLEAN */
        #define OD_AIRS                                             CO_OD_RAM.AIRS

/*2005, Data Type: REAL32 */
        #define OD_packVoltage                                      CO_OD_RAM.packVoltage

/*2006, Data Type: REAL32 */
        #define OD_packDischargeCurrent                             CO_OD_RAM.packDischargeCurrent

/*2007, Data Type: UNSIGNED8 */
        #define OD_packSOC                                          CO_OD_RAM.packSOC

/*2008, Data Type: BOOLEAN */
        #define OD_chargeCableDetected                              CO_OD_RAM.chargeCableDetected

/*2009, Data Type: BOOLEAN */
        #define OD_chargingEnabled                                  CO_OD_RAM.chargingEnabled

/*200a, Data Type: REAL32 */
        #define OD_packChargeCurrent                                CO_OD_RAM.packChargeCurrent

/*200b, Data Type: UNSIGNED32 */
        #define OD_chargeTime                                       CO_OD_RAM.chargeTime

/*200c, Data Type: UNSIGNED32 */
        #define OD_timeSinceLastCharge                              CO_OD_RAM.timeSinceLastCharge

/*200d, Data Type: UNSIGNED8 */
        #define OD_pacManTSV_Current                                CO_OD_RAM.pacManTSV_Current

/*200e, Data Type: UNSIGNED8 */
        #define OD_packAmbientTemperature                           CO_OD_RAM.packAmbientTemperature

/*200f, Data Type: UNSIGNED8 */
        #define OD_smallestCellVoltage                              CO_OD_RAM.smallestCellVoltage

/*2010, Data Type: UNSIGNED8 */
        #define OD_greatestCellVoltage                              CO_OD_RAM.greatestCellVoltage

/*2011, Data Type: UNSIGNED8 */
        #define OD_averageCellVoltage                               CO_OD_RAM.averageCellVoltage

/*2012, Data Type: UNSIGNED8 */
        #define OD_smallestCellTemp                                 CO_OD_RAM.smallestCellTemp

/*2013, Data Type: UNSIGNED8 */
        #define OD_greatestCellTemp                                 CO_OD_RAM.greatestCellTemp

/*2014, Data Type: REAL32 */
        #define OD_averageCellTemperature                           CO_OD_RAM.averageCellTemperature

/*2015, Data Type: UNSIGNED8 */
        #define OD_numberOfExpectedCells                            CO_OD_EEPROM.numberOfExpectedCells

/*2016, Data Type: UNSIGNED8 */
        #define OD_numberOfDetectedCells                            CO_OD_RAM.numberOfDetectedCells

/*2017, Data Type: UNSIGNED8 */
        #define OD_cellmanRefreshRate                               CO_OD_EEPROM.cellmanRefreshRate

/*2018, Data Type: UNSIGNED8 */
        #define OD_ambientTempRefreshRate                           CO_OD_EEPROM.ambientTempRefreshRate

/*2019, Data Type: UNSIGNED8 */
        #define OD_fault                                            CO_OD_RAM.fault

/*201A, Data Type: UNSIGNED8 */
        #define OD_warning                                          CO_OD_RAM.warning

/*2100, Data Type: OCTET_STRING, Array[10] */
      #define OD_errorStatusBits                         CO_OD_RAM.errorStatusBits
      #define ODL_errorStatusBits_stringLength           10

/*2101, Data Type: UNSIGNED8 */
      #define OD_CANNodeID                               CO_OD_ROM.CANNodeID

/*2102, Data Type: UNSIGNED16 */
      #define OD_CANBitRate                              CO_OD_ROM.CANBitRate

/*2103, Data Type: UNSIGNED16 */
      #define OD_SYNCCounter                             CO_OD_RAM.SYNCCounter

/*2104, Data Type: UNSIGNED16 */
      #define OD_SYNCTime                                CO_OD_RAM.SYNCTime

/*2106, Data Type: UNSIGNED32 */
      #define OD_powerOnCounter                          CO_OD_EEPROM.powerOnCounter

/*2107, Data Type: UNSIGNED16, Array[5] */
      #define OD_performance                             CO_OD_RAM.performance
      #define ODL_performance_arrayLength                5
      #define ODA_performance_cyclesPerSecond            0
      #define ODA_performance_timerCycleTime             1
      #define ODA_performance_timerCycleMaxTime          2
      #define ODA_performance_mainCycleTime              3
      #define ODA_performance_mainCycleMaxTime           4

/*2108, Data Type: INTEGER16, Array[1] */
      #define OD_temperature                             CO_OD_RAM.temperature
      #define ODL_temperature_arrayLength                1
      #define ODA_temperature_mainPCB                    0

/*2109, Data Type: INTEGER16, Array[1] */
      #define OD_voltage                                 CO_OD_RAM.voltage
      #define ODL_voltage_arrayLength                    1
      #define ODA_voltage_mainPCBSupply                  0

/*2110, Data Type: INTEGER32, Array[16] */
      #define OD_variableInt32                           CO_OD_RAM.variableInt32
      #define ODL_variableInt32_arrayLength              16

/*2111, Data Type: INTEGER32, Array[16] */
      #define OD_variableROMInt32                        CO_OD_ROM.variableROMInt32
      #define ODL_variableROMInt32_arrayLength           16

/*2112, Data Type: INTEGER32, Array[16] */
      #define OD_variableNVInt32                         CO_OD_EEPROM.variableNVInt32
      #define ODL_variableNVInt32_arrayLength            16

/*2120, Data Type: OD_testVar_t */
      #define OD_testVar                                 CO_OD_RAM.testVar

/*2130, Data Type: OD_time_t */
      #define OD_time                                    CO_OD_RAM.time


/*3000, Data Type: UNSIGNED8, Array[16] */
        #define OD_I2C_Address                                      CO_OD_RAM.I2C_Address
        #define ODL_I2C_Address_arrayLength                         16
        #define ODA_I2C_Address_cell1                               0
        #define ODA_I2C_Address_cell2                               1
        #define ODA_I2C_Address_cell3                               2
        #define ODA_I2C_Address_cell4                               3
        #define ODA_I2C_Address_cell5                               4
        #define ODA_I2C_Address_cell6                               5
        #define ODA_I2C_Address_cell7                               6
        #define ODA_I2C_Address_cell8                               7
        #define ODA_I2C_Address_cell9                               8
        #define ODA_I2C_Address_cell10                              9
        #define ODA_I2C_Address_cell11                              10
        #define ODA_I2C_Address_cell12                              11
        #define ODA_I2C_Address_cell13                              12
        #define ODA_I2C_Address_cell14                              13
        #define ODA_I2C_Address_cell15                              14
        #define ODA_I2C_Address_cell16                              15

/*3001, Data Type: UNSIGNED8, Array[16] */
        #define OD_cellPosition                                     CO_OD_RAM.cellPosition
        #define ODL_cellPosition_arrayLength                        16
        #define ODA_cellPosition_cell1                              0
        #define ODA_cellPosition_cell2                              1
        #define ODA_cellPosition_cell3                              2
        #define ODA_cellPosition_cell4                              3
        #define ODA_cellPosition_cell5                              4
        #define ODA_cellPosition_cell6                              5
        #define ODA_cellPosition_cell7                              6
        #define ODA_cellPosition_cell8                              7
        #define ODA_cellPosition_cell9                              8
        #define ODA_cellPosition_cell10                             9
        #define ODA_cellPosition_cell11                             10
        #define ODA_cellPosition_cell12                             11
        #define ODA_cellPosition_cell13                             12
        #define ODA_cellPosition_cell14                             13
        #define ODA_cellPosition_cell15                             14
        #define ODA_cellPosition_cell16                             15

/*3002, Data Type: UNSIGNED8, Array[16] */
        #define OD_cellVoltage                                      CO_OD_RAM.cellVoltage
        #define ODL_cellVoltage_arrayLength                         16
        #define ODA_cellVoltage_cell1                               0
        #define ODA_cellVoltage_cell2                               1
        #define ODA_cellVoltage_cell3                               2
        #define ODA_cellVoltage_cell4                               3
        #define ODA_cellVoltage_cell5                               4
        #define ODA_cellVoltage_cell6                               5
        #define ODA_cellVoltage_cell7                               6
        #define ODA_cellVoltage_cell8                               7
        #define ODA_cellVoltage_cell9                               8
        #define ODA_cellVoltage_cell10                              9
        #define ODA_cellVoltage_cell11                              10
        #define ODA_cellVoltage_cell12                              11
        #define ODA_cellVoltage_cell13                              12
        #define ODA_cellVoltage_cell14                              13
        #define ODA_cellVoltage_cell15                              14
        #define ODA_cellVoltage_cell16                              15

/*3003, Data Type: UNSIGNED8, Array[16] */
        #define OD_cellTemperature                                  CO_OD_RAM.cellTemperature
        #define ODL_cellTemperature_arrayLength                     16
        #define ODA_cellTemperature_cell1                           0
        #define ODA_cellTemperature_cell2                           1
        #define ODA_cellTemperature_cell3                           2
        #define ODA_cellTemperature_cell4                           3
        #define ODA_cellTemperature_cell5                           4
        #define ODA_cellTemperature_cell6                           5
        #define ODA_cellTemperature_cell7                           6
        #define ODA_cellTemperature_cell8                           7
        #define ODA_cellTemperature_cell9                           8
        #define ODA_cellTemperature_cell10                          9
        #define ODA_cellTemperature_cell11                          10
        #define ODA_cellTemperature_cell12                          11
        #define ODA_cellTemperature_cell13                          12
        #define ODA_cellTemperature_cell14                          13
        #define ODA_cellTemperature_cell15                          14
        #define ODA_cellTemperature_cell16                          15

/*3004, Data Type: UNSIGNED8, Array[16] */
        #define OD_minusTerminalVoltage                             CO_OD_RAM.minusTerminalVoltage
        #define ODL_minusTerminalVoltage_arrayLength                16
        #define ODA_minusTerminalVoltage_cell1                      0
        #define ODA_minusTerminalVoltage_cell2                      1
        #define ODA_minusTerminalVoltage_cell3                      2
        #define ODA_minusTerminalVoltage_cell4                      3
        #define ODA_minusTerminalVoltage_cell5                      4
        #define ODA_minusTerminalVoltage_cell6                      5
        #define ODA_minusTerminalVoltage_cell7                      6
        #define ODA_minusTerminalVoltage_cell8                      7
        #define ODA_minusTerminalVoltage_cell9                      8
        #define ODA_minusTerminalVoltage_cell10                     9
        #define ODA_minusTerminalVoltage_cell11                     10
        #define ODA_minusTerminalVoltage_cell12                     11
        #define ODA_minusTerminalVoltage_cell13                     12
        #define ODA_minusTerminalVoltage_cell14                     13
        #define ODA_minusTerminalVoltage_cell15                     14
        #define ODA_minusTerminalVoltage_cell16                     15

/*3005, Data Type: BOOLEAN, Array[16] */
        #define OD_cellBalancingEnabled                             CO_OD_RAM.cellBalancingEnabled
        #define ODL_cellBalancingEnabled_arrayLength                16
        #define ODA_cellBalancingEnabled_cell1                      0
        #define ODA_cellBalancingEnabled_cell2                      1
        #define ODA_cellBalancingEnabled_cell3                      2
        #define ODA_cellBalancingEnabled_cell4                      3
        #define ODA_cellBalancingEnabled_cell5                      4
        #define ODA_cellBalancingEnabled_cell6                      5
        #define ODA_cellBalancingEnabled_cell7                      6
        #define ODA_cellBalancingEnabled_cell8                      7
        #define ODA_cellBalancingEnabled_cell9                      8
        #define ODA_cellBalancingEnabled_cell10                     9
        #define ODA_cellBalancingEnabled_cell11                     10
        #define ODA_cellBalancingEnabled_cell12                     11
        #define ODA_cellBalancingEnabled_cell13                     12
        #define ODA_cellBalancingEnabled_cell14                     13
        #define ODA_cellBalancingEnabled_cell15                     14
        #define ODA_cellBalancingEnabled_cell16                     15

/*3006, Data Type: UNSIGNED8, Array[16] */
        #define OD_cellFlybackVoltage                               CO_OD_RAM.cellFlybackVoltage
        #define ODL_cellFlybackVoltage_arrayLength                  16
        #define ODA_cellFlybackVoltage_cell1                        0
        #define ODA_cellFlybackVoltage_cell2                        1
        #define ODA_cellFlybackVoltage_cell3                        2
        #define ODA_cellFlybackVoltage_cell4                        3
        #define ODA_cellFlybackVoltage_cell5                        4
        #define ODA_cellFlybackVoltage_cell6                        5
        #define ODA_cellFlybackVoltage_cell7                        6
        #define ODA_cellFlybackVoltage_cell8                        7
        #define ODA_cellFlybackVoltage_cell9                        8
        #define ODA_cellFlybackVoltage_cell10                       9
        #define ODA_cellFlybackVoltage_cell11                       10
        #define ODA_cellFlybackVoltage_cell12                       11
        #define ODA_cellFlybackVoltage_cell13                       12
        #define ODA_cellFlybackVoltage_cell14                       13
        #define ODA_cellFlybackVoltage_cell15                       14
        #define ODA_cellFlybackVoltage_cell16                       15

/*3007, Data Type: UNSIGNED8, Array[16] */
        #define OD_cellBalancingCurrent                             CO_OD_RAM.cellBalancingCurrent
        #define ODL_cellBalancingCurrent_arrayLength                16
        #define ODA_cellBalancingCurrent_cell1                      0
        #define ODA_cellBalancingCurrent_cell2                      1
        #define ODA_cellBalancingCurrent_cell3                      2
        #define ODA_cellBalancingCurrent_cell4                      3
        #define ODA_cellBalancingCurrent_cell5                      4
        #define ODA_cellBalancingCurrent_cell6                      5
        #define ODA_cellBalancingCurrent_cell7                      6
        #define ODA_cellBalancingCurrent_cell8                      7
        #define ODA_cellBalancingCurrent_cell9                      8
        #define ODA_cellBalancingCurrent_cell10                     9
        #define ODA_cellBalancingCurrent_cell11                     10
        #define ODA_cellBalancingCurrent_cell12                     11
        #define ODA_cellBalancingCurrent_cell13                     12
        #define ODA_cellBalancingCurrent_cell14                     13
        #define ODA_cellBalancingCurrent_cell15                     14
        #define ODA_cellBalancingCurrent_cell16                     15

/*3008, Data Type: UNSIGNED8, Array[16] */
        #define OD_cellSOC                                          CO_OD_RAM.cellSOC
        #define ODL_cellSOC_arrayLength                             16
        #define ODA_cellSOC_cell1                                   0
        #define ODA_cellSOC_cell2                                   1
        #define ODA_cellSOC_cell3                                   2
        #define ODA_cellSOC_cell4                                   3
        #define ODA_cellSOC_cell5                                   4
        #define ODA_cellSOC_cell6                                   5
        #define ODA_cellSOC_cell7                                   6
        #define ODA_cellSOC_cell8                                   7
        #define ODA_cellSOC_cell9                                   8
        #define ODA_cellSOC_cell10                                  9
        #define ODA_cellSOC_cell11                                  10
        #define ODA_cellSOC_cell12                                  11
        #define ODA_cellSOC_cell13                                  12
        #define ODA_cellSOC_cell14                                  13
        #define ODA_cellSOC_cell15                                  14
        #define ODA_cellSOC_cell16                                  15

/*3009, Data Type: UNSIGNED16, Array[16] */
        #define OD_minCellVoltage                                   CO_OD_EEPROM.minCellVoltage
        #define ODL_minCellVoltage_arrayLength                      16
        #define ODA_minCellVoltage_cell1                            0
        #define ODA_minCellVoltage_cell2                            1
        #define ODA_minCellVoltage_cell3                            2
        #define ODA_minCellVoltage_cell4                            3
        #define ODA_minCellVoltage_cell5                            4
        #define ODA_minCellVoltage_cell6                            5
        #define ODA_minCellVoltage_cell7                            6
        #define ODA_minCellVoltage_cell8                            7
        #define ODA_minCellVoltage_cell9                            8
        #define ODA_minCellVoltage_cell10                           9
        #define ODA_minCellVoltage_cell11                           10
        #define ODA_minCellVoltage_cell12                           11
        #define ODA_minCellVoltage_cell13                           12
        #define ODA_minCellVoltage_cell14                           13
        #define ODA_minCellVoltage_cell15                           14
        #define ODA_minCellVoltage_cell16                           15

/*300a, Data Type: UNSIGNED16, Array[16] */
        #define OD_maxCellVoltage                                   CO_OD_EEPROM.maxCellVoltage
        #define ODL_maxCellVoltage_arrayLength                      16
        #define ODA_maxCellVoltage_cell1                            0
        #define ODA_maxCellVoltage_cell2                            1
        #define ODA_maxCellVoltage_cell3                            2
        #define ODA_maxCellVoltage_cell4                            3
        #define ODA_maxCellVoltage_cell5                            4
        #define ODA_maxCellVoltage_cell6                            5
        #define ODA_maxCellVoltage_cell7                            6
        #define ODA_maxCellVoltage_cell8                            7
        #define ODA_maxCellVoltage_cell9                            8
        #define ODA_maxCellVoltage_cell10                           9
        #define ODA_maxCellVoltage_cell11                           10
        #define ODA_maxCellVoltage_cell12                           11
        #define ODA_maxCellVoltage_cell13                           12
        #define ODA_maxCellVoltage_cell14                           13
        #define ODA_maxCellVoltage_cell15                           14
        #define ODA_maxCellVoltage_cell16                           15

/*300b, Data Type: UNSIGNED8, Array[16] */
        #define OD_minCellTemp                                      CO_OD_EEPROM.minCellTemp
        #define ODL_minCellTemp_arrayLength                         16
        #define ODA_minCellTemp_cell1                               0
        #define ODA_minCellTemp_cell2                               1
        #define ODA_minCellTemp_cell3                               2
        #define ODA_minCellTemp_cell4                               3
        #define ODA_minCellTemp_cell5                               4
        #define ODA_minCellTemp_cell6                               5
        #define ODA_minCellTemp_cell7                               6
        #define ODA_minCellTemp_cell8                               7
        #define ODA_minCellTemp_cell9                               8
        #define ODA_minCellTemp_cell10                              9
        #define ODA_minCellTemp_cell11                              10
        #define ODA_minCellTemp_cell12                              11
        #define ODA_minCellTemp_cell13                              12
        #define ODA_minCellTemp_cell14                              13
        #define ODA_minCellTemp_cell15                              14
        #define ODA_minCellTemp_cell16                              15

/*300c, Data Type: UNSIGNED8, Array[16] */
        #define OD_maxCellTemp                                      CO_OD_EEPROM.maxCellTemp
        #define ODL_maxCellTemp_arrayLength                         16
        #define ODA_maxCellTemp_cell1                               0
        #define ODA_maxCellTemp_cell2                               1
        #define ODA_maxCellTemp_cell3                               2
        #define ODA_maxCellTemp_cell4                               3
        #define ODA_maxCellTemp_cell5                               4
        #define ODA_maxCellTemp_cell6                               5
        #define ODA_maxCellTemp_cell7                               6
        #define ODA_maxCellTemp_cell8                               7
        #define ODA_maxCellTemp_cell9                               8
        #define ODA_maxCellTemp_cell10                              9
        #define ODA_maxCellTemp_cell11                              10
        #define ODA_maxCellTemp_cell12                              11
        #define ODA_maxCellTemp_cell13                              12
        #define ODA_maxCellTemp_cell14                              13
        #define ODA_maxCellTemp_cell15                              14
        #define ODA_maxCellTemp_cell16                              15

/*300d, Data Type: UNSIGNED16, Array[16] */
        #define OD_minCellCurrent                                   CO_OD_EEPROM.minCellCurrent
        #define ODL_minCellCurrent_arrayLength                      16
        #define ODA_minCellCurrent_cell1                            0
        #define ODA_minCellCurrent_cell2                            1
        #define ODA_minCellCurrent_cell3                            2
        #define ODA_minCellCurrent_cell4                            3
        #define ODA_minCellCurrent_cell5                            4
        #define ODA_minCellCurrent_cell6                            5
        #define ODA_minCellCurrent_cell7                            6
        #define ODA_minCellCurrent_cell8                            7
        #define ODA_minCellCurrent_cell9                            8
        #define ODA_minCellCurrent_cell10                           9
        #define ODA_minCellCurrent_cell11                           10
        #define ODA_minCellCurrent_cell12                           11
        #define ODA_minCellCurrent_cell13                           12
        #define ODA_minCellCurrent_cell14                           13
        #define ODA_minCellCurrent_cell15                           14
        #define ODA_minCellCurrent_cell16                           15

/*300e, Data Type: UNSIGNED16, Array[16] */
        #define OD_maxCellCurrent                                   CO_OD_EEPROM.maxCellCurrent
        #define ODL_maxCellCurrent_arrayLength                      16
        #define ODA_maxCellCurrent_cell1                            0
        #define ODA_maxCellCurrent_cell2                            1
        #define ODA_maxCellCurrent_cell3                            2
        #define ODA_maxCellCurrent_cell4                            3
        #define ODA_maxCellCurrent_cell5                            4
        #define ODA_maxCellCurrent_cell6                            5
        #define ODA_maxCellCurrent_cell7                            6
        #define ODA_maxCellCurrent_cell8                            7
        #define ODA_maxCellCurrent_cell9                            8
        #define ODA_maxCellCurrent_cell10                           9
        #define ODA_maxCellCurrent_cell11                           10
        #define ODA_maxCellCurrent_cell12                           11
        #define ODA_maxCellCurrent_cell13                           12
        #define ODA_maxCellCurrent_cell14                           13
        #define ODA_maxCellCurrent_cell15                           14
        #define ODA_maxCellCurrent_cell16                           15

/*300f, Data Type: UNSIGNED16, Array[16] */
        #define OD_maxCellChargeVoltage                             CO_OD_EEPROM.maxCellChargeVoltage
        #define ODL_maxCellChargeVoltage_arrayLength                16
        #define ODA_maxCellChargeVoltage_cell1                      0
        #define ODA_maxCellChargeVoltage_cell2                      1
        #define ODA_maxCellChargeVoltage_cell3                      2
        #define ODA_maxCellChargeVoltage_cell4                      3
        #define ODA_maxCellChargeVoltage_cell5                      4
        #define ODA_maxCellChargeVoltage_cell6                      5
        #define ODA_maxCellChargeVoltage_cell7                      6
        #define ODA_maxCellChargeVoltage_cell8                      7
        #define ODA_maxCellChargeVoltage_cell9                      8
        #define ODA_maxCellChargeVoltage_cell10                     9
        #define ODA_maxCellChargeVoltage_cell11                     10
        #define ODA_maxCellChargeVoltage_cell12                     11
        #define ODA_maxCellChargeVoltage_cell13                     12
        #define ODA_maxCellChargeVoltage_cell14                     13
        #define ODA_maxCellChargeVoltage_cell15                     14
        #define ODA_maxCellChargeVoltage_cell16                     15

/*3010, Data Type: UNSIGNED8, Array[16] */
        #define OD_cellSOC_Min                                      CO_OD_EEPROM.cellSOC_Min
        #define ODL_cellSOC_Min_arrayLength                         16
        #define ODA_cellSOC_Min_cell1                               0
        #define ODA_cellSOC_Min_cell2                               1
        #define ODA_cellSOC_Min_cell3                               2
        #define ODA_cellSOC_Min_cell4                               3
        #define ODA_cellSOC_Min_cell5                               4
        #define ODA_cellSOC_Min_cell6                               5
        #define ODA_cellSOC_Min_cell7                               6
        #define ODA_cellSOC_Min_cell8                               7
        #define ODA_cellSOC_Min_cell9                               8
        #define ODA_cellSOC_Min_cell10                              9
        #define ODA_cellSOC_Min_cell11                              10
        #define ODA_cellSOC_Min_cell12                              11
        #define ODA_cellSOC_Min_cell13                              12
        #define ODA_cellSOC_Min_cell14                              13
        #define ODA_cellSOC_Min_cell15                              14
        #define ODA_cellSOC_Min_cell16                              15

/*3011, Data Type: BOOLEAN, Array[16] */
        #define OD_cellSOH                                          CO_OD_RAM.cellSOH
        #define ODL_cellSOH_arrayLength                             16
        #define ODA_cellSOH_cell1                                   0
        #define ODA_cellSOH_cell2                                   1
        #define ODA_cellSOH_cell3                                   2
        #define ODA_cellSOH_cell4                                   3
        #define ODA_cellSOH_cell5                                   4
        #define ODA_cellSOH_cell6                                   5
        #define ODA_cellSOH_cell7                                   6
        #define ODA_cellSOH_cell8                                   7
        #define ODA_cellSOH_cell9                                   8
        #define ODA_cellSOH_cell10                                  9
        #define ODA_cellSOH_cell11                                  10
        #define ODA_cellSOH_cell12                                  11
        #define ODA_cellSOH_cell13                                  12
        #define ODA_cellSOH_cell14                                  13
        #define ODA_cellSOH_cell15                                  14
        #define ODA_cellSOH_cell16                                  15


/*6000, Data Type: UNSIGNED8, Array[8] */
      #define OD_readInput8Bit                           CO_OD_RAM.readInput8Bit
      #define ODL_readInput8Bit_arrayLength              8

/*6200, Data Type: UNSIGNED8, Array[8] */
      #define OD_writeOutput8Bit                         CO_OD_RAM.writeOutput8Bit
      #define ODL_writeOutput8Bit_arrayLength            8

/*6401, Data Type: INTEGER16, Array[12] */
      #define OD_readAnalogueInput16Bit                  CO_OD_RAM.readAnalogueInput16Bit
      #define ODL_readAnalogueInput16Bit_arrayLength     12

/*6411, Data Type: INTEGER16, Array[8] */
      #define OD_writeAnalogueOutput16Bit                CO_OD_RAM.writeAnalogueOutput16Bit
      #define ODL_writeAnalogueOutput16Bit_arrayLength   8

#endif
// clang-format on
