/*
	Copyright 2006-2025 The QElectroTech Team
	This file is part of QElectroTech.
	
	QElectroTech is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.
	
	QElectroTech is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with QElectroTech. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef AUTOSAVEFILE_H
#define AUTOSAVEFILE_H

#include <QList>
#include <QFile>
#include <QUrl>

/**
	@brief The AutoSaveFile class
*/
class AutoSaveFile : public QFile
{
	public:
    	AutoSaveFile();
		~AutoSaveFile();

		const QUrl managedFile();
		void setManagedFile(const QUrl &);
		static QList<AutoSaveFile*>allStaleFiles();

	private:
		QUrl m_managed_file;
};

#endif
