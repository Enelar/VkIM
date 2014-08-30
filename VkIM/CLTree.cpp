#include "stdafx.h"
#include "CLTree.h"
#include "Skin.h"

#include "Skin.h"
#include "Lang.h"
#include <GdiPlus.h>

#define _OWNER_DRAWN 1  // Set to 0 to use Windows draw code.  
extern CSkin skin;

// Gradient Filling Helper Routine
void GradientFillRect( CDC *pDC, CRect &rect, COLORREF col_from, COLORREF col_to, bool vert_grad )
{
	TRIVERTEX        vert[2];
	GRADIENT_RECT    mesh;

	vert[0].x      = rect.left;
	vert[0].y      = rect.top;
	vert[0].Alpha  = 0x0000;
	vert[0].Blue   = GetBValue(col_from) << 8;
	vert[0].Green  = GetGValue(col_from) << 8;
	vert[0].Red    = GetRValue(col_from) << 8;

	vert[1].x      = rect.right;
	vert[1].y      = rect.bottom; 
	vert[1].Alpha  = 0x0000;
	vert[1].Blue   = GetBValue(col_to) << 8;
	vert[1].Green  = GetGValue(col_to) << 8;
	vert[1].Red    = GetRValue(col_to) << 8;

	mesh.UpperLeft  = 0;
	mesh.LowerRight = 1;
#if _MSC_VER >= 1300  // only VS7 and above has GradientFill as a pDC member
	pDC->GradientFill( vert, 2, &mesh, 1, vert_grad ? GRADIENT_FILL_RECT_V : 
		GRADIENT_FILL_RECT_H );
#else
	GradientFill( pDC->m_hDC, vert, 2, &mesh, 1, vert_grad ? GRADIENT_FILL_RECT_V : 
		GRADIENT_FILL_RECT_H );
#endif
}

// CTreeCtrlEx

IMPLEMENT_DYNAMIC(CLTree, CTreeCtrl)

CLTree::CLTree()
{

    VERIFY( m_bmp_tree_closed.LoadBitmap( IDB_TREE_CLOSED ) ) ; 
    m_bmp_tree_closed.GetSafeHandle();
    VERIFY( m_bmp_tree_open.LoadBitmap( IDB_TREE_OPENED ) ) ; 
    m_bmp_tree_open.GetSafeHandle();
	m_icon = NULL;
	groupsMode=true;
	searching=false;
}

CLTree::~CLTree()
{
    if (m_bmp_tree_closed.GetSafeHandle())
        m_bmp_tree_closed.DeleteObject();
    if (m_bmp_tree_open.GetSafeHandle())
        m_bmp_tree_open.DeleteObject();

	for(int i=0;i<items_array.size();i++){
		//if(items_array[i].userName)delete items_array[i].userName;
		//if(items_array[i].userPhotoURL)delete items_array[i].userPhotoURL;
		if(items_array[i].userPhoto)DeleteObject(items_array[i].userPhoto);
		if(items_array[i].userPhoto30)DeleteObject(items_array[i].userPhoto30);
	}
}


BEGIN_MESSAGE_MAP(CLTree, CTreeCtrl)
	ON_WM_ERASEBKGND()
#if _OWNER_DRAWN
	ON_WM_PAINT()
#endif
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnTvnItemexpanding)
//	ON_WM_CREATE()
END_MESSAGE_MAP()



// CCLTree message handlers


BOOL CLTree::OnEraseBkgnd(CDC* pDC)
{
    // nothing to do here -- see OnPaint
	return TRUE;
}

