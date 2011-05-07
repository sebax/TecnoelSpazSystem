/*****************************************************

     TECNOEL S.R.L.
     PROGETTO:
     HARDWARE: 	TECNOEL CPU 40 V 0.00
     FIRMWARE:
     FILE: 			TECNOEL_CPU40_CARSPA_0000.C
     COMMENTO:  	MAIN PROGRAM DI GESTIONE CARRELLO SPAZZOLA GAVIRATE

     DATA: 			14.04.2011

     MODIFICHE:

     00.00 -> 00.00	Versione di base


     DA FARE:

     Set Confort in uscita
     Controllo temporizzazioni senza printf

     INFORMAZIONI:


*****************************************************************************
Gestione Modb
Si Possono Configurare 2 Moduli di Ingresso 2 Moduli di Uscita ed un Modulo Misto. In Tutto (40 In e 40 Out)
Gli Indirizzi dei moduli sono fissi: 7053-(00) 7053-(01) 7043-(02) 7043-(03) 7050-(04) Velocità 38400
Configurazione Ingresso = AAAAAAAABBBBBBBB  : A = Tipo 0 Nullo / 1 Passo Passo OUT : B = DATI / 2 Scenario : B = Dati
*****************************************************************************

*****************************************************************************
Tracciato Tessera Proxi 10 Caratteri

Offset 	Length     	Significato
00			02				Impianto Decimale [00..99]
02		   01				Tipo Decimale [0..9]
03       06      		Codice Tessera [000000 .. 999999]
09		   01				CheckSum tipo EAN13 [0..9]
*****************************************************************************

Descrizine Dip Switch:

SW1:

   1) Bit 0 : Reset Net Factory Default
   2) Bit 1 : Non Usato
   3) Bit 2 : Non Usato
   4) Bit 3 : Non Usato
   5) Bit 4 : Non Usato
   6) Bit 5 : Non Usato
   7) Bit 6 : Non Usato
   8) Bit 7 : Non Usato


SW2:

   1) Bit 0 : Non Usato
   2) Bit 1 : Non Usato
   3) Bit 2 : Non Usato
   4) Bit 3 : Non Usato
   5) Bit 4 : Non Usato
   6) Bit 5 : Non Usato
   7) Bit 6 : Non Usato
   8) Bit 7 : Non Usato

******************************************************/


/****** Definizione Porte Seriali *******************************************/
/* Porta Seriale B - RS485 SLAVE - PHOENIX **********************************/
#define BINBUFSIZE  127
#define BOUTBUFSIZE 127
#define SERB485_USEPORTF
#define SERB_USEPORTD
/* Porta Seriale C - IRF MASTER - PHOENIX **********************************/
#define CINBUFSIZE  127
#define COUTBUFSIZE 127
#define SERC485_USEPORTB
/* Porta Seriale D - RS485 I/O - RJ45 **************************************/
#define DINBUFSIZE  127
#define DOUTBUFSIZE 127
#define SERD485_USEPORTA

#memmap xmem

#define USE_ETHERNET 1
#define TCPCONFIG 0
#define MY_TCP_PORT     300

// Definizioni TCP/IP
#use "TECNOEL_CPU40_BIOS_0001_UTIL.LIB"
#use "TECNOEL_CPU40_BIOS_0202_IO.LIB"
#use "TECNOEL_CPU40_BIOS_0502_PROT.LIB"
#use "TECNOEL_CPU40_BIOS_0001_MEMORY.LIB"
#use "dcrtcp.lib"

#define VERSIONE_FW    "CARSPA V00.00"
#define CONST_COLDBOOT "6595473"
//#define DEBUG 0

#define OUTPUT_LED_RUN                   0
#define OUTPUT_LED_AUX                   1

//	USER FLAG ARRAY:
#define FLAG_MESSAGGIO_PRESENTE		  0x00   // Messaggio Presente

#define FLAG_RICHIESTA_RESET     	  0x11   // Riservato per logica Richiesta Reset da Protocollo

#define FLAG_THER_FLCIR1              0x20   // 25[00] Circuito Thermo Attivo 1
#define FLAG_THER_FLLIB1              0x21   // 25[01] Funzione Thermo Estate 1
#define FLAG_THER_FLEST1              0x22   // 25[02] Funzione Thermo Estate 1
#define FLAG_THER_FLSCH1              0x23   // 25[03] Funzione Thermo Scheduler 1

