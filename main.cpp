#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <fstream>

using namespace std;
using namespace cv;

string name="test1";
string method="/adaptive";
string filedir="/media/ckck/yoyo/data/file/0322/"+name;
string result="/result/";
string txt_name="/bbox_gt.txt";

Mat frame;
vector<int> ground_truth;

string imgdir=filedir+method+result;
int image=0;
int success=0;
int failure=0;

float iou(Rect& roi, Rect& gt_bbox );
float center_dis(Rect& roi, Rect& gt_bbox );
vector<string> split (const string &s, char delim) ;
void getGroundTruth(fstream& f, vector<Rect>& roi);
void getPredictbbox(fstream& f, vector<Rect>& roi);
//ground truth bbox
string bbox_dir=filedir + txt_name;
string pred_dir=filedir+method+"/bbox_pred.txt";
int previous=0;
Point c1,c2;

int main(int, char**) {
    fstream files(bbox_dir);
    cout<<"bbox_gt_dir: "<<bbox_dir<<endl;
    // Rect gt_bbox;
    fstream pred_files(pred_dir);
    cout<<"bbox_pred_dir: "<<pred_dir<<endl;
    // Rect pred_bbox;
    bool next=true;
    vector<Rect> pred_bbox;
    vector<Rect> gt_bbox;
    if(files.is_open()){
        getGroundTruth(files, gt_bbox);
    }
    if(pred_files.is_open()){
        getPredictbbox(pred_files, pred_bbox);
    }

    while(true){
        if(next){
            // get the image.
            image++;
            char ch[20];
            sprintf(ch,"%d", image);
            string dir=imgdir+ch+".jpg";
            cout<<"Image_dir: "<<dir<<endl;
            frame=imread(dir, 1);            
            next=false; 
        }
        //make a decision in auto compareing.
        float dis =center_dis(pred_bbox[image-1], gt_bbox[image-1]);
        if(dis<20){
            ground_truth.push_back(1);
            printf("Frame %d\n", image);
            cout<<"next!"<<endl<<endl; 
            next=true;
        }
        

        if(!frame.empty()){
            if(files.is_open()){
                float value= iou(pred_bbox[image-1], gt_bbox[image-1]);                
                string iou= "IOU: "+to_string(value);
                string ct_dis= "dist: "+to_string(dis);
                if(value<=0.5){
                    putText(frame, iou, Point(400,40),cv::FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(0,0,255), 1); 
                }else{
                    putText(frame, iou, Point(400,40),cv::FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(255,0,0), 1); 
                }
                if(dis>=20){
                    putText(frame, ct_dis, Point(400,80),cv::FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(0,0,255), 1); 
                }else{
                    putText(frame, ct_dis, Point(400,80),cv::FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(255,0,0), 1); 
                }
                circle(frame, c1, 2, Scalar(255, 0, 0), 2);
                circle(frame, c2, 2, Scalar(0,0,0),2);
                rectangle(frame,gt_bbox[image-1],Scalar(0,0,0),2,8,0);
            }
                
            imshow("label me!", frame);  
                   
        }else{
            cout<<"The END."<<endl;
            cout<<"Save the result!"<<endl;
            fstream file;
            string gt_name=filedir+method+"/gt.txt";
            file.open(gt_name, ios::out|ios::trunc);
            if(file.is_open()){
                for(int i=0; i<ground_truth.size();i++){
                     file<<ground_truth[i]<<"\n";
                     if(ground_truth[i]==1){
                         success++;
                     }else{
                         failure++;
                     }
                }               
            }
            file.close();
            cout<<"done!"<<endl;
            printf("Success: %d \n", success);
            printf("Failure: %d \n", failure);
            int total=ground_truth.size();
            printf("Track result: %d/%d", success, total);
            break;
        }

        char k=waitKey(30);
         if(k=='a'){
             if(image==1){
                cout<<"This is the first frame! don't test me!"<<endl;
            }else{
                previous++;
                cout<<"reset the frame:  "<<image-1<<endl;
                image=image-2;
                next=true;
            }    
        }

        if(k=='h'){
            cout<<"Press the key 'j' to set the result be TRUE."<<endl;
            cout<<"Press the key 'k' to set the result be FALSE."<<endl;
            cout<<"Press the key 'a' to reset the result from the previous image."<<endl;
        }

        if(k=='j'){
            cout<<"The number "<<image<<" is right(TRUE)."<<endl;
            if(previous==0){
                ground_truth.push_back(1);
            }else{
                cout<<"previous: "<<previous<<endl;
                ground_truth.at(image-1) = 1;
                previous--;
            }            
            printf("Frame %d\n", image);
             cout<<"next!"<<endl<<endl; 
            next=true;
        }
        if(k=='k'){
            cout<<"The number "<<image<<"is wrong (FALSE)."<<endl;
             if(previous==0){
                ground_truth.push_back(0);
            }else{
                cout<<"previous: "<<previous<<endl;
                ground_truth.at(image-1) = 0;
                previous--;
            }            
            printf("Frame %d\n", image);
             cout<<"next!"<<endl<<endl; 
            next=true;           
        }
       
        if(k=='q'){
            cout<<"bye!"<<endl;
            break;
        }
        if(k=='p'){
            waitKey(0);   
        }
    }
}

