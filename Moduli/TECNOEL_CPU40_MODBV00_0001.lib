//*************************************************************************************************
//************** Gestione Protocollo Modbus e sue periferiche
//*************************************************************************************************


/*** BeginHeader TaskModbV00*/
void TaskModbV00();
void TaskModbV00_Init();
/*** EndHeader  */

#define IO_Modb_WAIT 30

#define CAREL1_CONF 0
#define CAREL1_PRES 8
#define PLCONT_CONF 1
#define PLCONT_PRES 9
#define FEMTOA_CONF 2
#define FEMTOA_PRES 10
#define THERMO_CONF 3
#define THERMO_PRES 11
#define PLCIOS_CONF 4
#define PLCIOS_PRES 12
#define CAREL2_CONF 5
#define CAREL2_PRES 13

unsigned int TaskModbV00_PC;
unsigned int TaskModbV00_TimerSONCA1;
unsigned int TaskModbV00_TimerSONCA2;
unsigned int TaskModbV00_TimerPLCCNT;
unsigned int TaskModbV00_TimerARFEMT;

unsigned int TaskModbV00_Output;
unsigned int TaskModbV00_TaskTimer;
unsigned int TaskModbV00_RxCounter;
unsigned int TaskModbV00_RxWaitFirst;
unsigned int TaskModbV00_RxWaitNext;
char         TaskModbV00_RxString[200];


unsigned int BiosIOModbusThermoCounter;
unsigned int BiosIOModbusThermoVelFanc;
unsigned int BiosIOModbusThermoSet;
unsigned int BiosIOModbusThermoFlag2;

int TaskModbV00_SerOpen(long baud){switch (ModbPort)    {case 1: {return serBopen(baud); break;} case 2: {return serDopen(baud); break;} case 3: {return serCopen(baud); break;}  default:  return 0;}}
int TaskModbV00_SerPuts(char* s)  {switch (ModbPort)    {case 1: {return serBputs(s); break;} case 2: {return serDputs(s); break;} case 3: {return serCputs(s); break;}  default:  return 0;}}
int TaskModbV00_SerGetc()         {switch (ModbPort)    {case 1: {return serBgetc(); break;} case 2: {return serDgetc(); break;} case 3: {return serCgetc(); break;}  default:  return -1;}}
int TaskModbV00_SerPutc(char c)   {switch (ModbPort)    {case 1: {return serBputc(c); break;} case 2: {return serDputc(c); break;} case 3: {return serCputc(c); break;}  default:  return -1;}}

void TaskModbV00_Init()
{
 auto int Cont1;
 TaskModbV00_PC=0;
 TaskModbV00_TimerSONCA1=BiosIOGetNewTimer();
 TaskModbV00_TimerSONCA2=BiosIOGetNewTimer();
 TaskModbV00_TimerPLCCNT=BiosIOGetNewTimer();
 TaskModbV00_TimerARFEMT=BiosIOGetNewTimer();
 TaskModbV00_TaskTimer=0;
 TaskModbV00_SerOpen(9600);
 TaskModbV00_Output=0;
 BiosIOModbusThermoVelFanc=0;
 BiosIOModbusThermoSet=0;
 BiosIOModbusThermoFlag2=0;
 BiosIOModbusThermoCounter=50;
}


/********************************************************************************/
/********* Gestione Periferiche ModBus ******************************************/
/********************************************************************************/

unsigned int crc16(unsigned char *ptr, int count)
{
    unsigned int crc;
    char i;
    crc = 0xffff;
    while (--count >= 0)
    {
        crc = crc ^ (unsigned int) *ptr++;
        i = 8;
        do
        {
            if (crc & 0x0001)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc = (crc >> 1);
        } while(--i);
    }
    return (crc);
}

void TaskModbV00_RxAnswer()
{
 auto int Comando;
 TaskModbV00_RxCounter=0;
 Comando=0;
 while (Comando!=-1 && TaskModbV00_RxCounter<60)
  {
  Comando=TaskModbV00_SerGetc();
  if (Comando!=-1 && TaskModbV00_RxCounter<60)
   {
   TaskModbV00_RxString[TaskModbV00_RxCounter]=Comando;
   TaskModbV00_RxCounter++;
   //printf("%d ",Comando);
   }
  }
 //printf(" Next %d\n",TaskModbV00_RxCounter);
}