#define FLAG_THER_FLCIR2              0x24   // 25[04] Circuito Thermo Attivo 2
#define FLAG_THER_FLLIB2              0x25   // 25[05] Funzione Thermo Estate 2
#define FLAG_THER_FLEST2              0x26   // 25[06] Funzione Thermo Estate 2
#define FLAG_THER_FLSCH2              0x27   // 25[07] Funzione Thermo Scheduler 2

#define DIMENSIONE_STRINGA_FW            13  // 13 Caratteri Firmware Identification

#define VALORE_PRESCALER_TIMER_DECIMI          1000  // Risultano valori assoluti in decimi di secondo.
#define VALORE_PRESCALER_TIMER_TRENTESIMI       333  // Risultano valori assoluti in trentesimi di secondo.

#define Cr31Port          3   //   1 = SerB:Phoenix 5 Pin     2 = SerD:RJ485   3 = SerC:MASTER
#define ModbPort          2   //   1 = SerB:Phoenix 5 Pin     2 = SerD:RJ485   3 = SerC:MASTER
#define DConPort          1   //   1 = SerB:Phoenix 5 Pin     2 = SerD:RJ485   3 = SerC:MASTER

//*************************************************************************************************
//********       Application Registers
//*************************************************************************************************


//*************************************************************************************************
// Periferiche ModBus 24 Possibili
// Reg 100 Bit 00 Sonda Umidità Temperatura 1 (Modbus Address = 128d)
// Reg 100 Bit 01 Plc Controller (Modbus Address = 003d)
// Reg 100 Bit 02 Analizzatore Rete Femto (Modbus Address = 027d)
// Reg 100 Bit 03 Thermo Display (Modbus Address = 001d)
// Reg 100 Bit 04 Plc I/O (Modbus Address = 002d)
// Reg 100 Bit 05 Sonda Umidità Temperatura 2 (Modbus Address = 129d)
// Tipo Periferiche ModBus
// Reg 103 Bit 00 Sonda Umidità Temperatura 0 = Tipo CAREL 1 = Tipo HUATO
//*************************************************************************************************
#define MODB_CNFPR0  0x0100  // Configurazione Presenza Periferiche ModBus (Vedi Sopra)
#define MODB_CNFPR1  0x0101  // Configurazione Presenza Periferiche ModBus (Vedi Sopra)
#define MODB_CNFPR2  0x0102  // Configurazione Presenza Periferiche ModBus (Vedi Sopra)
#define MODB_CNFPER  0x0103  // Configurazione Tipo Periferiche ModBus (Vedi Sopra)

// Modulo MODBUS - Periferica Sonda Temperatura Umidità 1
#define MODB_SO1TOF  0x0104  // Valore Offset Temperatura Sonda 1
#define MODB_SO1UOF  0x0105  // Valore Offset Umidità Sonda  1
#define MODB_SO1TDF  0x0106  // Valore Differenziale Temperatura Sonda 1
#define MODB_SO1UDF  0x0107  // Valore Differenziale Umidità Sonda 1
#define MODB_SO1TVA  0x0108  // Valore Temperatura da Sonda 1
#define MODB_SO1UVA  0x0109  // Valore Umidità da Sonda 1
#define MODB_SO1RVA  0x010A  // Valore Punto Rugiada da Sonda 1

// Modulo MODBUS - Periferica PLC Controller
#define MODB_SL0116	0x010B  // Stato Luci Circuiti 01 .. 16
#define MODB_SL1732	0x010C  // Stato Luci Circuiti 07 .. 32
#define MODB_S00116	0x010D  // Imposta a 0 Luci Circuiti 01 .. 16
#define MODB_S01732	0x010E  // Imposta a 0 Luci Circuiti 17 .. 32
#define MODB_S10116	0x010F  // Imposta a 1 Luci Circuiti 01 .. 16
#define MODB_S11732	0x0110  // Imposta a 1 Luci Circuiti 17 .. 32
#define MODB_SX0000	0x0111  // Imposta Canali Liberi Plc Controller
#define MODB_SISTAI  0x0112  // Stato Ingressi Antintrusione
#define MODB_SILATI  0x0113  // Latch Ingressi Antintrusione
#define MODB_SICMD0  0x0114  // Comando Modbus Antintrusione
#define MODB_SICMD1  0x0115  // Comando Modbus Antintrusione

