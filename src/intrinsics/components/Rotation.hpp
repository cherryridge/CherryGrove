#pragma once

namespace Components {
    struct Rotation {
        //Range: [0.0, 360.0).
        //0.0 is facing south (+Z), 90.0 is facing east (+X).
        double yaw;
        //-90.0 is up (+Y in WP), 90.0 is down (-Y in WP).
        //Range: (-90.0, 90.0).
        double pitch;
    };
}