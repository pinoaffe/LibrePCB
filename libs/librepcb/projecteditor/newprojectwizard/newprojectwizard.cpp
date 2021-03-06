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
#include "newprojectwizard.h"

#include "newprojectwizardpage_initialization.h"
#include "newprojectwizardpage_metadata.h"
#include "ui_newprojectwizard.h"

#include <librepcb/common/application.h>
#include <librepcb/common/fileio/fileutils.h>
#include <librepcb/project/metadata/projectmetadata.h>
#include <librepcb/project/project.h>
#include <librepcb/project/settings/projectsettings.h>
#include <librepcb/workspace/settings/workspacesettings.h>
#include <librepcb/workspace/workspace.h>

#include <QtCore>
#include <QtWidgets>
//#include "newprojectwizardpage_versioncontrol.h"

/*******************************************************************************
 *  Namespace
 ******************************************************************************/
namespace librepcb {
namespace project {
namespace editor {

/*******************************************************************************
 *  Constructors / Destructor
 ******************************************************************************/

NewProjectWizard::NewProjectWizard(const workspace::Workspace& ws,
                                   QWidget*                    parent) noexcept
  : QWizard(parent), mWorkspace(ws), mUi(new Ui::NewProjectWizard) {
  mUi->setupUi(this);

  addPage(mPageMetadata = new NewProjectWizardPage_Metadata(mWorkspace, this));
  addPage(mPageInitialization = new NewProjectWizardPage_Initialization(this));
  // addPage(mPageVersionControl = new
  // NewProjectWizardPage_VersionControl(this));
}

NewProjectWizard::~NewProjectWizard() noexcept {
}

/*******************************************************************************
 *  Setters
 ******************************************************************************/

void NewProjectWizard::setLocation(const FilePath& dir) noexcept {
  mPageMetadata->setDefaultLocation(dir);
}

/*******************************************************************************
 *  General Methods
 ******************************************************************************/

Project* NewProjectWizard::createProject() const {
  // create project and set some metadata
  FilePath                projectFilePath = mPageMetadata->getFullFilePath();
  QScopedPointer<Project> project(Project::create(projectFilePath));
  project->getMetadata().setName(
      ElementName(mPageMetadata->getProjectName().trimmed()));  // can throw
  project->getMetadata().setAuthor(mPageMetadata->getProjectAuthor());

  // set project settings (copy from workspace settings)
  ProjectSettings& settings = project->getSettings();
  settings.setLocaleOrder(
      mWorkspace.getSettings().getLibLocaleOrder().getLocaleOrder());
  settings.setNormOrder(
      mWorkspace.getSettings().getLibNormOrder().getNormOrder());

  // add schematic
  if (mPageInitialization->getCreateSchematic()) {
    Schematic* schematic = project->createSchematic(
        ElementName(mPageInitialization->getSchematicName()));  // can throw
    project->addSchematic(*schematic);
  }

  // add board
  if (mPageInitialization->getCreateBoard()) {
    Board* board = project->createBoard(
        ElementName(mPageInitialization->getBoardName()));  // can throw
    project->addBoard(*board);
  }

  // save project to filesystem
  project->save(true);

  // copy license file
  if (mPageMetadata->isLicenseSet()) {
    try {
      FilePath source = mPageMetadata->getProjectLicenseFilePath();
      FilePath destination =
          projectFilePath.getParentDir().getPathTo("LICENSE.txt");
      FileUtils::copyFile(source, destination);  // can throw
    } catch (Exception& e) {
      qCritical() << "Could not copy the license file:" << e.getMsg();
    }
  }

  // copy readme file
  try {
    FilePath source =
        qApp->getResourcesDir().getPathTo("project/readme_template");
    FilePath destination =
        projectFilePath.getParentDir().getPathTo("README.md");
    QByteArray content = FileUtils::readFile(source);  // can throw
    content.replace("{PROJECT_NAME}", mPageMetadata->getProjectName().toUtf8());
    if (mPageMetadata->isLicenseSet()) {
      content.replace("{LICENSE_TEXT}", "See [LICENSE.txt](LICENSE.txt).");
    } else {
      content.replace("{LICENSE_TEXT}", "No license set.");
    }
    FileUtils::writeFile(destination, content);  // can throw
  } catch (Exception& e) {
    qCritical() << "Could not copy the readme file:" << e.getMsg();
  }

  // initialize git repository
  // if (mPageVersionControl->getInitGitRepository()) {
  // copy .gitignore
  try {
    FilePath source =
        qApp->getResourcesDir().getPathTo("project/gitignore_template");
    FilePath destination =
        projectFilePath.getParentDir().getPathTo(".gitignore");
    FileUtils::copyFile(source, destination);  // can throw
  } catch (Exception& e) {
    qCritical() << "Could not copy the .gitignore file:" << e.getMsg();
  }
  // copy .gitattributes
  try {
    FilePath source =
        qApp->getResourcesDir().getPathTo("project/gitattributes_template");
    FilePath destination =
        projectFilePath.getParentDir().getPathTo(".gitattributes");
    FileUtils::copyFile(source, destination);  // can throw
  } catch (Exception& e) {
    qCritical() << "Could not copy the .gitattributes file:" << e.getMsg();
  }
  // TODO: git init
  // TODO: create initial commit
  //}

  // all done, return the new project
  return project.take();
}

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace editor
}  // namespace project
}  // namespace librepcb
