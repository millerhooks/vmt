#pragma once

#include <opencv2/imgproc/imgproc.hpp>  
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O
#include <fstream>
#include <iostream> // for standard I/O
#include <queue>

#include <QString>
#include <QPoint>
#include <qhash.h>

#include "depthtotolerance.h"



#define I_MAX 255
#define X_SIZE 640
#define Y_SIZE 480
#define Z_SIZE 8000
#define MIN_Z 10
#define MAX_Z Z_SIZE
#define NORMALIZATION_INTERVAL 2000 //(MAX_Z - MIN_Z) // ==> no normalization


using namespace std;

inline uint qHash(const QPoint& r)
{
    return qHash(QString("%1,%2").arg(r.x()).arg(r.y()));
}

class VmtFunctions
{
protected:
	static const int dims = 3;
	int *matrixSize;
    unsigned int permittedMinZ;
    unsigned int permittedMaxZ;

//    unsigned int toleranceX;
//    unsigned int toleranceY;
//    unsigned int toleranceZ;

    unsigned int downsampleRate;

    bool isTrackPoint;
    int trackX;
    int trackY;

    bool saveVolumeObject;
    bool saveDelta;
    bool saveVmt;

    bool isGsuData;

    DepthToTolerance dynamicTolerance;

	enum DimIndex
	{
        X = 0,
        Y = 1,
		Z = 2
	};

    inline float raw_depth_to_meters(int raw_depth) const
	{
        if (raw_depth < 2047)
			return 1.0 / (raw_depth * -0.0030711016 + 3.3309495161);
		return 0;
	}



    struct VmtInfo
    {
        int numberOfPoints;
        int sizeInX;
        int sizeInY;
        int sizeInZ;
        int maxX;
        int maxY;
        int maxZ;
        int minX;
        int minY;
        int minZ;
    };

public:
	//constructor for 3D VMTs
    VmtFunctions(bool isGsuData = false, int xSize = X_SIZE, int ySize = Y_SIZE/*, unsigned int tolX = 0, unsigned int tolY = 0, unsigned int tolZ = 0*/);
	~VmtFunctions(void);

    //generate a VMT from a video folder and a sliding window (generated by Bingbing's algo, 40frames long)
    cv::SparseMat constructSparseVMT(QString videoFolderPath, QString trackFilePath);

    //calculates rotation angles based on first and last volume objects:
    QList<float> calculateRotationAngles(QString videoFolderPath, QString trackFilePath);

    void setTrackPoint(int x, int y);
    void setSavedObjects(bool saveVolObj, bool saveDelta, bool saveVmt){this->saveVolumeObject = saveVolObj; this->saveDelta = saveDelta; this->saveVmt = saveVmt;}
    void setDownsampleRate(int dsRate){this->downsampleRate = dsRate;}

    static void print3x3Matrix(const cv::Matx33d& mat);
    static void print3DSparseMatrix(const cv::SparseMat& sparse_mat);
    static void save3DSparseMatrix(const cv::SparseMat& sparse_mat, QString filePath);
    static void save3DMatrix(const cv::Mat& sparse_mat, QString filePath);

    //Get basic info of a 3d sparse mat
    VmtInfo getVmtInfo(const cv::SparseMat &vmt) const;

    //save a vmt as an image sequence to use it in klaser&schmid code later
    int saveVmtAsImageSequence(const cv::SparseMat &vmt, QString outputFolder) const;

    //shrink & trim the 3d sparse mat to get rid of unnecessarily large size
    cv::SparseMat trimSparseMat(const cv::SparseMat &vmt) const;

    cv::SparseMat spatiallyNormalizeSparseMat(cv::SparseMat vmt) const;


protected:

    //to be used with cropped images (with track files)
    cv::SparseMat generateSparseVolumeObject(cv::Mat image, int downsamplingRate = 2);

    cv::SparseMat subtractSparseMat(const cv::SparseMat& operand1, const cv::SparseMat& operand2);

    cv::SparseMat cleanUpVolumeObjectDifference(const cv::SparseMat& volObjDiff) const;

    double attenuationConstantForAnAction(const QList<cv::SparseMat> &volumeObjectsDifferences);

    //magnitude of motion is calculated over a volume object difference (delta) at a given time t
    double magnitudeOfMotion(const cv::SparseMat& sparseMat);

    //A VMT is constructed over a sequence of volume object differences
    cv::SparseMat calculateVMT(const QList<cv::SparseMat>& volumeObjectDifferences);

    cv::SparseMat calculateD_Old(cv::SparseMat lastVolumeObject, cv::SparseMat firstVolumeObject);
    cv::SparseMat calculateD_New(cv::SparseMat lastVolumeObject, cv::SparseMat firstVolumeObject);
    cv::Vec3i calculateMomentVector(const cv::SparseMat &volumeObject);

    double calculateAlpha(const cv::Vec3i &motionVector);
    double calculateBeta(const cv::Vec3i &motionVector);
    double calculateTheta(const cv::Vec3i &motionVector);

    cv::Matx33d calculateRotationX_alpha(double alpha);
    cv::Matx33d calculateRotationY_beta(double beta);
    cv::Matx33d calculateRotationZ_theta(double theta);

    cv::SparseMat rotateVMT(const cv::SparseMat& vmt, const cv::Matx33d& rotationMatrix);
    cv::Mat projectVMTOntoXY(const cv::SparseMat& vmt);    

    cv::Mat extractSilhouette(const cv::Mat& mat) const;

    int depthCorrectionCoefficient(int depthInMm) const;



    //not used
    //    cv::Mat GenerateVolumeObject(cv::Mat image, int downsamplingRate = 2);
    //    vector<cv::SparseMat> CalculateVolumeObjectDifferencesSparse(const vector<cv::SparseMat>& volumeObjects);
    //    vector<cv::SparseMat> CalculateVolumeObjectDifferencesSparse(const vector<cv::SparseMat>& volumeObjects, int depthTolerance);
    //    cv::SparseMat GenerateSparseVolumeObject(QString imagePath, int downsamplingRate = 2);
    //    vector<cv::SparseMat> ConstructVMTs(const vector<cv::SparseMat>& volumeObjectDifferences);

};