// Modulo MODBUS - Periferica Analizzatore Rete
#define MODB_ARFREQ  0x0120  // Valore Frequenza Rete
#define MODB_ARTENS  0x0121  // Valore Tensione Rete
#define MODB_ARCORR  0x0122  // Valore Corrente Totale
#define MODB_ARPOTA  0x0123  // Valore Potenza Attiva
#define MODB_ARPOTR  0x0124  // Valore Potenza Reattiva
#define MODB_ARPOTP  0x0125  // Valore Potenza Apparente
#define MODB_ARTENA  0x0126  // Valore Energia Attiva
#define MODB_ARTENC  0x0127  // Valore Energia Attiva
#define MODB_ARTENI  0x0128  // Valore Energia Reattiva
#define MODB_ARTENP  0x0129  // Valore Energia Apparente

// Modulo DCON -
#define DCON_CNFPRE  0x0130  // Configurazione Singolo Modulo (LSB) Presenza Singolo Modulo (MSB)
#define DCON_SCANNE  0x0131  // Scanner e inizializzazione
#define DCON_DCNI00  0x0132  // Modulo DCON 00 - 7053 - Valore Attuale ingressi
#define DCON_DCNI01  0x0133  // Modulo DCON 01 - 7053 - Valore Attuale ingressi
#define DCON_DCNI04  0x0134  // Modulo DCON 04 - 7050 - Valore Attuale ingressi
#define DCON_DCNO02  0x0135  // Modulo DCON 02 - 7043 - Valore Uscite
#define DCON_DCNO03  0x0136  // Modulo DCON 03 - 7043 - Valore Uscite
#define DCON_DCNO04  0x0137  // Modulo DCON 04 - 7050 - Valore Uscite

// Modulo MODBUS - Periferica Sonda Tempertaura Umidità 2
#define MODB_SO2TOF  0x0144  // Valore Offset Temperatura Sonda 2
#define MODB_SO2UOF  0x0145  // Valore Offset Umidità Sonda 2
#define MODB_SO2TDF  0x0146  // Valore Differenziale Temperatura Sonda 2
#define MODB_SO2UDF  0x0147  // Valore Differenziale Umidità Sonda 2
#define MODB_SO2TVA  0x0148  // Valore Temperatura da Sonda 2
#define MODB_SO2UVA  0x0149  // Valore Umidità da Sonda 2
#define MODB_SO2RVA  0x014A  // Valore Punto Rugiada da Sonda 2



// Modulo MACRO - Attuazione Logiche Complesse
#define MACR_FIRSTR  0x0150 // Primo registro Macro
#define MACR_LASTRE  0x017F // Ultimo registro Macro

#define MACR_TIMR0C  0x0180 // Primo  Regsitro Timer Configurazione
#define MACR_TIMR0V  0x0181 // Primo  Registro Timer Valore
#define MACR_TIMR7C  0x018E // Ultimo Regsitro Timer Configurazione  (8d Timers)
#define MACR_TIMR7V  0x018F // Ultimo Registro Timer Valore          (8d Timers)

#define SYST_DIPSWI	0x001A  // Flag Dip Switch
#define SYST_INPUT0	0x001B  // Registro Ingressi CPU40
#define SYST_OUTPU0	0x001C  // Registro Uscite CPU40
#define SYST_CNFIN0	0x001D  // Configurazione Ingresso O CPU40 Come DCON
#define SYST_CNFIN1	0x001E  // Configurazione Ingresso 1 CPU40 Come DCON

#define VAL_UFLAG0	0x0023  // Flag User 00
#define VAL_UFLAG1	0x0024  // Flag User 01
#define VAL_UFLAG2	0x0025  // Flag User 03
#define VAL_UFLAG3	0x0026  // Flag User 04
#define VAL_UFLAG4	0x0027  // Flag User 05
#define VAL_UFLAG5	0x0028  // Flag User 06
#define VAL_UFLAG6	0x0029  // Flag User 07
#define VAL_UFLAG7	0x002A  // Flag User 08

#define MESS_PRESEN  0x002C  // Messaggi Presenti in Buffer
#define MESS_FREEBY  0x002D  // Messaggi Free Bytes in Buffer

