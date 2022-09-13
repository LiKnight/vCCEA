#pragma once

#include<vector>
#include<iostream>
#include<fstream>
#include"Individual.h"
using namespace std;


//С��ģ  
int Jobs[4] = { 4, 5, 6, 7 };
int Stages[3] = { 2, 3, 4 };
int Types[5] = { 1,2,3,4,5 };

int Jobs1[5] = { 20, 40, 60, 80,100 };
int Stages1[4] = { 3, 5, 8,10 };


int pJob;
int pStage;
int pType;


vector<int> pLot;    //ÿ��Job(Lot)��������               Jobs
vector<vector<int>> pSublot;  // ÿ��Lot�ڸ��׶εķ���   Jobs
vector<int> pMachines;   //ÿ�׶εĻ�����                    Stages
vector<vector<int>> pUnitTime;  //ÿ�׶εĵ�λ�ӹ�ʱ��        Stages-Jobs

const int MaxSublotQuantity = 5;

void GenerateInstances(int InJob, int InStage, int InType, int Seed)
{
	srand(Seed);

	pJob = InJob;
	pStage = InStage;
	pType = InType;


	pMachines.resize(pStage);

	if (pType == 1)
	{
		for (int i = 0; i < pStage; i++)//��һ�׶�1̨�������׶�3̨
		{
			if (i == 0)
			{
				pMachines[i] = 1;
			}
			else
			{
				pMachines[i] = 3;
			}
		}
	}
	if (pType == 2)
	{
		for (int i = 0; i < pStage; i++)//�ڶ��׶�1̨�������׶�3̨
		{
			if (i == 1)
			{
				pMachines[i] = 1;
			}
			else
			{
				pMachines[i] = 3;
			}
		}
	}
	if (pType == 3)
	{
		for (int i = 0; i < pStage; i++)//�ڶ��׶�2̨�������׶�3̨
		{
			if (i == 1)
			{
				pMachines[i] = 2;
			}
			else
			{
				pMachines[i] = 3;
			}
		}
	}
	if (pType == 4)
	{
		for (int i = 0; i < pStage; i++)//ÿ���׶ζ�3̨����
		{
			pMachines[i] = 3;
		}
	}
	if (pType == 5)
	{
		for (int i = 0; i < pStage; i++)//ÿ���׶ζ�3̨����
		{
			pMachines[i] = rand() % 5 + 1;
			cout << pMachines[i] << " ";
		}
		cout << endl;
	}

	//��λ�ӹ�ʱ������
	pUnitTime.resize(InStage);
	for (int i = 0; i < pStage; i++)
	{
		pUnitTime[i].resize(pJob);
	}
	for (int i = 0; i < pStage; i++)
	{
		for (int j = 0; j < pJob; j++)
		{
			pUnitTime[i][j] = 1 + rand() % 10;
		}
	}

	//ÿ�����ε�����������
	pLot.resize(pJob);
	for (int j = 0; j < pJob; j++)
	{
		pLot[j] = 50 + rand() % 51;
	}
}



class Pair
{
public:
	int dim;
	int value[MaxSublotQuantity];//MaxSublotQuantity���������
};




class PairLess
{
public:
	bool operator() (Pair& a, Pair& b)
	{
		bool result = false;
		int i = 0;
		do
		{
			if (a.value[i] < b.value[i])
			{
				result = true;
			}
			else if (a.value[i] > b.value[i])
			{
				result = false;
			}
			i++;
		} while (a.value[i - 1] == b.value[i - 1] && i < MaxSublotQuantity);
		return result;

	}
};
class PairLess2
{
public:
	bool operator() (Pair& a, Pair& b)
	{

		if (a.value[0] < b.value[0])
		{
			return true;
		}
		else if (a.value[1] == b.value[1])
		{
			if (a.value[1] < b.value[1])
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}

	}
};


