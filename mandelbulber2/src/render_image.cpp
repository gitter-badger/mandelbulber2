/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cRenderer class - calculates image using multiple CPU cores and does post processing
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#include "render_image.hpp"

#include <QtCore>

#include "dof.hpp"
#include "system.hpp"
#include "render_worker.hpp"
#include "progress_text.hpp"
#include "error_message.hpp"
#include "render_ssao.h"
#include "global_data.hpp"

cRenderer::cRenderer(const cParamRender *_params, const cFourFractals *_fractal, sRenderData *_renderData, cImage *_image) : QObject()
{
	params = _params;
	fractal = _fractal;
	data = _renderData;
	image = _image;
	scheduler = NULL;
	netRemderAckReceived = true;
}

cRenderer::~cRenderer()
{
	if(scheduler) delete scheduler;
}

bool cRenderer::RenderImage()
{
	WriteLog("cRenderer::RenderImage()");

	image->SetImageParameters(params->imageAdjustments);

	int progressiveSteps;
	if(data->configuration.UseProgressive())
		progressiveSteps = (int)(log((double)max(image->GetWidth(), image->GetHeight())) / log(2.0))-3;
	else
		progressiveSteps = 0;

	if(progressiveSteps < 0) progressiveSteps = 0;
	int progressive = pow(2.0, (double)progressiveSteps - 1);
	if (progressive == 0) progressive = 1;

	//prepare multiple threads
	QThread **thread = new QThread*[data->configuration.GetNumberOfThreads()];
	cRenderWorker::sThreadData *threadData = new cRenderWorker::sThreadData[data->configuration.GetNumberOfThreads()];
	cRenderWorker **worker= new cRenderWorker*[data->configuration.GetNumberOfThreads()];

	if(scheduler) delete scheduler;
	scheduler = new cScheduler(image->GetHeight(), progressive);

	cProgressText progressText;
	progressText.ResetTimer();

	for(int i=0; i < data->configuration.GetNumberOfThreads(); i++)
	{
		threadData[i].id = i + 1;
		if(data->configuration.UseNetRender())
		{
			if(i < data->netRenderStartingPositions.size())
			{
				threadData[i].startLine = data->netRenderStartingPositions.at(i);
			}
			else
			{
				threadData[i].startLine = 0;
				qCritical() << "NetRender - Mising starting positions data";
			}
		}
		else
		{
			threadData[i].startLine = (image->GetHeight()/data->configuration.GetNumberOfThreads()) * i / scheduler->GetProgressiveStep() * scheduler->GetProgressiveStep();
		}
		threadData[i].scheduler = scheduler;
	}

	QString statusText;
	QString progressTxt;

	QElapsedTimer timerRefresh;
	timerRefresh.start();
	qint64 lastRefreshTime = 0;
	QList<int> listToRefresh;
	QList<int> listToSend;

	WriteLog("Start rendering");
	do
	{
		WriteLogDouble("Progressive loop", scheduler->GetProgressiveStep());
		for (int i = 0; i < data->configuration.GetNumberOfThreads(); i++)
		{
			WriteLog(QString("Thread ") + QString::number(i) + " create");
			thread[i] = new QThread;
			worker[i] = new cRenderWorker(params, fractal, &threadData[i], data, image); //Warning! not needed to delete object
			worker[i]->moveToThread(thread[i]);
			QObject::connect(thread[i], SIGNAL(started()), worker[i], SLOT(doWork()));
			QObject::connect(worker[i], SIGNAL(finished()), thread[i], SLOT(quit()));
			QObject::connect(worker[i], SIGNAL(finished()), worker[i], SLOT(deleteLater()));
			thread[i]->setObjectName("RenderWorker #" + QString::number(i));
			thread[i]->start();
			WriteLog(QString("Thread ") + QString::number(i) + " started");
		}

		while (!scheduler->AllLinesDone())
		{
			gApplication->processEvents();

			if (*data->stopRequest || progressText.getTime() > data->configuration.GetMaxRenderTime())
			{
				scheduler->Stop();
			}

			Wait(100); //wait 100ms

			if(data->configuration.UseRefreshRenderedList())
			{
				//get list of last rendered lines
				QList<int> list = scheduler->GetLastRenderedLines();
				//create list of lines for image refresh
				listToRefresh += list;
			}

			//status bar and progress bar
			double percentDone = scheduler->PercentDone();
			data->lastPercentage = percentDone;
			statusText = QObject::tr("Rendering image in progress");
			progressTxt = progressText.getText(percentDone);
			data->statistics.time = progressText.getTime();

			emit updateProgressAndStatus(statusText, progressTxt, percentDone);
			emit updateStatistics(data->statistics);

			//refresh image
			if (listToRefresh.size() > 0)
			{
				if (timerRefresh.elapsed() > lastRefreshTime && (scheduler->GetProgressivePass() > 1 || !data->configuration.UseProgressive()))
				{
					timerRefresh.restart();
					QSet<int> set_listToRefresh = listToRefresh.toSet(); //removing duplicates
					listToRefresh = set_listToRefresh.toList();
					qSort(listToRefresh);
					listToSend += listToRefresh;

					image->CompileImage(&listToRefresh);

					if(data->configuration.UseRenderTimeEffects())
					{
						if (params->ambientOcclusionEnabled && params->ambientOcclusionMode == params::AOmodeScreenSpace)
						{
							cRenderSSAO rendererSSAO(params, data, image);
							connect(&rendererSSAO, SIGNAL(updateProgressAndStatus(const QString&, const QString&, double)), this, SIGNAL(updateProgressAndStatus(const QString&, const QString&, double)));
							rendererSSAO.setProgressive(scheduler->GetProgressiveStep());
							rendererSSAO.RenderSSAO(&listToRefresh);
						}
					}

					image->ConvertTo8bit();

					if(data->configuration.UseImageRefresh())
					{
						image->UpdatePreview(&listToRefresh);
						image->GetImageWidget()->update();
					}

					//sending rendered lines to NetRender server
					if(data->configuration.UseNetRender() && gNetRender->IsClient() && gNetRender->GetStatus() == CNetRender::netRender_WORKING)
					{
						if(netRemderAckReceived) //is ACK was already received. Server is ready to take new data
						{
							QList<QByteArray> renderedLinesData;
							for(int i = 0; i < listToSend.size(); i++)
							{
								//avoid sending already rendered lines
								if(scheduler->IsLineDoneByServer(listToSend.at(i)))
								{
									listToSend.removeAt(i);
									i--;
									continue;
								}
								//creating data set to send
								QByteArray lineData;
								CreateLineData(listToSend.at(i), &lineData);
								renderedLinesData.append(lineData);
							}
							//sending data
							if(listToSend.size() > 0)
							{
								emit sendRenderedLines(listToSend, renderedLinesData);
								emit NotifyClientStatus();
								netRemderAckReceived = false;
								listToSend.clear();
							}
						}
					}

					if(data->configuration.UseNetRender() && gNetRender->IsServer())
					{
						QList<int> toDoList = scheduler->CreateDoneList();
						if(toDoList.size() > data->configuration.GetNumberOfThreads())
						{
							for(int c = 0; c < gNetRender->GetClientCount(); c++)
							{
								emit SendToDoList(c, toDoList);
							}
						}
					}

					lastRefreshTime = timerRefresh.elapsed() * data->configuration.GetRefreshRate() / (listToRefresh.size());

					//do not refresh and send data too often
					if(data->configuration.UseNetRender())
					{
						if(lastRefreshTime < 500) lastRefreshTime = 500;
					}

					timerRefresh.restart();
					listToRefresh.clear();
				} //timerRefresh
			} //isPreview
		}//while scheduler

		for (int i = 0; i < data->configuration.GetNumberOfThreads(); i++)
		{
			while(thread[i]->isRunning())
			{
				gApplication->processEvents();
			};
			WriteLog(QString("Thread ") + QString::number(i) + " finished");
			delete thread[i];
		}
	}
	while(scheduler->ProgressiveNextStep());

	//send last rendered lines
	if(data->configuration.UseNetRender() && gNetRender->IsClient() && gNetRender->GetStatus() == CNetRender::netRender_WORKING)
	{
		if (netRemderAckReceived)
		{
			QList<QByteArray> renderedLinesData;
			for (int i = 0; i < listToSend.size(); i++)
			{
				//avoid sending already rendered lines
				if (scheduler->IsLineDoneByServer(listToSend.at(i)))
				{
					listToSend.removeAt(i);
					i--;
					continue;
				}
				QByteArray lineData;
				CreateLineData(listToSend.at(i), &lineData);
				renderedLinesData.append(lineData);
			}

			if (listToSend.size() > 0)
			{
				emit sendRenderedLines(listToSend, renderedLinesData);
				emit NotifyClientStatus();
				netRemderAckReceived = false;
				listToSend.clear();
			}
		}
	}

	if(data->configuration.UseNetRender())
	{
		if(gNetRender->IsClient()) {
			gNetRender->SetStatus(CNetRender::netRender_READY);
			emit NotifyClientStatus();
		}

		if(gNetRender->IsServer())
		{
			emit StopAllClients();
		}
	}
	//refresh image at end
	WriteLog("image->CompileImage()");
	image->CompileImage();

	if(!(gNetRender->IsClient() && data->configuration.UseNetRender()))
	{
		if(params->ambientOcclusionEnabled && params->ambientOcclusionMode == params::AOmodeScreenSpace)
		{
			cRenderSSAO rendererSSAO(params, data, image);
			connect(&rendererSSAO, SIGNAL(updateProgressAndStatus(const QString&, const QString&, double)), this, SIGNAL(updateProgressAndStatus(const QString&, const QString&, double)));
			rendererSSAO.RenderSSAO();
		}
		if(params->DOFEnabled && !*data->stopRequest)
		{
			cPostRenderingDOF dof(image);
			connect(&dof, SIGNAL(updateProgressAndStatus(const QString&, const QString&, double)), this, SIGNAL(updateProgressAndStatus(const QString&, const QString&, double)));
			dof.Render(params->DOFRadius * (image->GetWidth() + image->GetHeight()) / 2000.0, params->DOFFocus, data->stopRequest);
		}
	}

	if(image->IsPreview())
	{
		WriteLog("image->ConvertTo8bit()");
		image->ConvertTo8bit();
		WriteLog("image->UpdatePreview()");
		image->UpdatePreview();
		WriteLog("image->GetImageWidget()->update()");
		image->GetImageWidget()->update();
	}

	WriteLog("Rendering finished");

	//status bar and progress bar
	double percentDone = 1.0;
	statusText = QObject::tr("Idle");
	progressTxt = progressText.getText(percentDone);

	//update histograms
	data->statistics.time = progressText.getTime();
	emit updateStatistics(data->statistics);
	emit updateProgressAndStatus(statusText, progressTxt, percentDone);

	delete[] thread;
	delete[] threadData;
	delete[] worker;

	WriteLog("cRenderer::RenderImage(): memory released");

	if (*data->stopRequest)
		return false;
	else
		return true;
}

