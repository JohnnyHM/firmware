#include "ConnectedDevicesManager.h"
#include "ConnectivityDisplay.h"
#include "TempControl.h"
#include "UI.h"

char * valueAsText(const ConnectedDevice* device, char* buf, size_t len) {
    char * start = buf;
    if (device->dt==DEVICETYPE_TEMP_SENSOR) {        
        start = device->value.temp.toTempString(buf, 1, len, tempControl.cc.tempFormat, true); // sets buf to first non-space character
    }
    else if (device->dt==DEVICETYPE_SWITCH_ACTUATOR || device->dt==DEVICETYPE_SWITCH_SENSOR) {
        strncpy(buf, device->value.state ? "On" : "Off", len);
    }
    else {
        buf[0] = 0;
    }
    buf[len-1] = 0;
    return start;
}

void connectionAsText(const ConnectedDevice* device, char* buf, size_t len) {
    if (device->dt==DEVICETYPE_NONE) {
        buf[0] = 0;
    }    
    else if (device->connection.type==DEVICE_CONNECTION_ONEWIRE && len>16) {
        printBytes(device->connection.address, 8, buf);        
    }
    else {
        itoa(device->connection.pin, buf);        
    }
    buf[len-1] = 0;
}

char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}


void ConnectedDevicesManager::handleDevice(DeviceConfig* config, DeviceCallbackInfo* info) 
{    
    if (config->deviceHardware == DEVICE_HARDWARE_ONEWIRE_TEMP) {     
        int slot = existingSlot(config);
        if (slot >= 0) { // found the device still active
            TempSensor * sensor = asInterface<TempSensor>(devices[slot].pointer);
            if(sensor == nullptr){
                return;
            }
            sensor->update();
            temp_t newTemp = sensor->read();
            if(newTemp == TEMP_SENSOR_DISCONNECTED){
                devices[slot].lastSeen+=2;                
            } 
            else {
                devices[slot].lastSeen = 0; // seen this one now
                // check if temperature has changed to notify that UI needs to be updated
                if(newTemp != devices[slot].value.temp){
                    devices[slot].value.temp = newTemp;
                    changed(this, slot, devices + slot, UPDATED);
                }
            }                                
        } else {
            // attempt to reuse previous location
            slot = existingSlot(config, false);
            if (slot < 0)
                slot = freeSlot(config);
            if (slot >= 0) {
                clearSlot(slot);
                sendRemoveEvent(slot);
                ConnectedDevice& device = devices[slot];
                device.lastSeen = 0;

                device.dh = config->deviceHardware;
                device.dt = device.dh == DEVICE_HARDWARE_ONEWIRE_TEMP ? DEVICETYPE_TEMP_SENSOR : DEVICETYPE_SWITCH_ACTUATOR;
                device.connection.type = deviceConnection(device.dh);
                memcpy(device.connection.address, config->hw.address, 8);
                device.value.temp = temp_t::invalid(); // flag invalid
                device.pointer = DeviceManager::createDevice(*config, device.dt);
                auto sensor = asInterface<TempSensor>(device.pointer);
                if (!sensor || !sensor->isConnected()) {
                    clearSlot(slot);
                    device.lastSeen = -1; // don't send REMOVED event since no added event has been sent
                } else
                    changed(this, slot, &device, ADDED); // new device added					
            }
            // just ignore the device - not enough free slots
        }
    }
}

void ConnectedDevicesManager::update() 
{
    DeviceCallbackInfo info;
    info.data = this;
    EnumerateHardware spec;
    memset(&spec, 0, sizeof (spec));
    spec.pin = -1; // any pin
    spec.hardware = -1; // any hardware

    // increment the last seen for all devices        
    for (int i = 0; i < MAX_CONNECTED_DEVICES; i++) {
        if (devices[i].pointer)
            devices[i].lastSeen++;
    }
    DeviceManager::enumerateHardware(spec, deviceCallback, &info);

    // flag disconnected slots as disconnected (once only)
    // This also ensures that the first update posts the states of disconnected slots.
    for (int i = 0; i < MAX_CONNECTED_DEVICES; i++) {
        if (devices[i].lastSeen > 2) {
            clearSlot(i);
        }
        sendRemoveEvent(i);
    }
    usbPresenter.update();
    wifiPresenter.update();
}