#define TCP_VADDRH	0x0030 // Parte Alta  XXX Indirizzo IP XXX.XXX.ZZZ.ZZZ
#define TCP_VADDRL	0x0031 // Parte Bassa ZZZ Indirizzo IP XXX.XXX.ZZZ.ZZZ
#define TCP_VMASKH	0x0032 // Parte Alta  XXX Mask      IP XXX.XXX.ZZZ.ZZZ
#define TCP_VMASKL	0x0033 // Parte Bassa ZZZ Mask      IP XXX.XXX.ZZZ.ZZZ
#define TCP_VGWAYH	0x0034 // Parte Alta  XXX Gateway   IP XXX.XXX.ZZZ.ZZZ
#define TCP_VGWAYL	0x0035 // Parte Bassa ZZZ Gateway   IP XXX.XXX.ZZZ.ZZZ

// Configurazione Ingressi SCEN_DCCIXX
// MSB = Numero Ingresso 0:Nullo 1:PassoPasso 2:Scenario
// LSB = Se MSB=1:Numero Canale Uscita Se MSB=2:Numero Scenario
// NB Il canale 40 ed il canale 41 sono i due ingressi della CPU
#define SCEN_DCCI00  0x00A0  // Configurazione Ingresso 00 (48 Registri Fino a 0x00CF)

// Configurazione SCENARI
// 8 Scenari Possibili 00..07
// Per ogni scenario è possibile accendere o spegnere circuiti luce
// Es. Scenario 0 Circuiti da Accendere del modulo DCON 2 in SCEN_DCS100+00
// Es. Scenario 0 Circuiti da Accendere del modulo DCON 3 in SCEN_DCS100+01
// Es. Scenario 0 Circuiti da Accendere del modulo DCON 4 in SCEN_DCS100+02
// Es. Scenario 0 Circuiti da Spegnere  del modulo DCON 2 in SCEN_DCS100+03
// Es. Scenario 0 Circuiti da Spegnere  del modulo DCON 3 in SCEN_DCS100+04
// Es. Scenario 0 Circuiti da Spegnere  del modulo DCON 4 in SCEN_DCS100+05
#define SCEN_DCS012  0x00D0  // Configurazione Scenario 00 (8 Scenari x 6 Word = 48 Registri fino a 0x00FF)


// Modulo I/O
#define IO_PIS01_DIRECTION_REG								0x0000000
#define IO_PIS01_DIRECTION_BIT							0
#define IO_PIS01_ENABLE_REG									0x0000000
#define IO_PIS01_ENABLE_BIT								0
#define IO_PIS01_POSITION_REG								0x0000000
#define IO_PIS01_VOLTAGE_REG								0x0000000
#define IO_PIS01_REFERENCE_REG								0x0000000
#define IO_AZM01_ALARM_REG			DCON_DCNI00
#define IO_AZM01_ALARM_BIT					0
#define IO_AZM01_ENABLE_REG			SYST_OUTPU0
#define IO_AZM01_ENABLE_BIT					0
#define IO_AZM01_VOLTAGE_REG								0x0000000
#define IO_AZM01_DIRSPEED_REG								0x0000000
#define IO_AZS01_ALARM_REG			DCON_DCNI00
#define IO_AZS01_ALARM_BIT					1
#define IO_AZS01_ENABLE_REG									0x0000000
#define IO_AZS01_ENABLE_BIT								0
#define IO_AZS01_VOLTAGE_REG								0x0000000
#define IO_AZS01_DIRSPEED_REG								0x0000000
#define IO_AUX01_ENABLE_REG			SYST_OUTPU0
#define IO_AUX01_ENABLE_BIT					1
#define IO_AUX01_VOLTAGE_REG								0x0000000
#define IO_PFV01_VOLTAGE_REG								0x0000000
#define IO_BAT01_VOLTAGE_REG								0x0000000
#define IO_BAT12_VOLTAGE_REG								0x0000000
#define IO_SEN01_STATE_REG			DCON_DCNI00
#define IO_SEN01_STATE_BIT					2
#define IO_SEN01_UNLOCKED_REG		DCON_DCNI00
#define IO_SEN01_UNLOCKED_BIT				3
#define IO_SEN01_LOCK_REG									0x0000000
#define IO_SEN01_LOCK_BIT								0
#define IO_SEN02_STATE_REG			SYST_INPUT0
#define IO_SEN02_STATE_BIT					0
#define IO_SEN03_STATE_REG			SYST_INPUT0
#define IO_SEN03_STATE_BIT					1
#define IO_SEN04_STATE_REG			DCON_DCNI00
#define IO_SEN04_STATE_BIT					4
#define IO_SEN04_UNLOCKED_REG		DCON_DCNI00
#define IO_SEN04_UNLOCKED_BIT				5
#define IO_SEN04_LOCK_REG									0x0000000
#define IO_SEN04_LOCK_BIT								0
#define IO_SEN04_BYPASS_REG									0x0000000
#define IO_SEN04_BYPASS_BIT								0
#define IO_SEN05_STATE_REG			DCON_DCNI00
#define IO_SEN05_STATE_BIT					6
#define IO_SEN05_BYPASS_REG									0x0000000
#define IO_SEN05_BYPASS_BIT								0
#define IO_ENC01_STATE_REG			DCON_DCNI00
#define IO_ENC01_STATE_BIT					7
#define IO_KEY01_STATE_REG			DCON_DCNI00
#define IO_KEY01_STATE_BIT					8
#define IO_FUN01_STATE_REG			DCON_DCNI00
#define IO_FUN01_STATE_BIT					9
#define IO_PWT01_ENABLE_REG									0x0000000
#define IO_PWT01_ENABLE_BIT								0
#define IO_PWN01_ENABLE_REG									0x0000000
#define IO_PWN01_ENABLE_BIT								0


