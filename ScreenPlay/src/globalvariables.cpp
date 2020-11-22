#include "globalvariables.h"
namespace ScreenPlay {
/*!
    \class ScreenPlay::GlobalVariables
    \inmodule ScreenPlay
    \brief  Contains all variables that are globally needed.


*/


/*!
    \brief .
*/
ScreenPlay::GlobalVariables::GlobalVariables(QObject* parent)
    : QObject(parent)
{
    setLocalSettingsPath(QUrl { QStandardPaths::writableLocation(QStandardPaths::DataLocation) });
}

}