int TaskModbV00_RxAnswerNew(unsigned int NChar)
{
 auto int Comando;
 if (!NChar)
  {
  TaskModbV00_RxCounter=1000;
  TaskModbV00_RxWaitFirst=0;
  TaskModbV00_RxWaitNext=0;
  //printf ("ModBus Preset\n");
  return 0;
  }
 Comando=TaskModbV00_SerGetc();
 if (Comando==-1 && TaskModbV00_RxCounter==1000) {TaskModbV00_RxWaitFirst++; return (TaskModbV00_RxWaitFirst<1000);}
 if (Comando!=-1)
  {
  //printf ("ModBus Received %d   %d\n",TaskModbV00_RxCounter,Comando);
  if (TaskModbV00_RxCounter==1000) TaskModbV00_RxCounter=0;
  TaskModbV00_RxWaitNext=0;
  TaskModbV00_RxString[TaskModbV00_RxCounter]=Comando;
  TaskModbV00_RxCounter++;
  return TaskModbV00_RxCounter<NChar;
  }
 else
  {TaskModbV00_RxWaitNext++; return TaskModbV00_RxWaitNext<100;}
}

void TaskModbV00_TxQuery(char c0, char c1, char c2, char c3, char c4, char c5, unsigned int Ritorno)
{
 auto char S[6];
 auto unsigned int Crc16;
 S[0]=c0; S[1]=c1; S[2]=c2; S[3]=c3; S[4]=c4; S[5]=c5; Crc16=crc16(S,6);
 TaskModbV00_SerPutc(S[0]);
 TaskModbV00_SerPutc(S[1]);
 TaskModbV00_SerPutc(S[2]);
 TaskModbV00_SerPutc(S[3]);
 TaskModbV00_SerPutc(S[4]);
 TaskModbV00_SerPutc(S[5]);
 TaskModbV00_SerPutc(Crc16%256);
 TaskModbV00_SerPutc(Crc16/256);
 TaskModbV00_TaskTimer=IO_Modb_WAIT;
 TaskModbV00_PC=Ritorno;
 TaskModbV00_RxAnswerNew(0);
}


/********************************************************************************/
/********* Gestione Periferica ModBus PLC I/O ***********************************/
/********************************************************************************/

void TaskModbV00_00()    // Leggi Input
{
 if (!BiosIOGetBitReg(MODB_CNFPR0,PLCIOS_CONF)) {BiosIOSetBitReg(MODB_CNFPR0,PLCIOS_PRES,0); TaskModbV00_PC=2; return;}
 TaskModbV00_TxQuery(0x02,0x03,0x00,0x12,0x00,0x01,1);
}

void TaskModbV00_01()   // Controlla risposta - Leggi Input
{
 TaskModbV00_TaskTimer--; if (TaskModbV00_TaskTimer!=0) return;
 TaskModbV00_RxAnswer();
 if (TaskModbV00_RxCounter==15)
  {
  BiosIOSetBitReg(MODB_CNFPR0,PLCIOS_PRES,1);
  }
 else
  {
  BiosIOSetBitReg(MODB_CNFPR0,PLCIOS_PRES,0);
  }
 TaskModbV00_PC=2;
}

/********************************************************************************/
/********* Gestione Periferica ModBus Display Thermostato ***********************/
/********************************************************************************/

void TaskModbV00_02()  // Leggi Thermo
{
 if (!BiosIOGetBitReg(MODB_CNFPR0,THERMO_CONF)) {BiosIOSetBitReg(MODB_CNFPR0,THERMO_PRES,0); TaskModbV00_PC=4; return;}
 BiosIOModbusThermoCounter--;
 if (BiosIOModbusThermoCounter!=0) {TaskModbV00_PC=4; return;}
 BiosIOModbusThermoCounter=50;
 TaskModbV00_TxQuery(0x01,0x03,0x00,0x00,0x00,0x08,3);
}


void TaskModbV00_03()  // Controlla risposta - Leggi Thermo
{
 TaskModbV00_TaskTimer--; if (TaskModbV00_TaskTimer!=0) return;
 TaskModbV00_RxAnswer();
 if (TaskModbV00_RxCounter==29)
  {
  Thermo_TempAmbGradi=TaskModbV00_RxString[12];
  Thermo_TempAmbDecimi=TaskModbV00_RxString[14];
  Thermo_TempSet=TaskModbV00_RxString[20];
  Thermo_VelFanc=TaskModbV00_RxString[22];
  Thermo_Flag2=TaskModbV00_RxString[26];
  BiosIOSetBitReg(MODB_CNFPR0,THERMO_PRES,1);
  }
 else
  {
  BiosIOSetBitReg(MODB_CNFPR0,THERMO_PRES,0);
  }
 TaskModbV00_PC=4;
}

