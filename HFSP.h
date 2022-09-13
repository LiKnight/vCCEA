#pragma once

#include<vector>
#include<iostream>
#include<fstream>
#include"Individual.h"
using namespace std;


//小规模  
int Jobs[4] = { 4, 5, 6, 7 };
int Stages[3] = { 2, 3, 4 };
int Types[5] = { 1,2,3,4,5 };

int Jobs1[5] = { 20, 40, 60, 80,100 };
int Stages1[4] = { 3, 5, 8,10 };


int pJob;
int pStage;
int pType;


vector<int> pLot;    //每个Job(Lot)的总批量               Jobs
vector<vector<int>> pSublot;  // 每个Lot在各阶段的分批   Jobs
vector<int> pMachines;   //每阶段的机器数                    Stages
vector<vector<int>> pUnitTime;  //每阶段的单位加工时间        Stages-Jobs

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
		for (int i = 0; i < pStage; i++)//第一阶段1台，其他阶段3台
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
		for (int i = 0; i < pStage; i++)//第二阶段1台，其他阶段3台
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
		for (int i = 0; i < pStage; i++)//第二阶段2台，其他阶段3台
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
		for (int i = 0; i < pStage; i++)//每个阶段都3台机器
		{
			pMachines[i] = 3;
		}
	}
	if (pType == 5)
	{
		for (int i = 0; i < pStage; i++)//每个阶段都3台机器
		{
			pMachines[i] = rand() % 5 + 1;
			cout << pMachines[i] << " ";
		}
		cout << endl;
	}

	//单位加工时间设置
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

	//每个批次的总批量设置
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
	int value[MaxSublotQuantity];//MaxSublotQuantity最大子批量
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


