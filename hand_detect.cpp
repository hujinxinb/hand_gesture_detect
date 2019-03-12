#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/opencv.hpp"
#include <string>
#include <vector>


using namespace cv;
using namespace std;

//放大的时候开始要慢慢的，单击的时候尽量按久些，双击的时候尽量按快些

//全局变量
cv::Rect tar_box(-1,-1,-1,-1);//标志为空的Rect




//1号摄像头里的按钮个数及各按钮所在区域的坐标
int BUTTON_NUMBER=3;

//cv::Rect rightview_click_area[12]={cv::Rect(158,317,38,23),cv::Rect(275,323,40,23),cv::Rect(270,346,46,22),cv::Rect(268,368,46,25),cv::Rect(235,323,40,23)
//        ,cv::Rect(229,346,41,22),cv::Rect(224,368,42,25),cv::Rect(196,320,39,22),cv::Rect(187,343,41,22),
//                                   cv::Rect(180,365,43,25),cv::Rect(142,340,44,23),cv::Rect(133,363,46,25)};


//cv::Rect rightview_click_area[1]={cv::Rect(126,252,97,61)};
cv::Rect rightview_click_area[3]={cv::Rect(464,213,41,31), cv::Rect(390,288,51,34), cv::Rect(375,171,45,36)};

//1号摄像头里的全屏幕坐标区域
Rect screen1_area = cv::Rect(37,52,568,406);


//0号摄像头里的滑动坐标区域
Rect centerview_scale_area = cv::Rect(114,141,387,242);


//右指的指尖坐标序列
vector<cv::Point> rightview_finger_series;

//缩放判断所需全局变量
int no_scale_count=0;
vector<int> scale_dis;
bool judge_scale=false;
int judge_scale_later = 0;
int scale_dis_most = 0,scale_dis_least = 10000;
bool judge_scale_up = false,judge_scale_down = false;

//根据0摄像头判断是否是单指
bool single_finger = true;

//显示结果用
int show_result = 0,show_resultc = 0 ,show_resultd=0;


//单双击指尖在不在按钮坐标区域内
bool is_tip_in_box(cv::Point point,cv::Rect box)
{
    int x=point.x;
    int y=point.y;

    if((box.x<=x)&&(x<=box.x+box.width) &&(box.y<=y)&&(y<=box.y+box.height))
        return true;

    return false;
}


int get_sto_tim(std::vector<cv::Point> sequence, cv::Rect box) {
    int time = 0;
    int point_num = sequence.size();
    for (int i=0;i<point_num;i++) {
        if (is_tip_in_box(sequence[i], box)) {
            time += 1;
        }
    }
    return time;
}


cv::Rect get_but_pos(std::vector<cv::Point> sequence,cv::Rect boxes[])
{
    cv::Rect cur_box=cv::Rect(-1,-1,-1,-1);
    for (int i = 0; i < BUTTON_NUMBER; i++) {
        int sto_tim = get_sto_tim(sequence, boxes[i]);
        if (sto_tim>1)
        {
            cur_box = boxes[i];
        }
    }
    return cur_box;
}


