/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * sRenderData struct - container for auxiliary data
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

#ifndef RENDER_DATA_HPP_
#define RENDER_DATA_HPP_

#include "lights.hpp"
#include "color_palette.hpp"
#include "region.hpp"
#include <QProgressBar>
#include <QStatusBar>
#include "statistics.h"
#include "rendering_configuration.hpp"

struct sRenderData
{
	sRenderData() :
			rendererID(0), stopRequest(NULL),
			lastPercentage(1.0), reduceDetail(1.0)
	{};

	int rendererID;
	cRegion<int> screenRegion;
	cRegion<double> imageRegion;
	sTextures textures;
	cColorPalette palette;
	cLights lights;
	bool *stopRequest;
	double lastPercentage;
	double reduceDetail;
	cStatistics statistics;
	QList<int> netRenderStartingPositions;
	cRenderingConfiguration configuration;
};

#endif /* RENDER_DATA_HPP_ */
