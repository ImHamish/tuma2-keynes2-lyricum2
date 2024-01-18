#pragma once

#include "Model.h"
#include "Motion.h"

class CGraphicThing : public CResource
{
	public:
		typedef CRef<CGraphicThing> TRef;

	public:
		static CGraphicThing::TType Type();

	public:
		CGraphicThing(const char * c_szFileName);
		virtual ~CGraphicThing();

		virtual bool			CreateDeviceObjects();
		virtual void			DestroyDeviceObjects();

		bool					CheckModelIndex(int iModel) const;
		CGrannyModel *			GetModelPointer(int iModel);
		int						GetModelCount() const;

		bool					CheckMotionIndex(int iMotion) const;
#if defined(ENABLE_GRANNY_INBUILD_ANIMATION)
		std::shared_ptr<CGrannyMotion> GetMotionPointer(int32_t iMotion);
#else
		CGrannyMotion* GetMotionPointer(int32_t iMotion);
#endif
		int						GetMotionCount() const;
#ifdef ENABLE_3D_MODELS_TEXTURE_FIX
		int						GetTextureCount() const;
		const char *			GetTexturePath(int iTexture);
#endif

	protected:
		void					Initialize();

		bool					LoadModels();
		bool					LoadMotions();

	protected:
		bool					OnLoad(int iSize, const void* c_pvBuf);
		void					OnClear();
		bool					OnIsEmpty() const;
		bool					OnIsType(TType type);

	protected:
		granny_file *			m_pgrnFile;
		granny_file_info *		m_pgrnFileInfo;

		granny_animation *		m_pgrnAni;

		CGrannyModel *			m_models;
#if defined(ENABLE_GRANNY_INBUILD_ANIMATION)
		std::vector<std::shared_ptr<CGrannyMotion>> m_motions;
#else
		CGrannyMotion* m_motions;
#endif
};
