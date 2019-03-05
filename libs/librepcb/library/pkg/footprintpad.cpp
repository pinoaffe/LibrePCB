/*
 * LibrePCB - Professional EDA for everyone!
 * Copyright (C) 2013 LibrePCB Developers, see AUTHORS.md for contributors.
 * https://librepcb.org/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include "footprintpad.h"

#include "footprintpadgraphicsitem.h"

#include <librepcb/common/graphics/graphicslayer.h>

#include <QtCore>

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {
namespace library {

/*******************************************************************************
 *  Constructors / Destructor
 ******************************************************************************/

FootprintPad::FootprintPad(const FootprintPad& other) noexcept
  : mPackagePadUuid(other.mPackagePadUuid),
    mPosition(other.mPosition),
    mRotation(other.mRotation),
    mShape(other.mShape),
    mWidth(other.mWidth),
    mHeight(other.mHeight),
    mDrillWidth(other.mDrillWidth),
    mDrillHeight(other.mDrillHeight),
    mBoardSide(other.mBoardSide),
    mRegisteredGraphicsItem(nullptr) {
}

FootprintPad::FootprintPad(const Uuid& padUuid, const Point& pos,
                           const Angle& rot, Shape shape,
                           const PositiveLength& width,
                           const PositiveLength& height,
                           const PositiveLength& drillWidth,
                           const PositiveLength& drillHeight,
                           BoardSide             side) noexcept
  : mPackagePadUuid(padUuid),
    mPosition(pos),
    mRotation(rot),
    mShape(shape),
    mWidth(width),
    mHeight(height),
    mDrillWidth(drillWidth),
    mDrillHeight(drillHeight),
    mBoardSide(side),
    mRegisteredGraphicsItem(nullptr) {
}

FootprintPad::FootprintPad(const SExpression& node)
  : mPackagePadUuid(node.getChildByIndex(0).getValue<Uuid>()),
    mPosition(node.getChildByPath("position")),
    mRotation(node.getValueByPath<Angle>("rotation")),
    mShape(node.getValueByPath<Shape>("shape")),
    mWidth(Point(node.getChildByPath("size")).getX()),
    mHeight(Point(node.getChildByPath("size")).getY()),
    mDrillWidth(Point::fromDrillSExpression(node).getX()),
    mDrillHeight(Point::fromDrillSExpression(node).getY()),
    mBoardSide(node.getValueByPath<BoardSide>("side")),
    mRegisteredGraphicsItem(nullptr) {
}

FootprintPad::~FootprintPad() noexcept {
  Q_ASSERT(mRegisteredGraphicsItem == nullptr);
}

/*******************************************************************************
 *  Getters
 ******************************************************************************/

QString FootprintPad::getLayerName() const noexcept {
  switch (mBoardSide) {
    case BoardSide::TOP:
      return GraphicsLayer::sTopCopper;
    case BoardSide::BOTTOM:
      return GraphicsLayer::sBotCopper;
    case BoardSide::THT:
      return GraphicsLayer::sBoardPadsTht;
    default:
      Q_ASSERT(false);
      return "";
  }
}

bool FootprintPad::isOnLayer(const QString& name) const noexcept {
  if (mBoardSide == BoardSide::THT) {
    return GraphicsLayer::isCopperLayer(name);
  } else {
    return (name == getLayerName());
  }
}

Path FootprintPad::getOutline(const Length& expansion) const noexcept {
  Length width  = mWidth + (expansion * 2);
  Length height = mHeight + (expansion * 2);
  if (width > 0 && height > 0) {
    PositiveLength pWidth(width);
    PositiveLength pHeight(height);
    switch (mShape) {
      case Shape::ROUND:
        return Path::obround(pWidth, pHeight);
      case Shape::RECT:
        return Path::centeredRect(pWidth, pHeight);
      case Shape::OCTAGON:
        return Path::octagon(pWidth, pHeight);
      default:
        Q_ASSERT(false);
        break;
    }
  }
  return Path();
}

QPainterPath FootprintPad::toQPainterPathPx(const Length& expansion) const
    noexcept {
  QPainterPath p = getOutline(expansion).toQPainterPathPx();
  if (mBoardSide == BoardSide::THT) {
    return p.subtracted(Path::obround(mDrillWidth, mDrillHeight).toQPainterPathPx());
    // TODO: check whether this is centered correctly
  }
  return p;
}

/*******************************************************************************
 *  Setters
 ******************************************************************************/
