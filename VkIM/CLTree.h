#pragma once

#include "resource.h"
#include <hash_map>
#include <GdiPlus.h>

#include "ContactListCtrl.h"

// Helper Routine
extern void GradientFillRect( CDC *pDC, CRect &rect, COLORREF col_from, COLORREF col_to, bool vert_grad );


/*typedef struct			// Структура для хранения списка друзей
{
	unsigned int uid;			// Айдишнег
	wchar_t* userName;			// Имя
	wchar_t* userStatus;		// Статус
	wchar_t* userPhotoURL;		// Адрес авы
	HBITMAP userPhoto;			// Ава 50х50
	HBITMAP userPhoto30;		// Ава 30х30
	int rating;					// Рейтинг
	bool online;				// Онлайн или нет
	std::vector<int> groups;	// Списки в которых состоит
}ListEntry;*/

#define DM_NAMES_LIST 0
#define DM_NAMES_WITH_PHOTOS 1
#define DM_EXTENDED 2


class CLTree : public CTreeCtrl
{
	DECLARE_DYNAMIC(CLTree)

public:
	CLTree();
	virtual ~CLTree();

	
	virtual void  SetItemIcon( HICON icon ) { m_icon = icon; }; // Overridable
	virtual HICON GetItemIcon( HTREEITEM item ); // Overridable

	HTREEITEM _InsertItem(ListEntry en, HTREEITEM parent);

// Operations
	bool ItemIsVisible( HTREEITEM item );

	stdext::hash_map<HTREEITEM, ListEntry> items;
	stdext::hash_map<unsigned int, std::vector<HTREEITEM>> items_by_uid;
	std::vector<ListEntry> items_array;
	std::vector<int> uids;
	stdext::hash_map<wchar_t*, wchar_t*> lists;
	std::vector<wchar_t*> lists_ids;
	void Sort(HTREEITEM it);
	bool groupsMode;
	bool _groupsMode;
	bool updating;


private:
	COLORREF m_gradient_bkgd_from;		// Gradient variables
	COLORREF m_gradient_bkgd_to;
	COLORREF m_gradient_bkgd_sel;
	bool     m_gradient_horz;			// horz or vertical gradient

	CBitmap m_bmp_tree_closed;			// Tree open image (marker)
	CBitmap m_bmp_tree_open;			// Tree closed image (marker)

	CRect m_rect;						// The client rect when drawing
	int   m_h_offset;					// 0... -x (scroll offset)
	int   m_h_size;						// width of unclipped window
	int   m_v_offset;					// 0... -y (scroll offset)
	int   m_v_size;						// height of unclipped window

	HICON m_icon;

	void DrawBackGround( CDC* pDC );	// Background Paint code
	void DrawItems( CDC* pDC );  // Item Paint Code

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
public:
	void StartSearch(void);
	void UpdateSearch(wchar_t* q);
	void StopSearch(void);
	bool searching;
	int displayMode;
	void SetMode(int m);
	void SetGroupsMode(bool m);
};


