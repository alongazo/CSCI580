// Application4.h: interface for the Application4 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPLICATION5_H__43A7FA9C_6CD6_4A79_9567_2354BFEFAFFB__INCLUDED_)
#define AFX_APPLICATION5_H__43A7FA9C_6CD6_4A79_9567_2354BFEFAFFB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Application.h"
#include "vec.h"

class Application5 : public Application  
{
public:
	static constexpr int AAKERNEL_SIZE = 6;
	static const Vec3 AAFilter[AAKERNEL_SIZE];

	Application5();
	virtual ~Application5();
	
	int	Initialize();
	virtual int Render(); 
	int Clean();

	GzPixel *m_pAABuffer;

private:
	inline int ARRAY(int x, int y) { return (x + y*m_nWidth); }	/* simplify fbuf indexing */
};

#endif // !defined(AFX_APPLICATION5_H__43A7FA9C_6CD6_4A79_9567_2354BFEFAFFB__INCLUDED_)