void cRenderer::CreateLineData(int y, QByteArray *lineData)
{
	if (y >= 0 && y < image->GetHeight())
	{
		int width = image->GetWidth();
		sAllImageData *lineOfImage = new sAllImageData[width];
		size_t dataSize = sizeof(sAllImageData) * width;
		for (int x = 0; x < width; x++)
		{
			lineOfImage[x].imageFloat = image->GetPixelImage(x, y);
			lineOfImage[x].alphaBuffer = image->GetPixelAlpha(x, y);
			lineOfImage[x].colourBuffer = image->GetPixelColor(x, y);
			lineOfImage[x].zBuffer = image->GetPixelZBuffer(x, y);
			lineOfImage[x].opacityBuffer = image->GetPixelOpacity(x, y);
		}
		lineData->append((char*)lineOfImage, dataSize);
		delete[] lineOfImage;
	}
	else
	{
		qCritical() << "cRenderer::CreateLineData(int y, QByteArray *lineData): wrong line:" << y;
	}
}

void cRenderer::NewLinesArrived(QList<int> lineNumbers, QList<QByteArray> lines)
{
	for(int i = 0; i < lineNumbers.size(); i++)
	{
		int y = lineNumbers.at(i);
		if (y >= 0 && y < image->GetHeight())
		{
			sAllImageData *lineOfImage = (sAllImageData *)lines.at(i).data();
			int width = image->GetWidth();
			for (int x = 0; x < width; x++)
			{
				image->PutPixelImage(x, y, lineOfImage[x].imageFloat);
				image->PutPixelAlpha(x, y, lineOfImage[x].alphaBuffer);
				image->PutPixelColour(x, y, lineOfImage[x].colourBuffer);
				image->PutPixelZBuffer(x, y, lineOfImage[x].zBuffer);
				image->PutPixelOpacity(x, y, lineOfImage[x].opacityBuffer);
			}
		}
		else
		{
			qCritical() << "cRenderer::NewLinesArrived(QList<int> lineNumbers, QList<QByteArray> lines): wrong line number:" << y;
			return;
		}
	}

	scheduler->MarkReceivedLines(lineNumbers);
}

void cRenderer::ToDoListArrived(QList<int> toDo)
{
	scheduler->UpdateDoneLines(toDo);
}

void cRenderer::AckReceived()
{
	netRemderAckReceived = true;
}