void FootprintPad::setPosition(const Point& pos) noexcept {
  mPosition = pos;
  if (mRegisteredGraphicsItem) mRegisteredGraphicsItem->setPosition(mPosition);
}

void FootprintPad::setPackagePadUuid(const Uuid& pad) noexcept {
  mPackagePadUuid = pad;
}

void FootprintPad::setRotation(const Angle& rot) noexcept {
  mRotation = rot;
  if (mRegisteredGraphicsItem) mRegisteredGraphicsItem->setRotation(mRotation);
}

void FootprintPad::setShape(Shape shape) noexcept {
  mShape = shape;
  if (mRegisteredGraphicsItem)
    mRegisteredGraphicsItem->setShape(toQPainterPathPx());
}

void FootprintPad::setWidth(const PositiveLength& width) noexcept {
  mWidth = width;
  if (mRegisteredGraphicsItem)
    mRegisteredGraphicsItem->setShape(toQPainterPathPx());
}

void FootprintPad::setHeight(const PositiveLength& height) noexcept {
  mHeight = height;
  if (mRegisteredGraphicsItem)
    mRegisteredGraphicsItem->setShape(toQPainterPathPx());
}

void FootprintPad::setDrillWidth(const PositiveLength& width) noexcept {
  mDrillWidth = width;
  if (mRegisteredGraphicsItem)
    mRegisteredGraphicsItem->setShape(toQPainterPathPx());
}

void FootprintPad::setDrillHeight(const PositiveLength& height) noexcept {
  mDrillHeight = height;
  if (mRegisteredGraphicsItem)
    mRegisteredGraphicsItem->setShape(toQPainterPathPx());
}

void FootprintPad::setBoardSide(BoardSide side) noexcept {
  mBoardSide = side;
  if (mRegisteredGraphicsItem)
    mRegisteredGraphicsItem->setLayerName(getLayerName());
  if (mRegisteredGraphicsItem)
    mRegisteredGraphicsItem->setShape(toQPainterPathPx());
}

/*******************************************************************************
 *  General Methods
 ******************************************************************************/

void FootprintPad::registerGraphicsItem(
    FootprintPadGraphicsItem& item) noexcept {
  Q_ASSERT(!mRegisteredGraphicsItem);
  mRegisteredGraphicsItem = &item;
}

void FootprintPad::unregisterGraphicsItem(
    FootprintPadGraphicsItem& item) noexcept {
  Q_ASSERT(mRegisteredGraphicsItem == &item);
  mRegisteredGraphicsItem = nullptr;
}

void FootprintPad::serialize(SExpression& root) const {
  root.appendChild(mPackagePadUuid);
  root.appendChild("side", mBoardSide, false);
  root.appendChild("shape", mShape, false);
  root.appendChild(mPosition.serializeToDomElement("position"), true);
  root.appendChild("rotation", mRotation, false);
  root.appendChild(Point(*mWidth, *mHeight).serializeToDomElement("size"),
                   false);
  root.appendChild(Point(*mDrillWidth,
                         *mDrillHeight).serializeToDomElement("drill_size"),
                   false);
}

/*******************************************************************************
 *  Operator Overloadings
 ******************************************************************************/

bool FootprintPad::operator==(const FootprintPad& rhs) const noexcept {
  if (mPackagePadUuid != rhs.mPackagePadUuid) return false;
  if (mPosition != rhs.mPosition) return false;
  if (mRotation != rhs.mRotation) return false;
  if (mShape != rhs.mShape) return false;
  if (mWidth != rhs.mWidth) return false;
  if (mHeight != rhs.mHeight) return false;
  if (mDrillWidth != rhs.mDrillWidth) return false;
  if (mDrillHeight != rhs.mDrillHeight) return false;
  if (mBoardSide != rhs.mBoardSide) return false;
  return true;
}

FootprintPad& FootprintPad::operator=(const FootprintPad& rhs) noexcept {
  mPackagePadUuid = rhs.mPackagePadUuid;
  mPosition       = rhs.mPosition;
  mRotation       = rhs.mRotation;
  mShape          = rhs.mShape;
  mWidth          = rhs.mWidth;
  mHeight         = rhs.mHeight;
  mDrillWidth     = rhs.mDrillWidth;
  mDrillHeight    = rhs.mDrillHeight;
  mBoardSide      = rhs.mBoardSide;
  return *this;
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace library
}  // namespace librepcb
