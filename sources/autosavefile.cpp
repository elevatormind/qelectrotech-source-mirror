/*
	Copyright 2006-2025 The QElectroTech Team
	This file is part of QElectroTech.
	
	QElectroTech is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.
	
	QElectroTech is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with QElectroTech.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "autosavefile.h"

#include "qetapp.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QSaveFile>

/**
 * @brief Constructor for AutoSaveFile.
 * 
 * Creates a new autosave file in the application's data location.
 * The file is opened in ReadWrite mode.
 */
AutoSaveFile::AutoSaveFile()
{
    // Create autosave file in app data directory with unique name
	setFileTemplate(QETApp::autosaveDir() % QDir::separator() % AUTOSAVE_FILETEMPLATE_BASE % AUTOSAVE_FILETEMPLATE_RANDOM);
	QDir().mkpath(fileTemplate().left(fileTemplate().lastIndexOf(QDir::separator())));

	const auto status = open(QIODeviceBase::ReadWrite);
	if(status == false)
		qWarning() << "Failed to open AutoSaveFile";
	else
		qDebug() << "Creating AutoSaveFile: " << fileName();
}

/**
 * @brief Destructor for AutoSaveFile.
 * 
 * Cleans up the managed file pointer before destruction.
 */
AutoSaveFile::~AutoSaveFile()
{
	auto file = QFile(fileName() % AUTOSAVE_MANAGED_FILE_SUFFIX);
	if(file.exists())
		file.remove();

	qDebug() << "Destroying AutoSaveFile for file: " << m_managed_file.toString();
}

/**
 * @brief Gets the URL of the file being managed.
 * @return The URL of the managed file.
 */
const QUrl AutoSaveFile::managedFile() const {
	return m_managed_file;
}

/**
 * @brief Sets the file to be managed by this autosave instance.
 * @param file The URL of the file to manage.
 */
void AutoSaveFile::setManagedFile(const QUrl &file) {
	m_managed_file = file;

	createManagedFilePointer();

	qDebug() << "Setting AutoSaveFile managed file: " << m_managed_file.toString();;
}

/**
 * @brief Finds all stale autosave files in the application's data directory.
 * 
 * Scans for autosave files that weren't properly cleaned up and returns them
 * as AutoSaveFile instances. Also performs cleanup of invalid files.
 * 
 * @return List of AutoSaveFile instances for stale files.
 */
QList<AutoSaveFile*> AutoSaveFile::allStaleFiles() {
	QList<AutoSaveFile*> list;

	auto asf_dir = QDir(QETApp::autosaveDir());

	if(!asf_dir.exists())
		return list;

	// First pass: Clean up orphaned pointer files
	for(const auto &ptr_filename : asf_dir.entryList(QStringList((QString)"*" % AUTOSAVE_MANAGED_FILE_SUFFIX), QDir::Files, QDir::Unsorted)) {
		auto ptr_file = QFile(asf_dir.absoluteFilePath(ptr_filename));
		const auto asf_filename = ptr_filename.left(ptr_filename.lastIndexOf("."));
		auto asf_file = QFile(asf_dir.absoluteFilePath(asf_filename));

		if(asf_file.exists() && asf_file.size() == 0)
			asf_file.remove();

		if(!asf_file.exists()) {
			qWarning() << "Removing stale pointer file: " << ptr_file.fileName();

			ptr_file.remove();
			continue;
		}
	}

	// Second pass: Recover valid autosave files
	for(const auto &asf_filename : asf_dir.entryList(QStringList((QString)AUTOSAVE_FILETEMPLATE_BASE % "*"), QDir::Files, QDir::Unsorted)) {
		// Read and remove the autosave file
		auto asf_file = QFile(asf_dir.absoluteFilePath(asf_filename));
		asf_file.open(QIODeviceBase::ReadOnly);
		QByteArray data = asf_file.readAll();
		asf_file.close();
		asf_file.remove();

		// Skip if pointer file is missing or empty
		auto ptr_file = QFile(asf_dir.absoluteFilePath(asf_filename % AUTOSAVE_MANAGED_FILE_SUFFIX));
		if (!ptr_file.exists() || ptr_file.size() == 0) {
			qWarning() << "Removing stale AutoSaveFile without pointer file: " << asf_filename;
			continue;
		}

		// Create new autosave instance and restore content
		auto asf = new AutoSaveFile();
		asf->write(data);

		ptr_file.open(QIODeviceBase::ReadOnly);
		data = ptr_file.readAll();
		ptr_file.close();
		asf->setManagedFile(QUrl(QString::fromUtf8(data)));

		qDebug() << "Found stale AutoSaveFile: " << asf->fileName() << " -> " << asf->managedFile().toString();

		list.append(asf);
	}


	return list;
}

/**
 * @brief Opens the autosave file.
 * 
 * Opens the temporary file and creates a pointer file to track the managed file.
 * 
 * @param mode The mode to open the file in.
 * @return true if the file was successfully opened, false otherwise.
 */
bool AutoSaveFile::open(QIODeviceBase::OpenMode mode) {
	const auto status = QTemporaryFile::open(mode);

	createManagedFilePointer();

	return status;
}

/**
 * @brief Creates a pointer file to track the managed file.
 * 
 * Creates a separate file that stores the URL of the managed file.
 * The pointer file has the same name as the autosave file with the define
 * AUTOSAVE_MANAGED_FILE_SUFFIX appended.
 * Does nothing if either the managed file URL or the autosave filename is empty.
 */
void AutoSaveFile::createManagedFilePointer() {
    // Skip if we don't have valid file info
	if (m_managed_file.isEmpty() || fileName().isEmpty())
		return;

    // Create pointer file with atomic save operation
	auto file = QSaveFile(fileName() % AUTOSAVE_MANAGED_FILE_SUFFIX);
	file.open(QIODeviceBase::WriteOnly);
	file.write(m_managed_file.toString().toUtf8());
	file.commit();

	qDebug() << "Creating managed file pointer: " << file.fileName() << " -> " << m_managed_file.toString();
}
