/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_SOCKET_SENSOR_H
#define ANDROID_SOCKET_SENSOR_H

#include <stdint.h>
#include <errno.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include "SensorBase.h"


#include "sensors.h"
//#include "InputEventReader.h"

/*****************************************************************************/

struct input_event;


typedef enum{
    ENABLE = 0,
    DELAY = 1
} sensor_config_key_t


typedef struct{
    int32_t handle;
    sensor_config_key_t key;
    int64_t value;
} sensor_config_message_t

typedef struct{
    int32_t handle;
    int32_t flag;
} sensor_config_ack_t


class SocketSensor : public SensorBase {
public:
            SocketSensor();
    virtual ~SocketSensor();

    enum {
		Accelerometer = 0,
        MagneticField,
        Orientation,
        numSensors
    };

    virtual int readEvents(sensors_event_t* data, int count);
    virtual int setDelay(int32_t handle, int64_t ns);
    virtual int setEnable(int32_t handle, int enabled);
    virtual int64_t getDelay(int32_t handle);
    virtual int getEnable(int32_t handle);

    virtual int getFd() const;
    static void * tcpThread(void*);



private:
    int mEnabled[numSensors];
	int64_t mDelay[numSensors];
    sensors_event_t mPendingEvents[numSensors];

    int command_socket_fd = -1;
    int data_socket_fd = -1;
    
    static int command_conn_fd;
    int data_conn_fd = -1;

	int handle2id(int32_t handle);


};

/*****************************************************************************/

#endif  // ANDROID_SOCKET_SENSOR_H
