# Grab_UsingGrabLoopThread.cpp
# Grab_UsingGrabLoopThread.cpp
# This sample illustrates how to grab and process images using the grab loop thread
# provided by the Instant Camera class.

from pypylon import genicam
from pypylon import pylon

import queue
import cv2
import numpy as np
import sys
import argparse
import json

def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--stereo_config', help='File with stereo camera calibration.')
    parser.add_argument('--out_stereo_config', default='calibration_output.json', help='File to write stereo calibration to.')
    args = parser.parse_args()
    return args


class ImageEventPrinter(pylon.ImageEventHandler):
    def OnImagesSkipped(self, camera, countOfSkippedImages):
        print("OnImagesSkipped event for device ", camera.GetDeviceInfo().GetModelName())
        print(countOfSkippedImages, " images have been skipped.")
        print()

    def OnImageGrabbed(self, camera, grabResult):
        print("OnImageGrabbed event for device ", camera.GetDeviceInfo().GetModelName())

        # Image grabbed successfully?
        if grabResult.GrabSucceeded():
            print("SizeX: ", grabResult.GetWidth())
            print("SizeY: ", grabResult.GetHeight())
            img = grabResult.GetArray()
            print("Gray values of first row: ", img[0])
            print()
        else:
            print("Error: ", grabResult.GetErrorCode(), grabResult.GetErrorDescription())


class CameraEventPrinter(pylon.CameraEventHandler):
    def OnCameraEvent(self, camera, userProvidedId, node):
        print("OnCameraEvent event for device ", camera.GetDeviceInfo().GetModelName())
        print("User provided ID: ", userProvidedId)
        print("Event data node name: ", node.GetName())
        value = genicam.CValuePtr(node)
        if value.IsValid():
            print("Event node data: ", value.ToString())
        print()


class ConfigurationEventPrinter(pylon.ConfigurationEventHandler):
    def OnAttach(self, camera):
        print("OnAttach event")

    def OnAttached(self, camera):
        print("OnAttached event for device ", camera.GetDeviceInfo().GetModelName())

    def OnOpen(self, camera):
        print("OnOpen event for device ", camera.GetDeviceInfo().GetModelName())

    def OnOpened(self, camera):
        print("OnOpened event for device ", camera.GetDeviceInfo().GetModelName())

    def OnGrabStart(self, camera):
        print("OnGrabStart event for device ", camera.GetDeviceInfo().GetModelName())

    def OnGrabStarted(self, camera):
        print("OnGrabStarted event for device ", camera.GetDeviceInfo().GetModelName())

    def OnGrabStop(self, camera):
        print("OnGrabStop event for device ", camera.GetDeviceInfo().GetModelName())

    def OnGrabStopped(self, camera):
        print("OnGrabStopped event for device ", camera.GetDeviceInfo().GetModelName())

    def OnClose(self, camera):
        print("OnClose event for device ", camera.GetDeviceInfo().GetModelName())

    def OnClosed(self, camera):
        print("OnClosed event for device ", camera.GetDeviceInfo().GetModelName())

    def OnDestroy(self, camera):
        print("OnDestroy event for device ", camera.GetDeviceInfo().GetModelName())

    def OnDestroyed(self, camera):
        print("OnDestroyed event")

    def OnDetach(self, camera):
        print("OnDetach event for device ", camera.GetDeviceInfo().GetModelName())

    def OnDetached(self, camera):
        print("OnDetached event for device ", camera.GetDeviceInfo().GetModelName())

    def OnGrabError(self, camera, errorMessage):
        print("OnGrabError event for device ", camera.GetDeviceInfo().GetModelName())
        print("Error Message: ", errorMessage)

    def OnCameraDeviceRemoved(self, camera):
        print("OnCameraDeviceRemoved event for device ", camera.GetDeviceInfo().GetModelName())


# Example of an image event handler.
class SampleImageEventHandler(pylon.ImageEventHandler):
    def __init__(self, q, cam_id):
        super().__init__()
        self.grab_times = [0.0]
        self.q = q
        self.cam_id = cam_id

    def OnImageGrabbed(self, camera, grabResult):
        cam_id = camera.GetDeviceInfo().GetDeviceGUID()
        if cam_id == '2676015C446C':
            cam_id = 0
        else:
            cam_id = 1
        self.q.put((cam_id, grabResult.GetArray()))


