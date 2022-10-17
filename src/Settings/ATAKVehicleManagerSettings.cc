/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "ATAKVehicleManagerSettings.h"

#include <QQmlEngine>
#include <QtQml>

DECLARE_SETTINGGROUP(ATAKVehicleManager, "ATAKVehicleManager")
{
    qmlRegisterUncreatableType<ATAKVehicleManagerSettings>("QGroundControl.SettingsManager", 1, 0, "ATAKVehicleManagerSettings", "Reference only");
}

DECLARE_SETTINGSFACT(ATAKVehicleManagerSettings, atakServerHostAddress)
DECLARE_SETTINGSFACT(ATAKVehicleManagerSettings, atakServerPort)
