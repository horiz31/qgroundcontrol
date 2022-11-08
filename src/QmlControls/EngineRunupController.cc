/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "EngineRunupController.h"
#include "QGCApplication.h"
#include "JoystickManager.h"
#include <QDebug>

EngineRunupController::EngineRunupController(void)
{

}

void EngineRunupController::connectJoystick()
{
    JoystickManager *pJoyMgr = qgcApp()->toolbox()->joystickManager();
    _activeJoystick = pJoyMgr->activeJoystick();

    if(_activeJoystick) {
        connect(_activeJoystick, &Joystick::startEngineRunup,               this, &EngineRunupController::_startEngineRunup);
        connect(_activeJoystick, &Joystick::stopEngineRunup,               this, &EngineRunupController::_stopEngineRunup);
    }
}

void EngineRunupController::disconnectJoystick()
{
    JoystickManager *pJoyMgr = qgcApp()->toolbox()->joystickManager();
    _activeJoystick = pJoyMgr->activeJoystick();

    if(_activeJoystick) {
        disconnect(_activeJoystick, &Joystick::startEngineRunup,               this, &EngineRunupController::_startEngineRunup);
        disconnect(_activeJoystick, &Joystick::stopEngineRunup,               this, &EngineRunupController::_stopEngineRunup);
    }
}

void EngineRunupController::_startEngineRunup()
{
    //qDebug() << "got engine start runup signal";
    if(_activeJoystick)
        _activeJoystick->setRunupEnabled(true);
    emit joystickStartRunup();
}

void EngineRunupController::_stopEngineRunup()
{
    //qDebug() << "got engine stop runup signal";
    if(_activeJoystick)
        _activeJoystick->setRunupEnabled(false);
    emit joystickStopRunup();
}
