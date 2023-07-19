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
			Point mouse = MousePosition();
			bool over = SDL_PointInFRect(&mouse, &widget->rect);

			if (over)
			{
				// If the mouse previously wasn't over the widget, invoke mouse-in callback
				if (!widget->mMouseOver && widget->mOnMouseIn != nullptr)
					widget->mOnMouseIn(widget->mMouseInData);
			}
			else
			{
				// If the mouse previously was over the widget, invoke mouse-out callback
				if (widget->mMouseOver && widget->mOnMouseOut != nullptr)
					widget->mOnMouseOut(widget->mMouseOutData);
			}

			if (over && widget->mOnMouseOver != nullptr)
				widget->mOnMouseOver(widget->mMouseOverData);
			widget->mMouseOver = over;
		}
	}

	if (IsMouseClicked())
	{
		for (Widget* widget : sWidgets)
		{
			if (widget->mMouseOver && widget->mOnMouseClick != nullptr)
				widget->mOnMouseClick(widget->mMouseClickData);
		}
	}
}