// REGISTRI DI APPLICAZIONE

// LPS
#define LPS_ODOMETER  	 	0x014B  // Posizione inerziale 			espressa in step (buchi letti dall'encoder)
#define LPS_SPEED  	 		0x014D  // Velocita' istantanea 		espressa in step/finestra temporale

// DPS
#define DPS_STATUS_REG				0x014E
#define DPS_STATUS_WORKING_BIT		0
#define DPS_STATUS_DONE_BIT			1
#define DPS_STATUS_FAULT_BIT		2
#define DPS_FLAGS_REG				0x014F
#define DPS_LOCATION				0x0150  // Localizzazione sul campo 	espressa con l'identificativo di area
#define DPS_TIMEOUT 				3
// DPS legge i 5 SEN (no unlocked) e il ENGINE_VERSUS e setta il registro DPS_LOCATION.
// Lo stato di "non so dove sono" e' UNO SOLO: 9999

// ENGINE
#define ENGINE_STATUS_REG			0x0151
#define ENGINE_STATUS_WORKING_BIT	0
#define ENGINE_STATUS_DONE_BIT		1
#define ENGINE_STATUS_FAULT_BIT		2
#define ENGINE_FLAGS_REG			0x0152
#define ENGINE_VERSUS_BIT			0		// 1:avanti/ 0:indietro

// BRUSH
#define BRUSH_STATUS_REG			0x0153
#define BRUSH_STATUS_WORKING_BIT	0
#define BRUSH_STATUS_DONE_BIT		1
#define BRUSH_STATUS_FAULT_BIT		2
#define BRUSH_FLAGS_REG				0x0154
#define BRUSH_VERSUS_BIT			0		// 1:avanti/ 0:indietro



//*************************************************************************************************
//********       Variabili dell'applicazione
//*************************************************************************************************

// HIGH LEVEL ENTITIES variables
//#use "boss.lib"
// #use "safeguard.lib"
// #use "communicationmanager.lib"
// DRIVERS variables
// #use "deparker.lib"
// #use "traslaholder.lib"
// #use "runner.lib"
// #use "cleaner.lib"
// #use "parker.lib"
// OBJECTS variables
#use "lps.lib"
#use "dps.lib"
// #use "engine.lib"
// #use "piston.lib"
//#use "brush.lib"
// #use "logger.lib"

unsigned long APPLICATION_BUFFER_MESSAGGI_ADDR;

char 				APPLICATION_COLDBOOT_TEST1[7];
char 				APPLICATION_VERF[DIMENSIONE_STRINGA_FW+1];

char			 Thermo_TempSet;
char			 Thermo_Flag2;
char			 Thermo_VelFanc;
char			 Thermo_TempAmbGradi;
char			 Thermo_TempAmbDecimi;

//int Task10RxProxi[20];

char APPLICATION_COLDBOOT_TEST2[7];

//*************************************************************************************************
//************** Utilities generiche
//*************************************************************************************************

