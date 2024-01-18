#pragma once

#include <cstdint>
#include "../eterBase/Utils.h"
#include "../UserInterface/Locale_inc.h"
#if defined(INGAME_WIKI) || defined(ENABLE_INGAME_WIKI2)
#include <cstdint>
#endif

namespace UI
{
	class CWindow
	{
		public:
			typedef std::list<CWindow *> TWindowContainer;

			static DWORD Type();
			BOOL IsType(DWORD dwType);

			enum EHorizontalAlign
			{
				HORIZONTAL_ALIGN_LEFT = 0,
				HORIZONTAL_ALIGN_CENTER = 1,
				HORIZONTAL_ALIGN_RIGHT = 2,
			};

			enum EVerticalAlign
			{
				VERTICAL_ALIGN_TOP = 0,
				VERTICAL_ALIGN_CENTER = 1,
				VERTICAL_ALIGN_BOTTOM = 2,
			};

			enum EFlags
			{
				FLAG_MOVABLE			= (1 <<  0),	// 움직일 수 있는 창
				FLAG_LIMIT				= (1 <<  1),	// 창이 화면을 벗어나지 않음
				FLAG_SNAP				= (1 <<  2),	// 스냅 될 수 있는 창
				FLAG_DRAGABLE			= (1 <<  3),
				FLAG_ATTACH				= (1 <<  4),	// 완전히 부모에 붙어 있는 창 (For Drag / ex. ScriptWindow)
				FLAG_RESTRICT_X			= (1 <<  5),	// 좌우 이동 제한
				FLAG_RESTRICT_Y			= (1 <<  6),	// 상하 이동 제한
				FLAG_NOT_CAPTURE		= (1 <<  7),
				FLAG_FLOAT				= (1 <<  8),	// 공중에 떠있어서 순서 재배치가 되는 창
				FLAG_NOT_PICK			= (1 <<  9),	// 마우스에 의해 Pick되지 않는 창
				FLAG_IGNORE_SIZE		= (1 << 10),
				FLAG_RTL				= (1 << 11),	// Right-to-left
			};

		public:
			CWindow(PyObject * ppyObject);
			virtual ~CWindow();

			void			AddChild(CWindow * pWin);

			void			Clear();
			void			DestroyHandle();
			void			Update();
			void			Render();

			void			SetName(const char * c_szName);
			const char *	GetName()		{ return m_strName.c_str(); }
			void			SetSize(long width, long height);
			long			GetWidth()		{ return m_lWidth; }
			long			GetHeight()		{ return m_lHeight; }

			void			SetHorizontalAlign(DWORD dwAlign);
			void			SetVerticalAlign(DWORD dwAlign);
			void			SetPosition(long x, long y);
			void			GetPosition(long * plx, long * ply);
			long			GetPositionX( void ) const		{ return m_x; }
			long			GetPositionY( void ) const		{ return m_y; }
			RECT &			GetRect()		{ return m_rect; }
			void			GetLocalPosition(long & rlx, long & rly);
			void			GetMouseLocalPosition(long & rlx, long & rly);
			long			UpdateRect();

			RECT &			GetLimitBias()	{ return m_limitBiasRect; }
			void			SetLimitBias(long l, long r, long t, long b) { m_limitBiasRect.left = l, m_limitBiasRect.right = r, m_limitBiasRect.top = t, m_limitBiasRect.bottom = b; }

			void			Show();
			void			Hide();
#if defined(INGAME_WIKI) || defined(ENABLE_INGAME_WIKI2)
			virtual	bool	IsShow();
			void			OnHideWithChilds();
			void			OnHide();
#else
			bool			IsShow() { return m_bShow; }
#endif
			bool			IsRendering();

			bool			HasParent()		{ return m_pParent ? true : false; }
			bool			HasChild()		{ return m_pChildList.empty() ? false : true; }
			int				GetChildCount()	{ return m_pChildList.size(); }

			CWindow *		GetRoot();
			CWindow *		GetParent();
#if defined(INGAME_WIKI) || defined(ENABLE_INGAME_WIKI2)
			bool			IsChild(CWindow* pWin, bool bCheckRecursive = false);
#else
			bool			IsChild(CWindow * pWin);
#endif
			void			DeleteChild(CWindow * pWin);
			void			SetTop(CWindow * pWin);