class ImageSource:
    def __init__(self, ):
        num_cameras = 2
        q = queue.Queue(4)
        cameras = [None, None]
        handlers = [None, None]
        device_info_list = pylon.TlFactory.GetInstance().EnumerateDevices()
        for info in device_info_list[:num_cameras]:
            print(info.GetDeviceGUID())
            if info.GetDeviceGUID() == '2676015C4468':
                cam_id = 1
            else:
                cam_id = 0

            cam = pylon.InstantCamera(
                pylon.TlFactory.GetInstance().CreateFirstDevice(info))
            cam.RegisterConfiguration(pylon.SoftwareTriggerConfiguration(),
                                      pylon.RegistrationMode_ReplaceAll,
                                      pylon.Cleanup_Delete)
            cam.RegisterConfiguration(ConfigurationEventPrinter(),
                                      pylon.RegistrationMode_Append,
                                      pylon.Cleanup_Delete)
            handlers[cam_id] = SampleImageEventHandler(q, cam_id)
            cam.RegisterImageEventHandler(handlers[cam_id],
                                          pylon.RegistrationMode_Append,
                                          pylon.Cleanup_Delete)
            cam.StartGrabbing(pylon.GrabStrategy_OneByOne,
                              pylon.GrabLoop_ProvidedByInstantCamera)
            cameras[cam_id] = cam

        self.q = q
        self.cameras = cameras

    def get_images(self):
        for cam in self.cameras:
            cam.ExecuteSoftwareTrigger()
        images = [None, None]
        for i in range(2):
            id, img = self.q.get()
            if id == 0:
                img = img[::-1, ::-1]

            images[i] = img

        print(self.q.qsize())
        return images


class StereoConfig:
    def __init__(self):
        # intrinsic
        self.K = [None, None]
        self.D = [None, None]

        # stereo params
        self.R = None
        self.T = None
        self.E = None
        self.F = None

        # rectification params
        self.R1 = None
        self.R2 = None
        self.P1 = None
        self.P2 = None
        self.Q  = None
        self.validPixROI1 = None
        self.validPixROI2 = None

        self.mapx_0 = None
        self.mapy_0 = None
        self.mapx_1 = None
        self.mapy_1 = None

    def load_json(self, json_str):
        params = json.loads (json_str)

        if 'K1' in params:
            self.K[0] = np.asarray(params['K1']).reshape(3, 3)
        if 'K2' in params:
            self.K[1] = np.asarray(params['K2']).reshape(3, 3)
        if 'D1' in params:
            self.D[0] = np.asarray(params['D1'])
        if 'D2' in params:
            self.D[1] = np.asarray(params['D2'])

        if 'R' in params:
            self.R = np.asarray(params['R']).reshape(3, 3)
        if 'T' in params:
            self.T = np.asarray(params['T'])
        if 'E' in params:
            self.E = np.asarray(params['E']).reshape(3, 3)
        if 'F' in params:
            self.F = np.asarray(params['F']).reshape(3, 3)
        if 'Q' in params:
            self.Q = np.asarray(params['Q']).reshape(4, 4)

    def get_json(self):
        params = {}
        if self.K[0] is not None:
            params['K1'] = self.K[0].reshape(-1).tolist()
        if self.K[1] is not None:
            params['K2'] = self.K[1].reshape(-1).tolist()
        if self.D[0] is not None:
            params['D1'] = self.D[0].reshape(-1).tolist()
        if self.D[1] is not None:
            params['D2'] = self.D[1].reshape(-1).tolist()

        if self.R is not None:
            params['R'] = self.R.reshape(-1).tolist()
        if self.T is not None:
            params['T'] = self.T.reshape(-1).tolist()
        if self.E is not None:
            params['E'] = self.E.reshape(-1).tolist()
        if self.F is not None:
            params['F'] = self.F.reshape(-1).tolist()
        if self.Q is not None:
            params['Q'] = self.Q.reshape(-1).tolist()

        return json.dumps(params, indent=4, sort_keys=True)

    def fill_intrinsics(self, cam_id):
        self.K[cam_id] = np.zeros((3, 3))
        self.K[cam_id][0, 0] = 3660
        self.K[cam_id][1, 1] = 3660
        self.K[cam_id][0, 2] = 800
        self.K[cam_id][1, 2] = 600
        self.K[cam_id][2, 2] = 1
        self.D[cam_id] = np.zeros(4)

    def have_camera_intrinsics(self, cam_id):
        return self.K[cam_id] is not None and self.K[cam_id] is not None

    def have_stereo_params(self):
        return self.R is not None and self.T is not None and self.E is not None and self.F is not None and self.Q is not None

    def export_openvslam(self):
        print('Camera.fx:', self.P2[0, 0])
        print('Camera.fy:', self.P2[1, 1])
        print('Camera.cx:', self.P2[0, 2])
        print('Camera.cy:', self.P2[1, 2])
        print('Camera.focal_x_baseline:', -self.P2[0][3])
        print('Camera.fps: 20')
        print('Camera.colxs:', self.mapx_0.shape[1])
        print('Camera.rows:', self.mapx_0.shape[0])

        print('Camera.k1: 0')
        print('Camera.k2: 0')
        print('Camera.p1: 0')
        print('Camera.p2: 0')
        print('Camera.k3: 0')

        print('StereoRectifier.K_left: [{}]'.format(', '.join([str(x) for x in self.K[0].reshape(-1)])))
        print('StereoRectifier.D_left: [{}]'.format(', '.join([str(x) for x in self.D[0]])))
        print('StereoRectifier.R_left: [{}]'.format(', '.join([str(x) for x in self.R1.reshape(-1)])))

        print('StereoRectifier.K_right: [{}]'.format(', '.join([str(x) for x in self.K[1].reshape(-1)])))
        print('StereoRectifier.D_right: [{}]'.format(', '.join([str(x) for x in self.D[1]])))
        print('StereoRectifier.R_right: [{}]'.format(', '.join([str(x) for x in self.R2.reshape(-1)])))


