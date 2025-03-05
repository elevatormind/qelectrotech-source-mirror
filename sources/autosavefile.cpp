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

#include <QDebug>

/**
	@brief AutoSaveFile::AutoSaveFile
	Simple constructor
*/
AutoSaveFile::AutoSaveFile()
{
	qDebug() << "Creating AutoSaveFile";
}

/**
	@brief AutoSaveFile::~AutoSaveFile
	Simple destructor
*/
AutoSaveFile::~AutoSaveFile()
{
	qDebug() << "Destroying AutoSaveFile for file: " << m_managed_file.toString();
}

/**
	@brief AutoSaveFile::managedFile
	@return the managed file
*/
const QUrl AutoSaveFile::managedFile() {
	return m_managed_file;
}

/**
	@brief AutoSaveFile::setManagedFile
	@param file the managed file
*/

void AutoSaveFile::setManagedFile(const QUrl &file) {
	m_managed_file = file;
	qDebug() << "Setting AutoSaveFile managed file: " << m_managed_file.toString();;
}

/**
	@brief AutoSaveFile::allStaleFiles
	@return the list of all stale files
*/
QList<AutoSaveFile*> AutoSaveFile::allStaleFiles() {
	QList<AutoSaveFile*> list;
	return list;
}