			bool			IsIn(long x, long y);
			bool			IsIn();
			CWindow *		PickWindow(long x, long y);
			CWindow *		PickTopWindow(long x, long y);	// NOTE : Children으로 내려가지 않고 상위에서만
															//        체크 하는 특화된 함수

			void			__RemoveReserveChildren();

			void			AddFlag(DWORD flag)		{ SET_BIT(m_dwFlag, flag);		}
			void			RemoveFlag(DWORD flag)	{ REMOVE_BIT(m_dwFlag, flag);	}
			bool			IsFlag(DWORD flag)		{ return (m_dwFlag & flag) ? true : false;	}
			/////////////////////////////////////
#if defined(INGAME_WIKI) || defined(ENABLE_INGAME_WIKI2)
			void			SetInsideRender(BOOL flag);
			void			GetRenderBox(RECT* box);
			void			UpdateTextLineRenderBox();
			void			UpdateRenderBox();
			void			UpdateRenderBoxRecursive();
			virtual void	OnAfterRender();
			virtual void	OnUpdateRenderBox() {}
#endif
			virtual void	OnRender();
			virtual void	OnUpdate();
			virtual void	OnChangePosition(){}

			virtual void	OnSetFocus();
			virtual void	OnKillFocus();

			virtual void	OnMouseDrag(long lx, long ly);
			virtual void	OnMouseOverIn();
			virtual void	OnMouseOverOut();
			virtual void	OnMouseOver();
			virtual void	OnDrop();
			virtual void	OnTop();
			virtual void	OnIMEUpdate();

			virtual void	OnMoveWindow(long x, long y);

			///////////////////////////////////////

			BOOL			RunIMETabEvent();
			BOOL			RunIMEReturnEvent();
			BOOL			RunIMEKeyDownEvent(int ikey);

			CWindow *		RunKeyDownEvent(int ikey);
			BOOL			RunKeyUpEvent(int ikey);
			BOOL			RunPressEscapeKeyEvent();
			BOOL			RunPressExitKeyEvent();

			virtual BOOL	OnIMETabEvent();
			virtual BOOL	OnIMEReturnEvent();
			virtual BOOL	OnIMEKeyDownEvent(int ikey);

			virtual BOOL	OnIMEChangeCodePage();
			virtual BOOL	OnIMEOpenCandidateListEvent();
			virtual BOOL	OnIMECloseCandidateListEvent();
			virtual BOOL	OnIMEOpenReadingWndEvent();
			virtual BOOL	OnIMECloseReadingWndEvent();

			virtual BOOL	OnMouseLeftButtonDown();
			virtual BOOL	OnMouseLeftButtonUp();
			virtual BOOL	OnMouseLeftButtonDoubleClick();
			virtual BOOL	OnMouseRightButtonDown();
			virtual BOOL	OnMouseRightButtonUp();
			virtual BOOL	OnMouseRightButtonDoubleClick();
			virtual BOOL	OnMouseMiddleButtonDown();
			virtual BOOL	OnMouseMiddleButtonUp();
			virtual BOOL	OnKeyDown(int ikey);
			virtual BOOL	OnKeyUp(int ikey);
			virtual BOOL	OnPressEscapeKey();
			virtual BOOL	OnPressExitKey();

			///////////////////////////////////////
			virtual bool	RunMouseWheelEvent(long nLen);
			virtual bool	OnRunMouseWheelEvent(long nLen);
			virtual void	SetColor(DWORD dwColor){}
			virtual BOOL	OnIsType(DWORD dwType);
			/////////////////////////////////////

			virtual BOOL	IsWindow() { return TRUE; }
			/////////////////////////////////////
#if defined(INGAME_WIKI) || defined(ENABLE_INGAME_WIKI2)
		public:
			virtual void	iSetRenderingRect(int iLeft, int iTop, int iRight, int iBottom);
			virtual void	SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom);
			virtual int		GetRenderingWidth();
			virtual int		GetRenderingHeight();
			void			ResetRenderingRect(bool bCallEvent = true);

		private:
			virtual void	OnSetRenderingRect();
#endif
		protected:
			std::string			m_strName;

			EHorizontalAlign	m_HorizontalAlign;
			EVerticalAlign		m_VerticalAlign;
			long				m_x, m_y;				// X,Y 상대좌표
			long				m_lWidth, m_lHeight;	// 크기
			RECT				m_rect;					// Global 좌표
			RECT				m_limitBiasRect;		// limit bias 값
#if defined(INGAME_WIKI) || defined(ENABLE_INGAME_WIKI2)
			RECT				m_renderingRect;
#endif
			bool				m_bMovable;
			bool				m_bShow;