#if _OWNER_DRAWN
void CLTree::OnPaint()
{
	CPaintDC dc(this);	// Device context for painting
	CDC dc_ff;			// Memory base device context for flicker free painting
	CBitmap bm_ff;		// The bitmap we paint into
	CBitmap *bm_old;
	CFont *font, *old_font;
	CFont fontDC;
	int old_mode;

	GetClientRect(&m_rect);
	SCROLLINFO scroll_info;
	// Determine window portal to draw into taking into account
	// scrolling position
	if ( GetScrollInfo( SB_HORZ, &scroll_info, SIF_POS | SIF_RANGE ) )
	{
		m_h_offset = -scroll_info.nPos;
		m_h_size = max( scroll_info.nMax+1, m_rect.Width());
	}
	else
	{
		m_h_offset = m_rect.left;
		m_h_size = m_rect.Width();
	}
	if ( GetScrollInfo( SB_VERT, &scroll_info, SIF_POS | SIF_RANGE ) )
	{
		if ( scroll_info.nMin == 0 && scroll_info.nMax == 100) 
			scroll_info.nMax = 0;
		m_v_offset = -scroll_info.nPos * GetItemHeight();
		m_v_size = max( (scroll_info.nMax+2)*((int)GetItemHeight()+1), m_rect.Height() );
	}
	else
	{
		m_v_offset = m_rect.top;
		m_v_size = m_rect.Height();
	}

	// Create an offscreen dc to paint with (prevents flicker issues)
	dc_ff.CreateCompatibleDC( &dc );
    bm_ff.CreateCompatibleBitmap( &dc, m_rect.Width(), m_rect.Height() );
    // Select the bitmap into the off-screen DC.
    bm_old = (CBitmap *)dc_ff.SelectObject( &bm_ff );
	// Default font in the DC is not the font used by 
	// the tree control, so grab it and select it in.
	font = GetFont();
	old_font = dc_ff.SelectObject( font );
	// We're going to draw text transparently
	old_mode = dc_ff.SetBkMode( TRANSPARENT );

	DrawBackGround( &dc_ff );
	DrawItems( &dc_ff );

    // Now Blt the changes to the real device context - this prevents flicker.
    dc.BitBlt( m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), &dc_ff, 0, 0, SRCCOPY);

	dc_ff.SelectObject( old_font );
	dc_ff.SetBkMode( old_mode );
    dc_ff.SelectObject( bm_old );
}
#endif


// Draw TreeCtrl Background - 
void CLTree::DrawBackGround( CDC* pDC )
{
	GradientFillRect( pDC, 
			CRect( m_h_offset, m_v_offset, m_h_size + m_h_offset, m_v_size + m_v_offset ), 
			RGB(255,255,255), RGB(255,255,255), !m_gradient_horz );
	
}



