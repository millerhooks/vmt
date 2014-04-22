#include <QCoreApplication>
#include <QStringList>
#include <QMap>
#include <QDir>
#include <QTextStream>
#include <iostream>

using namespace std;

#define TRAINING 1

void formatTrainingData(QString pathToBows, QString outFile);
void formatTestData(QString pathToBoWs, QString outFile);

int main(/*int argc, char *argv[]*/)
{


    //parameters
    QString pathToBoWs = "/home/emredog/LIRIS-data/training-validation_BagOfWords/training-validation_BoW-withNoAction_params01/with_K-Means_s100K_k4000_C50_e0.1";
    QString dataFileForLibSVM = "training-validation_data.dat";
//    QString pathToBoWs = "/home/emredog/LIRIS-data/test_BagOfWords/test_BoWs_params03/with_K-Means_s500K_k4000_C100_e0.1";
//    QString dataFileForLibSVM = "test_data.dat";

    if (TRAINING)
        formatTrainingData(pathToBoWs, dataFileForLibSVM);
    else
        formatTestData(pathToBoWs, dataFileForLibSVM);




}

void formatTestData(QString pathToBoWs, QString outFile)
{
    //get BoW files
    QDir dirBoWs(pathToBoWs);
    QStringList filters;  filters << "*.BoW";
    QStringList BoWFileNames = dirBoWs.entryList(filters, QDir::Files | QDir::NoDotAndDotDot);

    cout << "Obtained " << BoWFileNames.count() << " BoW files."  << endl;

    //create & prepare the output file
    QFile outputFile(dirBoWs.absoluteFilePath(outFile));
    if (!outputFile.open(QIODevice::Truncate | QIODevice::WriteOnly))
        cout << "ERROR opening file: " << dirBoWs.absoluteFilePath(outFile).toStdString() << endl;
    QTextStream dataOut(&outputFile);

    foreach(QString bowName, BoWFileNames)
    {
        //read data from bow file
        QFile bowFile(dirBoWs.absoluteFilePath(bowName));
        if (!bowFile.open(QIODevice::ReadOnly))
        {
            cout << "ERROR opening file: " << dirBoWs.absoluteFilePath(bowName).toStdString() << endl;
            continue;
        }
        QTextStream in(&bowFile);

        //write <label>
        dataOut << "0 ";    //"Labels in the testing file are only used
                            // to calculate accuracy or errors. If they are unknown, just fill the
                            // first column with any numbers."

        //write <index>:<value> pairs
        int index = 1; // from svm-scale help: minimal feature index is 0, but indices should start from 1
        while (!in.atEnd())
        {
            //write <index>:
            dataOut << index << ":";
            bool ok = false;
            int val = in.readLine().toInt(&ok);

            //write <value>
            if (ok)
                dataOut << val << " ";

            index++;
        }
        //write endline
        dataOut << "\n";

        cout << ".";
    }

    cout << endl << "Data file wrote successfully.." << endl;

}

void formatTrainingData(QString pathToBoWs, QString outFile)
{
    //prepare class names
    QMap<QString, int> classNames;
    classNames["no-action"] = 0;
    classNames["discussion"] = 1;
    classNames["give"] = 2;
    classNames["box-desk"] = 3;
    classNames["enter-leave"] = 4;
    classNames["try-to-enter"] = 5;
    classNames["unlock-enter"] = 6;
    classNames["baggage"] = 7;
    classNames["handshaking"] = 8;
    classNames["typing"] = 9;
    classNames["telephone"] = 10;

    //get BoW files
    QDir dirBoWs(pathToBoWs);
    QStringList filters;  filters << "*.BoW";
    QStringList BoWFileNames = dirBoWs.entryList(filters, QDir::Files | QDir::NoDotAndDotDot);

    cout << "Obtained " << BoWFileNames.count() << " BoW files."  << endl;

    //create & prepare the output file
    QFile outputFile(dirBoWs.absoluteFilePath(outFile));
    if (!outputFile.open(QIODevice::Truncate | QIODevice::WriteOnly))
        cout << "ERROR opening file: " << dirBoWs.absoluteFilePath(outFile).toStdString() << endl;
    QTextStream dataOut(&outputFile);

    foreach(QString bowName, BoWFileNames)
    {
        QString curClassName;
        {
            QStringList parts = bowName.split("_");
            curClassName = parts[2];
        }



        curClassName.truncate(curClassName.indexOf('.')); //removes after the first point

        //read data from bow file
        QFile bowFile(dirBoWs.absoluteFilePath(bowName));
        if (!bowFile.open(QIODevice::ReadOnly))
        {
            cout << "ERROR opening file: " << dirBoWs.absoluteFilePath(bowName).toStdString() << endl;
            continue;
        }
        QTextStream in(&bowFile);

        //write <label>
        int classIndex = classNames.value(curClassName, -1);

        if (classIndex < 0)
        {
            cout << "ERROR reading class name for: " << dirBoWs.absoluteFilePath(bowName).toStdString() << endl;
            continue;
        }

        dataOut << classIndex << " ";

        //write <index>:<value> pairs
        int index = 1; // from svm-scale help: minimal feature index is 0, but indices should start from 1
        while (!in.atEnd())
        {
            //write <index>:
            dataOut << index << ":";
            bool ok = false;
            int val = in.readLine().toInt(&ok);

            //write <value>
            if (ok)
                dataOut << val << " ";

            index++;
        }
        //write endline
        dataOut << "\n";

        cout << ".";
    }

    cout << endl << "Data file wrote successfully.." << endl;
}
