// This file is part of the AliceVision project and is made available under
// the terms of the MPL2 license (see the COPYING.md file).

#pragma once

#include "aliceVision/numeric/numeric.hpp"
#include "aliceVision/matching/IndMatch.hpp"

#include <vector>

namespace aliceVision {
namespace matching {


template < typename Scalar, typename Metric >
class ArrayMatcher
{
  public:
  typedef Scalar ScalarT;
  typedef typename Metric::ResultType DistanceType;
  typedef Metric MetricT;

  ArrayMatcher() {}
  virtual ~ArrayMatcher() {};

  /**
   * Build the matching structure
   *
   * \param[in] dataset   Input data.
   * \param[in] nbRows    The number of component.
   * \param[in] dimension Length of the data contained in the dataset.
   *
   * \return True if success.
   */
  virtual bool Build( const Scalar * dataset, int nbRows, int dimension)=0;

  /**
   * Search the nearest Neighbor of the scalar array query.
   *
   * \param[in]   query     The query array
   * \param[out]  indice    The indice of array in the dataset that
   *  have been computed as the nearest array.
   * \param[out]  distance  The distance between the two arrays.
   *
   * \return True if success.
   */
  virtual bool SearchNeighbour( const Scalar * query,
                                int * indice, DistanceType * distance)=0;


/**
   * Search the N nearest Neighbor of the scalar array query.
   *
   * \param[in]   query     The query array
   * \param[in]   nbQuery   The number of query rows
   * \param[out]  indices   The corresponding (query, neighbor) indices
   * \param[out]  distances The distances between the matched arrays.
   * \param[out]  NN        The number of maximal neighbor that could
   *  will be searched.
   *
   * \return True if success.
   */
  virtual bool SearchNeighbours( const Scalar * query, int nbQuery,
                                  IndMatches * indices,
                                  std::vector<DistanceType> * distances,
                                  size_t NN)=0;
};

}  // namespace matching
}  // namespace aliceVision
