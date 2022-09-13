#pragma once

using namespace std;

class Algorithm
{
public:
	virtual void InitializePop() = 0;
	virtual void EvolvePop() = 0;
	virtual void OutputBest(int k, int rep, double time) = 0;
	virtual void run(int k, int rep) = 0;
public:
	char* name;
};