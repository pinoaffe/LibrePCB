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
#include "excellongenerator.h"

#include "../fileio/fileutils.h"

#include <QtCore>

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {

/*******************************************************************************
 *  Constructors / Destructor
 ******************************************************************************/

ExcellonGenerator::ExcellonGenerator() noexcept : mOutput() {
}

ExcellonGenerator::~ExcellonGenerator() noexcept {
}

/*******************************************************************************
 *  General Methods
 ******************************************************************************/

void ExcellonGenerator::drill(const Point&          pos,
                              const PositiveLength& dia) noexcept {
  mDrillList.insert(dia, pos);
  if (!mToolList.contains(dia)) mToolList.append(dia);
}

void ExcellonGenerator::slot(const Point& start, const Point& end,
                             const PositiveLength& width) noexcept {
  mSlotList.insert(width, qMakePair(start, end));
  if (!mToolList.contains(width)) mToolList.append(width);
}

void ExcellonGenerator::generate() {
  std::sort(mToolList.begin(), mToolList.end());
  mOutput.clear();
  printHeader();
  printDrills();
  printFooter();
}

void ExcellonGenerator::saveToFile(const FilePath& filepath) const {
  FileUtils::writeFile(filepath, mOutput.toLatin1());  // can throw
}

void ExcellonGenerator::reset() noexcept {
  mOutput.clear();
  mDrillList.clear();
  mSlotList.clear();
  mToolList.clear();
}

/*******************************************************************************
 *  Private Methods
 ******************************************************************************/

void ExcellonGenerator::printHeader() noexcept {
  mOutput.append("M48\n");  // Beginning of Part Program Header

  // Comments
  mOutput.append(";DRILL FILE\n");
  mOutput.append(
      QString(";Generated by LibrePCB %1\n").arg(qApp->applicationVersion()));
  mOutput.append(QString(";Creation Date: %1\n")
                     .arg(QDateTime::currentDateTime().toString(Qt::ISODate)));
  mOutput.append("FMAT,2\n");     // Use Format 2 commands
  mOutput.append("METRIC,TZ\n");  // Metric Format, Trailing Zeros Mode

  printToolList();

  mOutput.append("%\n");    // Beginning of Pattern
  mOutput.append("G90\n");  // Absolute Mode
  mOutput.append("G05\n");  // Drill Mode
  mOutput.append("M71\n");  // Metric Measuring Mode
}

void ExcellonGenerator::printToolList() noexcept {
  for (int i = 0; i < mToolList.count(); ++i) {
    mOutput.append(
        QString("T%1C%2\n").arg(i + 1).arg(mToolList[i]->toMmString()));
  }
}

void ExcellonGenerator::printDrills() noexcept {
  for (int i = 0; i < mToolList.count(); ++i) {
    mOutput.append(QString("T%1\n").arg(i + 1));  // Select Tool

    // Circular drills
    foreach (const Point& pos, mDrillList.values(mToolList[i])) {
      mOutput.append(
          QString("X%1Y%2\n")
              .arg(pos.getX().toMmString(), pos.getY().toMmString()));
    }

    // Drilled Slots
    typedef QPair<Point, Point> qpair_point_point;
    foreach (const qpair_point_point& points, mSlotList.values(mToolList[i])) {
      mOutput.append(QString("X%1Y%2G85X%3Y%4\n")
                         .arg(points.first.getX().toMmString(),
                              points.first.getY().toMmString(),
                              points.second.getX().toMmString(),
                              points.second.getY().toMmString()));
    }
  }
}

void ExcellonGenerator::printFooter() noexcept {
  mOutput.append("T0\n");
  mOutput.append("M30\n");  // End of Program Rewind
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace librepcb
