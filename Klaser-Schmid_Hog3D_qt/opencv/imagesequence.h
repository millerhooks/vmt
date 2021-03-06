#ifndef IMAGESEQUENCE_H
#define IMAGESEQUENCE_H

#include <QDir>


// forward decleration
class IplImageWrapper;

class ImageSequence
{
public:
    typedef /*long*/ long FrameIndex;
protected:
    QDir directory;
    QStringList imageFiles;

    IplImageWrapper *_currentFrame;
    //IplImageWrapper *_peekBuffer;

    int _width;
    int _height;
    //long long _startPts;
    FrameIndex _currentFrameIndex;
    //FrameIndex _peekFrameIndex;

    void init();
    void cleanup();
    bool advance(FrameIndex frameIndex);

    bool isWorkingWithDepth; //ED
    int nbrChannels; //ED

public:
    //constructor
    ImageSequence(const std::string& folderPath, bool workingWithDepth, int channels) :
        directory(QString::fromStdString(folderPath)),
       /* _startPts(0),*/  _currentFrame(0) /*, _peekFrameIndex(0)*/
    {
        _currentFrameIndex = 0;
        isWorkingWithDepth = workingWithDepth; //ED
        nbrChannels = channels; //ED
        init();
    }

    //desctructor
    ~ImageSequence();

    // queries
    const std::string& getFileName() const
    {
        return this->directory.absolutePath().toStdString();
    }

    int getWidth() const { return this->_width; }
    int getHeight() const { return this->_height; }
    //double getDuration() const;
    FrameIndex numOfFrames() const { return static_cast<FrameIndex>(this->imageFiles.count()); }
    FrameIndex getFrameIndex() const { return this->_currentFrameIndex; }
    //double getFrameRate() const;
    //double getTime() const;
    //double getStartTime() const;

    // frame access
    const IplImageWrapper getFrame() const;

    // seeking functionality
    bool nextFrame();
    bool prevFrame();
    bool seek(FrameIndex frame);
    //bool seek(double seconds);

    // other functionalities
    bool probeFrame(FrameIndex frame);
    //bool probeFrame(double seconds);
    IplImageWrapper *currentBuffer() const;
    void setCurrentBuffer(IplImageWrapper *currentBuffer);
};

#endif // IMAGESEQUENCE_H
