import cv2 as cv
import numpy as np
import glob
import os
from math import pi, cos, sin

# 棋盘的世界坐标系中的点
objectPoint = np.zeros((7 * 7, 3), np.float32)
X, Y = np.meshgrid(range(7), range(7))
objectPoint[:, 1], objectPoint[:, 0] = X.flatten(), Y.flatten()

objPoint_list = []
imgPoint_list = []  # 存11张照片的corners


def calibrateCamera():
    images = glob.glob("calibrateImg/*.jpg")

    for imgPath in images:
        img = cv.imread(imgPath)
        grayImg = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
        assert (grayImg.shape[::-1] == (3000, 4000))  # 检查照片的尺寸

        retval, corners = cv.findChessboardCorners(grayImg, (7, 7))

        if retval:
            cv.drawChessboardCorners(img, (7, 7), corners, retval)
            cv.imwrite(os.path.join("cornerImg", os.path.basename(imgPath)), img)
            objPoint_list.append(objectPoint)
            imgPoint_list.append(corners)

    retval, cameraMatrix, distCoeffs, rvecs, tvecs = cv.calibrateCamera(objPoint_list, imgPoint_list,
                                                                        grayImg.shape[::-1], None, None)

    return cameraMatrix, rvecs, tvecs


if __name__ == "__main__":
    cameraMatrix, rvecs, tvecs = calibrateCamera()

    # print
    print("手机相机的内存矩阵如下：\n", cameraMatrix)

    # 改变外参生成新图像
    imgPoint = imgPoint_list[0].reshape(7 * 7, -1)
    transform_matrix = np.zeros((3, 4), np.float32)

    # 绕z轴旋转90°矩阵
    rotateM_z = np.array([[0, -1, 0], [1, 0, 0], [0, 0, 0]])
    # 绕x轴旋转45°矩阵
    rotateM_x = np.array([[1, 0, 0], [0, cos(pi / 4), -sin(pi / 4)], [0, sin(pi / 4), cos(pi / 4)]])

    transform_matrix[:, :3] = rotateM_x.dot(rotateM_z)
    transform_matrix[:, 3] = (3, -5, 30)
    # 验证从某张照片能否用这个方法得到正确的corner位置
    # transform_matrix2 = np.zeros((3, 4), np.float32)
    # transform_matrix2[:, :3] = cv.Rodrigues(rvecs[0])[0]
    # transform_matrix2[:, 3] = tvecs[0].ravel()

    # 齐次化objectPoint
    objectPoint_homogeneuos = np.concatenate((objectPoint.T, np.ones((1, 7 * 7), np.float32)), axis=0)
    new_objectPoint = cameraMatrix.dot(transform_matrix).dot(objectPoint_homogeneuos)
    # 齐次项归一化
    new_imgPoint = (new_objectPoint[:2, :] / new_objectPoint[2, :]).T
    # 计算IMG_0和新坐标间的最优单映射变换矩阵
    H = cv.findHomography(imgPoint_list[0], new_imgPoint)[0]
    img = cv.imread("calibrateImg/IMG_0.jpg")

    new_img = cv.warpPerspective(img, H, (3000, 4000))

    cv.imwrite("new-image.jpg", new_img)
    cv.namedWindow("image", cv.WINDOW_NORMAL)
    cv.resizeWindow("image", 300, 400)
    cv.imshow("image", new_img)
    cv.waitKey(0)
    cv.destroyAllWindows()
