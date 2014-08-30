// EnBitmap.cpp: implementation of the CEnBitmap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EnBitmap.h"
#include "Utils.h"

#include <AFXPRIV.H>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const int HIMETRIC_INCH	= 2540;

enum 
{
	FT_BMP,
	FT_ICO,
	FT_JPG,
	FT_GIF,
	FT_PNG,

	FT_UNKNOWN
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEnBitmap::CEnBitmap()
{
	useAlpha=true;
}

CEnBitmap::~CEnBitmap()
{

}

BOOL CEnBitmap::LoadImage(UINT uIDRes, LPCTSTR szResourceType, HMODULE hInst, COLORREF crBack)
{
	ASSERT(m_hObject == NULL);      // only attach once, detach on destroy

	if (m_hObject != NULL)
		return FALSE;

	return Attach(LoadImageResource(uIDRes, szResourceType, hInst, crBack));
}

BOOL CEnBitmap::LoadImage(LPCTSTR szImagePath, COLORREF crBack)
{
	ASSERT(m_hObject == NULL);      // only attach once, detach on destroy

	if (m_hObject != NULL)
		return FALSE;

	return Attach(LoadImageFile(szImagePath, crBack));
}

HBITMAP CEnBitmap::LoadImageFile(LPCTSTR szImagePath, COLORREF crBack)
{
	int nType = GetFileType(szImagePath);
	//TRACE("Load(%ws)\n", szImagePath);
	switch (nType)
	{
		// i suspect it is more efficient to load
		// bmps this way since it avoids creating device contexts etc that the 
		// IPicture methods requires. that method however is still valuable
		// since it handles other image types and transparency
		case FT_BMP:
			{
				HBITMAP bmp=(HBITMAP)::LoadImage(NULL, szImagePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
				BITMAP bi;
				::GetObject(bmp, sizeof( BITMAP ), &bi);

				BYTE bpp = bi.bmBitsPixel >> 3;
				if(bpp==4){
				BYTE* pBits = new BYTE[ bi.bmWidth * bi.bmHeight * bpp ];
				int  p = ::GetBitmapBits( bmp, bi.bmWidth * bi.bmHeight * bpp, pBits);
				for (int y=0; y<bi.bmHeight; ++y)
				{
					BYTE *pPixel= (BYTE *) pBits + bi.bmWidth * 4 * y;
						for (int x=0; x<bi.bmWidth ; ++x)
						{
							pPixel[0]= pPixel[0]*pPixel[3]/255;
							pPixel[1]= pPixel[1]*pPixel[3]/255;
							pPixel[2]= pPixel[2]*pPixel[3]/255;
							pPixel+= 4;
						}
				}
				::SetBitmapBits(bmp, bi.bmWidth*bi.bmHeight*bpp, pBits);
				delete pBits;
				}
				return bmp;
			}

		case FT_UNKNOWN:
			return NULL;

		default: // all the rest
		{
			USES_CONVERSION;
			IPicture* pPicture = NULL;
			
			HBITMAP hbm = NULL;
			HRESULT hr = OleLoadPicturePath(T2OLE((LPTSTR)szImagePath), NULL, 0, crBack, IID_IPicture, (LPVOID *)&pPicture);
					
			if (pPicture)
			{
				hbm = ExtractBitmap(pPicture, crBack);
				pPicture->Release();
			}

			return hbm;
		}
	}

	return NULL; // can't get here
}

HBITMAP CEnBitmap::LoadImageResource(UINT uIDRes, LPCTSTR szResourceType, HMODULE hInst, COLORREF crBack)
{
	BYTE* pBuff = NULL;
	int nSize = 0;
	HBITMAP hbm = NULL;

	// first call is to get buffer size
	/*if (GetResource(MAKEINTRESOURCE(uIDRes), szResourceType, hInst, 0, nSize))
	{
		if (nSize > 0)
		{
			pBuff = new BYTE[nSize];
			
			// this loads it
			if (GetResource(MAKEINTRESOURCE(uIDRes), szResourceType, hInst, pBuff, nSize))
			{
				IPicture* pPicture = LoadFromBuffer(pBuff, nSize);

				if (pPicture)
				{
					hbm = ExtractBitmap(pPicture, crBack);
					pPicture->Release();
				}
			}
			
			delete [] pBuff;
		}
	}*/
	hbm=::LoadBitmapW(hInst, MAKEINTRESOURCE(uIDRes));

	BITMAP bi;
				::GetObject(hbm, sizeof( BITMAP ), &bi);

				BYTE bpp = bi.bmBitsPixel >> 3;
				if(bpp==4){
				BYTE* pBits = new BYTE[ bi.bmWidth * bi.bmHeight * bpp ];
				int  p = ::GetBitmapBits( hbm, bi.bmWidth * bi.bmHeight * bpp, pBits);
				for (int y=0; y<bi.bmHeight; ++y)
				{
					BYTE *pPixel= (BYTE *) pBits + bi.bmWidth * 4 * y;
						for (int x=0; x<bi.bmWidth ; ++x)
						{
							pPixel[0]= pPixel[0]*pPixel[3]/255;
							pPixel[1]= pPixel[1]*pPixel[3]/255;
							pPixel[2]= pPixel[2]*pPixel[3]/255;
							pPixel+= 4;
						}
				}
				::SetBitmapBits(hbm, bi.bmWidth*bi.bmHeight*bpp, pBits);
				delete pBits;
				}
	return hbm;
}

IPicture* CEnBitmap::LoadFromBuffer(BYTE* pBuff, int nSize)
{
	bool bResult = false;

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, nSize);
	void* pData = GlobalLock(hGlobal);
	memcpy(pData, pBuff, nSize);
	GlobalUnlock(hGlobal);

	IStream* pStream = NULL;
	IPicture* pPicture = NULL;

	if (CreateStreamOnHGlobal(hGlobal, TRUE, &pStream) == S_OK)
	{
		HRESULT hr = OleLoadPicture(pStream, nSize, FALSE, IID_IPicture, (LPVOID *)&pPicture);
		pStream->Release();
	}

	return pPicture; // caller releases
}

BOOL CEnBitmap::GetResource(LPCTSTR lpName, LPCTSTR lpType, HMODULE hInst, void* pResource, int& nBufSize)
{ 
	HRSRC		hResInfo;
	HANDLE		hRes;
	LPSTR		lpRes	= NULL; 
	int			nLen	= 0;
	bool		bResult	= FALSE;

	// Find the resource
	hResInfo = FindResource(hInst, lpName, lpType);

	if (hResInfo == NULL) 
		return false;

	// Load the resource
	hRes = LoadResource(hInst, hResInfo);

	if (hRes == NULL) 
		return false;

	// Lock the resource
	lpRes = (char*)LockResource(hRes);

	if (lpRes != NULL)
	{ 
		if (pResource == NULL)
		{
			nBufSize = SizeofResource(hInst, hResInfo);
			bResult = true;
		}
		else
		{
			if (nBufSize >= (int)SizeofResource(hInst, hResInfo))
			{
				memcpy(pResource, lpRes, nBufSize);
				bResult = true;
			}
		} 

		UnlockResource(hRes);  
	}

	// Free the resource
	FreeResource(hRes);

	return bResult;
}

HBITMAP CEnBitmap::ExtractBitmap(IPicture* pPicture, COLORREF crBack)
{
	ASSERT(pPicture);

	if (!pPicture)
		return NULL;

	CBitmap bmMem;
	CDC dcMem;
	CDC* pDC = CWnd::GetDesktopWindow()->GetDC();

	if (dcMem.CreateCompatibleDC(pDC))
	{
		long hmWidth;
		long hmHeight;
		
		pPicture->get_Width(&hmWidth);
		pPicture->get_Height(&hmHeight);
		
		int nWidth	= MulDiv(hmWidth, pDC->GetDeviceCaps(LOGPIXELSX), HIMETRIC_INCH);
		int nHeight	= MulDiv(hmHeight, pDC->GetDeviceCaps(LOGPIXELSY), HIMETRIC_INCH);

		if (bmMem.CreateCompatibleBitmap(pDC, nWidth, nHeight))
		{
			CBitmap* pOldBM = dcMem.SelectObject(&bmMem);

			if (crBack != -1)
				dcMem.FillSolidRect(0, 0, nWidth, nHeight, crBack);
			HRESULT hr = pPicture->Render(dcMem, 0, 0, nWidth, nHeight, 0, hmHeight, hmWidth, -hmHeight, NULL);
			dcMem.SelectObject(pOldBM);
		}
	}

	CWnd::GetDesktopWindow()->ReleaseDC(pDC);

	return (HBITMAP)bmMem.Detach();
}

int CEnBitmap::GetFileType(LPCTSTR szImagePath)
{
	CString sPath(szImagePath);
	sPath.MakeUpper();

	if (sPath.Find(L".BMP") > 0)
		return FT_BMP;

	else if (sPath.Find(L".ICO") > 0)
		return FT_ICO;

	else if (sPath.Find(L".JPG") > 0 || sPath.Find(L".JPEG") > 0)
		return FT_JPG;

	else if (sPath.Find(L".GIF") > 0)
		return FT_GIF;

	else if (sPath.Find(L".PNG") > 0)
		return FT_PNG;

	// else
	return FT_UNKNOWN;
}

void CEnBitmap::DrawUnscaled(CDC* dc, int x, int y, int w, int h, int sx, int sy){
	CDC mdc;
	mdc.CreateCompatibleDC(0);
	mdc.SelectObject(this);
	BLENDFUNCTION func;
	func.BlendOp=AC_SRC_OVER;
	func.AlphaFormat=AC_SRC_ALPHA;
	func.SourceConstantAlpha=255;
	func.BlendFlags=0;
	dc->AlphaBlend(x, y, w, h, &mdc, sx, sy, w, h, func);
	//dc->BitBlt(x, y, w, h, &mdc, sx, sy, SRCCOPY);
	//ReleaseDC(NULL, mdc);
	mdc.DeleteDC();
}

void CEnBitmap::DrawStretched(CDC* dc, int x, int y, int w, int h, int sx, int sy, int sw, int sh){
	CDC mdc;
	//CDC* ddc=CWnd::GetDesktopWindow()->GetDC();
	mdc.CreateCompatibleDC(0);
	CBitmap* ob=mdc.SelectObject(this);
	BLENDFUNCTION func;
	func.BlendOp=AC_SRC_OVER;
	func.AlphaFormat=AC_SRC_ALPHA;
	func.SourceConstantAlpha=255;
	func.BlendFlags=0;
	if(scaleType==0){
		dc->AlphaBlend(x, y, w, h, &mdc, sx, sy, sw, sh, func);
	}else{
		int s9t=scale9.top;
		int s9b=scale9.bottom;
		int s9l=scale9.left;
		int s9r=scale9.right;

		int iw=rcImg.right;
		int ih=rcImg.bottom;

		if(useAlpha){
		// Углы
		dc->AlphaBlend(x, y, s9l, s9t, &mdc, 0, 0, s9l, s9t, func);
		dc->AlphaBlend(x+w-s9r, y, s9r, s9t, &mdc, iw-s9r, 0, s9r, s9t, func);
		dc->AlphaBlend(x, y+h-s9b, s9l, s9b, &mdc, 0, ih-s9b, s9l, s9b, func);
		dc->AlphaBlend(x+w-s9r, y+h-s9b, s9r, s9b, &mdc, iw-s9r, ih-s9b, s9r, s9b, func);
	
		//Вертикальные стороны
		dc->AlphaBlend(x, y+s9t, s9l, h-s9t-s9b, &mdc, 0, s9t, s9l, ih-s9t-s9b, func);
		dc->AlphaBlend(x+w-s9r, y+s9t, s9r, h-s9t-s9b, &mdc, iw-s9r, s9t, s9r, ih-s9t-s9b, func);
		
		//Горизонтальные стороны
		dc->AlphaBlend(x+s9l, y, w-s9l-s9r, s9t, &mdc, s9l, 0, iw-s9l-s9r, s9t, func);
		dc->AlphaBlend(x+s9l, y+h-s9b, w-s9l-s9r, s9b, &mdc, s9l, ih-s9b, iw-s9l-s9r, s9b, func);
	
		//И середина
		dc->AlphaBlend(x+s9l, y+s9t, w-s9l-s9r, h-s9t-s9b, &mdc, s9l, s9t, iw-s9l-s9r, ih-s9t-s9b, func);
		}
		else
		{
		// Углы
		dc->BitBlt(x, y, s9l, s9t, &mdc, 0, 0, SRCCOPY);
		dc->BitBlt(x+w-s9r, y, s9r, s9t, &mdc, iw-s9r, 0, SRCCOPY);
		dc->BitBlt(x, y+h-s9b, s9l, s9b, &mdc, 0, iw-s9b, SRCCOPY);
		dc->BitBlt(x+w-s9r, y+h-s9b, s9r, s9b, &mdc, iw-s9r, ih-s9b, SRCCOPY);
	
		//Вертикальные стороны
		dc->StretchBlt(x, y+s9t, s9l, h-s9t-s9b, &mdc, 0, s9t, s9l, ih-s9t-s9b, SRCCOPY);
		dc->StretchBlt(x+w-s9r, y+s9t, s9r, h-s9t-s9b, &mdc, iw-s9r, s9t, s9r, ih-s9t-s9b, SRCCOPY);
		
		//Горизонтальные стороны
		dc->StretchBlt(x+s9l, y, w-s9l-s9r, s9t, &mdc, s9l, 0, iw-s9l-s9r, s9t, SRCCOPY);
		dc->StretchBlt(x+s9l, y+h-s9b, w-s9l-s9r, s9b, &mdc, s9l, ih-s9b, iw-s9l-s9r, s9b, SRCCOPY);
	
		//И середина
		dc->StretchBlt(x+s9l, y+s9t, w-s9l-s9r, h-s9t-s9b, &mdc, s9l, s9t, iw-s9l-s9r, ih-s9t-s9b, SRCCOPY);
		}
	}
	mdc.SelectObject(ob);
	//ReleaseDC(NULL, mdc);
	mdc.DeleteDC();
	//CWnd::GetDesktopWindow()->ReleaseDC(ddc);
}
