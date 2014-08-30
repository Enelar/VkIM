#pragma once
#include "stackwalker.h"
#include <stdio.h>
#include <string>
class MyStackWalker :
	public StackWalker
{
public:
	MyStackWalker(void);
	~MyStackWalker(void);
	std::string buffer;
	virtual void OnOutput(LPCSTR szText)
    { buffer+=szText+std::string("\r\n"); StackWalker::OnOutput(szText); }
};

