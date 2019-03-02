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

#ifndef LIBREPCB_LIBRARY_EDITOR_LIBRARYOVERVIEWWIDGET_H
#define LIBREPCB_LIBRARY_EDITOR_LIBRARYOVERVIEWWIDGET_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include "../common/editorwidgetbase.h"

#include <QtCore>
#include <QtWidgets>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {
namespace library {

class Library;

namespace editor {

class LibraryListEditorWidget;

namespace Ui {
class LibraryOverviewWidget;
}

/*******************************************************************************
 *  Class LibraryOverviewWidget
 ******************************************************************************/

/**
 * @brief The LibraryOverviewWidget class
 *
 * @author ubruhin
 * @date 2016-10-08
 */
class LibraryOverviewWidget final : public EditorWidgetBase {
  Q_OBJECT

public:
  // Constructors / Destructor
  LibraryOverviewWidget()                                   = delete;
  LibraryOverviewWidget(const LibraryOverviewWidget& other) = delete;
  LibraryOverviewWidget(const Context& context, const FilePath& fp,
                        QWidget* parent = nullptr) noexcept;
  ~LibraryOverviewWidget() noexcept;

  // Getters
  Library& getLibrary() const noexcept { return *mLibrary; }

  // Operator Overloadings
  LibraryOverviewWidget& operator=(const LibraryOverviewWidget& rhs) = delete;

public slots:
  bool save() noexcept override;

signals:
  void editComponentCategoryTriggered(const FilePath& fp);
  void editPackageCategoryTriggered(const FilePath& fp);
  void editSymbolTriggered(const FilePath& fp);
  void editPackageTriggered(const FilePath& fp);
  void editComponentTriggered(const FilePath& fp);
  void editDeviceTriggered(const FilePath& fp);
  void removeElementTriggered(const FilePath& fp);

private:  // Methods
  void    updateMetadata() noexcept;
  QString commitMetadata() noexcept;
  bool    isInterfaceBroken() const noexcept override { return false; }
  bool    runChecks(LibraryElementCheckMessageList& msgs) const override;
  template <typename MessageType>
  void fixMsg(const MessageType& msg);
  template <typename MessageType>
  bool fixMsgHelper(std::shared_ptr<const LibraryElementCheckMessage> msg,
                    bool                                              applyFix);
  bool processCheckMessage(
      std::shared_ptr<const LibraryElementCheckMessage> msg,
      bool                                              applyFix) override;
  void updateElementLists() noexcept;
  template <typename ElementType>
  void updateElementList(QListWidget& listWidget, const QIcon& icon) noexcept;
  void openContextMenuAtPos(const QPoint& pos) noexcept;
  bool removeSelectedItem(const QString&  itemName,
                          const FilePath& itemPath) noexcept;

  // Event Handlers
  void btnIconClicked() noexcept;
  void lstCmpCatDoubleClicked(const QModelIndex& index) noexcept;
  void lstPkgCatDoubleClicked(const QModelIndex& index) noexcept;
  void lstSymDoubleClicked(const QModelIndex& index) noexcept;
  void lstPkgDoubleClicked(const QModelIndex& index) noexcept;
  void lstCmpDoubleClicked(const QModelIndex& index) noexcept;
  void lstDevDoubleClicked(const QModelIndex& index) noexcept;

private:  // Data
  QScopedPointer<Ui::LibraryOverviewWidget> mUi;
  QScopedPointer<LibraryListEditorWidget>   mDependenciesEditorWidget;
  QSharedPointer<Library>                   mLibrary;
  QByteArray                                mIcon;
};

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace library
}  // namespace librepcb

#endif  // LIBREPCB_LIBRARY_EDITOR_LIBRARYOVERVIEWWIDGET_H
