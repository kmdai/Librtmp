//
// Created by kmdai on 2019-07-30.
//

#include "FileSource.h"

bool FileSource::init(std::string name) {
    mMediaExtractor = AMediaExtractor_new();
    if (AMediaExtractor_setDataSource(mMediaExtractor, name.c_str()) != AMEDIA_OK) {
        return false;
    }

}
