/****************************************************************************
 *
 * (c) 2009-2020 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "VehicleLinkManager.h"
#include "Vehicle.h"
#include "QGCLoggingCategory.h"
#include "LinkManager.h"
#include "QGCApplication.h"
#include "UDPLink.h"
#include "LinkConfiguration.h"
#include "SettingsManager.h"

QGC_LOGGING_CATEGORY(VehicleLinkManagerLog, "VehicleLinkManagerLog")

VehicleLinkManager::VehicleLinkManager(Vehicle* vehicle)    
    : QObject   (vehicle)
    , _vehicle  (vehicle)
    , _linkMgr  (qgcApp()->toolbox()->linkManager())
{
    connect(this,                   &VehicleLinkManager::linkNamesChanged,  this, &VehicleLinkManager::linkStatusesChanged);
    connect(&_commLostCheckTimer,   &QTimer::timeout,                       this, &VehicleLinkManager::_commLostCheck);

    Fact* fact = _vehicle->_toolbox->settingsManager()->appSettings()->defaultConnectionName();
    connect(fact, &Fact::valueChanged, this, &VehicleLinkManager::_defaultNameChanged);

    _commLostCheckTimer.setSingleShot(false);
    _commLostCheckTimer.setInterval(_commLostCheckTimeoutMSecs);
}



void VehicleLinkManager::mavlinkMessageReceived(LinkInterface* link, mavlink_message_t message)
{
    // Radio status messages come from Sik Radios directly. It doesn't indicate there is any life on the other end.
    if (message.msgid != MAVLINK_MSG_ID_RADIO_STATUS) {
        int linkIndex = _containsLinkIndex(link);
        if (linkIndex == -1) {
            _addLink(link);
        } else {
            LinkInfo_t& linkInfo = _rgLinkInfo[linkIndex];
            linkInfo.heartbeatElapsedTimer.restart();
            if (_rgLinkInfo[linkIndex].commLost) {
                _commRegainedOnLink(link);
            }
        }
    }
}

void VehicleLinkManager::_commRegainedOnLink(LinkInterface* link)
{
    QString commRegainedMessage;
    QString primarySwitchMessage;

    int linkIndex = _containsLinkIndex(link);
    if (linkIndex == -1) {
        return;
    }

    _rgLinkInfo[linkIndex].commLost = false;

    // Notify the user of communication regained
    if (_rgLinkInfo.count() > 1) {
        //get the link name
        QString linkName = link->linkConfiguration()->name();
        if (linkName.isEmpty()) linkName = "link";
        if (linkName == "MPU5" || linkName == "mpu5")
            linkName = "m p u 5";
        commRegainedMessage = tr("%1Communication regained on %2.").arg(_vehicle->_vehicleIdSpeech()).arg(linkName);
        //commRegainedMessage = tr("%1Communication regained on %2 link").arg(_vehicle->_vehicleIdSpeech()).arg(isPrimaryLink ? tr("primary") : tr("secondary"));
    } else {
        commRegainedMessage = tr("%1Communication regained").arg(_vehicle->_vehicleIdSpeech());
    }

    // Try to switch to another link
    // volocomm edit
    /*
    if (_updatePrimaryLink()) {
        QString primarySwitchMessage = tr("%1Switching communication to new primary link").arg(_vehicle->_vehicleIdSpeech());
    }
    */

    if (!commRegainedMessage.isEmpty()) {
        _vehicle->_say(commRegainedMessage);
    }
    // volocomm edit
    /*
    if (!primarySwitchMessage.isEmpty()) {
        _vehicle->_say(primarySwitchMessage);
        qgcApp()->showAppMessage(primarySwitchMessage);
    }
    */

    emit linkStatusesChanged();

    // volocomm edit
    // if at least one channel returns, undo total loss
    // Check recovery from total communication loss
    if (_communicationLost) {

            _communicationLost = false;
            emit communicationLostChanged(false);

    }

    /*

    // Check recovery from total communication loss
    if (_communicationLost) {
        bool noCommunicationLoss = true;
        for (const LinkInfo_t& linkInfo: _rgLinkInfo) {
            if (linkInfo.commLost) {
                noCommunicationLoss = false;
                break;
            }
        }
        if (noCommunicationLoss) {
            _communicationLost = false;
            emit communicationLostChanged(false);
        }
    }*/
}

