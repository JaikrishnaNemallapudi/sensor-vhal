#ifndef _SENSORS_VHAL_H
#define _SENSORS_VHAL_H
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <log/log.h>
#include <queue>
#include <vector>
#include <condition_variable>
#include <cutils/sockets.h>
#include <hardware/sensors.h>
#include <time.h>
#include <cutils/properties.h>
#include <pthread.h>
#include "sock_utils.h"

#define  SENSORS_SERVICE_NAME "sensors"
#define MAX_NUM_SENSORS 3
#define SUPPORTED_SENSORS  ((1<<MAX_NUM_SENSORS)-1)

/**  SENSOR IDS AND NAMES **/
#define  ID_BASE                        SENSORS_HANDLE_BASE
#define  ID_ACCELERATION                (ID_BASE+0)
#define  ID_GYROSCOPE                   (ID_BASE+1)
#define  ID_MAGNETIC_FIELD              (ID_BASE+2)
#define  ID_ORIENTATION                 (ID_BASE+3)
#define  ID_TEMPERATURE                 (ID_BASE+4)
#define  ID_PROXIMITY                   (ID_BASE+5)
#define  ID_LIGHT                       (ID_BASE+6)
#define  ID_PRESSURE                    (ID_BASE+7)
#define  ID_HUMIDITY                    (ID_BASE+8)
#define  ID_MAGNETIC_FIELD_UNCALIBRATED (ID_BASE+9)

#define  SENSORS_ACCELERATION                 (1 << ID_ACCELERATION)
#define  SENSORS_GYROSCOPE                    (1 << ID_GYROSCOPE)
#define  SENSORS_MAGNETIC_FIELD               (1 << ID_MAGNETIC_FIELD)
#define  SENSORS_ORIENTATION                  (1 << ID_ORIENTATION)
#define  SENSORS_TEMPERATURE                  (1 << ID_TEMPERATURE)
#define  SENSORS_PROXIMITY                    (1 << ID_PROXIMITY)
#define  SENSORS_LIGHT                        (1 << ID_LIGHT)
#define  SENSORS_PRESSURE                     (1 << ID_PRESSURE)
#define  SENSORS_HUMIDITY                     (1 << ID_HUMIDITY)
#define  SENSORS_MAGNETIC_FIELD_UNCALIBRATED  (1 << ID_MAGNETIC_FIELD_UNCALIBRATED)

#define SENSOR_VHAL_PORT_PROP      "virtual.sensor.tcp.port"
#define SENSOR_VHAL_PORT           8772

#define DEBUG_OPTION          false
#define CMD_SENSOR_BATCH      0x11
#define CMD_SENSOR_ACTIVATE   0x22
#define MAX_MSG_QUEUE_SIZE    128

typedef struct {
    int32_t    cmd_type;         // acgmsg_sensor_conig_type_t
    int32_t    sensor_type;       // acgmsg_sensor_type_t
    union {
       int32_t    enabled;       // acgmsg_sensor_status_t for cmd: ACG_SENSOR_ACTIVATE
       int32_t    sample_period; // ACG_SENSOR_BATCH
    };
} sensor_config_msg_t;

typedef struct {
    union {
        struct {
            float x;
            float y;
            float z;
        };
        struct {
            float azimuth;
            float pitch;
            float roll;
        };
    };
} acgmsg_sensors_vec_t;

// ACG_MESSAGE_CLIENT_SENSOR payload
//now AIC only support acceleration, magnetic, gyro
typedef struct acgmsg_sensors_event_t {
    /* sensor type */
    int32_t type; // acgmsg_sensor_type_t
    /* time is in nanosecond */
    int64_t timestamp;
    union {
        union {
            /* acceleration values are in meter per second per second (m/s^2) */
            acgmsg_sensors_vec_t   acceleration;
            /* magnetic vector values are in micro-Tesla (uT) */
            acgmsg_sensors_vec_t   magnetic;
            /* orientation values are in degrees */
            acgmsg_sensors_vec_t   orientation;
            /* gyroscope values are in rad/s */
            acgmsg_sensors_vec_t   gyro;
            /* temperature is in degrees centigrade (Celsius) */
            float           temperature;
            /* distance in centimeters */
            float           distance;
            /* light in SI lux units */
            float           light;
            /* pressure in hectopascal (hPa) */
            float           pressure;
            /* relative humidity in percent */
            float           relative_humidity;
        };
    };
} acgmsg_sensors_event_t;

#define  SENSORS_LIST  \
    SENSOR_(ACCELERATION,"acceleration") \
    SENSOR_(GYROSCOPE,"gyroscope") \
    SENSOR_(MAGNETIC_FIELD,"magnetic-field") \

static const struct {
    const char*  name;
    int          id; } _sensorIds[MAX_NUM_SENSORS] =
{
#define SENSOR_(x,y)  { y, ID_##x },
    SENSORS_LIST
#undef  SENSOR_
};

class SensorDevice {
public:
    struct sensors_poll_device_1  device; //must be first
    SensorDevice();
    ~SensorDevice();
    int sensor_device_poll(sensors_event_t* data, int count);
    int sensor_device_activate(int handle, int enabled);
    int sensor_device_flush(int handle);
    int sensor_device_set_delay(int handle, int64_t ns);
    int sensor_device_batch(int sensor_handle, int64_t sampling_period_ns);

private:
    sensors_event_t               m_sensors[MAX_NUM_SENSORS];
    uint32_t                      m_flush_count[MAX_NUM_SENSORS];
    uint32_t                      m_pending_sensors;
    int64_t                       m_time_start;
    int64_t                       m_time_offset;
    pthread_mutex_t               m_lock;
    SockServer*                   m_socket_server;
    std::mutex                    m_msg_queue_mutex;
    std::condition_variable       m_msg_queue_ready_cv;
    std::condition_variable       m_msg_queue_empty_cv;
    std::queue<acgmsg_sensors_event_t> m_msg_queue;
    sensor_config_msg_t           m_sensor_activate_status[MAX_NUM_SENSORS];
    sensor_config_msg_t           m_sensor_batch_status[MAX_NUM_SENSORS];

private:
    int64_t now_ns(void);
    int get_type_from_hanle(int handle);
    const char* get_name_from_handle(int id);
    int sensor_device_poll_event_locked();
    int sensor_device_send_config_msg(const void* cmd, size_t len);
    int sensor_device_pick_pending_event_locked(sensors_event_t*  event);
    void sensor_event_callback(SockServer *sock, sock_client_proxy_t* client);
    void client_connected_callback(SockServer *sock, sock_client_proxy_t* client);
};

#endif
