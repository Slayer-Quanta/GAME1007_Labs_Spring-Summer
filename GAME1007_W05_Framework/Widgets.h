#pragma once
#include "Core.h"
#include <string>
#include <vector>
#include <algorithm>
// Add button integer if we want more than left-click
using OnMouseClick = void(*)(void* data);	// Called when mouse is inside the widget and clicked
using OnMouseOver = void(*)(void* data);	// Called when mouse is inside the widget
using OnMouseOut = void(*)(void* data);		// Called when mouse was previously inside the widget
using OnMouseIn = void(*)(void* data);		// Called when mouse was previously outside the widget

class Widget
{
public:
	Rect rect{};

	void SetOnClick(OnMouseClick onClick, void* data = nullptr);
	void SetOnOver(OnMouseOver onOver, void* data = nullptr);
	void SetOnOut(OnMouseOut onOut, void* data = nullptr);
	void SetOnIn(OnMouseIn onIn, void* data = nullptr);

	static void Register(Widget* widget);
	static void Unregister(Widget* widget);
	static void ClearRegistry();
	static void Update();

	virtual void Render() = 0;

	bool IsMouseOver() { return mMouseOver; }
protected:
	bool mMouseOver = false;

private:
	size_t mId = 0;

	OnMouseClick mOnMouseClick = nullptr;
	OnMouseOver mOnMouseOver = nullptr;
	OnMouseOut mOnMouseOut = nullptr;
	OnMouseIn mOnMouseIn = nullptr;

	void* mMouseClickData = nullptr;
	void* mMouseOverData = nullptr;
	void* mMouseOutData = nullptr;
	void* mMouseInData = nullptr;

	static std::vector<Widget*> sWidgets;

};

class Button : public Widget
{
public:
	Color col{ 255, 255, 255, 255 };		// White
	Color colOver{ 128, 128, 128, 255 };	// Grey

	void Render() override
	{
		if (mMouseOver)
			DrawRect(rect, colOver);
		else
			DrawRect(rect, col);
	}
};

class TexturedButton : public Button
{
public:
	SDL_Rect src{};
	SDL_Rect srcOver{};

	Texture* tex = nullptr;
	Texture* texOver = nullptr;

	bool useTint = false;

	void SetTexture(Texture* texture, SDL_Rect* source = nullptr)
	{
		src = source == nullptr ? Region(texture) : *source;
		tex = texture;
	}

	void SetOverTexture(Texture* texture, SDL_Rect* source = nullptr)
	{
		srcOver = source == nullptr ? Region(texture) : *source;
		texOver = texture;
	}

	void Render() override
	{
		assert(tex != nullptr);
		if (mMouseOver && texOver != nullptr)
		{
			if (useTint) Tint(texOver, colOver);
			DrawTexture(texOver, srcOver, rect);
		}
		else
		{
			if (useTint) Tint(tex, col);
			DrawTexture(tex, src, rect);
		}
	}

private:
	SDL_Rect Region(Texture* texture)
	{
		int w, h;
		SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
		return { 0, 0, w, h };
	}
};

class LabelledButton : public Button
{
public:
	std::string text;
	Color textColor = { 255, 255, 255, 255 };
	Font* font = nullptr;

	void Render() override
	{
		assert(font != nullptr);
		Button::Render();
		DrawTextCentered(text.c_str(), rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, font, textColor);
	}
};

class LabelledTexturedButton : public TexturedButton
{
	std::string text;
	Color textColor = { 255, 255, 255, 255 };
	Font* font = nullptr;

	void Render() override
	{
		assert(font != nullptr);
		TexturedButton::Render();
		DrawTextCentered(text.c_str(), rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f, font, textColor);
	}
};