void TaskModbV00_04()    // Scrivi Output se variati
{
 if (!BiosIOGetBitReg(MODB_CNFPR0,PLCIOS_CONF))     {TaskModbV00_PC=6; return;}
 if (bit(&TaskModbV00_Output,15)==0) {TaskModbV00_PC=6; return;}
 res(&TaskModbV00_Output,15);
 TaskModbV00_TxQuery(0x02,0x06,0x00,0x14,TaskModbV00_Output/256,TaskModbV00_Output%256,5);
}

void TaskModbV00_05()    // Leggi Risposta - Scrivi Output se variati
{
 TaskModbV00_TaskTimer--; if (TaskModbV00_TaskTimer!=0) return;
 TaskModbV00_RxAnswer();
 TaskModbV00_PC=6;
}

void TaskModbV00_06()   // Invio Velocità a Thermo
{
 if (!BiosIOGetBitReg(MODB_CNFPR0,THERMO_CONF))       {TaskModbV00_PC=8; return;}
 if (bit(&BiosIOModbusThermoVelFanc,15)==0) {TaskModbV00_PC=8; return;}
 res(&BiosIOModbusThermoVelFanc,15);
 TaskModbV00_TxQuery(0x01,0x06,0x00,0x05,0x00,BiosIOModbusThermoVelFanc,7);
}

void TaskModbV00_07()
{
 TaskModbV00_TaskTimer--; if (TaskModbV00_TaskTimer!=0) return;
 TaskModbV00_RxAnswer();
 TaskModbV00_PC=8;
}

void TaskModbV00_08()   // Invio Set a Thermo
{
 if (!BiosIOGetBitReg(MODB_CNFPR0,THERMO_CONF))     {TaskModbV00_PC=10; return;}
 if (bit(&BiosIOModbusThermoSet,15)==0)   {TaskModbV00_PC=10; return;}
 res(&BiosIOModbusThermoSet,15);
 TaskModbV00_TxQuery(0x01,0x06,0x00,0x04,0x00,BiosIOModbusThermoSet,9);
}

void TaskModbV00_09()
{
 TaskModbV00_TaskTimer--; if (TaskModbV00_TaskTimer!=0) return;
 TaskModbV00_RxAnswer();
 TaskModbV00_PC=10;
}

void TaskModbV00_10()   // Invio Flag2 a Thermo
{
 if (!BiosIOGetBitReg(MODB_CNFPR0,THERMO_CONF))     {TaskModbV00_PC=12; return;}
 if (bit(&BiosIOModbusThermoFlag2,15)==0) {TaskModbV00_PC=12; return;}
 res(&BiosIOModbusThermoFlag2,15);
 TaskModbV00_TxQuery(0x01,0x06,0x00,0x07,0x00,BiosIOModbusThermoFlag2,11);
}

void TaskModbV00_11()
{
 TaskModbV00_TaskTimer--; if (TaskModbV00_TaskTimer!=0) return;
 TaskModbV00_RxAnswer();
 TaskModbV00_PC=12;
}

/********************************************************************************/
/********* Gestione Periferica ModBus Sonda CAREL1  ADDR=128 ********************/
/********************************************************************************/

void TaskModbV00_12()  // Leggi Sonda Carel
{
 if (!BiosIOGetBitReg(MODB_CNFPR0,CAREL1_CONF)) {BiosIOSetBitReg(MODB_CNFPR0,CAREL1_PRES,0); TaskModbV00_PC=14; return;}
 if (!BiosIOGetTimer(TaskModbV00_TimerSONCA1))  {TaskModbV00_PC=14; return;}
 //printf("%s\n","Sonda Carel 1 Tx Request");
 BiosIOSetTimer(TaskModbV00_TimerSONCA1,200);
 TaskModbV00_TxQuery(0x80,0x03,0x00,0x00,0x00,0x08,13); // ADDR=128 - 8 Registri dal 000
 //printf("Ask Sonda Carel\n");
}

