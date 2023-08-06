#include "Widgets.h"

std::vector<Widget*> Widget::sWidgets;

void Widget::SetOnClick(OnMouseClick onClick, void* data)
{
	mOnMouseClick = onClick;
	mMouseClickData = data;
}

void Widget::SetOnOver(OnMouseOver onOver, void* data)
{
	mOnMouseOver = onOver;
	mMouseOverData = data;
}

void Widget::SetOnOut(OnMouseOut onOut, void* data)
{
	mOnMouseOut = onOut;
	mMouseOutData = data;
}

void Widget::SetOnIn(OnMouseIn onIn, void* data)
{
	mOnMouseIn = onIn;
	mMouseInData = data;
}

void Widget::Register(Widget* widget)
{
	static size_t id;
	++id;
	widget->mId = id;
	sWidgets.push_back(widget);
}

// Fast iteration but slow removal due to vector. More iterations than removals so vector is ideal.
void Widget::Unregister(Widget* widget)
{
	for (size_t i = 0; i < sWidgets.size(); i++)
	{
		if (widget->mId == sWidgets[i]->mId)
		{
			sWidgets.erase(sWidgets.begin() + i);
			widget->mId = 0;
			break;
		}
	}
}

void Widget::ClearRegistry()
{
	sWidgets.clear();
}


void Widget::Update()
{
	
	if (IsMouseMoved())
	{
		for (Widget* widget : sWidgets)
		{
			Point Mouse = MousePosition();
			bool over = false;
			if (SDL_PointInFRect(&Mouse, &widget->rect))
			{
				over = true;

				if (widget->mMouseOver != over && widget->mOnMouseOver != nullptr)
					widget->mOnMouseOver(widget->mMouseOverData);

				if (!widget->mMouseOver && widget->mOnMouseIn != nullptr)
					widget->mOnMouseIn(widget->mMouseInData);
			}
			else
			{
				if (widget->mMouseOver && widget->mOnMouseOut != nullptr)
					widget->mOnMouseOut(widget->mMouseOutData);
			}

			widget->mMouseOver = over;
		}
	}
}
