/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * Global variables
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


#ifndef SRC_GLOBAL_DATA_HPP_
#define SRC_GLOBAL_DATA_HPP_

#include "parameters.hpp"
#include "fractal_container.hpp"
#include "animation_frames.hpp"
#include "animation_flight.hpp"
#include "animation_keyframes.hpp"
#include "keyframes.hpp"
#include "interface.hpp"
#include "netrender.hpp"
#include "queue.hpp"
#include "error_message.hpp"

extern cParameterContainer *gPar;
extern cFractalContainer *gParFractal;
extern cAnimationFrames *gAnimFrames;
extern cKeyframes *gKeyframes;
extern cInterface *gMainInterface;
extern cFlightAnimation *gFlightAnimation;
extern cKeyframeAnimation *gKeyframeAnimation;
extern QApplication *gApplication;
extern CNetRender *gNetRender;
extern cQueue *gQueue;
extern cErrorMessage *gErrorMessage;

#endif /* SRC_GLOBAL_DATA_HPP_ */