corner_criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)


def calibrate_intrinsics(stereo_config: StereoConfig, cam_id: int, object_points, pattern_size, image_source: ImageSource):
    subsample = 4

    image_points = []
    while True:
        images = image_source.get_images()
        img = images[cam_id]

        ret, corners = cv2.findChessboardCorners(img[::subsample, ::subsample], pattern_size, None)
        if ret:
            corners *= subsample
            corners2 = cv2.cornerSubPix(img, corners, (11, 11), (-1, -1), corner_criteria)
            img = np.stack([img, img, img], axis=2)
            img = cv2.drawChessboardCorners(img, pattern_size, corners2, ret)

        cv2.imshow(str(cam_id), cv2.resize(img, (0, 0), fx=0.5, fy=0.5))
        key = cv2.waitKey(1)

        if key == ord(' ') and ret:
            image_points.append(corners2)
            print('New Points')
        elif key == ord('x'):
            break

    object_points = [object_points for i in range(len(image_points))]
    ret, K, D, rvecs, tvecs = cv2.calibrateCamera(
        object_points, image_points, (1600, 1200),
        None, None, flags=cv2.CALIB_FIX_PRINCIPAL_POINT | cv2.CALIB_FIX_ASPECT_RATIO)

    if ret:
        stereo_config.K[cam_id] = K
        stereo_config.D[cam_id] = D

    print('K', cam_id, K)
    print('D', cam_id, D)

    return ret



def calibrate_stereo(stereo_config: StereoConfig, object_points, pattern_size, image_source: ImageSource):
    subsample = 4
    all_points = []

    while True:
        image_points = [None, None]
        images = image_source.get_images()
        for id, img in enumerate(images):

            ret, corners = cv2.findChessboardCorners(img[::subsample, ::subsample], pattern_size, None)
            if ret:
                corners *= subsample
                corners2 = cv2.cornerSubPix(img, corners, (11, 11), (-1, -1), corner_criteria)
                img = np.stack([img, img, img], axis=2)
                img = cv2.drawChessboardCorners(img, pattern_size, corners2, ret)
                image_points[id] = corners2
            cv2.imshow(str(id), cv2.resize(img, (0, 0), fx=0.5, fy=0.5))

        key = cv2.waitKey(10)
        if key == ord(' '):
            if image_points[0] is not None and image_points[1] is not None:
                all_points.append(image_points)
                print('New Points')
        elif key == ord('x'):
            break

    object_points = [object_points for i in range(len(all_points))]
    all_image_points = np.stack(all_points, axis=0)

    retval, K1, D1, K2, D2, R, T, E, F = cv2.stereoCalibrate(
        object_points, all_image_points[:, 0], all_image_points[:, 1],
        stereo_config.K[0], stereo_config.D[0], stereo_config.K[1], stereo_config.D[1], (1600, 1200),
        flags=cv2.CALIB_FIX_INTRINSIC)

    if retval:
        print('R', R)
        print('T', T)
        print('E', E)
        print('F', F)
        stereo_config.R = R
        stereo_config.T = T
        stereo_config.E = E
        stereo_config.F = F

    return retval


def recompute_rectification(stereo_config):
    stereo_config.R1, stereo_config.R2, stereo_config.P1, stereo_config.P2, \
    stereo_config.Q, stereo_config.validPixROI1, stereo_config.validPixROI2 = cv2.stereoRectify(
        stereo_config.K[0], stereo_config.D[0], stereo_config.K[1], stereo_config.D[1], (1600, 1200),
        stereo_config.R, stereo_config.T,
        flags=cv2.CALIB_ZERO_DISPARITY, alpha=1)

    stereo_config.mapx_0, stereo_config.mapy_0 = cv2.initUndistortRectifyMap(
        stereo_config.K[0], stereo_config.D[0], stereo_config.R1, stereo_config.P1, (1600, 1200), cv2.CV_32F)
    stereo_config.mapx_1, stereo_config.mapy_1 = cv2.initUndistortRectifyMap(
        stereo_config.K[1], stereo_config.D[1], stereo_config.R2, stereo_config.P2, (1600, 1200), cv2.CV_32F)


