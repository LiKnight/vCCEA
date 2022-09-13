#include<iostream>
#include"HEA.h"
#include"HFSP.h"
#include"Individual.h"
#include<time.h>
using namespace std;

//说明：五种邻域结构，精英选择+锦标赛选择+每次都扰动
void run(int k)
{
	char name[50] = "HEA";
	//每种规模生成5个实例，每个实例跑5次
	for (int g = 0; g < 20; g++)
	{
		HEA* HEA_1 = new HEA(10,15,0.3,150,name);
		HEA_1->run(k, g);
		delete HEA_1;
	}
}


void main()
{
	int NumberOfInstances = 0;
	for (int j = 0; j < 5; j++)
	{
		for (int i = 0; i < 4; i++)
		{

			for (int k = 4; k < 5; k++)
			{

				for (int noi = 0; noi < 5; noi++)
				{
					NumberOfInstances++;
					int InJob = Jobs1[j];
					int InStage = Stages1[i];
					int InType = Types[k];
					GenerateInstances(InJob, InStage, InType, NumberOfInstances + 1234);
					run(InType);
				}
			}
		}
	}



	cout << "................." << endl;
}