/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#pragma once

#include <QObject>
#include "FactPanelController.h"


class EngineRunupController : public FactPanelController
{
    Q_OBJECT
    
public:
    EngineRunupController();
    Q_INVOKABLE void connectJoystick();
    Q_INVOKABLE void disconnectJoystick();

signals:
    void joystickStartRunup();
    void joystickStopRunup();

protected slots:
    virtual void    _startEngineRunup               ();
    virtual void    _stopEngineRunup               ();

private slots:


private:

    Joystick*           _activeJoystick     = nullptr;

signals:
private:
};