			DWORD				m_dwFlag;

			PyObject *			m_poHandler;

			CWindow	*			m_pParent;
			TWindowContainer	m_pChildList;

			BOOL				m_isUpdatingChildren;
			TWindowContainer	m_pReserveChildList;
#if defined(INGAME_WIKI) || defined(ENABLE_INGAME_WIKI2)
			BOOL				m_isInsideRender;
			RECT				m_renderBox;
#endif

#ifdef _DEBUG
		public:
			DWORD				DEBUG_dwCounter;
#endif
	};

	class CLayer : public CWindow
	{
		public:
			CLayer(PyObject * ppyObject) : CWindow(ppyObject) {}
			virtual ~CLayer() {}

			BOOL IsWindow() { return FALSE; }
	};

#if defined(INGAME_WIKI) || defined(ENABLE_INGAME_WIKI2)
	class CUiWikiRenderTarget : public CWindow
	{
		public:
			CUiWikiRenderTarget(PyObject * ppyObject);
			virtual ~CUiWikiRenderTarget();
		
		public:
			bool	SetWikiRenderTargetModule(int iRenderTargetModule);
			void	OnUpdateRenderBox();
		
		protected:
			void	OnRender();
		
		protected:
			DWORD	m_dwIndex;
	};
#endif

	class CBox : public CWindow
	{
		public:
			CBox(PyObject * ppyObject);
			virtual ~CBox();

			void SetColor(DWORD dwColor);

		protected:
			void OnRender();

		protected:
			DWORD m_dwColor;
	};

	class CBar : public CWindow
	{
		public:
			CBar(PyObject * ppyObject);
			virtual ~CBar();

			void SetColor(DWORD dwColor);

		protected:
			void OnRender();

		protected:
			DWORD m_dwColor;
	};

	class CLine : public CWindow
	{
		public:
			CLine(PyObject * ppyObject);
			virtual ~CLine();

			void SetColor(DWORD dwColor);

		protected:
			void OnRender();

		protected:
			DWORD m_dwColor;
	};

	class CBar3D : public CWindow
	{
		public:
			static DWORD Type();

		public:
			CBar3D(PyObject * ppyObject);
			virtual ~CBar3D();

			void SetColor(DWORD dwLeft, DWORD dwRight, DWORD dwCenter);

		protected:
			void OnRender();

		protected:
			DWORD m_dwLeftColor;
			DWORD m_dwRightColor;
			DWORD m_dwCenterColor;
	};

	// Text
	class CTextLine : public CWindow
	{
#if defined(INGAME_WIKI) || defined(ENABLE_INGAME_WIKI2)
		public:
			static DWORD Type();
#endif
		public:
			CTextLine(PyObject * ppyObject);
			virtual ~CTextLine();

			void SetMax(int iMax);
			void SetHorizontalAlign(int iType);
			void SetVerticalAlign(int iType);
			void SetSecret(BOOL bFlag);
			void SetOutline(BOOL bFlag);
			void SetFeather(BOOL bFlag);
			void SetMultiLine(BOOL bFlag);
			void SetFontName(const char * c_szFontName);
			void SetFontColor(DWORD dwColor);
			void SetLimitWidth(float fWidth);
#if defined(INGAME_WIKI) || defined(ENABLE_INGAME_WIKI2)
			void SetFixedRenderPos(WORD startPos, WORD endPos) { m_TextInstance.SetFixedRenderPos(startPos, endPos); }
			void GetRenderPositions(WORD& startPos, WORD& endPos) { m_TextInstance.GetRenderPositions(startPos, endPos); }
#endif
			void ShowCursor();
			void HideCursor();
#if defined(INGAME_WIKI) || defined(ENABLE_INGAME_WIKI2)
			bool IsShowCursor();
#endif
			int GetCursorPosition();

			void SetText(const char * c_szText);
			const char * GetText();

			void GetTextSize(int* pnWidth, int* pnHeight);
#if defined(INGAME_WIKI) || defined(ENABLE_INGAME_WIKI2)
			bool IsShow();
			int GetRenderingWidth();
			int GetRenderingHeight();
			void OnSetRenderingRect();
#endif

		protected:
			void OnUpdate();
			void OnRender();
			void OnChangePosition();

			virtual void OnSetText(const char * c_szText);
#if defined(INGAME_WIKI) || defined(ENABLE_INGAME_WIKI2)
			void OnUpdateRenderBox() {
				UpdateTextLineRenderBox();
				m_TextInstance.SetRenderBox(m_renderBox);
			}
#endif

		protected:
			CGraphicTextInstance m_TextInstance;
	};

	class CNumberLine : public CWindow
	{
		public:
			CNumberLine(PyObject * ppyObject);
			CNumberLine(CWindow * pParent);
			virtual ~CNumberLine();

			void SetPath(const char * c_szPath);
			void SetHorizontalAlign(int iType);
			void SetNumber(const char * c_szNumber);

		protected:
			void ClearNumber();
			void OnRender();
			void OnChangePosition();

		protected:
			std::string m_strPath;
			std::string m_strNumber;
			std::vector<CGraphicImageInstance *> m_ImageInstanceVector;

			int m_iHorizontalAlign;
			DWORD m_dwWidthSummary;
	};

	// Image
	class CImageBox : public CWindow
	{
		public:
			CImageBox(PyObject * ppyObject);
			virtual ~CImageBox();
			void UnloadImage()
			{
				OnDestroyInstance();
				SetSize(GetWidth(), GetHeight());
				UpdateRect();
			}

			BOOL LoadImage(const char * c_szFileName);
			void SetDiffuseColor(float fr, float fg, float fb, float fa);
#ifdef NEW_PET_SYSTEM	
			void SetScale(float sx, float sy);
#endif
			int GetWidth();
			int GetHeight();

		protected:
			virtual void OnCreateInstance();
			virtual void OnDestroyInstance();

			virtual void OnUpdate();
			virtual void OnRender();
			void OnChangePosition();

		protected:
			CGraphicImageInstance * m_pImageInstance;
	};

	class CMarkBox : public CWindow
	{
		public:
			CMarkBox(PyObject * ppyObject);
			virtual ~CMarkBox();

			void LoadImage(const char * c_szFilename);
			void SetDiffuseColor(float fr, float fg, float fb, float fa);
			void SetIndex(UINT uIndex);
			void SetScale(FLOAT fScale);

		protected:
			virtual void OnCreateInstance();
			virtual void OnDestroyInstance();

			virtual void OnUpdate();
			virtual void OnRender();
			void OnChangePosition();
		protected:
			CGraphicMarkInstance * m_pMarkInstance;
	};
	class CExpandedImageBox : public CImageBox
	{
		public:
			static DWORD Type();

		public:
			CExpandedImageBox(PyObject * ppyObject);
			virtual ~CExpandedImageBox();

			void SetScale(float fx, float fy);
			void SetOrigin(float fx, float fy);
			void SetRotation(float fRotation);
			void SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom);
			void SetRenderingMode(int iMode);
