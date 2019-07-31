//
// Created by kmdai on 2019-07-30.
//

#ifndef LIBRTMP_FILESOURCE_H
#define LIBRTMP_FILESOURCE_H

#include <media/NdkMediaExtractor.h>
#include <string>

class FileSource {
public:
    bool init(std::string name);
private:
    std::string mFileName{nullptr};
    AMediaExtractor *mMediaExtractor;
};


#endif //LIBRTMP_FILESOURCE_H