void TaskModbV00_13()  // Controlla risposta - Sonda Carel
{
 //printf("%s\n","Sonda Carel 1 Rx Test");
 if (TaskModbV00_RxAnswerNew(29)) return;
 if (TaskModbV00_RxCounter==29)
  {
  //printf("%d %d\n",TaskModbV00_RxString[11],TaskModbV00_RxString[12]);
  //printf("%d %d\n",TaskModbV00_RxString[13],TaskModbV00_RxString[14]);
  //printf("%d %d\n",TaskModbV00_RxString[15],TaskModbV00_RxString[16]);
  BiosIOSetReg(MODB_CA1TOF,TaskModbV00_RxString[11]*256+TaskModbV00_RxString[12]);
  BiosIOSetReg(MODB_CA1UOF,TaskModbV00_RxString[13]*256+TaskModbV00_RxString[14]);
  BiosIOSetReg(MODB_CA1TDF,TaskModbV00_RxString[15]*256+TaskModbV00_RxString[16]);
  BiosIOSetReg(MODB_CA1UDF,TaskModbV00_RxString[17]*256+TaskModbV00_RxString[18]);
  BiosIOSetReg(MODB_CA1TVA,TaskModbV00_RxString[21]*256+TaskModbV00_RxString[22]);
  BiosIOSetReg(MODB_CA1UVA,TaskModbV00_RxString[23]*256+TaskModbV00_RxString[24]);
  BiosIOSetReg(MODB_CA1RVA,TaskModbV00_RxString[25]*256+TaskModbV00_RxString[26]);
  BiosIOSetBitReg(MODB_CNFPR0,CAREL1_PRES,1);
  }
 else
  {
  BiosIOSetBitReg(MODB_CNFPR0,CAREL1_PRES,0);
  }
 TaskModbV00_PC=14;
}


/********************************************************************************/
/********* Gestione Periferica ModBus PLC CONTROLLER ****************************/
/********************************************************************************/

void TaskModbV00_14()  // Scrivi Plc Comando Luci
{
 if (!BiosIOGetBitReg(MODB_CNFPR0,PLCONT_CONF)) {BiosIOSetBitReg(MODB_CNFPR0,PLCONT_PRES,0); TaskModbV00_PC=16; return;}

 if (BiosIOGetReg(MODB_SICMD0)!=0) {TaskModbV00_TxQuery(0x03,0x06,0x00,0xCA,BiosIOGetHighReg(MODB_SICMD0),BiosIOGetLowReg(MODB_SICMD0),15); BiosIOSetReg(MODB_SICMD0,0); return;}
 if (BiosIOGetReg(MODB_SICMD1)!=0) {TaskModbV00_TxQuery(0x03,0x06,0x00,0xCB,BiosIOGetHighReg(MODB_SICMD1),BiosIOGetLowReg(MODB_SICMD1),15); BiosIOSetReg(MODB_SICMD1,0); return;}

 if (BiosIOGetReg(MODB_S10116)!=0) {
 TaskModbV00_TxQuery(0x03,0x06,0x00,0xD2,BiosIOGetHighReg(MODB_S10116),BiosIOGetLowReg(MODB_S10116),15); BiosIOSetReg(MODB_SL0116,BiosIOGetReg(MODB_SL0116)|BiosIOGetReg(MODB_S10116)); BiosIOSetReg(MODB_S10116,0); return;}
 if (BiosIOGetReg(MODB_S11732)!=0) {
 TaskModbV00_TxQuery(0x03,0x06,0x00,0xD3,BiosIOGetHighReg(MODB_S11732),BiosIOGetLowReg(MODB_S11732),15); BiosIOSetReg(MODB_SL1732,BiosIOGetReg(MODB_SL1732)|BiosIOGetReg(MODB_S11732)); BiosIOSetReg(MODB_S11732,0); return;}
 if (BiosIOGetReg(MODB_S00116)!=0) {
 TaskModbV00_TxQuery(0x03,0x06,0x00,0xC8,BiosIOGetHighReg(MODB_S00116),BiosIOGetLowReg(MODB_S00116),15); BiosIOSetReg(MODB_SL0116,BiosIOGetReg(MODB_SL0116)&~BiosIOGetReg(MODB_S00116)); BiosIOSetReg(MODB_S00116,0); return;}
 if (BiosIOGetReg(MODB_S01732)!=0) {
 TaskModbV00_TxQuery(0x03,0x06,0x00,0xC9,BiosIOGetHighReg(MODB_S01732),BiosIOGetLowReg(MODB_S01732),15); BiosIOSetReg(MODB_SL1732,BiosIOGetReg(MODB_SL1732)&~BiosIOGetReg(MODB_S01732)); BiosIOSetReg(MODB_S01732,0); return;}

 TaskModbV00_PC=16;
}