#if defined(INGAME_WIKI) || defined(ENABLE_INGAME_WIKI2)
			int GetRenderingWidth();
			int GetRenderingHeight();
			void OnSetRenderingRect();
			void SetExpandedRenderingRect(float fLeftTop, float fLeftBottom, float fTopLeft, float fTopRight, float fRightTop, float fRightBottom, float fBottomLeft, float fBottomRight);
			void SetTextureRenderingRect(float fLeft, float fTop, float fRight, float fBottom);
			DWORD GetPixelColor(DWORD x, DWORD y);
#endif
//#ifdef ENABLE_IMAGE_CLIP_RECT
			void SetImageClipRect(float fLeft, float fTop, float fRight, float fBottom, bool bIsVertical = false);
//#endif

		protected:
			void OnCreateInstance();
			void OnDestroyInstance();
#if defined(INGAME_WIKI) || defined(ENABLE_INGAME_WIKI2)
			void OnUpdateRenderBox();
#endif
			virtual void OnUpdate();
			virtual void OnRender();

			BOOL OnIsType(DWORD dwType);
	};
	class CAniImageBox : public CWindow
	{
		public:
			static DWORD Type();

		public:
			CAniImageBox(PyObject * ppyObject);
			virtual ~CAniImageBox();

			void SetDelay(int iDelay);
			void AppendImage(const char * c_szFileName, float xs = 0, float ys = 0
#ifdef ENABLE_ACCE_SYSTEM
			, float r = 1.0, float g = 1.0, float b = 1.0, float a = 1.0
#endif
			);
			void SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom);
			void SetRenderingMode(int iMode);
#if defined(ENABLE_OFFLINESHOP_REWORK)
			void SetImageClipRect(float fLeft, float fTop, float fRight, float fBottom, bool bIsVertical = false);