void VehicleLinkManager::_defaultNameChanged(void)
{
    //triggered when the default link name is changed in settings, this ensures the link name is updated in the _rgLinkInfo list
    for (const LinkInfo_t& linkInfo: _rgLinkInfo) {
        if (linkInfo.link->linkConfiguration()->type() == LinkConfiguration::TypeUdp)
        {
            UDPConfiguration* udpconfig = (qobject_cast<UDPConfiguration*>(linkInfo.link->linkConfiguration().get()));
            if (udpconfig)
            {
                if (udpconfig->localPort() == 14550)
                {
                    linkInfo.link->linkConfiguration()->setName(_vehicle->_toolbox->settingsManager()->appSettings()->defaultConnectionName()->rawValueString());
                    emit linkNamesChanged();
                }
            }
        }
    }
}

void VehicleLinkManager::_commLostCheck(void)
{
    QString switchingPrimaryLinkMessage;

    if (!_communicationLostEnabled) {
        return;
    }

    bool linkStatusChange = false;
    for (LinkInfo_t& linkInfo: _rgLinkInfo) {
        if (!linkInfo.commLost && !linkInfo.link->linkConfiguration()->isHighLatency() && linkInfo.heartbeatElapsedTimer.elapsed() > _heartbeatMaxElpasedMSecs) {
            linkInfo.commLost = true;
            linkStatusChange = true;

            // Notify the user of individual link communication loss
            //bool isPrimaryLink = linkInfo.link.get() == _primaryLink.lock().get();
            if (_rgLinkInfo.count() > 1) {
                //volocomm edit
                //say what link was lost
                //get the link name
                QString linkName = linkInfo.link.get()->linkConfiguration()->name();
                if (linkName.isEmpty()) linkName = "link";
                if (linkName == "MPU5" || linkName == "mpu5")
                    linkName = "m p u 5";
                QString msg = tr("%1Communication lost on %2.").arg(_vehicle->_vehicleIdSpeech()).arg(linkName);
                //QString msg = tr("%1Communication lost on %2.").arg(_vehicle->_vehicleIdSpeech()).arg(isPrimaryLink ? tr("primary") : tr("secondary"));
                _vehicle->_say(msg);
            }
        }
    }
    if (linkStatusChange) {
        emit linkStatusesChanged();
    }

    // Switch to better primary link if needed

    //debug
    /*
    if (_updatePrimaryLink()) {
        QString msg = tr("%1Switching communication to secondary link.").arg(_vehicle->_vehicleIdSpeech());
        _vehicle->_say(msg);
        qgcApp()->showAppMessage(msg);
    }
*/


    // Check for total communication loss
    if (!_communicationLost) {
        bool totalCommunicationLoss = true;
        for (const LinkInfo_t& linkInfo: _rgLinkInfo) {
            if (!linkInfo.commLost) {
                totalCommunicationLoss = false;
                break;
            }
        }
        if (totalCommunicationLoss) {
            if (_autoDisconnect) {
                // There is only one link to the vehicle and we want to auto disconnect from it
                closeVehicle();
                return;
            }
            _vehicle->_say(tr("%1Communication lost").arg(_vehicle->_vehicleIdSpeech()));

            _communicationLost = true;
            emit communicationLostChanged(true);
        }
    }
}

int VehicleLinkManager::_containsLinkIndex(LinkInterface* link)
{
    for (int i=0; i<_rgLinkInfo.count(); i++) {
        if (_rgLinkInfo[i].link.get() == link) {
            return i;
        }
    }
    return -1;
}

void VehicleLinkManager::_addLink(LinkInterface* link)
{
    //debug - see if the link is udp, and if so what is the port?
    bool isThisUDP14550 = false;
    LinkConfiguration::LinkType type = link->linkConfiguration()->type();
    if (type == LinkConfiguration::TypeUdp)
    {
        UDPConfiguration* udpconfig = (qobject_cast<UDPConfiguration*>(link->linkConfiguration().get()));
        if (udpconfig)
        {         
            if (udpconfig->localPort() == 14550)
                isThisUDP14550 = true;
        }
    }
    if (_containsLinkIndex(link) != -1) {
        qCWarning(VehicleLinkManagerLog) << "_addLink call with link which is already in the list";
        return;
    } else {
        SharedLinkInterfacePtr sharedLink = _linkMgr->sharedLinkInterfacePointerForLink(link);
        if (!sharedLink) {
            qCDebug(VehicleLinkManagerLog) << "_addLink stale link" << (void*)link;
            return;
        }
        qCDebug(VehicleLinkManagerLog) << "_addLink:" << link->linkConfiguration()->name() << QString("%1").arg((qulonglong)link, 0, 16);

        link->addVehicleReference();

        LinkInfo_t linkInfo;
        linkInfo.link = sharedLink;
        if (!link->linkConfiguration()->isHighLatency()) {
            linkInfo.heartbeatElapsedTimer.start();
        }
        _rgLinkInfo.append(linkInfo);


        _updatePrimaryLink();

        //volocomm
        //if this is not the first link and is not the primary udp 14550 link, announce its addition
        if (_rgLinkInfo.count() > 1 && !isThisUDP14550)
        {
            QString linkAddedMessage = tr("%1 link added").arg(link->linkConfiguration()->name());
            _vehicle->_say(linkAddedMessage);
        }


        connect(link, &LinkInterface::disconnected, this, &VehicleLinkManager::_linkDisconnected);

        emit linkNamesChanged();

        if (_rgLinkInfo.count() == 1) {
            _commLostCheckTimer.start();
        }
    }
}

