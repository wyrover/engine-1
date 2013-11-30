#pragma once

#include "UIAnimatedButton.h"
#include "../Color.h"

namespace star
{
	class UITextField;

	class UIAnimatedTextButton : public UIAnimatedButton
	{
	public:
		UIAnimatedTextButton(
			const tstring & name,
			const tstring & file,
			const tstring & spritesheet,
			const tstring & text,
			const tstring & fontName,
			const Color & color
			);

		UIAnimatedTextButton(
			const tstring & name,
			const tstring & file,
			const tstring & spritesheet,
			const tstring & text,
			const tstring & fontName,
			const tstring & fontPath,
			float32 fontSize,
			const Color & color
			);

		virtual ~UIAnimatedTextButton();
		virtual void AfterInitialized();

		void SetIdleColor(const Color & color);
#ifdef DESKTOP
		void SetHoverColor(const Color & color);
#endif
		void SetClickColor(const Color & color);
		void SetDisableColor(const Color & color);

		void SetText(const tstring & text);
		const tstring & GetText() const;

		void TranslateText(const vec2& translation);
		void TranslateText(float32 x, float32 y);
		void TranslateTextX(float32 x);
		void TranslateTextY(float32 y);
		void SetHorizontalAlignmentText(HorizontalAlignment alignment);
		void SetVerticalAlignmentText(VerticalAlignment alignment);
		void SetTextCentered();

	protected:
		virtual void GoIdle();
#ifdef DESKTOP
		virtual void GoHover();
#endif
		virtual void GoDown();
		virtual void GoDisable();
		
		UITextField * m_pTextField;
		Color m_StateColors[4];

	private:
		UIAnimatedTextButton(const UIAnimatedTextButton &);
		UIAnimatedTextButton(UIAnimatedTextButton &&);
		UIAnimatedTextButton & operator=(const UIAnimatedTextButton &);
		UIAnimatedTextButton & operator=(UIAnimatedTextButton &&);
	};
}