////返回-1是没有点击，返回0是单击，返回1是双击
//int diff_click(std::vector<cv::Point>& cli_seq)
//{
//
//    if (cli_seq.size()<1)
//    {
//        cout<<"click sequence is null!"<<endl;
//        return -1;
//    }
//    std::vector<int> dy_seq;
//
//    int length=cli_seq.size();
//
////    cout<<"cli_seq.size()="<<cli_seq.size()<<endl;
//
//    for(int i=0;i<length;i++)
//    {
//        if (is_tip_in_box(cli_seq[i],tar_box))
//            dy_seq.push_back(1);
//        else
//            dy_seq.push_back(0);
//    }
////        for (int i = 0; i < length; i++) {
//////            if((dy_seq[i]!=dy_seq[i+1])&&(dy_seq[i+1]!=dy_seq[i+2]))
//////            {
//////                dy_seq[i+1]=dy_seq[i];
//////            }
////            std::cout<<dy_seq[i];
////        }
//
//    std::string str="";
//
//
//    int  lst_dy=dy_seq[0];
//    int min_cnt=0;
//
////    cout<<"length"<<length<<endl;
////    for (int i=0;i<dy_seq.size();i++)
////        cout<<dy_seq.at(i);
////    cout<<endl;
//
//    for (int i = 0; i < length; i++)
//    {
//
//        int cur_dy=dy_seq[i];
//        if (cur_dy==lst_dy)
//        {
//            min_cnt++;
//        }
//        else
//        {
//
//            if (min_cnt>=1)
//            {
//                if (lst_dy==0)
//                    str+="0";
//                else
//                    str+="1";
//            }
//            lst_dy=cur_dy;
//            min_cnt=1;
//        }
//
//        if(i==length-1)
//        {
//            if (min_cnt>5&&lst_dy==0)
//                str+="0";
//            if (min_cnt>=1&&lst_dy==1)
//                str+="1";
//        }
//    }
//
//    cout<<"str="<<str<<endl;
//    if (str=="01010"||str=="0101"||str=="101")
//        return 1;
//    if (str=="010"||str=="01") {
//        return 0;
//    }
//
////    cout<<str<<endl;
//    return -1;
//}



//返回-1是没有点击，返回0是单击，返回1是双击
int diff_click(std::vector<cv::Point>& cli_seq)
{

    if (cli_seq.size()<1)
    {
        cout<<"click sequence is null!"<<endl;
        return -1;
    }
    std::vector<int> dy_seq;

    int length=cli_seq.size();

//    for(int i=0;i<length;i++)
//    {
//        if (is_tip_in_box(cli_seq[i],tar_box))
//            dy_seq.push_back(1);
//        else
//            dy_seq.push_back(0);
//    }
    int last_dy=-1;
    int interval=0;

    for(int i=0;i<length-1;i++)
    {
        if(cli_seq[i+1].y-cli_seq[i].y>0||!(is_tip_in_box(cli_seq[i],tar_box)))
        {
            dy_seq.push_back(0);
        }
        else
        {
            dy_seq.push_back(1);
        }
    }

//    for(int i=1;i<length-1;i++)
//    {
//        int cur_dy=-1;
//        if(cli_seq[i+1].y-cli_seq[i].y>0)
//        {
//            cur_dy=0;
//        }
//        else
//        {
//            cur_dy=1;
//        }
//        if (last_dy==cur_dy) {
//            if (last_dy == 1) {
//                dy_seq.push_back(1);
//            } else {
//                dy_seq.push_back(0);
//            }
//        }
//        else
//        {
//            interval++;
//            if(interval>1)
//            {
//                if (cur_dy==1)
//                {
//                    dy_seq.push_back(1);
//                } else{
//                    dy_seq.push_back(0);
//                }
//                interval=0;
//            }
//
//        }
//        last_dy=cur_dy;
//    }
//    dy_seq={0,0,0,0,0,0,1,1,1,0,1,1,1,1,1,1,1,1,0-1};
    dy_seq.push_back(-1);
    length=dy_seq.size();
    for (int j=0;j<length;j++)
    {
        //****************cout<<dy_seq[j];
    }
    //**************cout<<endl;
    std::string str="";
    int  lst_dy=dy_seq[0];
    int min_cnt=1;
    for (int i = 0; i < length; i++)
    {

        int cur_dy=dy_seq[i];
        if (cur_dy==lst_dy)
        {
            min_cnt++;
        }
        else
        {

            if (min_cnt>1)
            {
                if (dy_seq[i+1]==lst_dy)
                {
                    continue;
                }
                if (lst_dy==0)
                    str+="0";
                else
                    str+="1";

            }
            lst_dy=cur_dy;
            min_cnt=1;
        }

        if (i==length-1&&min_cnt>1)
        {
            if (lst_dy==0)
                str+="0";
            else
                str+="1";
        }


//        if(i==length-1)
//        {
//            if (min_cnt>0&&lst_dy==0)
//                str+="0";
//            if (min_cnt>0&&lst_dy==1)
//                str+="1";
//        }
    }

    //************************************   cout<<"str="<<str<<endl;
    if (str=="01010"||str=="0101"||str=="101")
        return 1;
    if (str=="010"||str=="01") {
        return 0;
    }

//    cout<<str<<endl;
    return -1;
}