void VehicleLinkManager::_removeLink(LinkInterface* link)
{
    int linkIndex = _containsLinkIndex(link);

    if (linkIndex == -1) {
        qCWarning(VehicleLinkManagerLog) << "_removeLink call with link which is already in the list";
        return;
    } else {
        qCDebug(VehicleLinkManagerLog) << "_removeLink:" << QString("%1").arg((qulonglong)link, 0, 16);

        if (link == _primaryLink.lock().get()) {
            _primaryLink.reset();
            emit primaryLinkChanged();
        }

        disconnect(link, &LinkInterface::disconnected, this, &VehicleLinkManager::_linkDisconnected);
        link->removeVehicleReference();
        emit linkNamesChanged();
        _rgLinkInfo.removeAt(linkIndex); // Remove the link last since it may cause the link itself to be deleted

        if (_rgLinkInfo.count() == 0) {
            _commLostCheckTimer.stop();
        }
    }
}

void VehicleLinkManager::_linkDisconnected(void)
{
    qCDebug(VehicleLog) << "_linkDisconnected linkCount" << _rgLinkInfo.count();

    LinkInterface* link = qobject_cast<LinkInterface*>(sender());
    if (link) {
        _removeLink(link);
        //debug
        //_updatePrimaryLink();
        if (_rgLinkInfo.count() == 0) {
            qCDebug(VehicleLog) << "All links removed. Closing down Vehicle.";
            emit allLinksRemoved(_vehicle);
        }
    }
}

SharedLinkInterfacePtr VehicleLinkManager::_bestActivePrimaryLink(void)
{
    //this is called when a link is added
    //priority for volocomm
    //1. 14550 UDP
    //2. Any other non-serial connection
    //3. Serial
    //4. High Latency Link like satcom
    //qDebug() << "adding new link and returning the best active primary link available";
    //udp link on 14550?
    for (const LinkInfo_t& linkInfo: _rgLinkInfo) {
        if (!linkInfo.commLost) {
            SharedLinkInterfacePtr      link    = linkInfo.link;
            UDPConfiguration* udpconfig = (qobject_cast<UDPConfiguration*>(link->linkConfiguration().get()));
            if (udpconfig)
            {
                if (udpconfig->localPort() == 14550 && !udpconfig->isHighLatency())
                {
                    return link;
                }
            }
        }
    }

    // Next best is normal latency link other than UDP 14550
    for (const LinkInfo_t& linkInfo: _rgLinkInfo) {
        if (!linkInfo.commLost) {
            SharedLinkInterfacePtr      link    = linkInfo.link;
            SharedLinkConfigurationPtr  config  = link->linkConfiguration();
            UDPConfiguration* udpconfig = (qobject_cast<UDPConfiguration*>(config.get()));
            if (config && !config->isHighLatency())
            {
                if (udpconfig && udpconfig->localPort() == 14550)  //return any other network link except udp 14550 which would have been handled above
                    continue;
                qDebug() << "returning other network link";
                return link;
            }
        }
    }

#ifndef NO_SERIAL_LINK
    // Last choice is a USB connection
    for (const LinkInfo_t& linkInfo: _rgLinkInfo) {
        if (!linkInfo.commLost) {
            SharedLinkInterfacePtr  link        = linkInfo.link;
            SerialLink*             serialLink  = qobject_cast<SerialLink*>(link.get());
            if (serialLink) {
                SharedLinkConfigurationPtr config = serialLink->linkConfiguration();
                if (config) {
                    SerialConfiguration* serialConfig = qobject_cast<SerialConfiguration*>(config.get());
                    if (serialConfig && serialConfig->usbDirect()) {
                        return link;
                    }
                }
            }
        }
    }
#endif


    // Last possible choice is a high latency link
    SharedLinkInterfacePtr link = _primaryLink.lock();
    if (link && link->linkConfiguration()->isHighLatency()) {
        // Best choice continues to be the current high latency link
        return link;
    } else {
        // Pick any high latency link if one exists
        for (const LinkInfo_t& linkInfo: _rgLinkInfo) {
            if (!linkInfo.commLost) {
                SharedLinkInterfacePtr      link    = linkInfo.link;
                SharedLinkConfigurationPtr  config  = link->linkConfiguration();
                if (config && config->isHighLatency()) {
                    return link;
                }
            }
        }
    }

    return {};
}