void TaskModbV00_15()  // Controlla risposta - Plc Comando Luci
{
 if (TaskModbV00_RxAnswerNew(16)) return;
 TaskModbV00_PC=16;
 //printf("Plc Eseguito: %u\n",TaskModbV00_RxCounter);
}

void TaskModbV00_16()  // Leggi Plc Stato Luci
{
 if (!BiosIOGetBitReg(MODB_CNFPR0,PLCONT_CONF))          {TaskModbV00_PC=20; return;}
 if (!BiosIOGetTimer(TaskModbV00_TimerPLCCNT)) {TaskModbV00_PC=20; return;}
 BiosIOSetTimer(TaskModbV00_TimerPLCCNT,10);
 TaskModbV00_TxQuery(0x03,0x03,0x00,0x64,0x00,0x04,17);   // ADDR=003 - 4 Registri dal 100 // Stato Luci
}

void TaskModbV00_17()  // Controlla risposta - Plc Stato Luci
{
 if (TaskModbV00_RxAnswerNew(21)) return;
 if (TaskModbV00_RxCounter==21)
  {
  BiosIOSetReg(MODB_SL0116,TaskModbV00_RxString[11]*256+TaskModbV00_RxString[12]);
  BiosIOSetReg(MODB_SL1732,TaskModbV00_RxString[13]*256+TaskModbV00_RxString[14]);
  BiosIOSetReg(MODB_SISTAI,TaskModbV00_RxString[15]*256+TaskModbV00_RxString[16]);
  BiosIOSetReg(MODB_SILATI,TaskModbV00_RxString[17]*256+TaskModbV00_RxString[18]);
  BiosIOSetBitReg(MODB_CNFPR0,PLCONT_PRES,1);
  }
 else
  {
  BiosIOSetBitReg(MODB_CNFPR0,PLCONT_PRES,0);
  }
 TaskModbV00_PC=18;
}

void TaskModbV00_18()  // Scrivi Plc Comando Canali Liberi
{
 TaskModbV00_TxQuery(0x03,0x06,0x00,0x6E,BiosIOGetHighReg(MODB_SX0000),BiosIOGetLowReg(MODB_SX0000),19);
}

void TaskModbV00_19()  // Controlla risposta - Plc Comando Canali Liberi
{
 TaskModbV00_TaskTimer--; if (TaskModbV00_TaskTimer!=0) return;
 TaskModbV00_RxAnswer();
 TaskModbV00_PC=20;
 //printf("Executed\n");
}

/********************************************************************************/
/********* Gestione Periferica ModBus ANALIZZATORE RETE FEMTO *******************/
/********************************************************************************/

void TaskModbV00_20()  // Leggi Analizzatore Rete Femto
{
 if (!BiosIOGetBitReg(MODB_CNFPR0,FEMTOA_CONF)){BiosIOSetBitReg(MODB_CNFPR0,FEMTOA_PRES,0); TaskModbV00_PC=26; return;}
 if (!BiosIOGetTimer(TaskModbV00_TimerARFEMT)) {TaskModbV00_PC=26; return;}
 BiosIOSetTimer(TaskModbV00_TimerARFEMT,100);
 TaskModbV00_TxQuery(0x1B,0x04,0x00,0xDA,0x00,0x04,21);   // ADDR=027 - 04 Registri dal 218 // Tensione
}

void TaskModbV00_21()  // Controlla risposta - Analizzatore Rete Femto
{
 union { float fVal; unsigned char bytes[4]; } value;
 if (TaskModbV00_RxAnswerNew(21)) return;
 if (TaskModbV00_RxCounter==21)
  {
  value.bytes[0]=TaskModbV00_RxString[14];
  value.bytes[1]=TaskModbV00_RxString[13];
  value.bytes[2]=TaskModbV00_RxString[12];
  value.bytes[3]=TaskModbV00_RxString[11];
  BiosIOSetReg(MODB_ARFREQ,value.fVal*100);
  value.bytes[0]=TaskModbV00_RxString[18];
  value.bytes[1]=TaskModbV00_RxString[17];
  value.bytes[2]=TaskModbV00_RxString[16];
  value.bytes[3]=TaskModbV00_RxString[15];
  BiosIOSetReg(MODB_ARTENS,value.fVal*100);
  BiosIOSetBitReg(MODB_CNFPR0,FEMTOA_PRES,1);
  }
 else
  {
  BiosIOSetBitReg(MODB_CNFPR0,FEMTOA_PRES,0);
  }
 TaskModbV00_PC=22;
}

