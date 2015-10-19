//
// Created by ryosuke on 15/10/19.
//

#ifndef MY_JACO_DRIVER_JACOAPI_H
#define MY_JACO_DRIVER_JACOAPI_H


class JacoAPI {
public:
    JacoAPI() {
        //We load the library
        commandLayer_handle = dlopen("Kinova.API.USBCommandLayerUbuntu.so", RTLD_NOW | RTLD_GLOBAL);
        if (commandLayer_handle == NULL) {
            std::cout << dlerror() << std::endl;
        }
        MyInitAPI = (int (*)()) dlsym(commandLayer_handle, "InitAPI");
        MyCloseAPI = (int (*)()) dlsym(commandLayer_handle, "CloseAPI");
        MyMoveHome = (int (*)()) dlsym(commandLayer_handle, "MoveHome");
        MyInitFingers = (int (*)()) dlsym(commandLayer_handle, "InitFingers");
        MyGetDevices = (int (*)(KinovaDevice devices[MAX_KINOVA_DEVICE], int &result)) dlsym(commandLayer_handle,
                                                                                             "GetDevices");
        MySetActiveDevice = (int (*)(KinovaDevice devices)) dlsym(commandLayer_handle, "SetActiveDevice");
        MySendBasicTrajectory = (int (*)(TrajectoryPoint)) dlsym(commandLayer_handle, "SendBasicTrajectory");
        MySendAdvanceTrajectory = (int (*)(TrajectoryPoint)) dlsym(commandLayer_handle, "SendAdvanceTrajectory");
        MyGetAngularCommand = (int (*)(AngularPosition &)) dlsym(commandLayer_handle, "GetAngularCommand");
    }

    ~JacoAPI() {
        dlclose(commandLayer_handle);
    }

    int (*MyInitAPI)();
    int (*MyCloseAPI)();
    int (*MySendBasicTrajectory)(TrajectoryPoint command);
    int (*MySendAdvanceTrajectory)(TrajectoryPoint command);
    int (*MyGetDevices)(KinovaDevice devices[MAX_KINOVA_DEVICE], int &result);
    int (*MySetActiveDevice)(KinovaDevice device);
    int (*MyMoveHome)();
    int (*MyInitFingers)();
    int (*MyGetAngularCommand)(AngularPosition &);
private:
    void *commandLayer_handle;
};


#endif //MY_JACO_DRIVER_JACOAPI_H