// Draw TreeCtrl Items
void CLTree::DrawItems( CDC *pDC )
{
	// draw items
	HTREEITEM show_item, parent;
	CRect rc_item;
	CString name;
	COLORREF color;
	DWORD tree_style;
	BITMAP bm;
	CDC dc_mem;
	CBitmap *button;
	int count = 0;
	int state;
	bool selected;
	bool has_children;

	show_item = GetFirstVisibleItem();
	if ( show_item == NULL )
		return;

	dc_mem.CreateCompatibleDC(NULL);
	color = pDC->GetTextColor();
	tree_style = ::GetWindowLong( m_hWnd, GWL_STYLE ); 

	do
	{
		state = GetItemState( show_item, TVIF_STATE );
		parent = GetParentItem( show_item );
		has_children = ItemHasChildren( show_item ) || parent == NULL;
		selected = (state & TVIS_SELECTED) && ((this == GetFocus()) || 
				(tree_style & TVS_SHOWSELALWAYS));
		if(!groupsMode)has_children=false;

		if ( GetItemRect( show_item, rc_item, FALSE ) )
		{rc_item.left+=20;
			if ( has_children  || selected )
			{
				COLORREF from;
				CRect rect;
				// Show 
				if ( selected )
					from = m_gradient_bkgd_sel;
				else
					from = m_gradient_bkgd_to - (m_gradient_bkgd_from - m_gradient_bkgd_to);
				rect.top = rc_item.top;
				rect.bottom = rc_item.bottom;
				rect.right = m_h_size + m_h_offset;
				if ( !has_children )
					rect.left = rc_item.left + m_h_offset;
				else
					rect.left = m_h_offset;
				//GradientFillRect( pDC, rect, from, m_gradient_bkgd_to, FALSE );
				if(has_children){
					int numBG=GetItemData(show_item);
					numBG%=skin.groupsBG.n-1;
					if(numBG==-1)numBG=skin.groupsBG.n-1;
					skin.groupsBG.bmp.DrawStretched(pDC, rc_item.left-20, rc_item.top, rect.right-rect.left, rc_item.bottom-rc_item.top, skin.groupsBG.w*numBG, 0, skin.groupsBG.w, skin.groupsBG.h);
				}//pDC->SetTextColor( RGB( 0, 0, 255 ) );
				

				if ( has_children )
				{
					// Draw an Open/Close button
					if ( state & TVIS_EXPANDED )
						button = &m_bmp_tree_open;
					else
						button = &m_bmp_tree_closed;
					VERIFY(button->GetObject(sizeof(bm), (LPVOID)&bm));
					CBitmap *bmp_old = (CBitmap*)dc_mem.SelectObject(button); 
					pDC->BitBlt( rc_item.left - bm.bmWidth - 2, rc_item.top, bm.bmWidth, bm.bmHeight, 
						&dc_mem, 0, 0, SRCAND );
					// CleanUp
					dc_mem.SelectObject( bmp_old );
				}
			}
			if ( !has_children )
			{
				// lookup the ICON instance (if any) and draw it
				if(displayMode==DM_NAMES_WITH_PHOTOS){
					rc_item.left=1;
				}
				if(selected){
					//pDC->FillSolidRect(rc_item, RGB(0,0,0));
					skin.listHl.bmp.DrawStretched(pDC, rc_item.left, rc_item.top, rc_item.Width(), rc_item.Height(), 0, 0, 0, 0);
				}
				rc_item.left+=2;
				rc_item.right-=1;
				HICON icon;
				icon = GetItemIcon( show_item );
				if ( icon != NULL )
					DrawIconEx( pDC->m_hDC, rc_item.left - 19, rc_item.top+1, icon, 16, 16,0,0, DI_NORMAL );
				if(displayMode==DM_NAMES_WITH_PHOTOS){
					if(items[show_item].userPhoto){
						HBITMAP bmp=items[show_item].userPhoto;
						bool scale=true;
						if(items[show_item].userPhoto30){
							bmp=items[show_item].userPhoto30;
							scale=false;
						}
						HGDIOBJ bmpOld=dc_mem.SelectObject(bmp);
						BLENDFUNCTION func;
						func.BlendOp=AC_SRC_OVER;
						func.AlphaFormat=0;
						func.SourceConstantAlpha=icon==skin.iconOnline?255:128;
						func.BlendFlags=0;
						if(scale)pDC->AlphaBlend(rc_item.left+1, rc_item.top+1, 30, 30, &dc_mem, 0, 0, 50, 50, func);
						else pDC->AlphaBlend(rc_item.left+1, rc_item.top+1, 30, 30, &dc_mem, 0, 0, 30, 30, func);
						dc_mem.SelectObject(bmpOld);
						if(icon==skin.iconOffline){
							if(selected){pDC->SetTextColor(RGB(154,177,198));}
							else{pDC->SetTextColor(RGB(128,128,128));}
						}else{
							if(selected)pDC->SetTextColor(RGB(255,255,255));
							else pDC->SetTextColor(RGB(0,0,0));
						}
					}else{
						if(icon)DrawIconEx( pDC->m_hDC, rc_item.left+8, rc_item.top+8, icon, 16, 16,0,0, DI_NORMAL );
					}
					rc_item.left=40;
				}
			}
			name = GetItemText( show_item );
			if ( selected )
			{
				if(displayMode==DM_NAMES_LIST){
				if ( !has_children  )
					pDC->SetTextColor( GetSysColor(COLOR_HIGHLIGHTTEXT) );
				}

				COLORREF col = pDC->GetBkColor();
				if(has_children)pDC->SetTextColor(RGB(0,0,0));
				pDC->DrawText( name, rc_item, DT_LEFT | DT_VCENTER | DT_SINGLELINE );
			}
			else
			{
				if(has_children)pDC->SetTextColor(RGB(0,0,0));
				if(displayMode==DM_NAMES_LIST){
					pDC->SetTextColor(RGB(0,0,0));
				}
				pDC->DrawText( name, rc_item, DT_LEFT | DT_VCENTER | DT_SINGLELINE );
			}
			//if ( state & TVIS_BOLD )
			//	pDC->SelectObject( font );
		}
	} while ( (show_item = GetNextVisibleItem( show_item )) != NULL );
}


// Determine if a referenced item is visible within the control window
bool CLTree::ItemIsVisible( HTREEITEM item )
{
	HTREEITEM scan_item;
	scan_item = GetFirstVisibleItem();
	while ( scan_item != NULL )
	{
		if ( item == scan_item )
			return true;
		scan_item = GetNextVisibleItem( scan_item );
	}
	return false;
}


// For a given tree node return an ICON for display on the left side.
// This default implementation only returns one icon.
// This function is virtual and meant to be overriden by deriving a class
// from CLTree and supplying your own icon images. 
HICON CLTree::GetItemIcon( HTREEITEM item )		
{
	ListEntry en=items[item];
	if(en.online)return skin.iconOnline;
	return skin.iconOffline;
}