void TaskModbV00_22()  // Leggi Analizzatore Rete Femto
{
 if (!BiosIOGetBitReg(MODB_CNFPR0,FEMTOA_CONF)) {TaskModbV00_PC=26; return;}
 if (!BiosIOGetBitReg(MODB_CNFPR0,FEMTOA_PRES)) {TaskModbV00_PC=26; return;}
 TaskModbV00_TxQuery(0x1B,0x04,0x01,0x12,0x00,0x0A,23);   // ADDR=027 - 10 Registri dal 274 // Tensione
}

void TaskModbV00_23()  // Controlla risposta - Analizzatore Rete Femto
{
 union { float fVal; unsigned char bytes[4]; } value;
 if (TaskModbV00_RxAnswerNew(33)) return;
 if (TaskModbV00_RxCounter==33)
  {
  value.bytes[0]=TaskModbV00_RxString[14];
  value.bytes[1]=TaskModbV00_RxString[13];
  value.bytes[2]=TaskModbV00_RxString[12];
  value.bytes[3]=TaskModbV00_RxString[11];
  BiosIOSetReg(MODB_ARCORR,value.fVal*100);
  value.bytes[0]=TaskModbV00_RxString[18];
  value.bytes[1]=TaskModbV00_RxString[17];
  value.bytes[2]=TaskModbV00_RxString[16];
  value.bytes[3]=TaskModbV00_RxString[15];
  BiosIOSetReg(MODB_ARPOTA,(int)value.fVal);
  value.bytes[0]=TaskModbV00_RxString[22];
  value.bytes[1]=TaskModbV00_RxString[21];
  value.bytes[2]=TaskModbV00_RxString[20];
  value.bytes[3]=TaskModbV00_RxString[19];
  BiosIOSetReg(MODB_ARPOTR,(int)value.fVal);
  //printf("Femto Presente: %f\n",value.fVal);
  value.bytes[0]=TaskModbV00_RxString[26];
  value.bytes[1]=TaskModbV00_RxString[25];
  value.bytes[2]=TaskModbV00_RxString[24];
  value.bytes[3]=TaskModbV00_RxString[23];
  BiosIOSetReg(MODB_ARPOTP,(int)value.fVal);
  }
 else
  {
  //printf("Femto Assente: %u\n",TaskModbV00_RxCounter);
  }
 TaskModbV00_PC=24;
}

void TaskModbV00_24()  // Leggi Analizzatore Rete Femto
{
 if (!BiosIOGetBitReg(MODB_CNFPR0,FEMTOA_CONF))  {TaskModbV00_PC=26; return;}
 if (!BiosIOGetBitReg(MODB_CNFPR0,FEMTOA_PRES))  {TaskModbV00_PC=26; return;}
 TaskModbV00_TxQuery(0x1B,0x04,0x01,0x9C,0x00,0x08,25);   // ADDR=027 - 8 Registri dal 412 // Energy
}

void TaskModbV00_25()  // Controlla risposta - Analizzatore Rete Femto
{
 union { long int fVal; unsigned char bytes[4]; } value;
 if (TaskModbV00_RxAnswerNew(29)) return;
 if (TaskModbV00_RxCounter==29)
  {
  value.bytes[0]=TaskModbV00_RxString[14];
  value.bytes[1]=TaskModbV00_RxString[13];
  value.bytes[2]=TaskModbV00_RxString[12];
  value.bytes[3]=TaskModbV00_RxString[11];
  BiosIOSetReg(MODB_ARTENA,(unsigned int)value.fVal);
  //printf("Femto Presente: %d\n",BiosIOGetReg(VAL_ARTENA));
  value.bytes[0]=TaskModbV00_RxString[18];
  value.bytes[1]=TaskModbV00_RxString[17];
  value.bytes[2]=TaskModbV00_RxString[16];
  value.bytes[3]=TaskModbV00_RxString[15];
  BiosIOSetReg(MODB_ARTENI,(unsigned int)value.fVal);
  //printf("Femto Presente: %d\n",BiosIOGetReg(VAL_ARTENI));
  value.bytes[0]=TaskModbV00_RxString[22];
  value.bytes[1]=TaskModbV00_RxString[21];
  value.bytes[2]=TaskModbV00_RxString[20];
  value.bytes[3]=TaskModbV00_RxString[19];
  BiosIOSetReg(MODB_ARTENC,(unsigned int)value.fVal);
  //printf("Femto Presente: %d\n",BiosIOGetReg(VAL_ARTENC));
  value.bytes[0]=TaskModbV00_RxString[26];
  value.bytes[1]=TaskModbV00_RxString[25];
  value.bytes[2]=TaskModbV00_RxString[24];
  value.bytes[3]=TaskModbV00_RxString[23];
  BiosIOSetReg(MODB_ARTENP,(unsigned int)value.fVal);
  //printf("Femto Presente: %d\n",BiosIOGetReg(VAL_ARTENP));
  }
 else
  {
  //printf("Femto Assente: %u\n",TaskModbV00_RxCounter);
  }
 TaskModbV00_PC=26;
}

