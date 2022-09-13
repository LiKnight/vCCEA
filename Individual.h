#pragma once
using namespace std;
//文件引用
#include<vector>
#include "HFSP.h"
#include <algorithm>
#include<time.h>
#include<Windows.h>
//类声明定义
class Individual
{
public:
	//构造函数
	Individual();
	void getdecoding();
	void getNeighbor(int NeighborhoodIndex, Individual& Neighbor);
	void getNeighborA(int NeighborhoodIndex, Individual& Neighbor,double pvnd);
	void getNeighborB(int NeighborhoodIndex, Individual& Neighbor,double pvnd);
	~Individual();
public:
	vector<int> pJobSeq;  //job
	vector<vector<int>> pJobSplit;  //job-sublot
	int makespan;
	int age;
	int BeatAge;
	int ptAge = 0;
	int col;
};
//构造函数
Individual::Individual()
{

	//Initialzie the pJobSeq
	pJobSeq.resize(pJob);//生成job序列
	for (int i = 0; i < pJob; i++)
	{
		pJobSeq[i] = i;
	}
	random_shuffle(pJobSeq.begin(), pJobSeq.end());//打乱顺序

	//Initialize the pJobSplit
	//产生 pSublotJobs
	//Step 1均匀划分
	pJobSplit.resize(pJob);
	for (int i = 0; i < pJob; i++)
	{
		pJobSplit[i].resize(MaxSublotQuantity);
	}
	for (int i = 0; i < pJob; i++)
	{
		for (int j = 0; j < MaxSublotQuantity; j++)
		{
			pJobSplit[i][j] = floor(pLot[i] / MaxSublotQuantity);//floor函数向下取整数
		}
	}

	vector<int> r;
	r.resize(pJob);
	for (int i = 0; i < pJob; i++)
	{
		int sum = 0;
		for (int j = 0; j < MaxSublotQuantity; j++)
		{
			sum += pJobSplit[i][j];
		}
		r[i] = pLot[i] - sum;
		int rIndex = rand() % MaxSublotQuantity;
		pJobSplit[i][rIndex] += r[i];
	}
	getdecoding();
	age = 0;

}
//解码
void Individual::getdecoding()
{

	vector<int> Seq = pJobSeq;
	vector<vector<vector<int>>> machineAssignment;
	machineAssignment.resize(pStage);
	for (int k = 0; k < pStage; k++)
	{
		machineAssignment[k].resize(pMachines[k]);
	}//二维数组，阶段以及每个阶段上的机器

	vector<vector<int>> mIdleTime;//每个阶段上机器的空闲时间
	mIdleTime.resize(pStage);
	for (int k = 0; k < pStage; k++)
	{
		mIdleTime[k].resize(pMachines[k]);
	}

	//all machines are availale at 0  其实可以不用要 默认都为0
	for (int k = 0; k < pStage; k++)
	{
		for (int i = 0; i < pMachines[k]; i++)
		{
			mIdleTime[k][i] = 0;
		}
	}//开始时机器都处于空闲状态

	//三维数组表示每个阶段上每个批次的每个子批的开始加工时间和结束加工时间
	vector<vector<vector<int>>> STime, CTime;
	STime.resize(pStage);
	for (int k = 0; k < pStage; k++)
	{
		STime[k].resize(pJob);
	}

	for (int k = 0; k < pStage; k++)
	{
		for (int j = 0; j < pJob; j++)
		{
			STime[k][j].resize(MaxSublotQuantity);
		}
	}
	CTime = STime;

	//Schedule the first stage第一阶段
	for (int j = 0; j < pJob; j++)
	{
		//Select a machine
		int mpt, minIdle = INT_MAX;

		//选出最先空闲机器
		for (int i = 0; i < pMachines[0]; i++)
		{
			if (mIdleTime[0][i] < minIdle)
			{
				minIdle = mIdleTime[0][i];
				mpt = i;
			}
		}

		machineAssignment[0][mpt].push_back(Seq[j]);

		//工件批量的调度
		for (int e = 0; e < MaxSublotQuantity; e++)
		{

			if (e == 0)
			{
				STime[0][Seq[j]][e] = mIdleTime[0][mpt];
				CTime[0][Seq[j]][e] = STime[0][Seq[j]][e] + pUnitTime[0][Seq[j]] * pJobSplit[Seq[j]][e];//加工完成时间=开始加工时间+每个单元加工时间*分批数
				mIdleTime[0][mpt] = CTime[0][Seq[j]][e];//空闲时间更新
			}
			else
			{
				STime[0][Seq[j]][e] = mIdleTime[0][mpt];
				CTime[0][Seq[j]][e] = STime[0][Seq[j]][e] + pUnitTime[0][Seq[j]] * pJobSplit[Seq[j]][e];
				mIdleTime[0][mpt] = CTime[0][Seq[j]][e];
			}

		}
	}

	//Schedule jobs in other stages as earlier as possible
	for (int k = 1; k < pStage; k++)
	{
		vector<Pair> ch;
		ch.resize(pJob);
		//Sorting jobs according to the completion time of the first sublot of jobs in previous stage
		for (int j = 0; j < pJob; j++)
		{
			ch[j].dim = j;
			for (int e = 0; e < MaxSublotQuantity; e++)
			{
				ch[j].value[e] = CTime[k - 1][j][e];  //子批优先
			}

		}
		sort(ch.begin(), ch.end(), PairLess());
		for (int j = 0; j < pJob; j++)
		{
			Seq[j] = ch[j].dim;
		}
		//schedule jobs
		for (int j = 0; j < pJob; j++)
		{
			//Select a machine
			int mpt, minIdle = INT_MAX;
			for (int i = 0; i < pMachines[k]; i++)
			{
				if (mIdleTime[k][i] < minIdle)
				{
					minIdle = mIdleTime[k][i];
					mpt = i;
				}
			}

			machineAssignment[k][mpt].push_back(Seq[j]);

			for (int e = 0; e < MaxSublotQuantity; e++)
			{
				if (e == 0)
				{
					CTime[k][Seq[j]][e] = max(mIdleTime[k][mpt], CTime[k - 1][Seq[j]][e]) + pUnitTime[k][Seq[j]] * pJobSplit[Seq[j]][e];
					STime[k][Seq[j]][e] = CTime[k][Seq[j]][e] - pUnitTime[k][Seq[j]] * pJobSplit[Seq[j]][e];
					mIdleTime[k][mpt] = CTime[k][Seq[j]][e];
				}
				else
				{
					STime[k][Seq[j]][e] = max(mIdleTime[k][mpt], CTime[k - 1][Seq[j]][e]);
					CTime[k][Seq[j]][e] = STime[k][Seq[j]][e] + pUnitTime[k][Seq[j]] * pJobSplit[Seq[j]][e];
					mIdleTime[k][mpt] = CTime[k][Seq[j]][e];
				}

			}

		}


	}


	makespan = 0;
	for (int i = 0; i < pJob; i++)
	{
		if (CTime[pStage - 1][i][MaxSublotQuantity - 1] > makespan)
		{
			makespan = CTime[pStage - 1][i][MaxSublotQuantity - 1];
		}
	}
}