void CRTCreaTimeStampDisplay(int Punto,char Dest[20])
{
 struct tm		rtc;					// time struct
 unsigned long	t0;					// seconds
 t0 = read_rtc();
 mktm(&rtc, t0);
 rtc.tm_year=rtc.tm_year-100;
 Dest[0]='2';
 Dest[1]=(rtc.tm_hour/10+48);
 Dest[2]=(rtc.tm_hour%10+48);
 if (Punto==0) Dest[3]=':'; else Dest[3]='.';
 Dest[4]=(rtc.tm_min/10+48);
 Dest[5]=(rtc.tm_min%10+48);
 Dest[6]=' ';
 Dest[7]=(rtc.tm_mday/10+48);
 Dest[8]=(rtc.tm_mday%10+48);
 Dest[9]='/ ';
 Dest[10]=(rtc.tm_mon/10+48);
 Dest[11]=(rtc.tm_mon%10+48);
 Dest[12]='/ ';
 Dest[13]='2 ';
 Dest[14]='0 ';
 Dest[15]=(rtc.tm_year/10+48);
 Dest[16]=(rtc.tm_year%10+48);
 Dest[17]=0;
}

//*************************************************************************************************
//************** Utilities dell'applicativo
//*************************************************************************************************

nodebug void MainPrintDebugTask(int TaskIndex)
{
 BiosIOSetBitFlag(FLAG_RICHIESTA_RESET,1);
 #ifdef DEBUG
 ("Default %d\n",TaskIndex);
 #endif
}

unsigned int MainGetApplChannel(unsigned int Ch)
{
 if (Ch<16){return BiosIOGetBitReg(DCON_DCNI00,Ch);}
 if (Ch<32){return BiosIOGetBitReg(DCON_DCNI01,Ch-16);}
 if (Ch<48){return BiosIOGetBitReg(DCON_DCNI04,Ch-32);}
 if (Ch<64){return BiosIOGetBitReg(DCON_DCNO02,Ch-48);}
 if (Ch<80){return BiosIOGetBitReg(DCON_DCNO03,Ch-64);}
 if (Ch<96){return BiosIOGetBitReg(DCON_DCNO04,Ch-80);}
 return 0;
}

void MainSetApplChannel(unsigned int Ch, char Flag)
{
 if (Ch<48){return;}
 if (Ch<64){BiosIOSetBitReg(DCON_DCNO02,Ch-48,Flag);}
 if (Ch<80){BiosIOSetBitReg(DCON_DCNO03,Ch-64,Flag);}
 if (Ch<96){BiosIOSetBitReg(DCON_DCNO04,Ch-80,Flag);}
 return;
}

unsigned int MainTestColdBoot()
{
 if (strncmp(APPLICATION_COLDBOOT_TEST1,CONST_COLDBOOT,7)!=0) return 0;
 if (strncmp(APPLICATION_COLDBOOT_TEST2,CONST_COLDBOOT,7)!=0) return 0;
 return 1;
}

void MainSetColdBoot()
{
 strncpy(APPLICATION_COLDBOOT_TEST1,CONST_COLDBOOT,7);
 strncpy(APPLICATION_COLDBOOT_TEST2,CONST_COLDBOOT,7);
}

#use "TECNOEL_CPU40_MESSV00_0000.LIB"
#use "TECNOEL_CPU40_MODBV00_0002.LIB"
#use "TECNOEL_CPU40_DCONV00_0002.LIB"

void MainClearMemory()
{
 BiosIOClearRegsValue();
 BiosIOClearCodsValue();
 TaskMessV00_Clear();
 // Falori di Default dei registri
 //IOREGS[DLY_IMPRL1].Value=20;
 //IOREGS[DLY_IMPRL2].Value=20;
}


int SetupISR()
{
	#asm
	// set up interrupt vector	0
	ld		a, iir				; get internal interrupt register
	ld		h, a					; save as high byte of address
	ld		l, 0B0h				; want timer B interrupt register
	ld		iy, hl				; iy now points to the interrupt vector for timer B
	ld		hl, my_isr
	ld		(iy), 0C3h			; the JP command
	ld		(iy+1), hl			; where to jump to
	#endasm
   WrPortI(TBCR, &TBCRShadow, 0x01);	// clock timer B with (perclk/2) and set interrupt level to 1
	WrPortI(TBL1R, NULL, 0x00);			// set initial match!
	WrPortI(TBM1R, NULL, 0x00);			// set initial match!
	WrPortI(TBCSR, &TBCSRShadow, 0x03);	// enable timer B and B1 match interrupts
}