/********************************************************************************/
/********* Gestione Periferica ModBus Sonda CAREL2  ADDR=129 ********************/
/********************************************************************************/

void TaskModbV00_26()  // Leggi Sonda Carel
{
 if (!BiosIOGetBitReg(MODB_CNFPR0,CAREL2_CONF))   {BiosIOSetBitReg(MODB_CNFPR0,CAREL2_PRES,0); TaskModbV00_PC=28; return;}
 if (!BiosIOGetTimer(TaskModbV00_TimerSONCA2))    {TaskModbV00_PC=28; return;}
 //printf("%s\n","Sonda Carel 2 Tx Request");
 BiosIOSetTimer(TaskModbV00_TimerSONCA2,200);
 TaskModbV00_TxQuery(0x81,0x03,0x00,0x00,0x00,0x08,27); // ADDR=129 - 8 Registri dal 000
 //printf("Ask Sonda Carel\n");
}

void TaskModbV00_27()  // Controlla risposta - Sonda Carel
{
 if (TaskModbV00_RxAnswerNew(29)) return;
 if (TaskModbV00_RxCounter==29)
  {
  BiosIOSetReg(MODB_CA2TOF,TaskModbV00_RxString[11]*256+TaskModbV00_RxString[12]);
  BiosIOSetReg(MODB_CA2UOF,TaskModbV00_RxString[13]*256+TaskModbV00_RxString[14]);
  BiosIOSetReg(MODB_CA2TDF,TaskModbV00_RxString[15]*256+TaskModbV00_RxString[16]);
  BiosIOSetReg(MODB_CA2UDF,TaskModbV00_RxString[17]*256+TaskModbV00_RxString[18]);
  BiosIOSetReg(MODB_CA2TVA,TaskModbV00_RxString[21]*256+TaskModbV00_RxString[22]);
  BiosIOSetReg(MODB_CA2UVA,TaskModbV00_RxString[23]*256+TaskModbV00_RxString[24]);
  BiosIOSetReg(MODB_CA2RVA,TaskModbV00_RxString[25]*256+TaskModbV00_RxString[26]);
  BiosIOSetBitReg(MODB_CNFPR0,CAREL2_PRES,1);
  }
 else
  {
  BiosIOSetBitReg(MODB_CNFPR0,CAREL2_PRES,0);
  }
 TaskModbV00_PC=28;
}


/********************************************************************************/
/********* Gestione Periferica ModBus Sonda CAREL1 CONFIGURAZIONE ADDR=128 ******/
/********************************************************************************/

void TaskModbV00_28()
{
 //printf("1Ask Sonda Carel\n");
 if (!BiosIOGetBitReg(MODB_CNFPR0,CAREL1_CONF))   {TaskModbV00_PC=30; return;}
 if (BiosIOGetReg(MODB_CA1TOF)>255) {TaskModbV00_TxQuery(0x80,0x06,0x00,0x00,0,BiosIOGetLowReg(MODB_CA1TOF),29); BiosIOSetReg(MODB_CA1TOF,BiosIOGetLowReg(MODB_CA1TOF)); return;}
 if (BiosIOGetReg(MODB_CA1UOF)>255) {TaskModbV00_TxQuery(0x80,0x06,0x00,0x01,0,BiosIOGetLowReg(MODB_CA1UOF),29); BiosIOSetReg(MODB_CA1UOF,BiosIOGetLowReg(MODB_CA1UOF)); return;}
 if (BiosIOGetReg(MODB_CA1TDF)>255) {TaskModbV00_TxQuery(0x80,0x06,0x00,0x02,0,BiosIOGetLowReg(MODB_CA1TDF),29); BiosIOSetReg(MODB_CA1TDF,BiosIOGetLowReg(MODB_CA1TDF)); return;}
 if (BiosIOGetReg(MODB_CA1UDF)>255) {TaskModbV00_TxQuery(0x80,0x06,0x00,0x03,0,BiosIOGetLowReg(MODB_CA1UDF),29); BiosIOSetReg(MODB_CA1UDF,BiosIOGetLowReg(MODB_CA1UDF)); return;}
 TaskModbV00_PC=30;
}