//判断是否找到全剧变量tarbox,也就是是否有按钮被点击
bool is_tar_box_null(cv::Rect box)
{
    if (box.width==-1)
    {
        return true;
    }
    return false;
}







//返回0是未检测到滑动，返回1是检测到scale_up，返回2是检测到scale_down
int JudgeScale(cv::Point p1,cv::Point p2)
{
    if(no_scale_count>=4)
    {
        scale_dis.clear();
        no_scale_count=0;
    }

    if(judge_scale==true)
    {
        judge_scale_later+=1;
        if(judge_scale_later>=12)
        {
            judge_scale=false;
            judge_scale_later=0;
        }
    }



    if( centerview_scale_area.x<=p1.x && p1.x<=centerview_scale_area.x+ centerview_scale_area.width &&
        centerview_scale_area.x<=p2.x && p2.x<=centerview_scale_area.x+ centerview_scale_area.width &&
        centerview_scale_area.y<=p1.y && p1.y<=centerview_scale_area.y+ centerview_scale_area.height &&
        centerview_scale_area.y<=p2.y && p2.y<=centerview_scale_area.y+ centerview_scale_area.height &&
        judge_scale==false)
    {
        int dis = (int)(sqrt(pow(p1.x-p2.x,2)+pow(p1.y-p2.y,2)));
        scale_dis.push_back(dis);

        int frame_now = scale_dis.size()-1;
        int i=0;

        if(judge_scale_up && dis>=scale_dis_most)
        {
            scale_dis_most=dis;
            scale_dis.clear();
            return 0;
        }

        if(judge_scale_up && dis<scale_dis_most){
            judge_scale=true;
            scale_dis.clear();
            judge_scale_later=0;
            judge_scale_up=false;
            scale_dis_most=0;
            return 0;
        }

        if(judge_scale_down && dis<=scale_dis_least)
        {
            scale_dis_least=dis;
            scale_dis.clear();
            return 0;
        }

        if(judge_scale_down && dis>scale_dis_least)
        {
            judge_scale=true;
            scale_dis.clear();
            judge_scale_later=0;
            judge_scale_down=false;
            scale_dis_least=10000;
            return 0;
        }

        //判断是scale_up
        while(i<scale_dis.size()-1)
        {
            if(scale_dis[frame_now] - scale_dis[i] > 20)
            {
                if(frame_now - i > 2 && scale_dis[i]<120)
                {
                    int j=i;
                    while(j<frame_now)
                    {
                        if(scale_dis[j+1]>=scale_dis[j])
                            j+=1;
                        else
                            break;
                    }

                    if(j==frame_now)
                    {
                        //判断出是一次scale_up
                        cout<<"scale up"<<endl;
                        show_result = 1;
                        judge_scale_up=true;
                        scale_dis_most = scale_dis[frame_now];
                        scale_dis.clear();
                        return 1;
                    }

                }
            }

            i+=1;
        }

        i=0;
        //判断是scale_down
        while(i<scale_dis.size()-1)
        {
            if(scale_dis[frame_now] - scale_dis[i] < -20)
            {
                if(frame_now - i > 2 && scale_dis[frame_now]<120)
                {
                    int j=i;
                    while(j<frame_now)
                    {
                        if(scale_dis[j+1]<=scale_dis[j])
                            j+=1;
                        else
                            break;
                    }

                    if(j==frame_now)
                    {
                        //判断出是一次scale_down
                        cout<<"scale down"<<endl;
                        show_result = -1;
                        judge_scale_down=true;
                        scale_dis_least = scale_dis[frame_now];
                        scale_dis.clear();
                        return 2;
                    }

                }
            }

            i+=1;
        }

    }

    else
    {
        no_scale_count+=1;
    }
    return 0;
}





//计算最大轮廓
int max_contour(vector<vector<cv::Point> > contours)
{
    int largestContour = 0;
    for (int i = 1; i < contours.size(); i++)
    {
        if (cv::contourArea(contours[i]) > cv::contourArea(contours[largestContour]))
            largestContour = i;
    }
    return largestContour;
}