#endif
			void SetDiffuseColor(float r, float g, float b, float a);
#ifdef ENABLE_NEW_FISHING_SYSTEM
			void SetRotation(float r);
#endif
			void ResetFrame();
#if defined(ENABLE_NEW_FISH_EVENT)
			void SetAniImgScale(float, float);
#endif

		protected:
			void OnUpdate();
			void OnRender();
			void OnChangePosition();
			virtual void OnEndFrame();

			BOOL OnIsType(DWORD dwType);

		protected:
			BYTE m_bycurDelay;
			BYTE m_byDelay;
			BYTE m_bycurIndex;
#ifdef ENABLE_NEW_FISHING_SYSTEM
			bool m_RotationProcess;
#endif
			std::vector<CGraphicExpandedImageInstance*> m_ImageVector;
#if defined(USE_ANI_IMAGE_RENEWAL)
			std::queue<std::string> m_ImageFileNames;
			std::function<void(CGraphicExpandedImageInstance*)> m_SetRenderingRect, m_SetRenderingMode, m_SetDiffuseColor;
#endif
	};

#ifdef ENABLE_NEW_FISHING_SYSTEM
	class CFishBox : public CWindow
	{
		public:
			static DWORD Type();

		public:
			CFishBox(PyObject * ppyObject);
			virtual ~CFishBox();

			bool GetMove();
			void MoveStart();
			void MoveStop();
			void SetRandomPosition();
			void GetPosition(int * x, int * y);
			void RegisterAni(CAniImageBox* ani);

		protected:
			void OnUpdate();
			virtual void OnEndMove();

			BOOL OnIsType(DWORD dwType);

			D3DXVECTOR2 m_v2SrcPos, m_v2DstPos, m_v2NextPos, m_v2Direction, m_v2NextDistance;
			float m_fDistance, m_fMoveSpeed;
			bool m_bIsMove;
			bool m_left, m_right;
			BYTE m_lastRandom;
			float m_direction;
	};
#endif

	// Button
	class CButton : public CWindow
	{
		public:
			CButton(PyObject * ppyObject);
			virtual ~CButton();

			BOOL SetUpVisual(const char * c_szFileName);
			BOOL SetOverVisual(const char * c_szFileName);
			BOOL SetDownVisual(const char * c_szFileName);
			BOOL SetDisableVisual(const char * c_szFileName);

			const char * GetUpVisualFileName();
			const char * GetOverVisualFileName();
			const char * GetDownVisualFileName();

			void Flash();

			// #if app.ENABLE_SKILL_COLOR_SYSTEM
			void EnableFlash();
			void DisableFlash();

			void Enable();
			void Disable();

			void SetUp();
			void Up();
			void Over();
			void Down();

			BOOL IsDisable();
			BOOL IsPressed();
#if defined(INGAME_WIKI) || defined(ENABLE_INGAME_WIKI2)
			void OnSetRenderingRect();
#endif
#ifdef ENABLE_NEW_GAMEOPTION
			static DWORD Type();
			void SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom);
			BOOL OnIsType(DWORD dwType);
#endif
		protected:
			void OnUpdate();
			void OnRender();
			void OnChangePosition();

			BOOL OnMouseLeftButtonDown();
			BOOL OnMouseLeftButtonDoubleClick();
			BOOL OnMouseLeftButtonUp();
			void OnMouseOverIn();
			void OnMouseOverOut();
			BOOL IsEnable();

#if defined(INGAME_WIKI) || defined(ENABLE_INGAME_WIKI2) || defined(ENABLE_NEW_GAMEOPTION)
			void SetCurrentVisual(CGraphicExpandedImageInstance* pVisual);
#else
			void SetCurrentVisual(CGraphicImageInstance * pVisual);
#endif

		protected:
			BOOL m_bEnable;
			BOOL m_isPressed;
			BOOL m_isFlash;
#if defined(INGAME_WIKI) || defined(ENABLE_INGAME_WIKI2) || defined(ENABLE_NEW_GAMEOPTION)
			CGraphicExpandedImageInstance* m_pcurVisual;
			CGraphicExpandedImageInstance m_upVisual;
			CGraphicExpandedImageInstance m_overVisual;
			CGraphicExpandedImageInstance m_downVisual;
			CGraphicExpandedImageInstance m_disableVisual;
