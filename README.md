# hand_gesture_detect
运用OPENCV，先颜色分离，分离出手部肤色，再根据凸包点算法定位手指坐标，根据坐标数据变化判断手势，可判断单双击，缩放手势

用H（颜色信息）S（颜色百分比）V（亮度百分比），人的皮肤大约在H=0和H=50，S等于58到S等于173之间

然后我们用threshold设置阈值，进行二值化。然后用median blur和dilate等图像手段进一步细化二值化区域。

用findContours找到最大的那块区域，即得到手掌的区域。

对这块区域，我们用Convex Hull包围盒，找到端点和凸包点，它们的连线即为五指。

检测滑动放大手势：
![Image text](https://github.com/hujinxinb/hand_gesture_detect/blob/master/img/1.jpg)
