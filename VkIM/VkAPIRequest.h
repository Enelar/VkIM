#pragma once

// CVkAPIRequest command target

#include <vector>

class CVkAPIRequest : public CObject
{
public:
	CVkAPIRequest(wchar_t* method_name);
	virtual ~CVkAPIRequest();
	void AddParameter(wchar_t* name, wchar_t* value);
	char* GetRequestString();
	std::vector<wchar_t*> param_names;
	std::vector<wchar_t*> param_values;
private:
	
};