//求指头与指头间的夹角
float innerAngle(float px1, float py1, float px2, float py2, float cx1, float cy1)
{
    float dist1 = std::sqrt(  (px1-cx1)*(px1-cx1) + (py1-cy1)*(py1-cy1) );
    float dist2 = std::sqrt(  (px2-cx1)*(px2-cx1) + (py2-cy1)*(py2-cy1) );
    float Ax, Ay;
    float Bx, By;
    float Cx, Cy;

    //find closest point to C
    //printf("dist = %lf %lf\n", dist1, dist2);
    Cx = cx1;
    Cy = cy1;
    if(dist1 < dist2)
    {
        Bx = px1;
        By = py1;
        Ax = px2;
        Ay = py2;

    }else{
        Bx = px2;
        By = py2;
        Ax = px1;
        Ay = py1;
    }
    float Q1 = Cx - Ax;
    float Q2 = Cy - Ay;
    float P1 = Bx - Ax;
    float P2 = By - Ay;

    float A = std::acos( (P1*Q1 + P2*Q2) / ( std::sqrt(P1*P1+P2*P2) * std::sqrt(Q1*Q1+Q2*Q2) ) );

    A = A*180/CV_PI;

    return A;
}



//主函数
int main()
{
    cv::VideoCapture cap(0);
    cv::VideoCapture cap1(1);

    //写入视频用
    Size size = Size(cap.get(CV_CAP_PROP_FRAME_WIDTH), cap.get(CV_CAP_PROP_FRAME_HEIGHT));
    VideoWriter writer;
    writer.open("/home/jxhu/ClionProjects/hand_gesture_detect/out.avi", CV_FOURCC('M', 'J', 'P', 'G'), 20, size, true);



    const char* windowName = "Front Video Detect";
    const char* windowName1 = "Right Video Detect";
    cv::namedWindow(windowName);
    cv::namedWindow(windowName1);
//    cv::setMouseCallback(windowName, CallbackFunc, NULL);
//    cv::setMouseCallback(windowName1, CallbackFunc1, NULL);

    int minH = 0, maxH = 20, minS = 100, maxS = 190, minV = 60, maxV = 230;
    int minH1 = 0, maxH1 = 20, minS1 = 60, maxS1 = 190, minV1 = 60, maxV1 = 240;
    cv::Mat frame;
    int counts = 0;
    cv::Mat frame1;
    int counts1 = 0;

    int inAngleMin = 200, inAngleMax = 300, angleMin = 180, angleMax = 359, lengthMin = 10, lengthMax = 80;
    cv::createTrackbar("Min H", windowName, &minH, 180);
    cv::createTrackbar("Max H", windowName, &maxH, 180);
    cv::createTrackbar("Min S", windowName, &minS, 255);
    cv::createTrackbar("Max S", windowName, &maxS, 255);
    cv::createTrackbar("Min V", windowName, &minV, 255);
    cv::createTrackbar("Max V", windowName, &maxV, 255);

    cv::createTrackbar("Min H", windowName1, &minH1, 180);
    cv::createTrackbar("Max H", windowName1, &maxH1, 180);
    cv::createTrackbar("Min S", windowName1, &minS1, 255);
    cv::createTrackbar("Max S", windowName1, &maxS1, 255);
    cv::createTrackbar("Min V", windowName1, &minV1, 255);
    cv::createTrackbar("Max V", windowName1, &maxV1, 255);


    bool is_draft = false;

    bool capture = false;//等于true的时候开始检测，也即按c键

    while (1)
    {

        cap >> frame;
        cap1 >> frame1;
        cv::Mat hsv;
        cv::Mat hsv1;

        cv::Mat frame_ ;
        cv::Mat frame1_ ;
        frame.copyTo(frame_);
        frame1.copyTo(frame1_);

        single_finger=true;
        is_draft = false;

        int interrupt = cv::waitKey(10);
        //按q键，退出程序
        if (interrupt == 'q') {
            break;
        }//按c键，开始检测
        else if (interrupt == 'c') {
            capture = true;
        }

        if (capture) {

            cv::cvtColor(frame, hsv, CV_BGR2HSV);
            cv::cvtColor(frame1, hsv1, CV_BGR2HSV);
            cv::inRange(hsv, cv::Scalar(minH, minS, minV), cv::Scalar(maxH, maxS, maxV), hsv);
            cv::inRange(hsv1, cv::Scalar(minH1, minS1, minV1), cv::Scalar(maxH1, maxS1, maxV1), hsv1);


            // 预处理实时视频帧
//            int blurSize = 5;
//            int elementSize = 5;
//            cv::medianBlur(hsv, hsv, blurSize);
//            cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2 * elementSize + 1, 2 * elementSize + 1), cv::Point(elementSize, elementSize));
//            cv::dilate(hsv, hsv, element);

            Mat blur;
            cv::GaussianBlur(hsv, blur, Size(0, 0), 3);
            cv::addWeighted(hsv, 1.5, blur, -0.5, 0, hsv);
            cv::morphologyEx(hsv, hsv, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_ELLIPSE, Size(3, 3)));
            cv::morphologyEx(hsv, hsv, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_ELLIPSE, Size(3, 3)));



            Mat blur1;
            cv::GaussianBlur(hsv1, blur1, Size(0, 0), 3);
            cv::addWeighted(hsv1, 1.5, blur1, -0.5, 0, hsv1);
            cv::morphologyEx(hsv1, hsv1, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_ELLIPSE, Size(3, 3)));
            cv::morphologyEx(hsv1, hsv1, cv::MORPH_CLOSE, cv::getStructuringElement(cv::MORPH_ELLIPSE, Size(3, 3)));

