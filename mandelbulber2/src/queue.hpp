/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cQueue - class to manage rendering queue
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Sebastian Jennen
 */


#ifndef MANDELBULBER2_SRC_QUEUE_HPP_
#define MANDELBULBER2_SRC_QUEUE_HPP_

#include "parameters.hpp"
#include "fractal_container.hpp"
#include "animation_keyframes.hpp"
#include "animation_flight.hpp"
#include <QtCore>

class cQueue : public QObject
{
	Q_OBJECT
public:
	enum enumRenderType {
		queue_STILL, queue_FLIGHT, queue_KEYFRAME
	};

	struct structQueueItem {
		structQueueItem(
			enumRenderType _renderType,
			QString _filename):
			renderType(_renderType), filename(_filename) {}

		enumRenderType renderType;
		QString filename;
	};

	cQueue(const QString &_queueListFileName, const QString &_queueFolder); //initializes queue and create necessary files and folders
	~cQueue();

	//add settings to queue
	void Append(const QString &filename, enumRenderType renderType = queue_STILL);
	void Append(enumRenderType renderType = queue_STILL);
	void Append(cParameterContainer *par, cFractalContainer *fractPar, cAnimationFrames *frames, cKeyframes *keyframes, enumRenderType renderType = queue_STILL);

	//get next queue element into given containers
	bool Get();
	bool Get(cParameterContainer *par, cFractalContainer *fractPar, cAnimationFrames *frames, cKeyframes *keyframes);

	// syncing methods
	QStringList RemoveOrphanedFiles(); //find and delete files which are not on the list
	QStringList AddOrphanedFilesToList(); //add orphaned files from queue folder to the end of the list

	QList<structQueueItem> GetListFromQueueFile(){ return queueListFromFile; } //returns list of fractals to render from queue file
	QStringList GetListFromFileSystem(){ return queueListFileSystem; } //returns list of fractals to render from file system
	QString GetQueueFolder(){ return queueFolder; }

	//get the queue type enum from qstring value
	static enumRenderType GetTypeEnum(const QString &queueText);
	//get the queue type QString from enum value
	static QString GetTypeText(enumRenderType queueType);
	//get a color for enum value
	static QString GetTypeColor(enumRenderType queueType);

	void RemoveQueueItem(int i);//remove queue item which is i'th element of list
	void RemoveQueueItem(const QString &filename, enumRenderType renderType = queue_STILL); //remove queue item from list and filesystem
	void UpdateQueueItemType(int i, enumRenderType renderType);

signals:
	//request to update table of queue items
	void queueChanged();
	void queueChanged(int i);
	void queueChanged(int i, int j);

private slots:
	void queueFileChanged(const QString &path);
	void queueFolderChanged(const QString &path);

private:
	structQueueItem GetNextFromList(); //gives next filename
	void AddToList(const QString &filename, enumRenderType renderType = queue_STILL); //add filename to the end of list

	void RemoveFromList(const QString &filename, enumRenderType renderType = queue_STILL); //remove queue item if it is on the list
	void RemoveFromFileSystem(const QString &filename); //remove queue file from filesystem

	bool ValidateEntry(const QString &filename);

	void UpdateListFromQueueFile(); //updates the list of fractals to render from queue file
	void UpdateListFromFileSystem(); //updates the list of fractals to render from file system

	QFileSystemWatcher queueFileWatcher;
	QFileSystemWatcher queueFolderWatcher;

	QList<structQueueItem> queueListFromFile;
	QStringList queueListFileSystem;

	QString queueListFileName;
	QString queueFolder;
};

#endif /* MANDELBULBER2_SRC_QUEUE_HPP_ */