#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "Individual.h"
#include"Algorithm.h"
#include <Windows.h>
#include"HFSP.h"
long StartTime = 0;
long EndTime = 0;

class HEA :public Algorithm
{
public:
	virtual void InitializePop();
	virtual void EvolvePop();
	virtual void OutputBest(int k, int rep, double time);
	virtual void run(int k, int rep);
public:
	HEA(int Popsize, int CvndIter, double PvndIter,int BupdateIter,char* name);
	~HEA();
public:
	bool UpdatePrimary(Individual& Ind);
	void UpdateBestSoFar(Individual& Ind);
	void CollaborativeOperator(Individual& Ind);
	void CollaborativeOperator1(Individual& Ind);
	void CollaborativeOperator2(Individual& Ind);
	void UpdateArchSet1(Individual& Ind, int pos);
	void UpdateArchSet2(Individual& Ind, int pos);
	void UpdatePt1(Individual& Ind, int pos,int col);
	void UpdatePt2(Individual& Ind, int pos,int col);
public:
	int Popsize;
	int CvndIter;
	double Pvnd;
	int BupdateIter;
public:
	int pt1Age = 0;
	int pt2Age = 0;
public:
	Individual Primary;
	Individual BestSoFar;
	vector<Individual>PopSet1;
	vector<Individual>PopSet;
	vector<Individual>pt1;
	vector<Individual>pt2;
};

HEA::HEA(int Popsize,int CvndIter, double PvndIter, int BupdateIter, char* name)
{
	this->Popsize = Popsize;
	this->CvndIter = CvndIter;
	this->Pvnd = PvndIter;
	this->BupdateIter = BupdateIter;
	this->name = name;
}

void HEA::InitializePop()
{

	PopSet.resize(Popsize);
	BestSoFar.makespan = INT_MAX;
	for (int i = 0; i < Popsize; i++)
	{
		UpdateBestSoFar(PopSet[i]);
	}
	pt1 = PopSet;
	pt2 = PopSet;
	for (int i = 0; i < Popsize; i++)
	{
		pt1[i].col = i;
		pt2[i].col = i;
	}
}


void HEA::EvolvePop()
{

	long Time = GetTickCount() - StartTime;
	long TimeLimit = (80 * pJob * pStage) - Time;
	long InitTime = GetTickCount();



	while (GetTickCount() - InitTime < TimeLimit)
	{
		for (int i = 0; i < Popsize; i++)
		{
			Individual ind;
			int RandNumber = rand() % Popsize;
			ind.pJobSeq = pt1[i].pJobSeq;
			ind.pJobSplit = PopSet[RandNumber].pJobSplit;
			ind.getdecoding();
			int Index = 1;
			while (Index <= 3)
			{
				int VndSearchCount = 0;
				while (VndSearchCount < CvndIter)
				{
					Primary = ind;
					ind.getNeighborA(Index, Primary,Pvnd);
					if (Primary.makespan < ind.makespan)
					{
						VndSearchCount = 0;
						Index = 1;
						ind = Primary;
						UpdateArchSet1(ind, RandNumber);
						UpdateBestSoFar(ind);
						UpdatePt1(ind, i,RandNumber);
					}
					else
					{
						VndSearchCount++;
					}
				}
				Index++;
			}
		}

		for (int i = 0; i < Popsize; i++)
		{
			Individual ind;
			int RandNumber = rand() % Popsize;
			ind.pJobSeq = PopSet[RandNumber].pJobSeq;
			ind.pJobSplit = pt2[i].pJobSplit;
			ind.getdecoding();
			int Index = 1;
			while (Index <= 3)
			{
				int VndSearchCount = 0;
				while (VndSearchCount < CvndIter)
				{
					Primary = ind;
					ind.getNeighborB(Index, Primary,Pvnd);
					if (Primary.makespan < ind.makespan)
					{
						VndSearchCount = 0;
						Index = 1;
						ind = Primary;
						UpdateArchSet2(ind, RandNumber);
						UpdateBestSoFar(ind);
						UpdatePt2(ind, i,RandNumber);
					}
					else
					{
						VndSearchCount++;
					}
				}
				Index++;
			}
		}

		for (int i = 0; i < Popsize; i++)
		{
			if (pt1[i].ptAge > BupdateIter)
			{
				pt1[i].ptAge = 0;
				CollaborativeOperator1(pt1[i]);
			}

			if (pt2[i].ptAge > BupdateIter)
			{
				pt2[i].ptAge = 0;
				CollaborativeOperator1(pt2[i]);
			}
		}
	}
}

void HEA::UpdatePt1(Individual& Ind, int pos,int col)
{
	if (Ind.makespan < pt1[pos].makespan)
	{
		pt1[pos].pJobSeq = Ind.pJobSeq;
		pt1[pos].pJobSplit = Ind.pJobSplit;
		pt1[pos].makespan = Ind.makespan;
		pt1[pos].ptAge = 0;
		pt1[pos].col = col;
	}
	else
	{
		pt1[pos].ptAge++;
	}
}

void HEA::UpdatePt2(Individual& Ind, int pos,int col)
{
	if (Ind.makespan < pt2[pos].makespan)
	{
		pt2[pos].pJobSeq = Ind.pJobSeq;
		pt2[pos].pJobSplit = Ind.pJobSplit;
		pt2[pos].makespan = Ind.makespan;		
		pt2[pos].ptAge = 0;
		pt2[pos].col = col;
	}
	else
	{
		pt2[pos].ptAge++;
	}
}

