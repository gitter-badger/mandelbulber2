/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * functions for calculation single fractal iterations
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Graeme McLaren
 */

#ifndef FRACTAL_FORMULAS_HPP_
#define FRACTAL_FORMULAS_HPP_

#include "four_fractals.hpp"
#include "commonparams.hpp"



void AexionIteration(CVector3 &z, double &w, const cFractal *fractal, sAexionAux &aux);
void BenesiIteration(CVector3 &z, CVector3 &c);
void BoxFoldBulbPow2Iteration(CVector3 &z, const cFractal *fractal);
void BristorbrotIteration(CVector3 &z);
void BuffaloIteration(CVector3 &z, const cFractal *fractal);
void GeneralizedFoldBoxIteration(CVector3 &z, const cFractal *fractal, sMandelboxAux &aux);
void HypercomplexIteration(CVector3 &z, double &w);
void Ides2Iteration(CVector3 &z, CVector3 &c);
void IdesIteration(CVector3 &z);
void KaleidoscopicIFSIteration(CVector3 &z, const cFractal *fractal, sIFSAux &aux);
void LkmitchIteration(CVector3 &z);
void Makin3D2Iteration(CVector3 &z);
void Mandelbox103Iteration(CVector3 &z, CVector3 &c, double minimumR, int &i, const cFractal *fractal, sExtendedAux &aux);
void MandelboxIteration(CVector3 &z, const cFractal *fractal, sMandelboxAux &aux);
void MandelboxVaryScale4DIteration(CVector3 &z, double &w, const cFractal *fractal, sMandelboxAux &aux);
void Mandelbulb2Iteration(CVector3 &z, sMandelbulbAux &aux);
void Mandelbulb3Iteration(CVector3 &z, sMandelbulbAux &aux);
void Mandelbulb4Iteration(CVector3 &z, const cFractal *fractal, sMandelbulbAux &aux);
void Mandelbulb5Iteration(CVector3 &z, CVector3 &c, double minimumR, int &i, const cFractal *fractal, sExtendedAux &aux);
void MandelbulbIteration(CVector3 &z, const cFractal *fractal, sMandelbulbAux &aux);
void MandelbulbPower2Iteration(CVector3 &z);
void MengerSponge105Iteration(CVector3 &z, CVector3 &c,double minimumR, int &i, const cFractal *fractal, sExtendedAux &aux);
void MengerSpongeIteration(CVector3 &z, sIFSAux &aux);
void MsltoeSym2(CVector3 &z, const cFractal *fractal);
void PlatonicSolidIteration(CVector3 &z, const cFractal *fractal);
void Quaternion104Iteration(CVector4 &z, const CVector4 &c, int &i, const cFractal *fractal, sExtendedAux &aux);
void QuaternionIteration(CVector3 &z, double &w);
void QuickDudleyIteration(CVector3 &z);
void SmoothMandelboxIteration(CVector3 &z, const cFractal *fractal, sMandelboxAux &aux);
void XenodreambuieIteration(CVector3 &z, const cFractal *fractal, sMandelbulbAux &aux);

void BoxFolding(CVector3 &z, const sFractalFoldings *foldings, double &foldColor);
void SphericalFolding(CVector3 &z, const sFractalFoldings *foldings, double &foldColor, double &foldDE, double r);

void Mandelbulb6BetaIteration(CVector3 &z, CVector3 &c, double minimumR, int &i, const cFractal *fractal, sExtendedAux &aux);
void BenesiTransformsIteration(CVector3 &z, CVector3 &c, double minimumR, int &i, const cFractal *fractal, sExtendedAux &aux);



#endif /* FRACTAL_FORMULAS_HPP_ */