bool VehicleLinkManager::_updatePrimaryLink(void)
{    
    //qDebug() << "running _updatePrimaryLink";

    SharedLinkInterfacePtr primaryLink = _primaryLink.lock();
    int linkIndex = _containsLinkIndex(primaryLink.get());

    //Volocomm edit, this kept the link as it was if it was not lost, which is not what we want, we want to switch it to udp/14550 if it exists
    /*
    if (linkIndex != -1 && !_rgLinkInfo[linkIndex].commLost && !primaryLink->linkConfiguration()->isHighLatency()) {
        // Current priority link is still valid
        qDebug() << "current link is still valid, not changing";
        return false;
    }
    */

    SharedLinkInterfacePtr bestActivePrimaryLink = _bestActivePrimaryLink();

    if (linkIndex != -1 && !bestActivePrimaryLink) {
        // Nothing better available, leave things set to current primary link
        qDebug() << "nothing better, leaving link as is";
        return false;
    } else {
        if (bestActivePrimaryLink != primaryLink) {
            if (primaryLink && primaryLink->linkConfiguration()->isHighLatency()) {
                _vehicle->sendMavCommand(MAV_COMP_ID_AUTOPILOT1,
                               MAV_CMD_CONTROL_HIGH_LATENCY,
                               true,
                               0); // Stop transmission on this link
            }
            //qDebug() << "changing link";
            _primaryLink = bestActivePrimaryLink;
            emit primaryLinkChanged();

            if (bestActivePrimaryLink && bestActivePrimaryLink->linkConfiguration()->isHighLatency()) {
                _vehicle->sendMavCommand(MAV_COMP_ID_AUTOPILOT1,
                               MAV_CMD_CONTROL_HIGH_LATENCY,
                               true,
                               1); // Start transmission on this link
            }
            return true;
        } else {
            return false;
        }
    }
}

void VehicleLinkManager::closeVehicle(void)
{
    // Vehicle is no longer communicating with us. Remove all link references

    QList<LinkInfo_t> rgLinkInfoCopy = _rgLinkInfo;
    for (const LinkInfo_t& linkInfo: rgLinkInfoCopy) {
        _removeLink(linkInfo.link.get());
    }

    _rgLinkInfo.clear();

    emit allLinksRemoved(_vehicle);
}

void VehicleLinkManager::setCommunicationLostEnabled(bool communicationLostEnabled)
{
    if (_communicationLostEnabled != communicationLostEnabled) {
        _communicationLostEnabled = communicationLostEnabled;
        emit communicationLostEnabledChanged(communicationLostEnabled);
    }
}

bool VehicleLinkManager::containsLink(LinkInterface* link)
{
    return _containsLinkIndex(link) != -1;
}

QString VehicleLinkManager::primaryLinkName() const
{
    if (!_primaryLink.expired()) {
        return _primaryLink.lock()->linkConfiguration()->name();
    }

    return QString();
}
void VehicleLinkManager::setPrimaryLinkByName(const QString& name)
{
    QString trimmedName = name.trimmed();
    for (const LinkInfo_t& linkInfo: _rgLinkInfo) {        
        if (linkInfo.link->linkConfiguration()->name() == trimmedName) {
            _primaryLink = linkInfo.link;
            if (trimmedName == "MPU5" || trimmedName == "mpu5")
                trimmedName = "m p u 5";
            QString SwitchMessage = tr("%1Switching communication to %2")
                                        .arg(_vehicle->_vehicleIdSpeech())
                                        .arg(trimmedName);
            _vehicle->_say(SwitchMessage);
            emit primaryLinkChanged();
            return;
        }
    }
     qDebug()  << "failed to change link to" << trimmedName;
}

QStringList VehicleLinkManager::linkNames(void) const
{
    QStringList rgNames;

    for (const LinkInfo_t& linkInfo: _rgLinkInfo) {
        rgNames.append(linkInfo.link->linkConfiguration()->name());
    }

    return rgNames;
}

QStringList VehicleLinkManager::linkStatuses(void) const
{
    QStringList rgStatuses;

    for (const LinkInfo_t& linkInfo: _rgLinkInfo) {
        rgStatuses.append(linkInfo.commLost ? tr("Comm Lost") : "");
    }

    return rgStatuses;
}

bool VehicleLinkManager::primaryLinkIsPX4Flow(void) const
{
    SharedLinkInterfacePtr sharedLink = _primaryLink.lock();
    if (!sharedLink) {
        return false;
    } else {
        return sharedLink->isPX4Flow();
    }
}