Individual::~Individual()
{

}


void Individual::getNeighborA(int NeighborhoodIndex, Individual& Neighbor,double pvnd)
{
	if (NeighborhoodIndex == 1)
	{

		int a = rand() % pJob;
		int b = rand() % pJob;
		while (a == b)
		{
			a = rand() % pJob;
			b = rand() % pJob;
		}//随机选择两个工件位置

		if (b > a)
		{
			int tempValue = Neighbor.pJobSeq[b];
			for (int i = b; i > a; i--)
			{
				Neighbor.pJobSeq[i] = Neighbor.pJobSeq[i - 1];
			}
			Neighbor.pJobSeq[a] = tempValue;
		}//前插
		else
		{
			int tempValue = Neighbor.pJobSeq[b];
			for (int i = b; i < a; i++)
			{
				Neighbor.pJobSeq[i] = Neighbor.pJobSeq[i + 1];
			}
			Neighbor.pJobSeq[a] = tempValue;
		}//后插

		Neighbor.getdecoding();
	}
	else if (NeighborhoodIndex == 2)
	{
		int a = rand() % pJob;
		int b = rand() % pJob;
		while (a == b)
		{
			a = rand() % pJob;
			b = rand() % pJob;
		}
		swap(Neighbor.pJobSeq[a], Neighbor.pJobSeq[b]);
		Neighbor.getdecoding();
	}
	else if (NeighborhoodIndex == 3)
	{
		int cnt = pJob * pvnd;
		for (int i = 0; i < cnt; i++)
		{
			int a = rand() % pJob;
			int b = rand() % pJob;
			while (a == b)
			{
				a = rand() % pJob;
				b = rand() % pJob;
			}
			swap(Neighbor.pJobSeq[a], Neighbor.pJobSeq[b]);
		}
		Neighbor.getdecoding();
	}

}