float iou(Rect& roi, Rect& gt_bbox ){
    //predict result: roi & gt_bbox
        //  iou test
        int xmin=roi.x;
        int ymin=roi.y;
        int xmax=roi.x+roi.width;
        int ymax=roi.y+roi.height;
        printf("c1: (%d, %d) (%d, %d)\n",xmin, ymin, xmax, ymax);
        printf("c2: (%d, %d) (%d, %d)\n",gt_bbox.x, gt_bbox.y, (gt_bbox.x+gt_bbox.width),(gt_bbox.y+gt_bbox.height));
        // intersection 
        int xx1=(xmin>=gt_bbox.x)?xmin:gt_bbox.x;
        int yy1=(ymin>=gt_bbox.y)?ymin:gt_bbox.y;
        int xx2=(xmax<=(gt_bbox.x+gt_bbox.width))? xmax:(gt_bbox.x+gt_bbox.width);
        int yy2=(ymax<=(gt_bbox.y+gt_bbox.height))?ymax:(gt_bbox.y+gt_bbox.height);
        printf("intersection: (%d, %d) (%d, %d) \n",xx1, yy1, xx2, yy2);
        int xx=(xx2-xx1)>0?(xx2-xx1):0;
        int yy=(yy2-yy1)>0?(yy2-yy1):0;
        printf("intersection w=%d, h=%d \n",xx, yy);
        int interArea=(xx)*(yy);
        printf("intersection Area= %d\n",interArea);
        int unionArea= gt_bbox.area()*2-interArea+1e-6;
        printf("unionArea= %d\n",unionArea);
        float iou= (float)interArea/unionArea;
        
        return iou;
}
//center distance
float center_dis(Rect& roi, Rect& gt_bbox ){
    c1.x= roi.x+(roi.width>>1);
    c1.y= roi.y+(roi.height>>1);
    c2.x=gt_bbox.x+(gt_bbox.width>>1);
    c2.y=gt_bbox.y+(gt_bbox.height>>1);
    int distance = pow(c1.x-c2.x, 2)+pow(c1.y-c2.y, 2);
  
    return sqrt(distance);
}

 vector<string> split (const string &s, char delim) {
    vector<string> result;
    stringstream ss (s);
    string item;

    while (getline (ss, item, delim)) {
        result.push_back (item);
    }
    return result;
}
// save with Rect
void getGroundTruth(fstream& f, vector<Rect>& roi){
    string str;
    Rect temp_roi;
    // int count=0;
    while(getline(f, str)){
            vector<string> v = split (str, ',');
            vector<int> num;
            for (auto i : v){
                num.push_back(stoi(i));
            }
            temp_roi.x=num[0];
            temp_roi.y=num[1];
            temp_roi.width=num[2];
            temp_roi.height=num[3];       
            roi.push_back(temp_roi);
            // count++;
            // cout<<to_string(count) <<endl;
        }
        cout<<"The end!"<<endl;
 }

// save with (p1,p2)
void getPredictbbox(fstream& f, vector<Rect>& roi){
    string str;
    Rect temp_roi;
    while(getline(f, str)){
        vector<string> v = split (str, ',');
        vector<int> num;
        for (auto i : v){
            num.push_back(stoi(i));
        }
        temp_roi.x=num[0];
        temp_roi.y=num[1];
        temp_roi.width=abs(num[2]-num[0]);
        temp_roi.height=abs(num[3]-num[1]);       
        roi.push_back(temp_roi);
        }
        cout<<"The end!"<<endl;
        
}