main()
{

   //MainMemoryDebug();
   //MainMemoryDebug();

   #ifdef DEBUG
   #endif

   strcpy(APPLICATION_VERF,VERSIONE_FW);
   if (!MainTestColdBoot()) {MainSetColdBoot(); MainClearMemory();}

   SetupISR();
  	BiosIOInit();
   BiosPROTNetInit();
	BiosIOSetBitFlag(FLAG_RICHIESTA_RESET,0);

   BiosIOSetReg(DCON_CNFPRE,0);
   BiosIOSetBitReg(DCON_CNFPRE,0,1);
   BiosIOSetBitReg(DCON_CNFPRE,2,1);

   // *************************************************************************
   // 				Inizializzazione Task e Stringhe Temporanee
   // *************************************************************************


   TaskMessV00_Init();
   TaskModbV00_Init();
   TaskDConV00_Init();

   // HIGH LEVEL ENTITIES
   //TaskBoss_Init();
   //TaskSafeGuard_Init();
   // TaskCommunicationManager_Init();

   // DRIVERS
   // TaskDriverDeparker_Init();
   // TaskDriverTraslaholder_Init();
   // TaskDriverRunner_Init();
   // TaskDriverCleaner_Init();
   // TaskDriverParker_Init();

   // OBJECTS
   TaskLPS_Init();
   TaskDPS_Init();	//field positioning system
   // TaskEngine_Init();
   // TaskPiston_Init();
   //TaskBrush_Init();
   // TaskLogger_Init();


   TaskMessV00_MessaggioPush("MAIN",0,"$40");

   // *************************************************************************
   // 				Ciclo Principale
   // *************************************************************************
  	BiosIOInitWatchDog();


  	//DEBUG PER TENERE AUSILIARI ACCESI
  	BiosIOSetBitReg(IO_AUX01_ENABLE_REG,IO_AUX01_ENABLE_BIT,1);

   while (1)
	 {

	 if (!BiosIOGetBitFlag(FLAG_RICHIESTA_RESET))VdHitWd(WatchDogRun);
    BiosProtMainTask();
    BiosIOMainTask();

	 //*************************************************************************************************
    //************** Sevizi Vari da eseguire ad ogni ciclo
    //*************************************************************************************************

    BiosProtMainTask();
    TaskModbV00();
    BiosProtMainTask();
    TaskDConV00();
    BiosProtMainTask();
    TaskMessV00();
    BiosProtMainTask();
    //TaskMacrV00();



    // HIGH LEVEL ENTITIES
    //TaskBoss_Main();
    //TaskSafeGuard_Main();
    //TaskCommunicationManager_Main();

    // DRIVERS
    //TaskDriverDeparker_Main();
    //TaskDriverTraslaholder_Main();
    //TaskDriverRunner_Main();
    //TaskDriverCleaner_Main();
    //TaskDriverParker_Main();

    // OBJECTS
    TaskLPS_Main();
    TaskDPS_Main();
    //TaskEngine_Main();
    //TaskPiston_Main();
    //TaskBrush_Main();
    //TaskLogger_Main();

    }
}


////////////////////////////////////////////////////////////////////////////////
// This short assembly routine sets up the interrupt handler for our timer B
// interrupt.  The assembly command "JP my_isr" is inserted in the internal
// interrupt vector table.
////////////////////////////////////////////////////////////////////////////////

void my_isr();

/****** interrupt routine for external interrupt 0 ******/

nodebug root interrupt void my_isr()
{
	//BiosCardInterrupt();
	if (FlagTimerDecimi==0) {if (PrescalerTimerDecimi!=0) PrescalerTimerDecimi--; else FlagTimerDecimi=1;}
	if (FlagTimerTrentesimi==0) {if (PrescalerTimerTrentesimi!=0) PrescalerTimerTrentesimi--; else FlagTimerTrentesimi=1;}
	#asm
   ioi	ld a, (TBCSR)			; load B1, B2 interrupt flags (clears flag)
	ld		a, 00h
	ioi	ld (TBL1R), a			; set up next B1 match (at timer=0000h)
	ioi	ld (TBM1R), a			; NOTE:  you _need_ to reload the match register after every interrupt!
	ipres								; restore interrupts
	#endasm
}


