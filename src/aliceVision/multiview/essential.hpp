// This file is part of the AliceVision project and is made available under
// the terms of the MPL2 license (see the COPYING.md file).

#ifndef ALICEVISION_MULTIVIEW_ESSENTIAL_H_
#define ALICEVISION_MULTIVIEW_ESSENTIAL_H_

#include <vector>

#include "aliceVision/numeric/numeric.hpp"

namespace aliceVision {

// Compute the relative camera motion between two cameras.
// Given the motion parameters of two cameras, computes the motion parameters
// of the second one assuming the first one to be at the origin.
// If T1 and T2 are the camera motions, the computed relative motion is
//
//      T = T2 T1^{-1}
//
void RelativeCameraMotion(const Mat3 &R1,
                          const Vec3 &t1,
                          const Mat3 &R2,
                          const Vec3 &t2,
                          Mat3 *R,
                          Vec3 *t);

/// Given F, Left/Right K matrix it compute the Essential matrix
void EssentialFromFundamental(const Mat3 &F,
                              const Mat3 &K1,
                              const Mat3 &K2,
                              Mat3 *E);

/// Compute E as E = [t12]x R12.
void EssentialFromRt(const Mat3 &R1,
                     const Vec3 &t1,
                     const Mat3 &R2,
                     const Vec3 &t2,
                     Mat3 *E);

/// Given E, Left/Right K matrix it compute the Fundamental matrix
void FundamentalFromEssential(const Mat3 &E,
                              const Mat3 &K1,
                              const Mat3 &K2,
                              Mat3 *F);

/// Test the possible R|t configuration to have point in front of the cameras
/// Return false if no possible configuration
bool MotionFromEssentialAndCorrespondence(const Mat3 &E,
                                          const Mat3 &K1,
                                          const Vec2 &x1,
                                          const Mat3 &K2,
                                          const Vec2 &x2,
                                          Mat3 *R,
                                          Vec3 *t);

/// Choose one of the four possible motion solutions from an essential matrix.
/// Decides the right solution by checking that the triangulation of a match
/// x1--x2 lies in front of the cameras.
/// Return the index of the right solution or -1 if no solution.
int MotionFromEssentialChooseSolution(const std::vector<Mat3> &Rs,
                                      const std::vector<Vec3> &ts,
                                      const Mat3 &K1,
                                      const Vec2 &x1,
                                      const Mat3 &K2,
                                      const Vec2 &x2);

// HZ 9.7 page 259 (Result 9.19)
void MotionFromEssential(const Mat3 &E,
  std::vector<Mat3> *Rs,
  std::vector<Vec3> *ts);


} // namespace aliceVision

#endif  // ALICEVISION_MULTIVIEW_ESSENTIAL_H_