void Individual::getNeighborB(int NeighborhoodIndex, Individual& Neighbor,double pvnd)
{
	if (NeighborhoodIndex == 1)
	{
		//任选一个批次
		int r = rand() % pJob;
		//任选这个批次的两个子批
		int a, b;
		do
		{
			a = rand() % MaxSublotQuantity;
			b = rand() % MaxSublotQuantity;
		} while (a == b);
		int temp1 = Neighbor.pJobSplit[r][a];
		int temp2 = Neighbor.pJobSplit[r][b];
		if (Neighbor.pJobSplit[r][a] > 0 || Neighbor.pJobSplit[r][b] > 0)
		{
			//定义一个步长
			int step = rand() % 3 + 1;
			int r2 = rand() * 1.0 / RAND_MAX;
			if (r2 < 0.5)
			{
				Neighbor.pJobSplit[r][a] += step;
				Neighbor.pJobSplit[r][b] -= step;
			}
			else
			{
				Neighbor.pJobSplit[r][a] -= step;
				Neighbor.pJobSplit[r][b] += step;
			}

			if (Neighbor.pJobSplit[r][0] == 0 || Neighbor.pJobSplit[r][a] < 0 || Neighbor.pJobSplit[r][b] < 0)
			{
				Neighbor.pJobSplit[r][a] = temp1;
				Neighbor.pJobSplit[r][b] = temp2;
			}

		}
		Neighbor.getdecoding();
	}
	else if (NeighborhoodIndex == 2)
	{
		int cnt =  pvnd* pJob;
		for (int y = 0; y < cnt; y++)
		{
			//任选一个批次
			int r = rand() % pJob;
			//任选这个批次的两个子批
			int a, b;
			do
			{
				a = rand() % MaxSublotQuantity;
				b = rand() % MaxSublotQuantity;
			} while (a == b);
			int temp1 = Neighbor.pJobSplit[r][a];
			int temp2 = Neighbor.pJobSplit[r][b];
			if (Neighbor.pJobSplit[r][a] > 0 || Neighbor.pJobSplit[r][b] > 0)
			{
				//定义一个步长
				int step = rand() % 3 + 1;
				int r2 = rand() * 1.0 / RAND_MAX;
				if (r2 < 0.5)
				{
					Neighbor.pJobSplit[r][a] += step;
					Neighbor.pJobSplit[r][b] -= step;
				}
				else
				{
					Neighbor.pJobSplit[r][a] -= step;
					Neighbor.pJobSplit[r][b] += step;
				}

				if (Neighbor.pJobSplit[r][0] == 0 || Neighbor.pJobSplit[r][a] < 0 || Neighbor.pJobSplit[r][b] < 0)
				{
					Neighbor.pJobSplit[r][a] = temp1;
					Neighbor.pJobSplit[r][b] = temp2;
				}

			}
		}
		Neighbor.getdecoding();
	}
	else if (NeighborhoodIndex == 3)
	{
	for (int k = 0; k < pJob; k++)
	{
		int RemainingSize = pLot[k];
		for (int j = 0; j < MaxSublotQuantity; j++)//分批
		{
			if (j == 0)
			{
				do
				{
					Neighbor.pJobSplit[k][j] = rand() % RemainingSize;
				} while (Neighbor.pJobSplit[k][j] == 0);

				RemainingSize -= Neighbor.pJobSplit[k][j];

			}
			if (j > 0 && j < MaxSublotQuantity - 1)
			{
				Neighbor.pJobSplit[k][j] = rand() % RemainingSize;
				RemainingSize -= Neighbor.pJobSplit[k][j];
			}
			if (j == MaxSublotQuantity - 1)
			{
				Neighbor.pJobSplit[k][j] = RemainingSize;
			}

		}
	}
	Neighbor.getdecoding();
	}
}