/* SinData.c */
#include "SinData.h"
#define PI 3.1415926
#define BUFFER_SIZE 1024

int g_Buffer[BUFFER_SIZE];
int g_BufferIndex;
int g_AxisX;
int g_AxisY;
int g_AxisIndex;
int g_Amplitude = 25;
int g_Cycle = 6;
int g_SinOffset = 135;

/*
Function Name   :    SetAmplitude
Description :        �������
Input Parameters:    Amplitude :���ֵ
*/
void SetAmplitude(int Amplitude)
{
	g_Amplitude = Amplitude;
}

/*
Function Name   :    SetCycle
Description :        ��������
Input Parameters:    Cycle :����
*/
void SetCycle(int Cycle)
{
	g_Cycle = Cycle;
}

/*
Function Name   :    SetSinOffset
Description :        ����ƫ����
Input Parameters:    SinOffset :y���ƫ����
*/
void SetSinOffset(int SinOffset)
{
	g_SinOffset = SinOffset;
}


/*
Function Name:   RecvData
Description  :   ��������������������������ݣ�����Sinͼ��
*/
void UpdateSinData(int AxisX[], int AxisY[])
{

	int m;
	for (m = 0; m <g_AxisIndex; m++)
	{
		AxisX[m] = m;
		AxisY[m] = g_Amplitude * sin(PI / g_Cycle * (AxisX[m])) + g_SinOffset;
	}
}


/*
Function Name:   void DrawSin
Description  :   ����sinͼ�������
*/
void DrawSin()
{
	g_AxisY = g_Amplitude * sin(PI / g_Cycle *  g_AxisIndex) + g_SinOffset;
	g_AxisX = g_AxisIndex;
	g_AxisIndex++;
}


/*
Function Name   :    GetSinData
Description     :    ��ȡsinͼ������꣬���ⲿ����
Input Parameters��   AxisX ��x������   AxisY��y������
Return Value    ��   1 ���ɹ���ȡ
0 ����ȡʧ��(�������ݵĻ�����û��)
-1����ȡʧ��
*/
int GetSinData(int *AxisX, int *AxisY)
{
	if (g_BufferIndex == BUFFER_SIZE)
	{
		DrawSin();
		*AxisX = g_AxisX;
		*AxisY = g_AxisY;
		static int s_Tick = 0;
		s_Tick++;
		g_BufferIndex = 0;
		return 1;
	}

	return 0;
}

/*
Function Name:   RecvData
Description  :   ģ�½���1K������
*/
void RecvData()
{
	while (1)
	{
		if (g_BufferIndex == BUFFER_SIZE)
			return;
		g_Buffer[g_BufferIndex] = g_BufferIndex;
		g_BufferIndex++;

	}
}

void *RecvDataPthread(void *arg)
{
	while (1)
	{
		RecvData();
	}
}

/*
Function Name   :    StartSin
Description     :    �����̣߳���ͣ�ػ�ȡ����
*/

void StartSin()
{
	pthread_t pid;
	pthread_create(&pid, NULL, RecvDataPthread, NULL);
}