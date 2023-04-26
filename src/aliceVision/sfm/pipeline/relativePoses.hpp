// This file is part of the AliceVision project.
// Copyright (c) 2023 AliceVision contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include <aliceVision/types.hpp>
#include <aliceVision/numeric/numeric.hpp>
#include <boost/json.hpp>
#include <aliceVision/sfm/liealgebra.hpp>

namespace aliceVision
{
namespace sfm
{

struct ReconstructedPair
{
    IndexT reference;
    IndexT next;
    Mat3 R;
    Vec3 t;
};

void tag_invoke(const boost::json::value_from_tag&, boost::json::value& jv, sfm::ReconstructedPair const& input)
{
    jv = {{"reference", input.reference}, {"next", input.next}, {"R", SO3::logm(input.R)}, {"t", input.t}};
}

ReconstructedPair tag_invoke(boost::json::value_to_tag<ReconstructedPair>, boost::json::value const& jv)
{
    const boost::json::object& obj = jv.as_object();

    ReconstructedPair ret;

    /*ret.reference = boost::json::value_to<IndexT>(obj.at("reference"));
    ret.next = boost::json::value_to<IndexT>(obj.at("next"));
    ret.R = SO3::expm(boost::json::value_to<Vec3>(obj.at("R")));
    ret.t = boost::json::value_to<Vec3>(obj.at("t"));*/

    return ret;
}

}
}