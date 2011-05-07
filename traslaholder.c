/*
 * positionholder.c
 *
 *  Created on: Apr 18, 2011
 *      Author: seba
 */

#use "traslaholder.lib"

void TaskDriverTraslaholder_Init()
{
 TaskDriverTraslaholder_PC=0;
}

void TaskDriverTraslaholder_Main()
{
	switch (TaskDriverTraslaholder_PC)
	{
		case 0: {TaskDriverTraslaholder_00(); break;}
		case 1: {TaskDriverTraslaholder_01(); break;}
		case 2: {TaskDriverTraslaholder_02(); break;}
		case 3: {TaskDriverTraslaholder_03(); break;}
	}
}


void TaskDriverTraslaholder_00()
{
#ifdef DRIVERTRASLAHOLDER_DEBUG
	myPrint("esecuzione");
#endif

 TaskDriverTraslaholder_PC=1;
}

void TaskDriverTraslaholder_01()
{
#ifdef DRIVERTRASLAHOLDER_DEBUG
	myPrint("esecuzione");
#endif

 TaskDriverTraslaholder_PC=2;
}

void TaskDriverTraslaholder_02()
{
#ifdef DRIVERTRASLAHOLDER_DEBUG
	myPrint("esecuzione");
#endif

 TaskDriverTraslaholder_PC=3;
}

void TaskDriverTraslaholder_03()
{
#ifdef DRIVERTRASLAHOLDER_DEBUG
	myPrint("esecuzione");
#endif

 TaskDriverTraslaholder_PC=0;
}

void myPrint(char * msg)
{
	printf("DRIVERTRASLAHOLDER(PC=%d): %s\n", TaskDriverTraslaholder_PC, msg);
}