void TaskModbV00_29()
{
 if (TaskModbV00_RxAnswerNew(16)) return;
 TaskModbV00_PC=30;
}

/********************************************************************************/
/********* Gestione Periferica ModBus Sonda CAREL2 CONFIGURAZIONE ADDR=129 ******/
/********************************************************************************/

void TaskModbV00_30()
{
 if (!BiosIOGetBitReg(MODB_CNFPR0,CAREL2_CONF))   {TaskModbV00_PC=32; return;}
 if (BiosIOGetReg(MODB_CA2TOF)>255) {TaskModbV00_TxQuery(0x81,0x06,0x00,0x00,0,BiosIOGetLowReg(MODB_CA2TOF),29); BiosIOSetReg(MODB_CA2TOF,BiosIOGetLowReg(MODB_CA2TOF)); return;}
 if (BiosIOGetReg(MODB_CA2UOF)>255) {TaskModbV00_TxQuery(0x81,0x06,0x00,0x01,0,BiosIOGetLowReg(MODB_CA2UOF),29); BiosIOSetReg(MODB_CA2UOF,BiosIOGetLowReg(MODB_CA2UOF)); return;}
 if (BiosIOGetReg(MODB_CA2TDF)>255) {TaskModbV00_TxQuery(0x81,0x06,0x00,0x02,0,BiosIOGetLowReg(MODB_CA2TDF),29); BiosIOSetReg(MODB_CA2TDF,BiosIOGetLowReg(MODB_CA2TDF)); return;}
 if (BiosIOGetReg(MODB_CA2UDF)>255) {TaskModbV00_TxQuery(0x81,0x06,0x00,0x03,0,BiosIOGetLowReg(MODB_CA2UDF),29); BiosIOSetReg(MODB_CA2UDF,BiosIOGetLowReg(MODB_CA2UDF)); return;}
 TaskModbV00_PC=32;
}

void TaskModbV00_31()
{
 if (TaskModbV00_RxAnswerNew(16)) return;
 TaskModbV00_PC=32;
}

void TaskModbV00_32()
{
 TaskModbV00_PC=0;
}

void TaskModbV00()
{
 switch (TaskModbV00_PC)
                     {
                     case  0:  {TaskModbV00_00(); break;}
                     case  1:  {TaskModbV00_01(); break;}
                     case  2:  {TaskModbV00_02(); break;}
                     case  3:  {TaskModbV00_03(); break;}
                     case  4:  {TaskModbV00_04(); break;}
                     case  5:  {TaskModbV00_05(); break;}
                     case  6:  {TaskModbV00_06(); break;}
                     case  7:  {TaskModbV00_07(); break;}
                     case  8:  {TaskModbV00_08(); break;}
                     case  9:  {TaskModbV00_09(); break;}
                     case 10:  {TaskModbV00_10(); break;}
                     case 11:  {TaskModbV00_11(); break;}
                     case 12:  {TaskModbV00_12(); break;}
                     case 13:  {TaskModbV00_13(); break;}
                     case 14:  {TaskModbV00_14(); break;}
                     case 15:  {TaskModbV00_15(); break;}
                     case 16:  {TaskModbV00_16(); break;}
                     case 17:  {TaskModbV00_17(); break;}
                     case 18:  {TaskModbV00_18(); break;}
                     case 19:  {TaskModbV00_19(); break;}
                     case 20:  {TaskModbV00_20(); break;}
                     case 21:  {TaskModbV00_21(); break;}
                     case 22:  {TaskModbV00_22(); break;}
                     case 23:  {TaskModbV00_23(); break;}
                     case 24:  {TaskModbV00_24(); break;}
                     case 25:  {TaskModbV00_25(); break;}
                     case 26:  {TaskModbV00_26(); break;}
                     case 27:  {TaskModbV00_27(); break;}
                     case 28:  {TaskModbV00_28(); break;}
                     case 29:  {TaskModbV00_29(); break;}
                     case 30:  {TaskModbV00_30(); break;}
                     case 31:  {TaskModbV00_31(); break;}
                     case 32:  {TaskModbV00_32(); break;}
                     }
}




