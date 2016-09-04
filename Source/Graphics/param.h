#pragma once

#ifdef _WIN32
#pragma warning(disable:4786)
#endif

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;


///  base class for named parameter
class ParamBase
{
protected:
	string name;
public:
	ParamBase(char* name1) {  name = name1;  }
	virtual ~ParamBase() {  }
	string* GetName() {  return &name;  }

	virtual string GetValueString() = 0;
	
	virtual void Reset() = 0;
	virtual void Increment(float t) = 0;	virtual void Decrement(float t) = 0;

	virtual float GetPercentage() = 0;	virtual void SetPercentage(float p) = 0;
	virtual void Write(ostream &stream) = 0;	virtual void Read(istream &stream) = 0;
	virtual bool IsList() = 0;
	static bool bChangedAny;
	static void Changed() {  bChangedAny = true;  }
};

typedef  vector<ParamBase*>::const_iterator  ParamIter;


///  single-valued parameter

template<class T> class Param : public ParamBase
{
private:
	T value, *ptr;	 // pointer to value declared elsewhere
	T default, min,max, width, step;
	float fpow,fpow1;  //log

public:
	Param(char *name, T min1,T max1,T step1, T* ptr1, float pow1 = 1.f, T val1 = -77) : ParamBase(name)
	{
		ptr = ptr1;  if (val1 != -77) {  value = default = val1;  *ptr = val1;  }  else  {  value = default = *ptr;  }
		min = min1;  max = max1;  width = max-min;
		step = step1;  fpow = pow1;  fpow1 = 1.f/pow1;
	}
	virtual ~Param() {  }

	T GetValue() const			{  return *ptr;  }
	T SetValue(const T value)	{  *ptr = value;  }

	string GetValueString()
	{	ostringstream ost;	int p = 4;
		if (abs(*ptr) < 0.001f)	 p = 2;
		if (abs(*ptr) > 9000.f)  p = 6;
		ost << setprecision(p) << *ptr;  return ost.str();	}

	float GetPercentage() {			float v = (*ptr - min) / (float)width;  if (v>1.f) v=1.f;
									if (fpow==1.f)  return v;  else  return powf(v, fpow1);  }
	void SetPercentage(float p)	{	if (fpow!=1.f)  p = powf(p, fpow);
									*ptr = (T)(min + p * width);	}

	void Increment(float t) {  *ptr += step * t;	if (*ptr > max)  *ptr = max;	}
	void Decrement(float t) {  *ptr -= step * t;	if (*ptr < min)  *ptr = min;	}

	void Reset() { *ptr = default; }
	bool IsList() { return false; }

	void Write(ostream &stream) { stream << name << " " << *ptr << '\n'; }
	void Read(istream &stream)  { stream >> name >> *ptr; }
};


extern const Param<int> dummy;


///  list of parameters

class ParamList : public ParamBase
{
public:
	ParamList(char *name = "") : ParamBase(name) {	active = true;	}
	virtual ~ParamList() { }

	
	void AddEmpty()  //  empty spacer
	{	params.push_back(NULL);	 }

	void AddParam(ParamBase* param)
	{
		params.push_back(param);
		map[*param->GetName()] = param;
		cur = params.begin();
	}

	ParamBase* GetParam(char *name)  // based on name
	{
		ParamBase*p = map[name];
		if (p)	return p;
		else	return (ParamBase*) &dummy;
	}

	ParamBase* GetParam(int i){	return params[i];		}
	ParamBase* GetCurrent()	{	return *cur;	}
	int GetSize() {  return (int)params.size();  }
	bool IsList() {  return true;  }
	void Reset() {	cur = params.begin();	}

	float GetPercentage() { return 0.0f; }
	void SetPercentage(float /*p*/) {  }
	string GetValueString()	{	/*return name;*/	return "list";	}

	
	void Increment(float t)
	{	do	{
			cur++;	if (cur == params.end())  cur = params.begin();
		}  while (!*cur);
	}

	void Decrement(float t)
	{	do  {
			if (cur == params.begin())
				cur = params.end()-1;	else  cur--;
		}  while (!*cur);
	}

	void Write(ostream &stream)	{	stream << name << '\n';
		for(ParamIter p = params.begin(); p != params.end(); ++p)	(*p)->Write(stream);	}

	void Read(istream &stream)	{	stream >> name;
		for(ParamIter p = params.begin(); p != params.end(); ++p)	(*p)->Read(stream);		}

protected:
	bool active;
	vector<ParamBase*> params;
	map<string, ParamBase*> map;
	ParamIter cur;
};