// If the background is a bitmap, and a tree is expanded/collapsed we
// need to redraw the entire background because windows moves the bitmap
// up (on collapse) destroying the position of the background.
void CLTree::OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
		Invalidate();  // redraw everything
	*pResult = 0;
}

HTREEITEM CLTree::_InsertItem(ListEntry en, HTREEITEM parent){
/*	HTREEITEM res=InsertItem(en.userName, 1, 1, parent);
	SetItem(res, TVIF_PARAM, NULL, 0, 0, 0, 0, (LPARAM)res);
	if(displayMode==DM_NAMES_WITH_PHOTOS){
		TVITEMEX itx;
		itx.mask=TVIF_INTEGRAL;
		itx.iIntegral=2;
		itx.hItem=res;
		SendMessage(TVM_SETITEMW, 0, (LPARAM)&itx);
	}
	items[res]=en;
	bool add_to_items=true;
	for(int i=0;i<items_array.size();i++){
		if(items_array[i].uid==en.uid){
			items_array[i]=en;
			add_to_items=false;
			break;
		}
	}
	if(add_to_items)items_array.push_back(en);
	if(!&items_by_uid[en.uid]){
		std::vector<HTREEITEM> v;
		v.push_back(res);
		items_by_uid[en.uid]=v;
	}
	else
	{
		items_by_uid[en.uid].push_back(res);
	}
	return res;*/return 0;
}

int CALLBACK MyCompareProc(HTREEITEM lParam1, HTREEITEM lParam2, LPARAM lParamSort)
{
   /*CLTree* tc = (CLTree*)lParamSort;
   /*if(!tc->items[lParam1].online){
	   TVITEMEX itx;
		itx.mask=TVIF_INTEGRAL;
		itx.iIntegral=0;
		itx.hItem=lParam1;
		tc->SendMessage(TVM_SETITEMW, 0, (LPARAM)&itx);
   }
   if(!tc->items[lParam2].online){
	   TVITEMEX itx;
		itx.mask=TVIF_INTEGRAL;
		itx.iIntegral=0;
		itx.hItem=lParam2;
		tc->SendMessage(TVM_SETITEMW, 0, (LPARAM)&itx);
   }* /
   if(!tc->items[lParam1].online && tc->items[lParam2].online){
   return 1;
   }
   if(tc->items[lParam1].online && !tc->items[lParam2].online){
   return -1;
   }
   if(tc->items[lParam1].online==tc->items[lParam2].online){
   return wcscmp(tc->items[lParam1].userName, tc->items[lParam2].userName);
   }*/return 0;
}

void CLTree::Sort(HTREEITEM it){
	TVSORTCB tvs;
	tvs.hParent = it;
	tvs.lpfnCompare = (PFNTVCOMPARE)MyCompareProc;
	tvs.lParam = (LPARAM)this;
	SortChildrenCB(&tvs);
}


void CLTree::StartSearch(void)
{
	updating=true;
	DeleteAllItems();
	items.clear();
	items_by_uid.clear();
	_groupsMode=groupsMode;
	groupsMode=false;
	searching=true;
	updating=false;
}


void CLTree::UpdateSearch(wchar_t* q)
{
	/*updating=true;
	DeleteAllItems();
	items.clear();
	items_by_uid.clear();
	CString wq=q;
	wq.MakeLower();
	for(int i=0;i<wq.GetLength();i++){
		if(wq[i]>=L'À' && wq[i]<=L'ß'){
			wq.SetAt(i, wq[i]+(L'à'-L'À'));
		}
	}
	for(int i=0;i<items_array.size();i++){
		CString ws1=items_array[i].userName;
		ws1.MakeLower();
		for(int j=0;j<ws1.GetLength();j++){
			if(ws1[j]>=L'À' && ws1[j]<=L'ß'){
				ws1.SetAt(j, ws1[j]+(L'à'-L'À'));
			}
		}
		if(ws1.Find(wq)>=0){
			_InsertItem(items_array[i], NULL);
		}
	}
	Sort(NULL);
	UpdateWindow();
	updating=false;*/
}