//*********************************要删掉的，只是显示用
            // cv::imshow("hsv", hsv);
            // cv::imshow("hsv1", hsv1);


            // 查找hsv中的轮廓
            std::vector<std::vector<cv::Point> > contours;
            std::vector<cv::Vec4i> hierarchy;
            cv::findContours(hsv, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

            std::vector<std::vector<cv::Point> > contours1;
            std::vector<cv::Vec4i> hierarchy1;
            cv::findContours(hsv1, contours1, hierarchy1, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));


            int largestContour = max_contour(contours);

            for (int i=0;i<BUTTON_NUMBER;i++)
            {
                cv::rectangle(frame1, rightview_click_area[i], cv::Scalar(255, 0, 0));
            }




            if (contours.size()>0 && cv::contourArea(contours[largestContour]) > 10) {
                cv::drawContours(frame, contours, largestContour, cv::Scalar(0, 69, 255), 2);

                // Convex hull
                if (!contours.empty()) {
                    std::vector<std::vector<cv::Point> > hull(1);
                    cv::convexHull(cv::Mat(contours[largestContour]), hull[0], false);
                    cv::drawContours(frame, hull, 0, cv::Scalar(0, 255, 0), 3);
                    if (hull[0].size() > 2) {
                        std::vector<int> hullIndexes;
                        cv::convexHull(cv::Mat(contours[largestContour]), hullIndexes, true);
                        std::vector<cv::Vec4i> convexityDefects;
                        cv::convexityDefects(cv::Mat(contours[largestContour]), hullIndexes, convexityDefects);
                        cv::Rect boundingBox = cv::boundingRect(hull[0]);
                        cv::rectangle(frame, boundingBox, cv::Scalar(255, 0, 0));
                        cv::Point center = cv::Point(boundingBox.x + boundingBox.width / 2,
                                                     boundingBox.y + boundingBox.height / 2);
                        std::vector<cv::Point> validPoints;
                        for (size_t i = 0; i < convexityDefects.size(); i++) {
                            cv::Point p1 = contours[largestContour][convexityDefects[i][0]];
                            cv::Point p2 = contours[largestContour][convexityDefects[i][1]];
                            cv::Point p3 = contours[largestContour][convexityDefects[i][2]];
                            double angle = std::atan2(center.y - p1.y, center.x - p1.x) * 180 / CV_PI;
                            double inAngle = innerAngle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
                            double lengthss = std::sqrt(std::pow(p1.x - p3.x, 2) + std::pow(p1.y - p3.y, 2));
                            if (inAngle > inAngleMin - 180 && inAngle < inAngleMax - 180 &&
                                lengthss < lengthMax / 100.0 * boundingBox.height && lengthss > 15 &&
                                p1.x != boundingBox.x + boundingBox.width
                                && p1.y - boundingBox.y > 0.2 * boundingBox.height) {
                                validPoints.push_back(p1);
                            }
                        }

                        for (size_t i = 0; i < validPoints.size(); i++)
                            cv::circle(frame, validPoints[i], 5, cv::Scalar(0, 0, 255), -1);

                        if (validPoints.size()>1)
                            single_finger=false;


                        if (validPoints.size() >= 2 && centerview_scale_area.x <= validPoints[0].x &&
                            validPoints[0].x <= centerview_scale_area.x + centerview_scale_area.width &&
                            centerview_scale_area.x <= validPoints[1].x &&
                            validPoints[1].x <= centerview_scale_area.x + centerview_scale_area.width &&
                            centerview_scale_area.y <= validPoints[0].y &&
                            validPoints[0].y <= centerview_scale_area.y + centerview_scale_area.height &&
                            centerview_scale_area.y <= validPoints[1].y &&
                            validPoints[1].y <= centerview_scale_area.y + centerview_scale_area.height &&
                            validPoints[0].x==boundingBox.x)
                        {
                            //判断双指缩放
                            JudgeScale(validPoints[0], validPoints[1]);
                        }

                    }
                }

                int largestContour1 = max_contour(contours1);


                //1号摄像头识别
                if (!contours1.empty() && cv::contourArea(contours1[largestContour1]) > 10) {

                    cv::drawContours(frame1, contours1, largestContour1, cv::Scalar(0, 69, 255), 2);

                    std::vector<std::vector<cv::Point> > hull1(1);
                    cv::convexHull(cv::Mat(contours1[largestContour1]), hull1[0], false);
                    cv::drawContours(frame1, hull1, 0, cv::Scalar(0, 255, 0), 3);
                    if (hull1[0].size() > 2) {
                        std::vector<int> hullIndexes1;
                        cv::convexHull(cv::Mat(contours1[largestContour1]), hullIndexes1, true);
                        std::vector<cv::Vec4i> convexityDefects1;
                        cv::convexityDefects(cv::Mat(contours1[largestContour1]), hullIndexes1, convexityDefects1);
                        cv::Rect boundingBox1 = cv::boundingRect(hull1[0]);
                        cv::rectangle(frame1, boundingBox1, cv::Scalar(255, 0, 0));
                        cv::Point center1 = cv::Point(boundingBox1.x + boundingBox1.width / 2,
                                                      boundingBox1.y + boundingBox1.height / 2);

                        //保存单双击用的指头的1号屏幕坐标
                        cv::Point rightview_finger = cv::Point(0, 0);

                        for (size_t i = 0; i < convexityDefects1.size(); i++) {
                            cv::Point p1 = contours1[largestContour1][convexityDefects1[i][0]];
                            cv::Point p2 = contours1[largestContour1][convexityDefects1[i][1]];
                            cv::Point p3 = contours1[largestContour1][convexityDefects1[i][2]];
                            double angle = std::atan2(center1.y - p1.y, center1.x - p1.x) * 180 / CV_PI;
                            double inAngle = innerAngle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
                            double lengths = std::sqrt(std::pow(p1.x - p3.x, 2) + std::pow(p1.y - p3.y, 2));

                            if (p1.y > rightview_finger.y) {
                                rightview_finger.x = (int) p1.x;
                                rightview_finger.y = (int) p1.y;
                            }

                        }


                        //如果单双击用的指头的1号屏幕坐标 在 1号屏幕坐标内 显示指尖并检测单双击手势
                        if (screen1_area.x < rightview_finger.x &&
                            rightview_finger.x < screen1_area.x + screen1_area.width &&
                            screen1_area.y < rightview_finger.y &&
                            rightview_finger.y < screen1_area.y + screen1_area.height)
                        {

                            cv::circle(frame1, rightview_finger, 5, cv::Scalar(0, 0, 255), -1);

                            rightview_finger_series.push_back( cv::Point(rightview_finger.x, rightview_finger.y) );
//
//                            if(is_tar_box_null(tar_box))
//                            {
//
////                                cout<<"tar_box="<<tar_box<<endl;
//                                tar_box = get_but_pos(rightview_finger_series, rightview_click_area);
////                                cout<<"tar_box="<<tar_box<<endl;
//
//                                if (!is_tar_box_null(tar_box))
//                                {
//                                    int length = rightview_finger_series.size();
//
////                                    cout<<"length="<<length<<endl;
//                                    if (length > 6) {
//
//                                        //////////////////////////////////////
////                                        auto it1 = rightview_finger_series.begin();
////                                        for (int i = 0; i < rightview_finger_series.size(); i++) {
////                                            cout<<*it1<<endl;
////                                            it1++;
////                                        }
//                                        ////////////////////////////////////
////                                        cout<<"////////////////////////////////////"<<endl;
//                                        vector<cv::Point>::iterator it = rightview_finger_series.begin();
//
//                                        while (it!=rightview_finger_series.end()-5)
//                                        {
////                                            cout<<"===================================="<<endl;
//                                            it=rightview_finger_series.erase(it);
////                                            it++;
//                                        }
//
////                                        auto it2 = rightview_finger_series.begin();
////                                        for (int i = 0; i < rightview_finger_series.size(); i++) {
////                                            cout<<*it2<<endl;
////                                            it2++;
////                                        }
//                                    }
//                                }
//                            }
//                            if (!is_tar_box_null(tar_box))
//                            {
////                                cout<<"diff_click:tar_box="<<tar_box<<endl;
//
////                                for (int i = 0; i < rightview_finger_series.size(); ++i) {
////                                    cout<<rightview_finger_series[i]<<",";
////                                }
////                                cout<<endl;
//                                int click_result=diff_click(rightview_finger_series);
//
//                                if(click_result==0)
//                                {
//                                    std::cout<<"single click"<<endl;
//                                    show_resultc = 1;
//
//
//                                    tar_box =cv::Rect(-1,-1,-1,-1);
//                                    rightview_finger_series.clear();
//                                }
//                                if (click_result == 1)
//                                {
//                                    std::cout<<"double click"<<endl;
//                                    show_resultc=-1;
//
//
//                                    rightview_finger_series.clear();
//                                    tar_box =cv::Rect(-1,-1,-1,-1);
//                                }
//
//                                if (click_result==-1 && rightview_finger_series.size()>30)
//                                {
//                                    rightview_finger_series.clear();
//                                    tar_box =cv::Rect(-1,-1,-1,-1);
//                                }
//
//                            }
//
//
//


                            if (single_finger)
                            {
                                //这里要添加拖拽识别
                                //如果这里右摄像头中的指尖坐标在点击框内，且大于等于10帧的y坐标变化都在10以内，那认为是拖拽动作
//                                if(rightview_finger_series.size()>5)
//                                {
//                                    int frame_now = length-1;
//                                    int i=0;
//                                    for(;i<BUTTON_NUMBER;i++)
//                                    {
//
//                                        int count_draft_frame = 0;
//                                        int j=frame_now;
//                                        for(;j>0;j--)
//                                        {
//                                            if(rightview_click_area[i].x<rightview_finger_series[j].x &&
//                                               (rightview_finger_series[j].x<rightview_click_area[i].x + rightview_click_area[i].width) &&
//                                               rightview_click_area[i].y<rightview_finger_series[j].y &&
//                                               (rightview_finger_series[j].y<rightview_click_area[i].y + rightview_click_area[i].height)
//                                               && abs( rightview_finger_series[j].y - rightview_finger_series[j-1].y)<=10
//                                                    )
//                                            {
//                                                count_draft_frame+=1;
//                                            }
//                                            else{
//                                                break;
//                                            }
//
//                                        }
//
//                                        if(count_draft_frame>=8)
//                                        {
//                                            is_draft=true;
//                                            show_resultd=1;
//                                            break;
//                                        }
//
//                                    }
//                                }
//
//
//
//
                                //这部分是单双击识别
                                if(is_tar_box_null(tar_box))
                                {

                                    tar_box = get_but_pos(rightview_finger_series, rightview_click_area);

                                    if (!is_tar_box_null(tar_box))
                                    {

                                        if (rightview_finger_series.size() > 6)
                                        {
                                            auto it = rightview_finger_series.begin();

                                            while (it!=rightview_finger_series.end()-5)
                                            {
                                                it=rightview_finger_series.erase(it);
                                            }

                                        }
                                    }
                                }

                                if ((rightview_finger_series.size()>=15)&&(!is_tar_box_null(tar_box)))
                                {
                                    int click_result=diff_click(rightview_finger_series);

                                    if(click_result==0)
                                    {
                                        std::cout<<"single click"<<endl;
                                        show_resultc = 1;


                                        tar_box =cv::Rect(-1,-1,-1,-1);
                                        rightview_finger_series.clear();
                                    }
                                    if (click_result == 1)
                                    {
                                        std::cout<<"double click"<<endl;
                                        show_resultc=-1;
                                        rightview_finger_series.clear();
                                        tar_box =cv::Rect(-1,-1,-1,-1);
                                    }

                                    if (click_result==-1 && rightview_finger_series.size()>30)
                                    {
                                        rightview_finger_series.clear();
                                        tar_box =cv::Rect(-1,-1,-1,-1);
                                    }

                                }

                            }
                            else
                            {
                                rightview_finger_series.clear();
                                tar_box =cv::Rect(-1,-1,-1,-1);
                            }

                        }

                        else
                        {
                            rightview_finger_series.clear();
                            tar_box =cv::Rect(-1,-1,-1,-1);
                        }
                    }
                }

            }

            // 显示是否是拖拽
            if(show_resultd>0)
            {
                cv::putText(frame, "draft detect", cv::Point(40, 70), cv::FONT_HERSHEY_SIMPLEX, 1,
                            cv::Scalar(0, 255, 0), 2);
                cv::putText(frame1, "draft detect", cv::Point(40, 70), cv::FONT_HERSHEY_SIMPLEX, 1,
                            cv::Scalar(0, 255, 0), 2);
                show_resultd += 1;
            }

            if(show_resultd>25)
                show_resultd=0;


            //显示是否是缩放
            if (show_result > 0) {
                cv::putText(frame, "scale up detect", cv::Point(40, 70), cv::FONT_HERSHEY_SIMPLEX, 1,
                            cv::Scalar(0, 255, 0), 2);
                cv::putText(frame1, "scale up detect", cv::Point(40, 70), cv::FONT_HERSHEY_SIMPLEX, 1,
                            cv::Scalar(0, 255, 0), 2);
                show_result += 1;
            }
            if (show_result < 0) {
                cv::putText(frame, "scale down detect", cv::Point(40, 70), cv::FONT_HERSHEY_SIMPLEX, 1,
                            cv::Scalar(0, 255, 0), 2);
                cv::putText(frame1, "scale down detect", cv::Point(40, 70), cv::FONT_HERSHEY_SIMPLEX, 1,
                            cv::Scalar(0, 255, 0), 2);
                show_result -= 1;
            }
            if (show_result > 25 || show_result < -25)
                show_result = 0;


            //显示是否单双击
            if (show_resultc > 0) {
                cv::putText(frame, "single click detect", cv::Point(40, 70), cv::FONT_HERSHEY_SIMPLEX, 1,
                            cv::Scalar(0, 255, 0), 2);
                cv::putText(frame1, "single click detect", cv::Point(40, 70), cv::FONT_HERSHEY_SIMPLEX, 1,
                            cv::Scalar(0, 255, 0), 2);
                show_resultc += 1;
            }
            if (show_resultc < 0) {
                cv::putText(frame, "double clicks detect", cv::Point(40, 70), cv::FONT_HERSHEY_SIMPLEX, 1,
                            cv::Scalar(0, 255, 0), 2);
                cv::putText(frame1, "double clicks detect", cv::Point(40, 70), cv::FONT_HERSHEY_SIMPLEX, 1,
                            cv::Scalar(0, 255, 0), 2);
                show_resultc -= 1;
            }
            if (show_resultc > 25 || show_resultc < -25)
                show_resultc = 0;

            writer.write(frame);

        }

        cv::imshow(windowName, frame);
        cv::imshow(windowName1, frame1);




    }

    cap.release();
    cap1.release();
    cv::destroyAllWindows();
    return 0;
}

