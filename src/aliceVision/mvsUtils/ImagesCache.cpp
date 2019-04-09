// This file is part of the AliceVision project.
// Copyright (c) 2017 AliceVision contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include "ImagesCache.hpp"
#include <aliceVision/mvsUtils/common.hpp>
#include <aliceVision/mvsUtils/fileIO.hpp>

#include <future>

namespace aliceVision {
namespace mvsUtils {

ImagesCache::ImagesCache(const MultiViewParams& mp, int _bandType)
  : _mp(mp)
  , bandType( _bandType )
{
    std::vector<std::string> _imagesNames;
    for(int rc = 0; rc < _mp.getNbCameras(); rc++)
    {
        _imagesNames.push_back(_mp.getImagePath(rc));
    }
    initIC( _imagesNames );
}

ImagesCache::ImagesCache(const MultiViewParams& mp, int _bandType, std::vector<std::string>& _imagesNames)
  : _mp(mp)
  , bandType( _bandType )
{
    initIC( _imagesNames );
}

void ImagesCache::initIC( std::vector<std::string>& _imagesNames )
{
    float oneimagemb = (sizeof(Color) * _mp.getMaxImageWidth() * _mp.getMaxImageHeight()) / 1024.f / 1024.f;
    float maxmbCPU = (float)_mp.userParams.get<int>("images_cache.maxmbCPU", 5000);
    int _npreload = std::max((int)(maxmbCPU / oneimagemb), 5); // image cache has a minimum size of 5
    N_PRELOADED_IMAGES = std::min(_mp.ncams, _npreload);

    for(int rc = 0; rc < _mp.ncams; rc++)
    {
        imagesNames.push_back(_imagesNames[rc]);
    }

    imgs.resize(N_PRELOADED_IMAGES); // = new Color*[N_PRELOADED_IMAGES];

    camIdMapId.resize(_mp.ncams, -1 );
    mapIdCamId.resize( N_PRELOADED_IMAGES, -1 );
    mapIdClock.resize( N_PRELOADED_IMAGES, clock() );

    {
        // Cannot resize the vector<mutex> directly, as mutex class is not move-constructible.
        // imagesMutexes.resize(_mp.ncams); // cannot compile
        // So, we do the same with a new vector and swap.
        std::vector<std::mutex> imagesMutexesTmp(_mp.ncams);
        imagesMutexes.swap(imagesMutexesTmp);
    }


}

ImagesCache::~ImagesCache()
{
}

void ImagesCache::refreshData(int camId)
{
    // printf("camId %i\n",camId);
    // test if the image is in the memory
    if(camIdMapId[camId] == -1)
    {
        // remove the oldest one
        int mapId = mapIdClock.minValId();
        int oldCamId = mapIdCamId[mapId];
        if(oldCamId>=0)
            camIdMapId[oldCamId] = -1;
            // TODO: oldCamId should be protected if already used

        // replace with new new
        camIdMapId[camId] = mapId;
        mapIdCamId[mapId] = camId;
        mapIdClock[mapId] = clock();

        // reload data from files
        long t1 = clock();
        if (imgs[mapId] == nullptr)
        {
            const std::size_t maxSize = _mp.getMaxImageWidth() * _mp.getMaxImageHeight();
            imgs[mapId] = std::make_shared<Img>( maxSize );
        }

        const std::string imagePath = imagesNames.at(camId);
        memcpyRGBImageFromFileToArr(camId, imgs[mapId]->data, imagePath, _mp, bandType);
        imgs[mapId]->setWidth(  _mp.getWidth(camId) );
        imgs[mapId]->setHeight( _mp.getHeight(camId) );

        ALICEVISION_LOG_DEBUG("Add " << imagePath << " to image cache. " << formatElapsedTime(t1));
    }
    else
    {
      ALICEVISION_LOG_DEBUG("Reuse " << imagesNames.at(camId) << " from image cache. ");
    }
}

void ImagesCache::refreshImage_sync(int camId)
{
  std::lock_guard<std::mutex> lock(imagesMutexes[camId]);
  refreshData(camId);
}

void ImagesCache::refreshImage_async(int camId)
{
    _asyncObjects.emplace_back(std::async(std::launch::async, &ImagesCache::refreshImage_sync, this, camId));
}

void ImagesCache::refreshImages_sync(const std::vector<int>& camIds)
{
    for(int camId: camIds)
        refreshImage_sync(camId);
}

void ImagesCache::refreshImages_async(const std::vector<int>& camIds)
{
    _asyncObjects.emplace_back(std::async(std::launch::async, &ImagesCache::refreshImages_sync, this, camIds));
}

Color ImagesCache::getPixelValueInterpolated(const Point2d* pix, int camId)
{
    // get the image index in the memory
    const int i = camIdMapId[camId];
    const ImgPtr& img = imgs[i];
    
    const int xp = static_cast<int>(pix->x);
    const int yp = static_cast<int>(pix->y);

    // precision to 4 decimal places
    const float ui = pix->x - static_cast<float>(xp);
    const float vi = pix->y - static_cast<float>(yp);

    const Color lu = img->at( xp  , yp   );
    const Color ru = img->at( xp+1, yp   );
    const Color rd = img->at( xp+1, yp+1 );
    const Color ld = img->at( xp  , yp+1 );

    // bilinear interpolation of the pixel intensity value
    const Color u = lu + (ru - lu) * ui;
    const Color d = ld + (rd - ld) * ui;
    const Color out = u + (d - u) * vi;

    return out;
}


} // namespace mvsUtils
} // namespace aliceVision