void CLTree::StopSearch(void)
{
	DeleteAllItems();
	items.clear();
	items_by_uid.clear();

	searching=false;
	groupsMode=_groupsMode;
	updating=true;
	if(groupsMode){
		DeleteAllItems();
		items.clear();
		items_by_uid.clear();
		groupsMode=true;
		std::vector<HTREEITEM> groups;
		for(int i=0;i<lists_ids.size();i++){
			HTREEITEM it=InsertItem(lists[lists_ids[i]]);
			SetItemData(it, i);
			groups.push_back(it);
		}
		HTREEITEM tiNoGroup=InsertItem(CLang::Get(L"ungrouped"), 0, 0);
		SetItemData(tiNoGroup, -1);
		for(int i=0;i<items_array.size();i++){
			//if((hideOffline && items_array[i].online) || !hideOffline){
			for(int j=0;j<items_array[i].groups.size();j++){
				if(items_array[i].groups[j]==-1){
					_InsertItem(items_array[i], tiNoGroup);
				}else{
					_InsertItem(items_array[i], groups[items_array[i].groups[j]-1]);
				}
			}
			//}
		}
		for(int i=0;i<groups.size();i++){
			Sort(groups[i]);
		}
		Sort(tiNoGroup);
	}else{
		groupsMode=false;
		DeleteAllItems();
		items.clear();
		items_by_uid.clear();
		for(int i=0;i<items_array.size();i++){
			//if((hideOffline && items_array[i].online) || !hideOffline){
			_InsertItem(items_array[i], NULL);
			//}
		}
		Sort(NULL);
	}
	UpdateWindow();
	updating=false;
}


void CLTree::SetMode(int m)
{
	if(m==displayMode)return;
	displayMode=m;
	updating=true;
	bool needShowHide=IsWindowVisible();
	if(m==DM_NAMES_LIST){
		if(needShowHide)ShowWindow(SW_HIDE);
		SetItemHeight(18);
		for(int i=0;i<uids.size();i++){
			for(int j=0;j<items_by_uid[uids[i]].size();j++){
				TVITEMEX itx;
				itx.mask=TVIF_INTEGRAL;
				itx.iIntegral=1;
				itx.hItem=items_by_uid[uids[i]][j];
				SendMessage(TVM_SETITEMW, 0, (LPARAM)&itx);
			}
		}
		if(needShowHide)ShowWindow(SW_SHOW);
	}
	if(m==DM_NAMES_WITH_PHOTOS){
		if(needShowHide)ShowWindow(SW_HIDE);
		SetItemHeight(16);
		//if(groupsMode){
		for(int i=0;i<uids.size();i++){
			for(int j=0;j<items_by_uid[uids[i]].size();j++){
				TVITEMEX itx;
				itx.mask=TVIF_INTEGRAL;
				itx.iIntegral=2;
				itx.hItem=items_by_uid[uids[i]][j];
				SendMessage(TVM_SETITEMW, 0, (LPARAM)&itx);
			}
		}
		//}else{

		//}
		if(needShowHide)ShowWindow(SW_SHOW);
	}
	UpdateWindow();
	RedrawWindow();
	updating=false;
}


void CLTree::SetGroupsMode(bool m)
{
	if(m==groupsMode)return;
	updating=true;
	if(m){
		DeleteAllItems();
		items.clear();
		items_by_uid.clear();
		groupsMode=true;
		std::vector<HTREEITEM> groups;
		for(int i=0;i<lists_ids.size();i++){
			HTREEITEM it=InsertItem(lists[lists_ids[i]]);
			SetItemData(it, i);
			groups.push_back(it);
		}
		HTREEITEM tiNoGroup=InsertItem(CLang::Get(L"ungrouped"), 0, 0);
		SetItemData(tiNoGroup, -1);
		for(int i=0;i<items_array.size();i++){
			for(int j=0;j<items_array[i].groups.size();j++){
				if(items_array[i].groups[j]==-1){
					_InsertItem(items_array[i], tiNoGroup);
				}else{
					_InsertItem(items_array[i], groups[items_array[i].groups[j]-1]);
				}
			}
		}
		for(int i=0;i<groups.size();i++){
			Sort(groups[i]);
		}
		Sort(tiNoGroup);
	}else{
		groupsMode=false;
		DeleteAllItems();
		items.clear();
		items_by_uid.clear();
		for(int i=0;i<items_array.size();i++){
			_InsertItem(items_array[i], NULL);
		}
		Sort(NULL);
	}
	UpdateWindow();
	updating=false;
}