void HEA::UpdateArchSet1(Individual& Ind, int pos)
{
	if (Ind.makespan < PopSet[pos].makespan)
	{
		PopSet[pos].pJobSeq = Ind.pJobSeq;
		PopSet[pos].getdecoding();
		for (int i = 0; i < Popsize; i++)
		{
			if (pt2[i].col == pos)
			{
				pt2[i].pJobSeq = Ind.pJobSeq;
				pt2[i].getdecoding();
			}
		}
	}
}

void HEA::UpdateArchSet2(Individual& Ind, int pos)
{
	if (Ind.makespan < PopSet[pos].makespan)
	{
		PopSet[pos].pJobSplit = Ind.pJobSplit;
		PopSet[pos].getdecoding();
		for (int i = 0; i < Popsize; i++)
		{
			if (pt1[i].col == pos)
			{
				pt1[i].pJobSplit = Ind.pJobSplit;
				pt1[i].getdecoding();
			}
		}
	}
}

void HEA::CollaborativeOperator1(Individual& Ind)
{
	vector<Individual>ArchiveSet(PopSet);
	//select two different individuals from the set
	int pt1, pt2;
	do
	{
		pt1 = rand() % Popsize;
		pt2 = rand() % Popsize;
	} while (pt1 == pt2);//归档集中随机选择不同的两点


	for (int i = 0; i < pJob; i++)
	{
		Ind.pJobSeq[i] = -1;
	}


	int a, b, m;
	int temp;
	do
	{
		a = rand() % pJob;
		b = rand() % pJob;
	} while (a == b);

	if (a > b)   //assure that a<b
	{
		m = a;
		a = b;
		b = m;
	}//保证a小，b大

	if (a != 0)
	{
		temp = 0;
	}
	else
	{
		temp = b + 1;
	}

	//扰动
	for (int i = a; i <= b; i++)
	{
		Ind.pJobSeq[i] = ArchiveSet[pt1].pJobSeq[i];
	}
	for (int i = 0; i < pJob; i++)
	{
		vector<int>::iterator found = find(Ind.pJobSeq.begin(), Ind.pJobSeq.end(), ArchiveSet[pt2].pJobSeq[i]);
		if (found == Ind.pJobSeq.end())
		{
			Ind.pJobSeq[temp] = ArchiveSet[pt2].pJobSeq[i];
			temp++;

			if (temp == a)
			{
				temp = b + 1;
			}
		}
	}



	Ind.getdecoding();
}

void HEA::CollaborativeOperator2(Individual& Ind)
{
	vector<Individual>ArchiveSet(PopSet);
	//select two different individuals from the set
	
	int a, b;
	for (int i = 0; i < pJob; i++)
	{
		do
		{
			a = rand() % Popsize;
			b = rand() % Popsize;
		} while (a == b);
		if (PopSet[a].makespan < PopSet[b].makespan)
		{
			Ind.pJobSplit[i] = PopSet[a].pJobSplit[i];
		}
		else
		{
			Ind.pJobSplit[i] = PopSet[b].pJobSplit[i];
		}

	}

	/*if (rand() * 1.0 / RAND_MAX < 0.2)
	{
		//任选一个批次
		int r = rand() % pJob;

		//任选这个批次的两个子批
		do
		{
			a = rand() % MaxSublotQuantity;
			b = rand() % MaxSublotQuantity;
		} while (a == b);


		int temp1 = Ind.pJobSplit[r][a];
		int temp2 = Ind.pJobSplit[r][b];

		if (Ind.pJobSplit[r][a] > 0 || Ind.pJobSplit[r][b] > 0)
		{
			//定义一个步长
			int step = rand() % 3 + 1;
			int r2 = rand() * 1.0 / RAND_MAX;
			if (r2 < 0.5)
			{
				Ind.pJobSplit[r][a] += step;
				Ind.pJobSplit[r][b] -= step;
			}
			else
			{
				Ind.pJobSplit[r][a] += step;
				Ind.pJobSplit[r][b] -= step;
			}
			if (Ind.pJobSplit[r][0] == 0 || Ind.pJobSplit[r][a] < 0 || Ind.pJobSplit[r][b] < 0)
			{
				Ind.pJobSplit[r][a] = temp1;
				Ind.pJobSplit[r][b] = temp2;
			}
		}
	}*/

	

	Ind.getdecoding();


}

bool HEA::UpdatePrimary(Individual& Ind)
{
	if (Ind.makespan < Primary.makespan)//如果当前方案比主解决方案好，就更新
	{
		Primary = Ind;
		return true;
	}
	else
	{
		return false;
	}

}

void HEA::UpdateBestSoFar(Individual& Ind)
{
	if (BestSoFar.makespan > Ind.makespan)
	{
		BestSoFar = Ind;
		BestSoFar.BeatAge = 0;
	}
	else
	{
		BestSoFar.BeatAge++;
	}
}

void HEA::OutputBest(int k, int rep, double time)
{
	cout << pJob << "_" << pStage << "_" << k << "_" << rep << ":" << BestSoFar.makespan << "   时间：" << time << endl;
	ofstream output;
	output.open("number.txt", ios::app);
	output << BestSoFar.makespan << " ";
	if ((rep + 1) % 10 == 0)
		output << '\n';
	output.close();
}

void HEA::run(int k, int rep)
{
	StartTime = GetTickCount();
	InitializePop();
	EvolvePop();
	EndTime = GetTickCount();
	double UsedTime = (EndTime - StartTime) * 1.0 / 1000;
	OutputBest(k, rep, UsedTime);
}

HEA::~HEA()
{

}