def calibrate_cameras(stereo_config: StereoConfig, pattern_size, image_source: ImageSource, square_size=1):
    object_points = np.zeros((pattern_size[0] * pattern_size[1], 3), np.float32)
    object_points[:, :2] = np.mgrid[0:pattern_size[0], 0:pattern_size[1]].T.reshape(-1, 2) * square_size

    for cam_id in range(2):
        if not stereo_config.have_camera_intrinsics(cam_id):
            calibrate_intrinsics(stereo_config, cam_id, object_points, pattern_size, image_source)

        print(stereo_config.get_json())

    if not stereo_config.have_stereo_params():
        calibrate_stereo(stereo_config, object_points, pattern_size, image_source)

    recompute_rectification(stereo_config)


def main():

    args = parse_args()
    image_source = ImageSource()

    pattern_size = (5, 4)

    stereo_config = StereoConfig()
    if args.stereo_config:
        with open(args.stereo_config, 'r') as f:
            stereo_config.load_json(f.read())

    calibrate_cameras(stereo_config, pattern_size, image_source)

    if args.out_stereo_config:
        with open(args.out_stereo_config, 'w') as f:
            f.write(stereo_config.get_json())

    stereo_config.export_openvslam()

    window_size = 7
    stereoProcessor = cv2.StereoSGBM_create(
        minDisparity=64,
        numDisparities=256,  # max_disp has to be dividable by 16 f. E. HH 192, 256
        blockSize=27,
        P1=4 * 3 * window_size ** 2,
    #    # wsize default 3; 5; 7 for SGBM reduced size image; 15 for SGBM full size image (1300px and above); 5 Works nicely
        P2=8 * 3 * window_size ** 2,
        disp12MaxDiff=10,
        uniquenessRatio=15,
        speckleWindowSize=5,
        speckleRange=2,
        preFilterCap=63,
        mode=cv2.STEREO_SGBM_MODE_SGBM_3WAY
    )

    while True:
        images = image_source.get_images()


        images[0] = cv2.remap(images[0], stereo_config.mapx_0, stereo_config.mapy_0, cv2.INTER_LINEAR)
        images[1] = cv2.remap(images[1], stereo_config.mapx_1, stereo_config.mapy_1, cv2.INTER_LINEAR)
        scale = 0.5
        small = (cv2.resize(images[0], (0, 0), fx=scale, fy=scale, interpolation=cv2.INTER_AREA),
                 cv2.resize(images[1], (0, 0), fx=scale, fy=scale, interpolation=cv2.INTER_AREA))
        cv2.imshow('0', small[0])
        cv2.imshow('1', small[1])

        disparity = stereoProcessor.compute(small[0], small[1]).astype(np.float32)

        disparity -= disparity.min()
        disparity /= disparity.max() / 255;
        disparity = cv2.applyColorMap(disparity.astype(np.uint8), cv2.COLORMAP_JET)
        cv2.imshow('disparity', disparity)

        key = cv2.waitKey(1)
        if key == ord(' '):
            break


if __name__ == '__main__':
    main()





if False:
    try:
        np_object_points = np.load('pattern.npy')
    except:
        criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)

        all_points = []
        while True:
            points = [None, None]
            images = get_images(q, cameras)
            for id, img in enumerate(images):

                subsample = 6
                ret, corners = cv2.findChessboardCorners(img[::subsample, ::subsample], pattern_size, None)
                if ret == True:
                    corners *= subsample
                    corners2 = cv2.cornerSubPix(img, corners, (11, 11), (-1, -1), criteria)
                    points[id] = corners2
                    img = np.stack([img, img, img], axis=2)
                    img = cv2.drawChessboardCorners(img, pattern_size, corners2, ret)
                cv2.imshow(str(id), cv2.resize(img, (0,0), fx=0.5, fy=0.5))

            key = cv2.waitKey(1)
            if key == ord(' '):
                if points[0] is not None and points[1] is not None:
                    all_points.append(points)
                    print('New Points', len(points))
            elif key == ord('x'):
                break

        if all_points:
            np_object_points = np.stack(all_points, axis=0)
        else:
            np_object_points = None
        #np.save('pattern.npy', np_object_points)