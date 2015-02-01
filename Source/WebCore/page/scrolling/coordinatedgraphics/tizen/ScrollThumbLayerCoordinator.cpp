/*
   Copyright (C) 2014 Samsung Electronics

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"

#if ENABLE(TIZEN_OVERFLOW_SCROLLBAR)

#include "ScrollThumbLayerCoordinator.h"

#include "GraphicsLayer.h"
#include "ScrollableArea.h"
#include "Scrollbar.h"
#include "ScrollbarThemeTizen.h"

namespace WebCore {

ScrollThumbLayerCoodinator::~ScrollThumbLayerCoodinator()
{
    m_horizontalScrollThumbs.clear();
    m_verticalScrollThumbs.clear();
}

void ScrollThumbLayerCoodinator::createScrollThumbLayer(GraphicsLayerFactory* factory, ScrollableArea* scrollableArea, GraphicsLayer* scrollbarLayer, ScrollbarOrientation orientation)
{
    OwnPtr<GraphicsLayer> scrollThumbLayer = GraphicsLayer::create(factory, scrollbarLayer->client());

    scrollbarLayer->addChild(scrollThumbLayer.get());

    switch (orientation) {
    case HorizontalScrollbar:
        m_horizontalScrollThumbs.add(scrollableArea, scrollThumbLayer.release());
        break;
    case VerticalScrollbar:
        m_verticalScrollThumbs.add(scrollableArea, scrollThumbLayer.release());
        break;
    }
}

void ScrollThumbLayerCoodinator::scrollBarLayerDestroyed(ScrollableArea* scrollableArea, ScrollbarOrientation orientation)
{
    switch (orientation) {
    case HorizontalScrollbar:
        m_horizontalScrollThumbs.remove(scrollableArea);
        break;
    case VerticalScrollbar:
        m_verticalScrollThumbs.remove(scrollableArea);
        break;
    }
}

GraphicsLayer* ScrollThumbLayerCoodinator::updateScrollThumbLayer(ScrollableArea* scrollableArea, ScrollbarOrientation orientation)
{
    GraphicsLayer* scrollThumbLayer = 0;
    Scrollbar* bar = 0;

    switch (orientation) {
    case HorizontalScrollbar:
        ASSERT(m_horizontalScrollThumbs.contains(scrollableArea));
        scrollThumbLayer = m_horizontalScrollThumbs.get(scrollableArea);
        bar = scrollableArea->horizontalScrollbar();
        break;

    case VerticalScrollbar:
        ASSERT(m_verticalScrollThumbs.contains(scrollableArea));
        scrollThumbLayer = m_verticalScrollThumbs.get(scrollableArea);
        bar = scrollableArea->verticalScrollbar();
        break;
    }
    ASSERT(bar);

    ScrollbarThemeTizen* theme = static_cast<ScrollbarThemeTizen*>(bar->theme());
    ASSERT(theme);

    FloatPoint thumbPosition; // = FloatPoint(0, theme->thumbPosition(bar)) + theme->thumbPositionOffset();
    if (orientation == HorizontalScrollbar)
        thumbPosition.setX(theme->thumbPosition(bar));
    else
        thumbPosition.setY(theme->thumbPosition(bar));
    thumbPosition.move(theme->thumbPositionOffset());

    IntRect thumbRect = theme->thumbRect(bar);
    thumbRect.contract(theme->thumbSizeOffset());

    scrollThumbLayer->setPosition(thumbPosition);
    scrollThumbLayer->setSize(thumbRect.size());
    scrollThumbLayer->setContentsRect(IntRect(0, 0, thumbRect.width(), thumbRect.height()));
    scrollThumbLayer->setContentsToSolidColor(theme->thumbColor());
}

} // namespace WebCore

#endif // ENABLE(TIZEN_OVERFLOW_SCROLLBAR)