#else
			CGraphicImageInstance * m_pcurVisual;
			CGraphicImageInstance m_upVisual;
			CGraphicImageInstance m_overVisual;
			CGraphicImageInstance m_downVisual;
			CGraphicImageInstance m_disableVisual;
#endif
	};

	class CRadioButton : public CButton
	{
		public:
#ifdef ENABLE_NEW_GAMEOPTION
			static DWORD Type();
#endif
			CRadioButton(PyObject * ppyObject);
			virtual ~CRadioButton();

		protected:
#ifdef ENABLE_NEW_GAMEOPTION
			BOOL OnIsType(DWORD dwType);
#endif
			BOOL OnMouseLeftButtonDown();
			BOOL OnMouseLeftButtonUp();
			void OnMouseOverIn();
			void OnMouseOverOut();
	};
	class CToggleButton : public CButton
	{
		public:
#ifdef ENABLE_NEW_GAMEOPTION
			static DWORD Type();
#endif
			CToggleButton(PyObject * ppyObject);
			virtual ~CToggleButton();

		protected:
#ifdef ENABLE_NEW_GAMEOPTION
			BOOL OnIsType(DWORD dwType);
#endif
			BOOL OnMouseLeftButtonDown();
			BOOL OnMouseLeftButtonUp();
			void OnMouseOverIn();
			void OnMouseOverOut();
	};
	class CDragButton : public CButton
	{
		public:
			CDragButton(PyObject * ppyObject);
			virtual ~CDragButton();

			void SetRestrictMovementArea(int ix, int iy, int iwidth, int iheight);

		protected:
			void OnChangePosition();
			void OnMouseOverIn();
			void OnMouseOverOut();

		protected:
			RECT m_restrictArea;
	};
	
#ifdef ENABLE_UI_EXTRA
	class CMoveTextLine : public CTextLine
	{
	public:
		CMoveTextLine(PyObject * ppyObject);
		virtual ~CMoveTextLine();

	public:
		static DWORD Type();

		void SetMoveSpeed(float fSpeed);
		void SetMovePosition(float fDstX, float fDstY);
		bool GetMove();
		void MoveStart();
		void MoveStop();

	protected:
		void OnUpdate();
		void OnRender();
		void OnEndMove();
		void OnChangePosition();

		BOOL OnIsType(DWORD dwType);

		D3DXVECTOR2 m_v2SrcPos, m_v2DstPos, m_v2NextPos, m_v2Direction, m_v2NextDistance;
		float m_fDistance, m_fMoveSpeed;
		bool m_bIsMove;
	};
	class CMoveImageBox : public CImageBox
	{
		public:
			CMoveImageBox(PyObject * ppyObject);
			virtual ~CMoveImageBox();

			static DWORD Type();

			void SetMoveSpeed(float fSpeed);
			void SetMovePosition(float fDstX, float fDstY);
			bool GetMove();
			void MoveStart();
			void MoveStop();

		protected:
			virtual void OnCreateInstance();
			virtual void OnDestroyInstance();

			virtual void OnUpdate();
			virtual void OnRender();
			virtual void OnEndMove();

			BOOL OnIsType(DWORD dwType);

			D3DXVECTOR2 m_v2SrcPos, m_v2DstPos, m_v2NextPos, m_v2Direction, m_v2NextDistance;
			float m_fDistance, m_fMoveSpeed;
			bool m_bIsMove;
	};
	class CMoveScaleImageBox : public CMoveImageBox
	{
		public:
			CMoveScaleImageBox(PyObject * ppyObject);
			virtual ~CMoveScaleImageBox();

			static DWORD Type();

			void SetMaxScale(float fMaxScale);
			void SetMaxScaleRate(float fMaxScaleRate);
			void SetScalePivotCenter(bool bScalePivotCenter);

		protected:
			virtual void OnCreateInstance();
			virtual void OnDestroyInstance();

			virtual void OnUpdate();

			BOOL OnIsType(DWORD dwType);

			float m_fMaxScale, m_fMaxScaleRate, m_fScaleDistance, m_fAdditionalScale;
			D3DXVECTOR2 m_v2CurScale;
	};
#endif
};

extern BOOL g_bOutlineBoxEnable;
#ifdef ENABLE_NEW_FISHING_SYSTEM
#ifndef MAX_FISHING_WAYS
#define MAX_FISHING_WAYS 6
#endif
extern float listFishLeft[MAX_FISHING_WAYS][3];
extern float listFishRight[MAX_FISHING_WAYS][3];
#endif
