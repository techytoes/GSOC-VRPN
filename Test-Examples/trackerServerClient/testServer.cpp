#include <vrpn_Connection.h>
#include <vrpn_Tracker.h>

#include <stdio.h>  // fprintf()
#include <stdlib.h>  // exit()

#include <math.h>  // sin(), cos() for debugging

int main(int argc, char const *argv[])
{   
    static float angle = 0;
    angle += 0.001f;
    vrpn_float64 pos[3], d_quat[4];

    // the pos array contains the position value of the tracker
    // XXX Set your values here
    pos[0] = sin ( angle );
    pos[1] = 0.0f;
    pos[2] = 0.0f;

    // the d_quat array contains the orientation value of the tracker, stored as a quaternion
    // XXX Set your values here
    d_quat[0] = 0.0f;
    d_quat[1] = 0.0f;
    d_quat[2] = 0.0f;
    d_quat[3] = 1.0f;

    const vrpn_uint32 class_of_service = vrpn_CONNECTION_LOW_LATENCY;
    int sensor = 0;

    vrpn_Connection * c;
    vrpn_Tracker_Server * tracker;

    struct timeval delay;

    c = vrpn_create_server_connection();
    tracker = new vrpn_Tracker_Server ("Tracker0@localhost", c);

    delay.tv_sec = 0L;
    delay.tv_usec = 0L;

    while(1) {
        tracker->report_pose(sensor, delay, pos, d_quat, class_of_service);
        c->mainloop();
        fprintf(stderr, "Server Open");
    }
}
